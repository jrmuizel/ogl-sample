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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_ctable.c#19 $
*/
#include "global.h"
#include "types.h"
#include "g_imports.h"
#include "ctable.h"
#include <string.h>
#include "g_imfncs.h"

static __GLcolorTable *LookupColorTable(__GLcontext *gc, GLenum target,
				  GLboolean *isProxy)
{
    __GLcolorTable *ct;
    __GLtexture *tex;

    switch (target) {
    case GL_PROXY_TEXTURE_1D:
    case GL_PROXY_TEXTURE_2D:
	*isProxy = GL_TRUE;
	tex = __glLookupActiveTexture(gc, target);
	ct = &tex->CT;
	break;
    case GL_TEXTURE_1D:
    case GL_TEXTURE_2D:
	*isProxy = GL_FALSE;
	tex = __glLookupActiveTexture(gc, target);
	ct = &tex->CT;
	break;
    case GL_COLOR_TABLE:
	*isProxy = GL_FALSE;
	ct = &gc->pixel.colorTable[__GL_COLOR_TABLE_INDEX];
	break;
    case GL_PROXY_COLOR_TABLE:
	*isProxy = GL_TRUE;
	ct = &gc->pixel.proxyColorTable[__GL_COLOR_TABLE_INDEX];
	break;
    case GL_POST_CONVOLUTION_COLOR_TABLE:
	*isProxy = GL_FALSE;
	ct = &gc->pixel.colorTable[__GL_POST_CONVOLUTION_COLOR_TABLE_INDEX];
	break;
    case GL_PROXY_POST_CONVOLUTION_COLOR_TABLE:
	*isProxy = GL_TRUE;
	ct = &gc->pixel.proxyColorTable[__GL_POST_CONVOLUTION_COLOR_TABLE_INDEX];
	break;
    case GL_POST_COLOR_MATRIX_COLOR_TABLE:
	*isProxy = GL_FALSE;
	ct = &gc->pixel.colorTable[__GL_POST_COLOR_MATRIX_COLOR_TABLE_INDEX];
	break;
    case GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE:
	*isProxy = GL_TRUE;
	ct = &gc->pixel.proxyColorTable[__GL_POST_COLOR_MATRIX_COLOR_TABLE_INDEX];
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return NULL;
    }
    return ct;
}

GLenum __glCheckColorTableArgs(__GLcontext *gc, GLenum target,
				GLenum internalFormat, GLsizei width,
				GLenum format, GLenum type)
{
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_TEXTURE_2D:
    case GL_PROXY_TEXTURE_1D:
    case GL_PROXY_TEXTURE_2D:
    case GL_COLOR_TABLE:
    case GL_PROXY_COLOR_TABLE:
    case GL_POST_CONVOLUTION_COLOR_TABLE:
    case GL_PROXY_POST_CONVOLUTION_COLOR_TABLE:
    case GL_POST_COLOR_MATRIX_COLOR_TABLE:
    case GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE:
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
	return GL_INVALID_ENUM;
    }

    /* width must be a positive power of two */
    if ((width < 0) || (width & (width - 1))) {
	return GL_INVALID_VALUE;
    }

    switch (format) {
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_RGB:
    case GL_RGBA:
    case GL_ABGR_EXT:
    case GL_BGR:
    case GL_BGRA:
    case GL_LUMINANCE:
    case GL_LUMINANCE_ALPHA:
	break;
    default:
	return GL_INVALID_ENUM;
    }

    switch (type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_INT:
    case GL_UNSIGNED_INT:
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
	    return GL_INVALID_OPERATION;
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
	    return GL_INVALID_OPERATION;
	}
	break;
    default:
	return GL_INVALID_ENUM;
    }

    return 0;
}

