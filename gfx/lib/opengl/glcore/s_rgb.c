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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_rgb.c#14 $
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

static GLuint ColorToPixel(__GLcontext *gc, __GLcolorBuffer *cfb,
			   GLint r, GLint g, GLint b)
{
    GLuint pixel;

    if (r < 0) r = 0;
    else if (r > cfb->redMax) r = cfb->redMax;
    if (g < 0) g = 0;
    else if (g > cfb->greenMax) g = cfb->greenMax;
    if (b < 0) b = 0;
    else if (b > cfb->blueMax) b = cfb->blueMax;

    pixel = (r << cfb->redShift) | (g << cfb->greenShift) |
	(b << cfb->blueShift);
    return pixel;
}

/* 8 bit generic Store */
static void Store_8(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLubyte *fp;
    __GLcolor blendColor;
    const __GLcolor *color;
    __GLfloat inc;
    GLubyte fbcolor, result;
    GLint ix;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLubyte *), frag->x, frag->y);
    if (modeFlags & __GL_SHADE_DITHER) {
	ix = __GL_DITHER_INDEX(frag->x, frag->y);
	inc = ((__glDitherTable[ix] << 1) + 1) / 
	    (__GLfloat) (2 * __GL_DITHER_PRECISION);
    } else {
	inc = __glHalf;
    }
    if (modeFlags & __GL_SHADE_BLEND) {
	color = &blendColor;
	(*gc->procs.blend)(gc, cfb, frag, &blendColor);
    } else {
	color = &(frag->color[__GL_PRIMARY_COLOR]);
    }
    result = (GLubyte) ColorToPixel(gc, cfb,
				    color->r + inc,
				    color->g + inc,
				    color->b + inc);

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
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLushort *fp;
    __GLcolor blendColor;
    const __GLcolor *color;
    __GLfloat inc;
    GLushort fbcolor, result;
    GLint ix;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLushort *), frag->x, frag->y);
    if (modeFlags & __GL_SHADE_DITHER) {
	ix = __GL_DITHER_INDEX(frag->x, frag->y);
	inc = ((__glDitherTable[ix] << 1) + 1) / 
	    (__GLfloat) (2 * __GL_DITHER_PRECISION);
    } else {
	inc = __glHalf;
    }
    if (modeFlags & __GL_SHADE_BLEND) {
	color = &blendColor;
	(*gc->procs.blend)(gc, cfb, frag, &blendColor);
    } else {
	color = &(frag->color[__GL_PRIMARY_COLOR]);
    }
    result = (GLushort) ColorToPixel(gc, cfb,
				     color->r + inc,
				     color->g + inc,
				     color->b + inc);

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

/* 24 bit generic Store */
static void Store_24(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLubyte *fp;
    __GLcolor blendColor;
    const __GLcolor *color;
    __GLfloat inc;
    GLuint fbcolor, result;
    GLint ix;
    GLuint stride;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    stride = ((cfb->buf.drawableBuf->outerWidth * 3) + 3) & ~3;
    fp = ((GLubyte *) cfb->buf.drawableBuf->base) +
	(frag->y - gc->constants.viewportYAdjust) * stride +
	(frag->x - gc->constants.viewportXAdjust) * 3;

    if (modeFlags & __GL_SHADE_DITHER) {
	ix = __GL_DITHER_INDEX(frag->x, frag->y);
	inc = ((__glDitherTable[ix] << 1) + 1) / 
	    (__GLfloat) (2 * __GL_DITHER_PRECISION);
    } else {
	inc = __glHalf;
    }
    if (modeFlags & __GL_SHADE_BLEND) {
	color = &blendColor;
	(*gc->procs.blend)(gc, cfb, frag, &blendColor);
    } else {
	color = &(frag->color[__GL_PRIMARY_COLOR]);
    }
    result = ColorToPixel(gc, cfb,
			  color->r + inc,
			  color->g + inc,
			  color->b + inc);

    fbcolor = ColorToPixel(gc, cfb, fp[2], fp[1], fp[0]);
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

    result = (fbcolor & cfb->destMask) | (result & cfb->sourceMask);
    fp[2] = (GLubyte) (result >> cfb->redShift);
    fp[1] = (GLubyte) (result >> cfb->greenShift);
    fp[0] = (GLubyte) (result >> cfb->blueShift);
}

