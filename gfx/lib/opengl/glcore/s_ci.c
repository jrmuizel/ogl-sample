/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.0 (the "License"), the contents of this
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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_ci.c#9 $
*/
#include "context.h"
#include "global.h"
#include "glmath.h"
#include "fmacros.h"
#if defined(__GL_FR_RAST)
#include "fr_raster.h"
#endif /* __GL_FR_RAST */
#if defined(__GL_USE_ASMCODE) && defined(i386)
#include "i386.h"
#endif /* __GL_USE_ASMCODE && i386 */

#if 0
/* No Dither, No blend, No Write, No Nothing */
static void Store_NOT(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
}
#endif

/* 8 bit generic Store */
static void Store_8(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *dBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLubyte *fp;
    __GLfloat inc;
    GLubyte fbcolor, result;
    GLint ix;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *), frag->x, frag->y);
    if (modeFlags & __GL_SHADE_DITHER) {
        ix = __GL_DITHER_INDEX(frag->x, frag->y);
        inc = ((__glDitherTable[ix] << 1) + 1) / 
            (__GLfloat) (2 * __GL_DITHER_PRECISION);
    } else {
        inc = __glHalf;
    }
    result = (frag->color[__GL_PRIMARY_COLOR].r + inc); 
    fbcolor = *fp;
    if (modeFlags & __GL_SHADE_LOGICOP) {
        switch(gc->state.raster.logicOp) {
          case GL_CLEAR:         result = 0; break;
          case GL_AND:           result = result & fbcolor; break;
          case GL_AND_REVERSE:   result = result & (~fbcolor); break;
          case GL_COPY:          result = result; break;
          case GL_AND_INVERTED:  result = (~result) & fbcolor; break;
          case GL_NOOP:          result = fbcolor; break;
          case GL_XOR:           result = result ^ fbcolor; break;
          case GL_OR:            result = result | fbcolor; break;
          case GL_NOR:           result = ~(result | fbcolor); break;
          case GL_EQUIV:         result = ~(result ^ fbcolor); break;
          case GL_INVERT:        result = ~fbcolor; break;
          case GL_OR_REVERSE:    result = result | (~fbcolor); break;
          case GL_COPY_INVERTED: result = ~result; break;
          case GL_OR_INVERTED:   result = (~result) | fbcolor; break;
          case GL_NAND:          result = ~(result & fbcolor); break;
          case GL_SET:           result = ~0; break;
        }
    }

    *fp = (fbcolor & cfb->destMask) | (result & cfb->sourceMask);
}

/* 16 bit generic Store */
static void Store_16(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *dBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLushort *fp;
    __GLfloat inc;
    GLushort fbcolor, result;
    GLint ix;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *), frag->x, frag->y);
    if (modeFlags & __GL_SHADE_DITHER) {
        ix = __GL_DITHER_INDEX(frag->x, frag->y);
        inc = ((__glDitherTable[ix] << 1) + 1) / 
            (__GLfloat) (2 * __GL_DITHER_PRECISION);
    } else {
        inc = __glHalf;
    }
    result = (frag->color[__GL_PRIMARY_COLOR].r + inc); 
    fbcolor = *fp;
    if (modeFlags & __GL_SHADE_LOGICOP) {
        switch(gc->state.raster.logicOp) {
          case GL_CLEAR:         result = 0; break;
          case GL_AND:           result = result & fbcolor; break;
          case GL_AND_REVERSE:   result = result & (~fbcolor); break;
          case GL_COPY:          result = result; break;
          case GL_AND_INVERTED:  result = (~result) & fbcolor; break;
          case GL_NOOP:          result = fbcolor; break;
          case GL_XOR:           result = result ^ fbcolor; break;
          case GL_OR:            result = result | fbcolor; break;
          case GL_NOR:           result = ~(result | fbcolor); break;
          case GL_EQUIV:         result = ~(result ^ fbcolor); break;
          case GL_INVERT:        result = ~fbcolor; break;
          case GL_OR_REVERSE:    result = result | (~fbcolor); break;
          case GL_COPY_INVERTED: result = ~result; break;
          case GL_OR_INVERTED:   result = (~result) | fbcolor; break;
          case GL_NAND:          result = ~(result & fbcolor); break;
          case GL_SET:           result = ~0; break;
        }
    }

    *fp = (fbcolor & cfb->destMask) | (result & cfb->sourceMask);
}

