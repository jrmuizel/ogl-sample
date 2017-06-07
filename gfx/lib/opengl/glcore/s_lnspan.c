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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_lnspan.c#5 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "g_imports.h"

/*
** Process the incoming line by calling all of the appropriate line procs.
** Return value is ignored.
**
** It sets gc->polygon.shader.cfb to gc->drawBuffer.
*/
GLboolean __glProcessLine(__GLcontext *gc)
{
    GLboolean stippling;
    GLint i,n;

#if !defined(__GL_ALLOCATE_SCANLINE_ARRAYS)
    __GLcolor colors[__GL_MAX_MAX_VIEWPORT];/*XXX oink */
    __GLcolor fbcolors[__GL_MAX_MAX_VIEWPORT];/*XXX oink */
    __GLstippleWord words[__GL_MAX_STIPPLE_WORDS];

    gc->polygon.shader.colors = colors;/*XXX*/
    gc->polygon.shader.fbcolors = fbcolors;/*XXX*/
    gc->polygon.shader.stipplePat = words;
#endif /* __GL_ALLOCATE_SCANLINE_ARRAYS */

    gc->polygon.shader.cfb = gc->drawBuffer;

    stippling = GL_FALSE;
    n = gc->procs.line.n;

    gc->polygon.shader.done = GL_FALSE;

    /* Step 1:  Perform early line stipple, coloring procs */
    for (i = 0; i < n; i++) {
	if (stippling) {
	    if ((*gc->procs.line.stippledLineFuncs[i])(gc)) {
		/* Line stippled away! */
		return GL_TRUE;
	    }
	} else {
	    if ((*gc->procs.line.lineFuncs[i])(gc)) {
		if (gc->polygon.shader.done) return GL_TRUE;
		stippling = GL_TRUE;
	    }
	}
    }

    if (stippling) {
	return (*gc->procs.line.wideStippledLineRep)(gc);
    } else {
	return (*gc->procs.line.wideLineRep)(gc);
    }
}

/*
** Process the incoming line by calling the 3 appropriate line procs.  It does
** not chain to gc->procs.line.wideLineRep, but returns instead.  This is a 
** specific fast path.
**
** Return value is ignored.
**
** It sets gc->polygon.shader.cfb to gc->drawBuffer.
*/
GLboolean __glProcessLine3NW(__GLcontext *gc)
{
#if !defined(__GL_ALLOCATE_SCANLINE_ARRAYS)
    __GLcolor colors[__GL_MAX_MAX_VIEWPORT];/*XXX oink */
    __GLcolor fbcolors[__GL_MAX_MAX_VIEWPORT];/*XXX oink */
    __GLstippleWord words[__GL_MAX_STIPPLE_WORDS];

    gc->polygon.shader.colors = colors;/*XXX*/
    gc->polygon.shader.fbcolors = fbcolors;/*XXX*/
    gc->polygon.shader.stipplePat = words;
#endif /* __GL_ALLOCATE_SCANLINE_ARRAYS */

    gc->polygon.shader.cfb = gc->drawBuffer;

    gc->polygon.shader.done = GL_FALSE;

    /* Call non-stippled procs... */
    if ((*gc->procs.line.lineFuncs[0])(gc)) {
	if (gc->polygon.shader.done) return GL_TRUE;
	goto stippled1;
    }
    if ((*gc->procs.line.lineFuncs[1])(gc)) {
	if (gc->polygon.shader.done) return GL_TRUE;
	goto stippled2;
    }
    return (*gc->procs.line.lineFuncs[2])(gc);

stippled1:
    if ((*gc->procs.line.stippledLineFuncs[1])(gc)) {
	return GL_TRUE;
    }
stippled2:
    return (*gc->procs.line.stippledLineFuncs[2])(gc);
}

/*
** Take incoming line, duplicate it, and continue processing.
**
** Return value is ignored.
*/
GLboolean __glWideLineRep(__GLcontext *gc)
{
    GLint i, m, n, width;
    GLboolean stippling;

    n = gc->procs.line.n;
    m = gc->procs.line.m;
    
    width = gc->line.options.width;

    /* Step 2:  Replicate wide line */
    while (--width >= 0) {
	stippling = GL_FALSE;
	for (i = n; i < m; i++) {
	    if (stippling) {
		if ((*gc->procs.line.stippledLineFuncs[i])(gc)) {
		    /* Line stippled away! */
		    goto nextLine;
		}
	    } else {
		if ((*gc->procs.line.lineFuncs[i])(gc)) {
		    if (gc->polygon.shader.done) {
			gc->polygon.shader.done = GL_FALSE;
			goto nextLine;
		    }
		    stippling = GL_TRUE;
		}
	    }
	}
	if (stippling) {
	    (*gc->procs.line.drawStippledLine)(gc);
	} else {
	    (*gc->procs.line.drawLine)(gc);
	}
nextLine:
	if (gc->line.options.axis == __GL_X_MAJOR) {
	    gc->line.options.yStart++;
	} else {
	    gc->line.options.xStart++;
	}
    }

    return GL_FALSE;
}

