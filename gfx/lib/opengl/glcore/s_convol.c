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

static GLenum __glBaseInternalFormat(GLenum internalFormat)
{
    switch (internalFormat) {
    case GL_ALPHA:
    case GL_ALPHA4:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
	return GL_ALPHA;
    case GL_LUMINANCE:
    case GL_LUMINANCE4:
    case GL_LUMINANCE8:
    case GL_LUMINANCE12:
    case GL_LUMINANCE16:
	return GL_LUMINANCE;
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE4_ALPHA4:
    case GL_LUMINANCE6_ALPHA2:
    case GL_LUMINANCE8_ALPHA8:
    case GL_LUMINANCE12_ALPHA4:
    case GL_LUMINANCE12_ALPHA12:
    case GL_LUMINANCE16_ALPHA16:
	return GL_LUMINANCE_ALPHA;
    case GL_INTENSITY:
    case GL_INTENSITY4:
    case GL_INTENSITY8:
    case GL_INTENSITY12:
    case GL_INTENSITY16:
	return GL_INTENSITY;
    case GL_RGB:
    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
	return GL_RGB;
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGBA8:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGB5_A1:
    case GL_RGB10_A2:
	return GL_RGBA;
    default:
	assert(0);
        return 0;
    }
}

GLboolean __glCheckConvolutionFilterArgs(__GLcontext *gc, GLenum target,
					 GLsizei width, GLsizei height,
					 GLenum internalFormat,
					 GLenum format, GLenum type)
{
    switch(target) {
      case GL_CONVOLUTION_1D:
	if(width > gc->constants.maxConvolution1DWidth || width < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return GL_FALSE;
	}
	break;
      case GL_CONVOLUTION_2D:
	if(width > gc->constants.maxConvolution2DWidth || width < 0 ||
	   height > gc->constants.maxConvolution2DHeight || height < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return GL_FALSE;
	}
	break;
      case GL_SEPARABLE_2D:
	if(width > gc->constants.maxSeparable2DWidth || width < 0 ||
	   height > gc->constants.maxSeparable2DHeight || height < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return GL_FALSE;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
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
    case GL_INTENSITY:
    case GL_INTENSITY4:
    case GL_INTENSITY8:
    case GL_INTENSITY12:
    case GL_INTENSITY16:
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
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
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
	return GL_FALSE;
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
	    return GL_FALSE;
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
	    return GL_FALSE;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
    }

    return GL_TRUE;
}

void GLAPI __glim_ConvolutionFilter1D(GLenum target, GLenum internalFormat,
				      GLsizei width, GLenum format,
				      GLenum type, const void* image)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    if (!__glCheckConvolutionFilterArgs(gc, target, width, 1,
					internalFormat, format, type)) {
	return;
    }

    if(target != GL_CONVOLUTION_1D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_1D_INDEX];
    cf->width = width;
    cf->height = 1;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    __glInitMemUnpack(gc, &spanInfo, width, 1, 
		      0, format, type, image, GL_FALSE);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __gllei_ConvolutionFilter1D(__GLcontext *gc,
			GLenum target, GLenum internalFormat,
			GLsizei width, GLenum format,
			GLenum type, const void* image)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    __GL_API_STATE();

    if (!__glCheckConvolutionFilterArgs(gc, target, width, 1,
					internalFormat, format, type)) {
	return;
    }

    if(target != GL_CONVOLUTION_1D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_1D_INDEX];
    cf->width = width;
    cf->height = 1;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    __glInitMemUnpack(gc, &spanInfo, width, 1, 
		      0, format, type, image, GL_TRUE);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_ConvolutionFilter2D(GLenum target, GLenum internalFormat,
				      GLsizei width, GLsizei height,
				      GLenum format, GLenum type,
				      const void* image)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    if (!__glCheckConvolutionFilterArgs(gc, target, width, height,
					internalFormat, format, type)) {
	return;
    }

    if(target != GL_CONVOLUTION_2D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_2D_INDEX];
    cf->width = width;
    cf->height = height;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    __glInitMemUnpack(gc, &spanInfo, width, height, 
		      0, format, type, image, GL_FALSE);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __gllei_ConvolutionFilter2D(__GLcontext *gc,
			GLenum target, GLenum internalFormat,
			GLsizei width, GLsizei height,
			GLenum format, GLenum type,
			const void* image)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    __GL_API_STATE();

    if (!__glCheckConvolutionFilterArgs(gc, target, width, height,
					internalFormat, format, type)) {
	return;
    }

    if(target != GL_CONVOLUTION_2D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_2D_INDEX];
    cf->width = width;
    cf->height = height;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    __glInitMemUnpack(gc, &spanInfo, width, height, 
		      0, format, type, image, GL_TRUE);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_SeparableFilter2D(GLenum target, GLenum internalFormat,
				    GLsizei width, GLsizei height,
				    GLenum format, GLenum type,
				    const void* row, const void* column)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    if (!__glCheckConvolutionFilterArgs(gc, target, width, height,
					internalFormat, format, type)) {
	return;
    }

    if(target != GL_SEPARABLE_2D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_SEPARABLE_2D_INDEX];
    cf->width = width;
    cf->height = height;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    /* treat filter as two spans */

    /* row span */

    __glInitMemUnpack(gc, &spanInfo, width, 1, 0,
    				format, type, row, GL_FALSE);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    /* column span */

    __glInitMemUnpack(gc, &spanInfo, height, 1, 0,
    				format, type, column, GL_FALSE);
    __glInitMemLoad(gc, &spanInfo, cf->format, 
		    cf->filter +
		    width * __glElementsPerGroup(cf->format, cf->type));

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __gllei_SeparableFilter2D(__GLcontext *gc,
			GLenum target, GLenum internalFormat,
			GLsizei width, GLsizei height,
			GLenum format, GLenum type,
			const void* row, const void* column)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    __GL_API_STATE();

    if (!__glCheckConvolutionFilterArgs(gc, target, width, height,
					internalFormat, format, type)) {
	return;
    }

    if(target != GL_SEPARABLE_2D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_SEPARABLE_2D_INDEX];
    cf->width = width;
    cf->height = height;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    /* treat filter as two spans */

    /* row span */

    __glInitMemUnpack(gc, &spanInfo, width, 1, 0,
    				format, type, row, GL_TRUE);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    /* column span */

    __glInitMemUnpack(gc, &spanInfo, height, 1, 0,
    				format, type, column, GL_TRUE);
    __glInitMemLoad(gc, &spanInfo, cf->format, 
		    cf->filter +
		    width * __glElementsPerGroup(cf->format, cf->type));

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_CopyConvolutionFilter1D(GLenum target, GLenum internalFormat,
					  GLint x, GLint y, GLsizei width)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* re-use function: inappropriate args are filled with legal values */
    if (!__glCheckConvolutionFilterArgs(gc, target, width, 0,
					internalFormat, GL_RGBA, GL_FLOAT)) {
	return;
    }

    if(target != GL_CONVOLUTION_1D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_1D_INDEX];
    cf->width = width;
    cf->height = 1;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    __glInitReadImageSrcInfo(gc, &spanInfo, x, y, width, 1);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    if (!__glClipReadPixels(gc, &spanInfo)) return;
    (*gc->procs.readImage)(gc, NULL, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_CopyConvolutionFilter2D(GLenum target, GLenum internalFormat,
					  GLint x, GLint y, GLsizei width,
					  GLsizei height)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* re-use function: inappropriate args are filled with legal values */
    if (!__glCheckConvolutionFilterArgs(gc, target, width, height,
					internalFormat, GL_RGBA, GL_FLOAT)) {
	return;
    }

    if(target != GL_CONVOLUTION_2D) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_2D_INDEX];
    cf->width = width;
    cf->height = height;
    cf->format = cf->baseFormat = __glBaseInternalFormat(internalFormat);
    cf->formatReturn = internalFormat;
    cf->type = GL_FLOAT;

    __glInitReadImageSrcInfo(gc, &spanInfo, x, y, width, height);
    __glInitMemLoad(gc, &spanInfo, cf->format, cf->filter);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &cf->state.scale, &cf->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = cf->state.scale;
	spanInfo.bias = cf->state.bias;
    }

    if (!__glClipReadPixels(gc, &spanInfo)) return;
    (*gc->procs.readImage)(gc, NULL, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_GetConvolutionFilter(GLenum target, GLenum format,
				       GLenum type, void* image)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* re-use function: inappropriate args are filled with legal values */
    if (!__glCheckConvolutionFilterArgs(gc, target, 0, 0,
					GL_RGBA, format, type)) {
	return;
    }

    switch(target) {
      case GL_CONVOLUTION_1D:
	cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_1D_INDEX];
	break;
      case GL_CONVOLUTION_2D:
	cf = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_2D_INDEX];
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
        return;
    }

    __glInitMemGet(gc, &spanInfo, cf->width, cf->height,
    						cf->format, cf->filter);
    __glInitMemPack(gc, &spanInfo, cf->width, cf->height,
    						0, format, type, image);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;

    (*gc->procs.copyImage)(gc, &spanInfo);
}