/* 32 bit generic Store */
static void Store_32(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *dBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint *fp;
    __GLfloat inc;
    GLuint fbcolor, result;
    GLint ix;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLuint *), frag->x, frag->y);
    if (modeFlags & __GL_SHADE_DITHER) {
        ix = __GL_DITHER_INDEX(frag->x, frag->y);
        inc = ((__glDitherTable[ix] << 1) + 1) / 
            (__GLfloat) (2 * __GL_DITHER_PRECISION);
    } else {
        inc = __glHalf;
    }
    result = (frag->color[__GL_PRIMARY_COLOR].r + inc); 
    fbcolor = *fp;
    if (modeFlags & __GL_SHADE_LOGICOP) {
        switch(gc->state.raster.logicOp) {
          case GL_CLEAR:         result = 0; break;
          case GL_AND:           result = result & fbcolor; break;
          case GL_AND_REVERSE:   result = result & (~fbcolor); break;
          case GL_COPY:          result = result; break;
          case GL_AND_INVERTED:  result = (~result) & fbcolor; break;
          case GL_NOOP:          result = fbcolor; break;
          case GL_XOR:           result = result ^ fbcolor; break;
          case GL_OR:            result = result | fbcolor; break;
          case GL_NOR:           result = ~(result | fbcolor); break;
          case GL_EQUIV:         result = ~(result ^ fbcolor); break;
          case GL_INVERT:        result = ~fbcolor; break;
          case GL_OR_REVERSE:    result = result | (~fbcolor); break;
          case GL_COPY_INVERTED: result = ~result; break;
          case GL_OR_INVERTED:   result = (~result) | fbcolor; break;
          case GL_NAND:          result = ~(result & fbcolor); break;
          case GL_SET:           result = ~0; break;
        }
    }

    *fp = (fbcolor & cfb->destMask) | (result & cfb->sourceMask);
}

/*
** Note: The Fetch routines below are called for both draw and read buffers.
** It's the responsibility of the caller to lock the appropriate buffers
*/
static void Fetch_8(__GLcolorBuffer *cfb, GLint x, GLint y,
                    __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *dBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLubyte *fp;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = 0;
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *), x, y);
    result->r = *fp;
}

static void Fetch_16(__GLcolorBuffer *cfb, GLint x, GLint y,
                     __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *dBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLushort *fp;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = 0;
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *), x, y);
    result->r = *fp;
}

static void Fetch_32(__GLcolorBuffer *cfb, GLint x, GLint y,
                     __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *dBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint *fp;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = 0;
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLuint *), x, y);
    result->r = *fp;
}

static void ClearRect(__GLcolorBuffer *cfb,
			GLint x, GLint y, GLint x1, GLint y1)
{
    __GLcontext *gc = cfb->buf.gc;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLint i, j;
    __GLfragment frag;

    /* Turn off logic op so store proc doesn't do it */
    gc->polygon.shader.modeFlags = modeFlags & ~__GL_SHADE_LOGICOP;

    frag.color[__GL_PRIMARY_COLOR].r = gc->state.raster.clearIndex;

    for (j = y; j < y1; j++) {
        frag.y = j;
        for (i = x; i < x1; i++) {
            frag.x = i;
                
            (*cfb->store)(cfb, &frag);
        }
    }

    gc->polygon.shader.modeFlags = modeFlags;
}

static void ClearRegionRects(__GLcolorBuffer *cfb,
	void (*clearRect)(__GLcolorBuffer *, GLint, GLint, GLint, GLint))
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawablePrivate *dp = gc->drawablePrivate;
    __GLregionRect *rect;
    int i, n;

    __GL_LOCK_DP(dp, gc);

    n = dp->ownershipRegion.numRects;
    for (i=0, rect = &dp->ownershipRegion.rects[0]; i<n; ++i, ++rect) {
#if defined(__GL_ALIGNED_BUFFERS)
	(*clearRect)(cfb,
		     rect->x0 + gc->constants.viewportXAdjustOffset,
		     rect->y0 + gc->constants.viewportYAdjustOffset,
		     rect->x1 + gc->constants.viewportXAdjustOffset,
		     rect->y1 + gc->constants.viewportYAdjustOffset);
#else /* __GL_ALIGNED_BUFFERS */
	(*clearRect)(cfb,
		     rect->x0 + gc->constants.viewportXAdjust,
		     rect->y0 + gc->constants.viewportYAdjust,
		     rect->x1 + gc->constants.viewportXAdjust,
		     rect->y1 + gc->constants.viewportYAdjust);
#endif /* __GL_ALIGNED_BUFFERS */
    }

    __GL_UNLOCK_DP(dp);
}

static void Clear(__GLcolorBuffer *cfb)
{
    __GLcontext *gc = cfb->buf.gc;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    __GL_LOCK_DP(gc->drawablePrivate, gc);
    __GL_LOCK_BUFFER(gc, &cfb->buf, cfb->buf.drawableBuf);

    if (modeFlags & __GL_SHADE_OWNERSHIP_TEST) {
	ClearRegionRects(cfb, cfb->clearRect);
    } else {
	(*cfb->clearRect)(cfb,
		  gc->transform.clipX0,
		  gc->transform.clipY0,
		  gc->transform.clipX1,
		  gc->transform.clipY1);
    }

    __GL_UNLOCK_BUFFER(gc, &cfb->buf, cfb->buf.drawableBuf);
    __GL_UNLOCK_DP(gc->drawablePrivate);
}

