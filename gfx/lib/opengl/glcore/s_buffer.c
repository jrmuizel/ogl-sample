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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_buffer.c#7 $
*/
#include "context.h"
#include "global.h"
#include "glmath.h"

/*
** initialize the buffer stuff
*/
/* ARGSUSED */
void __glInitBuffer(__GLbuffer *buf, __GLcontext *gc)
{
    buf->gc = NULL;
    buf->drawableBuf = NULL;
    buf->other = NULL;
    buf->resize = NULL;
    buf->makeCurrent = __glMakeCurrentBuffer;
    buf->loseCurrent = __glLoseCurrentBuffer;
    buf->free = NULL;
}

/*
** make a context current
*/
void __glMakeCurrentBuffer(__GLbuffer *buf, __GLdrawableBuffer *drawableBuf,
			   __GLcontext *gc)
{
    buf->gc = gc;
    buf->drawableBuf = drawableBuf;
}

/*
** make a context not current
*/
void __glLoseCurrentBuffer(__GLbuffer *buf, __GLcontext *gc)
{
    buf->gc = NULL;
    buf->drawableBuf = NULL;
}

/* -------------------------------------------------------------------- */

/*
** routines for keeping track of the system lock
*/

/*
** routines to lock/unlock buffer according to a mask
*/
void __glLockBuffers(__GLcontext *gc, GLuint mask)
{
    __GLdrawablePrivate *dp = gc->drawablePrivate;

    __GL_LOCK_DP(dp, gc);

    if ((*gc->buffers.lock.obtainSLock)(gc)) return;

    if (mask & __GL_FRONT_BUFFER_MASK) {
	__GLdrawableBuffer *db = gc->front->buf.drawableBuf;
	if (db->lock) {
	    (*db->lock)(db, dp);
	}
    }
    if (mask & __GL_BACK_BUFFER_MASK) {
	__GLdrawableBuffer *db = gc->back->buf.drawableBuf;
	if (db->lock) {
	    (*db->lock)(db, dp);
	}
    }
#if __GL_MAX_AUXBUFFERS > 0
    if (gc->modes.maxAuxBuffers > 0) {
	GLint i;

	for (i=0; i < gc->modes.maxAuxBuffers; i++) {
	    if (mask & __GL_AUX_BUFFER_MASK(i)) {
		if (dp->auxBuffer[i].lock) {
		    (*dp->auxBuffer[i].lock)(&dp->auxBuffer[i], dp);
		}
	    }
	}
    }
#endif /* __GL_MAX_AUXBUFFERS */

    if (mask & __GL_DEPTH_BUFFER_MASK) {
	if (dp->depthBuffer.lock) {
	    (*dp->depthBuffer.lock)(&dp->depthBuffer, dp);
	}
    }
    if (mask & __GL_STENCIL_BUFFER_MASK) {
	if (dp->stencilBuffer.lock) {
	    (*dp->stencilBuffer.lock)(&dp->stencilBuffer, dp);
	}
    }
    if (mask & __GL_ACCUM_BUFFER_MASK) {
	if (dp->accumBuffer.lock) {
	    (*dp->accumBuffer.lock)(&dp->accumBuffer, dp);
	}
    }

    if (gc->buffers.lock.lockDevice) (*gc->buffers.lock.lockDevice)(gc);
}

void __glUnlockBuffers(__GLcontext *gc, GLuint mask)
{
    __GLdrawablePrivate *dp = gc->drawablePrivate;

    if (gc->buffers.lock.sLockCnt == 1) {
	if (gc->buffers.lock.unlockDevice) 
	    (*gc->buffers.lock.unlockDevice)(gc);
    }

    if ((*gc->buffers.lock.releaseSLock)(gc) == GL_TRUE) {
	__GL_UNLOCK_DP(dp);
	return;
    }

    if (mask & __GL_FRONT_BUFFER_MASK) {
	__GLdrawableBuffer *db = gc->front->buf.drawableBuf;
	if (db->unlock) {
	    (*db->unlock)(db, dp);
	}
    }
    if (mask & __GL_BACK_BUFFER_MASK) {
	__GLdrawableBuffer *db = gc->back->buf.drawableBuf;
	if (db->unlock) {
	    (*db->unlock)(db, dp);
	}
    }
#if __GL_MAX_AUXBUFFERS > 0
    if (gc->modes.maxAuxBuffers > 0) {
	GLint i;

	for (i=0; i < gc->modes.maxAuxBuffers; i++) {
	    if (mask & __GL_AUX_BUFFER_MASK(i)) {
		if (dp->auxBuffer[i].unlock) {
		    (*dp->auxBuffer[i].unlock)(&dp->auxBuffer[i], dp);
		}
	    }
	}
    }
#endif /* __GL_MAX_AUXBUFFERS */

    if (mask & __GL_DEPTH_BUFFER_MASK) {
	if (dp->depthBuffer.unlock) {
	    (*dp->depthBuffer.unlock)(&dp->depthBuffer, dp);
	}
    }
    if (mask & __GL_STENCIL_BUFFER_MASK) {
	if (dp->stencilBuffer.unlock) {
	    (*dp->stencilBuffer.unlock)(&dp->stencilBuffer, dp);
	}
    }
    if (mask & __GL_ACCUM_BUFFER_MASK) {
	if (dp->accumBuffer.unlock) {
	    (*dp->accumBuffer.unlock)(&dp->accumBuffer, dp);
	}
    }

    __GL_UNLOCK_DP(dp);
}

