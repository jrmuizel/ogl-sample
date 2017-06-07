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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_get.c#26 $
*/
#include "global.h"
#include "context.h"
#include "g_imfncs.h"
#include "pixel.h"
#include "render.h"
#include "g_imports.h"
#include "texmgr.h"


#define __GL_FLOAT	0	/* __GLfloat */
#define __GL_FLOAT32	1	/* api 32 bit float */
#define __GL_FLOAT64	2	/* api 64 bit float */
#define __GL_INT32	3	/* api 32 bit int */
#define __GL_BOOLEAN	4	/* api 8 bit boolean */
#define __GL_COLOR	5	/* unscaled color in __GLfloat */
#define __GL_SCOLOR	6	/* scaled color in __GLfloat */

void __glConvertResult(__GLcontext *gc, GLint fromType, const void *rawdata,
		       GLint toType, void *result, GLint size);

void GLAPI __glim_GetTexEnvfv(GLenum target,
			      GLenum pname, GLfloat v[])
{
    __GLcolor color;
    __GL_SETUP_NOT_IN_BEGIN();

    if (target != GL_TEXTURE_ENV) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_GET();

    switch (pname) {
      case GL_TEXTURE_ENV_MODE:
	v[0] = gc->state.texture.active->env[0].mode;
	break;
      case GL_TEXTURE_ENV_COLOR:
	color.r = gc->state.texture.active->env[0].color.r *
		gc->constants.redScale * gc->frontBuffer.oneOverRedScale;
	color.g = gc->state.texture.active->env[0].color.g *
		gc->constants.greenScale * gc->frontBuffer.oneOverGreenScale;
	color.b = gc->state.texture.active->env[0].color.b *
		gc->constants.blueScale * gc->frontBuffer.oneOverBlueScale;
	color.a = gc->state.texture.active->env[0].color.a *
		gc->constants.alphaScale * gc->frontBuffer.oneOverAlphaScale;
	__glUnScaleColorf(gc, v, &color);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetTexEnviv(GLenum target,
			      GLenum pname, GLint v[])
{
    __GLcolor color;
    __GL_SETUP_NOT_IN_BEGIN();

    if (target != GL_TEXTURE_ENV) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_GET();

    switch (pname) {
      case GL_TEXTURE_ENV_MODE:
	v[0] = gc->state.texture.active->env[0].mode;
	break;
      case GL_TEXTURE_ENV_COLOR:
	color.r = gc->state.texture.active->env[0].color.r *
		gc->constants.redScale * gc->frontBuffer.oneOverRedScale;
	color.g = gc->state.texture.active->env[0].color.g *
		gc->constants.greenScale * gc->frontBuffer.oneOverGreenScale;
	color.b = gc->state.texture.active->env[0].color.b *
		gc->constants.blueScale * gc->frontBuffer.oneOverBlueScale;
	color.a = gc->state.texture.active->env[0].color.a *
		gc->constants.alphaScale * gc->frontBuffer.oneOverAlphaScale;
	__glUnScaleColori(gc, v, &color);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/************************************************************************/

void GLAPI __glim_GetTexGenfv(GLenum coord, GLenum pname,
			      GLfloat v[])
{
    __GLtextureGenState* tcs;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    switch (coord) {
      case GL_S:
	tcs = &gc->state.texture.active->s;
	break;
      case GL_T:
	tcs = &gc->state.texture.active->t;
	break;
      case GL_R:
	tcs = &gc->state.texture.active->r;
	break;
      case GL_Q:
	tcs = &gc->state.texture.active->q;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	v[0] = tcs->mode;
	break;
      case GL_OBJECT_PLANE:
	v[0] = tcs->objectPlaneEquation.x;
	v[1] = tcs->objectPlaneEquation.y;
	v[2] = tcs->objectPlaneEquation.z;
	v[3] = tcs->objectPlaneEquation.w;
	break;
      case GL_EYE_PLANE:
	v[0] = tcs->eyePlaneEquation.x;
	v[1] = tcs->eyePlaneEquation.y;
	v[2] = tcs->eyePlaneEquation.z;
	v[3] = tcs->eyePlaneEquation.w;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetTexGendv(GLenum coord, GLenum pname,
			      GLdouble v[])
{
    __GLtextureGenState* tcs;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    switch (coord) {
      case GL_S:
	tcs = &gc->state.texture.active->s;
	break;
      case GL_T:
	tcs = &gc->state.texture.active->t;
	break;
      case GL_R:
	tcs = &gc->state.texture.active->r;
	break;
      case GL_Q:
	tcs = &gc->state.texture.active->q;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	v[0] = tcs->mode;
	break;
      case GL_OBJECT_PLANE:
	v[0] = tcs->objectPlaneEquation.x;
	v[1] = tcs->objectPlaneEquation.y;
	v[2] = tcs->objectPlaneEquation.z;
	v[3] = tcs->objectPlaneEquation.w;
	break;
      case GL_EYE_PLANE:
	v[0] = tcs->eyePlaneEquation.x;
	v[1] = tcs->eyePlaneEquation.y;
	v[2] = tcs->eyePlaneEquation.z;
	v[3] = tcs->eyePlaneEquation.w;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetTexGeniv(GLenum coord, GLenum pname,
			      GLint v[])
{
    __GLtextureGenState* tcs;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    switch (coord) {
      case GL_S:
	tcs = &gc->state.texture.active->s;
	break;
      case GL_T:
	tcs = &gc->state.texture.active->t;
	break;
      case GL_R:
	tcs = &gc->state.texture.active->r;
	break;
      case GL_Q:
	tcs = &gc->state.texture.active->q;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	v[0] = tcs->mode;
	break;
      case GL_OBJECT_PLANE:
	__glConvertResult(gc, __GL_FLOAT, &tcs->objectPlaneEquation.x,
			  __GL_INT32, v, 4);
	break;
      case GL_EYE_PLANE:
	__glConvertResult(gc, __GL_FLOAT, &tcs->eyePlaneEquation.x,
			  __GL_INT32, v, 4);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/************************************************************************/

void GLAPI __glim_GetTexParameterfv(GLenum target,
				    GLenum pname, GLfloat v[])
{
    __GLtextureParamState *pts;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    pts = __glLookupActiveTextureParams(gc, target);

    if (!pts) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
      case GL_TEXTURE_WRAP_S:
	v[0] = pts->sWrapMode;
	break;
      case GL_TEXTURE_WRAP_T:
	v[0] = pts->tWrapMode;
	break;
      case GL_TEXTURE_WRAP_R:
	v[0] = pts->rWrapMode;
	break;
      case GL_TEXTURE_MIN_FILTER:
	v[0] = pts->minFilter;
	break;
      case GL_TEXTURE_MAG_FILTER:
	v[0] = pts->magFilter;
	break;
      case GL_TEXTURE_BORDER_COLOR:
	v[0] = pts->borderColor.r;
	v[1] = pts->borderColor.g;
	v[2] = pts->borderColor.b;
	v[3] = pts->borderColor.a;
	break;
      case GL_TEXTURE_PRIORITY:
	v[0] = pts->priority;
	break;
      case GL_TEXTURE_RESIDENT:
	{
	    __GLtexture *tex = __glLookupActiveTexture(gc, target);
	    v[0] = (GLfloat)(tex->residence != __GL_TEXTURE_NOT_RESIDENT);
	}
	break;
      case GL_TEXTURE_MIN_LOD:
	v[0] = pts->minLOD;
	break;
      case GL_TEXTURE_MAX_LOD:
	v[0] = pts->maxLOD;
	break;
      case GL_TEXTURE_BASE_LEVEL:
	v[0] = (GLfloat)(pts->baseLevel);
	break;
      case GL_TEXTURE_MAX_LEVEL:
	v[0] = (GLfloat)(pts->maxLevel);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetTexParameteriv(GLenum target,
				    GLenum pname, GLint v[])
{
    __GLtextureParamState *pts;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    pts = __glLookupActiveTextureParams(gc, target);

    if (!pts) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
      case GL_TEXTURE_WRAP_S:
	v[0] = pts->sWrapMode;
	break;
      case GL_TEXTURE_WRAP_T:
	v[0] = pts->tWrapMode;
	break;
      case GL_TEXTURE_WRAP_R:
	v[0] = pts->rWrapMode;
	break;
      case GL_TEXTURE_MIN_FILTER:
	v[0] = pts->minFilter;
	break;
      case GL_TEXTURE_MAG_FILTER:
	v[0] = pts->magFilter;
	break;
      case GL_TEXTURE_BORDER_COLOR:
	v[0] = __GL_FLOAT_TO_I(pts->borderColor.r);
	v[1] = __GL_FLOAT_TO_I(pts->borderColor.g);
	v[2] = __GL_FLOAT_TO_I(pts->borderColor.b);
	v[3] = __GL_FLOAT_TO_I(pts->borderColor.a);
	break;
      case GL_TEXTURE_PRIORITY:
	v[0] = __GL_FLOAT_TO_I(pts->priority);
	break;
      case GL_TEXTURE_RESIDENT:
	{
	    __GLtexture *tex = __glLookupActiveTexture(gc, target);
	    v[0] = (GLint)(tex->residence != __GL_TEXTURE_NOT_RESIDENT);
	}
	break;
      case GL_TEXTURE_MIN_LOD:
	v[0] = (GLint)pts->minLOD;
	break;
      case GL_TEXTURE_MAX_LOD:
	v[0] = (GLint)pts->maxLOD;
	break;
      case GL_TEXTURE_BASE_LEVEL:
	v[0] = pts->baseLevel;
	break;
      case GL_TEXTURE_MAX_LEVEL:
	v[0] = pts->maxLevel;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/************************************************************************/

void GLAPI __glim_GetTexLevelParameterfv(GLenum target, GLint level,
					 GLenum pname, GLfloat v[])
{
    __GLtexture *tex;
    __GLmipMapLevel *lp;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    tex = __glLookupActiveTexture(gc, target);

    if (!tex) {
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if ((level < 0) || (level >= gc->constants.maxMipMapLevel)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    lp = tex->level[level];

    switch (pname) {
      case GL_TEXTURE_WIDTH:
	v[0] = lp->width;
	break;
      case GL_TEXTURE_HEIGHT:
	if (tex->dim < 2) {
	    v[0] = lp->height - lp->border*2;
	} else {
	    v[0] = lp->height;
	}
	break;
      case GL_TEXTURE_DEPTH:
	if (tex->dim < 3) {
	    v[0] = lp->depth - lp->border*2;
	} else {
	    v[0] = lp->depth;
	}
	break;
      case GL_TEXTURE_INTERNAL_FORMAT:
	v[0] = lp->requestedFormat;
	break;
      case GL_TEXTURE_BORDER:
	v[0] = lp->border;
	break;
      case GL_TEXTURE_RED_SIZE:
	v[0] = lp->redSize;
	break;
      case GL_TEXTURE_GREEN_SIZE:
	v[0] = lp->greenSize;
	break;
      case GL_TEXTURE_BLUE_SIZE:
	v[0] = lp->blueSize;
	break;
      case GL_TEXTURE_ALPHA_SIZE:
	v[0] = lp->alphaSize;
	break;
      case GL_TEXTURE_LUMINANCE_SIZE:
	v[0] = lp->luminanceSize;
	break;
      case GL_TEXTURE_INTENSITY_SIZE:
	v[0] = lp->intensitySize;
	break;
      default:
	goto bad_enum;
    }
}

void GLAPI __glim_GetTexLevelParameteriv(GLenum target, GLint level,
					 GLenum pname, GLint v[])
{
    __GLtexture *tex;
    __GLmipMapLevel *lp;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    tex = __glLookupActiveTexture(gc, target);

    if (!tex) {
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if ((level < 0) || (level >= gc->constants.maxMipMapLevel)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    lp = tex->level[level];

    switch (pname) {
      case GL_TEXTURE_WIDTH:
	v[0] = lp->width;
	break;
      case GL_TEXTURE_HEIGHT:
	if (tex->dim < 2) {
	    v[0] = lp->height - lp->border*2;
	} else {
	    v[0] = lp->height;
	}
	break;
      case GL_TEXTURE_DEPTH:
	if (tex->dim < 3) {
	    v[0] = lp->depth - lp->border*2;
	} else {
	    v[0] = lp->depth;
	}
	break;
      case GL_TEXTURE_INTERNAL_FORMAT:
	v[0] = lp->requestedFormat;
	break;
      case GL_TEXTURE_BORDER:
	v[0] = lp->border;
	break;
      case GL_TEXTURE_RED_SIZE:
	v[0] = lp->redSize;
	break;
      case GL_TEXTURE_GREEN_SIZE:
	v[0] = lp->greenSize;
	break;
      case GL_TEXTURE_BLUE_SIZE:
	v[0] = lp->blueSize;
	break;
      case GL_TEXTURE_ALPHA_SIZE:
	v[0] = lp->alphaSize;
	break;
      case GL_TEXTURE_LUMINANCE_SIZE:
	v[0] = lp->luminanceSize;
	break;
      case GL_TEXTURE_INTENSITY_SIZE:
	v[0] = lp->intensitySize;
	break;
      default:
	goto bad_enum;
    }
}

/************************************************************************/

void GLAPI __glim_GetClipPlane(GLenum plane, GLdouble eqn[4])
{
    GLint index;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    index = plane - GL_CLIP_PLANE0;
    if ((index < 0) || (index >= gc->constants.numberOfClipPlanes)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    eqn[0] = gc->state.transform.eyeClipPlanes[index].x;
    eqn[1] = gc->state.transform.eyeClipPlanes[index].y;
    eqn[2] = gc->state.transform.eyeClipPlanes[index].z;
    eqn[3] = gc->state.transform.eyeClipPlanes[index].w;
}

/************************************************************************/

void __glInitImagePack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       GLint width, GLint height, GLenum format, GLenum type,
		       GLvoid *buf)
{
    spanInfo->x = __glZero;
    spanInfo->zoomx = __glOne;
    spanInfo->width = width;
    spanInfo->height = height;
    spanInfo->depth = 1;
    spanInfo->dim = 2;

    spanInfo->srcAlignment = 4;
    spanInfo->srcSkipImages = 0;
    spanInfo->srcSkipPixels = 0;
    spanInfo->srcSkipLines = 0;
    spanInfo->srcSwapBytes = GL_FALSE;
#ifdef __GL_STIPPLE_MSB
    spanInfo->srcLsbFirst = GL_FALSE;
#else
    spanInfo->srcLsbFirst = GL_TRUE;
#endif
    spanInfo->srcLineLength = width;
    spanInfo->srcImageHeight = height;

    spanInfo->dstFormat = format;
    spanInfo->dstType = type;
    spanInfo->dstImage = buf;
    __glLoadPackModes(gc, spanInfo);
}

void GLAPI __glim_GetTexImage(GLenum target, GLint level, GLenum format,
			      GLenum type, GLvoid *texels)
{
    GLint width, height, depth;
    __GLtexture *tex;
    __GLmipMapLevel *lp;
    __GLpixelSpanInfo spanInfo;
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_GET();

    tex = __glLookupActiveTexture(gc, target);

    if (!tex || (target == GL_PROXY_TEXTURE_1D) ||
		(target == GL_PROXY_TEXTURE_2D) ||
		(target == GL_PROXY_TEXTURE_3D))
    {
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if ((level < 0) || (level >= gc->constants.maxMipMapLevel)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    switch (format) {
      case GL_RGBA:
      case GL_RGB:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
	break;
      default:
	goto bad_enum;
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
	  case GL_ABGR_EXT:
	  case GL_BGRA:
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	goto bad_enum;
    }

    lp = tex->level[level];

    width = lp->width;
    if (tex->dim < 2) {
	height = lp->height - lp->border*2;
    } else {
	height = lp->height;
    }
    if (tex->dim < 3) {
	depth = lp->depth - lp->border*2;
    } else {
	depth = lp->depth;
    }

    /* If no texture has been loaded */
    if (!width || !height || !depth) return;

    __glInitTexDestPack(gc, &spanInfo, width, height, depth,
			format, type, texels);
    __glInitTexImageGet(gc, &spanInfo, tex, level);
    __glInitPacker(gc, &spanInfo);
    __glInitUnpacker(gc, &spanInfo);
    spanInfo.applyPixelTransfer = GL_FALSE;
    (*gc->procs.fetchTex)(gc, lp, &spanInfo);
}

/************************************************************************/

void GLAPI __glim_GetPolygonStipple(GLubyte *outImage)
{
    __GLpixelSpanInfo spanInfo;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    spanInfo.srcImage = &(gc->state.polygonStipple.stipple[0]);
    spanInfo.srcType = GL_BITMAP;
    spanInfo.srcFormat = GL_COLOR_INDEX;
    __glInitImagePack(gc, &spanInfo, 32, 32, GL_COLOR_INDEX, GL_BITMAP,
	    outImage);
    __glInitPacker(gc, &spanInfo);
    __glInitUnpacker(gc, &spanInfo);
    (*gc->procs.copyImage)(gc, &spanInfo);
}

/************************************************************************/

void GLAPI __glim_GetLightfv(GLenum light, GLenum pname,
		       GLfloat result[])
{
    GLint index;
    __GLlightSourceState *src;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    index = light - GL_LIGHT0;
    if ((index < 0) || (index >= gc->constants.numberOfLights)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    src = &gc->state.light.source[index];
    switch (pname) {
      case GL_AMBIENT:
	__glUnScaleColorf(gc, result, &src->ambient);
	break;
      case GL_DIFFUSE:
	__glUnScaleColorf(gc, result, &src->diffuse);
	break;
      case GL_SPECULAR:
	__glUnScaleColorf(gc, result, &src->specular);
	break;
      case GL_POSITION:
	result[0] = src->positionEye.x;
	result[1] = src->positionEye.y;
	result[2] = src->positionEye.z;
	result[3] = src->positionEye.w;
	break;
      case GL_SPOT_DIRECTION:
	result[0] = src->direction.x;
	result[1] = src->direction.y;
	result[2] = src->direction.z;
	break;
      case GL_SPOT_EXPONENT:
	result[0] = src->spotLightExponent;
	break;
      case GL_SPOT_CUTOFF:
	result[0] = src->spotLightCutOffAngle;
	break;
      case GL_CONSTANT_ATTENUATION:
	result[0] = src->constantAttenuation;
	break;
      case GL_LINEAR_ATTENUATION:
	result[0] = src->linearAttenuation;
	break;
      case GL_QUADRATIC_ATTENUATION:
	result[0] = src->quadraticAttenuation;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetLightiv(GLenum light, GLenum pname,
			     GLint result[])
{
    GLint index;
    __GLlightSourceState *src;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    index = light - GL_LIGHT0;
    if ((index < 0) || (index >= gc->constants.numberOfLights)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    src = &gc->state.light.source[index];
    switch (pname) {
      case GL_AMBIENT:
	__glUnScaleColori(gc, result, &src->ambient);
	break;
      case GL_DIFFUSE:
	__glUnScaleColori(gc, result, &src->diffuse);
	break;
      case GL_SPECULAR:
	__glUnScaleColori(gc, result, &src->specular);
	break;
      case GL_POSITION:
	__glConvertResult(gc, __GL_FLOAT, &src->positionEye.x,
			  __GL_INT32, result, 4);
	break;
      case GL_SPOT_DIRECTION:
	__glConvertResult(gc, __GL_FLOAT, &src->direction.x,
			  __GL_INT32, result, 3);
	break;
      case GL_SPOT_EXPONENT:
	__glConvertResult(gc, __GL_FLOAT, &src->spotLightExponent,
			  __GL_INT32, result, 1);
	break;
      case GL_SPOT_CUTOFF:
	__glConvertResult(gc, __GL_FLOAT, &src->spotLightCutOffAngle,
			  __GL_INT32, result, 1);
	break;
      case GL_CONSTANT_ATTENUATION:
	__glConvertResult(gc, __GL_FLOAT, &src->constantAttenuation,
			  __GL_INT32, result, 1);
	break;
      case GL_LINEAR_ATTENUATION:
	__glConvertResult(gc, __GL_FLOAT, &src->linearAttenuation,
			  __GL_INT32, result, 1);
	break;
      case GL_QUADRATIC_ATTENUATION:
	__glConvertResult(gc, __GL_FLOAT, &src->quadraticAttenuation,
			  __GL_INT32, result, 1);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/************************************************************************/

void GLAPI __glim_GetMaterialfv(GLenum face, GLenum pname,
				GLfloat result[])
{
    __GLmaterialState *mat;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    switch (face) {
      case GL_FRONT:
	mat = &gc->state.light.front;
	break;
      case GL_BACK:
	mat = &gc->state.light.back;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
      case GL_COLOR_INDEXES:
	result[0] = mat->cmapa;
	result[1] = mat->cmapd;
	result[2] = mat->cmaps;
	break;
      case GL_SHININESS:
	result[0] = mat->specularExponent;
	break;
      case GL_EMISSION:
	__glUnScaleColorf(gc, result, &mat->emissive);
	break;
      case GL_AMBIENT:
	result[0] = mat->ambient.r;
	result[1] = mat->ambient.g;
	result[2] = mat->ambient.b;
	result[3] = mat->ambient.a;
	break;
      case GL_DIFFUSE:
	result[0] = mat->diffuse.r;
	result[1] = mat->diffuse.g;
	result[2] = mat->diffuse.b;
	result[3] = mat->diffuse.a;
	break;
      case GL_SPECULAR:
	result[0] = mat->specular.r;
	result[1] = mat->specular.g;
	result[2] = mat->specular.b;
	result[3] = mat->specular.a;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetMaterialiv(GLenum face, GLenum pname,
				GLint result[])
{
    __GLmaterialState *mat;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    switch (face) {
      case GL_FRONT:
	mat = &gc->state.light.front;
	break;
      case GL_BACK:
	mat = &gc->state.light.back;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (pname) {
      case GL_COLOR_INDEXES:
	__glConvertResult(gc, __GL_FLOAT, &mat->cmapa,
			  __GL_INT32, result, 3);
	break;
      case GL_SHININESS:
	__glConvertResult(gc, __GL_FLOAT, &mat->specularExponent,
			  __GL_INT32, result, 1);
	break;
      case GL_EMISSION:
	__glUnScaleColori(gc, result, &mat->emissive);
	break;
      case GL_AMBIENT:
	result[0] = __GL_FLOAT_TO_I(mat->ambient.r);
	result[1] = __GL_FLOAT_TO_I(mat->ambient.g);
	result[2] = __GL_FLOAT_TO_I(mat->ambient.b);
	result[3] = __GL_FLOAT_TO_I(mat->ambient.a);
	break;
      case GL_DIFFUSE:
	result[0] = __GL_FLOAT_TO_I(mat->diffuse.r);
	result[1] = __GL_FLOAT_TO_I(mat->diffuse.g);
	result[2] = __GL_FLOAT_TO_I(mat->diffuse.b);
	result[3] = __GL_FLOAT_TO_I(mat->diffuse.a);
	break;
      case GL_SPECULAR:
	result[0] = __GL_FLOAT_TO_I(mat->specular.r);
	result[1] = __GL_FLOAT_TO_I(mat->specular.g);
	result[2] = __GL_FLOAT_TO_I(mat->specular.b);
	result[3] = __GL_FLOAT_TO_I(mat->specular.a);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/************************************************************************/

void GLAPI __glim_GetMapfv(GLenum target, GLenum query, GLfloat buf[])
{
    __GLevaluator1 *eval1;
    __GLevaluator2 *eval2;
    __GLfloat *eval1Data, *eval2Data;
    GLfloat *rp;
    GLint index, i, t;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    /*
    ** Check if target is valid.
    */
    rp = buf;
    switch (target) {
      case GL_MAP1_COLOR_4:
      case GL_MAP1_INDEX:
      case GL_MAP1_NORMAL:
      case GL_MAP1_TEXTURE_COORD_1:
      case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3:
      case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3:
      case GL_MAP1_VERTEX_4:
	index = __GL_EVAL1D_INDEX(target);
	eval1 = &gc->eval.eval1[index];
	switch (query) {
	  case GL_COEFF:
	    t = eval1->order * eval1->k;
	    eval1Data = gc->eval.eval1Data[index];
	    for (i = 0; i < t; i++) {
		*rp++ = eval1Data[i];
	    }
	    break;
	  case GL_DOMAIN:
	    *rp++ = eval1->u1;
	    *rp++ = eval1->u2;
	    break;
	  case GL_ORDER:
	    *rp++ = gc->eval.eval1[index].order;
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_MAP2_COLOR_4:
      case GL_MAP2_INDEX:
      case GL_MAP2_NORMAL:
      case GL_MAP2_TEXTURE_COORD_1:
      case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3:
      case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3:
      case GL_MAP2_VERTEX_4:
	index = __GL_EVAL2D_INDEX(target);
	eval2 = &gc->eval.eval2[index];
	switch (query) {
	  case GL_COEFF:
	    eval2Data = gc->eval.eval2Data[index];
	    t = eval2->majorOrder * eval2->minorOrder * eval2->k;
	    for (i = 0; i < t; i++) {
		*rp++ = eval2Data[i];
	    }
	    break;
	  case GL_DOMAIN:
	    *rp++ = eval2->u1;
	    *rp++ = eval2->u2;
	    *rp++ = eval2->v1;
	    *rp++ = eval2->v2;
	    break;
	  case GL_ORDER:
	    *rp++ = gc->eval.eval2[index].majorOrder;
	    *rp++ = gc->eval.eval2[index].minorOrder;
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetMapdv(GLenum target, GLenum query, GLdouble buf[])
{
    __GLevaluator1 *eval1;
    __GLevaluator2 *eval2;
    __GLfloat *eval1Data, *eval2Data;
    GLdouble *rp;
    GLint index, i, t;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    /*
    ** Check if target is valid.
    */
    rp = buf;
    switch (target) {
      case GL_MAP1_COLOR_4:
      case GL_MAP1_INDEX:
      case GL_MAP1_NORMAL:
      case GL_MAP1_TEXTURE_COORD_1:
      case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3:
      case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3:
      case GL_MAP1_VERTEX_4:
	index = __GL_EVAL1D_INDEX(target);
	eval1 = &gc->eval.eval1[index];
	switch (query) {
	  case GL_COEFF:
	    eval1Data = gc->eval.eval1Data[index];
	    t = eval1->order * eval1->k;
	    for (i = 0; i < t; i++) {
		*rp++ = eval1Data[i];
	    }
	    break;
	  case GL_DOMAIN:
	    *rp++ = eval1->u1;
	    *rp++ = eval1->u2;
	    break;
	  case GL_ORDER:
	    *rp++ = gc->eval.eval1[index].order;
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_MAP2_COLOR_4:
      case GL_MAP2_INDEX:
      case GL_MAP2_NORMAL:
      case GL_MAP2_TEXTURE_COORD_1:
      case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3:
      case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3:
      case GL_MAP2_VERTEX_4:
	index = __GL_EVAL2D_INDEX(target);
	eval2 = &gc->eval.eval2[index];
	switch (query) {
	  case GL_COEFF:
	    eval2Data = gc->eval.eval2Data[index];
	    t = eval2->majorOrder * eval2->minorOrder * eval2->k;
	    for (i = 0; i < t; i++) {
		*rp++ = eval2Data[i];
	    }
	    break;
	  case GL_DOMAIN:
	    *rp++ = eval2->u1;
	    *rp++ = eval2->u2;
	    *rp++ = eval2->v1;
	    *rp++ = eval2->v2;
	    break;
	  case GL_ORDER:
	    *rp++ = gc->eval.eval2[index].majorOrder;
	    *rp++ = gc->eval.eval2[index].minorOrder;
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetMapiv(GLenum target, GLenum query, GLint buf[])
{
    __GLevaluator1 *eval1;
    __GLevaluator2 *eval2;
    __GLfloat *eval1Data, *eval2Data;
    GLint *rp;
    GLint index, t;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    /*
    ** Check if target is valid.
    */
    rp = buf;
    switch (target) {
      case GL_MAP1_COLOR_4:
      case GL_MAP1_INDEX:
      case GL_MAP1_NORMAL:
      case GL_MAP1_TEXTURE_COORD_1:
      case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3:
      case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3:
      case GL_MAP1_VERTEX_4:
	index = __GL_EVAL1D_INDEX(target);
	eval1 = &gc->eval.eval1[index];
	switch (query) {
	  case GL_COEFF:
	    eval1Data = gc->eval.eval1Data[index];
	    t = eval1->order * eval1->k;
	    __glConvertResult(gc, __GL_FLOAT, eval1Data,
			      __GL_INT32, rp, t);
	    break;
	  case GL_DOMAIN:
	    __glConvertResult(gc, __GL_FLOAT, &eval1->u1,
			      __GL_INT32, rp, 2);
	    break;
	  case GL_ORDER:
	    *rp++ = gc->eval.eval1[index].order;
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_MAP2_COLOR_4:
      case GL_MAP2_INDEX:
      case GL_MAP2_NORMAL:
      case GL_MAP2_TEXTURE_COORD_1:
      case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3:
      case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3:
      case GL_MAP2_VERTEX_4:
	index = __GL_EVAL2D_INDEX(target);
	eval2 = &gc->eval.eval2[index];
	switch (query) {
	  case GL_COEFF:
	    eval2Data = gc->eval.eval2Data[index];
	    t = eval2->majorOrder * eval2->minorOrder * eval2->k;
	    __glConvertResult(gc, __GL_FLOAT, eval2Data,
			      __GL_INT32, rp, t);
	    break;
	  case GL_DOMAIN:
	    __glConvertResult(gc, __GL_FLOAT, &eval2->u1,
			      __GL_INT32, rp, 4);
	    break;
	  case GL_ORDER:
	    *rp++ = gc->eval.eval2[index].majorOrder;
	    *rp++ = gc->eval.eval2[index].minorOrder;
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/*****************************************************************************/

void GLAPI __glim_GetPixelMapfv(GLenum map, GLfloat buf[])
{
    GLint index;
    GLint limit;
    GLfloat *rp;
    __GLpixelMapHead *pMap;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    pMap = gc->pixel.pixelMap;
    index = map - GL_PIXEL_MAP_I_TO_I;
    rp = buf;
    switch (map) {
      case GL_PIXEL_MAP_I_TO_I:
      case GL_PIXEL_MAP_S_TO_S:
	{
	    GLint *fromp = pMap[index].base.mapI;
	    limit = pMap[index].size;
	    while (--limit >= 0) {
		*rp++ = *fromp++;
	    }
	}
	break;
      case GL_PIXEL_MAP_I_TO_R:      case GL_PIXEL_MAP_I_TO_G:
      case GL_PIXEL_MAP_I_TO_B:      case GL_PIXEL_MAP_I_TO_A:
      case GL_PIXEL_MAP_R_TO_R:      case GL_PIXEL_MAP_G_TO_G:
      case GL_PIXEL_MAP_B_TO_B:      case GL_PIXEL_MAP_A_TO_A:
	{
	    __GLfloat *fromp = pMap[index].base.mapF;
	    limit = pMap[index].size;
	    while (--limit >= 0) {
		*rp++ = *fromp++;
	    }
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetPixelMapuiv(GLenum map, GLuint buf[])
{
    GLint index;
    GLint limit;
    GLuint *rp;
    __GLpixelMapHead *pMap;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    pMap = gc->pixel.pixelMap;
    index = map - GL_PIXEL_MAP_I_TO_I;
    rp = buf;
    switch (map) {
      case GL_PIXEL_MAP_I_TO_I:
      case GL_PIXEL_MAP_S_TO_S:
	{
	    GLint *fromp = pMap[index].base.mapI;
	    limit = pMap[index].size;
	    while (--limit >= 0) {
		*rp++ = *fromp++;
	    }
	}
	break;
      case GL_PIXEL_MAP_I_TO_R:      case GL_PIXEL_MAP_I_TO_G:
      case GL_PIXEL_MAP_I_TO_B:      case GL_PIXEL_MAP_I_TO_A:
      case GL_PIXEL_MAP_R_TO_R:      case GL_PIXEL_MAP_G_TO_G:
      case GL_PIXEL_MAP_B_TO_B:      case GL_PIXEL_MAP_A_TO_A:
	{
	    __GLfloat *fromp = pMap[index].base.mapF;
	    limit = pMap[index].size;
	    while (--limit >= 0) {
		*rp++ = (GLuint) *fromp++;
	    }
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_GetPixelMapusv(GLenum map, GLushort buf[])
{
    GLint index;
    GLint limit;
    GLushort *rp;
    __GLpixelMapHead *pMap;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    pMap = gc->pixel.pixelMap;
    index = map - GL_PIXEL_MAP_I_TO_I;
    rp = buf;
    switch (map) {
      case GL_PIXEL_MAP_I_TO_I:
      case GL_PIXEL_MAP_S_TO_S:
	{
	    GLint *fromp = pMap[index].base.mapI;
	    limit = pMap[index].size;
	    while (--limit >= 0) {
		*rp++ = (GLushort) *fromp++;
	    }
	}
	break;
      case GL_PIXEL_MAP_I_TO_R:      case GL_PIXEL_MAP_I_TO_G:
      case GL_PIXEL_MAP_I_TO_B:      case GL_PIXEL_MAP_I_TO_A:
      case GL_PIXEL_MAP_R_TO_R:      case GL_PIXEL_MAP_G_TO_G:
      case GL_PIXEL_MAP_B_TO_B:      case GL_PIXEL_MAP_A_TO_A:
	{
	    __GLfloat *fromp = pMap[index].base.mapF;
	    limit = pMap[index].size;
	    while (--limit >= 0) {
		*rp++ = (GLushort) *fromp++;
	    }
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/************************************************************************/

/*
** Convert the results of a query from one type to another.
*/
void __glConvertResult(__GLcontext *gc, GLint fromType, const void *rawdata,
		       GLint toType, void *result, GLint size)
{
    GLint i;

    switch (fromType) {
      case __GL_FLOAT:
	switch (toType) {
	  case __GL_FLOAT32:
	    for (i=0; i < size; i++) {
		((GLfloat *)result)[i] = ((const __GLfloat *)rawdata)[i];
	    }
	    break;
	  case __GL_FLOAT64:
	    for (i=0; i < size; i++) {
		((GLdouble *)result)[i] = ((const __GLfloat *)rawdata)[i];
	    }
	    break;
	  case __GL_INT32:
	    for (i=0; i < size; i++) {
		((GLint *)result)[i] =
			(GLint)(((const __GLfloat *)rawdata)[i] >= 0.0 ?
			((const __GLfloat *)rawdata)[i] + __glHalf:
			((const __GLfloat *)rawdata)[i] - __glHalf);
	    }
	    break;
	  case __GL_BOOLEAN:
	    for (i=0; i < size; i++) {
		((GLboolean *)result)[i] =
		    ((const __GLfloat *)rawdata)[i] ? 1 : 0;
	    }
	    break;
	}
	break;
      case __GL_COLOR:
	switch (toType) {
	  case __GL_FLOAT32:
	    for (i=0; i < size; i++) {
		((GLfloat *)result)[i] = ((const __GLfloat *)rawdata)[i];
	    }
	    break;
	  case __GL_FLOAT64:
	    for (i=0; i < size; i++) {
		((GLdouble *)result)[i] = ((const __GLfloat *)rawdata)[i];
	    }
	    break;
	  case __GL_INT32:
	    for (i=0; i < size; i++) {
		((GLint *)result)[i] =
		    __GL_FLOAT_TO_I(((const __GLfloat *)rawdata)[i]);
	    }
	    break;
	  case __GL_BOOLEAN:
	    for (i=0; i < size; i++) {
		((GLboolean *)result)[i] =
		    ((const __GLfloat *)rawdata)[i] ? 1 : 0;
	    }
	    break;
	}
	break;
      case __GL_SCOLOR:
	switch (toType) {
	  case __GL_FLOAT32:
	    ((GLfloat *)result)[0] =
		((const __GLfloat *)rawdata)[0] * gc->constants.oneOverRedScale;
	    ((GLfloat *)result)[1] =
		((const __GLfloat *)rawdata)[1] * gc->constants.oneOverGreenScale;
	    ((GLfloat *)result)[2] =
		((const __GLfloat *)rawdata)[2] * gc->constants.oneOverBlueScale;
	    ((GLfloat *)result)[3] =
		((const __GLfloat *)rawdata)[3] * gc->constants.oneOverAlphaScale;
	    break;
	  case __GL_FLOAT64:
	    ((GLdouble *)result)[0] =
		((const __GLfloat *)rawdata)[0] * gc->constants.oneOverRedScale;
	    ((GLdouble *)result)[1] =
		((const __GLfloat *)rawdata)[1] * gc->constants.oneOverGreenScale;
	    ((GLdouble *)result)[2] =
		((const __GLfloat *)rawdata)[2] * gc->constants.oneOverBlueScale;
	    ((GLdouble *)result)[3] =
		((const __GLfloat *)rawdata)[3] * gc->constants.oneOverAlphaScale;
	    break;
	  case __GL_INT32:
	    ((GLint *)result)[0] =
		__GL_FLOAT_TO_I(((const __GLfloat *)rawdata)[0] *
				gc->constants.oneOverRedScale);
	    ((GLint *)result)[1] =
		__GL_FLOAT_TO_I(((const __GLfloat *)rawdata)[1] *
				gc->constants.oneOverGreenScale);
	    ((GLint *)result)[2] =
		__GL_FLOAT_TO_I(((const __GLfloat *)rawdata)[2] *
				gc->constants.oneOverBlueScale);
	    ((GLint *)result)[3] =
		__GL_FLOAT_TO_I(((const __GLfloat *)rawdata)[3] *
				gc->constants.oneOverAlphaScale);
	    break;
	  case __GL_BOOLEAN:
	    for (i=0; i < size; i++) {
		((GLboolean *)result)[i] =
		    ((const __GLfloat *)rawdata)[i] ? 1 : 0;
	    }
	    break;
	}
	break;
      case __GL_INT32:
	switch (toType) {
	  case __GL_FLOAT32:
	    for (i=0; i < size; i++) {
		((GLfloat *)result)[i] = ((const GLint *)rawdata)[i];
	    }
	    break;
	  case __GL_FLOAT64:
	    for (i=0; i < size; i++) {
		((GLdouble *)result)[i] = ((const GLint *)rawdata)[i];
	    }
	    break;
	  case __GL_INT32:
	    for (i=0; i < size; i++) {
		((GLint *)result)[i] = ((const GLint *)rawdata)[i];
	    }
	    break;
	  case __GL_BOOLEAN:
	    for (i=0; i < size; i++) {
		((GLboolean *)result)[i] = ((const GLint *)rawdata)[i] ? 1 : 0;
	    }
	    break;
	}
	break;
      case __GL_BOOLEAN:
	switch (toType) {
	  case __GL_FLOAT32:
	    for (i=0; i < size; i++) {
		((GLfloat *)result)[i] = ((const GLboolean *)rawdata)[i];
	    }
	    break;
	  case __GL_FLOAT64:
	    for (i=0; i < size; i++) {
		((GLdouble *)result)[i] = ((const GLboolean *)rawdata)[i];
	    }
	    break;
	  case __GL_INT32:
	    for (i=0; i < size; i++) {
		((GLint *)result)[i] = ((const GLboolean *)rawdata)[i];
	    }
	    break;
	  case __GL_BOOLEAN:
	    for (i=0; i < size; i++) {
		((GLboolean *)result)[i] =
		    ((const GLboolean *)rawdata)[i] ? 1 : 0;
	    }
	    break;
	}
	break;
    }
}

/*
** Fetch the data for a query in its internal type, then convert it to the
** type that the user asked for.
*/
static void DoGet(GLenum sq, void *result, GLint type, const char *procName)
{
    GLint index;
    __GLfloat ftemp[100], *fp = ftemp;		/* NOTE: for floats */
    __GLfloat ctemp[100], *cp = ctemp;		/* NOTE: for colors */
    __GLfloat sctemp[100], *scp = sctemp;	/* NOTE: for scaled colors */
    GLint itemp[100], *ip = itemp;		/* NOTE: for ints */
    GLboolean ltemp[100], *lp = ltemp;		/* NOTE: for logicals */
    __GLfloat *mp;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

#ifdef __GL_LINT
    procName = procName;
#endif
    switch (sq) {
      case GL_ALPHA_TEST:
      case GL_BLEND:
      case GL_COLOR_MATERIAL:
      case GL_CULL_FACE:
#ifdef XXX_IS_CULLVERTEX_SUPPORTED
      case GL_CULL_VERTEX_EXT:
#endif
      case GL_DEPTH_TEST:
      case GL_DITHER:
      case GL_FOG:
      case GL_LIGHTING:
      case GL_LINE_SMOOTH:
      case GL_LINE_STIPPLE:
      case GL_INDEX_LOGIC_OP:
      case GL_COLOR_LOGIC_OP:
      case GL_NORMALIZE:
      case GL_POINT_SMOOTH:
      case GL_POLYGON_SMOOTH:
      case GL_POLYGON_STIPPLE:
      case GL_RESCALE_NORMAL:
      case GL_SCISSOR_TEST:
      case GL_STENCIL_TEST:
      case GL_TEXTURE_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_3D:
      case GL_AUTO_NORMAL:
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_GEN_R:
      case GL_TEXTURE_GEN_Q:
      case GL_CLIP_PLANE0: case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2: case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4: case GL_CLIP_PLANE5:
      case GL_LIGHT0: case GL_LIGHT1:
      case GL_LIGHT2: case GL_LIGHT3:
      case GL_LIGHT4: case GL_LIGHT5:
      case GL_LIGHT6: case GL_LIGHT7:
      case GL_MAP1_COLOR_4:
      case GL_MAP1_NORMAL:
      case GL_MAP1_INDEX:
      case GL_MAP1_TEXTURE_COORD_1: case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3: case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3: case GL_MAP1_VERTEX_4:
      case GL_MAP2_COLOR_4:
      case GL_MAP2_NORMAL:
      case GL_MAP2_INDEX:
      case GL_MAP2_TEXTURE_COORD_1: case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3: case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3: case GL_MAP2_VERTEX_4:
      case GL_POLYGON_OFFSET_POINT:
      case GL_POLYGON_OFFSET_LINE:
      case GL_POLYGON_OFFSET_FILL:
      case GL_VERTEX_ARRAY:
      case GL_NORMAL_ARRAY:
      case GL_COLOR_ARRAY:
      case GL_INDEX_ARRAY:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_EDGE_FLAG_ARRAY:
      case GL_COLOR_TABLE:
      case GL_CONVOLUTION_1D:
      case GL_CONVOLUTION_2D:
      case GL_SEPARABLE_2D:
      case GL_POST_CONVOLUTION_COLOR_TABLE:
      case GL_POST_COLOR_MATRIX_COLOR_TABLE:
      case GL_HISTOGRAM:
      case GL_MINMAX:
	*lp++ = __glim_IsEnabled(sq);
	break;
      case GL_MAX_TEXTURE_SIZE:
	*ip++ = gc->constants.maxTextureSize;
	break;
      case GL_MAX_3D_TEXTURE_SIZE:
	*ip++ = gc->constants.maxTexture3DSize;
	break;
      case GL_MAX_TEXTURE_UNITS_ARB:
	*ip++ = gc->constants.maxTextureUnits;
	break;
      case GL_ACTIVE_TEXTURE_ARB:
	*ip++ = gc->state.texture.activeTexture + GL_TEXTURE0_ARB;
	break;
      case GL_CLIENT_ACTIVE_TEXTURE_ARB:
	*ip++ = gc->clientTexture.activeTexture + GL_TEXTURE0_ARB;
	break;
      case GL_SUBPIXEL_BITS:
	*ip++ = gc->constants.subpixelBits;
	break;
      case GL_MAX_LIST_NESTING:
	*ip++ = __GL_MAX_LIST_NESTING;
	break;
      case GL_CURRENT_COLOR:
	*cp++ = gc->state.current.userColor.r;
	*cp++ = gc->state.current.userColor.g;
	*cp++ = gc->state.current.userColor.b;
	*cp++ = gc->state.current.userColor.a;
	break;
      case GL_CURRENT_INDEX:
	*fp++ = gc->state.current.userColorIndex;
	break;
      case GL_CURRENT_NORMAL:
	*cp++ = gc->state.current.normal.x;
	*cp++ = gc->state.current.normal.y;
	*cp++ = gc->state.current.normal.z;
	break;
      case GL_CURRENT_TEXTURE_COORDS:
	*fp++ = gc->state.current.texture[gc->state.texture.activeTexture].x;
	*fp++ = gc->state.current.texture[gc->state.texture.activeTexture].y;
	*fp++ = gc->state.current.texture[gc->state.texture.activeTexture].z;
	*fp++ = gc->state.current.texture[gc->state.texture.activeTexture].w;
	break;
      case GL_CURRENT_RASTER_INDEX:
	if (gc->modes.rgbMode) {
	    /* Always return 1 */
	    *fp++ = 1.0;
	} else {
	    *fp++ = gc->state.current.rasterPos.colors[__GL_FRONTFACE].r;
	}
	break;
      case GL_CURRENT_RASTER_COLOR:
	if (gc->modes.colorIndexMode) {
	    /* Always return 1,1,1,1 */
	    *fp++ = 1.0;
	    *fp++ = 1.0;
	    *fp++ = 1.0;
	    *fp++ = 1.0;
	} else {
	    *scp++ = gc->state.current.rasterPos.colors[__GL_FRONTFACE].r;
	    *scp++ = gc->state.current.rasterPos.colors[__GL_FRONTFACE].g;
	    *scp++ = gc->state.current.rasterPos.colors[__GL_FRONTFACE].b;
	    *scp++ = gc->state.current.rasterPos.colors[__GL_FRONTFACE].a;
	}
	break;
      case GL_CURRENT_RASTER_TEXTURE_COORDS:
	*fp++ = gc->state.current.rasterPos.texture[gc->state.texture.activeTexture].x;
	*fp++ = gc->state.current.rasterPos.texture[gc->state.texture.activeTexture].y;
	*fp++ = gc->state.current.rasterPos.texture[gc->state.texture.activeTexture].z;
	*fp++ = gc->state.current.rasterPos.texture[gc->state.texture.activeTexture].w;
	break;
      case GL_CURRENT_RASTER_POSITION:
	*fp++ = __glReturnWindowX(gc, gc->state.current.rasterPos.window.x);
	*fp++ = __glReturnWindowY(gc, gc->state.current.rasterPos.window.y);
	*fp++ = gc->state.current.rasterPos.window.z
					* gc->constants.oneOverDepthScale;
	*fp++ = gc->state.current.rasterPos.clip.w;
	break;
      case GL_CURRENT_RASTER_POSITION_VALID:
	*lp++ = gc->state.current.validRasterPos;
	break;
      case GL_CURRENT_RASTER_DISTANCE:
	*fp++ = gc->state.current.rasterPos.eye.z;
	break;
      case GL_POINT_SIZE:
	*fp++ = gc->state.point.requestedSize;
	break;
      case GL_POINT_SIZE_RANGE:
      /* case GL_SMOOTH_POINT_SIZE_RANGE: */ /* Alias for POINT_SIZE_RANGE */
	*fp++ = gc->constants.pointSizeMinimum;
	*fp++ = gc->constants.pointSizeMaximum;
	break;
      case GL_POINT_SIZE_GRANULARITY:
      /* case GL_SMOOTH_POINT_SIZE_GRANULARITY: */ /* Alias for POINT_SIZE_GRANULARITY */
	*fp++ = gc->constants.pointSizeGranularity;
	break;
      case GL_ALIASED_POINT_SIZE_RANGE:
	*fp++ = gc->constants.aliasedPointSizeMinimum;
	*fp++ = gc->constants.aliasedPointSizeMaximum;
	break;
      case GL_LINE_WIDTH:
	*fp++ = gc->state.line.requestedWidth;
	break;
      case GL_LINE_WIDTH_RANGE:
      /* case GL_SMOOTH_LINE_WIDTH_RANGE: */ /* Alias for LINE_WIDTH_RANGE */
	*fp++ = gc->constants.lineWidthMinimum;
	*fp++ = gc->constants.lineWidthMaximum;
	break;
      case GL_LINE_WIDTH_GRANULARITY:
      /* case GL_SMOOTH_LINE_WIDTH_GRANULARITY: */ /* Alias for LINE_WIDTH_GRANULARITY */
	*fp++ = gc->constants.lineWidthGranularity;
	break;
      case GL_ALIASED_LINE_WIDTH_RANGE:
	*fp++ = gc->constants.aliasedLineWidthMinimum;
	*fp++ = gc->constants.aliasedLineWidthMaximum;
	break;
      case GL_LINE_STIPPLE_PATTERN:
	*ip++ = gc->state.line.stipple;
	break;
      case GL_LINE_STIPPLE_REPEAT:
	*ip++ = gc->state.line.stippleRepeat;
	break;
      case GL_POLYGON_MODE:
	*ip++ = gc->state.polygon.frontMode;
	*ip++ = gc->state.polygon.backMode;
	break;
      case GL_EDGE_FLAG:
	*lp++ = (gc->state.current.edgeTag & __GL_VERTEX_EDGE_FLAG) ? GL_TRUE
								    : GL_FALSE;
	break;
      case GL_CULL_FACE_MODE:
	*ip++ = gc->state.polygon.cull;
	break;
      case GL_FRONT_FACE:
	*ip++ = gc->state.polygon.frontFaceDirection;
	break;
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
	*lp++ = gc->state.light.model.localViewer;
	break;
      case GL_LIGHT_MODEL_TWO_SIDE:
	*lp++ = gc->state.light.model.twoSided;
	break;
      case GL_LIGHT_MODEL_AMBIENT:
	__glUnScaleColorf(gc, cp, &gc->state.light.model.ambient);
	cp += 4;
	break;
      case GL_LIGHT_MODEL_COLOR_CONTROL:
	*ip++ = gc->state.light.model.colorControl;
	break;
      case GL_COLOR_MATERIAL_FACE:
	*ip++ = gc->state.light.colorMaterialFace;
	break;
      case GL_COLOR_MATERIAL_PARAMETER:
	*ip++ = gc->state.light.colorMaterialParam;
	break;
      case GL_SHADE_MODEL:
	*ip++ = gc->state.light.shadingModel;
	break;
      case GL_FOG_INDEX:
	*fp++ = gc->state.fog.index;
	break;
      case GL_FOG_DENSITY:
	*fp++ = gc->state.fog.density;
	break;
      case GL_FOG_START:
	*fp++ = gc->state.fog.start;
	break;
      case GL_FOG_END:
	*fp++ = gc->state.fog.end;
	break;
      case GL_FOG_MODE:
	*ip++ = gc->state.fog.mode;
	break;
      case GL_FOG_COLOR:
	*scp++ = gc->state.fog.color.r;
	*scp++ = gc->state.fog.color.g;
	*scp++ = gc->state.fog.color.b;
	*scp++ = gc->state.fog.color.a;
	break;
      case GL_DEPTH_RANGE:
	/* These get scaled like colors, to [0, 2^31-1] */
	*cp++ = gc->state.viewport.zNear;
	*cp++ = gc->state.viewport.zFar;
	break;
      case GL_DEPTH_WRITEMASK:
	*lp++ = gc->state.depth.writeEnable;
	break;
      case GL_DEPTH_CLEAR_VALUE:
	/* This gets scaled like colors, to [0, 2^31-1] */
	*cp++ = gc->state.depth.clear;
	break;
      case GL_DEPTH_FUNC:
	*ip++ = gc->state.depth.testFunc;
	break;
      case GL_ACCUM_CLEAR_VALUE:
	*cp++ = gc->state.accum.clear.r;
	*cp++ = gc->state.accum.clear.g;
	*cp++ = gc->state.accum.clear.b;
	*cp++ = gc->state.accum.clear.a;
	break;
      case GL_STENCIL_CLEAR_VALUE:
	*ip++ = gc->state.stencil.clear;
	break;
      case GL_STENCIL_FUNC:
	*ip++ = gc->state.stencil.testFunc;
	break;
      case GL_STENCIL_VALUE_MASK:
	*ip++ = gc->state.stencil.mask;
	break;
      case GL_STENCIL_FAIL:
	*ip++ = gc->state.stencil.fail;
	break;
      case GL_STENCIL_PASS_DEPTH_FAIL:
	*ip++ = gc->state.stencil.depthFail;
	break;
      case GL_STENCIL_PASS_DEPTH_PASS:
	*ip++ = gc->state.stencil.depthPass;
	break;
      case GL_STENCIL_REF:
	*ip++ = gc->state.stencil.reference;
	break;
      case GL_STENCIL_WRITEMASK:
	*ip++ = gc->state.stencil.writeMask;
	break;
      case GL_MATRIX_MODE:
	*ip++ = gc->state.transform.matrixMode;
	break;
      case GL_VIEWPORT:
	*ip++ = gc->state.viewport.x;
	*ip++ = gc->state.viewport.y;
	*ip++ = gc->state.viewport.width;
	*ip++ = gc->state.viewport.height;
	break;
      case GL_ATTRIB_STACK_DEPTH:
	*ip++ = gc->attributes.stackPointer - gc->attributes.stack;
	break;
      case GL_CLIENT_ATTRIB_STACK_DEPTH:
	*ip++ = gc->attributes.clientStackPointer - gc->attributes.clientStack;
	break;
      case GL_MODELVIEW_STACK_DEPTH:
	*ip++ = 1 + (gc->transform.modelView - gc->transform.modelViewStack);
	break;
      case GL_PROJECTION_STACK_DEPTH:
	*ip++ = 1 + (gc->transform.projection - gc->transform.projectionStack);
	break;
      case GL_TEXTURE_STACK_DEPTH:
	*ip++ = 1 + (gc->transform.texture[gc->state.texture.activeTexture] -
		gc->transform.textureStack[gc->state.texture.activeTexture]);
	break;
      case GL_COLOR_MATRIX_STACK_DEPTH:
	*ip++ = 1 + (gc->transform.color - gc->transform.colorStack);
	break;
      case GL_MODELVIEW_MATRIX:
	mp = &gc->transform.modelView->matrix.matrix[0][0];
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	break;
      case GL_PROJECTION_MATRIX:
	mp = &gc->transform.projection->matrix.matrix[0][0];
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	break;
      case GL_TEXTURE_MATRIX:
	mp = &gc->transform.texture[gc->state.texture.activeTexture]->matrix.matrix[0][0];
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	break;
      case GL_COLOR_MATRIX:
	mp = &gc->transform.color->matrix.matrix[0][0];
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	*fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++; *fp++ = *mp++;
	break;
      case GL_ALPHA_TEST_FUNC:
	*ip++ = gc->state.raster.alphaFunction;
	break;
      case GL_ALPHA_TEST_REF:
	*fp++ = gc->state.raster.alphaReference;
	break;
      case GL_BLEND_DST:
	*ip++ = gc->state.raster.blendDst;
	break;
      case GL_BLEND_SRC:
	*ip++ = gc->state.raster.blendSrc;
	break;
      case GL_BLEND_COLOR:
	*fp++ = gc->state.raster.blendColor.r * gc->constants.oneOverRedScale;
	*fp++ = gc->state.raster.blendColor.g * gc->constants.oneOverGreenScale;
	*fp++ = gc->state.raster.blendColor.b * gc->constants.oneOverBlueScale;
	*fp++ = gc->state.raster.blendColor.a * gc->constants.oneOverAlphaScale;
	break;
      case GL_BLEND_EQUATION:
	*ip++ = gc->state.raster.blendMode;
	break;
      case GL_LOGIC_OP_MODE:
	*ip++ = gc->state.raster.logicOp;
	break;
      case GL_DRAW_BUFFER:
	*ip++ = gc->state.raster.drawBufferReturn;
	break;
      case GL_READ_BUFFER:
	*ip++ = gc->state.pixel.readBufferReturn;
	break;
      case GL_SCISSOR_BOX:
	*ip++ = gc->state.scissor.scissorX;
	*ip++ = gc->state.scissor.scissorY;
	*ip++ = gc->state.scissor.scissorWidth;
	*ip++ = gc->state.scissor.scissorHeight;
	break;
      case GL_INDEX_CLEAR_VALUE:
	*fp++ = gc->state.raster.clearIndex;
	break;
      case GL_INDEX_MODE:
	*lp++ = gc->modes.colorIndexMode ? GL_TRUE : GL_FALSE;
	break;
      case GL_INDEX_WRITEMASK:
	*ip++ = gc->state.raster.writeMask;
	break;
      case GL_COLOR_CLEAR_VALUE:
	*cp++ = gc->state.raster.clear.r;
	*cp++ = gc->state.raster.clear.g;
	*cp++ = gc->state.raster.clear.b;
	*cp++ = gc->state.raster.clear.a;
	break;
      case GL_RGBA_MODE:
	*lp++ = gc->modes.rgbMode ? GL_TRUE : GL_FALSE;
	break;
      case GL_COLOR_WRITEMASK:
	*lp++ = gc->state.raster.rMask;
	*lp++ = gc->state.raster.gMask;
	*lp++ = gc->state.raster.bMask;
	*lp++ = gc->state.raster.aMask;
	break;
      case GL_RENDER_MODE:
	*ip++ = gc->renderMode;
	break;
      case GL_PERSPECTIVE_CORRECTION_HINT:
	*ip++ = gc->state.hints.perspectiveCorrection;
	break;
      case GL_POINT_SMOOTH_HINT:
	*ip++ = gc->state.hints.pointSmooth;
	break;
      case GL_LINE_SMOOTH_HINT:
	*ip++ = gc->state.hints.lineSmooth;
	break;
      case GL_POLYGON_SMOOTH_HINT:
	*ip++ = gc->state.hints.polygonSmooth;
	break;
      case GL_FOG_HINT:
	*ip++ = gc->state.hints.fog;
	break;
      case GL_LIST_BASE:
	*ip++ = gc->state.list.listBase;
	break;
      case GL_LIST_INDEX:
	*ip++ = gc->dlist.currentList;
	break;
      case GL_LIST_MODE:
	*ip++ = gc->dlist.mode;
	break;
      case GL_PACK_SWAP_BYTES:
	*lp++ = gc->clientPixel.packModes.swapEndian;
	break;
      case GL_PACK_LSB_FIRST:
	*lp++ = gc->clientPixel.packModes.lsbFirst;
	break;
      case GL_PACK_ROW_LENGTH:
	*ip++ = gc->clientPixel.packModes.lineLength;
	break;
      case GL_PACK_IMAGE_HEIGHT:
	*ip++ = gc->clientPixel.packModes.imageHeight;
	break;
      case GL_PACK_SKIP_ROWS:
	*ip++ = gc->clientPixel.packModes.skipLines;
	break;
      case GL_PACK_SKIP_PIXELS:
	*ip++ = gc->clientPixel.packModes.skipPixels;
	break;
      case GL_PACK_SKIP_IMAGES:
	*ip++ = gc->clientPixel.packModes.skipImages;
	break;
      case GL_PACK_ALIGNMENT:
	*ip++ = gc->clientPixel.packModes.alignment;
	break;
      case GL_UNPACK_SWAP_BYTES:
	*lp++ = gc->clientPixel.unpackModes.swapEndian;
	break;
      case GL_UNPACK_LSB_FIRST:
	*lp++ = gc->clientPixel.unpackModes.lsbFirst;
	break;
      case GL_UNPACK_ROW_LENGTH:
	*ip++ = gc->clientPixel.unpackModes.lineLength;
	break;
      case GL_UNPACK_IMAGE_HEIGHT:
	*ip++ = gc->clientPixel.unpackModes.imageHeight;
	break;
      case GL_UNPACK_SKIP_ROWS:
	*ip++ = gc->clientPixel.unpackModes.skipLines;
	break;
      case GL_UNPACK_SKIP_PIXELS:
	*ip++ = gc->clientPixel.unpackModes.skipPixels;
	break;
      case GL_UNPACK_SKIP_IMAGES:
	*ip++ = gc->clientPixel.unpackModes.skipImages;
	break;
      case GL_UNPACK_ALIGNMENT:
	*ip++ = gc->clientPixel.unpackModes.alignment;
	break;
      case GL_MAP_COLOR:
	*lp++ = gc->state.pixel.transferMode.mapColor;
	break;
      case GL_MAP_STENCIL:
	*lp++ = gc->state.pixel.transferMode.mapStencil;
	break;
      case GL_INDEX_SHIFT:
	*ip++ = gc->state.pixel.transferMode.indexShift;
	break;
      case GL_INDEX_OFFSET:
	*ip++ = gc->state.pixel.transferMode.indexOffset;
	break;
      case GL_RED_SCALE:
	*fp++ = gc->state.pixel.transferMode.scale.r;
	break;
      case GL_GREEN_SCALE:
	*fp++ = gc->state.pixel.transferMode.scale.g;
	break;
      case GL_BLUE_SCALE:
	*fp++ = gc->state.pixel.transferMode.scale.b;
	break;
      case GL_ALPHA_SCALE:
	*fp++ = gc->state.pixel.transferMode.scale.a;
	break;
      case GL_RED_BIAS:
	*fp++ = gc->state.pixel.transferMode.bias.r;
	break;
      case GL_GREEN_BIAS:
	*fp++ = gc->state.pixel.transferMode.bias.g;
	break;
      case GL_BLUE_BIAS:
	*fp++ = gc->state.pixel.transferMode.bias.b;
	break;
      case GL_ALPHA_BIAS:
	*fp++ = gc->state.pixel.transferMode.bias.a;
	break;
      case GL_DEPTH_SCALE:
	*fp++ = gc->state.pixel.transferMode.depthScale;
	break;
      case GL_DEPTH_BIAS:
	*fp++ = gc->state.pixel.transferMode.depthBias;
	break;
      case GL_POST_CONVOLUTION_RED_SCALE:
	*fp++ = gc->state.pixel.transferMode.postConvolutionScale.r;
	break;
      case GL_POST_CONVOLUTION_GREEN_SCALE:
	*fp++ = gc->state.pixel.transferMode.postConvolutionScale.g;
	break;
      case GL_POST_CONVOLUTION_BLUE_SCALE:
	*fp++ = gc->state.pixel.transferMode.postConvolutionScale.b;
	break;
      case GL_POST_CONVOLUTION_ALPHA_SCALE:
	*fp++ = gc->state.pixel.transferMode.postConvolutionScale.a;
	break;
      case GL_POST_CONVOLUTION_RED_BIAS:
	*fp++ = gc->state.pixel.transferMode.postConvolutionBias.r;
	break;
      case GL_POST_CONVOLUTION_GREEN_BIAS:
	*fp++ = gc->state.pixel.transferMode.postConvolutionBias.g;
	break;
      case GL_POST_CONVOLUTION_BLUE_BIAS:
	*fp++ = gc->state.pixel.transferMode.postConvolutionBias.b;
	break;
      case GL_POST_CONVOLUTION_ALPHA_BIAS:
	*fp++ = gc->state.pixel.transferMode.postConvolutionBias.a;
	break;
      case GL_POST_COLOR_MATRIX_RED_SCALE:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixScale.r;
	break;
      case GL_POST_COLOR_MATRIX_GREEN_SCALE:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixScale.g;
	break;
      case GL_POST_COLOR_MATRIX_BLUE_SCALE:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixScale.b;
	break;
      case GL_POST_COLOR_MATRIX_ALPHA_SCALE:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixScale.a;
	break;
      case GL_POST_COLOR_MATRIX_RED_BIAS:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixBias.r;
	break;
      case GL_POST_COLOR_MATRIX_GREEN_BIAS:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixBias.g;
	break;
      case GL_POST_COLOR_MATRIX_BLUE_BIAS:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixBias.b;
	break;
      case GL_POST_COLOR_MATRIX_ALPHA_BIAS:
	*fp++ = gc->state.pixel.transferMode.postColorMatrixBias.a;
	break;
      case GL_ZOOM_X:
	*fp++ = gc->state.pixel.transferMode.zoomX;
	break;
      case GL_ZOOM_Y:
	*fp++ = gc->state.pixel.transferMode.zoomY;
	break;
      case GL_PIXEL_MAP_I_TO_I_SIZE:	  case GL_PIXEL_MAP_S_TO_S_SIZE:
      case GL_PIXEL_MAP_I_TO_R_SIZE:	  case GL_PIXEL_MAP_I_TO_G_SIZE:
      case GL_PIXEL_MAP_I_TO_B_SIZE:	  case GL_PIXEL_MAP_I_TO_A_SIZE:
      case GL_PIXEL_MAP_R_TO_R_SIZE:	  case GL_PIXEL_MAP_G_TO_G_SIZE:
      case GL_PIXEL_MAP_B_TO_B_SIZE:	  case GL_PIXEL_MAP_A_TO_A_SIZE:
	index = sq - GL_PIXEL_MAP_I_TO_I_SIZE;
	*ip++ = gc->pixel.pixelMap[index].size;
	break;
      case GL_MAX_EVAL_ORDER:
	*ip++ = gc->constants.maxEvalOrder;
	break;
      case GL_MAX_LIGHTS:
	*ip++ = gc->constants.numberOfLights;
	break;
      case GL_MAX_CLIP_PLANES:
	*ip++ = gc->constants.numberOfClipPlanes;
	break;
      case GL_MAX_PIXEL_MAP_TABLE:
	*ip++ = gc->constants.maxPixelMapTable;
	break;
      case GL_MAX_ATTRIB_STACK_DEPTH:
	*ip++ = gc->constants.maxAttribStackDepth;
	break;
      case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
	*ip++ = gc->constants.maxClientAttribStackDepth;
	break;
      case GL_MAX_MODELVIEW_STACK_DEPTH:
	*ip++ = gc->constants.maxModelViewStackDepth;
	break;
      case GL_MAX_NAME_STACK_DEPTH:
	*ip++ = gc->constants.maxNameStackDepth;
	break;
      case GL_MAX_PROJECTION_STACK_DEPTH:
	*ip++ = gc->constants.maxProjectionStackDepth;
	break;
      case GL_MAX_TEXTURE_STACK_DEPTH:
	*ip++ = gc->constants.maxTextureStackDepth;
	break;
      case GL_MAX_COLOR_MATRIX_STACK_DEPTH:
	*ip++ = gc->constants.maxColorStackDepth;
	break;
      case GL_INDEX_BITS:
	*ip++ = gc->modes.indexBits;
	break;
      case GL_RED_BITS:
	*ip++ = gc->modes.redBits;
	break;
      case GL_GREEN_BITS:
	*ip++ = gc->modes.greenBits;
	break;
      case GL_BLUE_BITS:
	*ip++ = gc->modes.blueBits;
	break;
      case GL_ALPHA_BITS:
	*ip++ = gc->modes.alphaBits;
	break;
      case GL_DEPTH_BITS:
	*ip++ = gc->modes.depthBits;
	break;
      case GL_STENCIL_BITS:
	*ip++ = gc->modes.stencilBits;
	break;
      case GL_ACCUM_RED_BITS:
	*ip++ = gc->modes.accumRedBits;
	break;
      case GL_ACCUM_GREEN_BITS:
	*ip++ = gc->modes.accumGreenBits;
	break;
      case GL_ACCUM_BLUE_BITS:
	*ip++ = gc->modes.accumBlueBits;
	break;
      case GL_ACCUM_ALPHA_BITS:
	*ip++ = gc->modes.accumAlphaBits;
	break;
      case GL_MAP1_GRID_DOMAIN:
	*fp++ = gc->state.evaluator.u1.start;
	*fp++ = gc->state.evaluator.u1.finish;
	break;
      case GL_MAP1_GRID_SEGMENTS:
	*ip++ = gc->state.evaluator.u1.n;
	break;
      case GL_MAP2_GRID_DOMAIN:
	*fp++ = gc->state.evaluator.u2.start;
	*fp++ = gc->state.evaluator.u2.finish;
	*fp++ = gc->state.evaluator.v2.start;
	*fp++ = gc->state.evaluator.v2.finish;
	break;
      case GL_MAP2_GRID_SEGMENTS:
	*ip++ = gc->state.evaluator.u2.n;
	*ip++ = gc->state.evaluator.v2.n;
	break;
      case GL_NAME_STACK_DEPTH:
	*ip++ = gc->select.sp - gc->select.stack;
	break;
      case GL_MAX_VIEWPORT_DIMS:
	*ip++ = gc->constants.maxViewportWidth;
	*ip++ = gc->constants.maxViewportHeight;
	break;
      case GL_DOUBLEBUFFER:
	*lp++ = gc->modes.doubleBufferMode ? GL_TRUE : GL_FALSE;
	break;
      case GL_AUX_BUFFERS:
	*ip++ = gc->modes.numAuxBuffers;
	break;
      case GL_STEREO:
	*lp++ = GL_FALSE;
	break;
      case GL_POLYGON_OFFSET_FACTOR:
	*fp++ = gc->state.polygon.factor;
	break;
      case GL_POLYGON_OFFSET_UNITS:
	*fp++ = gc->state.polygon.units;
	break;
      case GL_TEXTURE_BINDING_1D:
	{
	    __GLtexture *tex =
			__glLookupActiveTexture(gc, GL_TEXTURE_1D);
	    *ip++ = tex->state.name;
	}
	break;
      case GL_TEXTURE_BINDING_2D:
	{
	    __GLtexture *tex =
			__glLookupActiveTexture(gc, GL_TEXTURE_2D);
	    *ip++ = tex->state.name;
	}
	break;
      case GL_TEXTURE_BINDING_3D:
	{
	    __GLtexture *tex =
			__glLookupActiveTexture(gc, GL_TEXTURE_3D);
	    *ip++ = tex->state.name;
	}
	break;
      case GL_VERTEX_ARRAY_SIZE:
	*ip++ = gc->vertexArray.vp.size;
	break;
      case GL_VERTEX_ARRAY_TYPE:
	*ip++ = gc->vertexArray.vp.type;
	break;
      case GL_VERTEX_ARRAY_STRIDE:
	*ip++ = gc->vertexArray.vp.userStride;
	break;
#if 0
      case GL_VERTEX_ARRAY_COUNT_EXT:
	*ip++ = gc->vertexArray.vp.count;
	break;
#endif
      case GL_NORMAL_ARRAY_TYPE:
	*ip++ = gc->vertexArray.np.type;
	break;
      case GL_NORMAL_ARRAY_STRIDE:
	*ip++ = gc->vertexArray.np.userStride;
	break;
#if 0
      case GL_NORMAL_ARRAY_COUNT_EXT:
	*ip++ = gc->vertexArray.np.count;
	break;
#endif
      case GL_COLOR_ARRAY_SIZE:
	*ip++ = gc->vertexArray.cp.size;
	break;
      case GL_COLOR_ARRAY_TYPE:
	*ip++ = gc->vertexArray.cp.type;
	break;
      case GL_COLOR_ARRAY_STRIDE:
	*ip++ = gc->vertexArray.cp.userStride;
	break;
#if 0
      case GL_COLOR_ARRAY_COUNT_EXT:
	*ip++ = gc->vertexArray.cp.count;
	break;
#endif
      case GL_INDEX_ARRAY_TYPE:
	*ip++ = gc->vertexArray.ip.type;
	break;
      case GL_INDEX_ARRAY_STRIDE:
	*ip++ = gc->vertexArray.ip.userStride;
	break;
#if 0
      case GL_INDEX_ARRAY_COUNT_EXT:
	*ip++ = gc->vertexArray.ip.count;
	break;
#endif
      case GL_TEXTURE_COORD_ARRAY_SIZE:
	*ip++ = gc->vertexArray.tp[gc->clientTexture.activeTexture].size;
	break;
      case GL_TEXTURE_COORD_ARRAY_TYPE:
	*ip++ = gc->vertexArray.tp[gc->clientTexture.activeTexture].type;
	break;
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
	*ip++ = gc->vertexArray.tp[gc->clientTexture.activeTexture].userStride;
	break;
#if 0
      case GL_TEXTURE_COORD_ARRAY_COUNT_EXT:
	*ip++ = gc->vertexArray.tp[gc->clientTexture.activeTexture].count;
	break;
#endif
      case GL_EDGE_FLAG_ARRAY_STRIDE:
	*ip++ = gc->vertexArray.ep.userStride;
	break;
#if 0
      case GL_EDGE_FLAG_ARRAY_COUNT_EXT:
	*ip++ = gc->vertexArray.ep.count;
	break;
#endif
      case GL_MAX_ELEMENTS_VERTICES:
	*ip++ = gc->vertexArray.maxElementsVertices;
	break;
      case GL_MAX_ELEMENTS_INDICES:
	*ip++ = gc->vertexArray.maxElementsIndices;
	break;
      case GL_FEEDBACK_BUFFER_SIZE:
	*ip++ = gc->feedback.resultLength;
	break;
      case GL_FEEDBACK_BUFFER_TYPE:
	*ip++ = gc->feedback.type;
	break;
      case GL_SELECTION_BUFFER_SIZE:
	*ip++ = gc->select.resultLength;
	break;
#ifdef XXX_IS_CULLVERTEX_SUPPORTED
      case GL_ARRAY_ELEMENT_LOCK_FIRST_EXT:
	*ip++ = gc->vertexArray.start;
	break;
      case GL_ARRAY_ELEMENT_LOCK_COUNT_EXT:
	*ip++ = gc->vertexArray.count;
	break;
      case GL_CULL_VERTEX_EYE_POSITION_EXT:
	*fp++ = gc->state.transform.eyePos.x;
	*fp++ = gc->state.transform.eyePos.y;
	*fp++ = gc->state.transform.eyePos.z;
	*fp++ = gc->state.transform.eyePos.w;
	break;
      case GL_CULL_VERTEX_OBJECT_POSITION_EXT:
	*fp++ = gc->state.transform.eyePosObj.x;
	*fp++ = gc->state.transform.eyePosObj.y;
	*fp++ = gc->state.transform.eyePosObj.z;
	*fp++ = gc->state.transform.eyePosObj.w;
	break;
#endif /*XXX_IS_CULLVERTEX_SUPPORTED*/
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    /* Use the motion of the pointers to type convert the result */
    if (ip != itemp) {
	__glConvertResult(gc, __GL_INT32, itemp, type, result, ip - itemp);
    } else
    if (fp != ftemp) {
	__glConvertResult(gc, __GL_FLOAT, ftemp, type, result, fp - ftemp);
    } else
    if (lp != ltemp) {
	__glConvertResult(gc, __GL_BOOLEAN, ltemp, type, result, lp - ltemp);
    } else
    if (cp != ctemp) {
	__glConvertResult(gc, __GL_COLOR, ctemp, type, result, cp - ctemp);
    } else
    if (scp != sctemp) {
	__glConvertResult(gc, __GL_SCOLOR, sctemp, type, result, scp - sctemp);
    }
}

void GLAPI __glim_GetDoublev(GLenum sq, GLdouble result[])
{
    DoGet(sq, result, __GL_FLOAT64, "glGetDoublev");
}

void GLAPI __glim_GetFloatv(GLenum sq, GLfloat result[])
{
    DoGet(sq, result, __GL_FLOAT32, "glGetFloatv");
}

void GLAPI __glim_GetIntegerv(GLenum sq, GLint result[])
{
    DoGet(sq, result, __GL_INT32, "glGetIntegerv");
}

void GLAPI __glim_GetBooleanv(GLenum sq, GLboolean result[])
{
    DoGet(sq, result, __GL_BOOLEAN, "glGetBooleanv");
}

/*
** Return the current error code.
*/
GLenum GLAPI __glim_GetError(void)
{
    GLint error;
    __GL_SETUP_NOT_IN_BEGIN2();
    __GL_API_GET();

    error = gc->error;
    gc->error = 0;

    return error;
}

/*
** Return a pointer to the requested string
*/
const GLubyte * GLAPI __glim_GetString(GLenum name)
{
    __GL_SETUP_NOT_IN_BEGIN2();
    __GL_API_GET();

    switch (name) {
      case GL_VENDOR:
	return (GLubyte*) gc->constants.vendor;
      case GL_RENDERER:
	return (GLubyte*) gc->constants.renderer;
      case GL_VERSION:
	return (GLubyte*) gc->constants.version;
      case GL_EXTENSIONS:
	return (GLubyte*) gc->constants.extensions;
      default:
	__glSetError(GL_INVALID_ENUM);
	return 0;
    }
}


void GLAPI __glim_GetPointerv(GLenum pname,
			      void **params)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_GET();

    switch(pname) {
      case GL_VERTEX_ARRAY_POINTER:
	  *params = *(void **)&gc->vertexArray.vp.pointer;
	  return;
      case GL_NORMAL_ARRAY_POINTER:
	  *params = *(void **)&gc->vertexArray.np.pointer;
	  return;
      case GL_COLOR_ARRAY_POINTER:
	  *params = *(void **)&gc->vertexArray.cp.pointer;
	  return;
      case GL_INDEX_ARRAY_POINTER:
	  *params = *(void **)&gc->vertexArray.ip.pointer;
	  return;
      case GL_TEXTURE_COORD_ARRAY_POINTER:
	  *params = *(void **)&gc->vertexArray.tp[gc->clientTexture.activeTexture].pointer;
	  return;
      case GL_EDGE_FLAG_ARRAY_POINTER:
	  *params = *(void **)&gc->vertexArray.ep.pointer;
	  return;
      case GL_SELECTION_BUFFER_POINTER:
	  *params = *(void **)&gc->select.resultBase;
	  return;
      case GL_FEEDBACK_BUFFER_POINTER:
	  *params = *(void **)&gc->feedback.resultBase;
	  return;
      default:
	__glSetError(GL_INVALID_OPERATION);
	return;
    }
}

void GLAPI __glim_GetPointervEXT(GLenum pname,
				 void **params)
{
    __glim_GetPointerv(pname, params);
}
