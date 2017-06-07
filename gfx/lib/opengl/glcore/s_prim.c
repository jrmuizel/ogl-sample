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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_prim.c#6 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"
#include "vcache.h"

void GLAPI __glim_Begin(GLenum mode)
{
    __GL_SETUP();
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	    glBegin(mode);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    if ((GLuint)mode > GL_POLYGON) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __gl_beginMode = __GL_IN_BEGIN;
    __GL_API_BGN_RENDER();

    (*gc->procs.beginPrim[mode])(gc);
}

void GLAPI __glim_End(void)
{
    __GL_SETUP();
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode == __GL_NOT_IN_BEGIN || beginMode == __GL_NEED_VALIDATE) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }
    assert(beginMode == __GL_IN_BEGIN);
    __GL_API_END_RENDER();

    (*gc->procs.endPrim)(gc);

    __gl_beginMode = __GL_NOT_IN_BEGIN;
}

/************************************************************************/

/*
** Generic functions to point to when no work should be done. These take
**  different numbers of arguments so there's always a version which
**  cleans up the stack properly. Pointers to these functions are cast
**  to the correct type (usually with integer or typed pointer arguments)
**  wherever they're used.
*/
void __glNop0(void) {}
void __glNop1(GLint a1) {}
void __glNop2(GLint a1, GLint a2) {}
void __glNop3(GLint a1, GLint a2, GLint a3) {}
void __glNop4(GLint a1, GLint a2, GLint a3, GLint a4) {}
void __glNop5(GLint a1, GLint a2, GLint a3, GLint a4, GLint a5) {}
void __glNop6(GLint a1, GLint a2, GLint a3, GLint a4, GLint a5, GLint a6) {}

void GLAPI __glGLAPINop1(GLint a) {}

/*
** End a primitive that needs no special end processing
*/
void __glEndPrim(__GLcontext *gc)
{
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
    gc->procs.endPrim = __glEndPrim;
}


void GLAPI __glim_UnimplementedExtension(void) 
{
    __glSetError(GL_INVALID_OPERATION);
    return;
}
