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
** $Header: //depot/main/gfx/lib/opengl/drivers/DDX/ddxci.c#2 $
*/
#include <stdio.h>
#include <servermd.h>
#include "ddxcx.h"
#include "global.h"
#include "glxdrawable.h"
#include "ddxci.h"

/*
** This file contains a simple color buffer rendering implemention
** for a color index buffer.  It is slow and stupid and here just as a
** place holder so that the code can run inside an X server immediately.
** At the earliest opportunity it should be discarded.
*/

static void Store(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLDdxContext *hwcx = (__GLDdxContext *)gc;
    __GLXdrawablePrivate *glxPriv = 
	(__GLXdrawablePrivate *) gc->drawablePrivate->wsPriv;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GCPtr pGC = hwcx->pGC;
    DrawablePtr pDraw;
    xPoint p;
    __GLfloat frac;
    GLuint color, x, y;
    XID pixel, logicOp;

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &
	!__glTestOwnership(gc, frag->x, frag->y)) {
	return;
    }

    x = frag->x;
    y = frag->y;
    if ((x < gc->transform.clipX0) || (x >= gc->transform.clipX1)
	|| (y < gc->transform.clipY0) || (y >= gc->transform.clipY1)) {
	/* Scissor test failed */
	return;
    }

    pDraw = glxPriv->pDraw;

    if (modeFlags & __GL_SHADE_DITHER) {
	GLint ix = __GL_DITHER_INDEX(x, y);
	/*
	** Convert color index into integer, keeping 4 bits of fractional
	** precision (and rounding up).
	*/
	color = (frag->color[__GL_PRIMARY_COLOR].r * __GL_DITHER_PRECISION +
		 __glHalf);
	color = (color + __glDitherTable[ix]) >> __GL_DITHER_BITS;
    } else {
	/*
	** Colors are rounded
	*/
	color = (frag->color[__GL_PRIMARY_COLOR].r + __glHalf);
    }
    
    /* Set color */
    pixel = color & (cfb->redMax);
    DoChangeGC(pGC, GCForeground, &pixel, 0);

    /* Set the logic op */
    if (modeFlags & __GL_SHADE_LOGICOP) {
	logicOp = gc->state.raster.logicOp - GL_CLEAR;
    } else {
	logicOp = GXcopy;
    }
    DoChangeGC(pGC, GCFunction, (XID*)&logicOp, 0);

    /* Draw the pixel */
    ValidateGC(pDraw, pGC);
    p.x = __GL_UNBIAS_X(gc, frag->x);
    p.y = __GL_UNBIAS_Y(gc, frag->y);
    (*pGC->ops->PolyPoint)(pDraw, pGC, CoordModeOrigin, 1, &p);

    /* Undo the logicop */
    if (modeFlags & __GL_SHADE_LOGICOP) {
	logicOp = GXcopy;
	DoChangeGC(pGC, GCFunction, (XID*)&logicOp, 0);
    }
}

static void Fetch(__GLcolorBuffer *cfb, GLint x, GLint y,
		  __GLcolor *result)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLDdxContext *hwcx = (__GLDdxContext *)gc;
    __GLXdrawablePrivate *glxPriv =
	(__GLXdrawablePrivate *) gc->drawablePrivate->wsPriv;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    DrawablePtr pDraw;
    unsigned long pixel;
    unsigned long planeMask;
    int widthBytes, paddedBytes, i;
    unsigned char buf[16];

    if ((modeFlags & __GL_SHADE_OWNERSHIP_TEST) &&
	!__glTestOwnership(gc, x, y)) {
	result->r = 0;
	return;
    }

    pDraw = glxPriv->pDraw;

    planeMask = (1 << pDraw->depth) - 1;
    x = __GL_UNBIAS_X(gc, x);
    y = __GL_UNBIAS_Y(gc, y);

    /*
    ** If pixel is not on screen, don't try to read.
    */
    if (pDraw->x + x < 0 || pDraw->x + x >= pDraw->pScreen->width ||
	pDraw->y + y < 0 || pDraw->y + y >= pDraw->pScreen->height) {
	result->r = __glZero;
	return;
    }
    paddedBytes = PixmapBytePad(1, pDraw->depth);
    if (paddedBytes > 16) {
	ErrorF("GLX DDX Fetch routine: padded pixel length too large\n");
	return;
    }
    (*pDraw->pScreen->GetImage)(pDraw, x, y, 1, 1, ZPixmap, planeMask,
				(char *)buf);

    widthBytes = gc->drawablePrivate->frontBuffer.elementSize;
    /*
    ** Shift according to the endianness of the machine.
    */