/* 32 bit generic Store with alpha */
static void Store_32A(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint *fp;
    __GLcolor blendColor;
    const __GLcolor *color;
    GLuint fbcolor, result;
    GLfloat  a;

    /*
    ** No dithering for 32 bit pixmaps.  It would probably be a waste
    ** of time.
    */

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLuint *), frag->x, frag->y);
    if (modeFlags & __GL_SHADE_BLEND) {
	color = &blendColor;
	(*gc->procs.blend)(gc, cfb, frag, &blendColor);
    } else {
	color = &(frag->color[__GL_PRIMARY_COLOR]);
    }

    result = (GLuint) ColorToPixel( gc, cfb,
				    color->r + __glHalf,
				    color->g + __glHalf,
				    color->b + __glHalf );
    a = color->a + __glHalf;

    if ( a < __glZero )
	a = __glZero;
    else if ( a > cfb->iAlphaScale )
	a = cfb->iAlphaScale;
    
    result |= ((GLuint) a) << cfb->alphaShift;

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

/* 32 bit generic Store without alpha */
static void Store_32(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint *fp;
    __GLcolor blendColor;
    const __GLcolor *color;
    GLuint fbcolor, result;

    /*
    ** No dithering for 32 bit pixmaps.  It would probably be a waste
    ** of time.
    */

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLuint *), frag->x, frag->y);
    if (modeFlags & __GL_SHADE_BLEND) {
	color = &blendColor;
	(*gc->procs.blend)(gc, cfb, frag, &blendColor);
    } else {
	color = &(frag->color[__GL_PRIMARY_COLOR]);
    }

    result = (GLuint) ColorToPixel( gc, cfb,
				    color->r + __glHalf,
				    color->g + __glHalf,
				    color->b + __glHalf );

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
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLubyte *fp;
    GLubyte value;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = result->g = result->b = result->a = 0;
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLubyte *), x, y);
    value = *fp;
    result->r = (value & gc->modes.redMask) >> cfb->redShift;
    result->g = (value & gc->modes.greenMask) >> cfb->greenShift;
    result->b = (value & gc->modes.blueMask) >> cfb->blueShift;
    result->a = cfb->alphaScale;
}

static void Fetch_16(__GLcolorBuffer *cfb, GLint x, GLint y,
		     __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLushort *fp;
    GLushort value;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = result->g = result->b = result->a = 0;
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLushort *), x, y);
    value = *fp;
    result->r = (value & gc->modes.redMask) >> cfb->redShift;
    result->g = (value & gc->modes.greenMask) >> cfb->greenShift;
    result->b = (value & gc->modes.blueMask) >> cfb->blueShift;
    result->a = cfb->alphaScale;
}

static void Fetch_24(__GLcolorBuffer *cfb, GLint x, GLint y,
		     __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLubyte *fp;
    GLuint stride;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = result->g = result->b = result->a = 0;
	return;
    }

    stride = ((drawableBuf->outerWidth * 3) + 3) & ~3;
    fp = ((GLubyte *) drawableBuf->base) +
	(y - gc->constants.viewportYAdjust) * stride +
	(x - gc->constants.viewportXAdjust) * 3;

    result->r = fp[2];
    result->g = fp[1];
    result->b = fp[0];
    result->a = cfb->alphaScale;
}

/* 32 bit generic fetch with alpha */
static void Fetch_32A(__GLcolorBuffer *cfb, GLint x, GLint y,
		      __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint *fp;
    GLuint value;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = result->g = result->b = result->a = 0;
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLuint *), x, y);
    value = *fp;
    result->r = (value & gc->modes.redMask) >> cfb->redShift;
    result->g = (value & gc->modes.greenMask) >> cfb->greenShift;
    result->b = (value & gc->modes.blueMask) >> cfb->blueShift;
    result->a = (value & gc->modes.alphaMask) >> cfb->alphaShift;
}

