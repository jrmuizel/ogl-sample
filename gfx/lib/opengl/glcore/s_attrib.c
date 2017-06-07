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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_attrib.c#22 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"
#include "lighting.h"
#include "g_imports.h"
#include "image.h"


void GLAPI __glim_AlphaFunc(GLenum af, GLfloat ref)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if ((af < GL_NEVER) || (af > GL_ALWAYS)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    if (ref < __glZero) ref = __glZero;
    if (ref > __glOne) ref = __glOne;

    gc->state.raster.alphaFunction = af;
    gc->state.raster.alphaReference = ref;
    __GL_DELAY_VALIDATE(gc);
    gc->validateMask |= __GL_VALIDATE_ALPHA_FUNC;
}

void GLAPI __glim_BlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
    __GLfloat zero;
    __GLfloat one;
    __GL_SETUP_NOT_IN_BEGIN();

    zero = (__GLfloat)__glZero;
    one = (__GLfloat)__glOne;
    if (r < zero) r = zero;
    if (r > one) r = one;
    if (g < zero) g = zero;
    if (g > one) g = one;
    if (b < zero) b = zero;
    if (b > one) b = one;
    if (a < zero) a = zero;
    if (a > one) a = one;

    gc->state.raster.blendColor.r = r * gc->constants.redScale;
    gc->state.raster.blendColor.g = g * gc->constants.greenScale;
    gc->state.raster.blendColor.b = b * gc->constants.blueScale;
    gc->state.raster.blendColor.a = a * gc->constants.alphaScale;
#else /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
    __glSetError(GL_INVALID_OPERATION);
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
}

