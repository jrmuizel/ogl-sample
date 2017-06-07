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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_hist.c#3 $
*/

#include "context.h"
#include "global.h"
#include "g_imports.h"
#include "pixel.h"

/*
** Span routines to find the minimum and maximum values of a given
** set of color components, and pass on the pixel values.
*/

void __glSpanMinmaxA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	*out++ = *in++;
	*out++ = *in++;
	*out++ = *in++;
	v = *out++ = *in++;
	v *= as;
	if(v < array[0]) { array[0] = v; }
	if(v > array[1]) { array[1] = v; }
    }
}

void __glSpanMinmaxL(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	v *= rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[1]) { array[1] = v; }
	*out++ = *in++;
	*out++ = *in++;
	*out++ = *in++;
    }
}

void __glSpanMinmaxLA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	v *= rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[2]) { array[2] = v; }
	*out++ = *in++;
	*out++ = *in++;
	v = *out++ = *in++;
	v *= as;
	if(v < array[1]) { array[1] = v; }
	if(v > array[3]) { array[3] = v; }
    }
}

void __glSpanMinmaxRGB(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	v *= rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[3]) { array[3] = v; }
	v = *out++ = *in++;
	v *= gs;
	if(v < array[1]) { array[1] = v; }
	if(v > array[4]) { array[4] = v; }
	v = *out++ = *in++;
	v *= bs;
	if(v < array[2]) { array[2] = v; }
	if(v > array[5]) { array[5] = v; }
	*out++ = *in++;
    }
}

void __glSpanMinmaxRGBA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	v *= rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[4]) { array[4] = v; }
	v = *out++ = *in++;
	v *= gs;
	if(v < array[1]) { array[1] = v; }
	if(v > array[5]) { array[5] = v; }
	v = *out++ = *in++;
	v *= bs;
	if(v < array[2]) { array[2] = v; }
	if(v > array[6]) { array[6] = v; }
	v = *out++ = *in++;
	v *= as;
	if(v < array[3]) { array[3] = v; }
	if(v > array[7]) { array[7] = v; }
    }
}

/*
** Span routines to find the minimum and maximum values of a given
** set of color components, and "sink" the pixel values.
*/

/*ARGSUSED*/
void __glSpanMinmaxSinkA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat as = gc->frontBuffer.oneOverRedScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	in++;
	in++;
	in++;
	v = *in++ *as;
	if(v < array[0]) { array[0] = v; }
	if(v > array[1]) { array[1] = v; }
    }
}

/*ARGSUSED*/
void __glSpanMinmaxSinkL(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++ * rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[1]) { array[1] = v; }
	in++;
	in++;
	in++;
    }
}

/*ARGSUSED*/
void __glSpanMinmaxSinkLA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++ *rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[2]) { array[2] = v; }
	in++;
	in++;
	v = *in++ * as;
	if(v < array[1]) { array[1] = v; }
	if(v > array[3]) { array[3] = v; }
    }
}

/*ARGSUSED*/
void __glSpanMinmaxSinkRGB(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++ * rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[3]) { array[3] = v; }
	v = *in++ * gs;
	if(v < array[1]) { array[1] = v; }
	if(v > array[4]) { array[4] = v; }
	v = *in++ * bs;
	if(v < array[2]) { array[2] = v; }
	if(v > array[5]) { array[5] = v; }
	in++;
    }
}

/*ARGSUSED*/
void __glSpanMinmaxSinkRGBA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *array = gc->pixel.minmax.array;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++ * rs;
	if(v < array[0]) { array[0] = v; }
	if(v > array[4]) { array[4] = v; }
	v = *in++ * gs;
	if(v < array[1]) { array[1] = v; }
	if(v > array[5]) { array[5] = v; }
	v = *in++ * bs;
	if(v < array[2]) { array[2] = v; }
	if(v > array[6]) { array[6] = v; }
	v = *in++ * as;
	if(v < array[3]) { array[3] = v; }
	if(v > array[7]) { array[7] = v; }
    }
}

/*
** Temporary Function to make code simplier. This should be replaced
** with something faster.
*/
static GLint ClampZeroMax(GLint in, GLint max)
{
    if (in < 0)
	return 0;
    if (in > max)
	return max;
    return in;
}

/*
** Span routines to find the histogram of a given
** set of color components, and pass on the pixel values.
*/

void __glSpanHistogramA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	*out++ = *in++;
	*out++ = *in++;
	*out++ = *in++;
	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * as + half), maxEntry)]++;
    }
}

void __glSpanHistogramL(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry)]++;
	*out++ = *in++;
	*out++ = *in++;
	*out++ = *in++;
    }
}

void __glSpanHistogramLA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry) * 2 + 0]++;
	*out++ = *in++;
	*out++ = *in++;
	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * as + half), maxEntry) * 2 + 1]++;
    }
}

void __glSpanHistogramRGB(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale * scale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry) * 3 + 0]++;
	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * gs + half), maxEntry) * 3 + 1]++;
	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * bs + half), maxEntry) * 3 + 2]++;
	*out++ = *in++;
    }
}

void __glSpanHistogramRGBA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLfloat *out = (GLfloat *)outspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale * scale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale * scale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry) * 4 + 0]++;
	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * gs + half), maxEntry) * 4 + 1]++;
	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * bs + half), maxEntry) * 4 + 2]++;
	v = *out++ = *in++;
	array[ClampZeroMax((GLint)(v * as + half), maxEntry) * 4 + 3]++;
    }
}

/*
** Span routines to find the histogram of a given
** set of color components, and "sink" the pixel values.
*/

/*ARGSUSED*/
void __glSpanHistogramSinkA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	in++;
	in++;
	in++;
	v = *in++;
	array[ClampZeroMax((GLint)(v * as + half), maxEntry)]++;
    }
}

/*ARGSUSED*/
void __glSpanHistogramSinkL(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry)]++;
	in++;
	in++;
	in++;
    }
}

/*ARGSUSED*/
void __glSpanHistogramSinkLA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry) * 2 + 0]++;
	in++;
	in++;
	v = *in++;
	array[ClampZeroMax((GLint)(v * as + half), maxEntry) * 2 + 1]++;
    }
}

/*ARGSUSED*/
void __glSpanHistogramSinkRGB(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale * scale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry) * 3 + 0]++;
	v = *in++;
	array[ClampZeroMax((GLint)(v * gs + half), maxEntry) * 3 + 1]++;
	v = *in++;
	array[ClampZeroMax((GLint)(v * bs + half), maxEntry) * 3 + 2]++;
	in++;
    }
}

/*ARGSUSED*/
void __glSpanHistogramSinkRGBA(__GLcontext *gc,
	__GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width;
    const GLfloat *in = (const GLfloat *)inspan;
    GLuint *array = gc->pixel.histogram.array;
    GLuint maxEntry = gc->pixel.histogram.width - 1;
    GLfloat scale = (GLfloat) maxEntry;
    GLfloat rs = gc->frontBuffer.oneOverRedScale * scale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale * scale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale * scale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale * scale;
    GLfloat half = __glHalf;

    while ( width-- > 0 ) {
        __GLfloat v;

	v = *in++;
	array[ClampZeroMax((GLint)(v * rs + half), maxEntry) * 4 + 0]++;
	v = *in++;
	array[ClampZeroMax((GLint)(v * gs + half), maxEntry) * 4 + 1]++;
	v = *in++;
	array[ClampZeroMax((GLint)(v * bs + half), maxEntry) * 4 + 2]++;
	v = *in++;
	array[ClampZeroMax((GLint)(v * as + half), maxEntry) * 4 + 3]++;
    }
}

