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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_sten.c#8 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "g_imports.h"

static GLint Fetch(__GLstencilBuffer *sfb, GLint x, GLint y)
{
    __GLcontext *gc = sfb->buf.gc;
    __GLdrawableBuffer *db = sfb->buf.drawableBuf;
    __GLstencilCell *fb;

    fb = __GL_STENCIL_ADDR(gc, sfb, db, (__GLstencilCell*), x, y);

    return fb[0];
}

static void Store(__GLstencilBuffer *sfb, GLint x, GLint y,GLint v)
{
    __GLcontext *gc = sfb->buf.gc;
    __GLdrawableBuffer *db = sfb->buf.drawableBuf;
    __GLstencilCell *fb;

    fb = __GL_STENCIL_ADDR(gc, sfb, db, (__GLstencilCell*), x, y);

    fb[0] = (__GLstencilCell) ((v & sfb->buf.gc->state.stencil.writeMask)
		       | (fb[0] & ~sfb->buf.gc->state.stencil.writeMask));
}

static GLboolean TestFunc(__GLstencilBuffer *sfb, GLint x, GLint y)
{
    __GLcontext *gc = sfb->buf.gc;
    __GLdrawableBuffer *db = sfb->buf.drawableBuf;
    __GLstencilCell *fb;

    fb = __GL_STENCIL_ADDR(gc, sfb, db, (__GLstencilCell*), x, y);

    return sfb->testFuncTable[fb[0] & sfb->buf.gc->state.stencil.mask];
}

static void FailOp(__GLstencilBuffer *sfb, GLint x, GLint y)
{
    __GLcontext *gc = sfb->buf.gc;
    __GLdrawableBuffer *db = sfb->buf.drawableBuf;
    __GLstencilCell *fb;

    fb = __GL_STENCIL_ADDR(gc, sfb, db, (__GLstencilCell*), x, y);

    fb[0] = sfb->failOpTable[fb[0]];
}

static void PassDepthFailOp(__GLstencilBuffer *sfb, GLint x, GLint y)
{
    __GLcontext *gc = sfb->buf.gc;
    __GLdrawableBuffer *db = sfb->buf.drawableBuf;
    __GLstencilCell *fb;

    fb = __GL_STENCIL_ADDR(gc, sfb, db, (__GLstencilCell*), x, y);

    fb[0] = sfb->depthFailOpTable[fb[0]];
}

static void DepthPassOp(__GLstencilBuffer *sfb, GLint x, GLint y)
{
    __GLcontext *gc = sfb->buf.gc;
    __GLdrawableBuffer *db = sfb->buf.drawableBuf;
    __GLstencilCell *fb;

    fb = __GL_STENCIL_ADDR(gc, sfb, db, (__GLstencilCell*), x, y);

    fb[0] = sfb->depthPassOpTable[fb[0]];
}

/************************************************************************/

static void Clear(__GLstencilBuffer *sfb)
{
    __GLcontext *gc = sfb->buf.gc;
    __GLdrawableBuffer *db;
    __GLstencilCell *fb;
    GLint x, y, x1, y1, skip, width, w8, w1;
    __GLstencilCell sten = gc->state.stencil.clear;

    __GL_LOCK_BUFFER_MASK(gc, __GL_STENCIL_BUFFER_MASK);

    x = gc->transform.clipX0;
    y = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;
    if (((width = x1 - x) == 0) || (y1 - y == 0)) {
	__GL_UNLOCK_BUFFER_MASK(gc, __GL_STENCIL_BUFFER_MASK);
	return;
    }

    db = sfb->buf.drawableBuf;

    fb = __GL_STENCIL_ADDR(gc, sfb, db, (__GLstencilCell*), x, y);

    skip = sfb->buf.drawableBuf->outerWidth - width;
    w1 = width & 7;
    w8 = width >> 3;
    if (gc->state.stencil.writeMask == __GL_MAX_STENCIL_VALUE) {
	for (; y < y1; y++) {
	    width = w8;
	    while (--width >= 0) {
		fb[0] = sten; fb[1] = sten; fb[2] = sten; fb[3] = sten;
		fb[4] = sten; fb[5] = sten; fb[6] = sten; fb[7] = sten;
		fb += 8;
	    }
	    width = w1;
	    while (--width >= 0) {
		*fb++ = sten;
	    }
	    fb += skip;
	}
    } else {
	GLint mask;

	mask = gc->state.stencil.writeMask;
	sten = sten & mask;
	mask = ~mask;

	for (; y < y1; y++) {
	    width = w8;
	    while (--width >= 0) {
		fb[0] = (fb[0] & mask) | (sten); 
		fb[1] = (fb[1] & mask) | (sten); 
		fb[2] = (fb[2] & mask) | (sten); 
		fb[3] = (fb[3] & mask) | (sten); 
		fb[4] = (fb[4] & mask) | (sten); 
		fb[5] = (fb[5] & mask) | (sten); 
		fb[6] = (fb[6] & mask) | (sten); 
		fb[7] = (fb[7] & mask) | (sten); 
		fb += 8;
	    }
	    width = w1;
	    while (--width >= 0) {
		fb[0] = (fb[0] & mask) | (sten);
		fb++;
	    }
	    fb += skip;
	}
    }

    __GL_UNLOCK_BUFFER_MASK(gc, __GL_STENCIL_BUFFER_MASK);
}

/************************************************************************/