/*
** Take incoming stippled line, duplicate it, and continue processing.
**
** Return value is ignored.
*/
GLboolean __glWideStippleLineRep(__GLcontext *gc)
{
    GLint i, m, n, width;
    GLint stipLen;
    GLint w;
    __GLlineState *ls = &gc->state.line;
    __GLstippleWord stipplePat[__GL_MAX_STIPPLE_WORDS], *fsp, *tsp;

    n = gc->procs.line.n;
    m = gc->procs.line.m;
    w = gc->polygon.shader.length;
    
    width = ls->aliasedWidth;

    /*
    ** XXX - Saving the stipple like this is only really necessary if 
    ** depth or stencil testing.
    */
    stipLen = (w + __GL_STIPPLE_BITS - 1) >> __GL_STIPPLE_COUNT_BITS;

    fsp = gc->polygon.shader.stipplePat;
    tsp = stipplePat;
    for (i = 0; i < stipLen; i++) {
	*tsp++ = *fsp++;
    }

    /* Step 2:  Replicate wide line */
    while (--width >= 0) {
	for (i = n; i < m; i++) {
	    if ((*gc->procs.line.stippledLineFuncs[i])(gc)) {
		/* Line stippled away! */
		goto nextLine;
	    }
	}
	(*gc->procs.line.drawStippledLine)(gc);
nextLine:
	if (width) {
	    tsp = gc->polygon.shader.stipplePat;
	    fsp = stipplePat;
	    for (i = 0; i < stipLen; i++) {
		*tsp++ = *fsp++;
	    }

	    if (gc->line.options.axis == __GL_X_MAJOR) {
		gc->line.options.yStart++;
	    } else {
		gc->line.options.xStart++;
	    }
	}
    }

    return GL_FALSE;
}

/*
** Take incoming line and draw it to both FRONT and BACK buffers.
**
** This routines sets gc->polygon.shader.cfb to &gc->frontBuffer
** and then to &gc->backBuffer
**
** Return value is ignored.
*/
GLboolean __glDrawBothLine(__GLcontext *gc)
{
    GLint i, j, m, l;
    GLboolean stippling;
    __GLcolor colors[__GL_MAX_MAX_VIEWPORT], *fcp, *tcp;
    GLint w;

    w = gc->polygon.shader.length;
    m = gc->procs.line.m;
    l = gc->procs.line.l;
    
    /*
    ** XXX - Saving colors like this is only really necessary if blending,
    ** logicOping, or masking.  
    */
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

    /* Step 3:  Draw to FRONT_AND_BACK */
    for (j = 0; j < 2; j++) {
	if (j == 0) {
	    gc->polygon.shader.cfb = &gc->frontBuffer;
	} else {
	    gc->polygon.shader.cfb = &gc->backBuffer;
	}
	stippling = GL_FALSE;
	for (i = m; i < l; i++) {
	    if (stippling) {
		if ((*gc->procs.line.stippledLineFuncs[i])(gc)) {
		    /* Line stippled away! */
		    break;
		}
	    } else {
		if ((*gc->procs.line.lineFuncs[i])(gc)) {
		    if (gc->polygon.shader.done) {
			gc->polygon.shader.done = GL_FALSE;
			break;
		    }
		    stippling = GL_TRUE;
		}
	    }
	}

	if (j == 0) {
	    tcp = gc->polygon.shader.colors;
	    fcp = colors;
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
	}
    }

    return GL_FALSE;
}

/*
** Take incoming stippled line and draw it to both FRONT and BACK buffers.
**
** Return value is ignored.
*/
GLboolean __glDrawBothStippledLine(__GLcontext *gc)
{
    GLint i, m, l, j;
    GLint stipLen;
    GLint w;
    __GLstippleWord stipplePat[__GL_MAX_STIPPLE_WORDS], *fsp, *tsp;
    __GLcolor colors[__GL_MAX_MAX_VIEWPORT], *fcp, *tcp;

    l = gc->procs.line.l;
    m = gc->procs.line.m;
    
    w = gc->polygon.shader.length;

    /*
    ** XXX - Saving colors like this is only really necessary if blending,
    ** logicOping, or masking, and not drawing to FRONT_AND_BACK (because
    ** if we are, then that proc will save colors too)
    */
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

    /*
    ** XXX - Saving the stipple like this is only really necessary if 
    ** depth or stencil testing.
    */
    stipLen = (w + __GL_STIPPLE_BITS - 1) >> __GL_STIPPLE_COUNT_BITS;

    fsp = gc->polygon.shader.stipplePat;
    tsp = stipplePat;
    for (i = 0; i < stipLen; i++) {
	*tsp++ = *fsp++;
    }

    /* Step 2:  Replicate wide line */
    for (j = 0; j < 2; j++) {
	if (j == 0) {
	    gc->polygon.shader.cfb = &gc->frontBuffer;
	} else {
	    gc->polygon.shader.cfb = &gc->backBuffer;
	}
	for (i = m; i < l; i++) {
	    if ((*gc->procs.line.stippledLineFuncs[i])(gc)) {
		/* Line stippled away! */
		break;
	    }
	}
	if (j == 0) {
	    tcp = gc->polygon.shader.colors;
	    fcp = colors;
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

	    tsp = gc->polygon.shader.stipplePat;
	    fsp = stipplePat;
	    for (i = 0; i < stipLen; i++) {
		*tsp++ = *fsp++;
	    }
	}
    }

    return GL_FALSE;
}

