#ifndef __gl_buffers_h_
#define	__gl_buffers_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/buffers.h#8 $
*/

#include "GL/internal/glcore.h"
#include "render.h"
#include "procs.h"

/*
** This structure contains everything that has to do with 
** buffer/system locks
*/
typedef struct __GLlockMachineRec {
    /* a lock count */
    GLint lockCnt;

    /* which buffer to lock (used for fastpath locking */
    __GLbuffer *bufferToLock;

    /* a mask of which buffers are renderable */
    GLuint renderBufferMask;

    /* a mask of which buffer is the current draw(read) buffer */
    GLuint drawBufferMask;
    GLuint readBufferMask;

    /* a mask telling us which buffers are in hw */
    GLuint accelBufferMask;

    /*
    ** lock/unlock buffers
    */
    void (*lockBuffers)(__GLcontext *gc, GLuint mask);
    void (*unlockBuffers)(__GLcontext *gc, GLuint mask);
    void (*lockRenderBuffers)(__GLcontext *gc);
    void (*unlockRenderBuffers)(__GLcontext *gc);

    /*
    ** functions to obtain/release a system lock.  The lock function
    ** waits till the lock is obtained, and returned a boolean indicating
    ** if something changed since the last time we had the lock.  This
    ** can be used to context switch the hw, or optimize performing a
    ** buffer lock.
    */
    GLboolean (*obtainSLock)(__GLcontext *gc);
    GLboolean (*releaseSLock)(__GLcontext *gc);
    GLboolean sLockCnt;

    /*
    ** Context switch the hardware in/out
    */
    void (*lockDevice)(__GLcontext *gc);
    void (*unlockDevice)(__GLcontext *gc);

} __GLlockMachine;

typedef struct __GLbufferMachineRec {
    /*
    ** GL_TRUE if store procs need to call gc->front->store and 
    ** gc->back->store in order to store one fragment (only TRUE if 
    ** drawBuffer is GL_FRONT_AND_BACK).  This is needed because many
    ** frame buffers can draw to both front and back under some conditions
    ** (like when not blending), but not under other conditions.
    */
    GLboolean doubleStore;

    /*
    ** This structure contains all the lock information.  These locks
    ** are buffer locks as well as hw locks used for context switching
    ** the hw
    */
    __GLlockMachine lock;

} __GLbufferMachine;

/*
** Defines for locking/unlocking buffers.
**
** If we are using the system lock (__GL_USE_SLOCK), then we have to lock
** all buffers bwfore we take the lock, since there is no change we can
** lock other ones later (the lock will proevent us from doing that in the
** same way it will prevent other processes from taking the locks.
**
** If we are not using the system lock, we will only lock the buffers
** we need to.
*/

#ifdef __GL_USE_SLOCK

/* generic lock/unlock given a buffer mask */
#define __GL_LOCK_BUFFER_MASK(gc, mask) \
    (*(gc)->buffers.lock.lockBuffers)(gc, __GL_ALL_BUFFER_MASK);

#define __GL_UNLOCK_BUFFER_MASK(gc, mask) \
    (*(gc)->buffers.lock.unlockBuffers)(gc, __GL_ALL_BUFFER_MASK);

/* all the other ones are aliases for the above */
#define __GL_LOCK_RENDER_BUFFERS(gc) __GL_LOCK_BUFFER_MASK(gc, 0)
#define __GL_UNLOCK_RENDER_BUFFERS(gc) __GL_UNLOCK_BUFFER_MASK(gc, 0)
#define __GL_LOCK_READ_BUFFERS(gc) __GL_LOCK_BUFFER_MASK(gc, 0)
#define __GL_UNLOCK_READ_BUFFERS(gc) __GL_UNLOCK_BUFFER_MASK(gc, 0)
#define __GL_LOCK_BUFFER(gc, buf, db) __GL_LOCK_BUFFER_MASK(gc, 0)
#define __GL_UNLOCK_BUFFER(gc, buf, db) __GL_UNLOCK_BUFFER_MASK(gc, 0)
#define __GL_LOCK_PXL_COPY_BUFFERS(gc, format) __GL_LOCK_BUFFER_MASK(gc, 0)
#define __GL_UNLOCK_PXL_COPY_BUFFERS(gc, format) __GL_UNLOCK_BUFFER_MASK(gc, 0)
#define __GL_LOCK_PXL_DRAW_BUFFER(gc, format) __GL_LOCK_BUFFER_MASK(gc, 0)
#define __GL_UNLOCK_PXL_DRAW_BUFFER(gc, format) __GL_UNLOCK_BUFFER_MASK(gc, 0)
#define __GL_LOCK_PXL_READ_BUFFER(gc, format) __GL_LOCK_BUFFER_MASK(gc, 0)
#define __GL_UNLOCK_PXL_READ_BUFFER(gc, format) __GL_UNLOCK_BUFFER_MASK(gc, 0)

