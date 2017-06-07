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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_rect.c#4 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"
#include "pixel.h"

void GLAPI __glim_Rectd(GLdouble ax, GLdouble ay, GLdouble bx, GLdouble by)
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, ax, ay, bx, by);
}

void GLAPI __glim_Rectdv(const GLdouble v1[2], const GLdouble v2[2])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, v1[0], v1[1], v2[0], v2[1]);
}

void GLAPI __glim_Rectf(GLfloat ax, GLfloat ay, GLfloat bx, GLfloat by)
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, ax, ay, bx, by);
}

void GLAPI __glim_Rectfv(const GLfloat v1[2], const GLfloat v2[2])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, v1[0], v1[1], v2[0], v2[1]);
}

void GLAPI __glim_Recti(GLint ax, GLint ay, GLint bx, GLint by)
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, ax, ay, bx, by);
}

void GLAPI __glim_Rectiv(const GLint v1[2], const GLint v2[2])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, v1[0], v1[1], v2[0], v2[1]);
}

void GLAPI __glim_Rects(GLshort ax, GLshort ay, GLshort bx, GLshort by)
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, ax, ay, bx, by);
}

void GLAPI __glim_Rectsv(const GLshort v1[2], const GLshort v2[2])
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_NOTBE_RENDER();

    (*gc->procs.rect)(gc, v1[0], v1[1], v2[0], v2[1]);
}

void __glRect(__GLcontext *gc, __GLfloat x0, __GLfloat y0, 
	      __GLfloat x1, __GLfloat y1)
{
    glBegin(GL_QUADS);
    glVertex2f(x0, y0);
    glVertex2f(x1, y0);
    glVertex2f(x1, y1);
    glVertex2f(x0, y1);
    glEnd();
}
