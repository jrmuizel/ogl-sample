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
 */

#include "context.h"
#include "global.h"
#include "pixel.h"
#include "g_imports.h"
#include "g_imfncs.h"

/*
** Apply the post convolution scale and bias.
** Both in and out span data are RGBA float
*/
void __glSpanPostConvScaleBiasRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				   const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat rs, gs, bs, as;
    GLfloat rb, gb, bb, ab;

    rs = gc->state.pixel.transferMode.postConvolutionScale.r;
    gs = gc->state.pixel.transferMode.postConvolutionScale.g;
    bs = gc->state.pixel.transferMode.postConvolutionScale.b;
    as = gc->state.pixel.transferMode.postConvolutionScale.a;

    /*
    ** This "scaling" of the bias values is necessitated by color scaling
    ** which has already been applied to the pixels, and hence needs to be
    ** applied here.
    */
    rb = gc->state.pixel.transferMode.postConvolutionBias.r * 
	gc->frontBuffer.redScale;
    gb = gc->state.pixel.transferMode.postConvolutionBias.g *
	gc->frontBuffer.greenScale;
    bb = gc->state.pixel.transferMode.postConvolutionBias.b *
	gc->frontBuffer.blueScale;
    ab = gc->state.pixel.transferMode.postConvolutionBias.a *
	gc->frontBuffer.alphaScale;

    while ( width-- > 0 ) {
	*out++ = *in++ * rs + rb;
	*out++ = *in++ * gs + gb;
	*out++ = *in++ * bs + bb;
	*out++ = *in++ * as + ab;
    }
}

/*
** Intermediate results are stored in a spanCache during the application
** of a 2D convolution filter.  The number of entries in the spanCache
** that are used is equal to the height of the filter being applied.
**
** For example:  If filter rows are labeled with letters and image spans
** are labeled with integers and the height of the 2D filter is 3, then
** the span cache will contain the following while processing the nth
** span of the source image:
**
**   spanCache[row] : spanCache data
**     (n+1) % 3    :   A*(n-2) + B*(n-1) + C*(n)
**     (n+2) % 3    :   A*(n-1) + B*(n)
**     (n) % 3      :   A*(n)
**
** Each source image span which contributes to the destination image is
** processed exactly once, regardless of the filter type or border mode.
**
** The number of times that the current __glSpanConvolution*() routine is
** called during the processing of an image depends on the border mode of
** the filter.  For a REDUCE border mode, the __glSpanConvolution*() routine
** will be called once for each span of the source image.  For CONSTANT or
** REPLICATE border modes, the __glSpanConvolution*() routine will be called
** an additional (filterHeight-1) - (filterHeight/2) times in order to
** flush out the spanCache.
**
** Special care must be taken during the processing of the first and last
** spans of the source image for CONSTANT and REPLICATE border modes.
*/

/*
** Routines to apply a 1D convolution to a span of pixels.
** Both in and out span data are RGBA float.
*/

void __glSpanConvolution1DReduceRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLfloat **spanCache = (GLfloat **) &outspan;
    GLint width = spanInfo->width + gc->pixel.adjustWidth2D;

    __GL_MEMZERO(outspan,width*4*sizeof(GLfloat)); /* RGBA */
    (*spanInfo->convolveRows)(gc, spanInfo->spanCount,
			spanInfo->filter, 0, 0,
			width, 1, inspan,
			0, spanCache);
    spanInfo->width = width;
}

void __glSpanConvolution1DBorderRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLfloat **spanCache = (GLfloat **) &outspan;
    GLint width = spanInfo->width;

    __GL_MEMZERO(outspan,width*4*sizeof(GLfloat)); /* RGBA */
    (*spanInfo->convolveRows)(gc, spanInfo->spanCount,
			spanInfo->filter, 0, 0,
			width, 1, inspan,
			0, spanCache);
}

/*
** Routines to apply a 2D convolution to a span of pixels.
** Both in and out span data are RGBA float.
*/