GLboolean __glScissorLine(__GLcontext *gc)
{
    GLint clipX0, clipX1;
    GLint clipY0, clipY1;
    GLint xStart, yStart, xEnd, yEnd;
    GLint xLittle, yLittle;
    GLint xBig, yBig;
    GLint fraction, dfraction;
    GLint highWord, lowWord;
    GLint bigs, littles;
    GLint failed, count;
    GLint w;
    __GLstippleWord bit, outMask, *osp;

    w = gc->polygon.shader.length;
    
    clipX0 = gc->transform.clipX0;
    clipX1 = gc->transform.clipX1;
    clipY0 = gc->transform.clipY0;
    clipY1 = gc->transform.clipY1;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;

    xStart = gc->line.options.xStart;
    yStart = gc->line.options.yStart;

    /* If the start point is in the scissor region, we attempt to trivially
    ** accept the line.
    */
    if (xStart >= clipX0 && xStart < clipX1 &&
	    yStart >= clipY0 && yStart < clipY1) {

	w--;	/* Makes our math simpler */
	/* Trivial accept attempt */
	xEnd = xStart + xBig * w;
	yEnd = yStart + yBig * w;
	if (xEnd >= clipX0 && xEnd < clipX1 && 
		yEnd >= clipY0 && yEnd < clipY1) {
	    return GL_FALSE;
	}

	xLittle = gc->line.options.xLittle;
	yLittle = gc->line.options.yLittle;
	fraction = gc->line.options.fraction;
	dfraction = gc->line.options.dfraction;

	/* Invert negative minor slopes so we can assume dfraction > 0 */
	if (dfraction < 0) {
	    dfraction = -dfraction;
	    fraction = 0x7fffffff - fraction;
	}

	/* Now we compute number of littles and bigs in this line */

	/* We perform a 16 by 32 bit multiply.  Ugh. */
	highWord = (((GLuint) dfraction) >> 16) * w + 
		(((GLuint) fraction) >> 16);
	lowWord = (dfraction & 0xffff) * w + (fraction & 0xffff);
	highWord += (((GLuint) lowWord) >> 16);
	bigs = ((GLuint) highWord) >> 15;
	littles = w - bigs;

	/* Second trivial accept attempt */
	xEnd = xStart + xBig*bigs + xLittle*littles;
	yEnd = yStart + yBig*bigs + yLittle*littles;
	if (xEnd >= clipX0 && xEnd < clipX1 && 
		yEnd >= clipY0 && yEnd < clipY1) {
	    return GL_FALSE;
	}
	w++;	/* Restore w */
    } else {
	xLittle = gc->line.options.xLittle;
	yLittle = gc->line.options.yLittle;
	fraction = gc->line.options.fraction;
	dfraction = gc->line.options.dfraction;
    }

    /*
    ** Note that we don't bother to try trivially rejecting this line.  After
    ** all, it has already been clipped, and the only way that it might
    ** possibly be trivially rejected is if it is a piece of a wide line that
    ** runs right along the edge of the window.
    */

    /*
    ** This sucks.  The line needs to be scissored.
    ** Well, it should only happen rarely, so we can afford
    ** to make it slow.  We achieve this by tediously stippling the line.
    ** (rather than clipping it, of course, which would be faster but harder).
    */
    failed = 0;
    osp = gc->polygon.shader.stipplePat;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	outMask = ~0;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (xStart < clipX0 || xStart >= clipX1 ||
		    yStart < clipY0 || yStart >= clipY1) {
		outMask &= ~bit;
		failed++;
	    }

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		xStart += xBig;
		yStart += yBig;
	    } else {
		xStart += xLittle;
		yStart += yLittle;
	    }

#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}

	*osp++ = outMask;
    }

    if (failed != gc->polygon.shader.length) {
	/* Call next proc */
	return GL_TRUE;
    }

    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}