/*
** routines to lock/unlock the renderable buffers.  Slow path.
*/
void __glLockRenderBuffers(__GLcontext *gc)
{
    __glLockBuffers(gc, gc->buffers.lock.renderBufferMask);
}

void __glUnlockRenderBuffers(__GLcontext *gc)
{
    __glUnlockBuffers(gc, gc->buffers.lock.renderBufferMask);
}

/*
** Certain fast paths for locking/unlocking renderable buffers.
*/
void __glLockColorBuffer(__GLcontext *gc)
{
    __GLbuffer *buf = gc->buffers.lock.bufferToLock;
    __GLdrawableBuffer *drawableBuf = buf->drawableBuf;
    __GLdrawablePrivate *dp = gc->drawablePrivate;

    __GL_LOCK_DP(dp, gc);

    if ((*gc->buffers.lock.obtainSLock)(gc)) return;

    assert(drawableBuf->lock);
    (*drawableBuf->lock)(drawableBuf, dp);

    if (gc->buffers.lock.lockDevice) (*gc->buffers.lock.lockDevice)(gc);
}

void __glUnlockColorBuffer(__GLcontext *gc)
{
    __GLbuffer *buf = gc->buffers.lock.bufferToLock;
    __GLdrawableBuffer *drawableBuf = buf->drawableBuf;
    __GLdrawablePrivate *dp = gc->drawablePrivate;

    if (gc->buffers.lock.sLockCnt == 1) {
	if (gc->buffers.lock.unlockDevice) 
	    (*gc->buffers.lock.unlockDevice)(gc);
    }
    if ((*gc->buffers.lock.releaseSLock)(gc) == GL_TRUE) {
	__GL_UNLOCK_DP(dp);
	return;
    }

    assert(drawableBuf->unlock);
    (*drawableBuf->unlock)(drawableBuf, dp);

    __GL_UNLOCK_DP(dp);
}

void __glLockColorBufferDepthBuffer(__GLcontext *gc)
{
    __GLbuffer *buf;
    __GLdrawableBuffer *drawableBuf;
    __GLdrawablePrivate *dp = gc->drawablePrivate;

    __GL_LOCK_DP(dp, gc);

    if ((*gc->buffers.lock.obtainSLock)(gc)) return;

    /* lock color */
    buf = gc->buffers.lock.bufferToLock;
    drawableBuf = buf->drawableBuf;
    assert (drawableBuf->lock);
    (*drawableBuf->lock)(drawableBuf, dp);

    /* lock depth */
    buf = &gc->depthBuffer.buf;
    drawableBuf = buf->drawableBuf;
    assert(drawableBuf->lock);
    (*drawableBuf->lock)(drawableBuf, dp);

    if (gc->buffers.lock.lockDevice) (*gc->buffers.lock.lockDevice)(gc);
}

void __glUnlockColorBufferDepthBuffer(__GLcontext *gc)
{
    __GLbuffer *buf;
    __GLdrawableBuffer *drawableBuf;
    __GLdrawablePrivate *dp = gc->drawablePrivate;

    if (gc->buffers.lock.sLockCnt == 1) {
	if (gc->buffers.lock.unlockDevice) 
	    (*gc->buffers.lock.unlockDevice)(gc);
    }
    if ((*gc->buffers.lock.releaseSLock)(gc) == GL_TRUE) {
	__GL_UNLOCK_DP(dp);
	return;
    }

    /* unlock color */
    buf = gc->buffers.lock.bufferToLock;
    drawableBuf = buf->drawableBuf;
    assert(drawableBuf->unlock);
    (*drawableBuf->unlock)(drawableBuf, dp);

    /* unlock depth */
    buf = &gc->depthBuffer.buf;
    drawableBuf = buf->drawableBuf;
    assert(drawableBuf->unlock);
    (*drawableBuf->unlock)(drawableBuf, dp);

    __GL_UNLOCK_DP(dp);
}
    

