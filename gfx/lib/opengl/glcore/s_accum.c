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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_accum.c#5 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"

static void Load(__GLaccumBuffer* afb, __GLfloat val)
{
    __GLcontext *gc = afb->buf.gc;
    __GLdrawableBuffer *dBuf;
    GLint x0, y0, x1, y1;
    GLint w, w4, w1, ow, skip;
    __GLfloat rval, gval, bval, aval;
    __GLaccumCell *ac;
    __GLcolorBuffer *cfb;
    __GLcolor cbuf[4096];/*XXX*/

    __GL_LOCK_BUFFER_MASK(gc, gc->buffers.lock.readBufferMask | __GL_ACCUM_BUFFER_MASK);
    x0 = gc->transform.clipX0;
    y0 = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;
    w = x1 - x0;
    assert(w < 4096); /* make sure w in within viewport range! */

    dBuf = afb->buf.drawableBuf;

    ac = __GL_ACCUM_ADDRESS(gc,afb,dBuf,(__GLaccumCell*),x0,y0);
    cfb = gc->readBuffer;
    ow = w;
    w4 = w >> 2;
    w1 = w & 3;
    skip = dBuf->outerWidth - w;

    rval = val * afb->redScale;
    gval = val * afb->greenScale;
    bval = val * afb->blueScale;
    aval = val * afb->alphaScale;

    for (; y0 < y1; y0++) {
	__GLcolor *cp = &cbuf[0];
	(*cfb->readSpan)(cfb, x0, y0, &cbuf[0], ow);

	w = w4;
	while (--w >= 0) {
	    ac[0].r = (__GLaccumCellElement) (cp[0].r * rval);
	    ac[0].g = (__GLaccumCellElement) (cp[0].g * gval);
	    ac[0].b = (__GLaccumCellElement) (cp[0].b * bval);
	    ac[0].a = (__GLaccumCellElement) (cp[0].a * aval);

	    ac[1].r = (__GLaccumCellElement) (cp[1].r * rval);
	    ac[1].g = (__GLaccumCellElement) (cp[1].g * gval);
	    ac[1].b = (__GLaccumCellElement) (cp[1].b * bval);
	    ac[1].a = (__GLaccumCellElement) (cp[1].a * aval);

	    ac[2].r = (__GLaccumCellElement) (cp[2].r * rval);
	    ac[2].g = (__GLaccumCellElement) (cp[2].g * gval);
	    ac[2].b = (__GLaccumCellElement) (cp[2].b * bval);
	    ac[2].a = (__GLaccumCellElement) (cp[2].a * aval);

	    ac[3].r = (__GLaccumCellElement) (cp[3].r * rval);
	    ac[3].g = (__GLaccumCellElement) (cp[3].g * gval);
	    ac[3].b = (__GLaccumCellElement) (cp[3].b * bval);
	    ac[3].a = (__GLaccumCellElement) (cp[3].a * aval);
	    ac += 4;
	    cp += 4;
	}

	w = w1;
	while (--w >= 0) {
	    ac->r = (__GLaccumCellElement) (cp->r * rval);
	    ac->g = (__GLaccumCellElement) (cp->g * gval);
	    ac->b = (__GLaccumCellElement) (cp->b * bval);
	    ac->a = (__GLaccumCellElement) (cp->a * aval);
	    ac++;
	    cp++;
	}
	ac += skip;
    }

    __GL_UNLOCK_BUFFER_MASK(gc, gc->buffers.lock.readBufferMask | __GL_ACCUM_BUFFER_MASK);
}