GLenum __glCheckColorSubTableArgs(__GLcontext *gc, GLenum target,
				  GLsizei start, GLsizei count,
				  GLenum format, GLenum type)
{
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_TEXTURE_2D:
    case GL_COLOR_TABLE:
    case GL_PROXY_COLOR_TABLE:
    case GL_POST_CONVOLUTION_COLOR_TABLE:
    case GL_PROXY_POST_CONVOLUTION_COLOR_TABLE:
    case GL_POST_COLOR_MATRIX_COLOR_TABLE:
    case GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE:
	break;
    default:
	return GL_INVALID_ENUM;
    }

    if (start < 0 || count < 0) {
	return GL_INVALID_VALUE;
    }

    switch (format) {
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_RGB:
    case GL_RGBA:
    case GL_ABGR_EXT:
    case GL_BGR:
    case GL_BGRA:
    case GL_LUMINANCE:
    case GL_LUMINANCE_ALPHA:
	break;
    default:
	return GL_INVALID_ENUM;
    }

    switch (type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_INT:
    case GL_UNSIGNED_INT:
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
	    return GL_INVALID_OPERATION;
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
	    return GL_INVALID_OPERATION;
	}
	break;
    default:
	return GL_INVALID_ENUM;
    }
    return 0;
}

static GLboolean AllocColorTable(__GLcontext *gc, __GLcolorTable *ct,
		GLboolean isProxy, GLenum internalFormat, GLsizei width)
{
    GLenum baseFormat, type;
    GLint redSize, greenSize, blueSize, alphaSize, luminanceSize, intensitySize;
    GLint bufferSize;

    /*
    ** Map internal format to actual format and type
    */
    switch (internalFormat) {
    case GL_ALPHA:
    case GL_ALPHA4:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
	baseFormat = GL_ALPHA;
	type = GL_FLOAT;
	redSize = 0;
	greenSize = 0;
	blueSize = 0;
	alphaSize = 8;
	luminanceSize = 0;
	intensitySize = 0;
	break;
    case GL_LUMINANCE:
    case GL_LUMINANCE4:
    case GL_LUMINANCE8:
    case GL_LUMINANCE12:
    case GL_LUMINANCE16:
	baseFormat = GL_LUMINANCE;
	type = GL_FLOAT;
	redSize = 0;
	greenSize = 0;
	blueSize = 0;
	alphaSize = 0;
	luminanceSize = 8;
	intensitySize = 0;
	break;
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE4_ALPHA4:
    case GL_LUMINANCE6_ALPHA2:
    case GL_LUMINANCE8_ALPHA8:
    case GL_LUMINANCE12_ALPHA4:
    case GL_LUMINANCE12_ALPHA12:
    case GL_LUMINANCE16_ALPHA16:
	baseFormat = GL_LUMINANCE_ALPHA;
	type = GL_FLOAT;
	redSize = 0;
	greenSize = 0;
	blueSize = 0;
	alphaSize = 8;
	luminanceSize = 8;
	intensitySize = 0;
	break;
    case GL_INTENSITY:
    case GL_INTENSITY4:
    case GL_INTENSITY8:
    case GL_INTENSITY12:
    case GL_INTENSITY16:
	baseFormat = GL_INTENSITY;
	type = GL_FLOAT;
	redSize = 0;
	greenSize = 0;
	blueSize = 0;
	alphaSize = 0;
	luminanceSize = 0;
	intensitySize = 8;
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
	type = GL_FLOAT;
	redSize = 8;
	greenSize = 8;
	blueSize = 8;
	alphaSize = 0;
	luminanceSize = 0;
	intensitySize = 0;
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
	type = GL_FLOAT;
	redSize = 8;
	greenSize = 8;
	blueSize = 8;
	alphaSize = 8;
	luminanceSize = 0;
	intensitySize = 0;
	break;
    default:
	assert(0);
	return GL_FALSE;
    }

    /*
    ** Texture palettes are currently always stored as GL_UNSIGNED_BYTE
    */
    if ((ct->target == GL_TEXTURE_1D) ||
        (ct->target == GL_TEXTURE_2D) ||
        (ct->target == GL_TEXTURE_3D)) {
        type = GL_UNSIGNED_BYTE;
    }

    bufferSize = width *
    		__glElementsPerGroup(baseFormat, type) *
		__glBytesPerElement(type);

    /* Limit color table size so that pixel operations can be used */
    if (bufferSize > __GL_MAX_SPAN_SIZE) {
	ct->width = 0;
	ct->format = 0;
	ct->formatReturn = 0;
	ct->baseFormat = 0;
	ct->type = 0;
	ct->redSize = 0;
	ct->greenSize = 0;
	ct->blueSize = 0;
	ct->alphaSize = 0;
	ct->luminanceSize = 0;
	ct->intensitySize = 0;
	if (!isProxy) {
	    __glSetError(GL_TABLE_TOO_LARGE);
	}
	return GL_FALSE;
    }
    
    /* Allocate color table buffer memory */
    if (!isProxy) {
	ct->table = (*gc->imports.realloc)(gc, ct->table, bufferSize);
	if (!ct->table && bufferSize > 0) {
	    __glSetError(GL_OUT_OF_MEMORY);
	    return GL_FALSE;
	}
    }

    /* Update color table state */
    ct->width = width;

    ct->formatReturn = internalFormat;
    ct->format =
    ct->baseFormat = baseFormat;
    ct->type = type;

    ct->redSize = redSize;
    ct->greenSize = greenSize;
    ct->blueSize = blueSize;
    ct->alphaSize = alphaSize;
    ct->luminanceSize = luminanceSize;
    ct->intensitySize = intensitySize;

    return GL_TRUE;
}

