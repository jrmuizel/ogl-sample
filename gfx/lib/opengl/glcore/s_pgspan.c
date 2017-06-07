/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/s_pgspan.c#12 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#if defined(DEBUG)
#include <glmath.h>
#endif /* DEBUG */

/*
** Process the incoming span by calling all of the appropriate span procs.
*/
GLboolean __glProcessSpan(__GLcontext *gc)
{
    GLint m, i;

    m = gc->procs.span.m;

    gc->polygon.shader.done = GL_FALSE;
    for (i = 0; i < m; i++) {
	if ((*gc->procs.span.spanFuncs[i])(gc)) {
	    i++;
	    break;
	}
    }

    if (i != m && !gc->polygon.shader.done) {
	for (; i<m; i++) {
	    if ((*gc->procs.span.stippledSpanFuncs[i])(gc)) {
		break;
	    }
	}
    }

    return GL_FALSE;
}

/*
** Process the incoming span by calling all of the appropriate span procs.
**
** This routine sets gc->polygon.shader.cfb to &gc->frontBuffer and then
** to &gc->backBuffer.
*/
GLboolean __glProcessReplicateSpan(__GLcontext *gc)
{
    GLint n, m, i;
    __GLcolor colors[__GL_MAX_MAX_VIEWPORT], *fcp, *tcp;
    GLint w;

    w = gc->polygon.shader.length;
    n = gc->procs.span.n;
    m = gc->procs.span.m; 

    gc->polygon.shader.done = GL_FALSE;
    for (i = 0; i < n; i++) {
	if ((*gc->procs.span.spanFuncs[i])(gc)) {
	    i++;
	    goto earlyStipple;
	}
    }

    fcp = gc->polygon.shader.colors;
    tcp = colors;
    if (gc->modes.rgbMode) {
	for (i = 0; i < w; i++) {
	    *tcp++ = *fcp++;
	}
    } else {
	for (i = 0; i < w; i++) {
	    tcp->r = fcp->r;
	    fcp++;
	    tcp++;
	}
    }
    gc->polygon.shader.cfb = &gc->frontBuffer;
    for (i = n; i < m; i++) {
	(*gc->procs.span.spanFuncs[i])(gc);
    }
    fcp = colors;
    tcp = gc->polygon.shader.colors;
    if (gc->modes.rgbMode) {
	for (i = 0; i < w; i++) {
	    *tcp++ = *fcp++;
	}
    } else {
	for (i = 0; i < w; i++) {
	    tcp->r = fcp->r;
	    fcp++;
	    tcp++;
	}
    }
    gc->polygon.shader.cfb = &gc->backBuffer;
    for (i = n; i < m; i++) {
	(*gc->procs.span.spanFuncs[i])(gc);
    }
    return GL_FALSE;

earlyStipple:
    if (gc->polygon.shader.done) return GL_FALSE;

    for (; i < n; i++) {
	if ((*gc->procs.span.stippledSpanFuncs[i])(gc)) {
	    return GL_FALSE;
	}
    }

    fcp = gc->polygon.shader.colors;
    tcp = colors;
    if (gc->modes.rgbMode) {
	for (i = 0; i < w; i++) {
	    *tcp++ = *fcp++;
	}
    } else {
	for (i = 0; i < w; i++) {
	    tcp->r = fcp->r;
	    fcp++;
	    tcp++;
	}
    }
    gc->polygon.shader.cfb = &gc->frontBuffer;
    for (i = n; i < m; i++) {
	(*gc->procs.span.stippledSpanFuncs[i])(gc);
    }
    fcp = colors;
    tcp = gc->polygon.shader.colors;
    if (gc->modes.rgbMode) {
	for (i = 0; i < w; i++) {
	    *tcp++ = *fcp++;
	}
    } else {
	for (i = 0; i < w; i++) {
	    tcp->r = fcp->r;
	    fcp++;
	    tcp++;
	}
    }
    gc->polygon.shader.cfb = &gc->backBuffer;
    for (i = n; i < m; i++) {
	(*gc->procs.span.stippledSpanFuncs[i])(gc);
    }
    return GL_FALSE;
}

/*
** Perform scissoring on the incoming span, advancing parameter
** values only if necessary.
**
** Returns GL_TRUE if span was entirely (or sometimes when partially) clipped, 
** GL_FALSE otherwise.
*/
GLboolean __glClipSpan(__GLcontext *gc)
{
    GLint clipX0, clipX1, delta;
    GLint x, xr;
    GLint w, w2;
    GLboolean stippled;

    w = gc->polygon.shader.length;

    x = gc->polygon.shader.frag.x;
    stippled = GL_FALSE;
    clipX0 = gc->transform.clipX0;
    clipX1 = gc->transform.clipX1;
    xr = x + w;
    if ((x < clipX0) || (xr > clipX1)) {
	/*
	** Span needs to be scissored in some fashion
	*/
	if ((xr <= clipX0) || (x >= clipX1)) {
	    /* Scissor out the entire span */
	    gc->polygon.shader.done = GL_TRUE;
	    return GL_TRUE;
	}
	if (xr > clipX1) {
	    /*
	    ** Span is clipped by the right edge of the scissor.  This is 
	    ** easy, we will simply reduce the width of this span!
	    */
	    w = clipX1 - x;
	}
	if (x < clipX0) {
	    __GLstippleWord bit, outMask, *osp;
	    GLint count;

	    /*
	    ** Span is clipped by the left edge of the scissor.  This is hard.
	    ** We have two choices.
	    **
	    ** 1) We can stipple the first half of the span.
	    ** 2) We can bump all of the iterator values.
	    **
	    ** The problem with approach number 2 is that the routine 
	    ** which originally asks to have a span processed has assumed 
	    ** that the iterator values will not be munged.  So, if we 
	    ** wanted to implement 2 (which would make this case faster),
	    ** we would need to change that assumption, and make the higher
	    ** routine shadow all of the iterator values, which would slow
	    ** down all paths.  This is probably not a good trade to speed
	    ** this path up, since this path will only occur when the scissor
	    ** region (or window) is smaller than the viewport, and this span
	    ** happens to hit the left edge of the scissor region (or window).
	    **
	    ** Therefore, we choose number 1.
	    */
	    delta = clipX0 - x;

	    osp = gc->polygon.shader.stipplePat;
	    w2 = w;
	    while (w2) {
		count = w2;
		if (count > __GL_STIPPLE_BITS) {
		    count = __GL_STIPPLE_BITS;
		}
		w2 -= count;

		outMask = ~0;
		bit = __GL_STIPPLE_SHIFT(0);
		while (--count >= 0) {
		    if (delta > 0) {
			delta--;
			outMask &= ~bit;
		    }
#ifdef __GL_STIPPLE_MSB
		    bit >>= 1;
#else
		    bit <<= 1;
#endif
		}

		*osp++ = outMask;
	    }

	    stippled = GL_TRUE;
	}
    }
    assert(w <= __GL_MAX_MAX_VIEWPORT);

    gc->polygon.shader.length = w;

    return stippled;
}

/*
** Generate the polygon stipple for a span.
*/
GLboolean __glStippleSpan(__GLcontext *gc)
{
    __GLstippleWord stipple;
    __GLstippleWord *sp;
    GLint count;
    GLint shift;
    GLint w;

    w = gc->polygon.shader.length;

    if (gc->constants.yInverted) {
	stipple = gc->polygon.stipple[(gc->constants.height - 
		(gc->polygon.shader.frag.y - gc->constants.viewportYAdjust)-1) 
		& (__GL_STIPPLE_BITS-1)];
    } else {
	stipple = gc->polygon.stipple[gc->polygon.shader.frag.y & 
		(__GL_STIPPLE_BITS-1)];
    }
    shift = gc->polygon.shader.frag.x & (__GL_STIPPLE_BITS - 1);
#ifdef __GL_STIPPLE_MSB
    stipple = (stipple << shift) | (stipple >> (__GL_STIPPLE_BITS - shift));
#else
    stipple = (stipple >> shift) | (stipple << (__GL_STIPPLE_BITS - shift));
#endif
    if (stipple == 0) {
	/* No point in continuing */
	gc->polygon.shader.done = GL_TRUE;
	return GL_TRUE;
    }

    /* Replicate stipple word */
    count = w;
    sp = gc->polygon.shader.stipplePat;
    while (count > 0) {
	*sp++ = stipple;
	count -= __GL_STIPPLE_BITS;
    }

    return GL_TRUE;
}