/* 32 bit generic fetch without alpha */
static void Fetch_32(__GLcolorBuffer *cfb, GLint x, GLint y,
		     __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint *fp;
    GLuint value;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		!__glTestOwnership(gc, x, y)) {
	result->r = result->g = result->b = result->a = 0;
	return;
    }

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLuint *), x, y);
    value = *fp;
    result->r = (value & gc->modes.redMask) >> cfb->redShift;
    result->g = (value & gc->modes.greenMask) >> cfb->greenShift;
    result->b = (value & gc->modes.blueMask) >> cfb->blueShift;
    result->a = cfb->alphaScale;
}

static void ClearRect(__GLcolorBuffer *cfb,
		      GLint x, GLint y, GLint x1, GLint y1)
{
    __GLcontext *gc = cfb->buf.gc;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLint i, j;
    __GLfragment frag;

    /* Turn off blending and logicop so store proc doesn't do it */
    gc->polygon.shader.modeFlags =
	modeFlags & ~(__GL_SHADE_BLEND|__GL_SHADE_LOGICOP);

    frag.color[__GL_PRIMARY_COLOR].r = gc->state.raster.clear.r *
	gc->frontBuffer.redScale;
    frag.color[__GL_PRIMARY_COLOR].g = gc->state.raster.clear.g *
	gc->frontBuffer.greenScale;
    frag.color[__GL_PRIMARY_COLOR].b = gc->state.raster.clear.b *
	gc->frontBuffer.blueScale;
    frag.color[__GL_PRIMARY_COLOR].a = gc->state.raster.clear.a *
	gc->frontBuffer.alphaScale;

    for (j = y; j < y1; j++) {
	frag.y = j;

	for (i = x; i < x1; i++) {
	    frag.x = i;
		
	    (*cfb->store)(cfb, &frag);
	}
    }

    gc->polygon.shader.modeFlags = modeFlags;
}

static void ClearRect_24(__GLcolorBuffer *cfb,
			 GLint x0, GLint y, GLint x1, GLint y1)
{
    __GLcontext *gc = cfb->buf.gc;
    GLubyte *fp, *rfp;
    __GLfloat fr, fg, fb;
    unsigned int r, g, b;
    unsigned int color;
    GLuint pack0, pack1, pack2;
    int      stride;
    int      width, height;
    int      dword_width, pre_step_pixels, fixup_pixels;
    
    fr = gc->state.raster.clear.r * gc->frontBuffer.redScale;
    fg = gc->state.raster.clear.g * gc->frontBuffer.greenScale;
    fb = gc->state.raster.clear.b * gc->frontBuffer.blueScale;
                
    Float2Int(fr, r);
    Float2Int(fg, g);
    Float2Int(fb, b);

    color = (r << cfb->redShift) |
	    (g << cfb->greenShift) |
	    (b << cfb->blueShift);

    width  = x1 - x0;
    height = y1 - y;

    /*
    ** Setup packed uint32's
    */
#if defined(__GL_LITTLE_ENDIAN)
    pack0 = (color << 24) | color;
    pack1 = (color << 16) | (color >> 8);
    pack2 = (color << 8) | (color >> 16);
#else /* __GL_LITTLE_ENDIAN */
    pack0 = (color << 8) | (color >> 16);
    pack1 = (color << 16) | (color >> 8);
    pack2 = (color << 24) | color;
#endif /* __GL_LITTLE_ENDIAN */

    stride = ((cfb->buf.drawableBuf->outerWidth * 3) + 3) & ~3;
    fp = ((GLubyte *) cfb->buf.drawableBuf->base) +
	(y - gc->constants.viewportYAdjust) * stride +
	(x0 - gc->constants.viewportXAdjust) * 3;

    while (height) {
	rfp = fp;

	pre_step_pixels = (((unsigned) rfp) & 3);
	if (pre_step_pixels > width) {
	    pre_step_pixels = width;
	    dword_width = 0;
	    fixup_pixels = 0;
	} else {
	    dword_width     = ( width - pre_step_pixels ) >> 2;
	    fixup_pixels    = width - (dword_width*4 + pre_step_pixels);
	}

	while (pre_step_pixels) {
	    rfp[0] = b;
	    rfp[1] = g;
	    rfp[2] = r;
	    rfp += 3;
	    pre_step_pixels--;
	}

	while (dword_width) {
	    ((GLuint *)rfp)[0] = pack0;
	    ((GLuint *)rfp)[1] = pack1;
	    ((GLuint *)rfp)[2] = pack2;

	    rfp += 12;
	    dword_width--;
	}

	while (fixup_pixels) {
	    rfp[0] = b;
	    rfp[1] = g;
	    rfp[2] = r;

	    rfp += 3;
	    fixup_pixels--;
	}

	fp += stride;
	height--;
    }
}