#else /* __GL_USE_SLOCK */

/* generic lock/unlock given a buffer mask */
#define __GL_LOCK_BUFFER_MASK(gc, mask) \
    (*(gc)->buffers.lock.lockBuffers)(gc, mask);

#define __GL_UNLOCK_BUFFER_MASK(gc, mask) \
    (*(gc)->buffers.lock.unlockBuffers)(gc, mask);

/* quick lock/unlock for the rendering buffers */
#define __GL_LOCK_RENDER_BUFFERS(gc) \
    if ( ++(gc)->buffers.lock.lockCnt == 1 && \
	 (gc)->buffers.lock.lockRenderBuffers) { \
	(*(gc)->buffers.lock.lockRenderBuffers)(gc); \
    }

#define __GL_UNLOCK_RENDER_BUFFERS(gc) \
    if ( --(gc)->buffers.lock.lockCnt == 0 && \
	 (gc)->buffers.lock.unlockRenderBuffers) { \
	(*(gc)->buffers.lock.unlockRenderBuffers)(gc); \
    }

/* lock/unlock for the read buffer */
#define __GL_LOCK_READ_BUFFER(gc) \
    __GL_LOCK_BUFFER_MASK(gc, gc->buffers.readBufferMask);

#define __GL_UNLOCK_READ_BUFFER(gc) \
    __GL_UNLOCK_BUFFER_MASK(gc, gc->buffers.readBufferMask);

/* lock/unlock for a specific buffer */
#define __GL_LOCK_BUFFER(gc, buf, db) { \
    if ((buf)->drawableBuf->lock) { \
        (*(buf)->drawableBuf->lock)(db, (gc)->drawablePrivate); \
    } \
    if ((gc)->buffers.lock.lockDevice) (*(gc)->buffers.lock.lockDevice)(gc); \
}

#define __GL_UNLOCK_BUFFER(gc, buf, db) { \
    if ((gc)->buffers.lock.unlockDevice) (*(gc)->buffers.lock.unlockDevice)(gc); \
    if ((buf)->drawableBuf->unlock) { \
	(*(buf)->drawableBuf->unlock)(db, (gc)->drawablePrivate); \
    } \
}

#define __GL_LOCK_PXL_COPY_BUFFERS(gc, format) { \
    GLuint mask = gc->buffers.lock.renderBufferMask | gc->buffers.lock.drawBufferMask; \
    if (format == GL_DEPTH_COMPONENT) mask |= __GL_DEPTH_BUFFER_MASK; \
    if (format == GL_STENCIL_INDEX) mask = __GL_STENCIL_BUFFER_MASK; \
    __GL_LOCK_BUFFER_MASK(gc, mask); \
}

#define __GL_UNLOCK_PXL_COPY_BUFFERS(gc, format) { \
    GLuint mask = gc->buffers.lock.renderBufferMask | gc->buffers.lock.drawBufferMask; \
    if (format == GL_DEPTH_COMPONENT) mask |= __GL_DEPTH_BUFFER_MASK; \
    if (format == GL_STENCIL_INDEX) mask = __GL_STENCIL_BUFFER_MASK; \
    __GL_UNLOCK_BUFFER_MASK(gc, mask); \
}