GLboolean __glScissorStippledLine(__GLcontext *gc)
{
    GLint clipX0, clipX1;
    GLint clipY0, clipY1;
    GLint xStart, yStart, xEnd, yEnd;
    GLint xLittle, yLittle;
    GLint xBig, yBig;
    GLint fraction, dfraction;
    GLint highWord, lowWord;
    GLint bigs, littles;
    GLint failed, count;
    GLint w;
    __GLstippleWord *sp;
    __GLstippleWord bit, outMask, inMask;

    w = gc->polygon.shader.length;

    clipX0 = gc->transform.clipX0;
    clipX1 = gc->transform.clipX1;
    clipY0 = gc->transform.clipY0;
    clipY1 = gc->transform.clipY1;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;

    xStart = gc->line.options.xStart;
    yStart = gc->line.options.yStart;

    /* If the start point is in the scissor region, we attempt to trivially
    ** accept the line.
    */
    if (xStart >= clipX0 && xStart < clipX1 &&
	    yStart >= clipY0 && yStart < clipY1) {

	w--;	/* Makes our math simpler */
	/* Trivial accept attempt */
	xEnd = xStart + xBig * w;
	yEnd = yStart + yBig * w;
	if (xEnd >= clipX0 && xEnd < clipX1 && 
		yEnd >= clipY0 && yEnd < clipY1) {
	    return GL_FALSE;
	}

	xLittle = gc->line.options.xLittle;
	yLittle = gc->line.options.yLittle;
	fraction = gc->line.options.fraction;
	dfraction = gc->line.options.dfraction;

	/* Invert negative minor slopes so we can assume dfraction > 0 */
	if (dfraction < 0) {
	    dfraction = -dfraction;
	    fraction = 0x7fffffff - fraction;
	}

	/* Now we compute number of littles and bigs in this line */

	/* We perform a 16 by 32 bit multiply.  Ugh. */
	highWord = (((GLuint) dfraction) >> 16) * w + 
		(((GLuint) fraction) >> 16);
	lowWord = (dfraction & 0xffff) * w + (fraction & 0xffff);
	highWord += (((GLuint) lowWord) >> 16);
	bigs = ((GLuint) highWord) >> 15;
	littles = w - bigs;

	/* Second trivial accept attempt */
	xEnd = xStart + xBig*bigs + xLittle*littles;
	yEnd = yStart + yBig*bigs + yLittle*littles;
	if (xEnd >= clipX0 && xEnd < clipX1 && 
		yEnd >= clipY0 && yEnd < clipY1) {
	    return GL_FALSE;
	}
	w++;	/* Restore w */
    } else {
	xLittle = gc->line.options.xLittle;
	yLittle = gc->line.options.yLittle;
	fraction = gc->line.options.fraction;
	dfraction = gc->line.options.dfraction;
    }

    /*
    ** Note that we don't bother to try trivially rejecting this line.  After
    ** all, it has already been clipped, and the only way that it might
    ** possibly be trivially rejected is if it is a piece of a wide line that
    ** runs right along the edge of the window.
    */

    /*
    ** This sucks.  The line needs to be scissored.
    ** Well, it should only happen rarely, so we can afford
    ** to make it slow.  We achieve this by tediously stippling the line.
    ** (rather than clipping it, of course, which would be faster but harder).
    */
    sp = gc->polygon.shader.stipplePat;
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
		if (xStart < clipX0 || xStart >= clipX1 ||
			yStart < clipY0 || yStart >= clipY1) {
		    outMask &= ~bit;
		    failed++;
		}
	    } else failed++;

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		xStart += xBig;
		yStart += yBig;
	    } else {
		xStart += xLittle;
		yStart += yLittle;
	    }

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
** Create a stipple based upon the current line stipple for this line.
*/
GLboolean __glStippleLine(__GLcontext *gc)
{
    GLint failed, count, stippleRepeat;
    GLint stipple, currentBit, stipplePos, repeat;
    __GLstippleWord bit, outMask, *osp;
    __GLlineState *ls = &gc->state.line;
    GLint w;

    w = gc->polygon.shader.length;
    osp = gc->polygon.shader.stipplePat;
    repeat = gc->line.repeat;
    stippleRepeat = ls->stippleRepeat;
    stipplePos = gc->line.stipplePosition;
    currentBit = 1 << stipplePos;
    stipple = ls->stipple;
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
	    if ((stipple & currentBit) == 0) {
		/* Stippled fragment away */
		outMask &= ~bit;
		failed++;
	    }

	    if (++repeat >= stippleRepeat) {
		stipplePos = (stipplePos + 1) & 0xf;
		currentBit = 1 << stipplePos;
		repeat = 0;
	    }

#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*osp++ = outMask;
    }

    gc->line.repeat = repeat;
    gc->line.stipplePosition = stipplePos;

    if (failed == 0) {
	return GL_FALSE;
    } else if (failed != gc->polygon.shader.length) {
	return GL_TRUE;
    }
    gc->polygon.shader.done = GL_TRUE;
    return GL_TRUE;
}

