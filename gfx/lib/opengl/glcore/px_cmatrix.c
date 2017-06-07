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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_cmatrix.c#3 $
*/

#include "context.h"
#include "global.h"
#include "g_imports.h"
#include "pixel.h"

void __glSpanColorMatrixGeneralRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    const __GLmatrix *m = &gc->transform.color->matrix;
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat rScaleIn = gc->constants.oneOverRedScale;
    GLfloat gScaleIn = gc->constants.oneOverGreenScale;
    GLfloat bScaleIn = gc->constants.oneOverBlueScale;
    GLfloat aScaleIn = gc->constants.oneOverAlphaScale;
    GLfloat rScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.r
    			* gc->constants.redScale;
    GLfloat gScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.g
    			* gc->constants.greenScale;
    GLfloat bScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.b
    			* gc->constants.blueScale;
    GLfloat aScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.a
    			* gc->constants.alphaScale;
    GLfloat rBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.r
    			* gc->constants.redScale;
    GLfloat gBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.g
    			* gc->constants.greenScale;
    GLfloat bBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.b
    			* gc->constants.blueScale;
    GLfloat aBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.a
    			* gc->constants.alphaScale;

    while ( width-- > 0 ) {
        GLfloat r = *in++ * rScaleIn;
        GLfloat g = *in++ * gScaleIn;
        GLfloat b = *in++ * bScaleIn;
        GLfloat a = *in++ * aScaleIn;

	*out++ = (m->matrix[0][0]*r + m->matrix[1][0]*g +
		  m->matrix[2][0]*b + m->matrix[3][0]*a) * rScaleOut + rBiasOut;
	*out++ = (m->matrix[0][1]*r + m->matrix[1][1]*g +
		  m->matrix[2][1]*b + m->matrix[3][1]*a) * gScaleOut + gBiasOut;
	*out++ = (m->matrix[0][2]*r + m->matrix[1][2]*g +
		  m->matrix[2][2]*b + m->matrix[3][2]*a) * bScaleOut + bBiasOut;
	*out++ = (m->matrix[0][3]*r + m->matrix[1][3]*g +
		  m->matrix[2][3]*b + m->matrix[3][3]*a) * aScaleOut + aBiasOut;
    }
}

void __glSpanColorMatrixNoScaleBiasRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    const __GLmatrix *m = &gc->transform.color->matrix;
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat rScaleIn = gc->constants.oneOverRedScale;
    GLfloat gScaleIn = gc->constants.oneOverGreenScale;
    GLfloat bScaleIn = gc->constants.oneOverBlueScale;
    GLfloat aScaleIn = gc->constants.oneOverAlphaScale;
    GLfloat rScaleOut = gc->constants.redScale;
    GLfloat gScaleOut = gc->constants.greenScale;
    GLfloat bScaleOut = gc->constants.blueScale;
    GLfloat aScaleOut = gc->constants.alphaScale;

    while ( width-- > 0 ) {
        GLfloat r = *in++ * rScaleIn;
        GLfloat g = *in++ * gScaleIn;
        GLfloat b = *in++ * bScaleIn;
        GLfloat a = *in++ * aScaleIn;

	*out++ = (m->matrix[0][0]*r + m->matrix[1][0]*g +
		  m->matrix[2][0]*b + m->matrix[3][0]*a) * rScaleOut;
	*out++ = (m->matrix[0][1]*r + m->matrix[1][1]*g +
		  m->matrix[2][1]*b + m->matrix[3][1]*a) * gScaleOut;
	*out++ = (m->matrix[0][2]*r + m->matrix[1][2]*g +
		  m->matrix[2][2]*b + m->matrix[3][2]*a) * bScaleOut;
	*out++ = (m->matrix[0][3]*r + m->matrix[1][3]*g +
		  m->matrix[2][3]*b + m->matrix[3][3]*a) * aScaleOut;
    }
}

void __glSpanColorMatrixOnlyScaleBiasRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat rScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.r;
    GLfloat gScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.g;
    GLfloat bScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.b;
    GLfloat aScaleOut = gc->state.pixel.transferMode.postColorMatrixScale.a;
    GLfloat rBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.r
    			* gc->constants.redScale;
    GLfloat gBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.g
    			* gc->constants.greenScale;
    GLfloat bBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.b
    			* gc->constants.blueScale;
    GLfloat aBiasOut = gc->state.pixel.transferMode.postColorMatrixBias.a
    			* gc->constants.alphaScale;

    while ( width-- > 0 ) {
        GLfloat r=*in++, g=*in++, b=*in++, a=*in++;

	*out++ = r * rScaleOut + rBiasOut;
	*out++ = g * gScaleOut + gBiasOut;
	*out++ = b * bScaleOut + bBiasOut;
	*out++ = a * aScaleOut + aBiasOut;
    }
}