#define __GL_LOCK_PXL_DRAW_BUFFER(gc, format) { \
    GLuint mask = gc->buffers.lock.renderBufferMask; \
    if (format == GL_DEPTH_COMPONENT) mask |= __GL_DEPTH_BUFFER_MASK; \
    if (format == GL_STENCIL_INDEX) mask = __GL_STENCIL_BUFFER_MASK; \
    __GL_LOCK_BUFFER_MASK(gc, mask); \
}

#define __GL_UNLOCK_PXL_DRAW_BUFFER(gc, format) { \
    GLuint mask = gc->buffers.lock.renderBufferMask; \
    if (format == GL_DEPTH_COMPONENT) mask |= __GL_DEPTH_BUFFER_MASK; \
    if (format == GL_STENCIL_INDEX) mask = __GL_STENCIL_BUFFER_MASK; \
    __GL_UNLOCK_BUFFER_MASK(gc, mask); \
}

#define __GL_LOCK_PXL_READ_BUFFER(gc, format) { \
    GLuint mask = gc->buffers.lock.readBufferMask; \
    if (format == GL_DEPTH_COMPONENT) mask = __GL_DEPTH_BUFFER_MASK; \
    if (format == GL_STENCIL_INDEX) mask = __GL_STENCIL_BUFFER_MASK; \
    __GL_LOCK_BUFFER_MASK(gc, mask); \
}

#define __GL_UNLOCK_PXL_READ_BUFFER(gc, format) { \
    GLuint mask = gc->buffers.lock.readBufferMask; \
    if (format == GL_DEPTH_COMPONENT) mask = __GL_DEPTH_BUFFER_MASK; \
    if (format == GL_STENCIL_INDEX) mask = __GL_STENCIL_BUFFER_MASK; \
    __GL_UNLOCK_BUFFER_MASK(gc, mask); \
}
#endif /* __GL_USE_SLOCK */

/************************************************************************/

/*
** Generic buffer description.  This description is used for software
** and hardware buffers of all kinds.
*/
struct __GLbufferRec {
    /*
    ** Which context is using this buffer.
    */
    __GLcontext *gc;

    /*
    ** Pointer to drawableBuf, where all the buffer size (and other)
    ** information is being kept
    */
    __GLdrawableBuffer *drawableBuf;

    /*
    ** Private pointer used by particular buffer implementations to
    ** contain special data not supported by this generic structure.
    ** This is for "hardware" data.
    */
    void *other;

    /*
    ** The buffer structure is conceptually an "object".
    **
    ** There are two parts on every buffer:
    ** - The drawable buffer contains the buffer memory itself, together
    **   with buffer management (allocation and freeing) and other
    **   miscellaneous operations.
    ** - The context buffer contains context-side specific information
    **   to the buffer.  Buffer scales, etc. are an example.
    **
    ** This separation was necessery to allow for MT-safe operation and 
    ** to allow multiple contexts to be bound to the same drawable at 
    ** the same time. Because of that, the drawable buffer contains 
    ** buffer information that is expected to be common between all 
    ** contexts that share that drawable.  The context buffer 
    ** contains context-private data.
    **
    ** In order to make sure that the drawable buffer is accessed in an
    ** MT-safe manner, locks are used to arbitrate access.  There is also
    ** "constant" or mostly-constant data in the drawable that is allowed 
    ** to be accessed without locks, since there is no worry that it is
    ** going to be changed.
    **
    ** Context buffer destruction does not imply drawable buffer destruction.
    ** A context buffer may be destroyed when a context is being freed, but
    ** the same drawable could be bound to another context.  Using the same
    ** logic, a context buffer may exist without a corresponding drawable
    ** buffer.  This can happen when a context has not been made current
    ** to any drawable.  The drawable buffer and the context buffer are
    ** therefore tightly coupled, but not attached across creation and
    ** destruction.
    */

    /* resize: resize and move a buffer */
    GLboolean (*resize)(__GLbuffer *buf,
			GLint x, GLint y, GLint width, GLint height,
			GLuint bufferMask);

