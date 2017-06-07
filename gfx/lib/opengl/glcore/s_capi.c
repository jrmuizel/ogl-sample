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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_capi.c#6 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"

void GLAPI __glim_Color3ub(GLubyte r, GLubyte g, GLubyte b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UB_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_UB_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_UB_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_FastColor3ub(GLubyte r, GLubyte g, GLubyte b)
{
    __GLpixelMachine *pm;
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UB_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_UB_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_UB_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;

    /* The incoming values are never below zero or over 255, so
    ** there is no need to do the usual ClampAndScale call.
    ** Scaled values have been calculated and stoed in a table.
    */
    pm = &gc->pixel;
    gc->state.current.color.r = pm->redMap[r];
    gc->state.current.color.g = pm->greenMap[g];
    gc->state.current.color.b = pm->blueMap[b];
    gc->state.current.color.a = gc->frontBuffer.alphaScale;
}

void GLAPI __glim_Color3ubv(const GLubyte v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UB_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_UB_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_UB_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_FastColor3ubv(const GLubyte v[3])
{
    __GLpixelMachine *pm;
    GLubyte r, g, b;
    __GL_SETUP();
    __GL_API_VAPI();

    r = v[0];
    g = v[1];
    b = v[2];

    gc->state.current.userColor.r = __GL_UB_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_UB_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_UB_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;

    /* The incoming values are never below zero or over 255, so
    ** there is no need to do the usual ClampAndScale call.
    ** Scaled values have been calculated and stoed in a table.
    */
    pm = &gc->pixel;
    gc->state.current.color.r = pm->redMap[r];
    gc->state.current.color.g = pm->greenMap[g];
    gc->state.current.color.b = pm->blueMap[b];
    gc->state.current.color.a = gc->frontBuffer.alphaScale;
}

void GLAPI __glim_Color3b(GLbyte r, GLbyte g, GLbyte b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_B_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_B_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_B_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3bv(const GLbyte v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_B_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_B_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_B_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3d(GLdouble r, GLdouble g, GLdouble b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = r;
    gc->state.current.userColor.g = g;
    gc->state.current.userColor.b = b;
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3dv(const GLdouble v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = v[0];
    gc->state.current.userColor.g = v[1];
    gc->state.current.userColor.b = v[2];
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3f(GLfloat r, GLfloat g, GLfloat b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = r;
    gc->state.current.userColor.g = g;
    gc->state.current.userColor.b = b;
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3fv(const GLfloat v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = v[0];
    gc->state.current.userColor.g = v[1];
    gc->state.current.userColor.b = v[2];
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3i(GLint r, GLint g, GLint b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_I_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_I_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_I_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3iv(const GLint v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_I_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_I_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_I_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3ui(GLuint r, GLuint g, GLuint b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UI_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_UI_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_UI_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3uiv(const GLuint v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UI_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_UI_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_UI_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3s(GLshort r, GLshort g, GLshort b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_S_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_S_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_S_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3sv(const GLshort v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_S_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_S_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_S_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3us(GLushort r, GLushort g, GLushort b)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_US_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_US_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_US_TO_FLOAT(b);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color3usv(const GLushort v[3])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_US_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_US_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_US_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __glOne;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

/************************************************************************/

void GLAPI __glim_Color4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UB_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_UB_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_UB_TO_FLOAT(b);
    gc->state.current.userColor.a = __GL_UB_TO_FLOAT(a);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4ubv(const GLubyte v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UB_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_UB_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_UB_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __GL_UB_TO_FLOAT(v[3]);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4b(GLbyte r, GLbyte g, GLbyte b, GLbyte a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_B_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_B_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_B_TO_FLOAT(b);
    gc->state.current.userColor.a = __GL_B_TO_FLOAT(a);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4bv(const GLbyte v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_B_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_B_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_B_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __GL_B_TO_FLOAT(v[3]);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4d(GLdouble r, GLdouble g, GLdouble b, GLdouble a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = r;
    gc->state.current.userColor.g = g;
    gc->state.current.userColor.b = b;
    gc->state.current.userColor.a = a;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4dv(const GLdouble v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = v[0];
    gc->state.current.userColor.g = v[1];
    gc->state.current.userColor.b = v[2];
    gc->state.current.userColor.a = v[3];
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = r;
    gc->state.current.userColor.g = g;
    gc->state.current.userColor.b = b;
    gc->state.current.userColor.a = a;
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4fv(const GLfloat v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = v[0];
    gc->state.current.userColor.g = v[1];
    gc->state.current.userColor.b = v[2];
    gc->state.current.userColor.a = v[3];
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4i(GLint r, GLint g, GLint b, GLint a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_I_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_I_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_I_TO_FLOAT(b);
    gc->state.current.userColor.a = __GL_I_TO_FLOAT(a);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4iv(const GLint v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_I_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_I_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_I_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __GL_I_TO_FLOAT(v[3]);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4ui(GLuint r, GLuint g, GLuint b, GLuint a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UI_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_UI_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_UI_TO_FLOAT(b);
    gc->state.current.userColor.a = __GL_UI_TO_FLOAT(a);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4uiv(const GLuint v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_UI_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_UI_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_UI_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __GL_UI_TO_FLOAT(v[3]);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4s(GLshort r, GLshort g, GLshort b, GLshort a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_S_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_S_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_S_TO_FLOAT(b);
    gc->state.current.userColor.a = __GL_S_TO_FLOAT(a);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4sv(const GLshort v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_S_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_S_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_S_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __GL_S_TO_FLOAT(v[3]);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4us(GLushort r, GLushort g, GLushort b, GLushort a)
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_US_TO_FLOAT(r);
    gc->state.current.userColor.g = __GL_US_TO_FLOAT(g);
    gc->state.current.userColor.b = __GL_US_TO_FLOAT(b);
    gc->state.current.userColor.a = __GL_US_TO_FLOAT(a);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

void GLAPI __glim_Color4usv(const GLushort v[4])
{
    __GL_SETUP();
    __GL_API_VAPI();

    gc->state.current.userColor.r = __GL_US_TO_FLOAT(v[0]);
    gc->state.current.userColor.g = __GL_US_TO_FLOAT(v[1]);
    gc->state.current.userColor.b = __GL_US_TO_FLOAT(v[2]);
    gc->state.current.userColor.a = __GL_US_TO_FLOAT(v[3]);
    gc->vertexCache.colorChanged = GL_TRUE;
    (*gc->procs.applyColor)(gc);
}

/************************************************************************/

void GLAPI __glim_Indexd(GLdouble c)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c;
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexf(GLfloat c)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c;
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexi(GLint c)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c;
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexs(GLshort c)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c;
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexub(GLubyte c)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c;
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexdv(const GLdouble c[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c[0];
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexfv(const GLfloat c[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c[0];
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexiv(const GLint c[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c[0];
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexsv(const GLshort c[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c[0];
    gc->vertexCache.colorChanged = GL_TRUE;
}

void GLAPI __glim_Indexubv(const GLubyte c[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.userColorIndex = c[0];
    gc->vertexCache.colorChanged = GL_TRUE;
}
