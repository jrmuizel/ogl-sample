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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_lnfast.c#5 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "g_imports.h"
#include "fmacros.h"

/* Flat shaded only! */
GLboolean __glStoreLine_CI_8_Flat(__GLcontext *gc)
{
    GLint xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    GLint len, ii;
    GLubyte *fp;
    GLuint result;

    len = gc->polygon.shader.length;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *),
			 gc->line.options.xStart, gc->line.options.yStart);

    SFloat2Int(cp->r, result);

    for (ii = 0; ii < len; ii++) {

	*fp = result;

	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    fp += dBig;
	} else {
	    fp += dLittle;
	}
    }

    return GL_FALSE;
}

/* No dither, color per pixel */
GLboolean __glStoreLine_CI_8_Smooth(__GLcontext *gc)
{
    GLint dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    GLint len, ii;
    GLubyte *fp;
    GLuint result;

    len = gc->polygon.shader.length;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    dBig = gc->line.options.xBig +
	gc->line.options.yBig * dBuf->outerWidth;
    dLittle = gc->line.options.xLittle +
	gc->line.options.yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *),
			 gc->line.options.xStart, gc->line.options.yStart);

    for (ii = 0; ii < len; ii++, cp++) {

	SFloat2Int(cp->r, result);
	*fp = result;

	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    fp += dBig;
	} else {
	    fp += dLittle;
	}
    }

    return GL_FALSE;
}

/* Flat shaded, dithered */
GLboolean __glStoreLine_CI_8_Flat_Dither(__GLcontext *gc)
{
    GLint dLittle, dBig, fraction, dfraction, len;
    GLubyte *fp;
    GLint x, y, xLittle, yLittle, xBig, yBig;
    __GLfloat fresult;

    /* Initialization */
    {
	__GLcolorBuffer *cfb;
	__GLdrawableBuffer *dBuf;
	__GLlineOptions *options;
	GLint outerWidth;

        {
	    __GLshade *shader;
	    shader = &gc->polygon.shader;
	    cfb = shader->cfb;
	    dBuf = cfb->buf.drawableBuf;
	    len = shader->length;
	    fresult = shader->colors->r;
	}

	outerWidth = dBuf->outerWidth;
	options = &gc->line.options;
	xBig = options->xBig;
	yBig = options->yBig;
	dBig = xBig + yBig * outerWidth;
	yBig <<= 2;
	xLittle = options->xLittle;
	yLittle = options->yLittle;
	dLittle = xLittle + yLittle * outerWidth;
	yLittle <<= 2;
	x = options->xStart;
	y = options->yStart << 2;
	fraction = options->fraction;
	dfraction = options->dfraction;
	fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *), 
			     options->xStart, options->yStart);
    }

    while (--len >= 0) {

	GLuint ix = ((x & 3) | (y & 12));
	__GLfloat ftmp;
	GLuint result;

	ftmp = fresult + __glDitherTableFloat[ix];
	SFloat2Int(ftmp, result);
	*fp = result;

	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    x += xBig;
	    y += yBig;
	    fp += dBig;
	} else {
	    x += xLittle;
	    y += yLittle;
	    fp += dLittle;
	}
    }

    return GL_FALSE;
}

/* dithered, color per pixel */
GLboolean __glStoreLine_CI_8_Smooth_Dither(__GLcontext *gc)
{
    GLint dLittle, dBig, fraction, dfraction, len;
    GLubyte *fp;
    __GLcolor *cp;
    GLint x, y, xLittle, yLittle, xBig, yBig;

    /* Initialization */
    {
	__GLcolorBuffer *cfb;
	__GLdrawableBuffer *dBuf;
	__GLlineOptions *options;
	GLint outerWidth;

        {
	    __GLshade *shader;
	    shader = &gc->polygon.shader;
	    cfb = shader->cfb;
	    dBuf = cfb->buf.drawableBuf;
	    len = shader->length;
	    cp = shader->colors;
	}

	outerWidth = dBuf->outerWidth;
	options = &gc->line.options;
	xBig = options->xBig;
	yBig = options->yBig;
	dBig = xBig + yBig * outerWidth;
	yBig <<= 2;
	xLittle = options->xLittle;
	yLittle = options->yLittle;
	dLittle = xLittle + yLittle * outerWidth;
	yLittle <<= 2;
	x = options->xStart;
	y = options->yStart << 2;
	fraction = options->fraction;
	dfraction = options->dfraction;
	fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *), 
			     options->xStart, options->yStart);
    }

    while (--len >= 0) {

	GLuint ix = ((x & 3) | (y & 12));
	__GLfloat fresult;
	GLuint result;

	fresult = cp->r + __glDitherTableFloat[ix];
	SFloat2Int(fresult, result);
	*fp = result;
	cp++;

	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    x += xBig;
	    y += yBig;
	    fp += dBig;
	} else {
	    x += xLittle;
	    y += yLittle;
	    fp += dLittle;
	}
    }

    return GL_FALSE;
}