    /* make a context current / lose a context */
    void (*makeCurrent)(__GLbuffer *buf, __GLdrawableBuffer *drawableBuf,
			__GLcontext *gc);
    void (*loseCurrent)(__GLbuffer *buf, __GLcontext *gc);

    /* Free context-side data ("object" destructor) */
    void (*free)(__GLbuffer *buf, __GLcontext *gc);
};

/*
** Generic address macros for a buffer.
** The input coordinates x,y are biased by the x & y viewport
** adjusts in gc->transform, and thus they need to be de-adjusted
** here.
*/

#define	__GL_FB_ADDRESS(gc,fb,drawableBuf,cast,x,y) \
    (cast (((GLubyte *) (drawableBuf)->base) \
	+ (((y) - (gc)->constants.viewportYAdjust) \
            * (drawableBuf)->outerWidth \
	   + (x) - (gc)->constants.viewportXAdjust) \
	* (drawableBuf)->elementSize))

/************************************************************************/

struct __GLcolorBufferRec {
    __GLbuffer buf;

    GLint redMax;
    GLint greenMax;
    GLint blueMax;

    GLboolean needColorFragmentOps;

    GLubyte *alphaTestFuncTable;

    /*
    ** Color component scale factors.  Given a component value between
    ** zero and one, this scales the component into a zero-N value
    ** which is suitable for usage in the color buffer.  Note that these
    ** values are not necessarily the same as the max values above,
    ** which define precise bit ranges for the buffer.  These values
    ** are never zero, for instance.
    **/
    __GLfloat redScale;
    __GLfloat greenScale;
    __GLfloat blueScale;

    /* Integer versions of above */
    GLint iRedScale;
    GLint iGreenScale;
    GLint iBlueScale;

    /* Used primarily by pixmap code */
    GLint redShift;
    GLint greenShift;
    GLint blueShift;
    GLint alphaShift;

    /*
    ** Alpha is treated a little bit differently.  alphaScale and
    ** iAlphaScale are used to define a range of alpha values that are
    ** generated during various rendering steps.  These values will then
    ** be used as indices into a lookup table to see if the alpha test
    ** passes or not.  Because of this, the number should be fairly large
    ** (e.g., one is not good enough).
    */
    __GLfloat alphaScale;
    GLint iAlphaScale;

    __GLfloat oneOverRedScale;
    __GLfloat oneOverGreenScale;
    __GLfloat oneOverBlueScale;
    __GLfloat oneOverAlphaScale;

    /*
    ** Color mask state for the buffer.  When writemasking is enabled
    ** the source and dest mask will contain depth depedent masking.
    */
    GLuint sourceMask, destMask;

    /*
    ** This function updates the internal procedure pointers based
    ** on a state change in the context.
    */
    void (*pick)(__GLcontext *gc, __GLcolorBuffer *cfb);

    /*
    ** Get the display masks for each primary color
    */
    GLint (*getDisplayMasks)(GLint *rMask, GLint *gMask, 
			     GLint *bMask, GLint *aMask);

    /*
    ** Store a fragment into the buffer.  For color buffers, the
    ** procedure will optionally dither, writemask, blend and logic op
    ** the fragment before final storage.
    */
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);

    /*
    ** Fetch a color from the buffer.  This returns the r, g, b and a
    ** values for an RGB buffer.  For an index buffer the "r" value
    ** returned is the index.
    */
    void (*fetch)(__GLcolorBuffer *cfb, GLint x, GLint y,
		  __GLcolor *result);

    /*
    ** Similar to fetch, except that the data is always read from
    ** the current read buffer, not from the current draw buffer.
    */
    void (*readColor)(__GLcolorBuffer *cfb, GLint x, GLint y,
		      __GLcolor *result);
    GLboolean (*readSpan)(__GLcolorBuffer *cfb, GLint x, GLint y,
		          __GLcolor *results, GLint w);

    /*
    ** Return a span of data from the accumulation buffer into the
    ** color buffer(s), multiplying by "scale" before storage.
    */
    void (*returnSpan)(__GLcolorBuffer *cfb, GLint x, GLint y,
		       const __GLaccumCell *acbuf, __GLfloat scale, GLint w);

    /*
    ** Store a span (line) of colors into the color buffer.  A minimal
    ** implementation need only copy the values directly into
    ** the framebuffer, assuming that the PickSpanProcs is providing
    ** software implementations of all of the modes.
    */
    __GLspanFunc storeSpan;
    __GLstippledSpanFunc storeStippledSpan;
    __GLspanFunc storeLine; 
    __GLstippledSpanFunc storeStippledLine;

    /*
    ** Read a span (line) of colors from the color buffer.  The returned
    ** format is in the same format used for storage.
    */
    __GLspanFunc fetchSpan;
    __GLstippledSpanFunc fetchStippledSpan;
    __GLspanFunc fetchLine;
    __GLstippledSpanFunc fetchStippledLine;