/*
** Generate the polygon stipple for a stippled span.
*/
GLboolean __glStippleStippledSpan(__GLcontext *gc)
{
    __GLstippleWord stipple;
    __GLstippleWord *sp;
    GLint count;
    GLint shift;
    GLint w;

    w = gc->polygon.shader.length;

    if (gc->constants.yInverted) {
	stipple = gc->polygon.stipple[(gc->constants.height - 
		(gc->polygon.shader.frag.y - gc->constants.viewportYAdjust)-1) 
		& (__GL_STIPPLE_BITS-1)];
    } else {
	stipple = gc->polygon.stipple[gc->polygon.shader.frag.y & 
		(__GL_STIPPLE_BITS-1)];
    }
    shift = gc->polygon.shader.frag.x & (__GL_STIPPLE_BITS - 1);
#ifdef __GL_STIPPLE_MSB
    stipple = (stipple << shift) | (stipple >> (__GL_STIPPLE_BITS - shift));
#else
    stipple = (stipple >> shift) | (stipple << (__GL_STIPPLE_BITS - shift));
#endif
    if (stipple == 0) {
	/* No point in continuing */
	gc->polygon.shader.done = GL_TRUE;
	return GL_TRUE;
    }

    /* Replicate stipple word */
    count = w;
    sp = gc->polygon.shader.stipplePat;
    while (count > 0) {
	*sp++ &= stipple;
	count -= __GL_STIPPLE_BITS;
    }

    return GL_FALSE;
}

/************************************************************************/

/*
** Alpha test span uses a lookup table to do the alpha test function.
** Output a stipple with 1's where the test passed, and 0's where the
** test failed.
*/
GLboolean __glAlphaTestSpan(__GLcontext *gc)
{
    GLubyte *atft;
    GLint failed, count, ia;
    __GLstippleWord bit, outMask, *osp;
    __GLcolor *cp;
    GLint maxAlpha;
    GLint w;

    w = gc->polygon.shader.length;

    atft = &gc->frontBuffer.alphaTestFuncTable[0];
    cp = gc->polygon.shader.colors;
    maxAlpha = gc->constants.alphaTestSize - 1;
    osp = gc->polygon.shader.stipplePat;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    ia = (GLint)(gc->constants.alphaTableConv * cp->a);
	    if (ia < 0) ia = 0;
	    if (ia > maxAlpha) ia = maxAlpha;
	    if (!atft[ia]) {
		/* Test failed */
		outMask &= ~bit;
		failed++;
	    }
	    cp++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*osp++ = outMask;
    }

    if (failed == 0) {
	/* Call next span proc */
	return GL_FALSE;
    } else {
	if (failed != gc->polygon.shader.length) {
	    /* Call next stippled span proc */
	    return GL_TRUE;
	}
    }
    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}

/*
** Stippled form of alpha test span that checks the stipple at each
** pixel and avoids the test where the stipple disallows it.
*/
GLboolean __glAlphaTestStippledSpan(__GLcontext *gc)
{
    GLubyte *atft;
    GLint count, ia, failed;
    __GLstippleWord bit, inMask, outMask, *isp;
    __GLcolor *cp;
    GLint maxAlpha;
    GLint w;

    w = gc->polygon.shader.length;
    isp = gc->polygon.shader.stipplePat;

    atft = &gc->frontBuffer.alphaTestFuncTable[0];
    cp = gc->polygon.shader.colors;
    maxAlpha = gc->constants.alphaTestSize - 1;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *isp;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		ia = (GLint)(gc->constants.alphaTableConv * cp->a);
		if (ia < 0) ia = 0;
		if (ia > maxAlpha) ia = maxAlpha;
		if (!atft[ia]) {
		    /* Test failed */
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;
	    cp++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*isp++ = outMask & inMask;
    }

    if (failed != gc->polygon.shader.length) {
	/* Call next stippled span proc */
	return GL_FALSE;
    }
    return GL_TRUE;
}

/************************************************************************/

#if !(defined(mips) && defined(__GL_USE_ASMCODE))
/*
** Perform stencil testing.  Apply test fail operation as we go.
** Generate a stipple with 1's where the test passed and 0's where the
** test failed.
*/
GLboolean __glStencilTestSpan(__GLcontext *gc)
{
    __GLstencilCell *tft, *sfb, *fail, cell;
    GLint count, failed;
    __GLstippleWord bit, outMask, *osp;
    GLint w;

    w = gc->polygon.shader.length;

    sfb = gc->polygon.shader.sbuf;
    tft = gc->stencilBuffer.testFuncTable;
    fail = gc->stencilBuffer.failOpTable;
    osp = gc->polygon.shader.stipplePat;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    cell = sfb[0];
	    /* test func table already anded cell values with mask */
	    if (!tft[cell]) {
		/* Test failed */
		outMask &= ~bit;
		sfb[0] = fail[cell];
		failed++;
	    }
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*osp++ = outMask;
    }

    if (failed == 0) {
	return GL_FALSE;
    } else {
	if (failed != gc->polygon.shader.length) {
	    /* Call next proc */
	    return GL_TRUE;
	}
    }
    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}
#endif /* !(mips && __GL_USE_ASMCODE) */

/*
** Stippled form of stencil test.
*/
GLboolean __glStencilTestStippledSpan(__GLcontext *gc)
{
    __GLstencilCell *tft, *sfb, *fail, cell;
    GLint failed, count;
    __GLstippleWord bit, inMask, outMask, *sp;
    GLuint smask;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    sfb = gc->polygon.shader.sbuf;
    tft = gc->stencilBuffer.testFuncTable;
    fail = gc->stencilBuffer.failOpTable;
    smask = gc->state.stencil.mask;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		cell = sfb[0];
		if (!tft[cell & smask]) {
		    /* Test failed */
		    outMask &= ~bit;
		    sfb[0] = fail[cell];
		    failed++;
		}
	    } else failed++;
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*sp++ = outMask & inMask;
    }

    if (failed != gc->polygon.shader.length) {
	/* Call next proc */
	return GL_FALSE;
    }
    return GL_TRUE;
}

/*
** Depth test a polygon offset span, when stenciling is disabled.
*/
GLboolean __glDepthTestOffsetSpan(__GLcontext *gc)
{
    __GLzValue *zfb, z;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc[2])( __GLzValue, __GLzValue * );
    __GLstippleWord bit, outMask, *osp;

    GLuint i; /* index */
    GLint w[3]; /* segment count (last entry is always 0) */
    __GLzValue zFrac[2];
    __GLzValue dzdx[2];
    __GLzValue dzdxBig[2];

    __GLfloat offsetZ; /* actual z value */
    __GLfloat outCount;
    __GLfloat range;

    GLint boundary;
    __GLzValue boundarydzdxBig;

#if defined(POLYGON_OFFSET_DEBUG)
    GLdouble offsetZtest;
    GLdouble ztest;
    
    offsetZtest = gc->polygon.shader.offsetZ;