/*
** Apply the stencil test to this line.
*/
GLboolean __glStencilTestLine(__GLcontext *gc)
{
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    __GLdrawableBuffer *dBuf = sb->buf.drawableBuf;
    __GLstencilCell *tft, *sfb, *fail, cell;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint dspLittle, dspBig;
    GLint count, failed;
    __GLstippleWord bit, outMask, *osp;
    GLuint smask;
    GLint w;

    w = gc->polygon.shader.length;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    sfb = __GL_STENCIL_ADDR(gc, sb, dBuf, (__GLstencilCell *),
			    gc->line.options.xStart, gc->line.options.yStart);
    dspLittle = xLittle + yLittle * dBuf->outerWidth;
    dspBig = xBig + yBig * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    tft = gc->stencilBuffer.testFuncTable;
    fail = gc->stencilBuffer.failOpTable;
    smask = gc->state.stencil.mask;
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
	    if (!tft[cell & smask]) {
		/* Test failed */
		outMask &= ~bit;
		sfb[0] = fail[cell];
		failed++;
	    }

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		sfb += dspBig;
	    } else {
		sfb += dspLittle;
	    }
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

/*
** Apply the stencil test to this stippled line.
*/
GLboolean __glStencilTestStippledLine(__GLcontext *gc)
{
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    __GLdrawableBuffer *dBuf = sb->buf.drawableBuf;
    __GLstencilCell *tft, *sfb, *fail, cell;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint count, failed;
    __GLstippleWord bit, inMask, outMask, *sp;
    GLint dspLittle, dspBig;
    GLuint smask;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    sfb = __GL_STENCIL_ADDR(gc, sb, dBuf, (__GLstencilCell *),
			    gc->line.options.xStart, gc->line.options.yStart);
    dspLittle = xLittle + yLittle * dBuf->outerWidth;
    dspBig = xBig + yBig * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
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

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		sfb += dspBig;
	    } else {
		sfb += dspLittle;
	    }
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

#if !(defined(__GL_USE_ASMCODE) && defined(mips))

GLboolean __glDepthTestLine(__GLcontext *gc)
{
    __GLdepthBuffer *db = &gc->depthBuffer;
    __GLdrawableBuffer *dBuf = db->buf.drawableBuf;
    __GLzValue zFrac, z, *zfb;
    GLint dzdx, dzdxBig;
    GLuint cnt;
    GLint numFracBits;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint dzpLittle, dzpBig;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * );
    __GLstippleWord bit, outMask, *osp;
    GLint w;

    w = gc->polygon.shader.length;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    zfb = __GL_DEPTH_ADDR(gc, db, dBuf, (__GLzValue *),
			  gc->line.options.xStart, gc->line.options.yStart);
    dzpLittle = (xLittle + yLittle * dBuf->outerWidth) * dBuf->elementSize;
    dzpBig = (xBig + yBig * dBuf->outerWidth) * dBuf->elementSize;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    numFracBits = __GL_DEPTH_DP(db)->numFracBits;
    cnt = __GL_DEPTH_DP(db)->cnt;
    testFunc = gc->procs.DTPixel;
    zFrac = gc->polygon.shader.frag.z;
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

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpBig);
	    } else {
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpLittle);
	    }
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

#endif /* !(__GL_USE_ASMCODE && mips) */

GLboolean __glDepthTestStippledLine(__GLcontext *gc)
{
    __GLdepthBuffer *db = &gc->depthBuffer;
    __GLdrawableBuffer *dBuf = db->buf.drawableBuf;
    __GLzValue zFrac, z, *zfb;
    GLint dzdx, dzdxBig;
    GLuint cnt;
    GLint numFracBits;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint dzpLittle, dzpBig;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * );
    __GLstippleWord bit, inMask, outMask, *sp;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    zfb = __GL_DEPTH_ADDR(gc, db, dBuf, (__GLzValue *),
			  gc->line.options.xStart, gc->line.options.yStart);
    dzpLittle = (xLittle + yLittle * dBuf->outerWidth) * dBuf->elementSize;
    dzpBig = (xBig + yBig * dBuf->outerWidth) * dBuf->elementSize;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    numFracBits = __GL_DEPTH_DP(db)->numFracBits;
    cnt = __GL_DEPTH_DP(db)->cnt;
    testFunc = gc->procs.DTPixel;
    zFrac = gc->polygon.shader.frag.z;
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

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpBig);
	    } else {
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpLittle);
	    }
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

