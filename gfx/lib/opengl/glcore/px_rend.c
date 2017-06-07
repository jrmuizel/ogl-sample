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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_rend.c#10 $
*/

#include "context.h"
#include "global.h"
#include "pixel.h"

/*
** This file contains routines to render a span of pixel data (from a 
** glDrawPixels or possibly a glCopyPixels request).
*/

/*
** This routine is used to store one fragment from a DrawPixels request.
** It should only be used if the user is either texturing or fogging.
*/
void __glSlowDrawPixelsStore(__GLcolorBuffer *cfb, const __GLfragment *frag)
{
    __GLcontext *gc = cfb->buf.gc;
    __GLvertex *rp = &gc->state.current.rasterPos;
    __GLfragment newfrag;

    newfrag = *frag;
    if (gc->texture.textureEnabled) {
	__GLcoord strq[__GL_NUM_TEXTURE_UNITS];
	__GLfloat rhow[__GL_NUM_TEXTURE_UNITS];
	GLint i;

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    strq[i] = rp->texture[i];
	    rhow[i] = __glOne;
	}
	(*gc->procs.textureRaster)(gc,
			&newfrag.color[__GL_PRIMARY_COLOR], strq, rhow);
    }
    if (gc->state.enables.general & __GL_FOG_ENABLE) {
	(*gc->procs.fogPoint)(gc, &newfrag, rp->eye.z);
    }
    (*gc->procs.store)(cfb, &newfrag);
}

/*
** The only span format supported by this routine is GL_RGB, GL_UNSIGNED_BYTE.
** The store proc is assumed not to mess with the fragment color or alpha.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderRGBubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		            const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLubyte *spanData;
    GLfloat *redMap, *greenMap, *blueMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLint startCol;
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    frag.color[__GL_PRIMARY_COLOR].a =
	((GLfloat *) (gc->pixel.alphaCurMap))[255];
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLubyte*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[*spanData++];
	    do {
		frag.x = column;

		/* This procedure will do the rest */
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** The only span format supported by this routine is GL_BGR, GL_UNSIGNED_BYTE.
** The store proc is assumed not to mess with the fragment color or alpha.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
**
** Copied from __glSpanRenderRGBubyte().
*/
void __glSpanRenderBGRubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		            const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLubyte *spanData;
    GLfloat *redMap, *greenMap, *blueMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLint startCol;
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    frag.color[__GL_PRIMARY_COLOR].a =
	((GLfloat *) (gc->pixel.alphaCurMap))[255];
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLubyte*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[*spanData++];
	    do {
		frag.x = column;

		/* This procedure will do the rest */
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a RGB, UNSIGNED_BYTE span.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
**
** The store proc is assumed not to mess with the fragment alpha.
*/
void __glSpanRenderRGBubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLubyte *spanData;
    GLfloat *redMap, *greenMap, *blueMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint endCol, startCol;
    GLint rows;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    frag.color[__GL_PRIMARY_COLOR].a =
	((GLfloat *) (gc->pixel.alphaCurMap))[255];
    frag.z = spanInfo->fragz;
    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    store = gc->procs.pxStore;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLubyte*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[*spanData++];
	    frag.x = i;

	    /* This procedure will do the rest */
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a BGR, UNSIGNED_BYTE span.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
**
** The store proc is assumed not to mess with the fragment alpha.
**
** Copied from __glSpanRenderRGBubyte2().
*/
void __glSpanRenderBGRubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLubyte *spanData;
    GLfloat *redMap, *greenMap, *blueMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint endCol, startCol;
    GLint rows;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    frag.color[__GL_PRIMARY_COLOR].a =
	((GLfloat *) (gc->pixel.alphaCurMap))[255];
    frag.z = spanInfo->fragz;
    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    store = gc->procs.pxStore;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLubyte*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[*spanData++];
	    frag.x = i;

	    /* This procedure will do the rest */
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_RGBA, GL_UNSIGNED_BYTE span.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderRGBAubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLubyte *spanData;
    GLfloat *redMap, *greenMap, *blueMap, *alphaMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLint startCol;
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    alphaMap = (GLfloat*) gc->pixel.alphaCurMap;
    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLubyte*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].a = alphaMap[*spanData++];
	    do {
		frag.x = column;

		/* This procedure will do the rest */
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_RGBA, GL_UNSIGNED_BYTE span.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderRGBAubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		              const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLubyte *spanData;
    GLfloat *redMap, *greenMap, *blueMap, *alphaMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint endCol, startCol;
    GLint rows;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    frag.z = spanInfo->fragz;
    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    alphaMap = (GLfloat*) gc->pixel.alphaCurMap;
    store = gc->procs.pxStore;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLubyte*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[*spanData++];
	    frag.color[__GL_PRIMARY_COLOR].a = alphaMap[*spanData++];
	    frag.x = i;

	    /* This procedure will do the rest */
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_DEPTH_COMPONENT, GL_UNSIGNED_INT span.  This implementation
** grabs the resolution from gc->depthBuffer.buf.depth.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderDepthUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLuint *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLint startCol;
    GLshort *pixelArray;
    /* Accessing drawableBuf outside of a lock */
    __GLdrawableDepthBuffer *db = __GL_DEPTH_DP(&gc->depthBuffer);
    GLint shiftAmt = 32 - gc->depthBuffer.buf.drawableBuf->depth - 
			  db->numFracBits;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    frag.color[__GL_PRIMARY_COLOR].r =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].r *
	gc->constants.redRescale;
    frag.color[__GL_PRIMARY_COLOR].g =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].g *
	gc->constants.greenRescale;
    frag.color[__GL_PRIMARY_COLOR].b =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].b *
	gc->constants.blueRescale;
    frag.color[__GL_PRIMARY_COLOR].a =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].a *
	gc->constants.alphaRescale;
    store = gc->procs.pxStore;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLuint*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.z = (*spanData++) >> shiftAmt;
	    do {
		frag.x = column;

		/* This procedure will do the rest */
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_DEPTH_COMPONENT, GL_UNSIGNED_INT span.  This implementation
** grabs the resolution from gc->depthBuffer.buf.depth.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderDepthUint2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		              const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLuint *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint endCol, startCol;
    GLint rows;
    /* Accessing drawableBuf outside of a lock */
    __GLdrawableDepthBuffer *db = __GL_DEPTH_DP(&gc->depthBuffer);
    GLint shiftAmt = 32 - gc->depthBuffer.buf.drawableBuf->depth - 
			  db->numFracBits;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    frag.color[__GL_PRIMARY_COLOR].r =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].r *
	gc->constants.redRescale;
    frag.color[__GL_PRIMARY_COLOR].g =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].g *
	gc->constants.greenRescale;
    frag.color[__GL_PRIMARY_COLOR].b =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].b *
	gc->constants.blueRescale;
    frag.color[__GL_PRIMARY_COLOR].a =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].a *
	gc->constants.alphaRescale;
    store = gc->procs.pxStore;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLuint*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.z = (*spanData++) >> shiftAmt;
	    frag.x = i;

	    /* This procedure will do the rest */
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}