static void ClearRect_16_Dither(__GLcolorBuffer *cfb,
					GLint x0, GLint y, GLint x1, GLint y1)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    GLint x;
    GLushort *fp, *rfp;
    __GLfloat fr, fg, fb;
    
    fr = gc->state.raster.clear.r * gc->frontBuffer.redScale;
    fg = gc->state.raster.clear.g * gc->frontBuffer.greenScale;
    fb = gc->state.raster.clear.b * gc->frontBuffer.blueScale;
    
    rfp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLushort *), x0, y);
    
    for (; y < y1; y++, rfp += cfb->buf.drawableBuf->outerWidth) {
        __GLfloat *dither = __glDitherTableFloat + ((y<<2) & 12);
        
        for (x = x0, fp = rfp; x < x1; x++, fp++) {
            __GLfloat ftmp, inc;
            unsigned int r, g, b;
            GLushort result;
            
            inc = dither[x&3];

	    result = (GLushort) ColorToPixel( gc, cfb, fr + inc,
					      fg + inc, fb + inc );
            *fp = result;
        }
    }
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

/* Hardware-accelerated fill */
static void ClearFill(__GLcolorBuffer *cfb)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLdrawableBuffer *drawableBuf = cfb->buf.drawableBuf;
    __GLdrawablePrivate *dp = gc->drawablePrivate;
    GLint x, y, w, h; 
    __GLcolor color;
    GLuint clearVal;

    __GL_LOCK_DP(dp, gc);

    w = gc->transform.clipX1 - gc->transform.clipX0;
    h = gc->transform.clipY1 - gc->transform.clipY0;

    if ((w <= 0) || (h <= 0)) {
	__GL_UNLOCK_DP(dp);
	return;
    }

    x = gc->transform.clipX0 - gc->constants.viewportXAdjust;
    y = gc->transform.clipY0 - gc->constants.viewportYAdjust;

    color.r = gc->state.raster.clear.r * gc->frontBuffer.redScale;
    color.g = gc->state.raster.clear.g * gc->frontBuffer.greenScale;
    color.b = gc->state.raster.clear.b * gc->frontBuffer.blueScale;

    if (gc->modes.alphaBits>0) {
	color.a = gc->state.raster.clear.a * gc->frontBuffer.alphaScale;
	clearVal = 
	    (((GLuint) (color.r + __glHalf)) << cfb->redShift) |
	    (((GLuint) (color.g + __glHalf)) << cfb->greenShift) |
	    (((GLuint) (color.b + __glHalf)) << cfb->blueShift) |
	    (((GLuint) (color.a + __glHalf)) << cfb->alphaShift);
    }
    else {
	clearVal =
	    (((GLuint) (color.r + __glHalf)) << cfb->redShift) |
	    (((GLuint) (color.g + __glHalf)) << cfb->greenShift) |
	    (((GLuint) (color.b + __glHalf)) << cfb->blueShift);
    }
      
    assert(NULL != drawableBuf->fill);

    (*drawableBuf->fill)(drawableBuf, dp, clearVal, x, y, w, h);

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
	frag.color[__GL_PRIMARY_COLOR] = *cp++;

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
		frag.color[__GL_PRIMARY_COLOR] = *cp;

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