#endif /* POLYGON_OFFSET_DEBUG */

    w[0] = gc->polygon.shader.length;
    w[1] = 0;
    w[2] = 0; /* termination condition */

    zfb = gc->polygon.shader.zbuf;
    testFunc[0] = gc->procs.DTPixel;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;

    osp = gc->polygon.shader.stipplePat;

    failed = 0;
    boundary = 0;

    offsetZ = gc->polygon.shader.offsetZ; 
    outCount = gc->polygon.shader.outBoundCount;
    range = gc->polygon.shader.rangeCount;


    if(outCount > 0) { /* heading towards it */
      if(outCount < range) { /* starts inside */
	/* offsetZ should be within z buffer range */
	assert(offsetZ >= 0. && offsetZ <= gc->depthBuffer.scale);
	zFrac[0] = offsetZ;
	dzdx[0] = gc->polygon.shader.dzdx >> numFracBits;
	dzdxBig[0] = gc->polygon.shader.dzdxBig;
	if(w[0] > outCount) { /* going out */
	  w[1] = w[0] - outCount;
	  w[0] -= w[1];
	  if((gc->depthBuffer.testFunc == GL_LESS &&
	      gc->polygon.shader.endZLimit != 0) ||
	     (gc->depthBuffer.testFunc == GL_GREATER &&
	     gc->polygon.shader.endZLimit == 0))
	      testFunc[1] = __glCDTPixel[0]; /* pick GL_NEVER */
	    else
	      testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = gc->polygon.shader.endZLimit;
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  dzdx[1] = 0;
	  dzdxBig[1] = 0;
	}
      } else { /* starts outside */
	zFrac[0] = gc->polygon.shader.startZLimit;
	dzdx[0] = 0;
	dzdxBig[0] = 0;
	if((gc->depthBuffer.testFunc == GL_LESS &&
	    gc->polygon.shader.startZLimit != 0) ||
	   (gc->depthBuffer.testFunc == GL_GREATER &&
	    gc->polygon.shader.startZLimit == 0))
	  testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
	if(w[0] > (GLint)(outCount - range + 1.)) { /* goes in */
	  w[1] = w[0] - (GLint)(outCount - range + 1.);
	  w[0] = (GLint)(outCount - range + 1.);
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ;
	  /* starting Z for 2nd segment should be inside z-buffer range */
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) >= 0.);
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) <= gc->depthBuffer.scale);
	  dzdx[1] = gc->polygon.shader.dzdx >> numFracBits;
	  dzdxBig[1] = gc->polygon.shader.dzdxBig;
	}
      }
    } else { /* already past */
      if((gc->depthBuffer.testFunc == GL_LESS &&
	  gc->polygon.shader.endZLimit != 0) ||
	 (gc->depthBuffer.testFunc == GL_GREATER &&
	  gc->polygon.shader.endZLimit == 0))
	testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
      zFrac[0] = gc->polygon.shader.endZLimit;
      dzdx[0] = 0;
      dzdxBig[0] = 0;
    }

    /* this algorithm shouldn't change the number of pixels tested */
    assert(w[0] + w[1] == gc->polygon.shader.length);

    i = 0;
    while(w[i]) {
      while (w[i]) {
      
        count = w[i];

	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w[i] -= count;

#if defined(POLYGON_OFFSET_DEBUG)
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	z = (zFrac[i] >> numFracBits) + cnt;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	    if(ztest < 0.) /* clamp to zero */
	      assert(z == cnt);
	    else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	      assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	    else
	      assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	    if( (*testFunc[i])(z,zfb) == GL_FALSE ) {
		outMask &= ~bit;
		failed++;
	    }
#if defined(POLYGON_OFFSET_DEBUG)
	    ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	    z += dzdx[i];
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*osp++ = outMask;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig;
#endif /* POLYGON_OFFSET_DEBUG */
	zFrac[i] += dzdxBig[i];
      }
      i++;
      if(w[i] && boundary) { /* stitch mask together */
	count = __GL_STIPPLE_BITS - boundary;
	boundarydzdxBig = (GLint)((GLint)dzdxBig[i] * 
	                  ((GLdouble)count / (GLdouble)__GL_STIPPLE_BITS));
	if(count > w[i])
	  count = w[i];
	w[i] -= count;
	osp--;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest -= gc->polygon.shader.dzdxBig;
	offsetZtest += gc->polygon.shader.dzdxBig * (GLdouble)boundary / 
	               (GLdouble)__GL_STIPPLE_BITS;
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	z = (zFrac[i] >> numFracBits) + cnt;
	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	  if(ztest < 0.) /* clamp to zero */
	    assert(z == cnt);
	  else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	    assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	  else
	    assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	  if( (*testFunc[i])(z,zfb) == GL_FALSE ) {
	    outMask &= ~bit;
	    failed++;
	  }
#if defined(POLYGON_OFFSET_DEBUG)
	  ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	  z += dzdx[i];
	  zfb = (__GLzValue *) ((GLubyte *) zfb + 
				gc->depthBuffer.buf.drawableBuf->elementSize);
#ifdef __GL_STIPPLE_MSB
	  bit >>= 1;
#else
	  bit <<= 1;
#endif
	}
	*osp++ = outMask;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig * 
	               (GLdouble)(__GL_STIPPLE_BITS - boundary) /
		       (GLdouble)__GL_STIPPLE_BITS;
#endif /* POLYGON_OFFSET_DEBUG */
	zFrac[i] += boundarydzdxBig;
      }
    }

    if (failed == 0) {
	/* Call next span proc */
	return GL_FALSE;
    } else {
	if (failed != gc->polygon.shader.length) {
	    /* Call next stippled span proc */
	    return GL_TRUE;
	}
    }
    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}

/*
** Stippled form of depth test, polygon offset span, with stenciling enabled.
*/
GLboolean __glDepthTestStippledOffsetSpan(__GLcontext *gc)
{
    __GLzValue z, *zfb;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc[2])( __GLzValue, __GLzValue * );
    __GLstippleWord bit, inMask, outMask, *sp;

    GLuint i; /* index */
    GLint w[3]; /* segment count (last entry is always 0) */
    __GLzValue zFrac[2];
    __GLzValue dzdx[2];
    __GLzValue dzdxBig[2];

    __GLfloat offsetZ; /* actual z value */
    __GLfloat outCount;
    __GLfloat range;

    GLint boundary;
    __GLzValue boundarydzdxBig;

#if defined(POLYGON_OFFSET_DEBUG)
    GLdouble offsetZtest;
    GLdouble ztest;

    offsetZtest = gc->polygon.shader.offsetZ;