#ifdef __GL_FR_RAST
    /*
    ** Framebuffer characterization for rasterization code.
    ** See fr_fbtype.h for interpretation.
    */
    GLint fbtype;
#endif

    /*
    ** Clear the scissor area of the color buffer, clipped to
    ** the window size.  Apply dithering if enabled.
    */
    void (*clear)(__GLcolorBuffer *cfb);
    /*
    ** Clear the given rectangle of the color buffer.
    */
    void (*clearRect)(__GLcolorBuffer *cfb,
			GLint x0, GLint y0, GLint x1, GLint y1);

    /*
    ** Private pointer used by particular buffer implementations to
    ** contain special data not supported by this generic structure.
    */
    void *other;
};

/* generic span read routine */
extern GLboolean __glReadSpan(__GLcolorBuffer *cfb, GLint x, GLint y,
			      __GLcolor *results, GLint w);

/* generic accum return span routine */
extern void __glReturnSpan(__GLcolorBuffer *cfb, GLint x, GLint y,
			   const __GLaccumCell *ac, __GLfloat scale,
			   GLint w);

/* generic span fetch routine */
extern GLboolean __glFetchSpan(__GLcontext *gc);

/************************************************************************/

struct __GLdepthBufferRec {
    __GLbuffer buf;

    GLuint writeMask;

    /*
    ** Scale factor used to convert users ZValues (0.0 to 1.0, inclusive)
    ** into this depth buffers range.
    */
    GLuint scale;

    GLenum testFunc;

    /*
    ** the minimum resolvable difference for PolygonOffset
    */
    __GLfloat minResolution;

    /*
    ** a function that goes through the zbuffer and adjusts all the
    ** entries to a consistent range.  It is used in nasty
    ** situations, like changing the direction of the z compare
    ** (LESS to GREATER, for example), or altering the scissoring rect.
    */
    void (*fixZBuffer)(__GLcontext *gc, __GLdepthBuffer *dfb,
			GLuint destRange );

    /*
    ** This function updates the internal procedure pointers based
    ** on a state change in the context.
    */
    void (*pick)(__GLcontext *gc, __GLdepthBuffer *dfb, GLint depthIndex);

    /*
    ** Attempt to update the depth buffer using z.  If the depth function
    ** passes then the depth buffer is updated and True is returned,
    ** otherwise False is returned.  The caller is responsible for
    ** updating the stencil buffer.
    */
    GLboolean (*store)(__GLdepthBuffer *dfb, GLint x, GLint y, __GLzValue z);

    /*
    ** Clear the scissor area of the buffer clipped to the window
    ** area.  No other modes apply.
    */
    void (*clear)(__GLdepthBuffer *dfb);

    /*
    ** Direct access routines used by ReadPixels(), WritePixels(), 
    ** CopyPixels().
    */
    GLboolean (*store2)(__GLdepthBuffer *dfb, GLint x, GLint y, __GLzValue z);
    __GLzValue (*fetch)(__GLdepthBuffer *dfb, GLint x, GLint y);
};

