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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_hist.c#12 $
*/
#include "context.h"
#include "g_imports.h"
#include "global.h"
#include "g_imfncs.h"
#include "types.h"
#include "namesint.h"
#include "pixel.h"
#include "image.h"
#include "glmath.h"
#include "size.h"
#include <memory.h>

static __GLhistogram *LookupHistogram(__GLcontext *gc,
			GLenum target, GLboolean *isProxy)
{
    __GLhistogram *hg;

    switch (target) {
    case GL_HISTOGRAM:
	*isProxy = GL_FALSE;
	hg = &gc->pixel.histogram;
	break;
    case GL_PROXY_HISTOGRAM:
	*isProxy = GL_TRUE;
	hg = &gc->pixel.proxyHistogram;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return NULL;
    }
    return hg;
}

static GLenum CheckHistogramArgs(__GLcontext *gc,
		GLenum target, GLsizei width,
		GLenum internalFormat, GLboolean sink)
{
    switch (target) {
    case GL_HISTOGRAM:
    case GL_PROXY_HISTOGRAM:
        break;
    default:
	return GL_INVALID_ENUM;
    }

    switch (internalFormat) {
    case GL_ALPHA:
    case GL_ALPHA4:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
	break;
    case GL_LUMINANCE:
    case GL_LUMINANCE4:
    case GL_LUMINANCE8:
    case GL_LUMINANCE12:
    case GL_LUMINANCE16:
	break;
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE4_ALPHA4:
    case GL_LUMINANCE6_ALPHA2:
    case GL_LUMINANCE8_ALPHA8:
    case GL_LUMINANCE12_ALPHA4:
    case GL_LUMINANCE12_ALPHA12:
    case GL_LUMINANCE16_ALPHA16:
	break;
    case GL_RGB:
    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
	break;
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGBA8:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGB5_A1:
    case GL_RGB10_A2:
	break;
    default:
	return GL_INVALID_ENUM;
    }

    if ((width < 0) || (width & (width - 1))) {
	return GL_INVALID_VALUE;
    }

    return 0;
}

static GLenum AllocHistogramArray(__GLcontext *gc, __GLhistogram *hg,
		GLboolean isProxy, GLsizei width,
		GLenum internalFormat, GLboolean sink)
{
    GLenum baseFormat, type;
    GLint redSize, greenSize, blueSize, alphaSize, luminanceSize;
    GLint bufferSize;

    switch (internalFormat) {
    case GL_ALPHA:
    case GL_ALPHA4:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
	baseFormat = GL_ALPHA;
	type = GL_UNSIGNED_INT;
	redSize = 0;
	greenSize = 0;
	blueSize = 0;
	alphaSize = sizeof(GLuint) * 8;
	luminanceSize = 0;
	break;
    case GL_LUMINANCE:
    case GL_LUMINANCE4:
    case GL_LUMINANCE8:
    case GL_LUMINANCE12:
    case GL_LUMINANCE16:
	baseFormat = GL_LUMINANCE;
	type = GL_UNSIGNED_INT;
	redSize = 0;
	greenSize = 0;
	blueSize = 0;
	alphaSize = 0;
	luminanceSize = sizeof(GLuint) * 8;
	break;
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE4_ALPHA4:
    case GL_LUMINANCE6_ALPHA2:
    case GL_LUMINANCE8_ALPHA8:
    case GL_LUMINANCE12_ALPHA4:
    case GL_LUMINANCE12_ALPHA12:
    case GL_LUMINANCE16_ALPHA16:
	baseFormat = GL_LUMINANCE_ALPHA;
	type = GL_UNSIGNED_INT;
	redSize = 0;
	greenSize = 0;
	blueSize = 0;
	alphaSize = sizeof(GLuint) * 8;
	luminanceSize = sizeof(GLuint) * 8;
	break;
    case GL_RGB:
    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
	baseFormat = GL_RGB;
	type = GL_UNSIGNED_INT;
	redSize = sizeof(GLuint) * 8;
	greenSize = sizeof(GLuint) * 8;
	blueSize = sizeof(GLuint) * 8;
	alphaSize = 0;
	luminanceSize = 0;
	break;
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGBA8:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGB5_A1:
    case GL_RGB10_A2:
	baseFormat = GL_RGBA;
	type = GL_UNSIGNED_INT;
	redSize = sizeof(GLuint) * 8;
	greenSize = sizeof(GLuint) * 8;
	blueSize = sizeof(GLuint) * 8;
	alphaSize = sizeof(GLuint) * 8;
	luminanceSize = 0;
	break;
    default:
	assert(0);
	return GL_FALSE;
    }

    bufferSize = width *
    		__glElementsPerGroup(baseFormat, type) *
		__glBytesPerElement(type);

    /* Limit histogram array size so that pixel operations can be used */
    if (bufferSize > __GL_MAX_SPAN_SIZE) {
	hg->width = 0;
	hg->format = 0;
	hg->formatReturn = 0;
	hg->baseFormat = 0;
	hg->type = 0;
	hg->redSize = 0;
	hg->greenSize = 0;
	hg->blueSize = 0;
	hg->alphaSize = 0;
	hg->luminanceSize = 0;
	if (!isProxy) {
	    __glSetError(GL_TABLE_TOO_LARGE);
	}
	return GL_FALSE;
    }

    if (!isProxy) {
	/* histogram pointer is initialized to zero */
	hg->array = (GLuint *)(*gc->imports.realloc)(gc, hg->array, bufferSize);
	if (!hg->array && bufferSize > 0) {
	    __glSetError(GL_OUT_OF_MEMORY);
	    return GL_FALSE;
	}
    }

    hg->width = width;
    hg->formatReturn = internalFormat;
    hg->format =
    hg->baseFormat = baseFormat;
    hg->type = type;

    hg->redSize = redSize;
    hg->greenSize = greenSize;
    hg->blueSize = blueSize;
    hg->alphaSize = alphaSize;
    hg->luminanceSize = luminanceSize;

    hg->sink = sink;

    return GL_TRUE;
}