#endif /* POLYGON_OFFSET_DEBUG */

    w[0] = gc->polygon.shader.length;
    w[1] = 0;
    w[2] = 0; /* termination condition */

    zfb = gc->polygon.shader.zbuf;
    testFunc[0] = gc->procs.DTPixel;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;

    sp = gc->polygon.shader.stipplePat;

    failed = 0;
    boundary = 0;

    offsetZ = gc->polygon.shader.offsetZ; 
    outCount = gc->polygon.shader.outBoundCount;
    range = gc->polygon.shader.rangeCount;

    if(outCount > 0) { /* heading towards it */
      if(outCount < range) { /* starts inside */
	/* offsetZ should be within z buffer range */
	assert(offsetZ >= 0. && offsetZ <= gc->depthBuffer.scale);
	zFrac[0] = offsetZ;
	dzdx[0] = gc->polygon.shader.dzdx >> numFracBits;
	dzdxBig[0] = gc->polygon.shader.dzdxBig;
	if(w[0] > outCount) { /* going out */
	  w[1] = w[0] - outCount;
	  w[0] -= w[1];
	  if((gc->depthBuffer.testFunc == GL_LESS &&
	      gc->polygon.shader.endZLimit != 0) ||
	     (gc->depthBuffer.testFunc == GL_GREATER &&
	     gc->polygon.shader.endZLimit == 0))
	      testFunc[1] = __glCDTPixel[0]; /* pick GL_NEVER */
	    else
	      testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = gc->polygon.shader.endZLimit;
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  dzdx[1] = 0;
	  dzdxBig[1] = 0;
	}
      } else { /* starts outside */
	zFrac[0] = gc->polygon.shader.startZLimit;
	dzdx[0] = 0;
	dzdxBig[0] = 0;
	if((gc->depthBuffer.testFunc == GL_LESS &&
	    gc->polygon.shader.startZLimit != 0) ||
	   (gc->depthBuffer.testFunc == GL_GREATER &&
	    gc->polygon.shader.startZLimit == 0))
	  testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
	if(w[0] > (GLint)(outCount - range + 1.)) { /* goes in */
	  w[1] = w[0] - (GLint)(outCount - range + 1.);
	  w[0] = (GLint)(outCount - range + 1.);
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ;
	  /* starting Z for 2nd segment should be inside z-buffer range */
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) >= 0.);
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) <= gc->depthBuffer.scale);
	  dzdx[1] = gc->polygon.shader.dzdx >> numFracBits;
	  dzdxBig[1] = gc->polygon.shader.dzdxBig;
	}
      }
    } else { /* already past */
      if((gc->depthBuffer.testFunc == GL_LESS &&
	  gc->polygon.shader.endZLimit != 0) ||
	 (gc->depthBuffer.testFunc == GL_GREATER &&
	  gc->polygon.shader.endZLimit == 0))
	testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
      zFrac[0] = gc->polygon.shader.endZLimit;
      dzdx[0] = 0;
      dzdxBig[0] = 0;
    }
	
    /* this algorithm shouldn't change the number of pixels tested */
    assert(w[0] + w[1] == gc->polygon.shader.length);

    i = 0;
    while(w[i]) {
      while (w[i]) {
	count = w[i];
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w[i] -= count;

#if defined(POLYGON_OFFSET_DEBUG)
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	z = (zFrac[i] >> numFracBits) + cnt;
	inMask = *sp;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	  if(ztest < 0.) /* clamp to zero */
	    assert(z == cnt);
	  else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	    assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	  else
	    assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	    if (inMask & bit) {
		if( (*testFunc[i])(z,zfb) == GL_FALSE ) {
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;
#if defined(POLYGON_OFFSET_DEBUG)
	    ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	    z += dzdx[i];
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*sp++ = outMask & inMask;
	zFrac[i] += dzdxBig[i];
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig;
#endif	 /* POLYGON_OFFSET_DEBUG */
      }
      i++;
      if(w[i] && boundary) { /* stitch mask together */
	count = __GL_STIPPLE_BITS - boundary;
	boundarydzdxBig = (GLint)((GLint)dzdxBig[i] * 
	                  ((GLdouble)count / (GLdouble)__GL_STIPPLE_BITS));
	if(count > w[i])
	  count = w[i];
	w[i] -= count;
	sp--;
	z = (zFrac[i] >> numFracBits) + cnt;

#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest -= gc->polygon.shader.dzdxBig;
	offsetZtest += gc->polygon.shader.dzdxBig * (GLdouble)boundary / 
	               (GLdouble)__GL_STIPPLE_BITS;
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	  if(ztest < 0.) /* clamp to zero */
	    assert(z == cnt);
	  else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	    assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	  else
	    assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	    if (inMask & bit) {
		if( (*testFunc[i])(z,zfb) == GL_FALSE ) {
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;
#if defined(POLYGON_OFFSET_DEBUG)
	    ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	    z += dzdx[i];
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
#ifdef __GL_STIPPLE_MSB
	  bit >>= 1;
#else
	  bit <<= 1;
#endif
	}
	*sp++ = outMask & inMask;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig * 
	               (GLdouble)(__GL_STIPPLE_BITS - boundary) /
		       (GLdouble)__GL_STIPPLE_BITS;
#endif /* POLYGON_OFFSET_DEBUG */
	zFrac[i] += boundarydzdxBig;
      }
    }


    if (failed != gc->polygon.shader.length) {
	/* Call next proc */
	return GL_FALSE;
    }
    return GL_TRUE;
}


/************************************************************************/



#ifndef __GL_USEASMCODE
/*
** Depth test a span, when stenciling is disabled.
*/
GLboolean __glDepthTestSpan(__GLcontext *gc)
{
    __GLzValue zFrac, z, dzdx, dzdxBig, *zfb;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * );
    __GLstippleWord bit, outMask, *osp;
    GLint w;

    w = gc->polygon.shader.length;

    zfb = gc->polygon.shader.zbuf;
    testFunc = gc->procs.DTPixel;
    zFrac = gc->polygon.shader.frag.z;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;
    dzdx = gc->polygon.shader.dzdx >> numFracBits;
    dzdxBig = gc->polygon.shader.dzdxBig;
    osp = gc->polygon.shader.stipplePat;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	z = (zFrac >> numFracBits) + cnt;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if( (*testFunc)(z,zfb) == GL_FALSE ) {
		outMask &= ~bit;
		failed++;
	    }
	    z += dzdx;
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*osp++ = outMask;
	zFrac += dzdxBig;
    }

    if (failed == 0) {
	/* Call next span proc */
	return GL_FALSE;
    } else {
	if (failed != gc->polygon.shader.length) {
	    /* Call next stippled span proc */
	    return GL_TRUE;
	}
    }
    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}

/*
** Stippled form of depth test span, when stenciling is disabled.
*/
GLboolean __glDepthTestStippledSpan(__GLcontext *gc)
{
    __GLzValue zFrac, z, dzdx, dzdxBig, *zfb;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * );
    __GLstippleWord bit, inMask, outMask, *sp;
    GLint w;

    sp = gc->polygon.shader.stipplePat;
    w = gc->polygon.shader.length;

    zfb = gc->polygon.shader.zbuf;
    testFunc = gc->procs.DTPixel;
    zFrac = gc->polygon.shader.frag.z;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;
    dzdx = gc->polygon.shader.dzdx >> numFracBits;
    dzdxBig = gc->polygon.shader.dzdxBig;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	z = (zFrac >> numFracBits) + cnt;
	inMask = *sp;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		if( (*testFunc)(z,zfb) == GL_FALSE ) {
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;
	    z += dzdx;
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*sp++ = outMask & inMask;
	zFrac += dzdxBig;
    }

    if (failed != gc->polygon.shader.length) {
	/* Call next proc */
	return GL_FALSE;
    }
    return GL_TRUE;
}

/*
** Depth test a span when stenciling is enabled.
*/
GLboolean __glDepthTestStencilSpan(__GLcontext *gc)
{
    __GLstencilCell *sfb, *zPassOp, *zFailOp;
    __GLzValue zFrac, z, dzdx, dzdxBig, *zfb;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * );
    __GLstippleWord bit, outMask, *osp;
    GLint w;

    w = gc->polygon.shader.length;

    zfb = gc->polygon.shader.zbuf;
    sfb = gc->polygon.shader.sbuf;
    zFailOp = gc->stencilBuffer.depthFailOpTable;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    testFunc = gc->procs.DTPixel;
    zFrac = gc->polygon.shader.frag.z;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;
    dzdx = gc->polygon.shader.dzdx >> numFracBits;
    dzdxBig = gc->polygon.shader.dzdxBig;
    osp = gc->polygon.shader.stipplePat;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	z = (zFrac >> numFracBits) + cnt;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if( (*testFunc)(z,zfb) ) {
		sfb[0] = zPassOp[sfb[0]];
	    } else {
		sfb[0] = zFailOp[sfb[0]];
		outMask &= ~bit;
		failed++;
	    }
	    z += dzdx;
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*osp++ = outMask;
	zFrac += dzdxBig;
    }

    if (failed == 0) {
	/* Call next span proc */
	return GL_FALSE;
    } else {
	if (failed != gc->polygon.shader.length) {
	    /* Call next stippled span proc */
	    return GL_TRUE;
	}
    }
    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}

GLboolean __glDepthTestStencilStippledSpan(__GLcontext *gc)
{
    __GLstencilCell *sfb, *zPassOp, *zFailOp;
    __GLzValue zFrac, z, dzdx, dzdxBig, *zfb;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * );
    __GLstippleWord bit, inMask, outMask, *sp;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    zfb = gc->polygon.shader.zbuf;
    sfb = gc->polygon.shader.sbuf;
    zFailOp = gc->stencilBuffer.depthFailOpTable;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    testFunc = gc->procs.DTPixel;
    zFrac = gc->polygon.shader.frag.z;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;
    dzdx = gc->polygon.shader.dzdx >> numFracBits;
    dzdxBig = gc->polygon.shader.dzdxBig;
    failed = 0;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	z = (zFrac >> numFracBits) + cnt;
	inMask = *sp;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		if( (*testFunc)(z,zfb) ) {
		    sfb[0] = zPassOp[sfb[0]];
		} else {
		    sfb[0] = zFailOp[sfb[0]];
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;
	    z += dzdx;
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*sp++ = outMask & inMask;
	zFrac += dzdxBig;
    }

    if (failed != gc->polygon.shader.length) {
	/* Call next proc */
	return GL_FALSE;
    }

    return GL_TRUE;
}
#endif

/*
** Depth test a span when stenciling is enabled.
*/
GLboolean __glDepthTestStencilOffsetSpan(__GLcontext *gc)
{
    __GLstencilCell *sfb, *zPassOp, *zFailOp;
    __GLzValue z, *zfb;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc[2])( __GLzValue, __GLzValue * );
    __GLstippleWord bit, outMask, *osp;

    GLuint i; /* index */
    GLint w[3]; /* segment count (last entry is always 0) */
    __GLzValue zFrac[2];
    __GLzValue dzdx[2];
    __GLzValue dzdxBig[2];

    __GLfloat offsetZ; /* actual z value */
    __GLfloat outCount;
    __GLfloat range;

    GLint boundary;
    __GLzValue boundarydzdxBig;