GLboolean __glStoreLine_RGB_16_Flat(__GLcontext *gc)
{
    GLint xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    GLint len, ii;
    GLushort *fp;
    GLushort result;
    GLuint r, g, b;

    len = gc->polygon.shader.length;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *),
			 gc->line.options.xStart, gc->line.options.yStart);

    SFloat2Int(cp->r, r);
    SFloat2Int(cp->g, g);
    SFloat2Int(cp->b, b);
    result =
	(r << cfb->redShift) |
	(g << cfb->greenShift) |
	(b << cfb->blueShift);

    for (ii = 0; ii < len; ii++) {
	*fp = result;
	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    fp += dBig;
	} else {
	    fp += dLittle;
	}
    }

    return GL_FALSE;
}

GLboolean __glStoreLine_RGB_16_Dither(__GLcontext *gc)
{
    GLint x, y, xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    GLint len, ii;
    GLushort *fp;
    GLuint r, g, b;

    len = gc->polygon.shader.length;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    x = gc->line.options.xStart;
    y = gc->line.options.yStart;
    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *), x, y);

    /* These are used only for dithering */
    y <<= 2;
    yBig <<= 2;
    yLittle <<= 2;

    for (ii = 0; ii < len; ii++, cp++) {
	
	GLuint ix = ((x & 3) | (y & 12));
	__GLfloat ftmp, inc;

	inc = __glDitherTableFloat[ix];
	ftmp = cp->r + inc;
	SFloat2Int(ftmp, r);
	ftmp = cp->g + inc;
	SFloat2Int(ftmp, g);
	ftmp = cp->b + inc;
	SFloat2Int(ftmp, b);

	*fp =
	    (r << cfb->redShift) |
	    (g << cfb->greenShift) |
	    (b << cfb->blueShift);

	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    x += xBig;
	    y += yBig;
	    fp += dBig;
	} else {
	    x += xLittle;
	    y += yLittle;
	    fp += dLittle;
	}
    }

    return GL_FALSE;
}

GLboolean __glStoreLine_RGB_16_Smooth(__GLcontext *gc)
{
    GLint xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    GLint len, ii;
    GLushort *fp;
    GLint rs, gs, bs;

    len = gc->polygon.shader.length;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *),
			 gc->line.options.xStart, gc->line.options.yStart);

    rs = cfb->redShift;
    gs = cfb->greenShift;
    bs = cfb->blueShift;

    for (ii = 0; ii < len; ii++, cp++) {
	unsigned int r, g, b;

	SFloat2Int(cp->r, r);
	SFloat2Int(cp->g, g);
	SFloat2Int(cp->b, b);

	*fp = (r << rs) | (g << gs) | (b << bs);

	fraction += dfraction;
	if (fraction < 0) {
	    fraction &= ~0x80000000;
	    fp += dBig;
	} else {
	    fp += dLittle;
	}
    }

    return GL_FALSE;
}

/* Stippled versions */

/* Flat shaded only! */
GLboolean __glStoreStippledLine_CI_8_Flat(__GLcontext *gc)
{
    GLint xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    __GLstippleWord inMask, bit, *sp;
    GLint len, count;
    GLubyte *fp;
    GLuint result;

    len = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *),
			 gc->line.options.xStart, gc->line.options.yStart);

    SFloat2Int(cp->r, result);

    do {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);

	do {
	    if (inMask & bit) {
		*fp = result;
	    }

#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		fp += dBig;
	    } else {
		fp += dLittle;
	    }

	} while (--count);
    } while (len);

    return GL_FALSE;
}