static GLboolean StoreSpan_16_General(__GLcontext *gc)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    int x, x1;
    int i;
    __GLfragment frag;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *drawableBuf;
    GLint w;
    GLushort *fp;
    __GLcolor blendColor;
    const __GLcolor *color;
    __GLfloat inc;
    GLushort fbcolor, result;

    w = gc->polygon.shader.length;

    frag.y = gc->polygon.shader.frag.y;
    x = gc->polygon.shader.frag.x;
    x1 = gc->polygon.shader.frag.x + w;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;
    drawableBuf = cfb->buf.drawableBuf;

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLushort *), x, frag.y);

    for (i = x; i < x1; i++, fp++, cp++) {

	frag.x = i;
	frag.color[__GL_PRIMARY_COLOR] = *cp;

	if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		    !__glTestOwnership(gc, frag.x, frag.y)) {
	    continue;
	}

	if (modeFlags & __GL_SHADE_DITHER) {
	    GLint ix = __GL_DITHER_INDEX(frag.x, frag.y);
	    inc = ((__glDitherTable[ix] << 1) + 1) / 
		(__GLfloat) (2 * __GL_DITHER_PRECISION);
	} else {
	    inc = __glHalf;
	}
	if (modeFlags & __GL_SHADE_BLEND) {
	    color = &blendColor;
	    (*gc->procs.blend)(gc, cfb, &frag, &blendColor);
	} else {
	    color = &(frag.color[__GL_PRIMARY_COLOR]);
	}

	result = (GLushort) ColorToPixel( gc, cfb, color->r + inc,
					  color->g + inc, color->b + inc );

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

    return GL_FALSE;
}

static GLboolean StoreSpan_16(__GLcontext *gc)
{
    int x, x1, y;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *drawableBuf;
    GLushort *fp;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    y = gc->polygon.shader.frag.y;
    x = gc->polygon.shader.frag.x;
    x1 = x + gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;
    drawableBuf = cfb->buf.drawableBuf;

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLushort *), x, y);

    for (;x < x1; x++, cp++, fp++) {

	unsigned int r, g, b;
	GLushort result;

	if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		    !__glTestOwnership(gc, x, y)) {
	    continue;
	}

	result = (GLushort) ColorToPixel( gc, cfb, cp->r, cp->g, cp->b );

	*fp = result;
    }

    return GL_FALSE;
}

static GLboolean StoreSpan_16_Dither(__GLcontext *gc)
{
    int x, x1, y;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *drawableBuf;
    GLushort *fp;
    __GLfloat *dither;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    y = gc->polygon.shader.frag.y;
    x = gc->polygon.shader.frag.x;
    x1 = x + gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;
    drawableBuf = cfb->buf.drawableBuf;

    fp = __GL_FB_ADDRESS(gc, cfb, drawableBuf, (GLushort *), x, y);

    dither = __glDitherTableFloat + ((y<<2) & 12);

    for (;x < x1; x++, cp++, fp++) {
	unsigned int r, g, b;
	__GLfloat ftmp, inc;
	GLushort result;

	if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		    !__glTestOwnership(gc, x, y)) {
	    continue;
	}

	inc = dither[x & 3];

	result = (GLushort) ColorToPixel( gc, cfb, cp->r + inc, cp->g + inc,
					  cp->b + inc );

	*fp = result;
    }

    return GL_FALSE;
}

static GLboolean StoreSpan_24(__GLcontext *gc)
{
    int x, x1, y;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *drawableBuf;
    GLubyte *fp;
    GLuint stride;
    __GLfloat inc;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    y = gc->polygon.shader.frag.y;
    x = gc->polygon.shader.frag.x;
    x1 = x + gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    cfb = gc->polygon.shader.cfb;
    drawableBuf = cfb->buf.drawableBuf;

    stride = ((drawableBuf->outerWidth * 3) + 3) & ~3;
    fp = ((GLubyte *) drawableBuf->base) +
	(y - gc->constants.viewportYAdjust) * stride +
	(x - gc->constants.viewportXAdjust) * 3;

    if (!(modeFlags & __GL_SHADE_DITHER)) {
	inc = __glHalf;
    }

    for (;x < x1; x++, cp++, fp+=3) {
	GLuint r, g, b;

	if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
		    !__glTestOwnership(gc, x, y)) {
	    continue;
	}

	if (modeFlags & __GL_SHADE_DITHER) {
	    GLint ix = __GL_DITHER_INDEX(x, y);
	    inc = ((__glDitherTable[ix] << 1) + 1) /
		(__GLfloat) (2 * __GL_DITHER_PRECISION);
	}

	r = (GLuint) (cp->r + inc);
	g = (GLuint) (cp->g + inc);
	b = (GLuint) (cp->b + inc);

	fp[2] = r;
	fp[1] = g;
	fp[0] = b;
    }

    return GL_FALSE;
}