#if defined(POLYGON_OFFSET_DEBUG)
    GLdouble offsetZtest;
    GLdouble ztest;

    offsetZtest = gc->polygon.shader.offsetZ;
#endif /* POLYGON_OFFSET_DEBUG */

    w[0] = gc->polygon.shader.length;
    w[1] = 0;
    w[2] = 0; /* termination condition */

    zfb = gc->polygon.shader.zbuf;
    sfb = gc->polygon.shader.sbuf;
    zFailOp = gc->stencilBuffer.depthFailOpTable;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    testFunc[0] = gc->procs.DTPixel;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;

    osp = gc->polygon.shader.stipplePat;
    failed = 0;
    boundary = 0;

    offsetZ = gc->polygon.shader.offsetZ; 
    outCount = gc->polygon.shader.outBoundCount;
    range = gc->polygon.shader.rangeCount;

    if(outCount > 0) { /* heading towards it */
      if(outCount < range) { /* starts inside */
	/* offsetZ should be within z buffer range */
	assert(offsetZ >= 0. && offsetZ <= gc->depthBuffer.scale);
	zFrac[0] = offsetZ;
	dzdx[0] = gc->polygon.shader.dzdx >> numFracBits;
	dzdxBig[0] = gc->polygon.shader.dzdxBig;
	if(w[0] > outCount) { /* going out */
	  w[1] = w[0] - outCount;
	  w[0] -= w[1];
	  if((gc->depthBuffer.testFunc == GL_LESS &&
	      gc->polygon.shader.endZLimit != 0) ||
	     (gc->depthBuffer.testFunc == GL_GREATER &&
	     gc->polygon.shader.endZLimit == 0))
	      testFunc[1] = __glCDTPixel[0]; /* pick GL_NEVER */
	    else
	      testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = gc->polygon.shader.endZLimit;
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  dzdx[1] = 0;
	  dzdxBig[1] = 0;
	}
      } else { /* starts outside */
	zFrac[0] = gc->polygon.shader.startZLimit;
	dzdx[0] = 0;
	dzdxBig[0] = 0;
	if((gc->depthBuffer.testFunc == GL_LESS &&
	    gc->polygon.shader.startZLimit != 0) ||
	   (gc->depthBuffer.testFunc == GL_GREATER &&
	    gc->polygon.shader.startZLimit == 0))
	  testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
	if(w[0] > (GLint)(outCount - range + 1.)) { /* goes in */
	  w[1] = w[0] - (GLint)(outCount - range + 1.);
	  w[0] = (GLint)(outCount - range + 1.);
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ;
	  /* starting Z for 2nd segment should be inside z-buffer range */
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) >= 0.);
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) <= gc->depthBuffer.scale);
	  dzdx[1] = gc->polygon.shader.dzdx >> numFracBits;
	  dzdxBig[1] = gc->polygon.shader.dzdxBig;
	}
      }
    } else { /* already past */
      if((gc->depthBuffer.testFunc == GL_LESS &&
	  gc->polygon.shader.endZLimit != 0) ||
	 (gc->depthBuffer.testFunc == GL_GREATER &&
	  gc->polygon.shader.endZLimit == 0))
	testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
      zFrac[0] = gc->polygon.shader.endZLimit;
      dzdx[0] = 0;
      dzdxBig[0] = 0;
    }
	
    /* this algorithm shouldn't change the number of pixels tested */
    assert(w[0] + w[1] == gc->polygon.shader.length);

    i = 0;
    while(w[i]) {
      while (w[i]) {
	count = w[i];
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w[i] -= count;

#if defined(POLYGON_OFFSET_DEBUG)
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	z = (zFrac[i] >> numFracBits) + cnt;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	    if(ztest < 0.) /* clamp to zero */
	      assert(z == cnt);
	    else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	      assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	    else
	      assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	    if( (*testFunc[i])(z,zfb) ) {
		sfb[0] = zPassOp[sfb[0]];
	    } else {
		sfb[0] = zFailOp[sfb[0]];
		outMask &= ~bit;
		failed++;
	    }
#if defined(POLYGON_OFFSET_DEBUG)
	    ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	    z += dzdx[i];
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*osp++ = outMask;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig;
#endif	 /* POLYGON_OFFSET_DEBUG */
	zFrac[i] += dzdxBig[i];
      }
      i++;
      if(w[i] && boundary) { /* stitch mask together */
	count = __GL_STIPPLE_BITS - boundary;
	boundarydzdxBig = (GLint)((GLint)dzdxBig[i] * 
	                  ((GLdouble)count / (GLdouble)__GL_STIPPLE_BITS));
	if(count > w[i])
	  count = w[i];
	w[i] -= count;
	osp--;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest -= gc->polygon.shader.dzdxBig;
	offsetZtest += gc->polygon.shader.dzdxBig * (GLdouble)boundary / 
	               (GLdouble)__GL_STIPPLE_BITS;
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	z = (zFrac[i] >> numFracBits) + cnt;

	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	    if(ztest < 0.) /* clamp to zero */
	      assert(z == cnt);
	    else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	      assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	    else
	      assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	    if( (*testFunc[i])(z,zfb) ) {
		sfb[0] = zPassOp[sfb[0]];
	    } else {
		sfb[0] = zFailOp[sfb[0]];
		outMask &= ~bit;
		failed++;
	    }
#if defined(POLYGON_OFFSET_DEBUG)
	    ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	    z += dzdx[i];
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	  bit >>= 1;
#else
	  bit <<= 1;
#endif
	}
	*osp++ = outMask;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig * 
	               (GLdouble)(__GL_STIPPLE_BITS - boundary) /
		       (GLdouble)__GL_STIPPLE_BITS;
#endif	 /* POLYGON_OFFSET_DEBUG */
	zFrac[i] += boundarydzdxBig;
      }
    }

    if (failed == 0) {
	/* Call next span proc */
	return GL_FALSE;
    } else {
	if (failed != gc->polygon.shader.length) {
	    /* Call next stippled span proc */
	    return GL_TRUE;
	}
    }

    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}