void __glSpanConvolution2DReduceRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint spanCount = spanInfo->spanCount;
    GLint convInitialSpans = spanInfo->convInitialSpans;
    GLfloat **spanCache = (GLfloat **)spanInfo->spanCache;
    GLint width = spanInfo->width + gc->pixel.adjustWidth2D;
    GLint height = spanInfo->height;
    GLint filterHeight = spanInfo->filter->height;
    GLint cacheRow = (spanCount*(filterHeight-1)) % filterHeight;

    if (spanCount < height) {
	if (spanCount < filterHeight) {
	    /* initialize spanCache */
	    __GL_MEMZERO(spanCache[cacheRow],width*4*sizeof(GLfloat));
	    (*spanInfo->convolveRows)(gc, spanCount,
	    			spanInfo->filter, 0, spanCount,
				width, height, inspan,
				cacheRow, spanCache);
	    if (spanCount < convInitialSpans) {
		return;
	    }

	} else {
	    /* convolve row of the source image and accumulate into spanCache */
	    (*spanInfo->convolveRows)(gc, spanCount,
	    			spanInfo->filter, 0, filterHeight-1,
				width, height, inspan,
				cacheRow, spanCache);
	}
    }

    /* copy a line of the spanCache to the output and adjust the span width */
    cacheRow = (cacheRow+(filterHeight-1)) % filterHeight;
    __GL_MEMMOVE(outspan,spanCache[cacheRow],width*4*sizeof(GLfloat));
    __GL_MEMZERO(spanCache[cacheRow],width*4*sizeof(GLfloat));
    spanInfo->width = width;
}

void __glSpanConvolution2DBorderRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint spanCount = spanInfo->spanCount;
    GLint convInitialSpans = spanInfo->convInitialSpans;
    GLfloat **spanCache = (GLfloat **)spanInfo->spanCache;
    GLint width = spanInfo->width;
    GLint height = spanInfo->height;
    GLint filterHeight = spanInfo->filter->height;
    GLint rowOffset = filterHeight / 2;
    GLint cacheRow = (spanCount*(filterHeight-1)) % filterHeight;
    GLint row;

    if (spanCount < height) {
	if (spanCount == 0) {
	    /* initialize spanCache rows touched by the first span */
	    for (row=0; row<filterHeight; ++row) {
		__GL_MEMZERO(spanCache[row],width*4*sizeof(GLfloat));
	    }
	    for (row=0; row<=rowOffset; ++row) {
		(*spanInfo->convolveRows)(gc, spanCount+row-rowOffset,
				spanInfo->filter, 0, row,
				width, height, inspan,
				rowOffset-row, spanCache);
	    }
	    return;

	} else if (spanCount < convInitialSpans) {
	    /* initialize remaining spanCache rows */
	    (*spanInfo->convolveRows)(gc, spanCount,
	    			spanInfo->filter, 0, spanCount+rowOffset,
				width, height, inspan,
				cacheRow, spanCache);
	    return;

	} else {
	    /* convolve row of the source image and accumulate into spanCache */
	    (*spanInfo->convolveRows)(gc, spanCount,
	    			spanInfo->filter, 0, filterHeight-1,
				width, height, inspan,
				cacheRow, spanCache);

	    if (spanCount == (height-1)) {
		/* finish spanCache rows touched by the last span */
		for (row=1; row<=rowOffset; ++row) {
		    (*spanInfo->convolveRows)(gc, spanCount+row,
		    	spanInfo->filter,
		    	(row-rowOffset)+(filterHeight-1), filterHeight-1,
			width, height, inspan,
			(cacheRow+row*(filterHeight-1)) % filterHeight,
			spanCache);
		}
	    }
	}
    }

    /* copy a line of the spanCache to the output and adjust the span width */
    cacheRow = (cacheRow+(filterHeight-1)) % filterHeight;
    __GL_MEMMOVE(outspan,spanCache[cacheRow],width*4*sizeof(GLfloat));
    __GL_MEMZERO(spanCache[cacheRow],width*4*sizeof(GLfloat));
    spanInfo->width = width;
}

/*
** Routines to apply a convolution filter to a span of pixels updating all
** of the rows of the spanCache which are affected by the span.
** Both in and out span data are RGBA float.  The suffix of the routine
** name indicates border mode and internal format of the active filter.
*/

