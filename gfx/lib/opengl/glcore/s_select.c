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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_select.c#5 $
*/
#include "context.h"
#include "global.h"
#include "g_imports.h"
#include "g_imfncs.h"

void GLAPI __glim_SelectBuffer(GLsizei bufferLength, GLuint buffer[])
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (bufferLength < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (gc->renderMode == GL_SELECT) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }
    gc->select.overFlowed = GL_FALSE;
    gc->select.resultBase = buffer;
    gc->select.resultLength = bufferLength;
    gc->select.result = buffer;
}

void GLAPI __glim_InitNames(void)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (gc->renderMode == GL_SELECT) {
	gc->select.sp = gc->select.stack;
	gc->select.hit = GL_FALSE;
    }
}

void GLAPI __glim_LoadName(GLuint name)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (gc->renderMode == GL_SELECT) {
	if (gc->select.sp == gc->select.stack) {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	gc->select.sp[ -1 ] = name;
	gc->select.hit = GL_FALSE;
    }
}

void GLAPI __glim_PopName(void)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (gc->renderMode == GL_SELECT) {
	if (gc->select.sp == gc->select.stack) {
	    __glSetError(GL_STACK_UNDERFLOW);
	    return;
	}
	gc->select.sp = gc->select.sp - 1;
	gc->select.hit = GL_FALSE;
    }
}

void GLAPI __glim_PushName(GLuint name)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (gc->renderMode == GL_SELECT) {
	if (gc->select.sp >= &gc->select.stack[gc->constants.maxNameStackDepth]) {
	    __glSetError(GL_STACK_OVERFLOW);
	    return;
	}
	gc->select.sp[0] = name;
	gc->select.sp = gc->select.sp + 1;
	gc->select.hit = GL_FALSE;
    }
}

/************************************************************************/

#define __GL_CONVERT_Z_TO_UINT(z)  ((GLuint) z)

/*
** Copy current name stack into the users result buffer.
*/
void __glSelectHit(__GLcontext *gc, __GLfloat z)
{
    GLuint *src;
    GLuint *dest = gc->select.result;
    GLuint *end = gc->select.resultBase + gc->select.resultLength;
    GLuint iz;

    if (gc->select.overFlowed) {
	return;
    }

    /* Scale z to [0, 2^32 -1] range.
    ** If buffer is 32 bit, no coversion necessary.  Otherwise we
    ** have to unscale to [0,1] range and rescale to final range.
    */
    if (gc->depthBuffer.buf.drawableBuf->depth == 32) {
	iz = __GL_CONVERT_Z_TO_UINT(z);
    } else {
	iz = z * gc->constants.val4294965000 / gc->depthBuffer.scale;
    }

    if (!gc->select.hit) {
	gc->select.hit = GL_TRUE;

	/* Put number of elements in name stack out first */
	if (dest == end) {
	  overflow:
	    gc->select.overFlowed = GL_TRUE;
	    gc->select.result = end;
	    return;
	}
	*dest++ = (GLuint)(gc->select.sp - gc->select.stack);
	gc->select.hits++;

	/* Put out smallest z */
	if (dest == end) goto overflow;
	gc->select.z = dest;
	*dest++ = iz;

	/* Put out largest z */
	if (dest == end) goto overflow;
	*dest++ = iz;

	/* Copy name stack into output buffer */
	for (src = gc->select.stack; src < gc->select.sp; src++) {
	    if (dest == end) {
		goto overflow;
	    }
	    *dest++ = *src;
	}
	gc->select.result = dest;
    } else {
	/* Update range of Z values */
	assert(gc->select.z != 0);
	if (iz < gc->select.z[0]) {
	    gc->select.z[0] = iz;
	}
	if (iz > gc->select.z[1]) {
	    gc->select.z[1] = iz;
	}
    }
}

void __glSelectTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			__GLvertex *c)
{
    if (gc->state.enables.general & __GL_CULL_FACE_ENABLE) {
	__GLfloat dxAC, dxBC, dyAC, dyBC, area;
	GLboolean ccw, frontFacing;

	/* Compute signed area of the triangle */
	dxAC = a->window.x - c->window.x;
	dxBC = b->window.x - c->window.x;
	dyAC = a->window.y - c->window.y;
	dyBC = b->window.y - c->window.y;
	area = dxAC * dyBC - dxBC * dyAC;
	ccw = area >= __glZero;

	if ((gc->state.polygon.frontFaceDirection == GL_CCW) ^
	    (gc->constants.yInverted)) {
	    frontFacing = ccw;
	} else {
	    frontFacing = !ccw;
	}
	if ((gc->state.polygon.cull == GL_FRONT_AND_BACK) ||
	    ((gc->state.polygon.cull == GL_FRONT) && frontFacing) ||
	    ((gc->state.polygon.cull == GL_BACK) && !frontFacing)) {
	    /* Culled */
	    return;
	}
    }
    __glSelectHit(gc, a->window.z);
    __glSelectHit(gc, b->window.z);
    __glSelectHit(gc, c->window.z);
}

void __glSelectLine(__GLcontext *gc, __GLvertex *a, __GLvertex *b)
{
    __glSelectHit(gc, a->window.z);
    __glSelectHit(gc, b->window.z);
}

void __glSelectPoint(__GLcontext *gc, __GLvertex *v)
{
    __glSelectHit(gc, v->window.z);
}