static void Pick(__GLcontext *gc, __GLcolorBuffer *cfb)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint totalMask;

    totalMask = gc->modes.redMask |
		gc->modes.greenMask |
		gc->modes.blueMask |
		gc->modes.alphaMask;

    if (gc->state.raster.drawBuffer == GL_NONE) {
	cfb->sourceMask = 0;
	cfb->destMask = totalMask;
    } else {
	GLuint sourceMask = 0;

	if (gc->state.raster.rMask) {
	    sourceMask |= gc->modes.redMask;
	}
	if (gc->state.raster.gMask) {
	    sourceMask |= gc->modes.greenMask;
	}
	if (gc->state.raster.bMask) {
	    sourceMask |= gc->modes.blueMask;
	}
	if (gc->state.raster.aMask) {
	    sourceMask |= gc->modes.alphaMask;
	}
	cfb->sourceMask = sourceMask;
	cfb->destMask = totalMask & ~sourceMask;
    }

    switch (cfb->buf.drawableBuf->elementSize) {
    case 1:
	cfb->store = Store_8;
	break;

    case 2:
	cfb->store = Store_16;
	break;

    case 3:
	cfb->store = Store_24;

	if (gc->polygon.shader.modeFlags & (__GL_SHADE_LOGICOP |
					    __GL_SHADE_BLEND)) {
	    cfb->storeSpan = StoreSpan;
	} else {
	    cfb->storeSpan = StoreSpan_24;
	}
	break;

    case 4:
	if (gc->modes.alphaMask) {
	    cfb->store = Store_32A;
	} else {
	    cfb->store = Store_32;
	}
	break;
    }

    cfb->clear = Clear;
    cfb->clearRect = ClearRect;

    if (cfb->sourceMask == totalMask) {
#if !defined(__GL_CORE_CLEARS_BUFFERS)
	if (cfb->buf.drawableBuf->fill) {
	    cfb->clear = ClearFill;
	    cfb->clearRect = NULL;
	} else
#endif /* __GL_CORE_CLEARS_BUFFERS */
	{
#if defined(__GL_USE_ASMCODE) && defined(i386)
	    if (modeFlags & __GL_SHADE_DITHER) {
		switch (cfb->buf.drawableBuf->elementSize) {
		  case 1:
		      cfb->clearRect = __glI386ClearRectDitherRGB8;
		      break;
		  case 2:
		      cfb->clearRect = __glI386ClearRectDitherRGB16;
		      break;
		  case 3:
		  case 4:
		      cfb->clearRect = __glI386ClearRectRGB;
		      break;
		}
	    } else {
		if (cfb->buf.drawableBuf->elementSize == 3) {
		    cfb->clearRect = ClearRect_24;
		} else {
		    cfb->clearRect = __glI386ClearRectRGB;
		}
	    }
#else /* __GL_USE_ASMCODE && i386 */
	    if (modeFlags & __GL_SHADE_DITHER) {
		if (cfb->buf.drawableBuf->elementSize == 2) {
		    cfb->clearRect = ClearRect_16_Dither;
		}
	    } else {
		if (cfb->buf.drawableBuf->elementSize == 3) {
		    cfb->clearRect = ClearRect_24;
		}
	    }
#endif /* __GL_USE_ASMCODE && i386 */
	}
    }
}