void __glConvolveRowsReduceL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* L */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = (GLfloat) __glZero;
	    a = in[3];
	    for (f=0; f<filterWidth; ++f) {
		const GLfloat *src = &in[f*4]; /* RGBA */

		r += src[0] * filterRow[f];
		g += src[1] * filterRow[f];
		b += src[2] * filterRow[f];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ = a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReduceLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*2*filterWidth; /* LA */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;
	    for (f=0; f<filterWidth; ++f) {
		const GLfloat *src = &in[f*4]; /* RGBA */
		GLint fi = f*2; /* LA */

		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+0];
		b += src[2] * filterRow[fi+0];
		a += src[3] * filterRow[fi+1];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReduceA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* A */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = in[0];
	    g = in[1];
	    b = in[2];
	    a = (GLfloat) __glZero;
	    for (f=0; f<filterWidth; ++f) {
		const GLfloat *src = &in[f*4]; /* RGBA */

		a += src[3] * filterRow[f];
	    }
	    *out++ = r;
	    *out++ = g;
	    *out++ = b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReduceI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* I */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;
	    for (f=0; f<filterWidth; ++f) {
		const GLfloat *src = &in[f*4]; /* RGBA */

		r += src[0] * filterRow[f];
		g += src[1] * filterRow[f];
		b += src[2] * filterRow[f];
		a += src[3] * filterRow[f];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReduceRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*3*filterWidth; /* RGB */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = (GLfloat) __glZero;
	    a = in[3];
	    for (f=0; f<filterWidth; ++f) {
		const GLfloat *src = &in[f*4]; /* RGBA */
		GLint fi = f*3; /* RGB */

		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+1];
		b += src[2] * filterRow[fi+2];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ = a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReduceRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*4*filterWidth; /* RGBA */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;
	    for (f=0; f<filterWidth; ++f) {
		const GLfloat *src = &in[f*4]; /* RGBA */
		GLint fi = f*4; /* RGBA */

		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+1];
		b += src[2] * filterRow[fi+2];
		a += src[3] * filterRow[fi+3];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsConstantL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* L */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = (GLfloat) __glZero;
	    a = in[3];

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		const GLfloat *src;

		if ((srcCol < 0) || (srcCol >= width) ||
		    (spanCount < 0) || (spanCount >= height)) {
		    src = &filter->state.borderColor.r;
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[f];
		g += src[1] * filterRow[f];
		b += src[2] * filterRow[f];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ = a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsConstantLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*2*filterWidth; /* LA */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		GLint fi = f*2; /* LA */
		const GLfloat *src;

		if ((srcCol < 0) || (srcCol >= width) ||
		    (spanCount < 0) || (spanCount >= height)) {
		    src = &filter->state.borderColor.r;
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+0];
		b += src[2] * filterRow[fi+0];
		b += src[3] * filterRow[fi+1];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsConstantA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* A */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = in[0];
	    g = in[1];
	    b = in[2];
	    a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		const GLfloat *src;

		if ((srcCol < 0) || (srcCol >= width) ||
		    (spanCount < 0) || (spanCount >= height)) {
		    src = &filter->state.borderColor.r;
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		a += src[3] * filterRow[f];
	    }
	    *out++ = r;
	    *out++ = g;
	    *out++ = b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsConstantI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* I */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		const GLfloat *src;

		if ((srcCol < 0) || (srcCol >= width) ||
		    (spanCount < 0) || (spanCount >= height)) {
		    src = &filter->state.borderColor.r;
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[f];
		g += src[1] * filterRow[f];
		b += src[2] * filterRow[f];
		b += src[3] * filterRow[f];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsConstantRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*3*filterWidth; /* RGB */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = (GLfloat) __glZero;
	    a = in[3];

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		GLint fi = f*3; /* RGB */
		const GLfloat *src;

		if ((srcCol < 0) || (srcCol >= width) ||
		    (spanCount < 0) || (spanCount >= height)) {
		    src = &filter->state.borderColor.r;
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+1];
		b += src[2] * filterRow[fi+2];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ = a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsConstantRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*4*filterWidth; /* RGBA */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		GLint fi = f*4; /* RGBA */
		const GLfloat *src;

		if ((srcCol < 0) || (srcCol >= width) ||
		    (spanCount < 0) || (spanCount >= height)) {
		    src = &filter->state.borderColor.r;
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+1];
		b += src[2] * filterRow[fi+2];
		a += src[3] * filterRow[fi+3];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReplicateL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* L */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = (GLfloat) __glZero;
	    a = in[3];

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		const GLfloat *src;

		if (srcCol < 0) {
		    src = &inspan[0]; /* RGBA */
		} else if (srcCol >= width) {
		    src = &inspan[(width-1)*4]; /* RGBA */
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[f];
		g += src[1] * filterRow[f];
		b += src[2] * filterRow[f];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ = a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReplicateLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*2*filterWidth; /* LA */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		GLint fi = f*2; /* LA */
		const GLfloat *src;

		if (srcCol < 0) {
		    src = &inspan[0]; /* RGBA */
		} else if (srcCol >= width) {
		    src = &inspan[(width-1)*4]; /* RGBA */
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+0];
		b += src[2] * filterRow[fi+0];
		a += src[3] * filterRow[fi+1];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReplicateA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* A */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = in[0];
	    g = in[1];
	    b = in[2];
	    a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		const GLfloat *src;

		if (srcCol < 0) {
		    src = &inspan[0]; /* RGBA */
		} else if (srcCol >= width) {
		    src = &inspan[(width-1)*4]; /* RGBA */
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		a += src[0] * filterRow[f];
	    }
	    *out++ = r;
	    *out++ = g;
	    *out++ = b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReplicateI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth; /* I */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		const GLfloat *src;

		if (srcCol < 0) {
		    src = &inspan[0]; /* RGBA */
		} else if (srcCol >= width) {
		    src = &inspan[(width-1)*4]; /* RGBA */
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[f];
		g += src[1] * filterRow[f];
		b += src[2] * filterRow[f];
		a += src[3] * filterRow[f];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReplicateRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*1*filterWidth;

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = (GLfloat) __glZero;
	    a = in[3];

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		GLint fi = f*3; /* RGB */
		const GLfloat *src;

		if (srcCol < 0) {
		    src = &inspan[0]; /* RGBA */
		} else if (srcCol >= width) {
		    src = &inspan[(width-1)*4]; /* RGBA */
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+1];
		b += src[2] * filterRow[fi+2];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ = a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsReplicateRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    GLint row, col;

    for (row=firstRow; row<=lastRow; ++row) {
	const GLfloat *in = (const GLfloat *) inspan;
	GLfloat *out = spanCache[(cacheRow+row) % filterHeight];
	__GLfloat *filterRow = filter->filter + row*4*filterWidth; /* RGBA */

	for (col=0; col<width; ++col) {
	    GLfloat r, g, b, a; 
	    GLint f;

	    r = g = b = a = (GLfloat) __glZero;

	    for (f=0; f<filterWidth; ++f) {
		GLint srcCol = col+f-colOffset;
		GLint fi = f*4; /* RGBA */
		const GLfloat *src;

		if (srcCol < 0) {
		    src = &inspan[0]; /* RGBA */
		} else if (srcCol >= width) {
		    src = &inspan[(width-1)*4]; /* RGBA */
		} else {
		    src = &in[(f-colOffset)*4]; /* RGBA */
		}
		r += src[0] * filterRow[fi+0];
		g += src[1] * filterRow[fi+1];
		b += src[2] * filterRow[fi+2];
		a += src[3] * filterRow[fi+3];
	    }
	    *out++ += r;
	    *out++ += g;
	    *out++ += b;
	    *out++ += a;
	    in += 4; /* RGBA */
	}
    }
}

void __glConvolveRowsSeparableReduceL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* L */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = (GLfloat) __glZero;
	a = in[3];

        for (f=0; f<filterWidth; ++f) {
	    const GLfloat *src = &in[f*4]; /* RGBA */

	    r += src[0] * filterRow[f];
	    g += src[1] * filterRow[f];
	    b += src[2] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0] += r * filterCol[row];
	    out[1] += g * filterCol[row];
	    out[2] += b * filterCol[row];
	    out[3]  = a ;
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReduceLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 2*filterWidth; /* LA */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    const GLfloat *src = &in[f*4]; /* RGBA */
	    GLint fi = f*2; /* LA */

	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+0];
	    b += src[2] * filterRow[fi+0];
	    a += src[3] * filterRow[fi+1];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*2; /* LA */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+0];
	    out[2] += b * filterCol[ri+0];
	    out[3] += a * filterCol[ri+1];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReduceA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* A */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = in[0];
	g = in[1];
	b = in[2];
	a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    const GLfloat *src = &in[f*4]; /* RGBA */

	    a += src[3] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0]  = r;
	    out[1]  = g;
	    out[2]  = b;
	    out[3] += a * filterCol[row];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReduceI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* I */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    const GLfloat *src = &in[f*4]; /* RGBA */

	    r += src[0] * filterRow[f];
	    g += src[1] * filterRow[f];
	    b += src[2] * filterRow[f];
	    a += src[3] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0] += r * filterCol[row+0];
	    out[1] += g * filterCol[row+0];
	    out[2] += b * filterCol[row+0];
	    out[3] += a * filterCol[row+1];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReduceRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 3*filterWidth; /* RGB */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;
	a = in[3];

        for (f=0; f<filterWidth; ++f) {
	    const GLfloat *src = &in[f*4]; /* RGBA */
	    GLint fi = f*3; /* RGB */

	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+1];
	    b += src[2] * filterRow[fi+2];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*3; /* RGB */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+1];
	    out[2] += b * filterCol[ri+2];
	    out[3]  = a;
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReduceRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 4*filterWidth; /* RGBA */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    const GLfloat *src = &in[f*4]; /* RGBA */
	    GLint fi = f*4; /* RGBA */

	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+1];
	    b += src[2] * filterRow[fi+2];
	    a += src[3] * filterRow[fi+3];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*4; /* RGBA */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+1];
	    out[2] += b * filterCol[ri+2];
	    out[3] += a * filterCol[ri+3];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableConstantL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* L */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = (GLfloat) __glZero;
	a = in[3];

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    const GLfloat *src;

	    if ((srcCol < 0) || (srcCol >= width) ||
		(spanCount < 0) || (spanCount >= height)) {
		src = &filter->state.borderColor.r;
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[f];
	    g += src[1] * filterRow[f];
	    b += src[2] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0] += r * filterCol[row];
	    out[1] += g * filterCol[row];
	    out[2] += b * filterCol[row];
	    out[3]  = a ;
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableConstantLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 2*filterWidth; /* LA */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    GLint fi = f*2; /* LA */
	    const GLfloat *src;

	    if ((srcCol < 0) || (srcCol >= width) ||
		(spanCount < 0) || (spanCount >= height)) {
		src = &filter->state.borderColor.r;
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+0];
	    b += src[2] * filterRow[fi+0];
	    a += src[3] * filterRow[fi+1];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*2; /* LA */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+0];
	    out[2] += b * filterCol[ri+0];
	    out[3] += a * filterCol[ri+1];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableConstantA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* A */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = in[0];
	g = in[1];
	b = in[2];
	a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    const GLfloat *src;

	    if ((srcCol < 0) || (srcCol >= width) ||
		(spanCount < 0) || (spanCount >= height)) {
		src = &filter->state.borderColor.r;
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    a += src[3] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0]  = r;
	    out[1]  = g;
	    out[2]  = b;
	    out[3] += a * filterCol[row];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableConstantI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* I */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    const GLfloat *src;

	    if ((srcCol < 0) || (srcCol >= width) ||
		(spanCount < 0) || (spanCount >= height)) {
		src = &filter->state.borderColor.r;
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[f];
	    g += src[1] * filterRow[f];
	    b += src[2] * filterRow[f];
	    a += src[3] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0] += r * filterCol[row];
	    out[1] += g * filterCol[row];
	    out[2] += b * filterCol[row];
	    out[3] += a * filterCol[row];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableConstantRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 3*filterWidth; /* RGB */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = (GLfloat) __glZero;
	a = in[3];

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    GLint fi = f*3; /* RGB */
	    const GLfloat *src;

	    if ((srcCol < 0) || (srcCol >= width) ||
		(spanCount < 0) || (spanCount >= height)) {
		src = &filter->state.borderColor.r;
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+1];
	    b += src[2] * filterRow[fi+2];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*3; /* RGB */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+1];
	    out[2] += b * filterCol[ri+2];
	    out[3]  = a;
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableConstantRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 4*filterWidth; /* RGBA */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    GLint fi = f*4; /* RGBA */
	    const GLfloat *src;

	    if ((srcCol < 0) || (srcCol >= width) ||
		(spanCount < 0) || (spanCount >= height)) {
		src = &filter->state.borderColor.r;
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+1];
	    b += src[2] * filterRow[fi+2];
	    a += src[3] * filterRow[fi+3];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*4; /* RGBA */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+1];
	    out[2] += b * filterCol[ri+2];
	    out[3] += a * filterCol[ri+3];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReplicateL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* L */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = (GLfloat) __glZero;
	a = in[3];

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    const GLfloat *src;

	    if (srcCol < 0) {
		src = &inspan[0]; /* RGBA */
	    } else if (srcCol >= width) {
		src = &inspan[(width-1)*4]; /* RGBA */
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[f];
	    g += src[1] * filterRow[f];
	    b += src[2] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0] += r * filterCol[row];
	    out[1] += g * filterCol[row];
	    out[2] += b * filterCol[row];
	    out[3]  = a ;
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReplicateLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 2*filterWidth; /* LA */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    GLint fi = f*2; /* LA */
	    const GLfloat *src;

	    if (srcCol < 0) {
		src = &inspan[0]; /* RGBA */
	    } else if (srcCol >= width) {
		src = &inspan[(width-1)*4]; /* RGBA */
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+0];
	    b += src[2] * filterRow[fi+0];
	    a += src[3] * filterRow[fi+1];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*2; /* LA */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+0];
	    out[2] += b * filterCol[ri+0];
	    out[3] += a * filterCol[ri+1];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReplicateA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* A */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = in[0];
	g = in[1];
	b = in[2];
	a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    const GLfloat *src;

	    if (srcCol < 0) {
		src = &inspan[0]; /* RGBA */
	    } else if (srcCol >= width) {
		src = &inspan[(width-1)*4]; /* RGBA */
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    a += src[3] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0]  = r;
	    out[1]  = g;
	    out[2]  = b;
	    out[3] += a * filterCol[row];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReplicateI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 1*filterWidth; /* I */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    const GLfloat *src;

	    if (srcCol < 0) {
		src = &inspan[0]; /* RGBA */
	    } else if (srcCol >= width) {
		src = &inspan[(width-1)*4]; /* RGBA */
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[f];
	    g += src[1] * filterRow[f];
	    b += src[2] * filterRow[f];
	    a += src[3] * filterRow[f];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */

	    out[0] += r * filterCol[row];
	    out[1] += g * filterCol[row];
	    out[2] += b * filterCol[row];
	    out[3] += a * filterCol[row];
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReplicateRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 3*filterWidth; /* RGB */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = (GLfloat) __glZero;
	a = in[3];

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    GLint fi = f*3; /* RGB */
	    const GLfloat *src;

	    if (srcCol < 0) {
		src = &inspan[0]; /* RGBA */
	    } else if (srcCol >= width) {
		src = &inspan[(width-1)*4]; /* RGBA */
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+1];
	    b += src[2] * filterRow[fi+2];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*3; /* RGB */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+1];
	    out[2] += b * filterCol[ri+2];
	    out[3]  = a;
	}
	in += 4; /* RGBA */
    }
}

void __glConvolveRowsSeparableReplicateRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache)
{
    const GLfloat *in = (const GLfloat *) inspan;
    GLint filterWidth = filter->width;
    GLint filterHeight = filter->height;
    GLint colOffset = filterWidth / 2;
    __GLfloat *filterRow = filter->filter;
    __GLfloat *filterCol = filter->filter + 4*filterWidth; /* RGBA */
    GLint row, col;

    for (col=0; col<width; ++col) {
        GLfloat r, g, b, a; 
	GLint f;

	r = g = b = a = (GLfloat) __glZero;

        for (f=0; f<filterWidth; ++f) {
	    GLint srcCol = col+f-colOffset;
	    GLint fi = f*4; /* RGBA */
	    const GLfloat *src;

	    if (srcCol < 0) {
		src = &inspan[0]; /* RGBA */
	    } else if (srcCol >= width) {
		src = &inspan[(width-1)*4]; /* RGBA */
	    } else {
		src = &in[(f-colOffset)*4]; /* RGBA */
	    }
	    r += src[0] * filterRow[fi+0];
	    g += src[1] * filterRow[fi+1];
	    b += src[2] * filterRow[fi+2];
	    a += src[3] * filterRow[fi+3];
	}
	for (row=firstRow; row<=lastRow; ++row) {
	    GLint outRow = (cacheRow+row) % filterHeight;
	    GLfloat *out = &spanCache[outRow][col*4]; /* RGBA */
	    GLint ri = row*4; /* RGBA */

	    out[0] += r * filterCol[ri+0];
	    out[1] += g * filterCol[ri+1];
	    out[2] += b * filterCol[ri+2];
	    out[3] += a * filterCol[ri+3];
	}
	in += 4; /* RGBA */
    }
}