/* -------------------------------------------------------------------- */

GLboolean __glTestOwnership(__GLcontext *gc, GLint x, GLint y)
{
    __GLdrawablePrivate *dp = gc->drawablePrivate;
    __GLdrawableRegion *glRegion = &dp->ownershipRegion;
    GLint i, max;
    __GLregionRect *rect;

#ifdef __GL_ALIGNED_BUFFERS
    x -= gc->constants.viewportXAdjustOffset;
    y -= gc->constants.viewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
    x -= gc->constants.viewportXAdjust;
    y -= gc->constants.viewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */

    max = glRegion->numRects;
    for (i=0; i<max; i++) {
	rect = &(glRegion->rects[i]);
	if ((x >= rect->x0) && (y >= rect->y0) &&
	    (x <  rect->x1) && (y <  rect->y1)) {
	    /* inside one of the rects.  We pass */
	    return GL_TRUE;
	}
    }

    /* not in any rects.  We fail */
    return GL_FALSE;
}

/*
** free the buffer info stored in the drawable.
** This is called whenever the window will be destroyed
*/
void __glFreePrivate(__GLdrawablePrivate *dp)
{
    if (dp->frontBuffer.freePrivate) {
	(*dp->frontBuffer.freePrivate)(&dp->frontBuffer, dp);
    }
    if (dp->backBuffer.freePrivate) {
	(*dp->backBuffer.freePrivate)(&dp->backBuffer, dp);
    }
    if (dp->accumBuffer.freePrivate) {
	(*dp->accumBuffer.freePrivate)(&dp->accumBuffer, dp);
    }
    if (dp->depthBuffer.freePrivate) {
	(*dp->depthBuffer.freePrivate)(&dp->depthBuffer, dp);
    }
    if (dp->stencilBuffer.freePrivate) {
	(*dp->stencilBuffer.freePrivate)(&dp->stencilBuffer, dp);
    }
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    if (dp->auxBuffer) {
	int i;
	for (i = 0; i < dp->modes->numAuxBuffers; i++ ) {
	    if (dp->auxBuffer[i].freePrivate) {
		(*dp->auxBuffer[i].freePrivate)(&dp->auxBuffer[i], dp);
	    }
	}
	(*dp->free)(buffers->auxBuffer);
    }
#endif
    dp->private = NULL;
}