static void buildOpTable(__GLcontext *gc, __GLstencilCell *tp, GLenum op,
			 __GLstencilCell reference, __GLstencilCell writeMask)
{
    GLuint i;
    __GLstencilCell newValue = 0;
    __GLstencilCell notWriteMask = ~writeMask;

    for (i = 0; i < (GLuint)__GL_STENCIL_RANGE; i++) {
	switch (op) {
	  case GL_KEEP:		newValue = i; break;
	  case GL_ZERO:		newValue = 0; break;
	  case GL_REPLACE:	newValue = reference; break;
	  case GL_INVERT:	newValue = ~i; break;
	  case GL_INCR:
	    /* Clamp so no overflow occurs */
	    if (i == (GLuint)__GL_MAX_STENCIL_VALUE) {
		newValue = i;
	    } else {
		newValue = i + 1;
	    }
	    break;
	  case GL_DECR:
	    /* Clamp so no underflow occurs */
	    if (i == 0) {
		newValue = 0;
	    } else {
		newValue = i - 1;
	    }
	    break;
	}
	*tp++ = (i & notWriteMask) | (newValue & writeMask);
    }
}

/*
** the stencil tables are to be deallocated in context destruction time.
**
** These tables are private to the context.  They are not part of
** the drawablePrivate structure, so they should be deallocated together
** with the context.
*/
static void FreeStencilBuf(__GLbuffer *buf, __GLcontext *gc)
{
    __GLstencilBuffer *sfb = (__GLstencilBuffer *)buf;

    /* deallocate and null out */
    (*gc->imports.free)(gc, sfb->testFuncTable);
    (*gc->imports.free)(gc, sfb->failOpTable);
    (*gc->imports.free)(gc, sfb->depthFailOpTable);
    (*gc->imports.free)(gc, sfb->depthPassOpTable);

    sfb->testFuncTable = NULL;
    sfb->failOpTable = NULL;
    sfb->depthFailOpTable = NULL;
    sfb->depthPassOpTable = NULL;
}
    

void __glValidateStencil(__GLcontext *gc)
{
    GLint i;
    __GLstencilCell reference, maskedRef, mask, writeMask;
    GLenum testFunc;
    GLboolean *tp;

    /*
    ** Validate the stencil tables even if stenciling is disabled.  This
    ** function is only called if the stencil func or op changes, and it
    ** won't get called later if stenciling is turned on, so we need to get
    ** it right now.
    */

    mask = (__GLstencilCell) gc->state.stencil.mask;
    reference = (__GLstencilCell) gc->state.stencil.reference;
    maskedRef = ((__GLstencilCell) gc->state.stencil.reference) & mask;
    testFunc = gc->state.stencil.testFunc;

    /*
    ** Build up test function table.  The current stencil buffer value
    ** will be the index to this table.
    */
    tp = &gc->stencilBuffer.testFuncTable[0];
    if( tp == NULL ) {
	gc->stencilBuffer.testFuncTable = tp = (GLboolean *)
	    (*gc->imports.malloc)(gc, sizeof(GLboolean) * __GL_STENCIL_RANGE);
	gc->stencilBuffer.failOpTable = (__GLstencilCell *)
	    (*gc->imports.malloc)(gc, sizeof(__GLstencilCell) * __GL_STENCIL_RANGE);
	gc->stencilBuffer.depthFailOpTable = (__GLstencilCell *)
	    (*gc->imports.malloc)(gc, sizeof(__GLstencilCell) * __GL_STENCIL_RANGE);
	gc->stencilBuffer.depthPassOpTable = (__GLstencilCell *)
	    (*gc->imports.malloc)(gc, sizeof(__GLstencilCell) * __GL_STENCIL_RANGE);
    }

    for (i = 0; i < __GL_STENCIL_RANGE; i++) {
	switch (testFunc) {
	  case GL_NEVER:	*tp++ = GL_FALSE; break;
	  case GL_LESS:		*tp++ = maskedRef < (i & mask); break;
	  case GL_EQUAL:	*tp++ = maskedRef == (i & mask); break;
	  case GL_LEQUAL:	*tp++ = maskedRef <= (i & mask); break;
	  case GL_GREATER:	*tp++ = maskedRef > (i & mask); break;
	  case GL_NOTEQUAL:	*tp++ = maskedRef != (i & mask); break;
	  case GL_GEQUAL:	*tp++ = maskedRef >= (i & mask); break;
	  case GL_ALWAYS:	*tp++ = GL_TRUE; break;
	}
    }

    /*
    ** Build up fail op table.
    */
    writeMask = (__GLstencilCell) gc->state.stencil.writeMask;
    buildOpTable(gc, &gc->stencilBuffer.failOpTable[0],
		 gc->state.stencil.fail, reference, writeMask);
    buildOpTable(gc, &gc->stencilBuffer.depthFailOpTable[0],
		 gc->state.stencil.depthFail, reference, writeMask);
    buildOpTable(gc, &gc->stencilBuffer.depthPassOpTable[0],
		 gc->state.stencil.depthPass, reference, writeMask);
}

/************************************************************************/


/************************************************************************/

static void Pick(__GLcontext *gc, __GLstencilBuffer *sfb)
{
#ifdef __GL_LINT
    sfb = sfb;
#endif
    if (gc->validateMask & (__GL_VALIDATE_STENCIL_FUNC |
			    __GL_VALIDATE_STENCIL_OP)) {
	__glValidateStencil(gc);
    }
}

void __glInitStencil8(__GLstencilBuffer *sfb, __GLcontext *gc )
{
    __glInitBuffer( &sfb->buf, gc );
    
    sfb->buf.free = FreeStencilBuf;

    sfb->pick = Pick;
    sfb->store = Store;
    sfb->fetch = Fetch;
    sfb->testFunc = TestFunc;
    sfb->failOp = FailOp;
    sfb->passDepthFailOp = PassDepthFailOp;
    sfb->depthPassOp = DepthPassOp;
    sfb->clear = Clear;
}