/* No dither, color per pixel */
GLboolean __glStoreStippledLine_CI_8_Smooth(__GLcontext *gc)
{
    GLint dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    __GLstippleWord inMask, bit, *sp;
    GLint len, count;
    GLubyte *fp;
    GLuint result;

    len = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    dBig = gc->line.options.xBig +
	gc->line.options.yBig * dBuf->outerWidth;
    dLittle = gc->line.options.xLittle +
	gc->line.options.yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *),
			 gc->line.options.xStart, gc->line.options.yStart);

    do {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);

	do {
	    if (inMask & bit) {
		SFloat2Int(cp->r, result);
		*fp = result;
	    }

#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */
	    cp++;

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		fp += dBig;
	    } else {
		fp += dLittle;
	    }
	} while (--count);
    } while (len);

    return GL_FALSE;
}

/* Flat shaded, dithered */
GLboolean __glStoreStippledLine_CI_8_Flat_Dither(__GLcontext *gc)
{
    GLint dLittle, dBig, fraction, dfraction, len;
    GLubyte *fp;
    GLint x, y, xLittle, yLittle, xBig, yBig, count;
    __GLstippleWord inMask, bit, *sp;
    __GLfloat fresult;

    /* Initialization */
    {
	__GLcolorBuffer *cfb;
	__GLdrawableBuffer *dBuf;
	__GLlineOptions *options;
	GLint outerWidth;

        {
	    __GLshade *shader;
	    shader = &gc->polygon.shader;
	    sp = shader->stipplePat;
	    cfb = shader->cfb;
	    dBuf = cfb->buf.drawableBuf;
	    len = shader->length;
	    fresult = shader->colors->r;
	}

	outerWidth = dBuf->outerWidth;
	options = &gc->line.options;
	xBig = options->xBig;
	yBig = options->yBig;
	dBig = xBig + yBig * outerWidth;
	yBig <<= 2;
	xLittle = options->xLittle;
	yLittle = options->yLittle;
	dLittle = xLittle + yLittle * outerWidth;
	yLittle <<= 2;
	x = options->xStart;
	y = options->yStart << 2;
	fraction = options->fraction;
	dfraction = options->dfraction;
	fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *), 
			     options->xStart, options->yStart);
    }

    do {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	do {
	    if (inMask & bit) {
		GLuint ix = ((x & 3) | (y & 12));
		__GLfloat ftmp;
		GLuint result;

		ftmp = fresult + __glDitherTableFloat[ix];
		SFloat2Int(ftmp, result);
		*fp = result;
	    }

#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		x += xBig;
		y += yBig;
		fp += dBig;
	    } else {
		x += xLittle;
		y += yLittle;
		fp += dLittle;
	    }
	} while (--count);
    } while (len);

    return GL_FALSE;
}

/* dithered, color per pixel */
GLboolean __glStoreStippledLine_CI_8_Smooth_Dither(__GLcontext *gc)
{
    GLint dLittle, dBig, fraction, dfraction, len;
    GLubyte *fp;
    __GLcolor *cp;
    GLint x, y, xLittle, yLittle, xBig, yBig, count;
    __GLstippleWord inMask, bit, *sp;

    /* Initialization */
    {
	__GLcolorBuffer *cfb;
	__GLdrawableBuffer *dBuf;
	__GLlineOptions *options;
	GLint outerWidth;

        {
	    __GLshade *shader;
	    shader = &gc->polygon.shader;
	    sp = shader->stipplePat;
	    cfb = shader->cfb;
	    dBuf = cfb->buf.drawableBuf;
	    len = shader->length;
	    cp = shader->colors;
	}

	outerWidth = dBuf->outerWidth;
	options = &gc->line.options;
	xBig = options->xBig;
	yBig = options->yBig;
	dBig = xBig + yBig * outerWidth;
	yBig <<= 2;
	xLittle = options->xLittle;
	yLittle = options->yLittle;
	dLittle = xLittle + yLittle * outerWidth;
	yLittle <<= 2;
	x = options->xStart;
	y = options->yStart << 2;
	fraction = options->fraction;
	dfraction = options->dfraction;
	fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLubyte *), 
			     options->xStart, options->yStart);
    }

    do {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	do {
	    if (inMask & bit) {
		GLuint ix = ((x & 3) | (y & 12));
		__GLfloat fresult;
		GLuint result;

		fresult = cp->r + __glDitherTableFloat[ix];
		SFloat2Int(fresult, result);
		*fp = result;
	    }
#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */
	    cp++;

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		x += xBig;
		y += yBig;
		fp += dBig;
	    } else {
		x += xLittle;
		y += yLittle;
		fp += dLittle;
	    }
	} while (--count);
    } while (len);

    return GL_FALSE;
}