/*
** resize context size buffers
*/
GLboolean __glResizeWindow(__GLcontext *gc)
{
    GLint x, y;
    GLuint width, height;
    __GLdrawablePrivate *dp = gc->drawablePrivate;
    GLboolean flag, sb;
#ifdef __GL_ALIGNED_BUFFERS
    GLint xo, yo;
    GLint rasterOffX, rasterOffY;
#endif /* __GL_ALIGNED_BUFFERS */

    __GL_LOCK_DP(dp, gc);

    (*dp->getDrawableSize)(dp, &x, &y, &width, &height);

    gc->buffers.lock.accelBufferMask = dp->accelBufferMask;

#ifdef __GL_ALIGNED_BUFFERS
    xo = dp->xOffset;
    yo = dp->yOffset;

    /* offset to use for rasterPos */
    rasterOffX = xo - gc->constants.viewportXAlignOffset;
    rasterOffY = yo - gc->constants.viewportYAlignOffset;

    gc->constants.viewportXAlignOffset = xo;
    gc->constants.viewportYAlignOffset = yo;

    gc->constants.viewportXAdjustOffset = gc->constants.viewportXAdjust + xo;
    gc->constants.viewportYAdjustOffset = gc->constants.viewportYAdjust + yo;
    gc->constants.fviewportXAdjustOffset = 
	(__GLfloat) gc->constants.viewportXAdjustOffset;
    gc->constants.fviewportYAdjustOffset = 
	(__GLfloat) gc->constants.viewportYAdjustOffset;

    gc->state.current.rasterPos.window.x += rasterOffX;
    gc->state.current.rasterPos.window.y += rasterOffY;
#endif /* __GL_ALIGNED_BUFFERS */

    /* resize buffers to the new window size */
    flag = GL_TRUE;
    if (gc->frontBuffer.buf.resize) {
	sb = (*gc->frontBuffer.buf.resize)(&gc->frontBuffer.buf, 
					   x, y, width, height, 
					   __GL_FRONT_BUFFER_MASK);
	if (sb == GL_FALSE) flag = GL_FALSE;
    }

    if (gc->modes.doubleBufferMode) {
	if (gc->backBuffer.buf.resize) {
	    sb = (*gc->backBuffer.buf.resize)(&gc->backBuffer.buf, 
					      x, y, width, height, 
					      __GL_BACK_BUFFER_MASK);
	    if (sb == GL_FALSE) flag = GL_FALSE;
	}
    }

    if (gc->modes.haveAccumBuffer) {
	if (gc->accumBuffer.buf.resize) {
	    sb = (*gc->accumBuffer.buf.resize)(&gc->accumBuffer.buf, 
					       x, y, width, height, 
					       __GL_ACCUM_BUFFER_MASK);
	    if (sb == GL_FALSE) flag = GL_FALSE;
	}
    }

    if (gc->modes.haveDepthBuffer) {
	if (gc->depthBuffer.buf.resize) {
	    sb = (*gc->depthBuffer.buf.resize)(&gc->depthBuffer.buf, 
					       x, y, width, height, 
					       __GL_DEPTH_BUFFER_MASK);
	    if (sb == GL_FALSE) flag = GL_FALSE;
	}
    }

    if (gc->modes.haveStencilBuffer) {
	if (gc->stencilBuffer.buf.resize) {
	    sb = (*gc->stencilBuffer.buf.resize)(&gc->stencilBuffer.buf, 
						 x, y, width, height, 
						 __GL_STENCIL_BUFFER_MASK);
	    if (sb == GL_FALSE) flag = GL_FALSE;
	}
    }
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    if (gc->modes.numAuxBuffers > 0) {
	int i;
	for (i = 0; i < gc->modes.numAuxBuffers; i++ ) {
	    if (gc->auxBuffer[i].buf.resize) {
		sb = (*gc->auxBuffer[i].buf.resize)(&gc->auxBuffer[i].buf, 
						    x, y, width, height, 
						    __GL_AUX_BUFFER_MASK(i));
		if (sb == GL_FALSE) flag = GL_FALSE;
	    }
	}
    }
#endif /* __GL_NUMBER_OF_AUX_BUFFERS */

    /* recompute derived state which depends on the window size */
    if ((width != gc->constants.width) ||
        (height != gc->constants.height))
    {
	if (gc->constants.yInverted && height != gc->constants.height) {
	    gc->state.current.rasterPos.window.y += 
		height - gc->constants.height;
	}

	gc->constants.width = width;
	gc->constants.height = height;
    }

    (*gc->procs.computeClipBox)(gc);

    __GL_UNLOCK_DP(dp);

    return flag;
}

void __glMakeCurrentBuffers(__GLcontext *gc)
{
    __GLdrawablePrivate *dp = (*gc->imports.getDrawablePrivate)(gc);

    /* get the drawable private */
    gc->drawablePrivate = dp;

    /* update the call back so that we will free anything we allocate */
    dp->freePrivate = __glFreePrivate;

    /* load updated info from the drawable */
    if (gc->drawablePrivate->yInverted != gc->constants.yInverted) {
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_ALL);
    }

    if (gc->drawablePrivate->yInverted) {
	gc->constants.yInverted = GL_TRUE;
	gc->constants.ySign = -1;
    } else {
	gc->constants.yInverted = GL_FALSE;
	gc->constants.ySign = 1;
    }

    __GL_LOCK_DP(dp, gc);

    gc->constants.width = dp->width;
    gc->constants.height = dp->height;

    gc->buffers.lock.accelBufferMask = dp->accelBufferMask;

    (*gc->frontBuffer.buf.makeCurrent)(&gc->frontBuffer.buf,
				       &dp->frontBuffer, gc);
    if (gc->modes.doubleBufferMode) {
	(*gc->backBuffer.buf.makeCurrent)(&gc->backBuffer.buf,
					  &dp->backBuffer, gc);
    }
    if (gc->modes.haveAccumBuffer) {
	(*gc->accumBuffer.buf.makeCurrent)(&gc->accumBuffer.buf,
					   &dp->accumBuffer, gc);
    }
    if (gc->modes.haveDepthBuffer) {
	(*gc->depthBuffer.buf.makeCurrent)(&gc->depthBuffer.buf,
					   &dp->depthBuffer, gc);
    }
    if (gc->modes.haveStencilBuffer) {
	(*gc->stencilBuffer.buf.makeCurrent)(&gc->stencilBuffer.buf,
					     &dp->stencilBuffer, gc);
    }
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    if (dp->modes->numAuxBuffers > 0) {
	int i;
	for (i=0; i < dp->modes->numAuxBuffers; i++) {
	    (*gc->auxBuffer[i].buf.makeCurrent)(&gc->auxBuffer[i].buf,
						&dp->auxBuffer[i], gc);
	}
    }