GLboolean __glDepthTestStencilLine(__GLcontext *gc)
{
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    __GLdrawableBuffer *sBuf = sb->buf.drawableBuf;
    __GLdepthBuffer *db = &gc->depthBuffer;
    __GLdrawableBuffer *dBuf = db->buf.drawableBuf;
    __GLstencilCell *sfb, *zPassOp, *zFailOp;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint dzpLittle, dzpBig;
    GLint dspLittle, dspBig;
    __GLzValue zFrac, z, *zfb;
    GLint dzdx, dzdxBig;
    GLuint cnt;
    GLint numFracBits;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * );
    __GLstippleWord bit, outMask, *osp;
    GLint w;

    w = gc->polygon.shader.length;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    zfb = __GL_DEPTH_ADDR(gc, db, dBuf, (__GLzValue *),
			  gc->line.options.xStart, gc->line.options.yStart);
    dzpLittle = (xLittle + yLittle * dBuf->outerWidth) * dBuf->elementSize;
    dzpBig = (xBig + yBig * dBuf->outerWidth) * dBuf->elementSize;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    sfb = __GL_STENCIL_ADDR(gc, sb, sBuf, (__GLstencilCell *),
			    gc->line.options.xStart, gc->line.options.yStart);
    dspLittle = xLittle + yLittle * sBuf->outerWidth;
    dspBig = xBig + yBig * sBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    zFailOp = gc->stencilBuffer.depthFailOpTable;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    numFracBits = __GL_DEPTH_DP(db)->numFracBits;
    cnt = __GL_DEPTH_DP(db)->cnt;
    testFunc = gc->procs.DTPixel;
    zFrac = gc->polygon.shader.frag.z;
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

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpBig);
		sfb += dspBig;
	    } else {
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpLittle);
		sfb += dspLittle;
	    }
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

GLboolean __glDepthTestStencilStippledLine(__GLcontext *gc)
{
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    __GLdrawableBuffer *sBuf = sb->buf.drawableBuf;
    __GLdepthBuffer *db = &gc->depthBuffer;
    __GLdrawableBuffer *dBuf = db->buf.drawableBuf;
    __GLstencilCell *sfb, *zPassOp, *zFailOp;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint dzpLittle, dzpBig;
    GLint dspLittle, dspBig;
    __GLzValue zFrac, z, *zfb;
    GLint dzdx, dzdxBig;
    GLuint cnt;
    GLint numFracBits;
    GLint failed, count;
    GLboolean (*testFunc)( __GLzValue, __GLzValue * ) ;
    __GLstippleWord bit, inMask, outMask, *sp;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    zfb = __GL_DEPTH_ADDR(gc, db, dBuf, (__GLzValue *),
			  gc->line.options.xStart, gc->line.options.yStart);
    dzpLittle = (xLittle + yLittle * dBuf->outerWidth) * dBuf->elementSize;
    dzpBig = (xBig + yBig * dBuf->outerWidth) * dBuf->elementSize;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    sfb = __GL_STENCIL_ADDR(gc, sb, sBuf, (__GLstencilCell *),
			    gc->line.options.xStart, gc->line.options.yStart);
    dspLittle = xLittle + yLittle * sBuf->outerWidth;
    dspBig = xBig + yBig * sBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    numFracBits = __GL_DEPTH_DP(db)->numFracBits;
    cnt = __GL_DEPTH_DP(db)->cnt;
    testFunc = gc->procs.DTPixel;
    zFailOp = gc->stencilBuffer.depthFailOpTable;
    zPassOp = gc->stencilBuffer.depthPassOpTable;
    zFrac = gc->polygon.shader.frag.z;
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

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpBig);
		sfb += dspBig;
	    } else {
		zfb = (__GLzValue *) ((GLubyte *) zfb + dzpLittle);
		sfb += dspLittle;
	    }
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

GLboolean __glDepthPassLine(__GLcontext *gc)
{
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    __GLdrawableBuffer *dBuf = sb->buf.drawableBuf;
    __GLstencilCell *sfb, *zPassOp;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint dspLittle, dspBig;
    GLint w;

    w = gc->polygon.shader.length;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    sfb = __GL_STENCIL_ADDR(gc, sb, dBuf, (__GLstencilCell *),
			    gc->line.options.xStart, gc->line.options.yStart);
    dspLittle = xLittle + yLittle * dBuf->outerWidth;
    dspBig = xBig + yBig * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    zPassOp = gc->stencilBuffer.depthPassOpTable;
    while (--w >= 0) {
	sfb[0] = zPassOp[sfb[0]];
	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    sfb += dspBig;
	} else {
	    sfb += dspLittle;
	}
    }

    return GL_FALSE;
}

GLboolean __glDepthPassStippledLine(__GLcontext *gc)
{
    __GLstencilBuffer *db = &gc->stencilBuffer;
    __GLdrawableBuffer *dBuf = db->buf.drawableBuf;
    __GLstencilCell *sfb, *zPassOp;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    GLint dspLittle, dspBig;
    __GLstippleWord bit, inMask, *sp;
    GLint count;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    sfb = __GL_STENCIL_ADDR(gc, db, dBuf, (__GLstencilCell *),
			    gc->line.options.xStart, gc->line.options.yStart);
    dspLittle = xLittle + yLittle * dBuf->outerWidth;
    dspBig = xBig + yBig * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
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
	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		sfb += dspBig;
	    } else {
		sfb += dspLittle;
	    }
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