static void Accumulate(__GLaccumBuffer* afb, __GLfloat val)
{
    __GLcontext *gc = afb->buf.gc;
    __GLdrawableBuffer *dBuf;
    GLint x0, y0, x1, y1;
    GLint w, ow, skip, w4, w1;
    __GLfloat rval, gval, bval, aval;
    __GLaccumCell *ac;
    __GLcolorBuffer *cfb;
    __GLcolor cbuf[4096];/*XXX*/

    __GL_LOCK_BUFFER_MASK(gc, gc->buffers.lock.readBufferMask | __GL_ACCUM_BUFFER_MASK);
    x0 = gc->transform.clipX0;
    y0 = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;
    w = x1 - x0;
    assert(w < 4096); /* make sure w in within viewport range! */

    dBuf = afb->buf.drawableBuf;

    ac = __GL_ACCUM_ADDRESS(gc,afb,dBuf,(__GLaccumCell*),x0,y0);
    cfb = gc->readBuffer;
    ow = w;
    w4 = w >> 2;
    w1 = w & 3;
    skip = dBuf->outerWidth - w;

    rval = val * afb->redScale;
    gval = val * afb->greenScale;
    bval = val * afb->blueScale;
    aval = val * afb->alphaScale;

    for (; y0 < y1; y0++) {
	__GLcolor *cp = &cbuf[0];
	(*cfb->readSpan)(cfb, x0, y0, &cbuf[0], ow);

	w = w4;
	while (--w >= 0) {
	    ac[0].r += (__GLaccumCellElement) (cp[0].r * rval);
	    ac[0].g += (__GLaccumCellElement) (cp[0].g * gval);
	    ac[0].b += (__GLaccumCellElement) (cp[0].b * bval);
	    ac[0].a += (__GLaccumCellElement) (cp[0].a * aval);

	    ac[1].r += (__GLaccumCellElement) (cp[1].r * rval);
	    ac[1].g += (__GLaccumCellElement) (cp[1].g * gval);
	    ac[1].b += (__GLaccumCellElement) (cp[1].b * bval);
	    ac[1].a += (__GLaccumCellElement) (cp[1].a * aval);

	    ac[2].r += (__GLaccumCellElement) (cp[2].r * rval);
	    ac[2].g += (__GLaccumCellElement) (cp[2].g * gval);
	    ac[2].b += (__GLaccumCellElement) (cp[2].b * bval);
	    ac[2].a += (__GLaccumCellElement) (cp[2].a * aval);

	    ac[3].r += (__GLaccumCellElement) (cp[3].r * rval);
	    ac[3].g += (__GLaccumCellElement) (cp[3].g * gval);
	    ac[3].b += (__GLaccumCellElement) (cp[3].b * bval);
	    ac[3].a += (__GLaccumCellElement) (cp[3].a * aval);
	    ac += 4;
	    cp += 4;
	}

	w = w1;
	while (--w >= 0) {
	    ac->r += (__GLaccumCellElement) (cp->r * rval);
	    ac->g += (__GLaccumCellElement) (cp->g * gval);
	    ac->b += (__GLaccumCellElement) (cp->b * bval);
	    ac->a += (__GLaccumCellElement) (cp->a * aval);
	    ac++;
	    cp++;
	}
	ac += skip;
    }

    __GL_UNLOCK_BUFFER_MASK(gc, gc->buffers.lock.readBufferMask | __GL_ACCUM_BUFFER_MASK);
}

static void Mult(__GLaccumBuffer *afb, __GLfloat val)
{
    __GLcontext *gc = afb->buf.gc;
    __GLdrawableBuffer *dBuf;
    GLint x0, y0, x1, y1;
    GLint w, w4, w1, skip;
    __GLaccumCell *ac;

    __GL_LOCK_BUFFER_MASK(gc, __GL_ACCUM_BUFFER_MASK);
    x0 = gc->transform.clipX0;
    y0 = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;

    dBuf = afb->buf.drawableBuf;

    ac = __GL_ACCUM_ADDRESS(gc,afb,dBuf,(__GLaccumCell*),x0,y0);
    w = x1 - x0;
    assert(w < 4096); /* make sure w in within viewport range! */
    skip = dBuf->outerWidth - w;

    if (val == __glZero) {
	/* Zero out the buffers contents */
	for (; y0 < y1; y0++) {
	    GLint ww = w;
	    while (ww > 0) {
		ac[0].r = 0; ac[0].g = 0; ac[0].b = 0; ac[0].a = 0;
		ac++;
		ww--;
	    }
	    ac += skip;
	}
	__GL_UNLOCK_BUFFER_MASK(gc, __GL_ACCUM_BUFFER_MASK);
	return;
    }

    w4 = w >> 2;
    w1 = w & 3;
    for (; y0 < y1; y0++) {
	w = w4;
	while (--w >= 0) {
	    ac[0].r = (__GLaccumCellElement) (ac[0].r * val);
	    ac[0].g = (__GLaccumCellElement) (ac[0].g * val);
	    ac[0].b = (__GLaccumCellElement) (ac[0].b * val);
	    ac[0].a = (__GLaccumCellElement) (ac[0].a * val);
	    ac[1].r = (__GLaccumCellElement) (ac[1].r * val);
	    ac[1].g = (__GLaccumCellElement) (ac[1].g * val);
	    ac[1].b = (__GLaccumCellElement) (ac[1].b * val);
	    ac[1].a = (__GLaccumCellElement) (ac[1].a * val);
	    ac[2].r = (__GLaccumCellElement) (ac[2].r * val);
	    ac[2].g = (__GLaccumCellElement) (ac[2].g * val);
	    ac[2].b = (__GLaccumCellElement) (ac[2].b * val);
	    ac[2].a = (__GLaccumCellElement) (ac[2].a * val);
	    ac[3].r = (__GLaccumCellElement) (ac[3].r * val);
	    ac[3].g = (__GLaccumCellElement) (ac[3].g * val);
	    ac[3].b = (__GLaccumCellElement) (ac[3].b * val);
	    ac[3].a = (__GLaccumCellElement) (ac[3].a * val);
	    ac += 4;
	}
	w = w1;
	while (--w >= 0) {
	    ac[0].r = (__GLaccumCellElement) (ac[0].r * val);
	    ac[0].g = (__GLaccumCellElement) (ac[0].g * val);
	    ac[0].b = (__GLaccumCellElement) (ac[0].b * val);
	    ac[0].a = (__GLaccumCellElement) (ac[0].a * val);
	    ac++;
	}
	ac += skip;
    }

    __GL_UNLOCK_BUFFER_MASK(gc, __GL_ACCUM_BUFFER_MASK);
}

