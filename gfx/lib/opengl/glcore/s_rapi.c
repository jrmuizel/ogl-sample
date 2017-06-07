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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_rapi.c#4 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"

#ifdef __GL_DOUBLE
void GLAPI __glim_RasterPos2dv(const GLdouble v[2])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    (*gc->procs.rasterPos2)(gc, v);
}

void GLAPI __glim_RasterPos2fv(const GLfloat v[2])
{
    __GLfloat vv[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1];
    (*gc->procs.rasterPos2)(gc, vv);
}
#else
void GLAPI __glim_RasterPos2dv(const GLdouble v[2])
{
    __GLfloat vv[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1];
    (*gc->procs.rasterPos2)(gc, vv);
}

void GLAPI __glim_RasterPos2fv(const GLfloat v[2])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    (*gc->procs.rasterPos2)(gc, v);
}
#endif

void GLAPI __glim_RasterPos2d(GLdouble x, GLdouble y)
{
    __GLfloat v[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y;
    (*gc->procs.rasterPos2)(gc, v);
}

void GLAPI __glim_RasterPos2f(GLfloat x, GLfloat y)
{
    __GLfloat v[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y;
    (*gc->procs.rasterPos2)(gc, v);
}

void GLAPI __glim_RasterPos2i(GLint x, GLint y)
{
    __GLfloat v[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y;
    (*gc->procs.rasterPos2)(gc, v);
}

void GLAPI __glim_RasterPos2iv(const GLint v[2])
{
    __GLfloat vv[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1];
    (*gc->procs.rasterPos2)(gc, vv);
}

void GLAPI __glim_RasterPos2s(GLshort x, GLshort y)
{
    __GLfloat v[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y;
    (*gc->procs.rasterPos2)(gc, v);
}

void GLAPI __glim_RasterPos2sv(const GLshort v[2])
{
    __GLfloat vv[2];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1];
    (*gc->procs.rasterPos2)(gc, vv);
}

#ifdef __GL_DOUBLE
void GLAPI __glim_RasterPos3dv(const GLdouble v[3])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    (*gc->procs.rasterPos3)(gc, v);
}

void GLAPI __glim_RasterPos3fv(const GLfloat v[3])
{
    __GLfloat vv[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2];
    (*gc->procs.rasterPos3)(gc, vv);
}
#else
void GLAPI __glim_RasterPos3dv(const GLdouble v[3])
{
    __GLfloat vv[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2];
    (*gc->procs.rasterPos3)(gc, vv);
}

void GLAPI __glim_RasterPos3fv(const GLfloat v[3])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    (*gc->procs.rasterPos3)(gc, v);
}
#endif

void GLAPI __glim_RasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
    __GLfloat v[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z;
    (*gc->procs.rasterPos3)(gc, v);
}

void GLAPI __glim_RasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
    __GLfloat v[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z;
    (*gc->procs.rasterPos3)(gc, v);
}

void GLAPI __glim_RasterPos3i(GLint x, GLint y, GLint z)
{
    __GLfloat v[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z;
    (*gc->procs.rasterPos3)(gc, v);
}

void GLAPI __glim_RasterPos3iv(const GLint v[3])
{
    __GLfloat vv[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2];
    (*gc->procs.rasterPos3)(gc, vv);
}

void GLAPI __glim_RasterPos3s(GLshort x, GLshort y, GLshort z)
{
    __GLfloat v[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z;
    (*gc->procs.rasterPos3)(gc, v);
}

void GLAPI __glim_RasterPos3sv(const GLshort v[3])
{
    __GLfloat vv[3];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2];
    (*gc->procs.rasterPos3)(gc, vv);
}

#ifdef __GL_DOUBLE
void GLAPI __glim_RasterPos4dv(const GLdouble v[4])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    (*gc->procs.rasterPos4)(gc, v);
}

void GLAPI __glim_RasterPos4fv(const GLfloat v[4])
{
    __GLfloat vv[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2]; vv[3] = v[3];
    (*gc->procs.rasterPos4)(gc, vv);
}
#else
void GLAPI __glim_RasterPos4dv(const GLdouble v[4])
{
    __GLfloat vv[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2]; vv[3] = v[3];
    (*gc->procs.rasterPos4)(gc, vv);
}

void GLAPI __glim_RasterPos4fv(const GLfloat v[4])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    (*gc->procs.rasterPos4)(gc, v);
}
#endif

void GLAPI __glim_RasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    __GLfloat v[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z; v[3] = w;
    (*gc->procs.rasterPos4)(gc, v);
}

void GLAPI __glim_RasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    __GLfloat v[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z; v[3] = w;
    (*gc->procs.rasterPos4)(gc, v);
}

void GLAPI __glim_RasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
    __GLfloat v[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z; v[3] = w;
    (*gc->procs.rasterPos4)(gc, v);
}

void GLAPI __glim_RasterPos4iv(const GLint v[4])
{
    __GLfloat vv[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2]; vv[3] = v[3];
    (*gc->procs.rasterPos4)(gc, vv);
}

void GLAPI __glim_RasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
    __GLfloat v[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    v[0] = x; v[1] = y; v[2] = z; v[3] = w;
    (*gc->procs.rasterPos4)(gc, v);
}

void GLAPI __glim_RasterPos4sv(const GLshort v[4])
{
    __GLfloat vv[4];
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_VAPI();

    vv[0] = v[0]; vv[1] = v[1]; vv[2] = v[2]; vv[3] = v[3];
    (*gc->procs.rasterPos4)(gc, vv);
}