GLboolean __glDepthTestStencilStippledOffsetSpan(__GLcontext *gc)
{
    __GLstencilCell *sfb, *zPassOp, *zFailOp;
    __GLzValue z, *zfb;
    GLint numFracBits;
    GLuint cnt;
    GLint failed, count;
    GLboolean (*testFunc[2])( __GLzValue, __GLzValue * );
    __GLstippleWord bit, inMask, outMask, *sp;

    GLuint i; /* index */
    GLint w[3]; /* segment count (last entry is always 0) */
    __GLzValue zFrac[2];
    __GLzValue dzdx[2];
    __GLzValue dzdxBig[2];

    __GLfloat offsetZ; /* actual z value */
    __GLfloat outCount;
    __GLfloat range;

    GLint boundary;
    __GLzValue boundarydzdxBig;

#if defined(POLYGON_OFFSET_DEBUG)
    GLdouble offsetZtest;
    GLdouble ztest;

    offsetZtest = gc->polygon.shader.offsetZ;
#endif /* POLYGON_OFFSET_DEBUG */

    w[0] = gc->polygon.shader.length;
    w[1] = 0;
    w[2] = 0; /* termination condition */

    zfb = gc->polygon.shader.zbuf;
    sfb = gc->polygon.shader.sbuf;
    zFailOp = gc->stencilBuffer.depthFailOpTable;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    testFunc[0] = gc->procs.DTPixel;
    numFracBits = __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;
    cnt = __GL_DEPTH_DP(&gc->depthBuffer)->cnt;

    sp = gc->polygon.shader.stipplePat;

    failed = 0;
    boundary = 0;

    offsetZ = gc->polygon.shader.offsetZ; 
    outCount = gc->polygon.shader.outBoundCount;
    range = gc->polygon.shader.rangeCount;

    if(outCount > 0) { /* heading towards it */
      if(outCount < range) { /* starts inside */
	/* offsetZ should be within z buffer range */
	assert(offsetZ >= 0. && offsetZ <= gc->depthBuffer.scale);
	zFrac[0] = offsetZ;
	dzdx[0] = gc->polygon.shader.dzdx >> numFracBits;
	dzdxBig[0] = gc->polygon.shader.dzdxBig;
	if(w[0] > outCount) { /* going out */
	  w[1] = w[0] - outCount;
	  w[0] -= w[1];
	  if((gc->depthBuffer.testFunc == GL_LESS &&
	      gc->polygon.shader.endZLimit != 0) ||
	     (gc->depthBuffer.testFunc == GL_GREATER &&
	     gc->polygon.shader.endZLimit == 0))
	      testFunc[1] = __glCDTPixel[0]; /* pick GL_NEVER */
	    else
	      testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = gc->polygon.shader.endZLimit;
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  dzdx[1] = 0;
	  dzdxBig[1] = 0;
	}
      } else { /* starts outside */
	zFrac[0] = gc->polygon.shader.startZLimit;
	dzdx[0] = 0;
	dzdxBig[0] = 0;
	if((gc->depthBuffer.testFunc == GL_LESS &&
	    gc->polygon.shader.startZLimit != 0) ||
	   (gc->depthBuffer.testFunc == GL_GREATER &&
	    gc->polygon.shader.startZLimit == 0))
	  testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
	if(w[0] > (GLint)(outCount - range + 1.)) { /* goes in */
	  w[1] = w[0] - (GLint)(outCount - range + 1.);
	  w[0] = (GLint)(outCount - range + 1.);
	  boundary = w[0] % __GL_STIPPLE_BITS;
	  testFunc[1] = gc->procs.DTPixel;
	  zFrac[1] = w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ;
	  /* starting Z for 2nd segment should be inside z-buffer range */
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) >= 0.);
	  assert((w[0] / __GL_STIPPLE_BITS * gc->polygon.shader.dzdxBig + 
	             boundary * gc->polygon.shader.dzdx +
		     (GLdouble)offsetZ) <= gc->depthBuffer.scale);
	  dzdx[1] = gc->polygon.shader.dzdx >> numFracBits;
	  dzdxBig[1] = gc->polygon.shader.dzdxBig;
	}
      }
    } else { /* already past */
      if((gc->depthBuffer.testFunc == GL_LESS &&
	  gc->polygon.shader.endZLimit != 0) ||
	 (gc->depthBuffer.testFunc == GL_GREATER &&
	  gc->polygon.shader.endZLimit == 0))
	testFunc[0] = __glCDTPixel[0]; /* pick GL_NEVER */
      zFrac[0] = gc->polygon.shader.endZLimit;
      dzdx[0] = 0;
      dzdxBig[0] = 0;
    }
	
    /* this algorithm shouldn't change the number of pixels tested */
    assert(w[0] + w[1] == gc->polygon.shader.length);

    i = 0;
    while(w[i]) {
      while (w[i]) {
	count = w[i];
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w[i] -= count;

#if defined(POLYGON_OFFSET_DEBUG)
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	z = (zFrac[i] >> numFracBits) + cnt;
	inMask = *sp;
	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	    if(ztest < 0.) /* clamp to zero */
	      assert(z == cnt);
	    else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	      assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	    else
	      assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	    if (inMask & bit) {
		if( (*testFunc[i])(z,zfb) ) {
		    sfb[0] = zPassOp[sfb[0]];
		} else {
		    sfb[0] = zFailOp[sfb[0]];
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;
#if defined(POLYGON_OFFSET_DEBUG)
	    ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	    z += dzdx[i];
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*sp++ = outMask & inMask;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig;
#endif /* POLYGON_OFFSET_DEBUG */
	zFrac[i] += dzdxBig[i];
      }
      i++;
      if(w[i] && boundary) { /* stitch mask together */
	count = __GL_STIPPLE_BITS - boundary;
	boundarydzdxBig = (GLint)((GLint)dzdxBig[i] * 
	                  ((GLdouble)count / (GLdouble)__GL_STIPPLE_BITS));
	if(count > w[i])
	  count = w[i];
	w[i] -= count;
	sp--;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest -= gc->polygon.shader.dzdxBig;
	offsetZtest += gc->polygon.shader.dzdxBig * (GLdouble)boundary / 
	               (GLdouble)__GL_STIPPLE_BITS;
	ztest = __glFloor((offsetZtest + .5) / (1 << numFracBits));
#endif /* POLYGON_OFFSET_DEBUG */
	z = (zFrac[i] >> numFracBits) + cnt;
	while (--count >= 0) {
#if defined(POLYGON_OFFSET_DEBUG)
	    /*
	    ** algorithm should generate the same z values that a per
	    ** pixel clamping scheme (i.e. spec description) would
	    */
	    if(ztest < 0.) /* clamp to zero */
	      assert(z == cnt);
	    else if(ztest >  (gc->depthBuffer.scale >> numFracBits))
	      assert(z == (gc->depthBuffer.scale >> numFracBits) + cnt);
	    else
	      assert(z == (__GLzValue)ztest + cnt);
#endif /* POLYGON_OFFSET_DEBUG */
	    if (inMask & bit) {
		if( (*testFunc[i])(z,zfb) ) {
		    sfb[0] = zPassOp[sfb[0]];
		} else {
		    sfb[0] = zFailOp[sfb[0]];
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;
#if defined(POLYGON_OFFSET_DEBUG)
	    ztest += gc->polygon.shader.dzdx >> numFracBits;
#endif /* POLYGON_OFFSET_DEBUG */
	    z += dzdx[i];
	    zfb = (__GLzValue *) ((GLubyte *) zfb + 
				  gc->depthBuffer.buf.drawableBuf->elementSize);
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	  }
	*sp++ = outMask & inMask;
#if defined(POLYGON_OFFSET_DEBUG)
	offsetZtest += gc->polygon.shader.dzdxBig * 
	               (GLdouble)(__GL_STIPPLE_BITS - boundary) /
		       (GLdouble)__GL_STIPPLE_BITS;
#endif /* POLYGON_OFFSET_DEBUG */
	zFrac[i] += boundarydzdxBig;
      }
    }


    if (failed != gc->polygon.shader.length) {
	/* Call next proc */
	return GL_FALSE;
    }

    return GL_TRUE;
}


/*
** Apply stencil depth pass op when depth testing is off.
*/
GLboolean __glDepthPassSpan(__GLcontext *gc)
{
    __GLstencilCell *sfb, *zPassOp;
    GLint count;
    GLint w;

    w = gc->polygon.shader.length;

    sfb = gc->polygon.shader.sbuf;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    count = w;
    while (--count >= 0) {
	sfb[0] = zPassOp[sfb[0]];
	sfb++;
    }

    return GL_FALSE;
}

/*
** Apply stencil depth pass op when depth testing is off.
*/
GLboolean __glDepthPassStippledSpan(__GLcontext *gc)
{
    __GLstencilCell *sfb, *zPassOp;
    GLint count;
    __GLstippleWord bit, inMask, *sp;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    sfb = gc->polygon.shader.sbuf;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		sfb[0] = zPassOp[sfb[0]];
	    }
	    sfb++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    /* Call next proc */
    return GL_FALSE;
}

/************************************************************************/

GLboolean __glShadeCISpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLfloat r, drdx;
    GLint w;

    w = gc->polygon.shader.length;

    r = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r;
    drdx = gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdx;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	cp->r = r;
	r += drdx;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glShadeRGBASpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLfloat r, g, b, a;
    __GLfloat drdx, dgdx, dbdx, dadx;
    GLint w;

    w = gc->polygon.shader.length;

    r = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r;
    g = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].g;
    b = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].b;
    a = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].a;
    drdx = gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdx;
    dgdx = gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dgdx;
    dbdx = gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dbdx;
    dadx = gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dadx;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	cp->r = r;
	cp->g = g;
	cp->b = b;
	cp->a = a;
	r += drdx;
	g += dgdx;
	b += dbdx;
	a += dadx;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glShadeSecondaryRGBASpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLfloat r, g, b, a;
    __GLfloat drdx, dgdx, dbdx, dadx;
    GLint w;

    w = gc->polygon.shader.length;

    r = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].r;
    g = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].g;
    b = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].b;
    a = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].a;
    drdx = gc->polygon.shader.colorIter[__GL_SECONDARY_COLOR].drdx;
    dgdx = gc->polygon.shader.colorIter[__GL_SECONDARY_COLOR].dgdx;
    dbdx = gc->polygon.shader.colorIter[__GL_SECONDARY_COLOR].dbdx;
    dadx = gc->polygon.shader.colorIter[__GL_SECONDARY_COLOR].dadx;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	cp->r += r;
	cp->g += g;
	cp->b += b;
	cp->a += a;
	r += drdx;
	g += dgdx;
	b += dbdx;
	a += dadx;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glFlatCISpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLfloat r;
    GLint w;

    w = gc->polygon.shader.length;

    r = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	cp->r = r;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glFlatRGBASpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLfloat r, g, b, a;
    GLint w;

    w = gc->polygon.shader.length;

    r = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r;
    g = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].g;
    b = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].b;
    a = gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].a;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	cp->r = r;
	cp->g = g;
	cp->b = b;
	cp->a = a;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glFlatSecondaryRGBASpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLfloat r, g, b, a;
    GLint w;

    w = gc->polygon.shader.length;

    r = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].r;
    g = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].g;
    b = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].b;
    a = gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].a;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	cp->r += r;
	cp->g += g;
	cp->b += b;
	cp->a += a;
	cp++;
    }

    return GL_FALSE;
}