/*
** Zero out every element of histogram array.
*/
static void ResetHistogramArray(__GLcontext *gc, __GLhistogram *hg)
{
    __GL_MEMZERO(hg->array, hg->width *
		 __glElementsPerGroup(hg->baseFormat, hg->type) *
		 __glBytesPerElement(hg->type));
}

void GLAPI __glim_ResetHistogram(GLenum target)
{
    __GLhistogram *hg;
    GLboolean isProxy;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    hg = LookupHistogram(gc, target, &isProxy);
    if (!hg || isProxy) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    ResetHistogramArray(gc, hg);
}

void GLAPI __glim_Histogram(GLenum target, GLsizei width, 
			    GLenum internalFormat, GLboolean sink)
{
    __GLhistogram *hg;
    GLboolean isProxy;
    GLenum rv;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if ((rv = CheckHistogramArgs(gc, target, width, internalFormat, sink))) {
        __glSetError(rv);
	return;
    }
    if (!(hg = LookupHistogram(gc, target, &isProxy))) {
        __glSetError(GL_INVALID_ENUM);
	return;
    }

    if (!AllocHistogramArray(gc, hg, isProxy, width, internalFormat, sink)) {
        return;
    }

    if (isProxy || width == 0) {
        return;
    }

    ResetHistogramArray(gc, hg);
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_GetHistogram(GLenum target, GLboolean reset, GLenum format,
			       GLenum type, GLvoid *values)
{
    __GLhistogram *hg;
    GLboolean isProxy;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_GET();

    hg = LookupHistogram(gc, target, &isProxy);
    if (!hg || isProxy) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (format) { /* smaller set of allowable formats than HistogramEXT */
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_RGB:
    case GL_BGR:
    case GL_RGBA:
    case GL_BGRA:
    case GL_ABGR_EXT:
    case GL_LUMINANCE:
    case GL_LUMINANCE_ALPHA:
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (type) {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
	break;
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
	switch (format) {
	case GL_RGB:
	    break;
	default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
	switch (format) {
	case GL_RGBA:
	case GL_BGRA:
	case GL_ABGR_EXT:
	    break;
	default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if (!hg->array) {
	return;
    }

    __glInitMemGet(gc, &spanInfo, hg->width, 1, hg->baseFormat, hg->array);
    __glInitMemPack(gc, &spanInfo, hg->width, 1, 0, format, type, values);
    spanInfo.srcType = GL_UNSIGNED_INT;

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    spanInfo.nonColorComp = GL_TRUE;

    (*gc->procs.copyImage)(gc, &spanInfo);

    if (reset) {
	ResetHistogramArray(gc, hg);
    }
}

static void GetHistogramParameters(GLenum target, GLenum pname,
				void *params, GLenum return_type)
{
    __GLhistogram *hg;
    GLboolean isProxy;
    GLint value;
    __GL_SETUP_NOT_IN_BEGIN();

    hg = LookupHistogram(gc, target, &isProxy);
    if (!hg) {
        return;
    }

    switch (pname) {
	case GL_HISTOGRAM_WIDTH:
	    value = (GLint)hg->width;
	    break;
	case GL_HISTOGRAM_FORMAT:
	    value = (GLint)hg->formatReturn;
	    break;
	case GL_HISTOGRAM_RED_SIZE:
	    value = (GLint)hg->redSize;
	    break;
	case GL_HISTOGRAM_GREEN_SIZE:
	    value = (GLint)hg->greenSize;
	    break;
	case GL_HISTOGRAM_BLUE_SIZE:
	    value = (GLint)hg->blueSize;
	    break;
	case GL_HISTOGRAM_ALPHA_SIZE:
	    value = (GLint)hg->alphaSize;
	    break;
	case GL_HISTOGRAM_LUMINANCE_SIZE:
	    value = (GLint)hg->luminanceSize;
	    break;
	case GL_HISTOGRAM_SINK:
	    value = (GLint)hg->sink;
	    break;
	default:
	    __glSetError(GL_INVALID_ENUM);
	    break;
    }

    switch (return_type) {
	case GL_INT:
	    *(GLint*)params = value;
	    break;
	case GL_FLOAT:
	    *(GLfloat*)params = (GLfloat)value;
	    break;
	default: 
	    /* did __glim_GetHistogramParameter{if}v pass wrong return type? */
	    assert(0);
    }
}

void GLAPI __glim_GetHistogramParameteriv(GLenum target, GLenum pname,
					  GLint *params)
{
    GetHistogramParameters(target, pname, params, GL_INT);
}

void GLAPI __glim_GetHistogramParameterfv(GLenum target, GLenum pname,
					  GLfloat *params)
{
    GetHistogramParameters(target, pname, params, GL_FLOAT);
}

void __glInitHistogramState(__GLcontext *gc)
{
    gc->pixel.histogram.format =
    gc->pixel.histogram.formatReturn =
    gc->pixel.histogram.baseFormat = GL_RGBA;

    gc->pixel.proxyHistogram.format =
    gc->pixel.proxyHistogram.formatReturn =
    gc->pixel.proxyHistogram.baseFormat = GL_RGBA;
}

void __glFreeHistogramState(__GLcontext *gc)
{
    if (gc->pixel.histogram.array) {
        (*gc->imports.free)(gc, gc->pixel.histogram.array);
    }
}

/************************************************************/

static __GLminmax *LookupMinmax(__GLcontext *gc,
			GLenum target)
{
    __GLminmax *mm;

    switch (target) {
    case GL_MINMAX:
	mm = &gc->pixel.minmax;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return NULL;
    }
    return mm;
}

static GLenum CheckMinmaxArgs(__GLcontext *gc,
		GLenum target, GLenum internalFormat, GLboolean sink)
{
    switch (target) {
    case GL_MINMAX:
	break;
    default:
	return GL_INVALID_ENUM;
    }

    switch (internalFormat) {
    case GL_ALPHA:
    case GL_ALPHA4:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
	break;
    case GL_LUMINANCE:
    case GL_LUMINANCE4:
    case GL_LUMINANCE8:
    case GL_LUMINANCE12:
    case GL_LUMINANCE16:
	break;
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE4_ALPHA4:
    case GL_LUMINANCE6_ALPHA2:
    case GL_LUMINANCE8_ALPHA8:
    case GL_LUMINANCE12_ALPHA4:
    case GL_LUMINANCE12_ALPHA12:
    case GL_LUMINANCE16_ALPHA16:
	break;
    case GL_RGB:
    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
	break;
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGBA8:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGB5_A1:
    case GL_RGB10_A2:
	break;
    default:
	return GL_INVALID_ENUM;
    }

    return 0;
}

static void InitMinmaxArray(__GLcontext *gc, __GLminmax *mm,
		GLenum internalFormat, GLboolean sink)
{
    switch (internalFormat) {
    case GL_ALPHA:
    case GL_ALPHA4:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
        mm->format = 
        mm->baseFormat = GL_ALPHA;
        mm->type = GL_FLOAT;
	break;
    case GL_LUMINANCE:
    case GL_LUMINANCE4:
    case GL_LUMINANCE8:
    case GL_LUMINANCE12:
    case GL_LUMINANCE16:
        mm->format = 
        mm->baseFormat = GL_LUMINANCE;
        mm->type = GL_FLOAT;
	break;
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE4_ALPHA4:
    case GL_LUMINANCE6_ALPHA2:
    case GL_LUMINANCE8_ALPHA8:
    case GL_LUMINANCE12_ALPHA4:
    case GL_LUMINANCE12_ALPHA12:
    case GL_LUMINANCE16_ALPHA16:
        mm->format = 
        mm->baseFormat = GL_LUMINANCE_ALPHA;
        mm->type = GL_FLOAT;
	break;
    case GL_RGB:
    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
        mm->format = 
        mm->baseFormat = GL_RGB;
        mm->type = GL_FLOAT;
	break;
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGBA8:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGB5_A1:
    case GL_RGB10_A2:
        mm->format = 
        mm->baseFormat = GL_RGBA;
        mm->type = GL_FLOAT;
	break;
    default:
	assert(0);
        return;
    }

    mm->formatReturn = internalFormat;
    mm->sink = sink;
}

static void ResetMinmaxArray(__GLcontext *gc, __GLminmax *mm)
{
    GLint lastMinValue, lastMaxValue;
    GLint i;

    switch (mm->baseFormat) {
    case GL_ALPHA:
        lastMinValue = 0;
        lastMaxValue = 1;
	break;
    case GL_LUMINANCE:
        lastMinValue = 0;
        lastMaxValue = 1;
	break;
    case GL_LUMINANCE_ALPHA:
        lastMinValue = 1;
        lastMaxValue = 2;
	break;
    case GL_RGB:
        lastMinValue = 2;
        lastMaxValue = 5;
	break;
    case GL_RGBA:
        lastMinValue = 3;
        lastMaxValue = 7;
	break;
    default:
        assert(0);
	return;
    }
    assert(mm->type == GL_FLOAT);

    /* set minimums to largest possible value */
    for (i=0; i<=lastMinValue; ++i) {
        mm->array[i] = __GL_MAX_FLOAT;
    }

    /* set maximums to smallest possible value */
    for (i=lastMinValue+1; i<=lastMaxValue; ++i) {
        mm->array[i] = -__GL_MAX_FLOAT;
    }
}

/*
** Minimum and maximum values are stored in an 8-element array in whatever
** the current format is. This makes it easier to handle getminmax().
*/
void GLAPI __glim_Minmax(GLenum target, GLenum internalFormat, GLboolean sink)
{
    __GLminmax *mm;
    GLenum rv;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if ((rv = CheckMinmaxArgs(gc, target, internalFormat, sink))) {
        __glSetError(rv);
	return;
    }
    mm = LookupMinmax(gc, target);
    if (!mm) {
        return;
    }

    InitMinmaxArray(gc, mm, internalFormat, sink);
    ResetMinmaxArray(gc, mm);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_ResetMinmax(GLenum target)
{
    __GLminmax *mm;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    mm = LookupMinmax(gc, target);
    if (!mm) {
	return;
    }

    ResetMinmaxArray(gc, mm);
}

void GLAPI __glim_GetMinmax(GLenum target, GLboolean reset, GLenum format,
			    GLenum type, GLvoid *values)
{
    __GLminmax *mm;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_GET();

    mm = LookupMinmax(gc, target);
    if (!mm) {
        return;
    }

    switch (format) {
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_RGB:
    case GL_BGR:
    case GL_RGBA:
    case GL_BGRA:
    case GL_ABGR_EXT:
    case GL_LUMINANCE:
    case GL_LUMINANCE_ALPHA:
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (type) {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
	break;
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
	switch (format) {
	case GL_RGB:
	    break;
	default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
	switch (format) {
	case GL_RGBA:
	case GL_BGRA: 
	case GL_ABGR_EXT:
	    break;
	default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __glInitMemGet(gc, &spanInfo, 2, 1, mm->baseFormat, mm->array);
    __glInitMemPack(gc, &spanInfo, 2, 1, 0, format, type, values);
    spanInfo.srcType = GL_FLOAT;

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;

    (*gc->procs.copyImage)(gc, &spanInfo);

    if (reset) {
        ResetMinmaxArray(gc, mm);
    }
}

static void GetMinmaxParameters(GLenum target, GLenum pname, void *values,
			     GLenum return_type)
{
    __GLminmax *mm;
    GLint return_value;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    mm = LookupMinmax(gc, target);
    if (!mm) {
        return;
    }

    switch (pname) {
    case GL_MINMAX_FORMAT:
	return_value = (GLint)mm->formatReturn;
	break;
    case GL_MINMAX_SINK:
	return_value = (GLint)mm->sink;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (return_type) {
    case GL_INT:
	*(GLint *)values = return_value;
	break;
    case GL_FLOAT:
	*(GLfloat *)values = (GLfloat)return_value;
	break;
    default:
	/* did getminmaxParameter{i,f}vEXT pass good return_type? */
	assert(0);
	break;
    }
}


void GLAPI __glim_GetMinmaxParameteriv(GLenum target, GLenum pname, 
				       GLint *values)
{
    GetMinmaxParameters(target, pname, values, GL_INT);
}

void GLAPI __glim_GetMinmaxParameterfv(GLenum target, GLenum pname, 
				       GLfloat *values)
{
    GetMinmaxParameters(target, pname, values, GL_FLOAT);
}

void __glInitMinmaxState(__GLcontext *gc)
{
    gc->pixel.minmax.format =
    gc->pixel.minmax.formatReturn =
    gc->pixel.minmax.baseFormat = GL_RGBA;
    gc->pixel.minmax.type = GL_FLOAT;
    ResetMinmaxArray(gc, &gc->pixel.minmax);
}

void __glFreeMinmaxState(__GLcontext *gc)
{
    /* nothing to do here */
}