/*
** the drawable version
*/
typedef struct __GLdrawableDepthBufferRec __GLdrawableDepthBuffer;
struct __GLdrawableDepthBufferRec {
    /*
    ** Used for fast depth clears.  The high order bits for each depth
    ** value contain the frame count. cnt is what should be or-ed to the
    ** depth value to be stored in the depth buffer.  For example, for
    ** 8 count bits, cnt is 0x**000000).
    */
    GLuint cnt;
    /*
    ** When scissoring is enabled, there is a different count for the
    ** part of the frame buffer inside the scissor rect from the count
    ** outside the scissor rect.  cnt always contains the count for the
    ** area that is renderable.  scissorCnt contains the count for the
    ** frame buffer outside the scissor rect, when scissoring is enabled.
    */
    GLuint scissorCnt;
    /*
    ** How many fractional bits (bits not stored in the frame buffer) are
    ** used.  Example: we use a 32 bit z buffer.  numFracBits is 8.
    ** Then, iteration is performed at 32 bits, but only the 24 MSB bits
    ** are stored, shifted right by 8.  The "hole" of 8 bits on the MSB
    ** side will be used for frame count.
    */
    GLint numFracBits;
    /*
    ** a write mask for the z buffer.  Similar to gc->depthBuffer.scale,
    ** but not quite.  zDepthMask must always be in the form 2^n - 1.
    ** gc->depthBuffer.scale does not have to.  This is because we are
    ** performing bitwise AND using zDepthMask.
    */
    GLuint zDepthMask;
    /*
    ** Direction of z buffer clears.  Actual value to be added to 'cnt'
    ** at each z clear.  It is either 0x01000000 or 0xff000000 if
    ** numFracBits is 8.  If set to zero, no fast z clears are performed.
    */
    GLuint zClearInc;
    /*
    ** This is set if something has happened that forces the implementation
    ** to perform a full z buffer clear.  A window resize is an example.
    */
    GLboolean fullZClear;
    GLboolean allowPartialZClear;
    /*
    ** An indicator of normal or inverted z buffer range.  If invertZRange
    ** is GL_TRUE then testFunc is the inverse of state.depth.testFunc.
    ** GL_FALSE means they are the same.
    */
    GLboolean invertZRange;
};

#define __GL_DEPTH_DP(buffer) \
	((__GLdrawableDepthBuffer *)((buffer)->buf.drawableBuf->private))

#define	__GL_DEPTH_ADDR(a,b,c,d,e,f) __GL_FB_ADDRESS(a,b,c,d,e,f)

/************************************************************************/

struct __GLstencilBufferRec {
    __GLbuffer buf;

    /*
    ** Stencil test lookup table.  The stencil buffer value is masked
    ** against the stencil mask and then used as an index into this
    ** table which contains either GL_TRUE or GL_FALSE for the
    ** index.
    */
    GLboolean *testFuncTable;

    /*
    ** Stencil op tables.  These tables contain the new stencil buffer
    ** value given the old stencil buffer value as an index.
    */
    __GLstencilCell *failOpTable;
    __GLstencilCell *depthFailOpTable;
    __GLstencilCell *depthPassOpTable;

    /*
    ** This function updates the internal procedure pointers based
    ** on a state change in the context.
    */
    void (*pick)(__GLcontext *gc, __GLstencilBuffer *sfb);

    /*
    ** Store a fragment into the buffer.
    */
    void (*store)(__GLstencilBuffer *sfb, GLint x, GLint y,
		  GLint value);

    /* 
    ** Fetch a value.
    */
    GLint (*fetch)(__GLstencilBuffer *sfb, GLint x, GLint y);

    /*
    ** Return GL_TRUE if the stencil test passes.
    */
    GLboolean (*testFunc)(__GLstencilBuffer *sfb, GLint x, GLint y);

    /*
    ** Apply the stencil ops to this position.
    */
    void (*failOp)(__GLstencilBuffer *sfb, GLint x, GLint y);
    void (*passDepthFailOp)(__GLstencilBuffer *sfb, GLint x, GLint y);
    void (*depthPassOp)(__GLstencilBuffer *sfb, GLint x, GLint y);

