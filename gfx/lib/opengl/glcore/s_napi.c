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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_napi.c#6 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"

void GLAPI __glim_Normal3d(GLdouble x, GLdouble y, GLdouble z)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.normal.x = x;
    gc->state.current.normal.y = y;
    gc->state.current.normal.z = z;
}

void GLAPI __glim_Normal3dv(const GLdouble v[3])
{
    GLdouble x, y, z;
    __GL_SETUP();
    __GL_API_VAPI();

    x = v[0];
    y = v[1];
    z = v[2];
    gc->state.current.normal.x = x;
    gc->state.current.normal.y = y;
    gc->state.current.normal.z = z;
}

void GLAPI __glim_Normal3f(GLfloat x, GLfloat y, GLfloat z)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.normal.x = x;
    gc->state.current.normal.y = y;
    gc->state.current.normal.z = z;
}

void GLAPI __glim_Normal3fv(const GLfloat v[3])
{
    GLfloat x, y, z;
    __GL_SETUP();
    __GL_API_VAPI();

    x = v[0];
    y = v[1];
    z = v[2];
    gc->state.current.normal.x = x;
    gc->state.current.normal.y = y;
    gc->state.current.normal.z = z;
}

void GLAPI __glim_Normal3b(GLbyte x, GLbyte y, GLbyte z)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.normal.x = __GL_B_TO_FLOAT(x);
    gc->state.current.normal.y = __GL_B_TO_FLOAT(y);
    gc->state.current.normal.z = __GL_B_TO_FLOAT(z);
}

void GLAPI __glim_Normal3bv(const GLbyte v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.normal.x = __GL_B_TO_FLOAT(v[0]);
    gc->state.current.normal.y = __GL_B_TO_FLOAT(v[1]);
    gc->state.current.normal.z = __GL_B_TO_FLOAT(v[2]);
}

void GLAPI __glim_Normal3s(GLshort x, GLshort y, GLshort z)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.normal.x = __GL_S_TO_FLOAT(x);
    gc->state.current.normal.y = __GL_S_TO_FLOAT(y);
    gc->state.current.normal.z = __GL_S_TO_FLOAT(z);
}

void GLAPI __glim_Normal3sv(const GLshort v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.normal.x = __GL_S_TO_FLOAT(v[0]);
    gc->state.current.normal.y = __GL_S_TO_FLOAT(v[1]);
    gc->state.current.normal.z = __GL_S_TO_FLOAT(v[2]);
}

void GLAPI __glim_Normal3i(GLint x, GLint y, GLint z)
{
    __GL_SETUP();
    __GL_API_VAPI();

    /* Beware of overflow! */
    gc->state.current.normal.x = __GL_I_TO_FLOAT(x);
    gc->state.current.normal.y = __GL_I_TO_FLOAT(y);
    gc->state.current.normal.z = __GL_I_TO_FLOAT(z);
}

void GLAPI __glim_Normal3iv(const GLint v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    /* Beware of overflow! */
    gc->state.current.normal.x = __GL_I_TO_FLOAT(v[0]);
    gc->state.current.normal.y = __GL_I_TO_FLOAT(v[1]);
    gc->state.current.normal.z = __GL_I_TO_FLOAT(v[2]);
}