static void __glInitColorTableStore(__GLcolorTable *ct,
				    __GLpixelSpanInfo *spanInfo)
{
    spanInfo->dstImage = ct->table;
    spanInfo->dstSkipImages = 0;
    spanInfo->dstSkipPixels = 0;
    spanInfo->dstSkipLines = 0;
    spanInfo->dstSwapBytes = GL_FALSE;
    spanInfo->dstLsbFirst = GL_TRUE;
    spanInfo->dstLineLength = ct->width;
    spanInfo->dstImageHeight = 1;
    spanInfo->dstFormat = ct->baseFormat;
    spanInfo->dstType = ct->type;
    spanInfo->dstAlignment = __glBytesPerElement(ct->type);
}

void GLAPI __glim_ColorTable(GLenum target,
		GLenum internalFormat, GLsizei width,
		GLenum format, GLenum type, const void *table)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    GLenum rv;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    if ((rv = __glCheckColorTableArgs(gc, target,
    				internalFormat, width, format, type))) {
	__glSetError(rv);
	return;
    }
    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (!AllocColorTable(gc, ct, isProxy, internalFormat, width)) {
        return;
    }

    if (isProxy || width == 0) {
        return;
    }

    __glInitMemUnpack(gc, &spanInfo, width, 1, 0,
			    format, type, table, GL_FALSE);
    __glInitColorTableStore(ct, &spanInfo);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_TRUE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &ct->state.scale, &ct->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = ct->state.scale;
	spanInfo.bias = ct->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL | __GL_DIRTY_GENERIC);
}

void __gllei_ColorTable(__GLcontext *gc, GLenum target,
		GLenum internalFormat, GLsizei width,
		GLenum format, GLenum type, const void *table)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    GLenum rv;
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

    if ((rv = __glCheckColorTableArgs(gc, target,
    				internalFormat, width, format, type))) {
	__glSetError(rv);
	return;
    }
    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (!AllocColorTable(gc, ct, isProxy, internalFormat, width)) {
        return;
    }

    if (width == 0) {
	return;
    }

    assert(!isProxy);

    __glInitMemUnpack(gc, &spanInfo, width, 1, 0,
			    format, type, table, GL_TRUE);
    __glInitColorTableStore(ct, &spanInfo);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_TRUE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &ct->state.scale, &ct->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = ct->state.scale;
	spanInfo.bias = ct->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL | __GL_DIRTY_GENERIC);
}