/************************************************************************/

GLboolean __glTextureSpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLcoord strq[__GL_NUM_TEXTURE_UNITS];
    __GLfloat rhow[__GL_NUM_TEXTURE_UNITS];
    GLint w, i;	

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	strq[i].x = gc->polygon.shader.frag.texture[i].s;
	strq[i].y = gc->polygon.shader.frag.texture[i].t;
	strq[i].z = gc->polygon.shader.frag.texture[i].r;
	strq[i].w = gc->polygon.shader.frag.texture[i].qw;
	rhow[i] = gc->polygon.shader.frag.texture[i].rhow;
    }

    w = gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;

    while( --w >= 0 ) {

	(*gc->procs.texture)(gc, cp, strq, rhow);

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    strq[i].x += gc->polygon.shader.texture[i].dsdx;
	    strq[i].y += gc->polygon.shader.texture[i].dtdx;
	    strq[i].z += gc->polygon.shader.texture[i].drdx;
	    strq[i].w += gc->polygon.shader.texture[i].dqwdx;
	    rhow[i] += gc->polygon.shader.texture[i].drhowdx;
	}
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glTextureStippledSpan(__GLcontext *gc)
{
    __GLstippleWord inMask, bit, *sp;
    GLint count;
    __GLcolor *cp;
    __GLcoord strq[__GL_NUM_TEXTURE_UNITS];
    __GLfloat rhow[__GL_NUM_TEXTURE_UNITS];
    GLint w, i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	strq[i].x = gc->polygon.shader.frag.texture[i].s;
	strq[i].y = gc->polygon.shader.frag.texture[i].t;
	strq[i].z = gc->polygon.shader.frag.texture[i].r;
	strq[i].w = gc->polygon.shader.frag.texture[i].qw;
	rhow[i] = gc->polygon.shader.frag.texture[i].rhow;
    }

    w = gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    sp = gc->polygon.shader.stipplePat;

    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {

	    if (inMask & bit) {
		(*gc->procs.texture)(gc, cp, strq, rhow);
	    }

	    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
		strq[i].x += gc->polygon.shader.texture[i].dsdx;
		strq[i].y += gc->polygon.shader.texture[i].dtdx;
		strq[i].z += gc->polygon.shader.texture[i].drdx;
		strq[i].w += gc->polygon.shader.texture[i].dqwdx;
		rhow[i] += gc->polygon.shader.texture[i].drhowdx;
	    }
	    cp++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/************************************************************************/

/*
** Dither CI span.
*/
GLboolean __glDitherCISpan(__GLcontext *gc)
{
    __GLcolor *cp;
    GLint ix, x, y, r, maxR, frac;
    GLint w;

    w = gc->polygon.shader.length;

    x = gc->polygon.shader.frag.x;
    y = gc->polygon.shader.frag.y;
    maxR = (1 << gc->modes.indexBits) - 1;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	ix = __GL_DITHER_INDEX(x, y);
	frac = __glDitherTable[ix];
	r = ((GLint) (cp->r * __GL_DITHER_PRECISION + __glHalf) + frac)
	    >> __GL_DITHER_BITS;
	if (r > maxR) r = maxR;
	cp->r = r;
	cp++;
	x++;
    }

    return GL_FALSE;
}

/*
** Dither CI stippled span.
*/
GLboolean __glDitherCIStippledSpan(__GLcontext *gc)
{
    __GLstippleWord bit, inMask, *sp;
    __GLcolor *cp;
    GLint count, ix, x, y, r, maxR, frac;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    x = gc->polygon.shader.frag.x;
    y = gc->polygon.shader.frag.y;
    maxR = (1 << gc->modes.indexBits) - 1;
    cp = gc->polygon.shader.colors;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		ix = __GL_DITHER_INDEX(x, y);
		frac = __glDitherTable[ix];
		r = ((GLint) (cp->r * __GL_DITHER_PRECISION + __glHalf) + frac)
		    >> __GL_DITHER_BITS;
		if (r > maxR) r = maxR;
		cp->r = r;
	    }
	    cp++;
	    x++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/*
** Dither RGBA span.  Only works if incoming colors have already been
** scaled into their destination format.
*/
GLboolean __glDitherRGBASpan(__GLcontext *gc)
{
    __GLcolor *cp;
    GLint ix, x, y, r, g, b, a, frac;
    GLint maxR, maxG, maxB, maxA;
    GLint w;

    w = gc->polygon.shader.length;

    x = gc->polygon.shader.frag.x;
    y = gc->polygon.shader.frag.y;
    maxR = gc->frontBuffer.iRedScale;
    maxG = gc->frontBuffer.iGreenScale;
    maxB = gc->frontBuffer.iBlueScale;
    maxA = gc->frontBuffer.iAlphaScale;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	ix = __GL_DITHER_INDEX(x, y);
	/*
	** Convert the color into integers, keeping 4 bits of fractional
	** precision (and rounding up).
	*/
	frac = __glDitherTable[ix];
	r = ((GLint) (cp->r * __GL_DITHER_PRECISION + __glHalf) + frac)
	    >> __GL_DITHER_BITS;
	if (r > maxR) r = maxR;
	cp->r = r;

	g = ((GLint) (cp->g * __GL_DITHER_PRECISION + __glHalf) + frac)
	    >> __GL_DITHER_BITS;
	if (g > maxG) g = maxG;
	cp->g = g;

	b = ((GLint) (cp->b * __GL_DITHER_PRECISION + __glHalf) + frac)
	    >> __GL_DITHER_BITS;
	if (b > maxB) b = maxB;
	cp->b = b;

	a = ((GLint) (cp->a * __GL_DITHER_PRECISION + __glHalf) + frac)
	    >> __GL_DITHER_BITS;
	if (a > maxA) a = maxA;
	cp->a = a;

	cp++;
	x++;
    }

    return GL_FALSE;
}

/*
** Dither RGBA stippled span.  Only works if incoming colors have already
** been scaled into their destination format.
*/
GLboolean __glDitherRGBAStippledSpan(__GLcontext *gc)
{
    __GLstippleWord bit, inMask, *sp;
    __GLcolor *cp;
    GLint count, ix, x, y, r, g, b, a, frac;
    GLint maxR, maxG, maxB, maxA;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    x = gc->polygon.shader.frag.x;
    y = gc->polygon.shader.frag.y;
    maxR = gc->frontBuffer.iRedScale;
    maxG = gc->frontBuffer.iGreenScale;
    maxB = gc->frontBuffer.iBlueScale;
    maxA = gc->frontBuffer.iAlphaScale;
    cp = gc->polygon.shader.colors;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		ix = __GL_DITHER_INDEX(x, y);
		/*
		** Convert the color into integers, keeping 4 bits of
		** fractional precision (and rounding up).
		*/
		frac = __glDitherTable[ix];
		r = ((GLint) (cp->r * __GL_DITHER_PRECISION + __glHalf) + frac)
		    >> __GL_DITHER_BITS;
		if (r > maxR) r = maxR;
		cp->r = r;

		g = ((GLint) (cp->g * __GL_DITHER_PRECISION + __glHalf) + frac)
		    >> __GL_DITHER_BITS;
		if (g > maxG) g = maxG;
		cp->g = g;

		b = ((GLint) (cp->b * __GL_DITHER_PRECISION + __glHalf) + frac)
		    >> __GL_DITHER_BITS;
		if (b > maxB) b = maxB;
		cp->b = b;

		a = ((GLint) (cp->a * __GL_DITHER_PRECISION + __glHalf) + frac)
		    >> __GL_DITHER_BITS;
		if (a > maxA) a = maxA;
		cp->a = a;
	    }
	    cp++;
	    x++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/*
** Round CI span.
*/
GLboolean __glRoundCISpan(__GLcontext *gc)
{
    __GLcolor *cp;
    GLint x, r, maxR;
    GLint w;

    w = gc->polygon.shader.length;
    x = gc->polygon.shader.frag.x;
    maxR = (1 << gc->modes.indexBits) - 1;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	r = (GLint)(cp->r + __glHalf);
	if (r > maxR) { r = maxR; }
	cp->r = r;
	cp++;
	x++;
    }

    return GL_FALSE;
}