#ifdef NOTUSED
GLboolean __glDitherCILine(__GLcontext *gc)
{
    /* XXX - Dither the CI line */
    return GL_FALSE;
}

GLboolean __glDitherCIStippledLine(__GLcontext *gc)
{
    /* XXX - Dither the CI stippled line */
    return GL_FALSE;
}

GLboolean __glDitherRGBALine(__GLcontext *gc)
{
    /* XXX - Dither the RGBA line */
    return GL_FALSE;
}

GLboolean __glDitherRGBAStippledLine(__GLcontext *gc)
{
    /* XXX - Dither the RGBA stippled line */
    return GL_FALSE;
}
#endif

/*
** This store line proc lives just above cfb->store, so it does
** fetching, blending, dithering, logicOping, masking, and storing.
**
** It uses the colorBuffer pointed to by gc->polygon.shader.cfb.
*/
GLboolean __glStoreLine(__GLcontext *gc)
{
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    __GLfragment frag;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint len;

    len = gc->polygon.shader.length;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;
    store = cfb->store;
    frag.x = gc->line.options.xStart;
    frag.y = gc->line.options.yStart;

    while (--len >= 0) {
	frag.color[__GL_PRIMARY_COLOR] = *cp++;
	(*store)(cfb, &frag);

	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    frag.x += xBig;
	    frag.y += yBig;
	} else {
	    frag.x += xLittle;
	    frag.y += yLittle;
	}
    }

    return GL_FALSE;
}

/*
** This store line proc lives just above cfb->store, so it does
** fetching, blending, dithering, logicOping, masking, and storing.
**
** It uses the colorBuffer pointed to by gc->polygon.shader.cfb.
*/
GLboolean __glStoreStippledLine(__GLcontext *gc)
{
    GLint x, y, xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    __GLfragment frag;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLstippleWord inMask, bit, *sp;
    GLint count;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint len;

    len = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;
    store = cfb->store;
    x = gc->line.options.xStart;
    y = gc->line.options.yStart;

    while (len) {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		frag.x = x;
		frag.y = y;
		frag.color[__GL_PRIMARY_COLOR] = *cp;
		(*store)(cfb, &frag);
	    }

	    cp++;
	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		x += xBig;
		y += yBig;
	    } else {
		x += xLittle;
		y += yLittle;
	    }
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

GLboolean __glAntiAliasLine(__GLcontext *gc)
{
    __GLfloat length;	/* Dist along length */
    __GLfloat width;	/* Dist along width */
    GLint fraction, dfraction;
    __GLfloat dlLittle, dlBig;
    __GLfloat ddLittle, ddBig;
    __GLcolor *cp;
    __GLfloat coverage;
    __GLfloat lineWidth;
    __GLfloat lineLength;
    GLint failed, count;
    __GLstippleWord bit, outMask, *osp;
    GLint w;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    w = gc->polygon.shader.length;

    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    cp = gc->polygon.shader.colors;

    dlLittle = gc->line.options.dlLittle;
    dlBig = gc->line.options.dlBig;
    ddLittle = gc->line.options.ddLittle;
    ddBig = gc->line.options.ddBig;

    length = gc->line.options.plength;
    width = gc->line.options.pwidth;
    lineLength = gc->line.options.realLength - __glHalf;
    lineWidth = __glHalf * gc->state.line.smoothWidth - __glHalf;


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
	    /* Coverage for sides */
	    if (width > lineWidth) {
		coverage = lineWidth - width + __glOne;
		if (coverage < __glZero) {
		    coverage = __glZero;
		    goto coverageZero;
		}
	    } else if (width < -lineWidth) {
		coverage = width + lineWidth + __glOne;
		if (coverage < __glZero) {
		    coverage = __glZero;
		    goto coverageZero;
		}
	    } else {
		coverage = __glOne;
	    }

	    /* Coverage for start, end */
	    if (length < __glHalf) {
		coverage *= length + __glHalf;
		if (coverage < __glZero) {
		    coverage = __glZero;
		    goto coverageZero;
		}
	    } else if (length > lineLength) {
		coverage *= lineLength - length + __glOne;
		if (coverage < __glZero) {
		    coverage = __glZero;
		    goto coverageZero;
		}
	    } 

	    /* Coverage for internal stipples */
	    if ( modeFlags & __GL_SHADE_LINE_STIPPLE ) {
		__GLfloat stippleOffset;
		GLint lowStip, highStip;
		GLint lowBit, highBit;
		GLint lowVal, highVal;
		__GLfloat percent;

		/* Minor correction */
		if (length > __glHalf) {
		    stippleOffset = gc->line.options.stippleOffset + length;
		} else {
		    stippleOffset = gc->line.options.stippleOffset + __glHalf;
		}
		lowStip = (GLint)__GL_FLOORF(stippleOffset);
		highStip = lowStip + 1;

		/* percent is the percent of highStip that will be used */
		percent = stippleOffset - lowStip;

		lowBit = (GLint) (lowStip * 
			gc->line.options.oneOverStippleRepeat) & 0xf;
		highBit = (GLint) (highStip * 
			gc->line.options.oneOverStippleRepeat) & 0xf;

		if (gc->state.line.stipple & (1<<lowBit)) {
		    lowVal = 1;
		} else {
		    lowVal = 0;
		}

		if (gc->state.line.stipple & (1<<highBit)) {
		    highVal = 1;
		} else {
		    highVal = 0;
		}

		coverage *= lowVal * (__glOne - percent) +
			highVal * percent;
	    }

	    if (coverage == __glZero) {
coverageZero:;
		outMask &= ~bit;
		failed++;
	    } else {
		if (gc->modes.colorIndexMode) {
		    cp->r = __glBuildAntiAliasIndex(cp->r, coverage);
		} else {
		    cp->a *= coverage;
		}
	    }
	    cp++;

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		length += dlBig;
		width += ddBig;
	    } else {
		length += dlLittle;
		width += ddLittle;
	    }

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