void GLAPI __glim_BlendEquation(GLenum mode)
{
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
    __GL_SETUP_NOT_IN_BEGIN();

    switch (mode) {
      case GL_MIN:
      case GL_MAX:
      case GL_FUNC_ADD:
      case GL_FUNC_SUBTRACT:
      case GL_FUNC_REVERSE_SUBTRACT:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    gc->state.raster.blendMode = mode;
    __GL_DELAY_VALIDATE(gc);
#else /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
    __glSetError(GL_INVALID_OPERATION);
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
}

void GLAPI __glim_BlendFunc(GLenum sf, GLenum df)
{
    __GL_SETUP_NOT_IN_BEGIN();

    switch (sf) {
      case GL_ZERO:
      case GL_ONE:
      case GL_DST_COLOR:
      case GL_ONE_MINUS_DST_COLOR:
      case GL_SRC_ALPHA:
      case GL_ONE_MINUS_SRC_ALPHA:
      case GL_DST_ALPHA:
      case GL_ONE_MINUS_DST_ALPHA:
      case GL_SRC_ALPHA_SATURATE:
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
      case GL_CONSTANT_COLOR:
      case GL_ONE_MINUS_CONSTANT_COLOR:
      case GL_CONSTANT_ALPHA:
      case GL_ONE_MINUS_CONSTANT_ALPHA:
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */	
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (df) {
      case GL_ZERO:
      case GL_ONE:
      case GL_SRC_COLOR:
      case GL_ONE_MINUS_SRC_COLOR:
      case GL_SRC_ALPHA:
      case GL_ONE_MINUS_SRC_ALPHA:
      case GL_DST_ALPHA:
      case GL_ONE_MINUS_DST_ALPHA:
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
      case GL_CONSTANT_COLOR:
      case GL_ONE_MINUS_CONSTANT_COLOR:
      case GL_CONSTANT_ALPHA:
      case GL_ONE_MINUS_CONSTANT_ALPHA:
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */	
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    gc->state.raster.blendSrc = sf;
    gc->state.raster.blendDst = df;
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_ClearAccum(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    __GLfloat minusOne;
    __GLfloat one;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    minusOne = __glMinusOne;
    one = __glOne;
    if (r < minusOne) r = minusOne;
    if (r > one) r = one;
    if (g < minusOne) g = minusOne;
    if (g > one) g = one;
    if (b < minusOne) b = minusOne;
    if (b > one) b = one;
    if (a < minusOne) a = minusOne;
    if (a > one) a = one;

    gc->state.accum.clear.r = r;
    gc->state.accum.clear.g = g;
    gc->state.accum.clear.b = b;
    gc->state.accum.clear.a = a;

    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    __GLfloat zero;
    __GLfloat one;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    zero = (__GLfloat)__glZero;
    one = (__GLfloat)__glOne;
    if (r < zero) r = zero;
    if (r > one) r = one;
    if (g < zero) g = zero;
    if (g > one) g = one;
    if (b < zero) b = zero;
    if (b > one) b = one;
    if (a < zero) a = zero;
    if (a > one) a = one;

    gc->state.raster.clear.r = r;
    gc->state.raster.clear.g = g;
    gc->state.raster.clear.b = b;
    gc->state.raster.clear.a = a;
}

void GLAPI __glim_ClearDepth(GLdouble z)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (z < 0) z = 0;
    if (z > 1) z = 1;
    gc->state.depth.clear = z;
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_ClearIndex(GLfloat val)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    val = __GL_MASK_INDEXF(gc, val);
    gc->state.raster.clearIndex = val;
}

void GLAPI __glim_ClearStencil(GLint s)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    gc->state.stencil.clear = (GLshort) (s & __GL_MAX_STENCIL_VALUE);
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_ColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    gc->state.raster.rMask = r;
    gc->state.raster.gMask = g;
    gc->state.raster.bMask = b;
    gc->state.raster.aMask = a;
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_ColorMaterial(GLenum face, GLenum p)
{
    __GL_SETUP_NOT_IN_BEGIN();

    switch (face) {
      case GL_FRONT:
      case GL_BACK:
      case GL_FRONT_AND_BACK:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (p) {
      case GL_EMISSION:
      case GL_SPECULAR:
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_AMBIENT_AND_DIFFUSE:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    gc->state.light.colorMaterialFace = face;
    gc->state.light.colorMaterialParam = p;

    if (gc->state.enables.general & __GL_COLOR_MATERIAL_ENABLE) {
	(*gc->procs.pickColorMaterialProcs)(gc);
	(*gc->procs.applyColor)(gc);
    }
}

void GLAPI __glim_CullFace(GLenum cfm)
{
    __GL_SETUP_NOT_IN_BEGIN();

    switch (cfm) {
      case GL_FRONT:
      case GL_BACK:
      case GL_FRONT_AND_BACK:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    gc->state.polygon.cull = cfm;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
}

void GLAPI __glim_DepthFunc(GLenum zf)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if ((zf < GL_NEVER) || (zf > GL_ALWAYS)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    gc->state.depth.testFunc = zf;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);
}

void GLAPI __glim_DepthMask(GLboolean enabled)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    gc->state.depth.writeEnable = enabled;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);
}

void GLAPI __glim_DrawBuffer(GLenum mode)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (mode) {
      case GL_NONE:
	gc->state.raster.drawBuffer = GL_NONE;
	break;
      case GL_FRONT_RIGHT:
      case GL_BACK_RIGHT:
      case GL_RIGHT:
      not_supported_in_this_implementation:
	__glSetError(GL_INVALID_OPERATION);
	return;
      case GL_FRONT:
      case GL_FRONT_LEFT:
	gc->state.raster.drawBuffer = GL_FRONT;
	break;
      case GL_FRONT_AND_BACK:
      case GL_LEFT:
	if (!gc->modes.doubleBufferMode) {
	    gc->state.raster.drawBuffer = GL_FRONT;
	} else {
	    gc->state.raster.drawBuffer = GL_FRONT_AND_BACK;
	}
	break;
      case GL_BACK:
      case GL_BACK_LEFT:
	if (!gc->modes.doubleBufferMode) {
	    goto not_supported_in_this_implementation;
	}
	gc->state.raster.drawBuffer = GL_BACK;
	break;
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3:
      aux_4_through_246:
	{
	    GLint i = mode - GL_AUX0;
	    if (i >= gc->modes.numAuxBuffers) {
		goto not_supported_in_this_implementation;
	    }
	    gc->state.raster.drawBuffer = mode;
	}
	break;
      default:
	if (mode > GL_AUX3 && mode < GL_AUX0+247) {
	    goto aux_4_through_246;
	}
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.raster.drawBufferReturn = mode;
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_Fogfv(GLenum p, const GLfloat pv[])
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (p) {
      case GL_FOG_COLOR:
	__glClampAndScaleColorf(gc, &gc->state.fog.color, pv);

	gc->state.fog.bufColor.r =
	    gc->state.fog.color.r * gc->constants.redRescale;
	gc->state.fog.bufColor.g =
	    gc->state.fog.color.g * gc->constants.greenRescale;
	gc->state.fog.bufColor.b =
	    gc->state.fog.color.b * gc->constants.blueRescale;

	gc->state.fog.r =
	    gc->state.fog.color.r * 255.0f * gc->constants.oneOverRedScale;
	gc->state.fog.g =
	    gc->state.fog.color.g * 255.0f * gc->constants.oneOverGreenScale;
	gc->state.fog.b =
	    gc->state.fog.color.b * 255.0f * gc->constants.oneOverBlueScale;
	break;
      case GL_FOG_DENSITY:
	if (pv[0] < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	gc->state.fog.density = pv[0];
	break;
      case GL_FOG_END:
	gc->state.fog.end = pv[0];
	break;
      case GL_FOG_START:
	gc->state.fog.start = pv[0];
	break;
      case GL_FOG_INDEX:
	{
	    GLint fogIndex, indexMask;

	    indexMask = (1 << gc->modes.indexBits) - 1;
	    fogIndex = (GLint)pv[0];
	    gc->state.fog.index = (__GLfloat)(fogIndex & indexMask);
	    gc->state.fog.i = fogIndex & indexMask;
	}
	break;
      case GL_FOG_MODE:
	switch ((GLenum) pv[0]) {
	  case GL_EXP:
	  case GL_EXP2:
	  case GL_LINEAR:
	    gc->state.fog.mode = (GLenum) pv[0];
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

    /*
    ** Recompute cached 1/(end - start) value for linear fogging.
    */
    if (gc->state.fog.mode == GL_LINEAR) {
	if (gc->state.fog.start != gc->state.fog.end) {
	    gc->state.fog.oneOverEMinusS =
		__glOne / (gc->state.fog.end - gc->state.fog.start);
	} else {
	    /*
	    ** Use zero as the undefined value.
	    */
	    gc->state.fog.oneOverEMinusS = __glZero;
	}
    }

    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_Fogf(GLenum p, GLfloat f)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_FOG_DENSITY:
      case GL_FOG_END:
      case GL_FOG_START:
      case GL_FOG_INDEX:
      case GL_FOG_MODE:
	__glim_Fogfv(p,&f);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_Fogiv(GLenum p, const GLint pv[])
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (p) {
      case GL_FOG_COLOR:
	__glClampAndScaleColori(gc, &gc->state.fog.color, pv);

	gc->state.fog.bufColor.r =
	    gc->state.fog.color.r * gc->constants.redRescale;
	gc->state.fog.bufColor.g =
	    gc->state.fog.color.g * gc->constants.greenRescale;
	gc->state.fog.bufColor.b =
	    gc->state.fog.color.b * gc->constants.blueRescale;

	gc->state.fog.r =
	    gc->state.fog.color.r * 255.0f * gc->constants.oneOverRedScale;
	gc->state.fog.g =
	    gc->state.fog.color.g * 255.0f * gc->constants.oneOverGreenScale;
	gc->state.fog.b =
	    gc->state.fog.color.b * 255.0f * gc->constants.oneOverBlueScale;
	break;
      case GL_FOG_DENSITY:
	if (pv[0] < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	gc->state.fog.density = pv[0];
	break;
      case GL_FOG_END:
	gc->state.fog.end = pv[0];
	break;
      case GL_FOG_START:
	gc->state.fog.start = pv[0];
	break;
      case GL_FOG_INDEX:
	{
	    GLint fogIndex, indexMask;

	    indexMask = (1 << gc->modes.indexBits) - 1;
	    fogIndex = (GLint)pv[0];
	    gc->state.fog.index = (__GLfloat)(fogIndex & indexMask);
	    gc->state.fog.i = fogIndex & indexMask;
	}
	break;
      case GL_FOG_MODE:
	switch ((GLenum) pv[0]) {
	  case GL_EXP:
	  case GL_EXP2:
	  case GL_LINEAR:
	    gc->state.fog.mode = (GLenum) pv[0];
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

    /*
    ** Recompute cached 1/(end - start) value for linear fogging.
    */
    if (gc->state.fog.mode == GL_LINEAR) {
	if (gc->state.fog.start != gc->state.fog.end) {
	    gc->state.fog.oneOverEMinusS =
		__glOne / (gc->state.fog.end - gc->state.fog.start);
	} else {
	    /*
	    ** Use zero as the undefined value.
	    */
	    gc->state.fog.oneOverEMinusS = __glZero;
	}
    }

    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_Fogi(GLenum p, GLint i)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_FOG_DENSITY:
      case GL_FOG_END:
      case GL_FOG_START:
      case GL_FOG_INDEX:
      case GL_FOG_MODE:
	__glim_Fogiv(p,&i);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_FrontFace(GLenum dir)
{
    __GL_SETUP_NOT_IN_BEGIN();

    switch (dir) {
      case GL_CW:
      case GL_CCW:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    gc->state.polygon.frontFaceDirection = dir;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
}

void GLAPI __glim_Hint(GLenum target, GLenum mode)
{
    __GLhintState *hs;
    __GL_SETUP_NOT_IN_BEGIN();

    hs = &gc->state.hints;
    switch (mode) {
      case GL_DONT_CARE:
      case GL_FASTEST:
      case GL_NICEST:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    switch (target) {
      case GL_PERSPECTIVE_CORRECTION_HINT:
	hs->perspectiveCorrection = mode;
	break;
      case GL_POINT_SMOOTH_HINT:
	hs->pointSmooth = mode;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POINT);
	return;
      case GL_LINE_SMOOTH_HINT:
	hs->lineSmooth = mode;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
	return;
      case GL_POLYGON_SMOOTH_HINT:
	hs->polygonSmooth = mode;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	return;
      case GL_FOG_HINT:
	hs->fog = mode;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_IndexMask(GLuint i)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    i = __GL_MASK_INDEXI(gc, i);
    gc->state.raster.writeMask = i;
    __GL_DELAY_VALIDATE(gc);
}

void __glTransformLightDirection(__GLcontext *gc, __GLlightSourceState *lss)
{
    __GLcoord dir;
    __GLfloat q;
    GLint target = lss - &gc->state.light.source[0];
    __GLtransform *tr;

    dir.x = lss->direction.x;
    dir.y = lss->direction.y;
    dir.z = lss->direction.z;
    if (lss->position.w != __glZero) {
	q = -(dir.x * lss->position.x + dir.y * lss->position.y +
	      dir.z * lss->position.z) / lss->position.w;
    } else {
	q = __glZero;
    }
    dir.w = q;

    tr = gc->transform.modelView;
    if (tr->updateInverse) {
	(*gc->procs.computeInverseTranspose)(gc, tr);
    }
    (*tr->inverseTranspose.xf4)(&lss->direction, &dir.x, &tr->inverseTranspose);
    (*gc->procs.normalize)(&gc->light.source[target].direction.x,
			   &lss->direction.x);
}

void GLAPI __glim_Lightfv(GLenum light, GLenum p, const GLfloat pv[])
{
    __GLlightSourceState *lss;
    __GLmatrix *m;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    light -= GL_LIGHT0;
    if (light >= (GLuint) gc->constants.numberOfLights){ /* assumes GLenum is unsigned */
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    lss = &gc->state.light.source[light];
    switch (p) {
      case GL_AMBIENT:
	__glScaleColorf(gc, &lss->ambient, pv);
	break;
      case GL_DIFFUSE:
	__glScaleColorf(gc, &lss->diffuse, pv);
	break;
      case GL_SPECULAR:
	__glScaleColorf(gc, &lss->specular, pv);
	break;
      case GL_POSITION:
	lss->position.x = pv[0];
	lss->position.y = pv[1];
	lss->position.z = pv[2];
	lss->position.w = pv[3];

	/*
	** Transform light position into eye space
	*/
	m = &gc->transform.modelView->matrix;
	(*m->xf4)(&lss->positionEye, &lss->position.x, m);
	break;
      case GL_SPOT_DIRECTION:
	lss->direction.x = pv[0];
	lss->direction.y = pv[1];
	lss->direction.z = pv[2];
	lss->direction.w = 1;
	__glTransformLightDirection(gc, lss);
	break;
      case GL_SPOT_EXPONENT:
	if ((pv[0] < 0) || (pv[0] > 128)) {
	  bad_value:
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	lss->spotLightExponent = pv[0];
	break;
      case GL_SPOT_CUTOFF:
	if ((pv[0] != 180) && ((pv[0] < 0) || (pv[0] > 90))) {
	    goto bad_value;
	}
	lss->spotLightCutOffAngle = pv[0];
	break;
      case GL_CONSTANT_ATTENUATION:
	if (pv[0] < __glZero) {
	    goto bad_value;
	}
	lss->constantAttenuation = pv[0];
	break;
      case GL_LINEAR_ATTENUATION:
	if (pv[0] < __glZero) {
	    goto bad_value;
	}
	lss->linearAttenuation = pv[0];
	break;
      case GL_QUADRATIC_ATTENUATION:
	if (pv[0] < __glZero) {
	    goto bad_value;
	}
	lss->quadraticAttenuation = pv[0];
	break;
      default:
	goto bad_enum;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
}

void GLAPI __glim_Lightf(GLenum light, GLenum p, GLfloat f)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_SPOT_EXPONENT:
      case GL_SPOT_CUTOFF:
      case GL_CONSTANT_ATTENUATION:
      case GL_LINEAR_ATTENUATION:
      case GL_QUADRATIC_ATTENUATION:
	__glim_Lightfv(light, p, &f);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_Lightiv(GLenum light, GLenum p, const GLint pv[])
{
    __GLlightSourceState *lss;
    __GLmatrix *m;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    light -= GL_LIGHT0;
    if (light >= (GLuint) gc->constants.numberOfLights){ /* assumes GLenum is unsigned */
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    lss = &gc->state.light.source[light];
    switch (p) {
      case GL_AMBIENT:
	__glScaleColori(gc, &lss->ambient, pv);
	break;
      case GL_DIFFUSE:
	__glScaleColori(gc, &lss->diffuse, pv);
	break;
      case GL_SPECULAR:
	__glScaleColori(gc, &lss->specular, pv);
	break;
      case GL_POSITION:
	lss->position.x = pv[0];
	lss->position.y = pv[1];
	lss->position.z = pv[2];
	lss->position.w = pv[3];
	/*
	** Transform light position into eye space
	*/
	m = &gc->transform.modelView->matrix;
	(*m->xf4)(&lss->positionEye, &lss->position.x, m);
	break;
      case GL_SPOT_DIRECTION:
	lss->direction.x = pv[0];
	lss->direction.y = pv[1];
	lss->direction.z = pv[2];
	lss->direction.w = 1;
	__glTransformLightDirection(gc, lss);
	break;
      case GL_SPOT_EXPONENT:
	if ((pv[0] < 0) || (pv[0] > 128)) {
	  bad_value:
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	lss->spotLightExponent = pv[0];
	break;
      case GL_SPOT_CUTOFF:
	if ((pv[0] != 180) && ((pv[0] < 0) || (pv[0] > 90))) {
	    goto bad_value;
	}
	lss->spotLightCutOffAngle = pv[0];
	break;
      case GL_CONSTANT_ATTENUATION:
	if (pv[0] < 0) {
	    goto bad_value;
	}
	lss->constantAttenuation = pv[0];
	break;
      case GL_LINEAR_ATTENUATION:
	if (pv[0] < 0) {
	    goto bad_value;
	}
	lss->linearAttenuation = pv[0];
	break;
      case GL_QUADRATIC_ATTENUATION:
	if (pv[0] < 0) {
	    goto bad_value;
	}
	lss->quadraticAttenuation = pv[0];
	break;
      default:
	goto bad_enum;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
}

void GLAPI __glim_Lighti(GLenum light, GLenum p, GLint i)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_SPOT_EXPONENT:
      case GL_SPOT_CUTOFF:
      case GL_CONSTANT_ATTENUATION:
      case GL_LINEAR_ATTENUATION:
      case GL_QUADRATIC_ATTENUATION:
	__glim_Lightiv(light, p, &i);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_LightModelfv(GLenum p, const GLfloat pv[])
{
    __GLlightModelState *model;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    model = &gc->state.light.model;
    switch (p) {
      case GL_LIGHT_MODEL_AMBIENT:
	__glScaleColorf(gc, &model->ambient, pv);
	break;
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
	model->localViewer = pv[0] != 0;
	break;
      case GL_LIGHT_MODEL_TWO_SIDE:
	model->twoSided = pv[0] != 0;
	break;
      case GL_LIGHT_MODEL_COLOR_CONTROL:
	if (pv[0] == GL_SINGLE_COLOR || pv[0] == GL_SEPARATE_SPECULAR_COLOR) {
	    model->colorControl = pv[0];
	} else {
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
}

void GLAPI __glim_LightModelf(GLenum p, GLfloat f)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
      case GL_LIGHT_MODEL_COLOR_CONTROL:
	__glim_LightModelfv(p, &f);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_LightModeliv(GLenum p, const GLint pv[])
{
    __GLlightModelState *model;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    model = &gc->state.light.model;
    switch (p) {
      case GL_LIGHT_MODEL_AMBIENT:
	__glScaleColori(gc, &model->ambient, pv);
	break;
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
	model->localViewer = pv[0] != 0;
	break;
      case GL_LIGHT_MODEL_TWO_SIDE:
	model->twoSided = pv[0] != 0;
	break;
      case GL_LIGHT_MODEL_COLOR_CONTROL:
	if (pv[0] == GL_SINGLE_COLOR || pv[0] == GL_SEPARATE_SPECULAR_COLOR) {
	    model->colorControl = pv[0];
	} else {
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
}

void GLAPI __glim_LightModeli(GLenum p, GLint i)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
      case GL_LIGHT_MODEL_COLOR_CONTROL:
	__glim_LightModeliv(p, &i);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_LineStipple(GLint factor, GLushort stipple)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (factor < 1) {
	factor = 1;
    }
    if (factor > 256) {
	factor = 256;
    }

    __GL_API_STATE();

    gc->state.line.stippleRepeat = (GLshort) factor;
    gc->state.line.stipple = stipple;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
}

static GLint RoundWidth(__GLcontext *gc, __GLfloat size)
{
    GLint minSize = gc->constants.aliasedLineWidthMinimum;
    GLint maxSize = gc->constants.aliasedLineWidthMaximum;

    if (size <= minSize) return minSize;
    if (size >= maxSize) return maxSize;

    return size + (__GLfloat) 0.5;
}

static __GLfloat ClampWidth(__GLcontext *gc, __GLfloat size)
{
    __GLfloat minSize = gc->constants.lineWidthMinimum;
    __GLfloat maxSize = gc->constants.lineWidthMaximum;
    __GLfloat gran = gc->constants.lineWidthGranularity;
    GLint i;

    if (size <= minSize) return minSize;
    if (size >= maxSize) return maxSize;

    /* choose closest fence post */
    i = (GLint)(((size - minSize) / gran) + __glHalf);
    return minSize + i * gran;
}

void GLAPI __glim_LineWidth(GLfloat width)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (width <= 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    __GL_API_STATE();

    gc->state.line.requestedWidth = width;
    gc->state.line.aliasedWidth = RoundWidth(gc, width);
    gc->state.line.smoothWidth = ClampWidth(gc, width);
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
}

void GLAPI __glim_LogicOp(GLenum op)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if ((op < GL_CLEAR) || (op > GL_SET)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    gc->state.raster.logicOp = op;
    __GL_DELAY_VALIDATE(gc);
}

static GLint ApplyParameterF(__GLcontext *gc, __GLmaterialState *ms,
			     GLenum p, const GLfloat pv[])
{
    switch (p) {
      case GL_COLOR_INDEXES:
	ms->cmapa = pv[0];
	ms->cmapd = pv[1];
	ms->cmaps = pv[2];
	return __GL_MATERIAL_COLORINDEXES;
      case GL_EMISSION:
	__glScaleColorf(gc, &ms->emissive, pv);
	return __GL_MATERIAL_EMISSIVE;
      case GL_SPECULAR:
	ms->specular.r = pv[0];
	ms->specular.g = pv[1];
	ms->specular.b = pv[2];
	ms->specular.a = pv[3];
	return __GL_MATERIAL_SPECULAR;
      case GL_SHININESS:
	ms->specularExponent = pv[0];
	return __GL_MATERIAL_SHININESS;
      case GL_AMBIENT:
	ms->ambient.r = pv[0];
	ms->ambient.g = pv[1];
	ms->ambient.b = pv[2];
	ms->ambient.a = pv[3];
	return __GL_MATERIAL_AMBIENT;
      case GL_DIFFUSE:
	ms->diffuse.r = pv[0];
	ms->diffuse.g = pv[1];
	ms->diffuse.b = pv[2];
	ms->diffuse.a = pv[3];
	return __GL_MATERIAL_DIFFUSE;
      case GL_AMBIENT_AND_DIFFUSE:
	ms->ambient.r = pv[0];
	ms->ambient.g = pv[1];
	ms->ambient.b = pv[2];
	ms->ambient.a = pv[3];
	ms->diffuse = ms->ambient;
	return __GL_MATERIAL_DIFFUSE | __GL_MATERIAL_AMBIENT;
      default:
	assert(0);
	return 0;
    }
}

GLenum __glErrorCheckMaterial(GLenum face, GLenum p, GLfloat pv0)
{
    switch (face) {
      case GL_FRONT:
      case GL_BACK:
      case GL_FRONT_AND_BACK:
	break;
      default:
	return GL_INVALID_ENUM;
    }
    switch (p) {
      case GL_COLOR_INDEXES:
      case GL_EMISSION:
      case GL_SPECULAR:
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_AMBIENT_AND_DIFFUSE:
	break;
      case GL_SHININESS:
	if (pv0 < 0 || pv0 > 128) {
	    return GL_INVALID_VALUE;
	}
	break;
      default:
	return GL_INVALID_ENUM;
    }
    return GL_NO_ERROR;
}

void GLAPI __glim_Materialfv(GLenum face, GLenum p, const GLfloat pv[])
{
    GLenum error;
    GLint frontChange = 0, backChange = 0;
    __GL_SETUP();

    error = __glErrorCheckMaterial(face, p, pv[0]);
    if (error != GL_NO_ERROR) {
	__glSetError(error);
	return;
    }

    /*
    ** If we are in the middle of contructing a primitive, we possibly
    ** need to validate the front and back colors of the vertices which
    ** we have queued.
    */
    if (__GL_IN_BEGIN()) {
	if (gc->vertex.materialNeeds) (*gc->procs.matValidate)(gc);
    } else {
	__GL_API_STATE();
    }

    switch (face) {
      case GL_FRONT:
	frontChange = ApplyParameterF(gc, &gc->state.light.front, p, pv);
	backChange = 0;
	break;
      case GL_BACK:
	backChange = ApplyParameterF(gc, &gc->state.light.back, p, pv);
	frontChange = 0;
	break;
      case GL_FRONT_AND_BACK:
	backChange = ApplyParameterF(gc, &gc->state.light.back, p, pv);
	frontChange = ApplyParameterF(gc, &gc->state.light.front, p, pv);
	break;
    }

    __glValidateMaterial(gc, frontChange, backChange);

    if (gc->state.enables.general & __GL_COLOR_MATERIAL_ENABLE) {
	(*gc->procs.applyColor)(gc);
    }
}

void GLAPI __glim_Materialf(GLenum face, GLenum p, GLfloat f)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_SHININESS:
	__glim_Materialfv(face, p, &f);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

static GLint ApplyParameterI(__GLcontext *gc, __GLmaterialState *ms,
			     GLenum p, const GLint pv[])
{
    switch (p) {
      case GL_COLOR_INDEXES:
	ms->cmapa = pv[0];
	ms->cmapd = pv[1];
	ms->cmaps = pv[2];
	return __GL_MATERIAL_COLORINDEXES;
      case GL_EMISSION:
	__glScaleColori(gc, &ms->emissive, pv);
	return __GL_MATERIAL_EMISSIVE;
      case GL_SPECULAR:
	ms->specular.r = __GL_I_TO_FLOAT(pv[0]);
	ms->specular.g = __GL_I_TO_FLOAT(pv[1]);
	ms->specular.b = __GL_I_TO_FLOAT(pv[2]);
	ms->specular.a = __GL_I_TO_FLOAT(pv[3]);
	return __GL_MATERIAL_SPECULAR;
      case GL_SHININESS:
	ms->specularExponent = pv[0];
	return __GL_MATERIAL_SHININESS;
      case GL_AMBIENT:
	ms->ambient.r = __GL_I_TO_FLOAT(pv[0]);
	ms->ambient.g = __GL_I_TO_FLOAT(pv[1]);
	ms->ambient.b = __GL_I_TO_FLOAT(pv[2]);
	ms->ambient.a = __GL_I_TO_FLOAT(pv[3]);
	return __GL_MATERIAL_AMBIENT;
      case GL_DIFFUSE:
	ms->diffuse.r = __GL_I_TO_FLOAT(pv[0]);
	ms->diffuse.g = __GL_I_TO_FLOAT(pv[1]);
	ms->diffuse.b = __GL_I_TO_FLOAT(pv[2]);
	ms->diffuse.a = __GL_I_TO_FLOAT(pv[3]);
	return __GL_MATERIAL_DIFFUSE;
      case GL_AMBIENT_AND_DIFFUSE:
	ms->ambient.r = __GL_I_TO_FLOAT(pv[0]);
	ms->ambient.g = __GL_I_TO_FLOAT(pv[1]);
	ms->ambient.b = __GL_I_TO_FLOAT(pv[2]);
	ms->ambient.a = __GL_I_TO_FLOAT(pv[3]);
	ms->diffuse = ms->ambient;
	return __GL_MATERIAL_DIFFUSE | __GL_MATERIAL_AMBIENT;
      default:
	assert(0);
	return 0;
    }
}

void GLAPI __glim_Materialiv(GLenum face, GLenum p,
		       const GLint pv[])
{
    GLenum error;
    GLint frontChange = 0, backChange = 0;
    __GL_SETUP();

    error = __glErrorCheckMaterial(face, p, pv[0]);
    if (error != GL_NO_ERROR) {
	__glSetError(error);
	return;
    }

    /*
    ** If we are in the middle of contructing a primitive, we possibly
    ** need to validate the front and back colors of the vertices which
    ** we have queued.
    */
    if (__GL_IN_BEGIN()) {
	if (gc->vertex.materialNeeds) (*gc->procs.matValidate)(gc);
    } else {
	__GL_API_STATE();
    }

    switch (face) {
      case GL_FRONT:
	frontChange = ApplyParameterI(gc, &gc->state.light.front, p, pv);
	backChange = 0;
	break;
      case GL_BACK:
	backChange = ApplyParameterI(gc, &gc->state.light.back, p, pv);
	frontChange = 0;
	break;
      case GL_FRONT_AND_BACK:
	backChange = ApplyParameterI(gc, &gc->state.light.back, p, pv);
	frontChange = ApplyParameterI(gc, &gc->state.light.front, p, pv);
	break;
    }

    __glValidateMaterial(gc, frontChange, backChange);

    if (gc->state.enables.general & __GL_COLOR_MATERIAL_ENABLE) {
	(*gc->procs.applyColor)(gc);
    }
}

void GLAPI __glim_Materiali(GLenum face, GLenum p, GLint i)
{
    /* Accept only enumerants that correspond to single values */
    switch (p) {
      case GL_SHININESS:
	__glim_Materialiv(face, p, &i);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

static GLint RoundSize(__GLcontext *gc, __GLfloat size)
{
    GLint minSize = gc->constants.aliasedPointSizeMinimum;
    GLint maxSize = gc->constants.aliasedPointSizeMaximum;

    if (size <= minSize) return minSize;
    if (size >= maxSize) return maxSize;

    return size + (__GLfloat) 0.5;
}

static __GLfloat ClampSize(__GLcontext *gc, __GLfloat size)
{
    __GLfloat minSize = gc->constants.pointSizeMinimum;
    __GLfloat maxSize = gc->constants.pointSizeMaximum;
    __GLfloat gran = gc->constants.pointSizeGranularity;
    GLint i;

    if (size <= minSize) return minSize;
    if (size >= maxSize) return maxSize;

    /* choose closest fence post */
    i = (GLint)(((size - minSize) / gran) + __glHalf);
    return minSize + i * gran;
}

void GLAPI __glim_PointSize(GLfloat f)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (f <= __glZero) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    __GL_API_STATE();

    gc->state.point.requestedSize = f;
    gc->state.point.aliasedSize = RoundSize(gc, f);
    gc->state.point.smoothSize = ClampSize(gc, f);
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POINT);
}

void GLAPI __glim_PolygonMode(GLenum face, GLenum mode)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (mode) {
      case GL_FILL:
	break;
      case GL_POINT:
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POINT);
	break;
      case GL_LINE:
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (face) {
      case GL_FRONT:
	gc->state.polygon.frontMode = mode;
	break;
      case GL_BACK:
	gc->state.polygon.backMode = mode;
	break;
      case GL_FRONT_AND_BACK:
	gc->state.polygon.frontMode = mode;
	gc->state.polygon.backMode = mode;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
}

void GLAPI __glim_PolygonStipple(const GLubyte *mask)
{
    GLubyte *stipple;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    stipple = &gc->state.polygonStipple.stipple[0];
    __glFillImage(gc, 32, 32, GL_COLOR_INDEX, GL_BITMAP, mask, stipple);
    (*gc->procs.convertPolygonStipple)(gc);
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
}

void GLAPI __glim_ShadeModel(GLenum sm)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if ((sm < GL_FLAT) || (sm > GL_SMOOTH)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    gc->state.light.shadingModel = sm;
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_StencilMask(GLuint sm)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    gc->state.stencil.writeMask = (GLshort) (sm & __GL_MAX_STENCIL_VALUE);
    __GL_DELAY_VALIDATE(gc);
    gc->validateMask |= __GL_VALIDATE_STENCIL_FUNC;
}

void GLAPI __glim_StencilFunc(GLenum func, GLint ref, GLuint mask)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if ((func < GL_NEVER) || (func > GL_ALWAYS)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    if (ref < 0) ref = 0;
    if (ref > __GL_MAX_STENCIL_VALUE) ref = __GL_MAX_STENCIL_VALUE;
    gc->state.stencil.testFunc = func;
    gc->state.stencil.reference = (GLshort) ref;
    gc->state.stencil.mask = (GLshort) (mask & __GL_MAX_STENCIL_VALUE);
    __GL_DELAY_VALIDATE(gc);
    gc->validateMask |= __GL_VALIDATE_STENCIL_FUNC;
}

void GLAPI __glim_StencilOp(GLenum fail, GLenum depthFail, GLenum depthPass)
{
    __GL_SETUP_NOT_IN_BEGIN();

    switch (fail) {
      case GL_KEEP: case GL_ZERO: case GL_REPLACE:
      case GL_INCR: case GL_DECR: case GL_INVERT:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (depthFail) {
      case GL_KEEP: case GL_ZERO: case GL_REPLACE:
      case GL_INCR: case GL_DECR: case GL_INVERT:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    switch (depthPass) {
      case GL_KEEP: case GL_ZERO: case GL_REPLACE:
      case GL_INCR: case GL_DECR: case GL_INVERT:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_STATE();

    gc->state.stencil.fail = fail;
    gc->state.stencil.depthFail = depthFail;
    gc->state.stencil.depthPass = depthPass;
    __GL_DELAY_VALIDATE(gc);
    gc->validateMask |= __GL_VALIDATE_STENCIL_OP;
}

/************************************************************************/

/*
** Copy context information from src to dst.  Mark dst for validation
** when done.
*/
GLboolean __glCopyContext(__GLcontext *dst, const __GLcontext *src, GLuint mask)
{
    const __GLattribute *sp;
    GLboolean rv = GL_TRUE;

    sp = &src->state;

    if (mask & GL_ACCUM_BUFFER_BIT) {
	dst->state.accum = sp->accum;
    }

    if (mask & GL_COLOR_BUFFER_BIT) {
	dst->state.raster = sp->raster;
	dst->state.enables.general &= ~__GL_COLOR_BUFFER_ENABLES;
	dst->state.enables.general |=
	    sp->enables.general & __GL_COLOR_BUFFER_ENABLES;
	dst->validateMask |= __GL_VALIDATE_ALPHA_FUNC; /*XXX*/
    }

    if (mask & GL_CURRENT_BIT) {
	dst->state.current = sp->current;
    }

    if (mask & GL_DEPTH_BUFFER_BIT) {
	dst->state.depth = sp->depth;
	dst->state.enables.general &= ~__GL_DEPTH_TEST_ENABLE;
	dst->state.enables.general |=
	    sp->enables.general & __GL_DEPTH_TEST_ENABLE;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_DEPTH);
    }

    if (mask & GL_ENABLE_BIT) {
	dst->state.enables = sp->enables;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_LINE | __GL_DIRTY_POLYGON |
		__GL_DIRTY_POINT | __GL_DIRTY_LIGHTING | __GL_DIRTY_DEPTH |
		__GL_DIRTY_SCISSOR);
	(*dst->procs.pickColorMaterialProcs)(dst);
	(*dst->procs.applyColor)(dst);
    }

    if (mask & GL_EVAL_BIT) {
	dst->state.evaluator = sp->evaluator;
	dst->state.enables.general &= ~__GL_AUTO_NORMAL_ENABLE;
	dst->state.enables.general |=
	    sp->enables.general & __GL_AUTO_NORMAL_ENABLE;
	dst->state.enables.eval1 = sp->enables.eval1;
	dst->state.enables.eval2 = sp->enables.eval2;
    }

    if (mask & GL_FOG_BIT) {
	dst->state.fog = sp->fog;
	dst->state.enables.general &= ~__GL_FOG_ENABLE;
	dst->state.enables.general |=
	    sp->enables.general & __GL_FOG_ENABLE;
    }

    if (mask & GL_HINT_BIT) {
	dst->state.hints = sp->hints;
    }

    if (mask & GL_LIGHTING_BIT) {
	dst->state.light.colorMaterialFace = sp->light.colorMaterialFace;
	dst->state.light.colorMaterialParam = sp->light.colorMaterialParam;
	dst->state.light.shadingModel = sp->light.shadingModel;
	dst->state.light.model = sp->light.model;
	dst->state.light.front = sp->light.front;
	dst->state.light.back = sp->light.back;
	__GL_MEMCOPY(dst->state.light.source, sp->light.source,
		     dst->constants.numberOfLights
		     * sizeof(__GLlightSourceState));
	dst->state.enables.general &= ~__GL_LIGHTING_ENABLES;
	dst->state.enables.general |=
	    sp->enables.general & __GL_LIGHTING_ENABLES;
	dst->state.enables.lights = sp->enables.lights;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_LIGHTING);
    }

    if (mask & GL_LINE_BIT) {
	dst->state.line = sp->line;
	dst->state.enables.general &= ~__GL_LINE_ENABLES;
	dst->state.enables.general |=
	    sp->enables.general & __GL_LINE_ENABLES;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_LINE);
    }

    if (mask & GL_LIST_BIT) {
	dst->state.list = sp->list;
    }

    if (mask & GL_PIXEL_MODE_BIT) {
	GLint i;

	dst->state.pixel.readBuffer = sp->pixel.readBuffer;
	dst->state.pixel.readBufferReturn = sp->pixel.readBufferReturn;
	dst->state.pixel.transferMode = sp->pixel.transferMode;
	dst->state.enables.pixelPath = sp->enables.pixelPath;
	for (i=0; i<__GL_NUM_COLOR_TABLE_TARGETS; ++i) {
	    *dst->state.pixel.colorTable[i] = *sp->pixel.colorTable[i];
	}
	for (i=0; i<__GL_NUM_CONVOLUTION_TARGETS; ++i) {
	    *dst->state.pixel.convolutionFilter[i] =
				    *sp->pixel.convolutionFilter[i];
	}
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_PIXEL);
    }

    if (mask & GL_POINT_BIT) {
	dst->state.point = sp->point;
	dst->state.enables.general &= ~__GL_POINT_SMOOTH_ENABLE;
	dst->state.enables.general |=
	    sp->enables.general & __GL_POINT_SMOOTH_ENABLE;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_POINT);
    }

    if (mask & GL_POLYGON_BIT) {
	dst->state.polygon = sp->polygon;
	dst->state.enables.general &= ~__GL_POLYGON_ENABLES;
	dst->state.enables.general |=
	    sp->enables.general & __GL_POLYGON_ENABLES;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_POLYGON);
    }

    if (mask & GL_POLYGON_STIPPLE_BIT) {
	dst->state.polygonStipple = sp->polygonStipple;
	dst->state.enables.general &= ~__GL_POLYGON_STIPPLE_ENABLE;
	dst->state.enables.general |=
	    sp->enables.general & __GL_POLYGON_STIPPLE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_POLYGON |
		__GL_DIRTY_POLYGON_STIPPLE);
    }

    if (mask & GL_SCISSOR_BIT) {
	dst->state.scissor = sp->scissor;
	dst->state.enables.general &= ~__GL_SCISSOR_TEST_ENABLE;
	dst->state.enables.general |=
	    sp->enables.general & __GL_SCISSOR_TEST_ENABLE;
	__GL_DELAY_VALIDATE_MASK(dst, __GL_DIRTY_SCISSOR);
    }

    if (mask & GL_STENCIL_BUFFER_BIT) {
	dst->state.stencil = sp->stencil;
	dst->state.enables.general &= ~__GL_STENCIL_TEST_ENABLE;
	dst->state.enables.general |=
	    sp->enables.general & __GL_STENCIL_TEST_ENABLE;
	dst->validateMask |= __GL_VALIDATE_STENCIL_FUNC |
	    __GL_VALIDATE_STENCIL_OP; /*XXX*/
    }

    if (mask & GL_TEXTURE_BIT) {
	GLint i, j;

	/* active texture */
	dst->state.texture.activeTexture = sp->texture.activeTexture;
	dst->state.texture.active =
		&dst->state.texture.unit[dst->state.texture.activeTexture];
	dst->texture.active =
		&dst->texture.unit[dst->state.texture.activeTexture];

	/* texture unit state */
	for (i=0; i < __GL_NUM_TEXTURE_UNITS; i++) {
	    const __GLtextureUnitState *tusSrc = &sp->texture.unit[i];
	    __GLtextureUnitState *tusDst = &dst->state.texture.unit[i];

	    /* TexGen state */
	    tusDst->s = tusSrc->s;
	    tusDst->t = tusSrc->t;
	    tusDst->r = tusSrc->r;
	    tusDst->q = tusSrc->q;

	    /*
	    ** Texture binding state.
	    ** If the texture name is different, a new binding is
	    ** called for.  Deferring the binding is dangerous, because
	    ** the state before the pop has to be saved with the
	    ** texture that is being unbound.  If we defer the binding,
	    ** we need to watch out for cases like two pops in a row
	    ** or a pop followed by a bind.
	    */
	    for (j=0; j < __GL_NUM_TEXTURE_BINDINGS; j++) {
		__GLtextureBindingState *tbsSrc = tusSrc->texture[j];
		__GLtextureBindingState *tbsDst = tusDst->texture[j];

		if (tbsDst->name != tbsSrc->name) {
		    __glBindTexture(dst, i, j, tbsSrc->name);
		}
		tbsDst->params = tbsSrc->params;
	    }

	    /* TexEnv state */
	    __GL_MEMCOPY(tusDst->env, tusSrc->env,
				__GL_NUM_TEXTURE_ENV_BINDINGS
				* sizeof(__GLtextureEnvState));

	    /* texture enable state */
	    dst->state.enables.texture[i] = sp->enables.texture[i];
	}
    }

    if (mask & GL_TRANSFORM_BIT) {
	dst->state.transform.matrixMode = sp->transform.matrixMode;
	__GL_MEMCOPY(dst->state.transform.eyeClipPlanes,
		     sp->transform.eyeClipPlanes,
		     dst->constants.numberOfClipPlanes * sizeof(__GLcoord));
	dst->state.enables.general &= ~__GL_NORMALIZE_ENABLE;
	dst->state.enables.general |=
	  sp->enables.general & __GL_NORMALIZE_ENABLE;
	dst->state.enables.general &= ~__GL_RESCALE_NORMAL_ENABLE;
	dst->state.enables.general |=
	  sp->enables.general & __GL_RESCALE_NORMAL_ENABLE;
    }

    if (mask & GL_VIEWPORT_BIT) {
	dst->state.viewport = sp->viewport;
	(*dst->procs.applyViewport)(dst);
    }

    __GL_DELAY_VALIDATE(dst);

    return rv;
}

/************************************************************************/

void GLAPI __glim_PushAttrib(GLuint mask)
{
    __GLattribute **spp;
    __GLattribute *sp;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    spp = gc->attributes.stackPointer;
    if (spp < &gc->attributes.stack[gc->constants.maxAttribStackDepth]) {
	if (!(sp = *spp)) {
	    sp = (__GLattribute*)
		(*gc->imports.calloc)(gc, 1, sizeof(__GLattribute));
	    *spp = sp;
	}
	sp->mask = mask;
	sp->enables = gc->state.enables;	/* Always save enables */
	gc->attributes.stackPointer = spp + 1;

	if (mask & GL_ACCUM_BUFFER_BIT) {
	    sp->accum = gc->state.accum;
	}
	if (mask & GL_COLOR_BUFFER_BIT) {
	    sp->raster = gc->state.raster;
	}
	if (mask & GL_CURRENT_BIT) {
	    sp->current = gc->state.current;
	}
	if (mask & GL_DEPTH_BUFFER_BIT) {
	    sp->depth = gc->state.depth;
	}
	if (mask & GL_ENABLE_BIT) {
	    sp->enables = gc->state.enables;
	}
	if (mask & GL_EVAL_BIT) {
	    sp->evaluator = gc->state.evaluator;
	}
	if (mask & GL_FOG_BIT) {
	    sp->fog = gc->state.fog;
	}
	if (mask & GL_HINT_BIT) {
	    sp->hints = gc->state.hints;
	}
	if (mask & GL_LIGHTING_BIT) {
	    size_t bytes = (size_t)
		(gc->constants.numberOfLights * sizeof(__GLlightSourceState));
	    sp->light.colorMaterialFace = gc->state.light.colorMaterialFace;
	    sp->light.colorMaterialParam = gc->state.light.colorMaterialParam;
	    sp->light.shadingModel = gc->state.light.shadingModel;
	    sp->light.model = gc->state.light.model;
	    sp->light.front = gc->state.light.front;
	    sp->light.back = gc->state.light.back;
	    sp->light.source = (__GLlightSourceState*)
		(*gc->imports.malloc)(gc, bytes);
	    __GL_MEMCOPY(sp->light.source, gc->state.light.source, bytes);
	}
	if (mask & GL_LINE_BIT) {
	    sp->line = gc->state.line;
	}
	if (mask & GL_LIST_BIT) {
	    sp->list = gc->state.list;
	}
	if (mask & GL_PIXEL_MODE_BIT) {
	    GLint i;

	    sp->pixel.readBuffer = gc->state.pixel.readBuffer;
	    sp->pixel.readBufferReturn = gc->state.pixel.readBufferReturn;
	    sp->pixel.transferMode = gc->state.pixel.transferMode;
	    sp->enables.pixelPath = gc->state.enables.pixelPath;
	    for (i=0; i<__GL_NUM_COLOR_TABLE_TARGETS; ++i) {
	        sp->pixel.colorTable[i] = (__GLcolorTableState *)
		  (*gc->imports.malloc)(gc, sizeof(__GLcolorTableState));
	        *sp->pixel.colorTable[i] = *gc->state.pixel.colorTable[i];
	    }
	    for (i=0; i<__GL_NUM_CONVOLUTION_TARGETS; ++i) {
	        sp->pixel.convolutionFilter[i] = (__GLconvolutionFilterState *)
		  (*gc->imports.malloc)(gc, sizeof(__GLconvolutionFilterState));
	        *sp->pixel.convolutionFilter[i] =
					*gc->state.pixel.convolutionFilter[i];
	    }
	}
	if (mask & GL_POINT_BIT) {
	    sp->point = gc->state.point;
	}
	if (mask & GL_POLYGON_BIT) {
	    sp->polygon = gc->state.polygon;
	}
	if (mask & GL_POLYGON_STIPPLE_BIT) {
	    sp->polygonStipple = gc->state.polygonStipple;
	}
	if (mask & GL_SCISSOR_BIT) {
	    sp->scissor = gc->state.scissor;
	}
	if (mask & GL_STENCIL_BUFFER_BIT) {
	    sp->stencil = gc->state.stencil;
	}
	if (mask & GL_TEXTURE_BIT) {
	    GLint i, j;

	    /* active texture */
	    sp->texture.activeTexture = gc->state.texture.activeTexture;
	    sp->texture.active = &sp->texture.unit[sp->texture.activeTexture];

	    /* texture unit state */
	    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
		const __GLtextureUnitState *tusSrc = &gc->state.texture.unit[i];
		__GLtextureUnitState *tusDst = &sp->texture.unit[i];

		/* TexGen state */
		tusDst->s = tusSrc->s;
		tusDst->t = tusSrc->t;
		tusDst->r = tusSrc->r;
		tusDst->q = tusSrc->q;

		/* Texture binding state */
		for (j=0; j < __GL_NUM_TEXTURE_BINDINGS; j++) {
		    __GLtextureBindingState *tbsSrc = tusSrc->texture[j];
		    __GLtextureBindingState *tbsDst;

		    tbsDst = (__GLtextureBindingState *)
			(*gc->imports.calloc)(gc, 1, sizeof(*tbsDst));
		    tusDst->texture[j] = tbsDst;

		    tbsDst->name = tbsSrc->name;
		    tbsDst->params = tbsSrc->params;
		}

		/* TexEnv state */
		__GL_MEMCOPY(tusDst->env, tusSrc->env,
				    __GL_NUM_TEXTURE_ENV_BINDINGS
				    * sizeof(__GLtextureEnvState));

		/* texture enable state */
		sp->enables.texture[i] = gc->state.enables.texture[i];
	    }
	}
	if (mask & GL_TRANSFORM_BIT) {
	    size_t bytes = (size_t)
		(gc->constants.numberOfClipPlanes * sizeof(__GLcoord));
	    sp->transform.matrixMode = gc->state.transform.matrixMode;
	    sp->transform.eyeClipPlanes = (__GLcoord*)
		(*gc->imports.malloc)(gc, bytes);
	    __GL_MEMCOPY(sp->transform.eyeClipPlanes,
			 gc->state.transform.eyeClipPlanes, bytes);
	}
	if (mask & GL_VIEWPORT_BIT) {
	    sp->viewport = gc->state.viewport;
	}
    } else {
	__glSetError(GL_STACK_OVERFLOW);
	return;
    }
}

/************************************************************************/

void GLAPI __glim_PopAttrib(void)
{
    __GLattribute **spp;
    __GLattribute *sp;
    GLuint mask;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    spp = gc->attributes.stackPointer;
    if (spp > &gc->attributes.stack[0]) {
	--spp;
	sp = *spp;
	assert(sp != 0);
	mask = sp->mask;
	gc->attributes.stackPointer = spp;
	if (mask & GL_ACCUM_BUFFER_BIT) {
	    gc->state.accum = sp->accum;
	}
	if (mask & GL_COLOR_BUFFER_BIT) {
	    gc->state.raster = sp->raster;
	    gc->state.enables.general &= ~__GL_COLOR_BUFFER_ENABLES;
	    gc->state.enables.general |=
		sp->enables.general & __GL_COLOR_BUFFER_ENABLES;
	    gc->validateMask |= __GL_VALIDATE_ALPHA_FUNC; /*XXX*/
	}
	if (mask & GL_CURRENT_BIT) {
	    gc->state.current = sp->current;
	}
	if (mask & GL_DEPTH_BUFFER_BIT) {
	    gc->state.depth = sp->depth;
	    gc->state.enables.general &= ~__GL_DEPTH_TEST_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_DEPTH_TEST_ENABLE;
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);
	}
	if (mask & GL_ENABLE_BIT) {
	    gc->state.enables = sp->enables;
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE | __GL_DIRTY_POLYGON |
		    __GL_DIRTY_POINT | __GL_DIRTY_LIGHTING | __GL_DIRTY_DEPTH |
		    __GL_DIRTY_PIXEL | __GL_DIRTY_SCISSOR);
	    (*gc->procs.pickColorMaterialProcs)(gc);
	    (*gc->procs.applyColor)(gc);
	    (*gc->procs.computeClipBox)(gc);
	    (*gc->procs.applyScissor)(gc);
	}
	if (mask & GL_EVAL_BIT) {
	    gc->state.evaluator = sp->evaluator;
	    gc->state.enables.general &= ~__GL_AUTO_NORMAL_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_AUTO_NORMAL_ENABLE;
	    gc->state.enables.eval1 = sp->enables.eval1;
	    gc->state.enables.eval2 = sp->enables.eval2;
	}
	if (mask & GL_FOG_BIT) {
	    gc->state.fog = sp->fog;
	    gc->state.enables.general &= ~__GL_FOG_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_FOG_ENABLE;
	}
	if (mask & GL_HINT_BIT) {
	    gc->state.hints = sp->hints;
	}
	if (mask & GL_LIGHTING_BIT) {
	    gc->state.light.colorMaterialFace = sp->light.colorMaterialFace;
	    gc->state.light.colorMaterialParam = sp->light.colorMaterialParam;
	    gc->state.light.shadingModel = sp->light.shadingModel;
	    gc->state.light.model = sp->light.model;
	    gc->state.light.front = sp->light.front;
	    gc->state.light.back = sp->light.back;
	    __GL_MEMCOPY(gc->state.light.source, sp->light.source,
			 gc->constants.numberOfLights
			     * sizeof(__GLlightSourceState));
	    (*gc->imports.free)(gc, sp->light.source);
	    sp->light.source = NULL;
	    gc->state.enables.general &= ~__GL_LIGHTING_ENABLES;
	    gc->state.enables.general |=
		sp->enables.general & __GL_LIGHTING_ENABLES;
	    gc->state.enables.lights = sp->enables.lights;
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
	}
	if (mask & GL_LINE_BIT) {
	    gc->state.line = sp->line;
	    gc->state.enables.general &= ~__GL_LINE_ENABLES;
	    gc->state.enables.general |=
		sp->enables.general & __GL_LINE_ENABLES;
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
	}
	if (mask & GL_LIST_BIT) {
	    gc->state.list = sp->list;
	}
	if (mask & GL_PIXEL_MODE_BIT) {
	    GLint i;
	    gc->state.pixel.transferMode = sp->pixel.transferMode;
	    gc->state.pixel.readBufferReturn = sp->pixel.readBufferReturn;
	    gc->state.pixel.readBuffer = sp->pixel.readBuffer;
	    gc->state.enables.pixelPath = sp->enables.pixelPath;
	    for (i=0; i<__GL_NUM_COLOR_TABLE_TARGETS; ++i) {
	        *gc->state.pixel.colorTable[i] = *sp->pixel.colorTable[i];
		(*gc->imports.free)(gc, sp->pixel.colorTable[i]);
		sp->pixel.colorTable[i] = NULL;
	    }
	    for (i=0; i<__GL_NUM_CONVOLUTION_TARGETS; ++i) {
	        *gc->state.pixel.convolutionFilter[i] =
					*sp->pixel.convolutionFilter[i];
		(*gc->imports.free)(gc, sp->pixel.convolutionFilter[i]);
		sp->pixel.convolutionFilter[i] = NULL;
	    }
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
	}
	if (mask & GL_POINT_BIT) {
	    gc->state.point = sp->point;
	    gc->state.enables.general &= ~__GL_POINT_SMOOTH_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_POINT_SMOOTH_ENABLE;
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POINT);
	}
	if (mask & GL_POLYGON_BIT) {
	    gc->state.polygon = sp->polygon;
	    gc->state.enables.general &= ~__GL_POLYGON_ENABLES;
	    gc->state.enables.general |=
		sp->enables.general & __GL_POLYGON_ENABLES;
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	}
	if (mask & GL_POLYGON_STIPPLE_BIT) {
	    gc->state.polygonStipple = sp->polygonStipple;
	    gc->state.enables.general &= ~__GL_POLYGON_STIPPLE_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_POLYGON_STIPPLE_ENABLE;
	    (*gc->procs.convertPolygonStipple)(gc);
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	}
	if (mask & GL_SCISSOR_BIT) {
	    gc->state.scissor = sp->scissor;
	    gc->state.enables.general &= ~__GL_SCISSOR_TEST_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_SCISSOR_TEST_ENABLE;
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_SCISSOR);
	    (*gc->procs.computeClipBox)(gc);
	    (*gc->procs.applyScissor)(gc);
	}
	if (mask & GL_STENCIL_BUFFER_BIT) {
	    gc->state.stencil = sp->stencil;
	    gc->state.enables.general &= ~__GL_STENCIL_TEST_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_STENCIL_TEST_ENABLE;
	    gc->validateMask |= __GL_VALIDATE_STENCIL_FUNC |
		__GL_VALIDATE_STENCIL_OP;/*XXX*/
	}
	if (mask & GL_TEXTURE_BIT) {
	    GLint i, j;

	    /* active texture */
	    gc->state.texture.activeTexture = sp->texture.activeTexture;
	    gc->state.texture.active =
		    &gc->state.texture.unit[gc->state.texture.activeTexture];
	    gc->texture.active =
		    &gc->texture.unit[gc->state.texture.activeTexture];

	    /* texture unit state */
	    for (i=0; i < __GL_NUM_TEXTURE_UNITS; i++) {
		__GLtextureUnitState *tusSrc = &sp->texture.unit[i];
		__GLtextureUnitState *tusDst = &gc->state.texture.unit[i];

		/* TexGen state */
		tusDst->s = tusSrc->s;
		tusDst->t = tusSrc->t;
		tusDst->r = tusSrc->r;
		tusDst->q = tusSrc->q;

		/*
		** Texture binding state.
		** If the texture name is different, a new binding is
		** called for.	Deferring the binding is dangerous, because
		** the state before the pop has to be saved with the
		** texture that is being unbound.  If we defer the binding,
		** we need to watch out for cases like two pops in a row
		** or a pop followed by a bind.
		*/
		for (j=0; j < __GL_NUM_TEXTURE_BINDINGS; j++) {
		    __GLtextureBindingState *tbsSrc = tusSrc->texture[j];
		    __GLtextureBindingState *tbsDst = tusDst->texture[j];

		    if (tbsDst->name != tbsSrc->name) {
			__glBindTexture(gc, i, j, tbsSrc->name);
		    }
		    tbsDst = tusDst->texture[j];
		    assert(tbsDst->name == tbsSrc->name);
		    tbsDst->params = tbsSrc->params;

		    (*gc->imports.free)(gc, tbsSrc);
		    tusSrc->texture[j] = NULL;
		}

		/* TexEnv state */
		__GL_MEMCOPY(tusDst->env, tusSrc->env,
				    __GL_NUM_TEXTURE_ENV_BINDINGS
				    * sizeof(__GLtextureEnvState));

		/* texture enable state */
		gc->state.enables.texture[i] = sp->enables.texture[i];
	    }
	}
	if (mask & GL_TRANSFORM_BIT) {
	    gc->state.transform.matrixMode = sp->transform.matrixMode;
	    __GL_MEMCOPY(gc->state.transform.eyeClipPlanes,
			 sp->transform.eyeClipPlanes,
			 gc->constants.numberOfClipPlanes * sizeof(__GLcoord));
	    (*gc->imports.free)(gc, sp->transform.eyeClipPlanes);
	    sp->transform.eyeClipPlanes = NULL;
	    gc->state.enables.general &= ~__GL_NORMALIZE_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_NORMALIZE_ENABLE;
	    gc->state.enables.general &= ~__GL_RESCALE_NORMAL_ENABLE;
	    gc->state.enables.general |=
		sp->enables.general & __GL_RESCALE_NORMAL_ENABLE;
	    gc->state.enables.clipPlanes = sp->enables.clipPlanes;
	}
	if (mask & GL_VIEWPORT_BIT) {
	    gc->state.viewport = sp->viewport;
	    (*gc->procs.applyViewport)(gc);
	}

	/*
	** Clear out mask so that any memory frees done above won't get
	** re-done when the context is destroyed
	*/
	sp->mask = 0;

	__GL_DELAY_VALIDATE(gc);
    } else {
	__glSetError(GL_STACK_UNDERFLOW);
	return;
    }
}

/*
** Free any attribute state left on the stack.	Stop at the first
** zero in the array.
*/
void __glFreeAttributeState(__GLcontext *gc)
{
    __GLattribute *sp, **spp;
    GLint i, j;

    /*
    ** Need to pop all pushed attributes to free storage.
    ** Then it will be safe to delete stack entries
    */
    for (spp = &gc->attributes.stack[0];
	 spp < &gc->attributes.stack[gc->constants.maxAttribStackDepth];
	 spp++) {
	sp = *spp;
	if (sp) {
	    if (sp->light.source) {
		(*gc->imports.free)(gc, sp->light.source);
	    }
	    for (i=0; i < __GL_NUM_COLOR_TABLE_TARGETS; i++) {
		if (sp->pixel.colorTable[i]) {
		    (*gc->imports.free)(gc, sp->pixel.colorTable[i]);
		}
	    }
	    for (i=0; i < __GL_NUM_CONVOLUTION_TARGETS; i++) {
		if (sp->pixel.convolutionFilter[i]) {
		    (*gc->imports.free)(gc, sp->pixel.convolutionFilter[i]);
	        }
	    }
	    for (i=0; i < __GL_NUM_TEXTURE_UNITS; i++) {
		const __GLtextureUnitState *tus = &sp->texture.unit[i];

		for (j=0; j < __GL_NUM_TEXTURE_BINDINGS; j++) {
		    if (tus->texture[j]) {
		        (*gc->imports.free)(gc, tus->texture[j]);
		    }
		}
	    }
	    if (sp->transform.eyeClipPlanes) {
		(*gc->imports.free)(gc, sp->transform.eyeClipPlanes);
	    }
	    (*gc->imports.free)(gc, sp);
	} else {
	    break;
	}
    }
}

/************************************************************************/

void GLAPI __glim_Enable(GLenum cap)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (cap) {
      case GL_ALPHA_TEST:
	gc->state.enables.general |= __GL_ALPHA_TEST_ENABLE;
	break;
      case GL_AUTO_NORMAL:
	gc->state.enables.general |= __GL_AUTO_NORMAL_ENABLE;
	break;
      case GL_BLEND:
	gc->state.enables.general |= __GL_BLEND_ENABLE;
	break;
      case GL_COLOR_MATERIAL:
	gc->state.enables.general |= __GL_COLOR_MATERIAL_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
	(*gc->procs.pickColorMaterialProcs)(gc);
	(*gc->procs.applyColor)(gc);
	return;				/* NOTE: return! */
      case GL_CULL_FACE:
	if (gc->state.enables.general & __GL_CULL_FACE_ENABLE) return;
	gc->state.enables.general |= __GL_CULL_FACE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	return;
      case GL_DEPTH_TEST:
	gc->state.enables.general |= __GL_DEPTH_TEST_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);
	break;
      case GL_DITHER:
	gc->state.enables.general |= __GL_DITHER_ENABLE;
	break;
      case GL_FOG:
	gc->state.enables.general |= __GL_FOG_ENABLE;
	break;
      case GL_LIGHTING:
	gc->state.enables.general |= __GL_LIGHTING_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
	(*gc->procs.pickColorMaterialProcs)(gc);
	(*gc->procs.applyColor)(gc);
	return;
      case GL_LINE_SMOOTH:
	gc->state.enables.general |= __GL_LINE_SMOOTH_ENABLE;
	break;
      case GL_LINE_STIPPLE:
	gc->state.enables.general |= __GL_LINE_STIPPLE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
	return;
      case GL_INDEX_LOGIC_OP:
	gc->state.enables.general |= __GL_INDEX_LOGIC_OP_ENABLE;
	break;
      case GL_COLOR_LOGIC_OP:
	gc->state.enables.general |= __GL_COLOR_LOGIC_OP_ENABLE;
	break;
      case GL_NORMALIZE:
	gc->state.enables.general |= __GL_NORMALIZE_ENABLE;
	break;
      case GL_POINT_SMOOTH:
	gc->state.enables.general |= __GL_POINT_SMOOTH_ENABLE;
	break;
      case GL_POLYGON_SMOOTH:
	gc->state.enables.general |= __GL_POLYGON_SMOOTH_ENABLE;
	break;
      case GL_POLYGON_STIPPLE:
	gc->state.enables.general |= __GL_POLYGON_STIPPLE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	return;
      case GL_RESCALE_NORMAL:
	gc->state.enables.general |= __GL_RESCALE_NORMAL_ENABLE;
	break;
      case GL_SCISSOR_TEST:
	if (gc->state.enables.general & __GL_SCISSOR_TEST_ENABLE) return;
	gc->state.enables.general |= __GL_SCISSOR_TEST_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_SCISSOR);
	(*gc->procs.computeClipBox)(gc);
	(*gc->procs.applyScissor)(gc);
	break;
      case GL_STENCIL_TEST:
	gc->state.enables.general |= __GL_STENCIL_TEST_ENABLE;
	break;
      case GL_TEXTURE_1D:
	gc->state.enables.texture[gc->state.texture.activeTexture] |=
						__GL_TEXTURE_1D_ENABLE;
	break;
      case GL_TEXTURE_2D:
	gc->state.enables.texture[gc->state.texture.activeTexture] |=
						__GL_TEXTURE_2D_ENABLE;
	break;
      case GL_TEXTURE_3D:
	gc->state.enables.texture[gc->state.texture.activeTexture] |=
						__GL_TEXTURE_3D_ENABLE;
	break;
      case GL_TEXTURE_GEN_S:
	gc->state.enables.texture[gc->state.texture.activeTexture] |=
						__GL_TEXTURE_GEN_S_ENABLE;
	break;
      case GL_TEXTURE_GEN_T:
	gc->state.enables.texture[gc->state.texture.activeTexture] |=
						__GL_TEXTURE_GEN_T_ENABLE;
	break;
      case GL_TEXTURE_GEN_R:
	gc->state.enables.texture[gc->state.texture.activeTexture] |=
						__GL_TEXTURE_GEN_R_ENABLE;
	break;
      case GL_TEXTURE_GEN_Q:
	gc->state.enables.texture[gc->state.texture.activeTexture] |=
						__GL_TEXTURE_GEN_Q_ENABLE;
	break;

      case GL_CLIP_PLANE0: case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2: case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4: case GL_CLIP_PLANE5:
	cap -= GL_CLIP_PLANE0;
	gc->state.enables.clipPlanes |= (1 << cap);
	break;
      case GL_LIGHT0: case GL_LIGHT1:
      case GL_LIGHT2: case GL_LIGHT3:
      case GL_LIGHT4: case GL_LIGHT5:
      case GL_LIGHT6: case GL_LIGHT7:
	cap -= GL_LIGHT0;
	gc->state.enables.lights |= (1 << cap);
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
	return;
      case GL_MAP1_COLOR_4:
      case GL_MAP1_NORMAL:
      case GL_MAP1_INDEX:
      case GL_MAP1_TEXTURE_COORD_1: case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3: case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3: case GL_MAP1_VERTEX_4:
	cap = __GL_EVAL1D_INDEX(cap);
	gc->state.enables.eval1 |= (GLushort) (1 << cap);
	break;
      case GL_MAP2_COLOR_4:
      case GL_MAP2_NORMAL:
      case GL_MAP2_INDEX:
      case GL_MAP2_TEXTURE_COORD_1: case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3: case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3: case GL_MAP2_VERTEX_4:
	cap = __GL_EVAL2D_INDEX(cap);
	gc->state.enables.eval2 |= (GLushort) (1 << cap);
	break;
      case GL_POLYGON_OFFSET_POINT:
	gc->state.enables.general |= __GL_POLYGON_OFFSET_POINT_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POINT);
	break;
      case GL_POLYGON_OFFSET_LINE:
	gc->state.enables.general |= __GL_POLYGON_OFFSET_LINE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
	break;
      case GL_POLYGON_OFFSET_FILL:
	gc->state.enables.general |= __GL_POLYGON_OFFSET_FILL_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	break;
#if 0
      case GL_VERTEX_ARRAY_EXT:
	glEnableClientState(GL_VERTEX_ARRAY);
	return;
      case GL_NORMAL_ARRAY_EXT:
	glEnableClientState(GL_NORMAL_ARRAY);
	return;
      case GL_COLOR_ARRAY_EXT:
	glEnableClientState(GL_COLOR_ARRAY);
	return;
      case GL_INDEX_ARRAY_EXT:
	glEnableClientState(GL_INDEX_ARRAY);
	return;
      case GL_TEXTURE_COORD_ARRAY_EXT:
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
      case GL_EDGE_FLAG_ARRAY_EXT:
	glEnableClientState(GL_EDGE_FLAG_ARRAY);
	return;
#endif
#ifdef XXX_IS_CULLVERTEX_SUPPORTED
      case GL_CULL_VERTEX_EXT:
	gc->state.enables.general |= __GL_CULL_VERTEX_ENABLE;
	break;
#endif
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
      case GL_COLOR_TABLE:
	gc->state.enables.pixelPath |= __GL_COLOR_TABLE_ENABLE;
	break;
      case GL_POST_CONVOLUTION_COLOR_TABLE:
	gc->state.enables.pixelPath |=
	    __GL_POST_CONVOLUTION_COLOR_TABLE_ENABLE;
	break;
      case GL_POST_COLOR_MATRIX_COLOR_TABLE:
	gc->state.enables.pixelPath |=
	    __GL_POST_COLOR_MATRIX_COLOR_TABLE_ENABLE;
	break;
      case GL_CONVOLUTION_1D:
	gc->state.enables.pixelPath |= __GL_CONVOLUTION_1D_ENABLE;
	break;
      case GL_CONVOLUTION_2D:
	gc->state.enables.pixelPath |= __GL_CONVOLUTION_2D_ENABLE;
	break;
      case GL_SEPARABLE_2D:
	gc->state.enables.pixelPath |= __GL_SEPARABLE_2D_ENABLE;
	break;
      case GL_HISTOGRAM:
	gc->state.enables.pixelPath |= __GL_HISTOGRAM_ENABLE;
	break;
      case GL_MINMAX:
	gc->state.enables.pixelPath |= __GL_MINMAX_ENABLE;
	break;
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_Disable(GLenum cap)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (cap) {
      case GL_ALPHA_TEST:
	gc->state.enables.general &= ~__GL_ALPHA_TEST_ENABLE;
	break;
      case GL_AUTO_NORMAL:
	gc->state.enables.general &= ~__GL_AUTO_NORMAL_ENABLE;
	break;
      case GL_BLEND:
	gc->state.enables.general &= ~__GL_BLEND_ENABLE;
	break;
      case GL_COLOR_MATERIAL:
	gc->state.enables.general &= ~__GL_COLOR_MATERIAL_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
	(*gc->procs.pickColorMaterialProcs)(gc);
	return;				/* NOTE: return! */
      case GL_CULL_FACE:
	if (!(gc->state.enables.general & __GL_CULL_FACE_ENABLE)) return;
	gc->state.enables.general &= ~__GL_CULL_FACE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	return;
      case GL_DEPTH_TEST:
	gc->state.enables.general &= ~__GL_DEPTH_TEST_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);
	break;
      case GL_DITHER:
	gc->state.enables.general &= ~__GL_DITHER_ENABLE;
	break;
      case GL_FOG:
	gc->state.enables.general &= ~__GL_FOG_ENABLE;
	break;
      case GL_LIGHTING:
	gc->state.enables.general &= ~__GL_LIGHTING_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
	(*gc->procs.pickColorMaterialProcs)(gc);
	(*gc->procs.applyColor)(gc);
	return;
      case GL_LINE_SMOOTH:
	gc->state.enables.general &= ~__GL_LINE_SMOOTH_ENABLE;
	break;
      case GL_LINE_STIPPLE:
	gc->state.enables.general &= ~__GL_LINE_STIPPLE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
	return;
      case GL_INDEX_LOGIC_OP:
	gc->state.enables.general &= ~__GL_INDEX_LOGIC_OP_ENABLE;
	break;
      case GL_COLOR_LOGIC_OP:
	gc->state.enables.general &= ~__GL_COLOR_LOGIC_OP_ENABLE;
	break;
      case GL_NORMALIZE:
	gc->state.enables.general &= ~__GL_NORMALIZE_ENABLE;
	break;
      case GL_POINT_SMOOTH:
	gc->state.enables.general &= ~__GL_POINT_SMOOTH_ENABLE;
	break;
      case GL_POLYGON_SMOOTH:
	gc->state.enables.general &= ~__GL_POLYGON_SMOOTH_ENABLE;
	break;
      case GL_POLYGON_STIPPLE:
	gc->state.enables.general &= ~__GL_POLYGON_STIPPLE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	return;
      case GL_RESCALE_NORMAL:
	gc->state.enables.general &= ~__GL_RESCALE_NORMAL_ENABLE;
	break;
      case GL_SCISSOR_TEST:
	gc->state.enables.general &= ~__GL_SCISSOR_TEST_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_SCISSOR);
	(*gc->procs.computeClipBox)(gc);
	(*gc->procs.applyScissor)(gc);
	break;
      case GL_STENCIL_TEST:
	gc->state.enables.general &= ~__GL_STENCIL_TEST_ENABLE;
	break;
      case GL_TEXTURE_1D:
	gc->state.enables.texture[gc->state.texture.activeTexture] &=
						~__GL_TEXTURE_1D_ENABLE;
	break;
      case GL_TEXTURE_2D:
	gc->state.enables.texture[gc->state.texture.activeTexture] &=
						~__GL_TEXTURE_2D_ENABLE;
	break;
      case GL_TEXTURE_3D:
	gc->state.enables.texture[gc->state.texture.activeTexture] &=
						~__GL_TEXTURE_3D_ENABLE;
	break;
      case GL_TEXTURE_GEN_S:
	gc->state.enables.texture[gc->state.texture.activeTexture] &=
						~__GL_TEXTURE_GEN_S_ENABLE;
	break;
      case GL_TEXTURE_GEN_T:
	gc->state.enables.texture[gc->state.texture.activeTexture] &=
						~__GL_TEXTURE_GEN_T_ENABLE;
	break;
      case GL_TEXTURE_GEN_R:
	gc->state.enables.texture[gc->state.texture.activeTexture] &=
						~__GL_TEXTURE_GEN_R_ENABLE;
	break;
      case GL_TEXTURE_GEN_Q:
	gc->state.enables.texture[gc->state.texture.activeTexture] &=
						~__GL_TEXTURE_GEN_Q_ENABLE;
	break;

      case GL_CLIP_PLANE0: case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2: case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4: case GL_CLIP_PLANE5:
	cap -= GL_CLIP_PLANE0;
	gc->state.enables.clipPlanes &= ~(1 << cap);
	break;
      case GL_LIGHT0: case GL_LIGHT1:
      case GL_LIGHT2: case GL_LIGHT3:
      case GL_LIGHT4: case GL_LIGHT5:
      case GL_LIGHT6: case GL_LIGHT7:
	cap -= GL_LIGHT0;
	gc->state.enables.lights &= ~(1 << cap);
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
	return;
      case GL_MAP1_COLOR_4:
      case GL_MAP1_NORMAL:
      case GL_MAP1_INDEX:
      case GL_MAP1_TEXTURE_COORD_1: case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3: case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3: case GL_MAP1_VERTEX_4:
	cap = __GL_EVAL1D_INDEX(cap);
	gc->state.enables.eval1 &= (GLushort) ~(1 << cap);
	break;
      case GL_MAP2_COLOR_4:
      case GL_MAP2_NORMAL:
      case GL_MAP2_INDEX:
      case GL_MAP2_TEXTURE_COORD_1: case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3: case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3: case GL_MAP2_VERTEX_4:
	cap = __GL_EVAL2D_INDEX(cap);
	gc->state.enables.eval2 &= (GLushort) ~(1 << cap);
	break;
      case GL_POLYGON_OFFSET_POINT:
	gc->state.enables.general &= ~__GL_POLYGON_OFFSET_POINT_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POINT);
	break;
      case GL_POLYGON_OFFSET_LINE:
	gc->state.enables.general &= ~__GL_POLYGON_OFFSET_LINE_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LINE);
	break;
      case GL_POLYGON_OFFSET_FILL:
	gc->state.enables.general &= ~__GL_POLYGON_OFFSET_FILL_ENABLE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
	break;
#if 0
      case GL_VERTEX_ARRAY_EXT:
	glDisableClientState(GL_VERTEX_ARRAY);
	return;
      case GL_NORMAL_ARRAY_EXT:
	glDisableClientState(GL_NORMAL_ARRAY);
	return;
      case GL_COLOR_ARRAY_EXT:
	glDisableClientState(GL_COLOR_ARRAY);
	return;
      case GL_INDEX_ARRAY_EXT:
	glDisableClientState(GL_INDEX_ARRAY);
	return;
      case GL_TEXTURE_COORD_ARRAY_EXT:
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	return;
      case GL_EDGE_FLAG_ARRAY_EXT:
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	return;
#endif
#ifdef XXX_IS_CULLVERTEX_SUPPORTED
      case GL_CULL_VERTEX_EXT:
	gc->state.enables.general &= ~__GL_CULL_VERTEX_ENABLE;
	break;
#endif
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
      case GL_COLOR_TABLE:
	gc->state.enables.pixelPath &= ~__GL_COLOR_TABLE_ENABLE;
	break;
      case GL_POST_CONVOLUTION_COLOR_TABLE:
	gc->state.enables.pixelPath &=
	    ~__GL_POST_CONVOLUTION_COLOR_TABLE_ENABLE;
	break;
      case GL_POST_COLOR_MATRIX_COLOR_TABLE:
	gc->state.enables.pixelPath &=
	    ~__GL_POST_COLOR_MATRIX_COLOR_TABLE_ENABLE;
	break;
      case GL_CONVOLUTION_1D:
	gc->state.enables.pixelPath &= ~__GL_CONVOLUTION_1D_ENABLE;
	break;
      case GL_CONVOLUTION_2D:
	gc->state.enables.pixelPath &= ~__GL_CONVOLUTION_2D_ENABLE;
	break;
      case GL_SEPARABLE_2D:
	gc->state.enables.pixelPath &= ~__GL_SEPARABLE_2D_ENABLE;
	break;
      case GL_HISTOGRAM:
	gc->state.enables.pixelPath &= ~__GL_HISTOGRAM_ENABLE;
	break;
      case GL_MINMAX:
	gc->state.enables.pixelPath &= ~__GL_MINMAX_ENABLE;
	break;
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE(gc);
}

GLboolean GLAPI __glim_IsEnabled(GLenum cap)
{
    GLuint bit;
    __GL_SETUP_NOT_IN_BEGIN2();
    __GL_API_GET();

    switch (cap) {
      case GL_ALPHA_TEST:
	bit = gc->state.enables.general & __GL_ALPHA_TEST_ENABLE;
	break;
      case GL_AUTO_NORMAL:
	bit = gc->state.enables.general & __GL_AUTO_NORMAL_ENABLE;
	break;
      case GL_BLEND:
	bit = gc->state.enables.general & __GL_BLEND_ENABLE;
	break;
      case GL_COLOR_MATERIAL:
	bit = gc->state.enables.general & __GL_COLOR_MATERIAL_ENABLE;
	break;
      case GL_CULL_FACE:
	bit = gc->state.enables.general & __GL_CULL_FACE_ENABLE;
	break;
      case GL_DEPTH_TEST:
	bit = gc->state.enables.general & __GL_DEPTH_TEST_ENABLE;
	break;
      case GL_DITHER:
	bit = gc->state.enables.general & __GL_DITHER_ENABLE;
	break;
      case GL_FOG:
	bit = gc->state.enables.general & __GL_FOG_ENABLE;
	break;
      case GL_LIGHTING:
	bit = gc->state.enables.general & __GL_LIGHTING_ENABLE;
	break;
      case GL_LINE_SMOOTH:
	bit = gc->state.enables.general & __GL_LINE_SMOOTH_ENABLE;
	break;
      case GL_LINE_STIPPLE:
	bit = gc->state.enables.general & __GL_LINE_STIPPLE_ENABLE;
	break;
      case GL_INDEX_LOGIC_OP:
	bit = gc->state.enables.general & __GL_INDEX_LOGIC_OP_ENABLE;
	break;
      case GL_COLOR_LOGIC_OP:
	bit = gc->state.enables.general & __GL_COLOR_LOGIC_OP_ENABLE;
	break;
      case GL_NORMALIZE:
	bit = gc->state.enables.general & __GL_NORMALIZE_ENABLE;
	break;
      case GL_POINT_SMOOTH:
	bit = gc->state.enables.general & __GL_POINT_SMOOTH_ENABLE;
	break;
      case GL_POLYGON_SMOOTH:
	bit = gc->state.enables.general & __GL_POLYGON_SMOOTH_ENABLE;
	break;
      case GL_POLYGON_STIPPLE:
	bit = gc->state.enables.general & __GL_POLYGON_STIPPLE_ENABLE;
	break;
      case GL_RESCALE_NORMAL:
	bit = gc->state.enables.general & __GL_RESCALE_NORMAL_ENABLE;
	break;
      case GL_SCISSOR_TEST:
	bit = gc->state.enables.general & __GL_SCISSOR_TEST_ENABLE;
	break;
      case GL_STENCIL_TEST:
	bit = gc->state.enables.general & __GL_STENCIL_TEST_ENABLE;
	break;
      case GL_TEXTURE_1D:
	bit = gc->state.enables.texture[gc->state.texture.activeTexture] &
						__GL_TEXTURE_1D_ENABLE;
	break;
      case GL_TEXTURE_2D:
	bit = gc->state.enables.texture[gc->state.texture.activeTexture] &
						__GL_TEXTURE_2D_ENABLE;
	break;
      case GL_TEXTURE_3D:
	bit = gc->state.enables.texture[gc->state.texture.activeTexture] &
						__GL_TEXTURE_3D_ENABLE;
	break;
      case GL_TEXTURE_GEN_S:
	bit = gc->state.enables.texture[gc->state.texture.activeTexture] &
						__GL_TEXTURE_GEN_S_ENABLE;
	break;
      case GL_TEXTURE_GEN_T:
	bit = gc->state.enables.texture[gc->state.texture.activeTexture] &
						__GL_TEXTURE_GEN_T_ENABLE;
	break;
      case GL_TEXTURE_GEN_R:
	bit = gc->state.enables.texture[gc->state.texture.activeTexture] &
						__GL_TEXTURE_GEN_R_ENABLE;
	break;
      case GL_TEXTURE_GEN_Q:
	bit = gc->state.enables.texture[gc->state.texture.activeTexture] &
						__GL_TEXTURE_GEN_Q_ENABLE;
	break;

      case GL_CLIP_PLANE0: case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2: case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4: case GL_CLIP_PLANE5:
	cap -= GL_CLIP_PLANE0;
	bit = gc->state.enables.clipPlanes & (1 << cap);
	break;
      case GL_LIGHT0: case GL_LIGHT1:
      case GL_LIGHT2: case GL_LIGHT3:
      case GL_LIGHT4: case GL_LIGHT5:
      case GL_LIGHT6: case GL_LIGHT7:
	cap -= GL_LIGHT0;
	bit = gc->state.enables.lights & (1 << cap);
	break;
      case GL_MAP1_COLOR_4:
      case GL_MAP1_NORMAL:
      case GL_MAP1_INDEX:
      case GL_MAP1_TEXTURE_COORD_1: case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3: case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3: case GL_MAP1_VERTEX_4:
	cap = __GL_EVAL1D_INDEX(cap);
	bit = gc->state.enables.eval1 & (1 << cap);
	break;
      case GL_MAP2_COLOR_4:
      case GL_MAP2_NORMAL:
      case GL_MAP2_INDEX:
      case GL_MAP2_TEXTURE_COORD_1: case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3: case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3: case GL_MAP2_VERTEX_4:
	cap = __GL_EVAL2D_INDEX(cap);
	bit = gc->state.enables.eval2 & (1 << cap);
	break;
      case GL_POLYGON_OFFSET_POINT:
	bit = gc->state.enables.general & __GL_POLYGON_OFFSET_POINT_ENABLE;
	break;
      case GL_POLYGON_OFFSET_LINE:
	bit = gc->state.enables.general & __GL_POLYGON_OFFSET_LINE_ENABLE;
	break;
      case GL_POLYGON_OFFSET_FILL:
	bit = gc->state.enables.general & __GL_POLYGON_OFFSET_FILL_ENABLE;
	break;
      case GL_VERTEX_ARRAY:
	bit = gc->vertexArray.mask & VERTARRAY_V_MASK;
	break;
      case GL_NORMAL_ARRAY:
	bit = gc->vertexArray.mask & VERTARRAY_N_MASK;
	break;
      case GL_COLOR_ARRAY:
	bit = gc->vertexArray.mask & VERTARRAY_C_MASK;
	break;
      case GL_INDEX_ARRAY:
	bit = gc->vertexArray.mask & VERTARRAY_I_MASK;
	break;
      case GL_TEXTURE_COORD_ARRAY:
	bit = (gc->vertexArray.mask & VERTARRAY_T_MASK) &&
		(gc->vertexArray.textureEnables &
			(1<<gc->clientTexture.activeTexture));
	break;
      case GL_EDGE_FLAG_ARRAY:
	bit = gc->vertexArray.mask & VERTARRAY_E_MASK;
	break;
#ifdef XXX_IS_CULLVERTEX_SUPPORTED
      case GL_CULL_VERTEX_EXT:
	bit = gc->state.enables.general & __GL_CULL_VERTEX_ENABLE;
	break;
#endif
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
      case GL_COLOR_TABLE:
	bit = gc->state.enables.pixelPath &__GL_COLOR_TABLE_ENABLE;
	break;
      case GL_POST_CONVOLUTION_COLOR_TABLE:
	bit = gc->state.enables.pixelPath &
	    __GL_POST_CONVOLUTION_COLOR_TABLE_ENABLE;
	break;
      case GL_POST_COLOR_MATRIX_COLOR_TABLE:
	bit = gc->state.enables.pixelPath &
	    __GL_POST_COLOR_MATRIX_COLOR_TABLE_ENABLE;
	break;
      case GL_CONVOLUTION_1D:
	bit = gc->state.enables.pixelPath & __GL_CONVOLUTION_1D_ENABLE;
	break;
      case GL_CONVOLUTION_2D:
	bit = gc->state.enables.pixelPath & __GL_CONVOLUTION_2D_ENABLE;
	break;
      case GL_SEPARABLE_2D:
	bit = gc->state.enables.pixelPath & __GL_SEPARABLE_2D_ENABLE;
	break;
      case GL_HISTOGRAM:
	bit = gc->state.enables.pixelPath & __GL_HISTOGRAM_ENABLE;
	break;
      case GL_MINMAX:
	bit = gc->state.enables.pixelPath & __GL_MINMAX_ENABLE;
	break;
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
      default:
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
    }
    return bit != 0;
}

/*
** OpenGL 1.1 entry points
*/

void GLAPI __glim_PolygonOffset(GLfloat factor, GLfloat units)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    gc->state.polygon.factor = factor;
    gc->state.polygon.units = units;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_POLYGON);
}

void GLAPI __glim_PushClientAttrib(GLbitfield mask)
{
    __GLclientAttribute **spp;
    __GLclientAttribute *sp;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    spp = gc->attributes.clientStackPointer;
    if (spp < &gc->attributes.clientStack[gc->constants.maxClientAttribStackDepth]) {
	if (!(sp = *spp)) {
	    sp = (__GLclientAttribute *)
		(*gc->imports.calloc)(gc, 1, sizeof(__GLclientAttribute));
	    *spp = sp;
	}
	sp->mask = mask;
	gc->attributes.clientStackPointer = spp + 1;
	if (mask & GL_CLIENT_PIXEL_STORE_BIT) {
	    sp->clientPixel = gc->clientPixel;
	}
	if (mask & GL_CLIENT_VERTEX_ARRAY_BIT) {
	    sp->vertexArray = gc->vertexArray;
	    sp->clientTexture = gc->clientTexture;
	}
    } else {
	__glSetError(GL_STACK_OVERFLOW);
	return;
    }
}

void GLAPI __glim_PopClientAttrib(void)
{
    __GLclientAttribute **spp;
    __GLclientAttribute *sp;
    GLuint mask;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    spp = gc->attributes.clientStackPointer;
    if (spp > &gc->attributes.clientStack[0]) {
	--spp;
	sp = *spp;
	assert(sp != 0);
	mask = sp->mask;
	gc->attributes.clientStackPointer = spp;
	if (mask & GL_CLIENT_PIXEL_STORE_BIT) {
	    gc->clientPixel = sp->clientPixel;
	}
	if (mask & GL_CLIENT_VERTEX_ARRAY_BIT) {
	    GLint i;
	    /*
	    ** XXX we don't want to overwrite everything! But there
	    ** must be a faster way than this.
	    */
	    gc->vertexArray.vp = sp->vertexArray.vp;
	    gc->vertexArray.np = sp->vertexArray.np;
	    gc->vertexArray.cp = sp->vertexArray.cp;
	    gc->vertexArray.ip = sp->vertexArray.ip;
	    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
		gc->vertexArray.tp[i] = sp->vertexArray.tp[i];
	    }
	    gc->vertexArray.ep = sp->vertexArray.ep;

	    gc->vertexArray.textureEnables = sp->vertexArray.textureEnables;
	    gc->vertexArray.signature = sp->vertexArray.signature;
	    gc->vertexArray.mask = sp->vertexArray.mask;
	    gc->vertexArray.index = sp->vertexArray.index;

	    gc->vertexArray.interleavedPointer =
		sp->vertexArray.interleavedPointer;
	    gc->vertexArray.interleavedFormat =
		sp->vertexArray.interleavedFormat;
	    gc->vertexArray.controlWord = sp->vertexArray.controlWord;

	    gc->clientTexture = sp->clientTexture;
	}

	/*
	** Clear out mask so that any memory frees done above won't get
	** re-done when the context is destroyed
	*/
	sp->mask = 0;

	__GL_DELAY_VALIDATE(gc);
    } else {
	__glSetError(GL_STACK_UNDERFLOW);
	return;
    }
}

void GLAPI __glim_EnableClientState(GLenum mode)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch(mode) {
    case GL_VERTEX_ARRAY:
	gc->vertexArray.mask |= VERTARRAY_V_MASK;
	gc->vertexArray.index |= VERTARRAY_V_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_NORMAL_ARRAY:
	gc->vertexArray.mask |= VERTARRAY_N_MASK;
	gc->vertexArray.index |= VERTARRAY_N_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_COLOR_ARRAY:
	gc->vertexArray.mask |= VERTARRAY_C_MASK;
	gc->vertexArray.index |= VERTARRAY_C_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_INDEX_ARRAY:
	gc->vertexArray.mask |= VERTARRAY_I_MASK;
	gc->vertexArray.index |= VERTARRAY_I_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_TEXTURE_COORD_ARRAY:
        gc->vertexArray.textureEnables |= (1<<gc->clientTexture.activeTexture);
	gc->vertexArray.mask |= VERTARRAY_T_MASK;
	gc->vertexArray.index |= VERTARRAY_T_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_EDGE_FLAG_ARRAY:
	gc->vertexArray.mask |= VERTARRAY_E_MASK;
	gc->vertexArray.index |= VERTARRAY_E_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_DisableClientState(GLenum mode)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch(mode) {
    case GL_VERTEX_ARRAY:
	gc->vertexArray.mask &= ~VERTARRAY_V_MASK;
	gc->vertexArray.index &= ~VERTARRAY_V_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_NORMAL_ARRAY:
	gc->vertexArray.mask &= ~VERTARRAY_N_MASK;
	gc->vertexArray.index &= ~VERTARRAY_N_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_COLOR_ARRAY:
	gc->vertexArray.mask &= ~VERTARRAY_C_MASK;
	gc->vertexArray.index &= ~VERTARRAY_C_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_INDEX_ARRAY:
	gc->vertexArray.mask &= ~VERTARRAY_I_MASK;
	gc->vertexArray.index &= ~VERTARRAY_I_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_TEXTURE_COORD_ARRAY:
        gc->vertexArray.textureEnables &= ~(1<<gc->clientTexture.activeTexture);
	if (gc->vertexArray.textureEnables == 0) {
	    gc->vertexArray.mask &= ~VERTARRAY_T_MASK;
	    gc->vertexArray.index &= ~VERTARRAY_T_INDEX;
	}
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    case GL_EDGE_FLAG_ARRAY:
	gc->vertexArray.mask &= ~VERTARRAY_E_MASK;
	gc->vertexArray.index &= ~VERTARRAY_E_INDEX;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
	return;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/*
** EXT_cull_vertex entry points
*/

#ifdef XXX_IS_CULLVERTEX_SUPPORTED

void GLAPI __glim_CullParameterfvEXT(GLenum pname, GLfloat *params)
{
    __GLcoord *pEye;
    __GL_SETUP_NOT_IN_BEGIN();

    __GL_API_STATE();

    switch(pname) {
    case GL_CULL_VERTEX_EYE_POSITION_EXT:
	pEye = &gc->state.transform.eyePos;
	gc->state.transform.objEyeSpecified = GL_FALSE;
	break;
    case GL_CULL_VERTEX_OBJECT_POSITION_EXT:
	pEye = &gc->state.transform.eyePosObj;
	gc->state.transform.objEyeSpecified = GL_TRUE;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    pEye->x = params[0];
    pEye->y = params[1];
    pEye->z = params[2];
    pEye->w = params[3];
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_CullParameterdvEXT(GLenum pname, GLdouble *params)
{
    GLfloat floatParams[4];
    int i, numParams = 0;

    switch(pname) {
    case GL_CULL_VERTEX_EYE_POSITION_EXT:
    case GL_CULL_VERTEX_OBJECT_POSITION_EXT:
	numParams = 4;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    for (i=0; i<numParams; i++) floatParams[i] = (GLfloat) params[i];
    __glim_CullParameterfvEXT(pname, floatParams);
}

#endif /*XXX_IS_CULLVERTEX_SUPPORTED*/
