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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_read.c#5 $
*/

#include "context.h"
#include "global.h"
#include "pixel.h"

/*
** This file contains span reading routines.  These are routines which 
** read data from the depth buffer, stencil buffer, or frame buffer
** into internal software spans.  The type of internal span that it
** is read into varies from routine to routine.
*/

/*
** A reader that reads spans into scaled a RGBA, FLOAT span.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanReadRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		      GLvoid *span)
{
    GLint i;
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;
    GLshort *pixelArray;
    GLint skipCount;

    width = spanInfo->width;
    spanData = (GLfloat*) span;
    pixelArray = spanInfo->pixelArray;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    for (i=0; i<width; i++) {
	(*gc->readBuffer->readColor)(gc->readBuffer, readX, readY, 
		(__GLcolor *) spanData);

	spanData += 4;
	skipCount = *pixelArray++;
	readX += skipCount;
    }
}

/*
** A reader that reads spans into a scaled RGBA, FLOAT span.
**
** zoomx is assumed to be less than or equal to -1.0 or greater than or
** equal to 1.0.
*/
void __glSpanReadRGBA2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       GLvoid *span)
{
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;

    width = spanInfo->width;
    spanData = (GLfloat*) span;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    (*gc->readBuffer->readSpan)(gc->readBuffer, readX, readY, 
	    (__GLcolor *) spanData, width);
}

/*
** A reader that reads spans into a COLOR_INDEX, FLOAT span.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanReadCI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		    GLvoid *span)
{
    GLint i;
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;
    GLshort *pixelArray;
    GLint skipCount;

    width = spanInfo->width;
    spanData = (GLfloat*) span;
    pixelArray = spanInfo->pixelArray;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    for (i=0; i<width; i++) {
	(*gc->readBuffer->readColor)(gc->readBuffer, readX, readY, 
		(__GLcolor *) spanData);
	spanData++;
	skipCount = *pixelArray++;
	readX += skipCount;
    }
}

/*
** A reader that reads spans into a COLOR_INDEX, FLOAT span.
**
** zoomx is assumed to be less than or equal to -1.0 or greater than or
** equal to 1.0.
*/
void __glSpanReadCI2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		     GLvoid *span)
{
    GLint i;
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;

    width = spanInfo->width;
    spanData = (GLfloat*) span;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    for (i=0; i<width; i++) {
	(*gc->readBuffer->readColor)(gc->readBuffer, readX, readY, 
		(__GLcolor *) spanData);
	spanData++;
	readX++;
    }
}

/*
** A reader that reads spans into a DEPTH_COMPONENT, FLOAT span.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanReadDepth(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       GLvoid *span)
{
    GLint i;
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;
    GLshort *pixelArray;
    GLint skipCount;
    __GLfloat oneOverScale;
    /* Accessing drawableBuf outside of a lock */
    __GLdrawableDepthBuffer *db = __GL_DEPTH_DP(&gc->depthBuffer);

    width = spanInfo->width;
    spanData = (GLfloat*) span;
    pixelArray = spanInfo->pixelArray;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;
    oneOverScale = __glOne / (gc->depthBuffer.scale >> db->numFracBits);

    for (i=0; i<width; i++) {
	*spanData++ = 
		(*gc->depthBuffer.fetch)(&(gc->depthBuffer), readX, readY) *
		oneOverScale;
	skipCount = *pixelArray++;
	readX += skipCount;
    }
}

/*
** A reader that reads spans into a DEPTH_COMPONENT, FLOAT span.
**
** zoomx is assumed to be less than or equal to -1.0 or greater than or
** equal to 1.0.
*/
void __glSpanReadDepth2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		        GLvoid *span)
{
    GLint i;
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;
    __GLfloat oneOverScale;
    /* Accessing drawableBuf outside of a lock */
    __GLdrawableDepthBuffer *db = __GL_DEPTH_DP(&gc->depthBuffer);

    width = spanInfo->width;
    spanData = (GLfloat*) span;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;
    oneOverScale = __glOne / (gc->depthBuffer.scale >> db->numFracBits);

    for (i=0; i<width; i++) {
	*spanData++ = 
		(*gc->depthBuffer.fetch)(&(gc->depthBuffer), readX, readY) *
		oneOverScale;
	readX++;
    }
}

/*
** A reader that reads spans into a DEPTH_COMPONENT, Uint span.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanReadDepthUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       GLvoid *span)
{
    GLint i;
    GLint width;
    GLuint *spanData;
    GLint readY, readX;
    GLshort *pixelArray;
    GLint skipCount;
    /* Accessing drawableBuf outside of a lock */
    __GLdrawableDepthBuffer *db = __GL_DEPTH_DP(&gc->depthBuffer);
    GLint shiftAmt = 32 - gc->depthBuffer.buf.drawableBuf->depth - 
			  db->numFracBits;

    width = spanInfo->width;
    spanData = (GLuint*) span;
    pixelArray = spanInfo->pixelArray;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    for (i=0; i<width; i++) {
	*spanData++ = 
		(*gc->depthBuffer.fetch)(&(gc->depthBuffer), readX, readY) <<
		shiftAmt;
	skipCount = *pixelArray++;
	readX += skipCount;
    }
}

/*
** A reader that reads spans into a DEPTH_COMPONENT, Uint span.
**
** zoomx is assumed to be less than or equal to -1.0 or greater than or
** equal to 1.0.
*/
void __glSpanReadDepthUint2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		        GLvoid *span)
{
    GLint i;
    GLint width;
    GLuint *spanData;
    GLint readY, readX;
    /* Accessing drawableBuf outside of a lock */
    __GLdrawableDepthBuffer *db = __GL_DEPTH_DP(&gc->depthBuffer);
    GLint shiftAmt = 32 - gc->depthBuffer.buf.drawableBuf->depth - 
			  db->numFracBits;

    width = spanInfo->width;
    spanData = (GLuint*) span;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    for (i=0; i<width; i++) {
	*spanData++ = 
		(*gc->depthBuffer.fetch)(&(gc->depthBuffer), readX, readY) <<
		shiftAmt;
	readX++;
    }
}

/*
** A reader that reads spans into a STENCIL_INDEX, FLOAT span.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanReadStencil(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		         GLvoid *span)
{
    GLint i;
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;
    GLshort *pixelArray;
    GLint skipCount;

    width = spanInfo->width;
    spanData = (GLfloat*) span;
    pixelArray = spanInfo->pixelArray;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    for (i=0; i<width; i++) {
	*spanData++ = (GLfloat)
	    ((*gc->stencilBuffer.fetch)(&(gc->stencilBuffer), readX, readY));
	skipCount = *pixelArray++;
	readX += skipCount;
    }
}

/*
** A reader that reads spans into a STENCIL_INDEX, FLOAT span.
**
** zoomx is assumed to be less than or equal to -1.0 or greater than or
** equal to 1.0.
*/
void __glSpanReadStencil2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		          GLvoid *span)
{
    GLint i;
    GLint width;
    GLfloat *spanData;
    GLint readY, readX;

    width = spanInfo->width;
    spanData = (GLfloat*) span;

    readY = (GLint) spanInfo->readY;
    readX = (GLint) spanInfo->readX;

    for (i=0; i<width; i++) {
	*spanData++ =  (GLfloat)
	    ((*gc->stencilBuffer.fetch)(&(gc->stencilBuffer), readX, readY));
	readX++;
    }
}