#if IMAGE_BYTE_ORDER == MSBFirst    
    pixel = 0;
    for (i=0; i < widthBytes; i++) {
	pixel <<= 8;
	pixel |= buf[i];
    }
#else
    pixel = 0;
    for (i=widthBytes-1; i >= 0; i--) {
	pixel <<= 8;
	pixel |= buf[i];
    }
#endif
    result->r = pixel;
}

/************************************************************************/

static void Clear(__GLcolorBuffer *cfb)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLDdxContext *hwcx = (__GLDdxContext *)gc;
    __GLXdrawablePrivate *glxPriv =
	(__GLXdrawablePrivate *) gc->drawablePrivate->wsPriv;
    GCPtr pGC = hwcx->pGC;
    DrawablePtr pDraw;
    __GLfloat i, frac;
    GLint x, y, x0, y0, x1, y1;
    xPoint p;
    xRectangle rect;
    GLint pixel;

    pDraw = glxPriv->pDraw;

    x0 = __GL_UNBIAS_X(gc, (GLint)gc->transform.clipX0);
    y0 = __GL_UNBIAS_Y(gc, (GLint)gc->transform.clipY0);
    x1 = __GL_UNBIAS_X(gc, (GLint)gc->transform.clipX1);
    y1 = __GL_UNBIAS_Y(gc, (GLint)gc->transform.clipY1);
    if ((x1 - x0 == 0) || (y1 - y0 == 0)) {
	return;
    }

    /*
    ** Undo any mask that might be in effect.
    */

    i = gc->state.raster.clearIndex;
    if (i == (GLint)i) {
	/*
	** The clear index has no fraction, so no dithering needed.
	** NOTE: since this code is really just a place holder for
	** real code, this case is not strictly needed.  However, in
	** the interests of having the tests run in our lifetime...
	*/
	rect.x = x0;
	rect.y = y0;
	rect.width = x1 - x0;
	rect.height = y1 - y0;

	/* Store */
	if (i > cfb->redMax) {
	    pixel = cfb->redMax;
	} else {
	    pixel = i;
	}
	DoChangeGC(pGC, GCForeground, (XID *)&pixel, 0);
	ValidateGC(pDraw, pGC);
	(*pGC->ops->PolyFillRect)(pDraw, pGC, 1, &rect);
	
    } else {
	GLint color;

	/*
	** Clear with a dithered color index.
	*/
	color = (gc->state.raster.clearIndex * __GL_DITHER_PRECISION + 
		 __glHalf);
	for (y=y0; y < y1; y++) {
	    for (x=x0; x < x1; x++) {
		GLint ix = __GL_DITHER_INDEX(x, y);
		pixel = (color + __glDitherTable[ix]) >> __GL_DITHER_BITS;
		if (pixel > cfb->redMax) pixel = cfb->redMax;

		/* Store */
		p.x = x;
		p.y = y;
		DoChangeGC(pGC, GCForeground, (XID *)&pixel, 0);
		ValidateGC(pDraw, pGC);
		(*pGC->ops->PolyPoint)(pDraw, pGC, CoordModeOrigin, 1, &p);
	    }
	}
    }
}

/************************************************************************/

static void Pick(__GLcontext *gc, __GLcolorBuffer *cfb)
{
    __GLDdxContext *hwcx = (__GLDdxContext *)gc;
    GCPtr pGC = hwcx->pGC;

    /*
    ** The redMax will always be a mask of contiguous ones bits, as can be
    ** seen from its initialization down below.
    */
    cfb->sourceMask = gc->state.raster.writeMask & cfb->redMax;

    /* Set the write mask. */
    DoChangeGC(pGC, GCPlaneMask, (XID*)&cfb->sourceMask, 0);
}

void __glDdxInitCI(__GLcolorBuffer *cfb, __GLcontext *gc)
{
    __glInitCI(cfb, gc);

    /*
    ** override methods with local implementation
    */
    cfb->pick = Pick;
    cfb->store = Store;
    cfb->fetch = Fetch;
    cfb->readColor = Fetch;	/* NOTE: only works for singlebuffer! */
    cfb->clear = Clear;
}