static void ClearFill(__GLcolorBuffer *cfb)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *dBuf= cfb->buf.drawableBuf;
    __GLdrawablePrivate *dp = gc->drawablePrivate;
    GLuint clearIndex = (GLuint) gc->state.raster.clearIndex;
    GLint x, y, width, height;

    __GL_LOCK_DP(dp, gc);

    width = gc->transform.clipX1 - gc->transform.clipX0;
    height = gc->transform.clipY1 - gc->transform.clipY0;

    if ((width <= 0) || (height <= 0)) {
	__GL_UNLOCK_DP(dp);
	return;
    }

    x = gc->transform.clipX0 - gc->constants.viewportXAdjust;
    y = gc->transform.clipY0 - gc->constants.viewportYAdjust;

    assert (dBuf->fill);
    (*dBuf->fill)(dBuf, dp, clearIndex, x, y, width, height);

    __GL_UNLOCK_DP(dp);
}


static GLboolean StoreSpan(__GLcontext *gc)
{
    int x, x1;
    int i;
    __GLfragment frag;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    GLint w;

    w = gc->polygon.shader.length;

    frag.y = gc->polygon.shader.frag.y;
    x = gc->polygon.shader.frag.x;
    x1 = gc->polygon.shader.frag.x + w;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;

    for (i = x; i < x1; i++) {
        frag.x = i;
        frag.color[__GL_PRIMARY_COLOR].r = cp->r;
        cp++;

        (*cfb->store)(cfb, &frag);
    }

    return GL_FALSE;
}

static GLboolean StoreStippledSpan(__GLcontext *gc)
{
    int x;
    __GLfragment frag;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLstippleWord inMask, bit, *sp;
    GLint count;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    frag.y = gc->polygon.shader.frag.y;
    x = gc->polygon.shader.frag.x;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;

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
                frag.x = x;
                frag.color[__GL_PRIMARY_COLOR].r = cp->r;

                (*cfb->store)(cfb, &frag);
            }
            x++;
            cp++;
#if defined(__GL_STIPPLE_MSB)
            bit >>= 1;
#else /* __GL_STIPPLE_MSB */
            bit <<= 1;
#endif /* __GL_STIPPLE_MSB */
        }
    }

    return GL_FALSE;
}

static void Pick(__GLcontext *gc, __GLcolorBuffer *cfb)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    if (gc->state.raster.drawBuffer == GL_NONE) {
        cfb->sourceMask = 0;
        cfb->destMask = cfb->redMax;
    } else {
        cfb->sourceMask = gc->state.raster.writeMask & cfb->redMax;
        cfb->destMask = cfb->redMax & ~gc->state.raster.writeMask;
    }

#if defined(__GL_CORE_CLEARS_BUFFERS)
    cfb->clear = Clear;
    cfb->clearRect = ClearRect;
#if defined(__GL_USE_ASMCODE) && defined(i386)
    /* pick a clear routine */
    if ((cfb->sourceMask == (GLuint) cfb->redMax) &&
	(cfb->buf.drawableBuf->elementSize == 1)) {
	if (modeFlags & __GL_SHADE_DITHER) {
	    cfb->clearRect = __glI386ClearRectDitherCI8;
	} else {
	    cfb->clearRect = __glI386ClearRectCI8;
	}
    }
#endif /* __GL_USE_ASMCODE && i386 */
#else /* __GL_CORE_CLEARS_BUFFERS */
    cfb->clear = ClearFill;
    cfb->clearRect = NULL;
#endif /* __GL_CORE_CLEARS_BUFFERS */
}

void __glInitCI(__GLcolorBuffer *cfb, __GLcontext *gc )
{
    GLint indexBits;

    __glInitBuffer( &cfb->buf, gc );

    cfb->readSpan = __glReadSpan;
    cfb->returnSpan = __glReturnSpan;

    cfb->clear = Clear;
    cfb->pick = Pick;
    cfb->fetchSpan = __glFetchSpan;
    cfb->fetchStippledSpan = __glFetchSpan;
    cfb->storeSpan = StoreSpan;
    cfb->storeStippledSpan = StoreStippledSpan;

    cfb->redScale = 1.0;
    cfb->greenScale = 1.0;
    cfb->blueScale = 1.0;
    cfb->alphaScale = 1.0;

    indexBits = gc->modes.indexBits;
    if (indexBits < 32) {
        cfb->sourceMask = (1 << indexBits) - 1;
    } else {
        cfb->sourceMask = 0xffffffff;
    }
    cfb->redMax = cfb->sourceMask;

    if (indexBits > 16) {       
	cfb->store = Store_32;
        cfb->fetch = cfb->readColor = Fetch_32;
    } else if (indexBits > 8) {
	cfb->store = Store_16;
        cfb->fetch = cfb->readColor = Fetch_16;
    } else {
	cfb->store = Store_8;
        cfb->fetch = cfb->readColor = Fetch_8;
    }

#if defined(__GL_FR_RAST)
    __glFRPickFBType(gc, cfb);
#endif
}