void GLAPI __glim_CopyColorTable(GLenum target, GLenum internalFormat,
			         GLint x, GLint y, GLsizei width)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    GLenum rv;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP();
    __GL_API_PIXEL_OP();

    if ((rv = __glCheckColorTableArgs(gc, target,
    				internalFormat, width, GL_RGBA, GL_FLOAT))) {
	__glSetError(rv);
	return;
    }
    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (isProxy == GL_TRUE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if (!AllocColorTable(gc, ct, isProxy, internalFormat, width)) {
        return;
    }

    if (width == 0) {
	return;
    }

    __glInitReadImageSrcInfo(gc, &spanInfo, x, y, width, 1);
    __glInitColorTableStore(ct, &spanInfo);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_TRUE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &ct->state.scale, &ct->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = ct->state.scale;
	spanInfo.bias = ct->state.bias;
    }

    if (!__glClipReadPixels(gc, &spanInfo)) {
	return;
    }
    (*gc->procs.readImage)(gc, NULL, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL | __GL_DIRTY_GENERIC);
}

void GLAPI __glim_ColorSubTable(GLenum target,
			GLsizei start, GLsizei count,
		        GLenum format, GLenum type, const void *table)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    GLenum rv;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    if ((rv = __glCheckColorSubTableArgs(gc, target,
    					start, count, format, type))) {
	__glSetError(rv);
	return;
    }
    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (start + count > ct->width) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    __glInitMemUnpack(gc, &spanInfo, count, 1, 0,
    				format, type, table, GL_FALSE);
    __glInitColorTableStore(ct, &spanInfo);
    spanInfo.dstSkipPixels = start;

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_TRUE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &ct->state.scale, &ct->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = ct->state.scale;
	spanInfo.bias = ct->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL | __GL_DIRTY_GENERIC);
}

void __gllei_ColorSubTable(__GLcontext *gc, GLenum target,
			GLsizei start, GLsizei count,
			GLenum format, GLenum type, const void *table)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    GLenum rv;
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

    if ((rv = __glCheckColorSubTableArgs(gc, target,
    					start, count, format, type))) {
	__glSetError(rv);
	return;
    }
    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (start + count > ct->width) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    __glInitMemUnpack(gc, &spanInfo, count, 1, 0,
    				format, type, table, GL_TRUE);
    __glInitColorTableStore(ct, &spanInfo);
    spanInfo.dstSkipPixels = start;

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_TRUE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &ct->state.scale, &ct->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = ct->state.scale;
	spanInfo.bias = ct->state.bias;
    }

    (*gc->procs.copyImage)(gc, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL | __GL_DIRTY_GENERIC);
}

void GLAPI __glim_CopyColorSubTable(GLenum target, GLsizei start,
				    GLint x, GLint y, GLsizei width)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    GLenum rv;
    __GLpixelSpanInfo spanInfo;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_PIXEL_OP();

    if ((rv = __glCheckColorSubTableArgs(gc, target,
    					start, width, GL_RGBA, GL_FLOAT))) {
	__glSetError(rv);
	return;
    }
    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (isProxy == GL_TRUE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __glInitReadImageSrcInfo(gc, &spanInfo, x, y, width, 1);
    __glInitColorTableStore(ct, &spanInfo);
    spanInfo.dstSkipPixels = start;

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_TRUE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    if (__glNeedScaleBias(gc, &ct->state.scale, &ct->state.bias)) {
	spanInfo.applyGenericScaleBias = GL_TRUE;
	spanInfo.scale = ct->state.scale;
	spanInfo.bias = ct->state.bias;
    }

    if (!__glClipReadPixels(gc, &spanInfo)) {
	return;
    }
    (*gc->procs.readImage)(gc, NULL, &spanInfo);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL | __GL_DIRTY_GENERIC);
}