void GLAPI __glim_GetSeparableFilter(GLenum target, GLenum format, GLenum type,
				     void* row, void* column, void* span)
{
    __GLconvolutionFilter *cf;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* re-use function with some dummy args that will never fail */
    if (!__glCheckConvolutionFilterArgs(gc, target, 0, 0,
					GL_RGBA, format, type)) {
	return;
    }

    if (target != GL_SEPARABLE_2D) { /* only valid target for this fun */
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    cf = &gc->pixel.convolutionFilter[__GL_SEPARABLE_2D_INDEX];

    __glInitMemGet(gc, &spanInfo, cf->width, 1, cf->format, cf->filter);
    __glInitMemPack(gc, &spanInfo, cf->width, 1, 0, format, type, row);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;

    (*gc->procs.copyImage)(gc, &spanInfo);

    __glInitMemGet(gc, &spanInfo, cf->height, 1, cf->format,
    	cf->filter + cf->width * __glElementsPerGroup(cf->format, cf->type));
    __glInitMemPack(gc, &spanInfo, cf->height, 1, 0, format, type, column);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_FALSE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;

    (*gc->procs.copyImage)(gc, &spanInfo);
}

/*
** Helper function to reduce code duplication. This function is used
** by: __glim_ConvlutionParameteri, __glim_ConvolutionParameteriv,
** __glim_ConvolutionParameterf, __glim_ConvolutionParameterfv.
**
** If any of these functions diverge, then the helper function will
** have to be modified or eliminated.
*/
/*
** Maybe this should be changed to:
** GLenum target, GLenum pname, GLenum scalar_param, float *vector_param)
*/

static void ConvolutionParameter(GLenum target,
		GLenum pname, const void* param, GLenum param_type)
{
    __GLconvolutionFilter *filter;
    GLenum pval;
    __GL_SETUP_NOT_IN_BEGIN();

    switch(target) {
      case GL_CONVOLUTION_1D:
	filter = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_1D_INDEX];
	break;
      case GL_CONVOLUTION_2D:
	filter = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_2D_INDEX];
	break;
      case GL_SEPARABLE_2D:
	filter = &gc->pixel.convolutionFilter[__GL_SEPARABLE_2D_INDEX];
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch(pname) {
      case GL_CONVOLUTION_BORDER_MODE:
	switch(param_type) {
	  case GL_INT:
	    pval = *(const GLint *)param;
	    break;
	  case GL_FLOAT:
	    pval = *(const GLfloat *)param;
	    break;
	  default:
	    assert(0); /* param_type uninitialized, or not float or int? */
	}
	switch(pval) {
	  case GL_REDUCE:
	  case GL_CONSTANT_BORDER:
	  case GL_REPLICATE_BORDER:
	    filter->state.borderMode = pval;
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_CONVOLUTION_BORDER_COLOR:
	if(param_type == GL_INT) {
	    /*
	    ** Integer color values are mapped [-1,1].
	    */
	    filter->state.borderColor.r =
	    		__GL_I_TO_FLOAT(((const GLint *)param)[0]) *
					gc->constants.redScale;
	    filter->state.borderColor.g =
	    		__GL_I_TO_FLOAT(((const GLint *)param)[1]);
					gc->constants.greenScale;
	    filter->state.borderColor.b =
	    		__GL_I_TO_FLOAT(((const GLint *)param)[2]);
					gc->constants.blueScale;
	    filter->state.borderColor.a =
	    		__GL_I_TO_FLOAT(((const GLint *)param)[3]);
					gc->constants.alphaScale;
	} else {
	    filter->state.borderColor.r = ((const GLfloat *)param)[0] *
	    				gc->constants.redScale;;
	    filter->state.borderColor.g = ((const GLfloat *)param)[1] *
	    				gc->constants.greenScale;;
	    filter->state.borderColor.b = ((const GLfloat *)param)[2] *
	    				gc->constants.blueScale;;
	    filter->state.borderColor.a = ((const GLfloat *)param)[3] *
	    				gc->constants.alphaScale;;
	}
	break;
      case GL_CONVOLUTION_FILTER_SCALE:
	if(param_type == GL_INT) {
	    filter->state.scale.r = ((const GLint *)param)[0];
	    filter->state.scale.g = ((const GLint *)param)[1];
	    filter->state.scale.b = ((const GLint *)param)[2];
	    filter->state.scale.a = ((const GLint *)param)[3];
	} else {
	    filter->state.scale.r = ((const GLfloat *)param)[0];
	    filter->state.scale.g = ((const GLfloat *)param)[1];
	    filter->state.scale.b = ((const GLfloat *)param)[2];
	    filter->state.scale.a = ((const GLfloat *)param)[3];
	}
	break;
      case GL_CONVOLUTION_FILTER_BIAS:
	if(param_type == GL_INT) {
	    filter->state.bias.r = ((const GLint *)param)[0];
	    filter->state.bias.g = ((const GLint *)param)[1];
	    filter->state.bias.b = ((const GLint *)param)[2];
	    filter->state.bias.a = ((const GLint *)param)[3];
	} else {
	    filter->state.bias.r = ((const GLfloat *)param)[0];
	    filter->state.bias.g = ((const GLfloat *)param)[1];
	    filter->state.bias.b = ((const GLfloat *)param)[2];
	    filter->state.bias.a = ((const GLfloat *)param)[3];
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_ConvolutionParameteri(GLenum target, GLenum pname,
					GLint param)
{
    switch(pname) {
      case GL_CONVOLUTION_BORDER_MODE:
	ConvolutionParameter(target, pname, (void*)&param, GL_INT);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    /* Don't need to validate with current architecture */
}

void GLAPI __glim_ConvolutionParameteriv(GLenum target, GLenum pname,
					 const GLint* params)
{
    switch(pname) {
      case GL_CONVOLUTION_BORDER_MODE:
      case GL_CONVOLUTION_BORDER_COLOR:
      case GL_CONVOLUTION_FILTER_BIAS:
      case GL_CONVOLUTION_FILTER_SCALE:
	ConvolutionParameter(target, pname, (const void*)params, GL_INT);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    /* Don't need to validate with current architecture */
}

void GLAPI __glim_ConvolutionParameterf(GLenum target, GLenum pname,
					GLfloat param)
{
    switch(pname) {
      case GL_CONVOLUTION_BORDER_MODE:
	ConvolutionParameter(target, pname, (void*)&param, GL_FLOAT);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    /* Don't need to validate with current architecture */
}

void GLAPI __glim_ConvolutionParameterfv(GLenum target, GLenum pname,
					 const GLfloat* params)
{
    switch(pname) {
      case GL_CONVOLUTION_BORDER_MODE:
      case GL_CONVOLUTION_BORDER_COLOR:
      case GL_CONVOLUTION_FILTER_BIAS:
      case GL_CONVOLUTION_FILTER_SCALE:
	ConvolutionParameter(target, pname, (const void*)params, GL_FLOAT);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    /* Don't need to validate with current architecture */
}

/*
** Helper function to reduce code duplication for
** GetConvolutionParameter{f,i}v code. You will never see the values
** of the dummy filter, since the proper filter is choosen using the
** target parameter.
*/
static void GetConvolutionParameter(GLenum target, GLenum pname, void *params,
				 GLenum return_type)
{
    __GLconvolutionFilter *filter;
    GLint val;
    __GL_SETUP_NOT_IN_BEGIN();

    switch(target) {
      case GL_CONVOLUTION_1D:
	if(pname == GL_CONVOLUTION_HEIGHT ||
	   pname == GL_MAX_CONVOLUTION_HEIGHT) {
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	filter = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_1D_INDEX];
	break;
      case GL_CONVOLUTION_2D:
	filter = &gc->pixel.convolutionFilter[__GL_CONVOLUTION_2D_INDEX];
	break;
      case GL_SEPARABLE_2D:
	filter = &gc->pixel.convolutionFilter[__GL_SEPARABLE_2D_INDEX];
	break;
      default: /* bad argument: set error, don't change anything */
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch(pname) {
      case GL_CONVOLUTION_FORMAT:
	if(return_type == GL_INT)
	    *(GLint *)params = filter->format;
	else
	    *(GLfloat *)params = (GLfloat)filter->format;
	break;
      case GL_CONVOLUTION_WIDTH:
	if(return_type == GL_INT)
	    *(GLint *)params = filter->width;
	else
	    *(GLfloat *)params = (GLfloat)filter->width;
	break;
      case GL_CONVOLUTION_HEIGHT:
	if(return_type == GL_INT)
	    *(GLint *)params = filter->height;
	else
	    *(GLfloat *)params = (GLfloat)filter->height;
	break;
      case GL_MAX_CONVOLUTION_WIDTH:
        switch (target) {
	  case GL_CONVOLUTION_1D:
	    val = gc->constants.maxConvolution1DWidth;
	    break;
	  case GL_CONVOLUTION_2D:
	    val = gc->constants.maxConvolution2DWidth;
	    break;
	  case GL_SEPARABLE_2D:
	    val = gc->constants.maxSeparable2DWidth;
	    break;
	}
	if(return_type == GL_INT)
	    *(GLint *)params = val;
	else
	    *(GLfloat *)params = (GLfloat)val;
	break;
      case GL_MAX_CONVOLUTION_HEIGHT:
        switch (target) {
	  case GL_CONVOLUTION_1D:
	    val = 0;
	    break;
	  case GL_CONVOLUTION_2D:
	    val = gc->constants.maxConvolution2DHeight;
	    break;
	  case GL_SEPARABLE_2D:
	    val = gc->constants.maxSeparable2DHeight;
	    break;
	}
	if(return_type == GL_INT)
	    *(GLint *)params = val;
	else
	    *(GLfloat *)params = (GLfloat)val;
	break;
      case GL_CONVOLUTION_BORDER_MODE:
	if(return_type == GL_INT)
	    *(GLint *)params = filter->state.borderMode;
	else
	    *(GLfloat *)params = (GLfloat)filter->state.borderMode;
	break;
      case GL_CONVOLUTION_BORDER_COLOR:
	if(return_type == GL_INT) {
	    ((GLint *)params)[0] = (GLint)filter->state.borderColor.r *
	    				gc->constants.oneOverRedScale;
	    ((GLint *)params)[1] = (GLint)filter->state.borderColor.g *
	    				gc->constants.oneOverGreenScale;
	    ((GLint *)params)[2] = (GLint)filter->state.borderColor.b *
	    				gc->constants.oneOverBlueScale;
	    ((GLint *)params)[3] = (GLint)filter->state.borderColor.a *
	    				gc->constants.oneOverAlphaScale;
	} else {
	    ((GLfloat *)params)[0] = filter->state.borderColor.r *
	    				gc->constants.oneOverRedScale;
	    ((GLfloat *)params)[1] = filter->state.borderColor.g *
	    				gc->constants.oneOverGreenScale;
	    ((GLfloat *)params)[2] = filter->state.borderColor.b *
	    				gc->constants.oneOverBlueScale;
	    ((GLfloat *)params)[3] = filter->state.borderColor.a *
	    				gc->constants.oneOverAlphaScale;
	}
	break;
      case GL_CONVOLUTION_FILTER_SCALE:
	if(return_type == GL_INT) {
	    ((GLint *)params)[0] = (GLint)filter->state.scale.r;
	    ((GLint *)params)[1] = (GLint)filter->state.scale.g;
	    ((GLint *)params)[2] = (GLint)filter->state.scale.b;
	    ((GLint *)params)[3] = (GLint)filter->state.scale.a;
	} else {
	    ((GLfloat *)params)[0] = filter->state.scale.r;
	    ((GLfloat *)params)[1] = filter->state.scale.g;
	    ((GLfloat *)params)[2] = filter->state.scale.b;
	    ((GLfloat *)params)[3] = filter->state.scale.a;
	}
	break;
      case GL_CONVOLUTION_FILTER_BIAS:
	if(return_type == GL_INT) {
	    ((GLint *)params)[0] = (GLint)filter->state.bias.r;
	    ((GLint *)params)[1] = (GLint)filter->state.bias.g;
	    ((GLint *)params)[2] = (GLint)filter->state.bias.b;
	    ((GLint *)params)[3] = (GLint)filter->state.bias.a;
	} else {
	    ((GLfloat *)params)[0] = filter->state.bias.r;
	    ((GLfloat *)params)[1] = filter->state.bias.g;
	    ((GLfloat *)params)[2] = filter->state.bias.b;
	    ((GLfloat *)params)[3] = filter->state.bias.a;
	}
	break;
      default: /* bad argument: set error, don't change anything */
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetConvolutionParameteriv(GLenum target, GLenum pname,
					    GLint* params)
{
    GetConvolutionParameter(target, pname, params, GL_INT);
}

void GLAPI __glim_GetConvolutionParameterfv(GLenum target, GLenum pname,
					    GLfloat* params)
{
    GetConvolutionParameter(target, pname, params, GL_FLOAT);
}

GLint __glConvolutionParameterfv_size(GLenum pname)
{
    switch(pname) {
      case GL_CONVOLUTION_BORDER_MODE:
	return 1;
      case GL_CONVOLUTION_BORDER_COLOR:
      case GL_CONVOLUTION_FILTER_SCALE:
      case GL_CONVOLUTION_FILTER_BIAS:
	return 4;
      default: /* error: bad enum value */
	return -1;
    }
}

GLint __glConvolutionParameteriv_size(GLenum pname)
{
    switch(pname) {
      case GL_CONVOLUTION_BORDER_MODE:
	return 1;
      case GL_CONVOLUTION_BORDER_COLOR:
      case GL_CONVOLUTION_FILTER_SCALE:
      case GL_CONVOLUTION_FILTER_BIAS:
	return 4;
      default: /* error: bad enum value */
	return -1;
    }
}

void __glEarlyInitConvolutionState(__GLcontext *gc)
{
    GLint i;

    for (i=0; i<__GL_NUM_CONVOLUTION_TARGETS; ++i) {
        __GLconvolutionFilter *cf = &gc->pixel.convolutionFilter[i];

	gc->state.pixel.convolutionFilter[i] = &cf->state;
    }
}

void __glInitConvolutionState(__GLcontext *gc)
{
    GLint i;

    for (i=0; i<__GL_NUM_CONVOLUTION_TARGETS; ++i) {
        __GLconvolutionFilter *cf = &gc->pixel.convolutionFilter[i];

	switch (i) {
	case __GL_CONVOLUTION_1D_INDEX:
	    cf->target = GL_CONVOLUTION_1D;
	    cf->filter = (*gc->imports.calloc)(gc,
	    		4*gc->constants.maxConvolution1DWidth,
			sizeof(__GLfloat));
	    break;
	case __GL_CONVOLUTION_2D_INDEX:
	    cf->target = GL_CONVOLUTION_2D;
	    cf->filter = (*gc->imports.calloc)(gc,
	    		4*gc->constants.maxConvolution2DWidth*
			  gc->constants.maxConvolution2DHeight,
			sizeof(__GLfloat));
	    break;
	case __GL_SEPARABLE_2D_INDEX:
	    cf->target = GL_SEPARABLE_2D;
	    cf->filter = (*gc->imports.calloc)(gc,
	    		4*(gc->constants.maxSeparable2DWidth+
	    		   gc->constants.maxSeparable2DHeight),
			sizeof(__GLfloat));
	    break;
	default:
	    assert(0);
	    break;
	}

	cf->format =
	cf->formatReturn =
	cf->baseFormat = GL_RGBA;
	cf->type = GL_FLOAT;
	cf->state.borderMode = GL_REDUCE;
	cf->state.scale.r = __glOne;
	cf->state.scale.g = __glOne;
	cf->state.scale.b = __glOne;
	cf->state.scale.a = __glOne;
    }
}

void __glFreeConvolutionState(__GLcontext *gc)
{
    GLint i;

    for (i=0; i<__GL_NUM_CONVOLUTION_TARGETS; ++i) {
        __GLconvolutionFilter *cf = &gc->pixel.convolutionFilter[i];

	if (cf->filter) {
	    (*gc->imports.free)(gc, cf->filter);
	}
    }
}