static void Pick16(__GLcontext *gc, __GLcolorBuffer *cfb)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLuint totalMask;

    totalMask = gc->modes.redMask |
		gc->modes.greenMask |
		gc->modes.blueMask |
		gc->modes.alphaMask;

    if (gc->state.raster.drawBuffer == GL_NONE) {
	cfb->sourceMask = 0;
	cfb->destMask = totalMask;
    } else {
	GLuint sourceMask = 0;

	if (gc->state.raster.rMask) {
	    sourceMask |= gc->modes.redMask;
	}
	if (gc->state.raster.gMask) {
	    sourceMask |= gc->modes.greenMask;
	}
	if (gc->state.raster.bMask) {
	    sourceMask |= gc->modes.blueMask;
	}
	if (gc->state.raster.aMask) {
	    sourceMask |= gc->modes.alphaMask;
	}
	cfb->sourceMask = sourceMask;
	cfb->destMask = totalMask & ~sourceMask;
    }

    cfb->store = Store_16;
    cfb->storeSpan = StoreSpan_16_General;

    cfb->clear = Clear;
    cfb->clearRect = ClearRect;

    if (cfb->sourceMask == totalMask) {

	if (0 == (modeFlags & (__GL_SHADE_LOGICOP | __GL_SHADE_BLEND))) {
	    if (modeFlags & __GL_SHADE_DITHER) {
		cfb->storeSpan = StoreSpan_16_Dither;
	    } else {
		cfb->storeSpan = StoreSpan_16;
	    }
	}

#if !defined(__GL_CORE_CLEARS_BUFFERS)
	if (cfb->buf.drawableBuf->fill) {
	    cfb->clear = ClearFill;
	    cfb->clearRect = NULL;
	} else {
#endif /* __GL_CORE_CLEARS_BUFFERS */
	    if (modeFlags & __GL_SHADE_DITHER) {
#if defined(__GL_USE_ASMCODE) && defined(i386)
		cfb->clearRect = __glI386ClearRectDitherRGB16;
#else /* __GL_USE_ASMCODE && i386 */
		cfb->clearRect = ClearRect_16_Dither;
#endif /* __GL_USE_ASMCODE && i386 */
	    } else {
#if defined(__GL_USE_ASMCODE) && defined(i386)
		cfb->clearRect = __glI386ClearRectRGB;
#endif /* __GL_USE_ASMCODE && i386 */
	    }
	}
    }
}

void __glInitRGB(__GLcolorBuffer *cfb, __GLcontext *gc )
{
    GLint i;
    GLuint mask;
    GLuint sourceMask;

    __glInitBuffer( &cfb->buf, gc );

    cfb->needColorFragmentOps = GL_FALSE;

    cfb->readSpan = __glReadSpan;
    cfb->returnSpan = __glReturnSpan;

    cfb->clear = Clear;
    cfb->pick = Pick;
    cfb->fetchSpan = __glFetchSpan;
    cfb->fetchStippledSpan = __glFetchSpan;
    cfb->storeSpan = StoreSpan;
    cfb->storeStippledSpan = StoreStippledSpan;

    mask = gc->modes.redMask;
    sourceMask = mask;
    for (i=0; mask; i++, mask>>=1) if (mask & 1) break;
    cfb->redShift = i;
    cfb->redScale = cfb->iRedScale = cfb->redMax = mask;

    mask = gc->modes.greenMask;
    sourceMask |= mask;
    for (i=0; mask; i++, mask>>=1) if (mask & 1) break;
    cfb->greenShift = i;
    cfb->greenScale = cfb->iGreenScale = cfb->greenMax = mask;

    mask = gc->modes.blueMask;
    sourceMask |= mask;
    for (i=0; mask; i++, mask>>=1) if (mask & 1) break;
    cfb->blueShift = i;
    cfb->blueScale = cfb->iBlueScale = cfb->blueMax = mask;

    mask = gc->modes.alphaMask;
    sourceMask |= mask;
    for (i=0; mask; i++, mask>>=1) if (mask & 1) break;
    cfb->alphaShift = i;
    if (mask) {
	cfb->alphaScale = cfb->iAlphaScale = mask;
    } else {
	/* Set to number large enough for table lookups */
	cfb->alphaScale = cfb->iAlphaScale = 255;
    }

    cfb->sourceMask = sourceMask;

    switch (gc->modes.rgbBits) {
    case 8:
	cfb->fetch = cfb->readColor = Fetch_8;
	break;
    case 16:
	cfb->fetch = cfb->readColor = Fetch_16;

	/* Enable FB-specific picker */
	cfb->pick = Pick16;
	break;
    case 24:
	cfb->fetch = cfb->readColor = Fetch_24;
	break;
    case 32:
	if (gc->modes.alphaMask) {
	    cfb->fetch = cfb->readColor = Fetch_32A;
	} else {
	    cfb->fetch = cfb->readColor = Fetch_32;
	}
	break;
    }

#if defined(__GL_FR_RAST)
    __glFRPickFBType(gc, cfb);
#endif
}
