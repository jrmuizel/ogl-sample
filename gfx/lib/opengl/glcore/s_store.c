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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_store.c#4 $
*/
#include "context.h"

/*
** Store fragment proc.
** alpha test on, stencil test on, depth test on
*/
void __glDoStore_ASD(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    if (!gc->frontBuffer.alphaTestFuncTable[(GLint) (frag->color[__GL_PRIMARY_COLOR].a
					    * gc->constants.alphaTableConv)]) {
	/* alpha test failed */
	return;
    }
    if (!(*gc->stencilBuffer.testFunc)(&gc->stencilBuffer, x, y)) {
	/* stencil test failed */
	(*gc->stencilBuffer.failOp)(&gc->stencilBuffer, x, y);
	return;
    }
    if (!(*gc->depthBuffer.store)(&gc->depthBuffer, x, y, frag->z)) {
	/* depth buffer test failed */
	(*gc->stencilBuffer.passDepthFailOp)(&gc->stencilBuffer, x, y);
	return;
    }
    (*gc->stencilBuffer.depthPassOp)(&gc->stencilBuffer, x, y);


    (*gc->procs.cfbStore)( cfb, frag );
}


/*
** Store fragment proc.
** alpha test on, stencil test on, depth test off
*/
void __glDoStore_AS(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    if (!gc->frontBuffer.alphaTestFuncTable[(GLint) (frag->color[__GL_PRIMARY_COLOR].a
					    * gc->constants.alphaTableConv)]) {
	/* alpha test failed */
	return;
    }
    if (!(*gc->stencilBuffer.testFunc)(&gc->stencilBuffer, x, y)) {
	/* stencil test failed */
	(*gc->stencilBuffer.failOp)(&gc->stencilBuffer, x, y);
	return;
    }
    (*gc->stencilBuffer.depthPassOp)(&gc->stencilBuffer, x, y);

    (*gc->procs.cfbStore)( cfb, frag );
}

/*
** Store fragment proc.
** alpha test on, stencil test off, depth test on
*/
void __glDoStore_AD(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    if (!gc->frontBuffer.alphaTestFuncTable[(GLint) (frag->color[__GL_PRIMARY_COLOR].a
					   * gc->constants.alphaTableConv)]) {
	/* alpha test failed */
	return;
    }
    if (!(*gc->depthBuffer.store)(&gc->depthBuffer, x, y, frag->z)) {
	/* depth buffer test failed */
	return;
    }

    (*gc->procs.cfbStore)( cfb, frag );
}

/*
** Store fragment proc.
** alpha test off, stencil test on, depth test on
*/
void __glDoStore_SD(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    if (!(*gc->stencilBuffer.testFunc)(&gc->stencilBuffer, x, y)) {
	/* stencil test failed */
	(*gc->stencilBuffer.failOp)(&gc->stencilBuffer, x, y);
	return;
    }
    if (!(*gc->depthBuffer.store)(&gc->depthBuffer, x, y, frag->z)) {
	/* depth buffer test failed */
	(*gc->stencilBuffer.passDepthFailOp)(&gc->stencilBuffer, x, y);
	return;
    }
    (*gc->stencilBuffer.depthPassOp)(&gc->stencilBuffer, x, y);

    (*gc->procs.cfbStore)( cfb, frag );
}

/*
** Store fragment proc.
** alpha test on, stencil test off, depth test off
*/
void __glDoStore_A(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    if (!gc->frontBuffer.alphaTestFuncTable[(GLint) (frag->color[__GL_PRIMARY_COLOR].a
					    * gc->constants.alphaTableConv)]) {
	/* alpha test failed */
	return;
    }

    (*gc->procs.cfbStore)( cfb, frag );
}

/*
** Store fragment proc.
** alpha test off, stencil test on, depth test off, draw to current buffer
*/
void __glDoStore_S(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    if (!(*gc->stencilBuffer.testFunc)(&gc->stencilBuffer, x, y)) {
	/* stencil test failed */
	(*gc->stencilBuffer.failOp)(&gc->stencilBuffer, x, y);
	return;
    }
    (*gc->stencilBuffer.depthPassOp)(&gc->stencilBuffer, x, y);

    (*gc->procs.cfbStore)( cfb, frag );
}

/*
** Store fragment proc.
** alpha test off, stencil test off, depth test on
*/
void __glDoStore_D(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    if (!(*gc->depthBuffer.store)(&gc->depthBuffer, x, y, frag->z)) {
	/* depth buffer test failed */
	return;
    }

    (*gc->procs.cfbStore)( cfb, frag );
}

/*
** Store fragment proc.
** alpha test off, stencil test off, depth test off, draw to current buffer
*/
void __glDoStore(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc;
    GLint x, y;

    gc = cfb->buf.gc;

    x = frag->x;
    y = frag->y;

    /* Pixel ownership, scissor */
    if (x < gc->transform.clipX0 || y < gc->transform.clipY0 ||
	    x >= gc->transform.clipX1 || y >= gc->transform.clipY1) {
	return;
    }

    (*gc->procs.cfbStore)( cfb, frag );
}

/************************************************************************/

typedef void (*StoreProc)(__GLcolorBuffer *cfb, const __GLfragment *frag);

static StoreProc storeProcs[] = {
    &__glDoStore,
    &__glDoStore_A,
    &__glDoStore_S,
    &__glDoStore_AS,
    &__glDoStore_D,
    &__glDoStore_AD,
    &__glDoStore_SD,
    &__glDoStore_ASD,
};

/* ARGSUSED */
void __glDoNullStore(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
}

void __glDoDoubleStore(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext * gc = cfb->buf.gc;
    cfb = gc->front;
    cfb->store( cfb, frag );
    cfb = gc->back;
    cfb->store( cfb, frag );
}

/* 
** Note:  You must call gc->front->pick and gc->back->pick before 
** __glGenericPickStoreProcs 
*/

void __glGenericPickStoreProcs(__GLcontext *gc)
{
    GLint ix = 0;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    if ((modeFlags & __GL_SHADE_ALPHA_TEST) && gc->modes.rgbMode) {
	ix |= 1;
    }
    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
	ix |= 2;
    }
    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
	ix |= 4;
    }
    switch (gc->state.raster.drawBuffer) {
      case GL_NONE:
        gc->procs.store = storeProcs[ix];
	gc->procs.cfbStore = __glDoNullStore;
	break;
      case GL_FRONT_AND_BACK:
	if (gc->buffers.doubleStore) {
            gc->procs.store = storeProcs[ix];
	    gc->procs.cfbStore = __glDoDoubleStore;
	    break;
	}
	/*
	** Note that there is an intentional drop through here.  If double
	** store is not set, then storing to this buffer is no different
	** that storing to the front buffer.
	*/
      case GL_FRONT:
      case GL_BACK:
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3:
	/*
	** This code knows that gc->drawBuffer will point to the
	** current buffer as chosen by glDrawBuffer
	*/
	if (ix == 0) {
	    /*
	    ** Since all modes are off, our code here is not doing any
	    ** good at all.  Put a copy of the color buffers store
	    ** proc in instead.
	    */
	    gc->procs.store = gc->drawBuffer->store;
	    gc->procs.cfbStore = gc->drawBuffer->store;
	} else {
            gc->procs.store = storeProcs[ix];
	    gc->procs.cfbStore = gc->drawBuffer->store;
	}
	break;
    }
}