    /*
    ** Clear the scissor area of the buffer clipped to the window
    ** area.  No other modes apply.
    */
    void (*clear)(__GLstencilBuffer *sfb);
};

#define	__GL_STENCIL_ADDR(a,b,c,d,e,f) __GL_FB_ADDRESS(a,b,c,d,e,f)

/************************************************************************/

struct __GLaccumBufferRec {
    __GLbuffer buf;

    /*
    ** Scaling factors to convert from color buffer values to accum
    ** buffer values.
    */
    __GLfloat redScale;
    __GLfloat greenScale;
    __GLfloat blueScale;
    __GLfloat alphaScale;

    __GLfloat oneOverRedScale;
    __GLfloat oneOverGreenScale;
    __GLfloat oneOverBlueScale;
    __GLfloat oneOverAlphaScale;

    /*
    ** This function updates the internal procedure pointers based
    ** on a state change in the context.
    */
    void (*pick)(__GLcontext *gc, __GLaccumBuffer *afb);

    /*
    ** Clear a rectangular region in the buffer.  The scissor area is
    ** cleared.
    */
    void (*clear)(__GLaccumBuffer *afb);

    /*
    ** Accumulate data into the accum buffer.
    */
    void (*accumulate)(__GLaccumBuffer *afb, __GLfloat value);

    /*
    ** Load data into the accum buffer.
    */
    void (*load)(__GLaccumBuffer *afb, __GLfloat value);

    /*
    ** Return data from the accum buffer to the current framebuffer.
    */
    void (*ret)(__GLaccumBuffer *afb, __GLfloat value);

    /*
    ** Multiply the accum buffer by the value.
    */
    void (*mult)(__GLaccumBuffer *afb, __GLfloat value);


    /*
    ** Add the value to the accum buffer.
    */
    void (*add)(__GLaccumBuffer *afb, __GLfloat value);
};

#define	__GL_ACCUM_ADDRESS(a,b,c,d,e,f) __GL_FB_ADDRESS(a,b,c,d,e,f)

/************************************************************************/

/* buffer specific routines */

extern void __glInitCI(__GLcolorBuffer *cfb, __GLcontext *gc );
extern void __glInitRGB(__GLcolorBuffer *cfb, __GLcontext *gc );

extern void __glInitAccum64(__GLaccumBuffer *afb, __GLcontext *gc );

extern void __glInitStencil8(__GLstencilBuffer *sfb, __GLcontext *gc );

extern void __glInitDepth(__GLdepthBuffer *dfb, __GLcontext *gc );
extern void __glInitDepth16(__GLdepthBuffer *dfb, __GLcontext *gc );
extern void __glInitDepth24(__GLdepthBuffer *dfb, __GLcontext *gc );
extern void __glInitDepth32(__GLdepthBuffer *dfb, __GLcontext *gc );

extern void __glValidateZCount(__GLdepthBuffer *dfb);

/************************************************************************/

/* generic buffer routines */

extern void __glInitBuffer(__GLbuffer *buf, __GLcontext *gc);
extern void __glMakeCurrentBuffer(__GLbuffer *buf,
				  __GLdrawableBuffer *drawableBuf,
				  __GLcontext *gc);
extern void __glLoseCurrentBuffer(__GLbuffer *buf, __GLcontext *gc);
extern void __glFreeBuffer(__GLdrawableBuffer *drawableBuf,
			    __GLdrawablePrivate *dp);

extern void __glLockBuffers(__GLcontext *gc, GLuint mask);
extern void __glUnlockBuffers(__GLcontext *gc, GLuint mask);
extern void __glLockRenderBuffers(__GLcontext *gc);
extern void __glUnlockRenderBuffers(__GLcontext *gc);
extern void __glLockColorBuffer(__GLcontext *gc);
extern void __glUnlockColorBuffer(__GLcontext *gc);
extern void __glLockColorBufferDepthBuffer(__GLcontext *gc);
extern void __glUnlockColorBufferDepthBuffer(__GLcontext *gc);

extern GLboolean __glTestOwnership(__GLcontext *gc, GLint x, GLint y);

#endif /* __gl_buffers_h_ */