static void Add(__GLaccumBuffer *afb, __GLfloat value)
{
    __GLcontext *gc = afb->buf.gc;
    __GLdrawableBuffer *dBuf;
    GLint x0, y0, x1, y1;
    GLint w, w4, w1, skip;
    __GLaccumCellElement rval, gval, bval, aval;
    __GLaccumCell *ac;

    rval = (__GLaccumCellElement)
	(value * gc->frontBuffer.redScale * afb->redScale + __glHalf);
    gval = (__GLaccumCellElement)
	(value * gc->frontBuffer.greenScale * afb->greenScale + __glHalf);
    bval = (__GLaccumCellElement)
	(value * gc->frontBuffer.blueScale * afb->blueScale + __glHalf);
    aval = (__GLaccumCellElement)
	(value * gc->frontBuffer.alphaScale * afb->alphaScale + __glHalf);

    __GL_LOCK_BUFFER_MASK(gc, __GL_ACCUM_BUFFER_MASK);
    x0 = gc->transform.clipX0;
    y0 = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;

    dBuf = afb->buf.drawableBuf;

    ac = __GL_ACCUM_ADDRESS(gc,afb,dBuf,(__GLaccumCell*),x0,y0);
    w = x1 - x0;
    assert(w < 4096); /* make sure w in within viewport range! */
    w4 = w >> 2;
    w1 = w & 3;
    skip = dBuf->outerWidth - w;
    for (; y0 < y1; y0++) {
	w = w4;
	while (--w >= 0) {
	    ac[0].r += rval; ac[0].g += gval; ac[0].b += bval; ac[0].a += aval;
	    ac[1].r += rval; ac[1].g += gval; ac[1].b += bval; ac[1].a += aval;
	    ac[2].r += rval; ac[2].g += gval; ac[2].b += bval; ac[2].a += aval;
	    ac[3].r += rval; ac[3].g += gval; ac[3].b += bval; ac[3].a += aval;
	    ac += 4;
	}
	w = w1;
	while (--w >= 0) {
	    ac[0].r += rval; ac[0].g += gval; ac[0].b += bval; ac[0].a += aval;
	    ac++;
	}
	ac += skip;
    }

    __GL_UNLOCK_BUFFER_MASK(gc, __GL_ACCUM_BUFFER_MASK);
}

static void Return(__GLaccumBuffer* afb, __GLfloat val)
{
    __GLcontext *gc = afb->buf.gc;
    __GLdrawableBuffer *dBuf;
    GLint x0, y0, x1, y1;
    GLint w, next;
    __GLaccumCell *ac;
    __GLcolorBuffer *cfb;
    __GLcolorBuffer *cfb2;

    __GL_LOCK_BUFFER_MASK(gc, gc->buffers.lock.drawBufferMask | __GL_ACCUM_BUFFER_MASK);
    x0 = gc->transform.clipX0;
    y0 = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;

    dBuf = afb->buf.drawableBuf;

    ac = __GL_ACCUM_ADDRESS(gc,afb,dBuf,(__GLaccumCell*),x0,y0);
    w = x1 - x0;
    assert(w < 4096); /* make sure w in within viewport range! */
    next = dBuf->outerWidth;

    if (gc->buffers.doubleStore) {
	/* Store to both buffers */
	cfb = &gc->frontBuffer;
	cfb2 = &gc->backBuffer;
	for (; y0 < y1; y0++) {
	    (*cfb->returnSpan)(cfb, x0, y0, ac, val, w);
	    (*cfb2->returnSpan)(cfb2, x0, y0, ac, val, w);
	    ac += next;
	}
    } else {
	cfb = gc->drawBuffer;
	for (; y0 < y1; y0++) {
	    (*cfb->returnSpan)(cfb, x0, y0, ac, val, w);
	    ac += next;
	}
    }

    __GL_UNLOCK_BUFFER_MASK(gc, gc->buffers.lock.drawBufferMask | __GL_ACCUM_BUFFER_MASK);
}