/*
** Round CI stippled span.
*/
GLboolean __glRoundCIStippledSpan(__GLcontext *gc)
{
    __GLstippleWord bit, inMask, *sp;
    __GLcolor *cp;
    GLint count, x, r, maxR;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    x = gc->polygon.shader.frag.x;
    maxR = (1 << gc->modes.indexBits) - 1;
    cp = gc->polygon.shader.colors;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		r = (GLint)(cp->r + __glHalf);
		if (r > maxR) { r = maxR; }
		cp->r = r;
	    }
	    cp++;
	    x++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/*
** Round RGBA span.  Only works if incoming colors have already been
** scaled into their destination format.
*/
GLboolean __glRoundRGBASpan(__GLcontext *gc)
{
    __GLcolor *cp;
    GLint x, r, g, b, a;
    GLint maxR, maxG, maxB, maxA;
    GLint w;

    w = gc->polygon.shader.length;

    x = gc->polygon.shader.frag.x;
    maxR = gc->frontBuffer.iRedScale;
    maxG = gc->frontBuffer.iGreenScale;
    maxB = gc->frontBuffer.iBlueScale;
    maxA = gc->frontBuffer.iAlphaScale;
    cp = gc->polygon.shader.colors;
    while (--w >= 0) {
	r = (GLint) (cp->r + __glHalf);
	if (r > maxR) r = maxR;
	cp->r = r;
	g = (GLint) (cp->g + __glHalf);
	if (g > maxG) g = maxG;
	cp->g = g;
	b = (GLint) (cp->b + __glHalf);
	if (b > maxB) b = maxB;
	cp->b = b;
	a = (GLint) (cp->a + __glHalf);
	if (a > maxA) a = maxA;
	cp->a = a;
	cp++;
	x++;
    }

    return GL_FALSE;
}

/*
** Round RGBA stippled span.  Only works if incoming colors have already
** been scaled into their destination format.
*/
GLboolean __glRoundRGBAStippledSpan(__GLcontext *gc)
{
    __GLstippleWord bit, inMask, *sp;
    __GLcolor *cp;
    GLint count, x, r, g, b, a;
    GLint maxR, maxG, maxB, maxA;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    x = gc->polygon.shader.frag.x;
    maxR = gc->frontBuffer.iRedScale;
    maxG = gc->frontBuffer.iGreenScale;
    maxB = gc->frontBuffer.iBlueScale;
    maxA = gc->frontBuffer.iAlphaScale;
    cp = gc->polygon.shader.colors;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		r = (GLint) (cp->r + __glHalf);
		if (r > maxR) r = maxR;
		cp->r = r;
		g = (GLint) (cp->g + __glHalf);
		if (g > maxG) g = maxG;
		cp->g = g;
		b = (GLint) (cp->b + __glHalf);
		if (b > maxB) b = maxB;
		cp->b = b;
		a = (GLint) (cp->a + __glHalf);
		if (a > maxA) a = maxA;
		cp->a = a;
	    }
	    cp++;
	    x++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/************************************************************************/

GLboolean __glLogicOpSpan(__GLcontext *gc)
{
    __GLcolor *cp, *fcp;
    GLint x, color, fbcolor;
    GLint w;

    w = gc->polygon.shader.length;

    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    x = gc->polygon.shader.frag.x;
    while (--w >= 0) {
	color = (GLint) cp->r;
	fbcolor = (GLint) fcp->r;
	switch (gc->state.raster.logicOp) {
	  case GL_CLEAR:         color = 0; break;
	  case GL_AND:           color = color & fbcolor; break;
	  case GL_AND_REVERSE:   color = color & (~fbcolor); break;
	  case GL_COPY:          color = color; break;
	  case GL_AND_INVERTED:  color = (~color) & fbcolor; break;
	  case GL_NOOP:          color = fbcolor; break;
	  case GL_XOR:           color = color ^ fbcolor; break;
	  case GL_OR:            color = color | fbcolor; break;
	  case GL_NOR:           color = ~(color | fbcolor); break;
	  case GL_EQUIV:         color = ~(color ^ fbcolor); break;
	  case GL_INVERT:        color = ~fbcolor; break;
	  case GL_OR_REVERSE:    color = color | (~fbcolor); break;
	  case GL_COPY_INVERTED: color = ~color; break;
	  case GL_OR_INVERTED:   color = (~color) | fbcolor; break;
	  case GL_NAND:          color = ~(color & fbcolor); break;
	  case GL_SET:           color = ~0; break;
	}
	cp->r = color;
	cp++;
	fcp++;
	x++;
    }

    return GL_FALSE;
}

GLboolean __glLogicOpStippledSpan(__GLcontext *gc)
{
    __GLstippleWord bit, inMask, *sp;
    __GLcolor *cp, *fcp;
    GLint count, x, color, fbcolor;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    x = gc->polygon.shader.frag.x;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		color = (GLint) cp->r;
		fbcolor = (GLint) fcp->r;
		switch (gc->state.raster.logicOp) {
		  case GL_CLEAR:         color = 0; break;
		  case GL_AND:           color = color & fbcolor; break;
		  case GL_AND_REVERSE:   color = color & (~fbcolor); break;
		  case GL_COPY:          color = color; break;
		  case GL_AND_INVERTED:  color = (~color) & fbcolor; break;
		  case GL_NOOP:          color = fbcolor; break;
		  case GL_XOR:           color = color ^ fbcolor; break;
		  case GL_OR:            color = color | fbcolor; break;
		  case GL_NOR:           color = ~(color | fbcolor); break;
		  case GL_EQUIV:         color = ~(color ^ fbcolor); break;
		  case GL_INVERT:        color = ~fbcolor; break;
		  case GL_OR_REVERSE:    color = color | (~fbcolor); break;
		  case GL_COPY_INVERTED: color = ~color; break;
		  case GL_OR_INVERTED:   color = (~color) | fbcolor; break;
		  case GL_NAND:          color = ~(color & fbcolor); break;
		  case GL_SET:           color = ~0; break;
		}
		cp->r = color;
	    }
	    cp++;
	    fcp++;
	    x++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/************************************************************************/

GLboolean __glMaskRGBASpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLcolor *fcp;
    GLboolean rEnable, gEnable, bEnable, aEnable;
    GLint w;

    w = gc->polygon.shader.length;

    rEnable = gc->state.raster.rMask;
    gEnable = gc->state.raster.gMask;
    bEnable = gc->state.raster.bMask;
    aEnable = gc->state.raster.aMask;
    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    while (--w >= 0) {
	cp->r = rEnable ? cp->r : fcp->r;
	cp->g = gEnable ? cp->g : fcp->g;
	cp->b = bEnable ? cp->b : fcp->b;
	cp->a = aEnable ? cp->a : fcp->a;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glMaskCISpan(__GLcontext *gc)
{
    __GLcolor *cp;
    __GLcolor *fcp;
    GLuint keepMask, changeMask, r, fr;
    GLint w;

    w = gc->polygon.shader.length;
    changeMask = gc->state.raster.writeMask;
    keepMask = __GL_MASK_INDEXI(gc, ~changeMask);
    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    while (--w >= 0) {
	r = (GLuint) cp->r;
	fr = (GLuint) fcp->r;
	cp->r = (r & changeMask) | (fr & keepMask);
	cp++;
    }

    return GL_FALSE;
}