GLboolean __glAntiAliasStippledLine(__GLcontext *gc)
{
    __GLfloat length;	/* Dist along length */
    __GLfloat width;	/* Dist along width */
    GLint fraction, dfraction;
    __GLfloat dlLittle, dlBig;
    __GLfloat ddLittle, ddBig;
    __GLcolor *cp;
    __GLfloat coverage;
    __GLfloat lineWidth;
    __GLfloat lineLength;
    GLint failed, count;
    __GLstippleWord bit, outMask, inMask, *sp;
    GLint w;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    cp = gc->polygon.shader.colors;

    dlLittle = gc->line.options.dlLittle;
    dlBig = gc->line.options.dlBig;
    ddLittle = gc->line.options.ddLittle;
    ddBig = gc->line.options.ddBig;

    length = gc->line.options.plength;
    width = gc->line.options.pwidth;
    lineLength = gc->line.options.realLength - __glHalf;
    lineWidth = __glHalf * gc->state.line.smoothWidth - __glHalf;

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
		/* Coverage for sides */
		if (width > lineWidth) {
		    coverage = lineWidth - width + __glOne;
		    if (coverage < __glZero) {
			coverage = __glZero;
			goto coverageZero;
		    }
		} else if (width < -lineWidth) {
		    coverage = width + lineWidth + __glOne;
		    if (coverage < __glZero) {
			coverage = __glZero;
			goto coverageZero;
		    }
		} else {
		    coverage = __glOne;
		}

		/* Coverage for start, end */
		if (length < __glHalf) {
		    coverage *= length + __glHalf;
		    if (coverage < __glZero) {
			coverage = __glZero;
			goto coverageZero;
		    }
		} else if (length > lineLength) {
		    coverage *= lineLength - length + __glOne;
		    if (coverage < __glZero) {
			coverage = __glZero;
			goto coverageZero;
		    }
		} 

		/* Coverage for internal stipples */
		if (modeFlags & __GL_SHADE_LINE_STIPPLE) {
		    __GLfloat stippleOffset;
		    GLint lowStip, highStip;
		    GLint lowBit, highBit;
		    GLint lowVal, highVal;
		    __GLfloat percent;

		    /* Minor correction */
		    if (length > __glHalf) {
			stippleOffset = gc->line.options.stippleOffset + length;
		    } else {
			stippleOffset = gc->line.options.stippleOffset + __glHalf;
		    }
		    lowStip = (GLint)__GL_FLOORF(stippleOffset);
		    highStip = lowStip + 1;

		    /* percent is the percent of highStip that will be used */
		    percent = stippleOffset - lowStip;

		    lowBit = (GLint) (lowStip * 
			    gc->line.options.oneOverStippleRepeat) & 0xf;
		    highBit = (GLint) (highStip * 
			    gc->line.options.oneOverStippleRepeat) & 0xf;

		    if (gc->state.line.stipple & (1<<lowBit)) {
			lowVal = 1;
		    } else {
			lowVal = 0;
		    }

		    if (gc->state.line.stipple & (1<<highBit)) {
			highVal = 1;
		    } else {
			highVal = 0;
		    }

		    coverage *= lowVal * (__glOne - percent) +
			    highVal * percent;
		}

		if (coverage == __glZero) {
coverageZero:;
		    outMask &= ~bit;
		    failed++;
		} else {
		    if (gc->modes.colorIndexMode) {
			cp->r = __glBuildAntiAliasIndex(cp->r, coverage);
		    } else {
			cp->a *= coverage;
		    }
		}
	    } else failed++;
	    cp++;

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		length += dlBig;
		width += ddBig;
	    } else {
		length += dlLittle;
		width += ddLittle;
	    }

#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
	*sp++ = outMask & inMask;
    }

    if (failed == gc->polygon.shader.length) {
	return GL_TRUE;
    }
    return GL_FALSE;
}