static void Clear(__GLaccumBuffer* afb)
{
    __GLcontext *gc = afb->buf.gc;
    __GLdrawableBuffer *dBuf;
    GLint x0, y0, x1, y1;
    GLint w, w4, w1, skip;
    __GLaccumCell *ac;
    __GLaccumCellElement r, g, b, a;
    __GLcolorBuffer *cfb = &gc->frontBuffer;
    __GLcolor *val = &gc->state.accum.clear;

    /*
    ** Convert abstract color into specific color value.
    */
    r = (__GLaccumCellElement) (val->r * cfb->redScale * afb->redScale);
    g = (__GLaccumCellElement) (val->g * cfb->greenScale * afb->greenScale);
    b = (__GLaccumCellElement) (val->b * cfb->blueScale * afb->blueScale);
    a = (__GLaccumCellElement) (val->a * cfb->alphaScale * afb->alphaScale);

    __GL_LOCK_BUFFER_MASK(gc, __GL_ACCUM_BUFFER_MASK);
    x0 = gc->transform.clipX0;
    y0 = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;

    dBuf = afb->buf.drawableBuf;

    ac = __GL_ACCUM_ADDRESS(gc,afb,dBuf,(__GLaccumCell*),x0,y0);
    w = x1 - x0;
    assert(w < 4096); /* make sure w in within viewport range! */
    w4 = w >> 2;
    w1 = w & 3;
    skip = dBuf->outerWidth - w;
    for (; y0 < y1; y0++) {
	w = w4;
	while (--w >= 0) {
	    ac[0].r = r; ac[0].g = g; ac[0].b = b; ac[0].a = a;
	    ac[1].r = r; ac[1].g = g; ac[1].b = b; ac[1].a = a;
	    ac[2].r = r; ac[2].g = g; ac[2].b = b; ac[2].a = a;
	    ac[3].r = r; ac[3].g = g; ac[3].b = b; ac[3].a = a;
	    ac += 4;
	}
	w = w1;
	while (--w >= 0) {
	    ac[0].r = r; ac[0].g = g; ac[0].b = b; ac[0].a = a;
	    ac++;
	}
	ac += skip;
    }

    __GL_UNLOCK_BUFFER_MASK(gc, __GL_ACCUM_BUFFER_MASK);
}

/************************************************************************/

static void Pick(__GLcontext *gc, __GLaccumBuffer *afb)
{
#ifdef __GL_LINT
    gc = gc;
    afb = afb;
#endif
}

void __glInitAccum64(__GLaccumBuffer *afb, __GLcontext *gc )
{
    __glInitBuffer( &afb->buf, gc );

    afb->pick = Pick;
    afb->clear = Clear;
    afb->accumulate = Accumulate;
    afb->load = Load;
    afb->ret = Return;
    afb->mult = Mult;
    afb->add = Add;

    if (gc->modes.rgbMode) {
	__GLcolorBuffer *cfb;
	__GLfloat scale;

	scale = (1 << (8 * sizeof(__GLaccumCellElement)))/2 - 1;

	cfb = &gc->frontBuffer;
	afb->redScale = scale / (cfb->redScale);
	afb->blueScale = scale / (cfb->blueScale);
	afb->greenScale = scale / (cfb->greenScale);
	afb->alphaScale = scale / (cfb->alphaScale);

	afb->oneOverRedScale = 1 / afb->redScale;
	afb->oneOverGreenScale = 1 / afb->greenScale;
	afb->oneOverBlueScale = 1 / afb->blueScale;
	afb->oneOverAlphaScale = 1 / afb->alphaScale;
    }
}

/************************************************************************/

void GLAPI __glim_Accum(GLenum op, GLfloat value)
{
    void (*accumOp)(__GLaccumBuffer *afb, __GLfloat value);
    __GLaccumBuffer *fb;
    __GL_SETUP();
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	    glAccum(op,value);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    fb = &gc->accumBuffer;
    if (!gc->modes.haveAccumBuffer || gc->modes.colorIndexMode) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    switch (op) {
      case GL_ACCUM:
	accumOp = Accumulate;
	break;
      case GL_LOAD:
	accumOp = Load;
	break;
      case GL_RETURN:
	accumOp = Return;
	break;
      case GL_MULT:
	accumOp = Mult;
	break;
      case GL_ADD:
	accumOp = Add;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if (gc->renderMode == GL_RENDER) {
	__GL_API_PIXEL_OP();
	(*accumOp)(fb, value);
    }
}
