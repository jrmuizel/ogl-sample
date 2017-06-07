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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_tapi.c#6 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"

void GLAPI __glim_TexCoord1d(GLdouble x)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord1f(GLfloat x)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord1i(GLint x)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord1s(GLshort x)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord1dv(const GLdouble x[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord1fv(const GLfloat x[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord1iv(const GLint x[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord1sv(const GLshort x[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = __glZero;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

/************************************************************************/

void GLAPI __glim_TexCoord2d(GLdouble x, GLdouble y)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord2f(GLfloat x, GLfloat y)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord2i(GLint x, GLint y)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord2s(GLshort x, GLshort y)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord2dv(const GLdouble x[2])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord2fv(const GLfloat x[2])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord2iv(const GLint x[2])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord2sv(const GLshort x[2])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = __glZero;
    gc->state.current.texture[0].w = __glOne;
}

/************************************************************************/

void GLAPI __glim_TexCoord3d(GLdouble x, GLdouble y, GLdouble z)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord3f(GLfloat x, GLfloat y, GLfloat z)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord3i(GLint x, GLint y, GLint z)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord3s(GLshort x, GLshort y, GLshort z)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord3dv(const GLdouble x[3])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord3fv(const GLfloat x[3])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord3iv(const GLint x[3])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = __glOne;
}

void GLAPI __glim_TexCoord3sv(const GLshort x[3])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = __glOne;
}

/************************************************************************/

void GLAPI __glim_TexCoord4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = w;
}

void GLAPI __glim_TexCoord4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = w;
}

void GLAPI __glim_TexCoord4i(GLint x, GLint y, GLint z, GLint w)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = w;
}

void GLAPI __glim_TexCoord4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x;
    gc->state.current.texture[0].y = y;
    gc->state.current.texture[0].z = z;
    gc->state.current.texture[0].w = w;
}

void GLAPI __glim_TexCoord4dv(const GLdouble x[4])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = x[3];
}

void GLAPI __glim_TexCoord4fv(const GLfloat x[4])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = x[3];
}

void GLAPI __glim_TexCoord4iv(const GLint x[4])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = x[3];
}

void GLAPI __glim_TexCoord4sv(const GLshort x[4])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.texture[0].x = x[0];
    gc->state.current.texture[0].y = x[1];
    gc->state.current.texture[0].z = x[2];
    gc->state.current.texture[0].w = x[3];
}

/************************************************************************/

void GLAPI __glim_MultiTexCoord1dARB(GLenum target, GLdouble x)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord1fARB(GLenum target, GLfloat x)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord1iARB(GLenum target, GLint x)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord1sARB(GLenum target, GLshort x)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord1dvARB(GLenum target, const GLdouble x[1])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord1fvARB(GLenum target, const GLfloat x[1])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord1ivARB(GLenum target, const GLint x[1])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord1svARB(GLenum target, const GLshort x[1])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = __glZero;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2dARB(GLenum target, GLdouble x, GLdouble y)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2fARB(GLenum target, GLfloat x, GLfloat y)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2iARB(GLenum target, GLint x, GLint y)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2sARB(GLenum target, GLshort x, GLshort y)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2dvARB(GLenum target, const GLdouble x[2])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2fvARB(GLenum target, const GLfloat x[2])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2ivARB(GLenum target, const GLint x[2])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord2svARB(GLenum target, const GLshort x[2])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = __glZero;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3dARB(GLenum target, GLdouble x, GLdouble y, GLdouble z)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3fARB(GLenum target, GLfloat x, GLfloat y, GLfloat z)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3iARB(GLenum target, GLint x, GLint y, GLint z)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3sARB(GLenum target, GLshort x, GLshort y, GLshort z)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3dvARB(GLenum target, const GLdouble x[3])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3fvARB(GLenum target, const GLfloat x[3])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3ivARB(GLenum target, const GLint x[3])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord3svARB(GLenum target, const GLshort x[3])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = __glOne;
}

void GLAPI __glim_MultiTexCoord4dARB(GLenum target, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = (__GLfloat) w;
}

void GLAPI __glim_MultiTexCoord4fARB(GLenum target, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = (__GLfloat) w;
}

void GLAPI __glim_MultiTexCoord4iARB(GLenum target, GLint x, GLint y, GLint z, GLint w)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = (__GLfloat) w;
}

void GLAPI __glim_MultiTexCoord4sARB(GLenum target, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x;
    gc->state.current.texture[unit].y = (__GLfloat) y;
    gc->state.current.texture[unit].z = (__GLfloat) z;
    gc->state.current.texture[unit].w = (__GLfloat) w;
}

void GLAPI __glim_MultiTexCoord4dvARB(GLenum target, const GLdouble x[4])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = (__GLfloat) x[3];
}

void GLAPI __glim_MultiTexCoord4fvARB(GLenum target, const GLfloat x[4])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = (__GLfloat) x[3];
}

void GLAPI __glim_MultiTexCoord4ivARB(GLenum target, const GLint x[4])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = (__GLfloat) x[3];
}

void GLAPI __glim_MultiTexCoord4svARB(GLenum target, const GLshort x[4])
{
    GLint unit;
    __GL_SETUP();
    __GL_API_VAPI();

    unit = (GLint) target - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.current.texture[unit].x = (__GLfloat) x[0];
    gc->state.current.texture[unit].y = (__GLfloat) x[1];
    gc->state.current.texture[unit].z = (__GLfloat) x[2];
    gc->state.current.texture[unit].w = (__GLfloat) x[3];
}