/*
** Render a GL_STENCIL_INDEX, GL_UNSIGNED_SHORT span.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderStencilUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		                 const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLushort *spanData;
    void (*store)(__GLstencilBuffer *sfb, GLint x, GLint y, GLint value);
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    GLint rows;
    GLint startCol;
    GLint value;
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    store = sb->store;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLushort*) span;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    value = *spanData++;
	    do {
		(*store)(sb, column, row, value);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_STENCIL_INDEX, GL_UNSIGNED_SHORT span.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderStencilUshort2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		                  const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLushort *spanData;
    void (*store)(__GLstencilBuffer *sfb, GLint x, GLint y, GLint value);
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    GLint endCol, startCol;
    GLint rows;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    store = sb->store;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLushort*) span;
	i = startCol;
	do {
	    (*store)(sb, i, row, *spanData++);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_STENCIL_INDEX, GL_UNSIGNED_BYTE span.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderStencilUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		                const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLubyte *spanData;
    void (*store)(__GLstencilBuffer *sfb, GLint x, GLint y, GLint value);
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    GLint rows;
    GLint startCol;
    GLint value;
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    store = sb->store;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLubyte*) span;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    value = *spanData++;
	    do {
		(*store)(sb, column, row, value);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_STENCIL_INDEX, GL_UNSIGNED_BYTE span.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderStencilUbyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		                 const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLubyte *spanData;
    void (*store)(__GLstencilBuffer *sfb, GLint x, GLint y, GLint value);
    __GLstencilBuffer *sb = &gc->stencilBuffer;
    GLint endCol, startCol;
    GLint rows;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    store = sb->store;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLubyte*) span;
	i = startCol;
	do {
	    (*store)(sb, i, row, *spanData++);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_INT span.  gc->modes.rgbMode must 
** be GL_FALSE.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderCIuint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLuint *spanData;
    GLint rows;
    GLint startCol;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLushort *pixelArray;
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;
    mask = gc->frontBuffer.redMax;

    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = (GLushort *) spanInfo->pixelArray;
	spanData = (const GLuint*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].r =
		(__GLfloat) (*spanData++ & mask);
	    do {
		frag.x = column;
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_INT.  gc->modes.rgbMode must 
** be GL_FALSE.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderCIuint2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLuint *spanData;
    GLint endCol, startCol;
    GLint rows;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    mask = gc->frontBuffer.redMax;

    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLuint*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.x = i;
	    frag.color[__GL_PRIMARY_COLOR].r =
		(__GLfloat) (*spanData++ & mask);
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_SHORT span.  gc->modes.rgbMode must 
** be GL_FALSE.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderCIushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLushort *spanData;
    GLint rows;
    GLint startCol;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLshort *pixelArray;
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;
    mask = gc->frontBuffer.redMax;

    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLushort*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].r =
		(__GLfloat) (*spanData++ & mask);
	    do {
		frag.x = column;
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_SHORT.  gc->modes.rgbMode must 
** be GL_FALSE.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderCIushort2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			     const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLushort *spanData;
    GLint endCol, startCol;
    GLint rows;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    mask = gc->frontBuffer.redMax;

    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLushort*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.x = i;
	    frag.color[__GL_PRIMARY_COLOR].r =
		(__GLfloat) (*spanData++ & mask);
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_BYTE span.  gc->modes.rgbMode must 
** be GL_FALSE.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderCIubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLubyte *spanData;
    GLint rows;
    GLint startCol;
    GLfloat *indexMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    indexMap = (GLfloat*) gc->pixel.iCurMap;
    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLubyte*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].r = indexMap[*spanData++];
	    do {
		frag.x = column;
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_BYTE span.  Also, gc->modes.rgbMode 
** must be GL_FALSE.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderCIubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLubyte *spanData;
    GLint endCol, startCol;
    GLint rows;
    GLfloat *indexMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    indexMap = (GLfloat*) gc->pixel.iCurMap;
    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLubyte*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.x = i;
	    frag.color[__GL_PRIMARY_COLOR].r = indexMap[*spanData++];
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_BYTE span.  Also, gc->modes.rgbMode 
** must be GL_TRUE.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderCIubyte3(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLubyte *spanData;
    GLint rows;
    GLint startCol;
    GLfloat *redMap, *greenMap, *blueMap, *alphaMap;
    __GLfragment frag;
    GLubyte value;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    alphaMap = (GLfloat*) gc->pixel.alphaCurMap;
    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    rows = spanInfo->rows;
    startCol = spanInfo->startCol;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLubyte*) span;
	frag.y = row;

	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    value = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[value];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[value];
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[value];
	    frag.color[__GL_PRIMARY_COLOR].a = alphaMap[value];
	    do {
		frag.x = column;
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_UNSIGNED_BYTE span.  Also, gc->modes.rgbMode 
** must be GL_TRUE.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderCIubyte4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row;
    GLint i;
    GLint rowadd, coladd;
    const GLubyte *spanData;
    GLint endCol, startCol;
    GLint rows;
    GLubyte value;
    GLfloat *redMap, *greenMap, *blueMap, *alphaMap;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);

    redMap = (GLfloat*) gc->pixel.redCurMap;
    greenMap = (GLfloat*) gc->pixel.greenCurMap;
    blueMap = (GLfloat*) gc->pixel.blueCurMap;
    alphaMap = (GLfloat*) gc->pixel.alphaCurMap;
    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;
    startCol = spanInfo->startCol;
    endCol = spanInfo->endCol;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	spanData = (const GLubyte*) span;
	frag.y = row;
	i = startCol;
	do {
	    frag.x = i;
	    value = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].r = redMap[value];
	    frag.color[__GL_PRIMARY_COLOR].g = greenMap[value];
	    frag.color[__GL_PRIMARY_COLOR].b = blueMap[value];
	    frag.color[__GL_PRIMARY_COLOR].a = alphaMap[value];
	    (*store)(gc->drawBuffer, &frag);
	    i += coladd;
	} while (i != endCol);
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_RGBA, scaled (by the implementation color scales) GL_FLOAT span.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    store = gc->procs.pxStore;

    rows = spanInfo->rows;
    frag.z = spanInfo->fragz;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLfloat*) span;
	frag.y = row;
	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].r = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].g = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].b = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].a = *spanData++;
	    do {
		frag.x = column;

		/* This procedure will do the rest */
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_RGBA, scaled (by the implementation color scales) GL_FLOAT span.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderRGBA2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    store = gc->procs.pxStore;

    frag.z = spanInfo->fragz;
    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	spanData = (const GLfloat*) span;
	frag.y = row;
	for (i=0; i<width; i++) {
	    frag.color[__GL_PRIMARY_COLOR].r = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].g = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].b = *spanData++;
	    frag.color[__GL_PRIMARY_COLOR].a = *spanData++;
	    frag.x = column;

	    /* This procedure will do the rest */
	    (*store)(gc->drawBuffer, &frag);
	    column += coladd;
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_DEPTH_COMPONENT, GL_FLOAT span.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderDepth(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLshort *pixelArray;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    store = gc->procs.pxStore;
    frag.color[__GL_PRIMARY_COLOR].r =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].r *
	gc->constants.redRescale;
    frag.color[__GL_PRIMARY_COLOR].g =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].g *
	gc->constants.greenRescale;
    frag.color[__GL_PRIMARY_COLOR].b =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].b *
	gc->constants.blueRescale;
    frag.color[__GL_PRIMARY_COLOR].a =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].a *
	gc->constants.alphaRescale;

    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLfloat*) span;
	frag.y = row;
	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;

	    frag.z = (__GLzValue) (*spanData++ * gc->depthBuffer.scale);

	    do {
		frag.x = column;

		/* This procedure will do the rest */
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_DEPTH_COMPONENT, GL_FLOAT.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderDepth2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		          const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;

    store = gc->procs.pxStore;
    frag.color[__GL_PRIMARY_COLOR].r =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].r *
	gc->constants.redRescale;
    frag.color[__GL_PRIMARY_COLOR].g =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].g *
	gc->constants.greenRescale;
    frag.color[__GL_PRIMARY_COLOR].b =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].b *
	gc->constants.blueRescale;
    frag.color[__GL_PRIMARY_COLOR].a =
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].a *
	gc->constants.alphaRescale;

    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	spanData = (const GLfloat*) span;
	frag.y = row;
	for (i=0; i<width; i++) {
	    frag.x = column;
	    frag.z = (__GLzValue) (*spanData++ * gc->depthBuffer.scale);

	    /* This procedure will do the rest */
	    (*store)(gc->drawBuffer, &frag);
	    column += coladd;
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_FLOAT span (gc->modes.rgbMode == GL_FALSE).
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderCI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		      const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLshort *pixelArray;
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;
    mask = gc->frontBuffer.redMax;

    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;

    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLfloat*) span;
	frag.y = row;
	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    frag.color[__GL_PRIMARY_COLOR].r =
		(__GLfloat) ((GLint) (*spanData++) & mask);
	    do {
		frag.x = column;

		/* This procedure will do the rest */
		(*store)(gc->drawBuffer, &frag);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_COLOR_INDEX, GL_FLOAT span (gc->modes.rgbMode == GL_FALSE).
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderCI2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    __GLfragment frag;
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);
    GLint rows;
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;
    mask = gc->frontBuffer.redMax;

    store = gc->procs.pxStore;
    frag.z = spanInfo->fragz;

    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	spanData = (const GLfloat*) span;
	frag.y = row;
	for (i=0; i<width; i++) {
	    frag.color[__GL_PRIMARY_COLOR].r =
		(__GLfloat) ((GLint) (*spanData++) & mask);
	    frag.x = column;

	    /* This procedure will do the rest */
	    (*store)(gc->drawBuffer, &frag);
	    column += coladd;
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_STENCIL_INDEX, GL_FLOAT span.
**
** zoomx is assumed to be less than -1.0 or greater than 1.0.
*/
void __glSpanRenderStencil(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		           const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom, iright;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    GLint index;
    __GLstencilBuffer *sb;
    GLint rows;
    GLshort *pixelArray;
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;
    mask = (1<<gc->modes.stencilBits)-1;

    sb = &gc->stencilBuffer;

    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	pixelArray = spanInfo->pixelArray;
	spanData = (const GLfloat*) span;
	for (i=0; i<width; i++) {
	    iright = column + *pixelArray++;
	    index = (GLint) (*spanData++) & mask;
	    do {
		(*sb->store)(sb, column, row, index);
		column += coladd;
	    } while (column != iright);
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}

/*
** Render a GL_STENCIL_INDEX, GL_FLOAT span.
**
** zoomx is assumed to be less than or equal to 1.0 and greater than or equal 
** to -1.0.
*/
void __glSpanRenderStencil2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		            const GLvoid *span)
{
    __GLfloat zoomy;
    GLint itop, ibottom;
    GLint row, column;
    GLint i, width;
    GLint coladd, rowadd;
    const GLfloat *spanData;
    GLint index;
    __GLstencilBuffer *sb;
    GLint rows;
    GLint mask;

    zoomy = spanInfo->zoomy;
    rowadd = spanInfo->rowadd;
    coladd = spanInfo->coladd;
    ibottom = spanInfo->startRow;
    itop = (GLint) (spanInfo->y + zoomy);
    width = spanInfo->width;
    mask = (1<<gc->modes.stencilBits)-1;

    sb = &gc->stencilBuffer;

    rows = spanInfo->rows;
    for (row = ibottom; row != itop; row += rowadd) {
	if (rows == 0) break;
	rows--;
	column = spanInfo->startCol;
	spanData = (const GLfloat*) span;
	for (i=0; i<width; i++) {
	    index = (GLint) (*spanData++) & mask;
	    (*sb->store)(sb, column, row, index);
	    column += coladd;
	}
    }
    spanInfo->rows = rows;
    spanInfo->startRow = itop;
}
