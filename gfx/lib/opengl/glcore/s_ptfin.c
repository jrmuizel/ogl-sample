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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_ptfin.c#4 $
*/
#include "context.h"
#include "global.h"
#include "render.h"

void __glBeginPoints(__GLcontext *gc)
{
    __GLvertex *v0 = &gc->vertex.vbuf[0];

    gc->procs.vertex = gc->procs.vertexPoints;
    gc->vertex.v0 = v0;
    gc->procs.endPrim = __glEndPoints;
    gc->procs.matValidate = (void (*)(__GLcontext *gc)) __glNop1;
}

void __glEndPoints(__GLcontext *gc)
{
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
    gc->procs.endPrim = __glEndPrim;
}

void __glPointLock(__GLcontext *gc, __GLvertex *vx)
{
    if ((vx->flagBits & __GL_CLIP_ALL_MASK) == 0) {
	DO_VALIDATE(gc, vx, gc->vertex.faceNeeds[__GL_FRONTFACE] | __GL_HAS_FRONT_COLOR);
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.renderPoint)(gc, vx);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __glPoint(__GLcontext *gc, __GLvertex *vx)
{
    if ((vx->flagBits & __GL_CLIP_ALL_MASK) == 0) {
	DO_VALIDATE(gc, vx, gc->vertex.faceNeeds[__GL_FRONTFACE] | __GL_HAS_FRONT_COLOR);
	(*gc->procs.renderPoint)(gc, vx);
    }
}

void __glPointFastLock(__GLcontext *gc, __GLvertex *vx)
{
    if ((vx->flagBits & __GL_CLIP_ALL_MASK) == 0) {
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.renderPoint)(gc, vx);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __glPointFast(__GLcontext *gc, __GLvertex *vx)
{
    if ((vx->flagBits & __GL_CLIP_ALL_MASK) == 0) {
	(*gc->procs.renderPoint)(gc, vx);
    }
}