void GLAPI __glim_GetColorTable(GLenum target, GLenum format, GLenum type,
			        void *table)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    GLenum rv;
    __GLpixelSpanInfo spanInfo;
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_GET();

    if ((rv = __glCheckColorTableArgs(gc, target, GL_RGB, 0, format, type))) {
	__glSetError(rv);
	return;
    }
    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (isProxy == GL_TRUE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __glInitMemGet(gc, &spanInfo, ct->width, 1, ct->baseFormat, ct->table);
    __glInitMemPack(gc, &spanInfo, ct->width, 1, 0, format, type, table);

    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    spanInfo.applySrcClamp = GL_FALSE;
    spanInfo.applyDstClamp = GL_TRUE;
    spanInfo.applyFbScale = GL_FALSE;
    spanInfo.applyPixelTransfer = GL_FALSE;
    (*gc->procs.copyImage)(gc, &spanInfo);
}

void GLAPI __glim_ColorTableParameteriv(GLenum target, GLenum pname,
				        const GLint *params)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    __GL_SETUP_NOT_IN_BEGIN();

    if (params == NULL) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (isProxy == GL_TRUE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
    case GL_COLOR_TABLE_SCALE:
	ct->state.scale.r = (__GLfloat)params[0];
	ct->state.scale.g = (__GLfloat)params[1];
	ct->state.scale.b = (__GLfloat)params[2];
	ct->state.scale.a = (__GLfloat)params[3];
	break;
    case GL_COLOR_TABLE_BIAS:
	ct->state.bias.r = (__GLfloat)params[0];
	ct->state.bias.g = (__GLfloat)params[1];
	ct->state.bias.b = (__GLfloat)params[2];
	ct->state.bias.a = (__GLfloat)params[3];
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_ColorTableParameterfv(GLenum target, GLenum pname,
				        const GLfloat *params)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    __GL_SETUP_NOT_IN_BEGIN();

    if (params == NULL) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (isProxy == GL_TRUE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
    case GL_COLOR_TABLE_SCALE:
	ct->state.scale.r = (__GLfloat)params[0];
	ct->state.scale.g = (__GLfloat)params[1];
	ct->state.scale.b = (__GLfloat)params[2];
	ct->state.scale.a = (__GLfloat)params[3];
	break;
    case GL_COLOR_TABLE_BIAS:
	ct->state.bias.r = (__GLfloat)params[0];
	ct->state.bias.g = (__GLfloat)params[1];
	ct->state.bias.b = (__GLfloat)params[2];
	ct->state.bias.a = (__GLfloat)params[3];
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetColorTableParameteriv(GLenum target, GLenum pname,
					   GLint *params)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    __GL_SETUP_NOT_IN_BEGIN();

    if (params == NULL) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
    case GL_COLOR_TABLE_FORMAT:
	*params = (GLint)ct->formatReturn;
	break;
    case GL_COLOR_TABLE_WIDTH:
	*params = (GLint)ct->width;
	break;
    case GL_COLOR_TABLE_RED_SIZE:
	*params = (GLint)ct->redSize;
	break;
    case GL_COLOR_TABLE_GREEN_SIZE:
	*params = (GLint)ct->greenSize;
	break;
    case GL_COLOR_TABLE_BLUE_SIZE:
	*params = (GLint)ct->blueSize;
	break;
    case GL_COLOR_TABLE_ALPHA_SIZE:
	*params = (GLint)ct->alphaSize;
	break;
    case GL_COLOR_TABLE_LUMINANCE_SIZE:
	*params = (GLint)ct->luminanceSize;
	break;
    case GL_COLOR_TABLE_INTENSITY_SIZE:
	*params = (GLint)ct->intensitySize;
	break;
    case GL_COLOR_TABLE_SCALE:
	if (isProxy) {
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	params[0] = (GLint)ct->state.scale.r;
	params[1] = (GLint)ct->state.scale.g;
	params[2] = (GLint)ct->state.scale.b;
	params[3] = (GLint)ct->state.scale.a;
	break;
    case GL_COLOR_TABLE_BIAS:
	if (isProxy) {
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	params[0] = (GLint)ct->state.bias.r;
	params[1] = (GLint)ct->state.bias.g;
	params[2] = (GLint)ct->state.bias.b;
	params[3] = (GLint)ct->state.bias.a;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetColorTableParameterfv(GLenum target, GLenum pname,
					   GLfloat *params)
{
    __GLcolorTable *ct;
    GLboolean isProxy;
    __GL_SETUP_NOT_IN_BEGIN();

    if (params == NULL) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if ((ct = LookupColorTable(gc, target, &isProxy)) == NULL) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
    case GL_COLOR_TABLE_FORMAT:
	*params = (GLfloat)ct->formatReturn;
	break;
    case GL_COLOR_TABLE_WIDTH:
	*params = (GLfloat)ct->width;
	break;
    case GL_COLOR_TABLE_RED_SIZE:
	*params = (GLfloat)ct->redSize;
	break;
    case GL_COLOR_TABLE_GREEN_SIZE:
	*params = (GLfloat)ct->greenSize;
	break;
    case GL_COLOR_TABLE_BLUE_SIZE:
	*params = (GLfloat)ct->blueSize;
	break;
    case GL_COLOR_TABLE_ALPHA_SIZE:
	*params = (GLfloat)ct->alphaSize;
	break;
    case GL_COLOR_TABLE_LUMINANCE_SIZE:
	*params = (GLfloat)ct->luminanceSize;
	break;
    case GL_COLOR_TABLE_INTENSITY_SIZE:
	*params = (GLfloat)ct->intensitySize;
	break;
    case GL_COLOR_TABLE_SCALE:
	if (isProxy) {
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	params[0] = (GLfloat)ct->state.scale.r;
	params[1] = (GLfloat)ct->state.scale.g;
	params[2] = (GLfloat)ct->state.scale.b;
	params[3] = (GLfloat)ct->state.scale.a;
	break;
    case GL_COLOR_TABLE_BIAS:
	if (isProxy) {
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	params[0] = (GLfloat)ct->state.bias.r;
	params[1] = (GLfloat)ct->state.bias.g;
	params[2] = (GLfloat)ct->state.bias.b;
	params[3] = (GLfloat)ct->state.bias.a;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void __glEarlyInitColorTableState(__GLcontext *gc)
{
    GLint i;

    for (i=0; i<__GL_NUM_COLOR_TABLE_TARGETS; ++i) {
	__GLcolorTable *ct = &gc->pixel.colorTable[i];

	gc->state.pixel.colorTable[i] = &ct->state;
    }
}

void __glInitColorTableState(__GLcontext *gc)
{
    static const GLenum targets[] = {
        GL_COLOR_TABLE,
        GL_POST_CONVOLUTION_COLOR_TABLE,
        GL_POST_COLOR_MATRIX_COLOR_TABLE,
    };
    static const GLenum proxyTargets[] = {
        GL_PROXY_COLOR_TABLE,
        GL_PROXY_POST_CONVOLUTION_COLOR_TABLE,
        GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE,
    };
    GLint i;

    for (i=0; i<__GL_NUM_COLOR_TABLE_TARGETS; ++i) {
	__GLcolorTable *ct;

	ct = &gc->pixel.colorTable[i];
	ct->target = targets[i];
	ct->format =
	ct->formatReturn =
	ct->baseFormat = GL_RGBA;
	ct->type = GL_FLOAT;

	ct->state.scale.r = __glOne;
	ct->state.scale.g = __glOne;
	ct->state.scale.b = __glOne;
	ct->state.scale.a = __glOne;

	ct = &gc->pixel.proxyColorTable[i];
	ct->target = proxyTargets[i];
	ct->format =
	ct->formatReturn =
	ct->baseFormat = GL_RGBA;
	ct->type = GL_FLOAT;

	ct->state.scale.r = __glOne;
	ct->state.scale.g = __glOne;
	ct->state.scale.b = __glOne;
	ct->state.scale.a = __glOne;
    }
}

void __glFreeColorTableState(__GLcontext *gc)
{
    GLint i; 

    for (i=0; i<__GL_NUM_COLOR_TABLE_TARGETS; ++i) {
	__GLcolorTable *ct = &gc->pixel.colorTable[i];

	if (ct->table) {
	    (*gc->imports.free)(gc, ct->table);
	}
    }
}