#endif /* __GL_NUMBER_OF_AUX_BUFFERS */

    __GL_UNLOCK_DP(dp);
}

void __glLoseCurrentBuffers(__GLcontext *gc)
{
    __GLdrawablePrivate *dp = gc->drawablePrivate;

    __GL_LOCK_DP(dp, gc);

    (*gc->frontBuffer.buf.loseCurrent)(&gc->frontBuffer.buf, gc);
    if (gc->modes.doubleBufferMode) {
	(*gc->backBuffer.buf.loseCurrent)(&gc->backBuffer.buf, gc);
    }
    if (gc->modes.haveAccumBuffer) {
	(*gc->accumBuffer.buf.loseCurrent)(&gc->accumBuffer.buf, gc);
    }
    if (gc->modes.haveDepthBuffer) {
	(*gc->depthBuffer.buf.loseCurrent)(&gc->depthBuffer.buf, gc);
    }
    if (gc->modes.haveStencilBuffer) {
	(*gc->stencilBuffer.buf.loseCurrent)(&gc->stencilBuffer.buf, gc);
    }
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    if (gc->modes.numAuxBuffers) {
	int i;
	for (i=0; i < gc->modes.numAuxBuffers; i++) {
	    (*gc->auxBuffer[i].buf.loseCurrent)(&gc->auxBuffer[i].buf, gc);
	}
    }
#endif /* __GL_NUMBER_OF_AUX_BUFFERS */

    /* mark as not bound to the dp */
    gc->drawablePrivate = NULL;

    __GL_UNLOCK_DP(dp);
}

/* 
** this frees tables that are really on a per-context basis,
** but they may (structurally) belong to the buffers.
*/
void __glFreeBufData(__GLcontext *gc)
{
    if (gc->frontBuffer.buf.free) {
	(*gc->frontBuffer.buf.free)(&gc->frontBuffer.buf, gc);
    }
    if (gc->backBuffer.buf.free) {
	(*gc->frontBuffer.buf.free)(&gc->backBuffer.buf, gc);
    }
    if (gc->stencilBuffer.buf.free) {
	(*gc->stencilBuffer.buf.free)(&gc->stencilBuffer.buf, gc);
    }
    if (gc->depthBuffer.buf.free) {
	(*gc->depthBuffer.buf.free)(&gc->depthBuffer.buf, gc);
    }
    if (gc->accumBuffer.buf.free) {
	(*gc->accumBuffer.buf.free)(&gc->accumBuffer.buf, gc);
    }
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    if (gc->modes.numAuxBuffers > 0) {
	int i;
	for (i = 0; i < gc->modes.numAuxBuffers; i++) {
	    if (gc->auxBuffer[i].buf.free) {
		(*gc->auxBuffer[i].buf.free)(&gc->auxBuffer[i].buf, gc);
	    }
	}
    }
#endif /* __GL_NUMBER_OF_AUX_BUFFERS */
}

#if defined(__GL_ADDSWAPHINTRECT)
void GLAPI __glim_AddSwapHintRectWIN(GLint x, GLint y, 
				     GLsizei width, GLsizei height)
{
    __GLdrawablePrivate *dp;
    GLint dpWidth, dpHeight;
    GLint x0, x1, y0, y1;
    __GL_SETUP_NOT_IN_BEGIN();

    dp = gc->drawablePrivate;

    x0 = x;
    y0 = y;
    x1 = x + width;
    y1 = y + height;

    dpWidth = dp->width;
    dpHeight = dp->height;

    x0 = MAX(x0, 0); x0 = MIN(x0, dpWidth);
    y0 = MAX(y0, 0); y0 = MIN(y0, dpHeight);
    x1 = MAX(x1, 0); x1 = MIN(x1, dpWidth);
    y1 = MAX(y1, 0); y1 = MIN(y1, dpHeight);

    /* addswaphintrect only makes sense for dbuf drawables. */
    if (gc->modes.doubleBufferMode) {
	if ((*gc->drawablePrivate->addSwapRect)(dp, x0, y0, x1-x0, y1-y0) 
	    == GL_FALSE) {
	    /* There was an error trying to add a swaprecthint */
	    /* XXX: unspecified behavior.  Any better ideas? */
	    __glSetError(GL_OUT_OF_MEMORY);
	    return;
	}
    }
}

#endif /* __GL_ADDSWAPHINTRECT */