GLboolean __glStoreStippledLine_RGB_16_Flat(__GLcontext *gc)
{
    GLint xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    __GLstippleWord inMask, bit, *sp;
    GLint len, count;
    GLushort *fp;
    GLushort result;
    GLuint r, g, b;

    len = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *),
			 gc->line.options.xStart, gc->line.options.yStart);

    SFloat2Int(cp->r, r);
    SFloat2Int(cp->g, g);
    SFloat2Int(cp->b, b);
    result =
	(r << cfb->redShift) |
	(g << cfb->greenShift) |
	(b << cfb->blueShift);

    do {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	do {
	    if (inMask & bit) {
		*fp = result;
	    }
#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */
	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		fp += dBig;
	    } else {
		fp += dLittle;
	    }
	} while (--count);
    } while (len);

    return GL_FALSE;
}

GLboolean __glStoreStippledLine_RGB_16_Dither(__GLcontext *gc)
{
    GLint x, y, xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    __GLstippleWord inMask, bit, *sp;
    GLint len, count;
    GLushort *fp;
    GLuint r, g, b;

    len = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    x = gc->line.options.xStart;
    y = gc->line.options.yStart;
    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *), x, y);

    /* These are used only for dithering */
    y <<= 2;
    yBig <<= 2;
    yLittle <<= 2;

    do {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	do {
	    if (inMask & bit) {
		GLuint ix = ((x & 3) | (y & 12));
		__GLfloat ftmp, inc;

		inc = __glDitherTableFloat[ix];
		ftmp = cp->r + inc;
		SFloat2Int(ftmp, r);
		ftmp = cp->g + inc;
		SFloat2Int(ftmp, g);
		ftmp = cp->b + inc;
		SFloat2Int(ftmp, b);

		*fp =
		     (r << cfb->redShift) |
		     (g << cfb->greenShift) |
		     (b << cfb->blueShift);
	    }
#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */
	    cp++;

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		x += xBig;
		y += yBig;
		fp += dBig;
	    } else {
		x += xLittle;
		y += yLittle;
		fp += dLittle;
	    }
	} while (--count);
    } while (len);

    return GL_FALSE;
}

GLboolean __glStoreStippledLine_RGB_16_Smooth(__GLcontext *gc)
{
    GLint xLittle, xBig, yLittle, yBig, dLittle, dBig;
    GLint fraction, dfraction;
    __GLcolor *cp;
    __GLcolorBuffer *cfb;
    __GLdrawableBuffer *dBuf;
    __GLstippleWord inMask, bit, *sp;
    GLint len, count;
    GLushort *fp;
    GLint rs, gs, bs;

    len = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;
    cfb = gc->polygon.shader.cfb;
    dBuf = cfb->buf.drawableBuf;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    dBig = xBig + yBig * dBuf->outerWidth;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;
    dLittle = xLittle + yLittle * dBuf->outerWidth;
    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;
    cp = gc->polygon.shader.colors;

    fp = __GL_FB_ADDRESS(gc, cfb, dBuf, (GLushort *),
			 gc->line.options.xStart, gc->line.options.yStart);

    rs = cfb->redShift;
    gs = cfb->greenShift;
    bs = cfb->blueShift;

    do {
	count = len;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	len -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	do {
	    if (inMask & bit) {
		unsigned int r, g, b;

		SFloat2Int(cp->r, r);
		SFloat2Int(cp->g, g);
		SFloat2Int(cp->b, b);

		*fp = (r << rs) | (g << gs) | (b << bs);
	    }
#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */
	    cp++;

	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		fp += dBig;
	    } else {
		fp += dLittle;
	    }
	} while (--count);
    } while (len);

    return GL_FALSE;
}
