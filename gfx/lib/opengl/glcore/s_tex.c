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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_tex.c#31 $
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
#include "texmgr.h"

/*
** Some math routines that are optimized in assembly
*/
#if defined(__GL_USE_ASMCODE) && defined(mips)
#define __GL_FRAC(f)	__glFrac(f)
#else /* __GL_USE_ASMCODE && mips */
#define __GL_FRAC(f)	((f) - __GL_FLOORF(f))
#endif /* __GL_USE_ASMCODE && mips */

/************************************************************************/

void GLAPI
__glim_ActiveTextureARB(GLenum texture)
{
    GLint unit;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    unit = (int) texture - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.texture.activeTexture = unit;

    /* Update unit pointers */
    gc->state.texture.active = &gc->state.texture.unit[unit];
    gc->texture.active = &gc->texture.unit[unit];
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI
__glim_ClientActiveTextureARB(GLenum texture)
{
    GLint unit;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    unit = (int) texture - GL_TEXTURE0_ARB;
    if (unit < 0 || gc->constants.maxTextureUnits <= unit) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->clientTexture.activeTexture = unit;
    __GL_DELAY_VALIDATE(gc);
}

/*
** Return the binding associated with a texture binding index.
*/
GLenum
__glTextureTargetFromIndex(int index)
{
    static const GLenum __glTextureBinding[__GL_NUM_TEXTURE_BINDINGS] = {
	GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
    };

    if (index < 0 || __GL_NUM_TEXTURE_BINDINGS <= index) {
	assert(0);
	return 0;
    }
    return __glTextureBinding[index];
}

/*
** Return the binding associated with a texture binding index.
*/
GLenum
__glProxyTextureTargetFromIndex(int index)
{
    static const GLenum __glTextureBinding[__GL_NUM_TEXTURE_BINDINGS] = {
	GL_PROXY_TEXTURE_1D, GL_PROXY_TEXTURE_2D, GL_PROXY_TEXTURE_3D,
    };

    if (index < 0 || __GL_NUM_TEXTURE_BINDINGS <= index) {
	assert(0);
	return 0;
    }
    return __glTextureBinding[index];
}

/*
** Returns a pointer to the texture parameter state for the specified target.
** Returns 0 if the target is not valid.
*/
__GLtextureParamState *
__glLookupActiveTextureParams(__GLcontext *gc, GLenum target)
{
    switch (target) {
      case GL_TEXTURE_1D:
	return &gc->texture.active->texture[__GL_TEXTURE_1D_INDEX]->state.params;
      case GL_TEXTURE_2D:
	return &gc->texture.active->texture[__GL_TEXTURE_2D_INDEX]->state.params;
      case GL_TEXTURE_3D:
	return &gc->texture.active->texture[__GL_TEXTURE_3D_INDEX]->state.params;
      default:
	return 0;
    }
}

/*
** Returns a pointer to the texture object state for the specified target.
** Returns 0 if the target is not valid.
*/
__GLtexture *
__glLookupActiveTexture(__GLcontext *gc, GLenum target)
{
    switch (target) {
      case GL_TEXTURE_1D:
	return gc->texture.active->texture[__GL_TEXTURE_1D_INDEX];
      case GL_TEXTURE_2D:
	return gc->texture.active->texture[__GL_TEXTURE_2D_INDEX];
      case GL_TEXTURE_3D:
	return gc->texture.active->texture[__GL_TEXTURE_3D_INDEX];

      case GL_PROXY_TEXTURE_1D:
	return gc->texture.proxyTexture[__GL_TEXTURE_1D_INDEX];
      case GL_PROXY_TEXTURE_2D:
	return gc->texture.proxyTexture[__GL_TEXTURE_2D_INDEX];
      case GL_PROXY_TEXTURE_3D:
	return gc->texture.proxyTexture[__GL_TEXTURE_3D_INDEX];
      default:
	return 0;
    }
}

/*
** Returns a pointer to the texture parameter state for the specified target.
** Returns 0 if the target is not valid.
*/
__GLtextureParamState *
__glLookupTextureParams(__GLcontext *gc, GLint unit, GLenum target)
{
    __GLtextureUnitMachine *tu = &gc->texture.unit[unit];

    switch (target) {
      case GL_TEXTURE_1D:
	return &tu->texture[__GL_TEXTURE_1D_INDEX]->state.params;
      case GL_TEXTURE_2D:
	return &tu->texture[__GL_TEXTURE_2D_INDEX]->state.params;
      case GL_TEXTURE_3D:
	return &tu->texture[__GL_TEXTURE_3D_INDEX]->state.params;
      default:
	return 0;
    }
}

/*
** Returns a pointer to the texture object state for the specified target.
** Returns 0 if the target is not valid.
*/
__GLtexture *
__glLookupTexture(__GLcontext *gc, GLint unit, GLenum target)
{
    __GLtextureUnitMachine *tu = &gc->texture.unit[unit];

    switch (target) {
      case GL_TEXTURE_1D:
	return tu->texture[__GL_TEXTURE_1D_INDEX];
      case GL_TEXTURE_2D:
	return tu->texture[__GL_TEXTURE_2D_INDEX];
      case GL_TEXTURE_3D:
	return tu->texture[__GL_TEXTURE_3D_INDEX];
      default:
	return 0;
    }
}

/************************************************************************/

static GLfloat Clampf(GLfloat fval, __GLfloat min, __GLfloat max)
{
    if (fval < min) { return min; }
    else if (fval > max) { return max; }
    else { return fval; }
}

/************************************************************************/

/*
** Used to share texture objects between two different contexts.
*/
void __glShareTextureObjects(__GLcontext *dst, __GLcontext *src)
{
    /* First get rid of our private texture object state */
    dst->texture.namesArray->refcount--;
    if (dst->texture.namesArray->refcount == 0) {
	__glNamesFreeArray(dst, dst->texture.namesArray);
    }
    dst->texture.namesArray = src->texture.namesArray;
    dst->texture.namesArray->refcount++;
}

void __glEarlyInitTextureState(__GLcontext *gc)
{
    GLint i;

    /* Initialize the texture manager */
    (*gc->texture.initTextureManager)(gc);

    /* XXX Override device dependent values */
    gc->constants.maxTextureSize = 1 << (gc->constants.maxMipMapLevel - 1);
    gc->constants.maxTexture3DSize = 1 << (gc->constants.maxMipMapLevel - 1);

    gc->state.texture.active = &gc->state.texture.unit[0];
    gc->texture.active = &gc->texture.unit[0];

    /*
    ** Init texture object structures, unless we're sharing texture
    ** objects with another context, in which case namesArray will
    ** already have been initialized.
    */
    if (NULL == gc->texture.namesArray) {
	gc->texture.namesArray = __glNamesNewArray(gc, __GL_NAMES_TEXOBJ);
	assert(NULL != gc->texture.namesArray);
    }

    /*
    ** Set up the texture objects for the default and proxy textures.
    ** Because the default and proxy textures are not shared, they are
    ** not hung off of the namesArray structure.
    */
    for (i=0; i < __GL_NUM_TEXTURE_BINDINGS; i++) {
	__GLtexture *tex;

	tex = (*gc->texture.createTexture)(gc, 0/*name*/,
					__glTextureTargetFromIndex(i));
	assert(tex->state.name == 0);
	assert(tex->refcount == 1);
	gc->texture.defaultTexture[i] = tex;

	tex = (*gc->texture.createTexture)(gc, 0/*name*/,
					__glProxyTextureTargetFromIndex(i));
	assert(tex->state.name == 0);
	assert(tex->refcount == 1);
	gc->texture.proxyTexture[i] = tex;
    }
}

void __glInitTextureState(__GLcontext *gc)
{
    GLint i, j;

    /* Init texture unit state */
    for (i=0; i < __GL_NUM_TEXTURE_UNITS; i++) {
	__GLtextureUnitState *tus = &gc->state.texture.unit[i];
	__GLtextureUnitMachine *tu = &gc->texture.unit[i];

	gc->state.current.texture[i].w = __glOne;

	/* Set up default bindings */
	for (j=0; j < __GL_NUM_TEXTURE_BINDINGS; j++) {
	    __glBindTexture(gc, i, j, 0);
	}

	/* Init each texture environment state */
	for (j = 0; j < __GL_NUM_TEXTURE_ENV_BINDINGS; j++) {
	    __GLtextureEnvState *tes = &tus->env[j];

	    tes->mode = GL_MODULATE;
	}

	/* Init rest of texture state */
	tus->s.mode = GL_EYE_LINEAR;
	tus->s.eyePlaneEquation.x = __glOne;
	tus->s.objectPlaneEquation.x = __glOne;
	tus->t.mode = GL_EYE_LINEAR;
	tus->t.eyePlaneEquation.y = __glOne;
	tus->t.objectPlaneEquation.y = __glOne;
	tus->r.mode = GL_EYE_LINEAR;
	tus->q.mode = GL_EYE_LINEAR;
    }

}

void __glFreeTextureState(__GLcontext *gc)
{
    GLint i, j;

    /* Unbind all non-default texture objects */
    for (i=0; i < __GL_NUM_TEXTURE_UNITS; i++) {
	for (j=0; j < __GL_NUM_TEXTURE_BINDINGS; j++) {
	    __glBindTexture(gc, i, j, 0);
	}
    }

    /* Free default and proxy texture objects */
    for (i=0; i < __GL_NUM_TEXTURE_BINDINGS; i++) {
	__GLtexture *tex;

	tex = gc->texture.defaultTexture[i];
	tex->refcount--;
	assert(tex->refcount == 0);
	(*tex->free)(gc, tex);
	gc->texture.defaultTexture[i] = NULL;

	tex = gc->texture.proxyTexture[i];
	tex->refcount--;
	assert(tex->refcount == 0);
	(*tex->free)(gc, tex);
	gc->texture.proxyTexture[i] = NULL;
    }

    /* Free texture object structures */
    gc->texture.namesArray->refcount--;
    if (gc->texture.namesArray->refcount == 0) {
	__glNamesFreeArray(gc, gc->texture.namesArray);
    }
    gc->texture.namesArray = NULL;

    /* Free the texture manager */
    (*gc->texture.textureManager->free)(gc);
}

/************************************************************************/

void GLAPI __glim_TexGenfv(GLenum coord, GLenum pname, const GLfloat pv[])
{
    __GLtextureGenState *tcs;
    __GLfloat v[4];
    __GLtransform *tr;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (coord) {
      case GL_S: tcs = &gc->state.texture.active->s; break;
      case GL_T: tcs = &gc->state.texture.active->t; break;
      case GL_R: tcs = &gc->state.texture.active->r; break;
      case GL_Q: tcs = &gc->state.texture.active->q; break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	switch ((GLenum) pv[0]) {
	  case GL_EYE_LINEAR:
	  case GL_OBJECT_LINEAR:
	    tcs->mode = (GLenum) pv[0];
	    break;
	  case GL_SPHERE_MAP:
	    if ((coord == GL_R) || (coord == GL_Q)) {
		__glSetError(GL_INVALID_ENUM);
		return;
	    }
	    tcs->mode = (GLenum) pv[0];
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_OBJECT_PLANE:
	tcs->objectPlaneEquation.x = pv[0];
	tcs->objectPlaneEquation.y = pv[1];
	tcs->objectPlaneEquation.z = pv[2];
	tcs->objectPlaneEquation.w = pv[3];
	break;
      case GL_EYE_PLANE:
	/*XXX transform should not be in generic code */
	v[0] = pv[0]; v[1] = pv[1]; v[2] = pv[2]; v[3] = pv[3];
	tr = gc->transform.modelView;
	if (tr->updateInverse) {
	    (*gc->procs.computeInverseTranspose)(gc, tr);
	}
	(*tr->inverseTranspose.xf4)(&tcs->eyePlaneEquation, v,
				    &tr->inverseTranspose);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexGenf(GLenum coord, GLenum pname, GLfloat f)
{
    /* Accept only enumerants that correspond to single values */
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	__glim_TexGenfv(coord, pname, &f);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_TexGendv(GLenum coord, GLenum pname, const GLdouble pv[])
{
    __GLtextureGenState *tcs;
    __GLfloat v[4];
    __GLtransform *tr;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (coord) {
      case GL_S: tcs = &gc->state.texture.active->s; break;
      case GL_T: tcs = &gc->state.texture.active->t; break;
      case GL_R: tcs = &gc->state.texture.active->r; break;
      case GL_Q: tcs = &gc->state.texture.active->q; break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	switch ((GLenum) pv[0]) {
	  case GL_EYE_LINEAR:
	  case GL_OBJECT_LINEAR:
	    tcs->mode = (GLenum) pv[0];
	    break;
	  case GL_SPHERE_MAP:
	    if ((coord == GL_R) || (coord == GL_Q)) {
		__glSetError(GL_INVALID_ENUM);
		return;
	    }
	    tcs->mode = (GLenum) pv[0];
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_OBJECT_PLANE:
	tcs->objectPlaneEquation.x = pv[0];
	tcs->objectPlaneEquation.y = pv[1];
	tcs->objectPlaneEquation.z = pv[2];
	tcs->objectPlaneEquation.w = pv[3];
	break;
      case GL_EYE_PLANE:
	/*XXX transform should not be in generic code */
	v[0] = pv[0]; v[1] = pv[1]; v[2] = pv[2]; v[3] = pv[3];
	tr = gc->transform.modelView;
	if (tr->updateInverse) {
	    (*gc->procs.computeInverseTranspose)(gc, tr);
	}
	(*tr->inverseTranspose.xf4)(&tcs->eyePlaneEquation, v,
				    &tr->inverseTranspose);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexGend(GLenum coord, GLenum pname, GLdouble d)
{
    /* Accept only enumerants that correspond to single values */
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	__glim_TexGendv(coord, pname, &d);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_TexGeniv(GLenum coord, GLenum pname, const GLint pv[])
{
    __GLtextureGenState *tcs;
    __GLfloat v[4];
    __GLtransform *tr;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    switch (coord) {
      case GL_S: tcs = &gc->state.texture.active->s; break;
      case GL_T: tcs = &gc->state.texture.active->t; break;
      case GL_R: tcs = &gc->state.texture.active->r; break;
      case GL_Q: tcs = &gc->state.texture.active->q; break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	switch ((GLenum) pv[0]) {
	  case GL_EYE_LINEAR:
	  case GL_OBJECT_LINEAR:
	    tcs->mode = (GLenum) pv[0];
	    break;
	  case GL_SPHERE_MAP:
	    if ((coord == GL_R) || (coord == GL_Q)) {
		__glSetError(GL_INVALID_ENUM);
		return;
	    }
	    tcs->mode = (GLenum) pv[0];
	    break;
	  default:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_OBJECT_PLANE:
	tcs->objectPlaneEquation.x = pv[0];
	tcs->objectPlaneEquation.y = pv[1];
	tcs->objectPlaneEquation.z = pv[2];
	tcs->objectPlaneEquation.w = pv[3];
	break;
      case GL_EYE_PLANE:
	/*XXX transform should not be in generic code */
	v[0] = pv[0]; v[1] = pv[1]; v[2] = pv[2]; v[3] = pv[3];
	tr = gc->transform.modelView;
	if (tr->updateInverse) {
	    (*gc->procs.computeInverseTranspose)(gc, tr);
	}
	(*tr->inverseTranspose.xf4)(&tcs->eyePlaneEquation, v,
				    &tr->inverseTranspose);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexGeni(GLenum coord, GLenum pname, GLint i)
{
    /* Accept only enumerants that correspond to single values */
    switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	__glim_TexGeniv(coord, pname, &i);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

GLint __glTexGendv_size(GLenum e)
{
    switch (e) {
      case GL_TEXTURE_GEN_MODE:
	return 1;
      case GL_OBJECT_PLANE:
      case GL_EYE_PLANE:
	return 4;
      default:
	return -1;
    }
}

GLint __glTexGenfv_size(GLenum e)
{
    return __glTexGendv_size(e);
}

GLint __glTexGeniv_size(GLenum e)
{
    return __glTexGendv_size(e);
}

/************************************************************************/

void GLAPI __glim_TexParameterfv(GLenum target, GLenum pname, const GLfloat pv[])
{
    GLenum e;
    __GLtexture *tex;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    tex = __glLookupActiveTexture(gc, target);
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    switch (pname) {
      case GL_TEXTURE_WRAP_S:
	switch (e = (GLenum) pv[0]) {
	  case GL_REPEAT:
	  case GL_CLAMP:
	  case GL_CLAMP_TO_EDGE:     
	    tex->state.params.sWrapMode = e;
	    break;
	  default:
	    bad_enum:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_TEXTURE_WRAP_T:
	switch (e = (GLenum) pv[0]) {
	  case GL_REPEAT:
	  case GL_CLAMP:
	  case GL_CLAMP_TO_EDGE:     
	    tex->state.params.tWrapMode = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_WRAP_R:
	switch (e = (GLenum) pv[0]) {
	  case GL_REPEAT:
	  case GL_CLAMP:
	  case GL_CLAMP_TO_EDGE:     
	    tex->state.params.rWrapMode = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_MIN_FILTER:
	switch (e = (GLenum) pv[0]) {
	  case GL_NEAREST:
	  case GL_LINEAR:
	  case GL_NEAREST_MIPMAP_NEAREST:
	  case GL_LINEAR_MIPMAP_NEAREST:
	  case GL_NEAREST_MIPMAP_LINEAR:
	  case GL_LINEAR_MIPMAP_LINEAR:
	    tex->state.params.minFilter = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_MAG_FILTER:
	switch (e = (GLenum) pv[0]) {
	  case GL_NEAREST:
	  case GL_LINEAR:
	    tex->state.params.magFilter = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_BORDER_COLOR:
	__glClampColorf(gc, &tex->state.params.borderColor, pv);
	break;

      case GL_TEXTURE_PRIORITY:
	tex->state.params.priority = Clampf(pv[0], __glZero, __glOne);
	break;

      case GL_TEXTURE_MIN_LOD:
	tex->state.params.minLOD = pv[0];
	if (__GL_ABSF(tex->state.params.minLOD) > 63.0F) {
	    if (tex->state.params.minLOD > __glZero) {
		tex->adjustedMinLOD = __GL_MAX_FLOAT;
	    } else {
		tex->adjustedMinLOD = -__GL_MAX_FLOAT;
	    }
	} else {
	    tex->adjustedMinLOD = __GL_POWF(2.0F, tex->state.params.minLOD);
	    tex->adjustedMinLOD *= tex->adjustedMinLOD;
	}
	break;

      case GL_TEXTURE_MAX_LOD:
	tex->state.params.maxLOD = pv[0];
	if (__GL_ABSF(tex->state.params.maxLOD) > 63.0F) {
	    if (tex->state.params.maxLOD > __glZero) {
		tex->adjustedMaxLOD = __GL_MAX_FLOAT;
	    } else {
		tex->adjustedMaxLOD = -__GL_MAX_FLOAT;
	    }
	} else {
	    tex->adjustedMaxLOD = __GL_POWF(2.0F, tex->state.params.maxLOD);
	    tex->adjustedMaxLOD *= tex->adjustedMaxLOD;
	}
	break;

      case GL_TEXTURE_BASE_LEVEL:
	if ((GLint)pv[0] < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	tex->state.params.baseLevel = (GLint)pv[0];
	break;

      case GL_TEXTURE_MAX_LEVEL:
	if ((GLint)pv[0] < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	tex->state.params.maxLevel = (GLint)pv[0];
	break;

      default:
	goto bad_enum;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexParameterf(GLenum target, GLenum pname, GLfloat f)
{
    /* Accept only enumerants that correspond to single values */
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
      case GL_TEXTURE_WRAP_T:
      case GL_TEXTURE_WRAP_R:
      case GL_TEXTURE_MIN_FILTER:
      case GL_TEXTURE_MAG_FILTER:
      case GL_TEXTURE_PRIORITY:
      case GL_TEXTURE_MIN_LOD:
      case GL_TEXTURE_MAX_LOD:
      case GL_TEXTURE_BASE_LEVEL:
      case GL_TEXTURE_MAX_LEVEL:
	__glim_TexParameterfv(target, pname, &f);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_TexParameteriv(GLenum target, GLenum pname, const GLint pv[])
{
    GLenum e;
    __GLtexture *tex;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    tex = __glLookupActiveTexture(gc, target);
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    switch (pname) {
      case GL_TEXTURE_WRAP_S:
	switch (e = (GLenum) pv[0]) {
	  case GL_REPEAT:
	  case GL_CLAMP:
	  case GL_CLAMP_TO_EDGE:     
	    tex->state.params.sWrapMode = e;
	    break;
	  default:
	    bad_enum:
	    __glSetError(GL_INVALID_ENUM);
	    return;
	}
	break;
      case GL_TEXTURE_WRAP_T:
	switch (e = (GLenum) pv[0]) {
	  case GL_REPEAT:
	  case GL_CLAMP:
	  case GL_CLAMP_TO_EDGE:     
	    tex->state.params.tWrapMode = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_WRAP_R:
	switch (e = (GLenum) pv[0]) {
	  case GL_REPEAT:
	  case GL_CLAMP:
	  case GL_CLAMP_TO_EDGE:     
	    tex->state.params.rWrapMode = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_MIN_FILTER:
	switch (e = (GLenum) pv[0]) {
	  case GL_NEAREST:
	  case GL_LINEAR:
	  case GL_NEAREST_MIPMAP_NEAREST:
	  case GL_LINEAR_MIPMAP_NEAREST:
	  case GL_NEAREST_MIPMAP_LINEAR:
	  case GL_LINEAR_MIPMAP_LINEAR:
	    tex->state.params.minFilter = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_MAG_FILTER:
	switch (e = (GLenum) pv[0]) {
	  case GL_NEAREST:
	  case GL_LINEAR:
	    tex->state.params.magFilter = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_BORDER_COLOR:
	__glClampColori(gc, &tex->state.params.borderColor, pv);
	break;
      case GL_TEXTURE_PRIORITY:
	{
	    __GLfloat priority;

	    priority = __GL_I_TO_FLOAT(pv[0]);
	    tex->state.params.priority = Clampf(priority, __glZero, __glOne);
	}
	break;

      case GL_TEXTURE_MIN_LOD:
	tex->state.params.minLOD = (__GLfloat)pv[0];
	if (__GL_ABSF(tex->state.params.minLOD) > 63.0F) {
	    if (tex->state.params.minLOD > __glZero) {
		tex->adjustedMinLOD = __GL_MAX_FLOAT;
	    } else {
		tex->adjustedMinLOD = -__GL_MAX_FLOAT;
	    }
	} else {
	    tex->adjustedMinLOD = __GL_POWF(2.0, tex->state.params.minLOD);
	    tex->adjustedMinLOD *= tex->adjustedMinLOD;
	}
	break;
      case GL_TEXTURE_MAX_LOD:
	tex->state.params.maxLOD = (__GLfloat)pv[0];
	if (__GL_ABSF(tex->state.params.maxLOD) > 63.0F) {
	    if (tex->state.params.maxLOD > __glZero) {
		tex->adjustedMaxLOD = __GL_MAX_FLOAT;
	    } else {
		tex->adjustedMaxLOD = -__GL_MAX_FLOAT;
	    }
	} else {
	    tex->adjustedMaxLOD = __GL_POWF(2.0, tex->state.params.maxLOD);
	    tex->adjustedMaxLOD *= tex->adjustedMaxLOD;
	}
	break;
      case GL_TEXTURE_BASE_LEVEL:
	if (pv[0] < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	tex->state.params.baseLevel = pv[0];
	break;
      case GL_TEXTURE_MAX_LEVEL:
	if (pv[0] < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	tex->state.params.maxLevel = pv[0];
	break;
      default:
	goto bad_enum;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexParameteri(GLenum target, GLenum pname, GLint i)
{
    /* Accept only enumerants that correspond to single values */
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
      case GL_TEXTURE_WRAP_T:
      case GL_TEXTURE_WRAP_R:
      case GL_TEXTURE_MIN_FILTER:
      case GL_TEXTURE_MAG_FILTER:
      case GL_TEXTURE_PRIORITY:
      case GL_TEXTURE_MIN_LOD:
      case GL_TEXTURE_MAX_LOD:
      case GL_TEXTURE_BASE_LEVEL:
      case GL_TEXTURE_MAX_LEVEL:
	__glim_TexParameteriv(target, pname, &i);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

GLint __glTexParameterfv_size(GLenum e)
{
    switch (e) {
      case GL_TEXTURE_WRAP_S:
      case GL_TEXTURE_WRAP_T:
      case GL_TEXTURE_WRAP_R:
      case GL_TEXTURE_MIN_FILTER:
      case GL_TEXTURE_MAG_FILTER:
      case GL_TEXTURE_PRIORITY:
	return 1;
      case GL_TEXTURE_BORDER_COLOR:
	return 4;
      case GL_TEXTURE_MIN_LOD:
      case GL_TEXTURE_MAX_LOD:
      case GL_TEXTURE_BASE_LEVEL:
      case GL_TEXTURE_MAX_LEVEL:
        return 1;
      default:
	return -1;
    }
}

GLint __glTexParameteriv_size(GLenum e)
{
    return __glTexParameterfv_size(e);
}

/************************************************************************/

void GLAPI __glim_TexEnvfv(GLenum target, GLenum pname, const GLfloat pv[])
{
    __GLtextureEnvState *tes;
    GLenum e;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if(target < GL_TEXTURE_ENV) {
      __glSetError(GL_INVALID_ENUM);
      return;
    }
    target -= GL_TEXTURE_ENV;
    if (target >= (GLuint) __GL_NUM_TEXTURE_ENV_BINDINGS) {
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    tes = &gc->state.texture.active->env[target];

    switch (pname) {
      case GL_TEXTURE_ENV_MODE:
	switch(e = (GLenum) pv[0]) {
	  case GL_MODULATE:
	  case GL_DECAL:
	  case GL_BLEND:
	  case GL_REPLACE:
	  case GL_ADD:
	    tes->mode = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_ENV_COLOR:
	__glClampAndScaleColorf(gc, &tes->color, pv);
	tes->color.r *= gc->constants.redRescale;
	tes->color.g *= gc->constants.greenRescale;
	tes->color.b *= gc->constants.blueRescale;
	tes->color.a *= gc->constants.alphaRescale;
	break;
      default:
	goto bad_enum;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexEnvf(GLenum target, GLenum pname, GLfloat f)
{
    /* Accept only enumerants that correspond to single values */
    switch (pname) {
      case GL_TEXTURE_ENV_MODE:
	__glim_TexEnvfv(target, pname, &f);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_TexEnviv(GLenum target, GLenum pname, const GLint pv[])
{
    __GLtextureEnvState *tes;
    GLenum e;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if(target < GL_TEXTURE_ENV) {
      __glSetError(GL_INVALID_ENUM);
      return;
    }
    target -= GL_TEXTURE_ENV;
    if (target >= (GLuint) __GL_NUM_TEXTURE_ENV_BINDINGS) {
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    tes = &gc->state.texture.active->env[target];

    switch (pname) {
      case GL_TEXTURE_ENV_MODE:
	switch(e = (GLenum) pv[0]) {
	  case GL_MODULATE:
	  case GL_DECAL:
	  case GL_BLEND:
	  case GL_REPLACE:
	  case GL_ADD:
	    tes->mode = e;
	    break;
	  default:
	    goto bad_enum;
	}
	break;
      case GL_TEXTURE_ENV_COLOR:
	__glClampAndScaleColori(gc, &tes->color, pv);
	tes->color.r *= gc->constants.redRescale;
	tes->color.g *= gc->constants.greenRescale;
	tes->color.b *= gc->constants.blueRescale;
	tes->color.a *= gc->constants.alphaRescale;
	break;
      default:
	goto bad_enum;
    }
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexEnvi(GLenum target, GLenum pname, GLint i)
{
    /* Accept only enumerants that correspond to single values */
    switch (pname) {
      case GL_TEXTURE_ENV_MODE:
	__glim_TexEnviv(target, pname, &i);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

GLint __glTexEnvfv_size(GLenum e)
{
    switch (e) {
      case GL_TEXTURE_ENV_MODE:
	return 1;
      case GL_TEXTURE_ENV_COLOR:
	return 4;
      default:
	return -1;
    }
}

GLint __glTexEnviv_size(GLenum e)
{
    return __glTexEnvfv_size(e);
}

/************************************************************************/

GLboolean __glIsTextureConsistent(__GLcontext *gc, __GLtexture *tex)
{
    __GLtextureParamState *params = &tex->state.params;
    GLint i, width, height, depth;
    GLint maxLevel;
    GLint border;
    GLenum baseFormat;
    GLenum requestedFormat;

    if ((tex->level[0]->width == 0) ||
	(tex->level[0]->height == 0) ||
	(tex->level[0]->depth == 0)) {
	return GL_FALSE;
    }

    border   = tex->level[0]->border;
    width    = tex->level[0]->width - border*2;
    height   = tex->level[0]->height - border*2;
    depth    = tex->level[0]->depth - border*2;
    maxLevel = gc->constants.maxMipMapLevel;

    baseFormat = tex->level[0]->baseFormat;
    if (gc->modes.rgbMode) {
	if (baseFormat == GL_COLOR_INDEX) {
	    baseFormat = tex->CT.baseFormat;
	    if (baseFormat == 0 || baseFormat == GL_COLOR_INDEX) {
		return GL_FALSE;
	    }
	}
    } else {
	return GL_FALSE;
    }

    requestedFormat = tex->level[0]->requestedFormat;

    switch(gc->state.texture.active->env[0].mode) {
      case GL_DECAL:
	if (!(gc->modes.rgbMode &&
	     (baseFormat == GL_RGB || baseFormat == GL_RGBA))) {
	    return GL_FALSE;
	}
	break;
      case GL_BLEND:
      case GL_MODULATE:
	if (!gc->modes.rgbMode) {
	    return GL_FALSE;
	}
	break;
      case GL_ADD:
	if (gc->modes.rgbMode) {
	    return GL_FALSE;
	}
      default:
	break;
    }

    /* If not-mipmapping, we are ok */
    switch (params->minFilter) {
      case GL_NEAREST:
      case GL_LINEAR:
	return GL_TRUE;
      default:
	break;
    }

    i = 0;
    while (++i < maxLevel) {
	if (width == 1 && height == 1 && depth == 1) break;
	width >>= 1;
	if (width == 0) width = 1;
	height >>= 1;
	if (height == 0) height = 1;
	depth >>= 1;
	if (depth == 0) depth = 1;

	if (tex->level[i]->border != border ||
		tex->level[i]->requestedFormat != requestedFormat ||
		tex->level[i]->width != width + border*2 ||
		tex->level[i]->height != height + border*2 ||
		tex->level[i]->depth != depth + border*2) {
	    return GL_FALSE;
	}
    }

    return GL_TRUE;
}

static __GLtexture *CheckTexImageArgs(__GLcontext *gc, GLenum target, GLint lod,
				      GLint components, GLint border,
				      GLenum format, GLenum type, GLint dim)
{
    __GLtexture *tex = __glLookupActiveTexture(gc, target);

    if (!tex || (tex->dim != dim)) {
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return 0;
    }

    switch (type) {
      case GL_BITMAP:
	if (format != GL_COLOR_INDEX) goto bad_enum;
	if (dim > 2) goto bad_enum;
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
	    return 0;
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
	    return 0;
	}
	break;

      default:
	goto bad_enum;
    }

    switch (format) {
      case GL_COLOR_INDEX:	case GL_RED:
      case GL_GREEN:		case GL_BLUE:
      case GL_ALPHA:		case GL_RGB:
      case GL_RGBA:		case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:	case GL_ABGR_EXT:
      case GL_BGRA:		case GL_BGR:
	break;
      default:
	goto bad_enum;
    }

    if ((lod < 0) || (lod >= gc->constants.maxMipMapLevel)) {
      bad_value:
	__glSetError(GL_INVALID_VALUE);
	return 0;
    }

    switch (components) {
      case 1: case 2: case 3: case 4:
      case GL_LUMINANCE:
      case GL_LUMINANCE4:	case GL_LUMINANCE8:
      case GL_LUMINANCE12:	case GL_LUMINANCE16:
	break;
      case GL_LUMINANCE_ALPHA:
      case GL_LUMINANCE4_ALPHA4:	case GL_LUMINANCE6_ALPHA2:
      case GL_LUMINANCE8_ALPHA8:	case GL_LUMINANCE12_ALPHA4:
      case GL_LUMINANCE12_ALPHA12:	case GL_LUMINANCE16_ALPHA16:
	break;
      case GL_RGB:
      case GL_R3_G3_B2:		case GL_RGB4:
      case GL_RGB5:		case GL_RGB8:
      case GL_RGB10:		case GL_RGB12:
      case GL_RGB16:
	break;
      case GL_RGBA:
      case GL_RGBA2:		case GL_RGBA4:
      case GL_RGBA8:		case GL_RGBA12:
      case GL_RGBA16:		case GL_RGB5_A1:
      case GL_RGB10_A2:
	break;
      case GL_ALPHA:
      case GL_ALPHA4:		case GL_ALPHA8:
      case GL_ALPHA12:		case GL_ALPHA16:
	break;
      case GL_INTENSITY:
      case GL_INTENSITY4:	case GL_INTENSITY8:
      case GL_INTENSITY12:	case GL_INTENSITY16:
	break;
#ifdef XXX_IS_COLOR_INDEX_FORMAT_SUPPORTED
      case GL_COLOR_INDEX1_EXT: case GL_COLOR_INDEX2_EXT:
      case GL_COLOR_INDEX4_EXT: case GL_COLOR_INDEX8_EXT:
      case GL_COLOR_INDEX12_EXT:	case GL_COLOR_INDEX16_EXT:
	if (format != GL_COLOR_INDEX)
	    goto bad_value;
	switch (type) {
	case GL_BYTE:	case GL_UNSIGNED_BYTE:
	case GL_SHORT:	case GL_UNSIGNED_SHORT:
	case GL_INT:	case GL_UNSIGNED_INT:
	    break;
	default:
	    goto bad_value;
	}

	break;
#endif
      default:
	goto bad_enum;
    }

    if ((border < 0) || (border > 1)) {
	goto bad_value;
    }

    return tex;
}


/*
** Used to store texture images.
*/
/* ARGSUSED */
static void InitTexImageStore(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				  __GLtexture *tex, GLint lod)
{
    __GLmipMapLevel *lp = tex->level[lod];

    spanInfo->dstImage = lp->buffer;
    spanInfo->dstSkipPixels = 0;
    spanInfo->dstSkipLines = 0;
    spanInfo->dstSkipImages = 0;
    spanInfo->dstSwapBytes = GL_FALSE;
    spanInfo->dstLsbFirst = GL_TRUE;
    spanInfo->dstLineLength = lp->width;
    spanInfo->dstImageHeight = lp->height;
    spanInfo->dim = tex->dim;
    if (tex->dim == 1) {
	spanInfo->dstSkipLines += lp->border;
    }

    spanInfo->dstFormat = lp->texFormat->pxFormat;
    spanInfo->dstType = lp->texFormat->pxType;
    spanInfo->dstAlignment = lp->texFormat->pxAlignment;
}

/*
** Used to get texture images.
*/
/*ARGSUSED*/
void __glInitTexImageGet(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 __GLtexture *tex, GLint lod)
{
    __GLmipMapLevel *lp = tex->level[lod];

    spanInfo->srcImage = lp->buffer;
    spanInfo->srcSkipPixels = 0;
    spanInfo->srcSkipLines = 0;
    spanInfo->srcSkipImages = 0;
    spanInfo->srcSwapBytes = GL_FALSE;
    spanInfo->srcLsbFirst = GL_TRUE;
    spanInfo->srcLineLength = lp->width;
    spanInfo->srcImageHeight = lp->height;
    spanInfo->dim = tex->dim;
    if (tex->dim == 1) {
	spanInfo->srcSkipLines += lp->border;
    }

    spanInfo->srcFormat = lp->texFormat->pxFormat;
    spanInfo->srcType = lp->texFormat->pxType;
    spanInfo->srcAlignment = lp->texFormat->pxAlignment;
}

/*
** Used to store texture images.  "packed" is set to GL_TRUE if this
** image is being pulled out of a display list, and GL_FALSE if it is
** being pulled directly out of an application.
*/
static void InitTexSourceUnpack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				GLsizei width, GLsizei height, GLsizei depth,
				GLenum format, GLenum type, const GLvoid *buf,
				GLboolean packed)
{
    spanInfo->x = 0;
    spanInfo->zoomx = __glOne;
    spanInfo->width = width;
    spanInfo->height = height;
    spanInfo->depth = depth;
    spanInfo->srcFormat = format;
    spanInfo->srcType = type;
    spanInfo->srcImage = buf;

    /* Set the remaining source modes according to the pixel state */
    __glLoadUnpackModes(gc, spanInfo, packed);
}

/*
** Used to get texture images.
*/
void __glInitTexDestPack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 GLsizei width, GLsizei height, GLsizei depth,
			 GLenum format, GLenum type, GLvoid *buf)
{
    spanInfo->x = 0;
    spanInfo->zoomx = __glOne;
    spanInfo->width = width;
    spanInfo->height = height;
    spanInfo->depth = depth;
    spanInfo->dstFormat = format;
    spanInfo->dstType = type;
    spanInfo->dstImage = buf;

    /* Set the remaining source modes according to the pixel state */
    __glLoadPackModes(gc, spanInfo);
}

/*
** Return GL_TRUE if the given range (length or width/height) is a legal
** power of 2, taking into account the border.	The range is not allowed
** to be negative either.
*/
static GLboolean IsLegalRange(__GLcontext *gc, GLsizei r, GLint border)
{
#if defined(__GL_LINT)
    gc = gc;
#endif
    r -= border * 2;
    if ((r < 0) || (r & (r - 1))) {
	__glSetError(GL_INVALID_VALUE);
	return GL_FALSE;
    }
    return GL_TRUE;
}

__GLtexture *__glCheckTexImage1DArgs(__GLcontext *gc, GLenum target, GLint lod,
				     GLint components, GLsizei length,
				     GLint border, GLenum format, GLenum type)
{
    __GLtexture *tex;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexImageArgs(gc, target, lod, components, border,
			    format, type, 1);
    if (!tex) {
	return 0;
    }
    if (!IsLegalRange(gc, length, border)) {
	return 0;
    }
    return tex;
}

__GLtexture *__glCheckTexImage2DArgs(__GLcontext *gc, GLenum target, GLint lod,
				     GLint components, GLsizei w, GLsizei h,
				     GLint border, GLenum format, GLenum type)
{
    __GLtexture *tex;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexImageArgs(gc, target, lod, components, border,
			    format, type, 2);
    if (!tex) {
	return 0;
    }
    if (!IsLegalRange(gc, w, border)) {
	return 0;
    }
    if (!IsLegalRange(gc, h, border)) {
	return 0;
    }
    return tex;
}

__GLtexture *__glCheckTexImage3DArgs(__GLcontext *gc, GLenum target, GLint lod,
				     GLint components,
				     GLsizei w, GLsizei h, GLsizei d,
				     GLint border, GLenum format, GLenum type)
{
    __GLtexture *tex;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexImageArgs(gc, target, lod, components, border,
			    format, type, 3);
    if (!tex) {
	return 0;
    }
    if (!IsLegalRange(gc, w, border)) {
	return 0;
    }
    if (!IsLegalRange(gc, h, border)) {
	return 0;
    }
    if (!IsLegalRange(gc, d, border)) { 
	return 0;
    }
    return tex;
}

/*ARGSUSED*/
static __GLtexture *CheckCopyTexImageArgs(__GLcontext *gc, GLenum target,
					  GLint level,
					  GLenum internalformat,
					  GLint x, GLint y,
					  GLsizei width, GLsizei height,
					  GLint border, GLint dim)
{
    __GLtexture *tex;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexImageArgs(gc, target, level, internalformat, border,
			    GL_RGBA, GL_FLOAT, dim);

    switch(target) {
      case GL_PROXY_TEXTURE_1D:
      case GL_PROXY_TEXTURE_2D:
      case GL_PROXY_TEXTURE_3D:
	__glSetError(GL_INVALID_ENUM);
	return NULL;
    }
    switch(internalformat) {
	case 1: case 2: case 3: case 4:
	    __glSetError(GL_INVALID_ENUM);
	    return NULL;
	default:
	    break;
    }
    if (!tex) {
	return 0;
    }
    if (!IsLegalRange(gc, width, border)) {
	return 0;
    }
    if ((dim > 1) &&
	!IsLegalRange(gc, height, border)) {
	return 0;
    }
    return tex;
}

/************************************************************************/

void GLAPI __glim_TexImage1D(GLenum target, GLint lod,
		       GLint components, GLsizei length,
		       GLint border, GLenum format,
		       GLenum type, const GLvoid *buf)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = length + gc->pixel.adjustWidth1D;

    /* Check arguments and get the right texture being changed */
    tex = __glCheckTexImage1DArgs(gc, target, lod, components, adjustedWidth,
				  border, format, type);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, lod, components,
    			adjustedWidth, 1+border*2, 1+border*2, border, 1);
    /* Copy image data */
    if (buf && dest) {
	InitTexSourceUnpack(gc, &spanInfo, length, 1, 1,
			    format, type, buf, GL_FALSE);
	InitTexImageStore(gc, &spanInfo, tex, lod);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);
	(*tex->copyTexImage)(gc, &spanInfo, tex, lod);
    }

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void __gllei_TexImage1D(__GLcontext *gc, GLenum target, GLint lod,
			GLint components, GLsizei length, GLint border,
			GLenum format, GLenum type, const GLubyte *image)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth;
    GLuint beginMode;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
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

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = length + gc->pixel.adjustWidth1D;

    /* Check arguments and get the right texture being changed */
    tex = __glCheckTexImage1DArgs(gc, target, lod, components, adjustedWidth,
				  border, format, type);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, lod, components,
    			adjustedWidth, 1+border*2, 1+border*2, border, 1);

    /* Copy image data */
    if (image && dest) {
	InitTexSourceUnpack(gc, &spanInfo, length, 1, 1,
			    format, type, image, GL_TRUE);
	InitTexImageStore(gc, &spanInfo, tex, lod);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);
	(*tex->copyTexImage)(gc, &spanInfo, tex, lod);
    }

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

/************************************************************************/

void GLAPI __glim_TexImage2D(GLenum target, GLint lod, GLint components,
		       GLsizei w, GLsizei h, GLint border, GLenum format,
		       GLenum type, const GLvoid *buf)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture being changed */
    tex = __glCheckTexImage2DArgs(gc, target, lod, components,
    			adjustedWidth, adjustedHeight, border, format, type);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, lod, components,
    			adjustedWidth, adjustedHeight, 1+border*2, border, 2);

    /* Copy image data */
    if (buf && dest) {
	InitTexSourceUnpack(gc, &spanInfo, w, h, 1,
			    format, type, buf, GL_FALSE);
	InitTexImageStore(gc, &spanInfo, tex, lod);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);
	(*tex->copyTexImage)(gc, &spanInfo, tex, lod);
    }

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void __gllei_TexImage2D(__GLcontext *gc, GLenum target, GLint lod,
			GLint components, GLsizei w, GLsizei h,
			GLint border, GLenum format, GLenum type,
			const GLubyte *image)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;
    GLuint beginMode;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
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

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture being changed */
    tex = __glCheckTexImage2DArgs(gc, target, lod, components,
    			adjustedWidth, adjustedHeight, border, format, type);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, lod, components,
    			adjustedWidth, adjustedHeight, 1+border*2, border, 2);

    /* Copy image data */
    if (image && dest) {
	InitTexSourceUnpack(gc, &spanInfo, w, h, 1,
			    format, type, image, GL_TRUE);
	InitTexImageStore(gc, &spanInfo, tex, lod);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);
	(*tex->copyTexImage)(gc, &spanInfo, tex, lod);
    }

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

/************************************************************************/

void GLAPI __glim_TexImage3D(GLenum target, GLint lod, GLenum internalFormat,
		             GLsizei w, GLsizei h, GLsizei d, GLint border,
		             GLenum format, GLenum type, const GLvoid *buf)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture being changed */
    tex = __glCheckTexImage3DArgs(gc, target, lod, internalFormat,
    			adjustedWidth, adjustedHeight, d, border, format, type);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, lod, internalFormat,
    			adjustedWidth, adjustedHeight, d, border, 3);

    /* Copy image data */
    if (buf && dest) {
	InitTexSourceUnpack(gc, &spanInfo, w, h, d, format, type, buf,
			    GL_FALSE);
	InitTexImageStore(gc, &spanInfo, tex, lod);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);
	(*tex->copyTexImage)(gc, &spanInfo, tex, lod);
    }

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void __gllei_TexImage3D(__GLcontext *gc, GLenum target, GLint lod, 
			GLint internalFormat,
			GLsizei w, GLsizei h, GLsizei d,
			GLint border, GLenum format, GLenum type,
			const GLubyte *image)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;
    GLuint beginMode;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
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

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture being changed */
    tex = __glCheckTexImage3DArgs(gc, target, lod, internalFormat,
    			adjustedWidth, adjustedHeight, d, border, format, type);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, lod, internalFormat,
    			adjustedWidth, adjustedHeight, d, border, 3);

    /* Copy image data */
    if (image && dest) {
	InitTexSourceUnpack(gc, &spanInfo, w, h, d, format, type, image,
			    GL_TRUE);
	InitTexImageStore(gc, &spanInfo, tex, lod);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);
	(*tex->copyTexImage)(gc, &spanInfo, tex, lod);
    }

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

/************************************************************************/

static __GLfloat Dot(const __GLcoord *v1, const __GLcoord *v2)
{
    return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

/*
** Compute the s & t coordinates for a sphere map.  The s & t values
** are stored in "result" even if both coordinates are not being
** generated.  The caller picks the right values out.
*/
static void SphereGen(__GLcontext *gc, __GLvertex *vx, __GLcoord *result)
{
    __GLcoord u, r;
    __GLfloat m, ndotu;

    /* Get unit vector from origin to the vertex in eye coordinates into u */
    (*gc->procs.normalize)(&u.x, &vx->eye.x);

    /* Dot the normal with the unit position u */
    ndotu = Dot(&vx->normal, &u);

    /* Compute r */
    r.x = u.x - 2 * vx->normal.x * ndotu;
    r.y = u.y - 2 * vx->normal.y * ndotu;
    r.z = u.z - 2 * vx->normal.z * ndotu;

    /* Compute m */
    m = 2 * __GL_SQRTF(r.x*r.x + r.y*r.y + (r.z + 1) * (r.z + 1));

    if (m) {
	result->x = r.x / m + __glHalf;
	result->y = r.y / m + __glHalf;
    } else {
	result->x = __glHalf;
	result->y = __glHalf;
    }
}

/*
** Transform or compute the texture coordinates for this vertex.
*/
void __glCalcMixedTexture(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLcoord sphereCoord, gen, *c;
    GLboolean didSphereGen = GL_FALSE;
    __GLmatrix *m;

    /* Generate/copy s coordinate */
    if (gc->state.enables.texture[unit] & __GL_TEXTURE_GEN_S_ENABLE) {
	switch (gc->state.texture.unit[unit].s.mode) {
	  case GL_EYE_LINEAR:
	    c = &gc->state.texture.unit[unit].s.eyePlaneEquation;
	    gen.x = c->x * vx->eye.x + c->y * vx->eye.y
		+ c->z * vx->eye.z + c->w * vx->eye.w;
	    break;
	  case GL_OBJECT_LINEAR:
	    c = &gc->state.texture.unit[unit].s.objectPlaneEquation;
	    gen.x = c->x * vx->obj.x + c->y * vx->obj.y
		+ c->z * vx->obj.z + c->w * vx->obj.w;
	    break;
	  case GL_SPHERE_MAP:
	    SphereGen(gc, vx, &sphereCoord);
	    gen.x = sphereCoord.x;
	    didSphereGen = GL_TRUE;
	    break;
	}
    } else {
	gen.x = vx->texture[unit].x;
    }

    /* Generate/copy t coordinate */
    if (gc->state.enables.texture[unit] & __GL_TEXTURE_GEN_T_ENABLE) {
	switch (gc->state.texture.unit[unit].t.mode) {
	  case GL_EYE_LINEAR:
	    c = &gc->state.texture.unit[unit].t.eyePlaneEquation;
	    gen.y = c->x * vx->eye.x + c->y * vx->eye.y
		+ c->z * vx->eye.z + c->w * vx->eye.w;
	    break;
	  case GL_OBJECT_LINEAR:
	    c = &gc->state.texture.unit[unit].t.objectPlaneEquation;
	    gen.y = c->x * vx->obj.x + c->y * vx->obj.y
		+ c->z * vx->obj.z + c->w * vx->obj.w;
	    break;
	  case GL_SPHERE_MAP:
	    if (!didSphereGen) {
		SphereGen(gc, vx, &sphereCoord);
	    }
	    gen.y = sphereCoord.y;
	    break;
	}
    } else {
	gen.y = vx->texture[unit].y;
    }

    /* Generate/copy r coordinate */
    if (gc->state.enables.texture[unit] & __GL_TEXTURE_GEN_R_ENABLE) {
	switch (gc->state.texture.unit[unit].r.mode) {
	  case GL_EYE_LINEAR:
	    c = &gc->state.texture.unit[unit].r.eyePlaneEquation;
	    gen.z = c->x * vx->eye.x + c->y * vx->eye.y
		+ c->z * vx->eye.z + c->w * vx->eye.w;
	    break;
	  case GL_OBJECT_LINEAR:
	    c = &gc->state.texture.unit[unit].r.objectPlaneEquation;
	    gen.z = c->x * vx->obj.x + c->y * vx->obj.y
		+ c->z * vx->obj.z + c->w * vx->obj.w;
	    break;
	}
    } else {
	gen.z = vx->texture[unit].z;
    }

    /* Generate/copy q coordinate */
    if (gc->state.enables.texture[unit] & __GL_TEXTURE_GEN_Q_ENABLE) {
	switch (gc->state.texture.unit[unit].q.mode) {
	  case GL_EYE_LINEAR:
	    c = &gc->state.texture.unit[unit].q.eyePlaneEquation;
	    gen.w = c->x * vx->eye.x + c->y * vx->eye.y
		+ c->z * vx->eye.z + c->w * vx->eye.w;
	    break;
	  case GL_OBJECT_LINEAR:
	    c = &gc->state.texture.unit[unit].q.objectPlaneEquation;
	    gen.w = c->x * vx->obj.x + c->y * vx->obj.y
		+ c->z * vx->obj.z + c->w * vx->obj.w;
	    break;
	}
    } else {
	gen.w = vx->texture[unit].w;
    }

    /* Finally, apply texture matrix */
    m = &gc->transform.texture[unit]->matrix;
    (*m->xf4)(&vx->texture[unit], &gen.x, m);
}

void __glCalcEyeLinear(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLcoord gen, *c;
    __GLmatrix *m;

    /* Generate texture coordinates from eye coordinates */
    c = &gc->state.texture.unit[unit].s.eyePlaneEquation;
    gen.x = c->x * vx->eye.x + c->y * vx->eye.y + c->z * vx->eye.z
	+ c->w * vx->eye.w;
    c = &gc->state.texture.unit[unit].t.eyePlaneEquation;
    gen.y = c->x * vx->eye.x + c->y * vx->eye.y + c->z * vx->eye.z
	+ c->w * vx->eye.w;
    c = &gc->state.texture.unit[unit].r.eyePlaneEquation;
    gen.z = c->x * vx->eye.x + c->y * vx->eye.y + c->z * vx->eye.z
	+ c->w * vx->eye.w;
    gen.w = vx->texture[unit].w;

    /* Finally, apply texture matrix */
    m = &gc->transform.texture[unit]->matrix;
    (*m->xf4)(&vx->texture[unit], &gen.x, m);
}

void __glCalcObjectLinear(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLcoord gen, *c;
    __GLmatrix *m;

    /* Generate texture coordinates from object coordinates */
    c = &gc->state.texture.unit[unit].s.objectPlaneEquation;
    gen.x = c->x * vx->obj.x + c->y * vx->obj.y + c->z * vx->obj.z
	+ c->w * vx->obj.w;
    c = &gc->state.texture.unit[unit].t.objectPlaneEquation;
    gen.y = c->x * vx->obj.x + c->y * vx->obj.y + c->z * vx->obj.z
	+ c->w * vx->obj.w;
    c = &gc->state.texture.unit[unit].r.objectPlaneEquation;
    gen.z = c->x * vx->obj.x + c->y * vx->obj.y + c->z * vx->obj.z
	+ c->w * vx->obj.w;
    gen.w = vx->texture[unit].w;

    /* Finally, apply texture matrix */
    m = &gc->transform.texture[unit]->matrix;
    (*m->xf4)(&vx->texture[unit], &gen.x, m);
}

void __glCalcSphereMap(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLcoord sphereCoord;
    __GLmatrix *m;

    SphereGen(gc, vx, &sphereCoord);
    sphereCoord.z = vx->texture[unit].z;
    sphereCoord.w = vx->texture[unit].w;

    /* Finally, apply texture matrix */
    m = &gc->transform.texture[unit]->matrix;
    (*m->xf4)(&vx->texture[unit], &sphereCoord.x, m);
}

void __glCalcTexture(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLcoord copy;
    __GLmatrix *m;

    copy.x = vx->texture[unit].x;
    copy.y = vx->texture[unit].y;
    copy.z = vx->texture[unit].z;
    copy.w = vx->texture[unit].w;

    /* Apply texture matrix */
    m = &gc->transform.texture[unit]->matrix;
    (*m->xf4)(&vx->texture[unit], &copy.x, m);
}

void __glCalcTexturePersp(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    (*gc->procs.calcTexture2Unit[unit])(gc, unit, vx);

    if (!(vx->flagBits & __GL_CLIP_MASK)) {
	vx->texture[unit].x *= vx->window.w;
	vx->texture[unit].y *= vx->window.w;
	vx->texture[unit].z *= vx->window.w;
	vx->texture[unit].w *= vx->window.w;
    }
}

void __glFastCalcTexturePersp(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    if (gc->transform.texture[unit]->matrix.matrixType != __GL_MT_IDENTITY) {
	(*gc->procs.calcTexture2Unit[unit])(gc, unit, vx);
    }

    if (!(vx->flagBits & __GL_CLIP_MASK)) {
	vx->texture[unit].x *= vx->window.w;
	vx->texture[unit].y *= vx->window.w;
	vx->texture[unit].z *= vx->window.w;
	vx->texture[unit].w *= vx->window.w;
    }
}

void __glCalcTextureUVScale(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLmipMapLevel *lp = gc->texture.active->currentTexture->level[0];

    (*gc->procs.calcTexture2Unit[unit])(gc, unit, vx);

    vx->texture[unit].x *= lp->width2f;
    vx->texture[unit].y *= lp->height2f;
    vx->texture[unit].z *= lp->depth2f;
}

void __glFastCalcTextureUVScale(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLmipMapLevel *lp = gc->texture.active->currentTexture->level[0];

    if (gc->transform.texture[unit]->matrix.matrixType != __GL_MT_IDENTITY) {
	(*gc->procs.calcTexture2Unit[unit])(gc, unit, vx);
    }

    vx->texture[unit].x *= lp->width2f;
    vx->texture[unit].y *= lp->height2f;
    vx->texture[unit].z *= lp->depth2f;
}

void __glCalcTexturePerspUVScale(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLmipMapLevel *lp = gc->texture.active->currentTexture->level[0];

    (*gc->procs.calcTexture2Unit[unit])(gc, unit, vx);

    if (!(vx->flagBits & __GL_CLIP_MASK)) {
	vx->texture[unit].x *= vx->window.w*lp->width2f;
	vx->texture[unit].y *= vx->window.w*lp->height2f;
	vx->texture[unit].z *= vx->window.w*lp->depth2f;
	vx->texture[unit].w *= vx->window.w;
    } else {
	vx->texture[unit].x *= lp->width2f;
	vx->texture[unit].y *= lp->height2f;
	vx->texture[unit].z *= lp->depth2f;
    }
}

void __glFastCalcTexturePerspUVScale(__GLcontext *gc, GLint unit, __GLvertex *vx)
{
    __GLmipMapLevel *lp = gc->texture.active->currentTexture->level[0];

    if (gc->transform.texture[unit]->matrix.matrixType != __GL_MT_IDENTITY) {
	(*gc->procs.calcTexture2Unit[unit])(gc, unit, vx);
    }

    if (!(vx->flagBits & __GL_CLIP_MASK)) {
	vx->texture[unit].x *= vx->window.w*lp->width2f;
	vx->texture[unit].y *= vx->window.w*lp->height2f;
	vx->texture[unit].z *= vx->window.w*lp->depth2f;
	vx->texture[unit].w *= vx->window.w;
    } else {
	vx->texture[unit].x *= lp->width2f;
	vx->texture[unit].y *= lp->height2f;
	vx->texture[unit].z *= lp->depth2f;
    }
}

void __glCalcMultiTexture(__GLcontext *gc, __GLvertex *vx)
{
    GLint i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	(*gc->procs.calcTextureUnit[i])(gc, i, vx);
    }
}

void __glValidateTextureModelView(__GLcontext *gc)
{
    GLboolean needModelViewInverse = GL_FALSE;
    GLint i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	if (((gc->state.enables.texture[i] & __GL_TEXTURE_GEN_S_ENABLE) &&
	     (gc->state.texture.active->s.mode == GL_SPHERE_MAP)) ||
	    ((gc->state.enables.texture[i] & __GL_TEXTURE_GEN_T_ENABLE) &&
	     (gc->state.texture.active->t.mode == GL_SPHERE_MAP)) ||
	    ((gc->state.enables.texture[i] & __GL_TEXTURE_GEN_R_ENABLE) &&
	     (gc->state.texture.active->r.mode == GL_SPHERE_MAP))) {
	     needModelViewInverse = GL_TRUE;
	}
    }

    if (needModelViewInverse) {
	__GLtransform *tr = gc->transform.modelView;

	if (tr->updateInverse) {
	    (*gc->procs.computeInverseTranspose)(gc, tr);
	}
    }
}

/************************************************************************/

static __GLtexture *CheckTexSubImageArgs(__GLcontext *gc, GLenum target,
					 GLint lod, GLenum format,
					 GLenum type, GLint dim)
{
    __GLtexture *tex = __glLookupActiveTexture(gc, target);

    if (!tex || (target == GL_PROXY_TEXTURE_1D) ||
		(target == GL_PROXY_TEXTURE_2D) ||
		(target == GL_PROXY_TEXTURE_3D))
    {
      bad_enum:
	__glSetError(GL_INVALID_ENUM);
	return 0;
    }

    if (tex->dim != dim) {
	goto bad_enum;
    }

    switch (type) {
      case GL_BITMAP:
	if (format != GL_COLOR_INDEX) goto bad_enum;
	if (dim > 2) goto bad_enum;
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
	    return 0;
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
	    return 0;
	}
	break;
      default:
	goto bad_enum;
    }

    switch (format) {
      case GL_COLOR_INDEX:	case GL_RED:
      case GL_GREEN:		case GL_BLUE:
      case GL_ALPHA:		case GL_RGB:
      case GL_RGBA:		case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:	case GL_ABGR_EXT:
      case GL_BGRA:		case GL_BGR:
	break;
      default:
	goto bad_enum;
    }

    if ((lod < 0) || (lod >= gc->constants.maxMipMapLevel)) {
	__glSetError(GL_INVALID_VALUE);
	return 0;
    }

    return tex;
}

static void InitTexSubImageStore(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				 __GLtexture *tex, GLint lod,
				 GLint xoffset, GLint yoffset, GLint zoffset)
{
    __GLmipMapLevel *lp = tex->level[lod];

    InitTexImageStore(gc, spanInfo, tex, lod);
    spanInfo->dstSkipPixels = xoffset + lp->border;
    spanInfo->dstSkipLines = yoffset + lp->border;
    spanInfo->dstSkipImages = zoffset + lp->border;
    spanInfo->dstLineLength = lp->width;
    spanInfo->dstImageHeight = lp->height;
    spanInfo->dim = tex->dim;
}

static GLboolean CheckTexSubImageRange(__GLcontext *gc, __GLmipMapLevel *lp,
				       GLint offset, GLsizei size, GLsizei max)
{
#if defined(__GL_LINT)
    gc = gc;
#endif
    if ((size < 0) || (offset < -lp->border) || (offset+size > max-lp->border))
    {
	__glSetError(GL_INVALID_VALUE);
	return GL_FALSE;
    }
    return GL_TRUE;
}

static __GLtexture *CheckTexSubImage1DArgs(__GLcontext *gc, GLenum target,
					   GLint lod,
					   GLint xoffset, GLint length,
					   GLenum format, GLenum type)
{
    __GLtexture *tex;
    __GLmipMapLevel *lp;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexSubImageArgs(gc, target, lod, format, type, 1);
    if (!tex) {
	return NULL;
    }
    lp = tex->level[lod];
    if (lp->buffer == NULL) {
	__glSetError(GL_INVALID_OPERATION);
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, xoffset, length, lp->width)) {
	return NULL;
    }
    return tex;
}

static __GLtexture *CheckTexSubImage2DArgs(__GLcontext *gc, GLenum target,
					   GLint lod,
					   GLint xoffset, GLint yoffset,
					   GLsizei w, GLsizei h,
					   GLenum format, GLenum type)
{
    __GLtexture *tex;
    __GLmipMapLevel *lp;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexSubImageArgs(gc, target, lod, format, type, 2);
    if (!tex) {
	return NULL;
    }
    lp = tex->level[lod];
    if (lp->buffer == NULL) {
	__glSetError(GL_INVALID_OPERATION);
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, xoffset, w, lp->width)) {
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, yoffset, h, lp->height)) {
	return NULL;
    }
    return tex;
}

static __GLtexture *CheckTexSubImage3DArgs(__GLcontext *gc, GLenum target,
					   GLint lod,
					   GLint xoffset, GLint yoffset,
					   GLint zoffset,
					   GLsizei w, GLsizei h, GLsizei d,
					   GLenum format, GLenum type)
{
    __GLtexture *tex;
    __GLmipMapLevel *lp;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexSubImageArgs(gc, target, lod, format, type, 3);
    if (!tex) {
	return NULL;
    }
    lp = tex->level[lod];
    if (lp->buffer == NULL) {
	__glSetError(GL_INVALID_OPERATION);
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, xoffset, w, lp->width)) {
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, yoffset, h, lp->height)) {
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, zoffset, d, lp->depth)) {
	return NULL;
    }
    return tex;
}

/*ARGSUSED*/
static __GLtexture *CheckCopyTexSubImageArgs(__GLcontext *gc, GLenum target,
					     GLint lod,
					     GLint xoffset, GLint yoffset,
					     GLint zoffset,
					     GLint x, GLint y,
					     GLsizei w, GLsizei h, GLint dim)
{
    __GLtexture *tex;
    __GLmipMapLevel *lp;

    /* Check arguments and get the right texture being changed */
    tex = CheckTexSubImageArgs(gc, target, lod, GL_RGBA, GL_FLOAT, dim);
    if (!tex) {
	return NULL;
    }
    lp = tex->level[lod];
    if (lp->buffer == NULL) {
	__glSetError(GL_INVALID_OPERATION);
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, xoffset, w, lp->width)) {
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, yoffset, h, lp->height)) {
	return NULL;
    }
    if (!CheckTexSubImageRange(gc, lp, zoffset, 1, lp->depth)) {
	return NULL;
    }
    return tex;
}

void GLAPI __glim_TexSubImage1D(GLenum target, GLint lod,
		       GLint xoffset, GLint length,
		       GLenum format, GLenum type, const GLvoid *buf)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = length + gc->pixel.adjustWidth1D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckTexSubImage1DArgs(gc, target, lod, xoffset,
    			adjustedWidth, format, type);
    if (!tex) {
	return;
    }

    /* Copy sub-image data */
    InitTexSourceUnpack(gc, &spanInfo, length, 1, 1,
			format, type, buf, GL_FALSE);
    InitTexSubImageStore(gc, &spanInfo, tex, lod, xoffset, 0, 0);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);
    (*tex->copyTexImage)(gc, &spanInfo, tex, lod);

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void __gllei_TexSubImage1D(__GLcontext *gc, GLenum target, GLint lod,
			GLint xoffset, GLint length,
			GLenum format, GLenum type, const GLubyte *image)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;
    GLuint beginMode;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    beginMode = gc->beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    gc->beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = length + gc->pixel.adjustWidth1D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckTexSubImage1DArgs(gc, target, lod, xoffset,
    			adjustedWidth, format, type);
    if (!tex) {
	return;
    }

    /* Copy sub-image data */
    InitTexSourceUnpack(gc, &spanInfo, length, 1, 1,
			format, type, image, GL_TRUE);
    InitTexSubImageStore(gc, &spanInfo, tex, lod, xoffset, 0, 0);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);
    (*tex->copyTexImage)(gc, &spanInfo, tex, lod);

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexSubImage2D(GLenum target, GLint lod,
		       GLint xoffset, GLint yoffset,
		       GLsizei w, GLsizei h, GLenum format,
		       GLenum type, const GLvoid *buf)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckTexSubImage2DArgs(gc, target, lod, xoffset, yoffset,
    			adjustedWidth, adjustedHeight, format, type);
    if (!tex) {
	return;
    }

    /* Copy sub-image data */
    InitTexSourceUnpack(gc, &spanInfo, w, h, 1,
			format, type, buf, GL_FALSE);
    InitTexSubImageStore(gc, &spanInfo, tex, lod, xoffset, yoffset, 0);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);
    (*tex->copyTexImage)(gc, &spanInfo, tex, lod);

    if (tex->level[lod]->pixelBuffer)
	__glCookSubTexture(gc, tex->level[lod], xoffset, yoffset, w, h);

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void __gllei_TexSubImage2D(__GLcontext *gc, GLenum target, GLint lod,
			GLint xoffset, GLint yoffset,
			GLsizei w, GLsizei h, GLenum format, GLenum type,
			const GLubyte *image)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;
    GLuint beginMode;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    beginMode = gc->beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    gc->beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckTexSubImage2DArgs(gc, target, lod, xoffset, yoffset,
    			adjustedWidth, adjustedHeight, format, type);
    if (!tex) {
	return;
    }

    /* Copy sub-image data */
    InitTexSourceUnpack(gc, &spanInfo, w, h, 1,
			format, type, image, GL_TRUE);
    InitTexSubImageStore(gc, &spanInfo, tex, lod, xoffset, yoffset, 0);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);
    (*tex->copyTexImage)(gc, &spanInfo, tex, lod);

    if (tex->level[lod]->pixelBuffer)
	__glCookSubTexture(gc, tex->level[lod], xoffset, yoffset, w, h);

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_TexSubImage3D(GLenum target, GLint lod,
		                GLint xoffset, GLint yoffset, GLint zoffset,
		                GLsizei w, GLsizei h, GLsizei d,
		                GLenum format, GLenum type, const GLvoid *buf)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckTexSubImage3DArgs(gc, target, lod, xoffset, yoffset, zoffset,
    			adjustedWidth, adjustedHeight, d, format, type);
    if (!tex) {
	return;
    }

    /* Copy sub-image data */
    InitTexSourceUnpack(gc, &spanInfo, w, h, d,
			    format, type, buf, GL_FALSE);
    InitTexSubImageStore(gc, &spanInfo, tex, lod, xoffset, yoffset, zoffset);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);
    (*tex->copyTexImage)(gc, &spanInfo, tex, lod);

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

void __gllei_TexSubImage3D(__GLcontext *gc, GLenum target, GLint lod, 
		           GLint xoffset, GLint yoffset, GLint zoffset,
			   GLsizei w, GLsizei h, GLsizei d,
			   GLenum format, GLenum type, const GLubyte *image)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;
    GLuint beginMode;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    beginMode = gc->beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    gc->beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckTexSubImage3DArgs(gc, target, lod, xoffset, yoffset, zoffset,
    			adjustedWidth, adjustedHeight, d, format, type);
    if (!tex) {
	return;
    }

    /* Copy sub-image data */
    InitTexSourceUnpack(gc, &spanInfo, w, h, d,
			    format, type, image, GL_TRUE);
    InitTexSubImageStore(gc, &spanInfo, tex, lod, xoffset, yoffset, zoffset);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);
    (*tex->copyTexImage)(gc, &spanInfo, tex, lod);

    /* remove texture from active list.  If needed, validation will reload */
    (*tex->unmakeResident)(gc, tex);

    /*
    ** by validating here, we do a delay load of the texture
    ** at validation time
    */
    __GL_DELAY_VALIDATE(gc);
}

/************************************************************************/

void GLAPI __glim_CopyTexImage1D(GLenum target,
			   GLint level,
			   GLenum internalformat,
			   GLint x,
			   GLint y,
			   GLsizei w,
			   GLint border)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_PIXEL_OP();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth1D;

    /* Check arguments and get the right texture being changed */
    tex = CheckCopyTexImageArgs(gc, target, level, internalformat,
    					x, y, adjustedWidth, 1, border, 1);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, level, internalformat,
    			adjustedWidth, 1+border*2, 1+border*2, border, 1);

    /* Copy image data */
    if (dest) {

	__glInitReadImageSrcInfo(gc, &spanInfo, x, y, w, 1);
	InitTexImageStore(gc, &spanInfo, tex, level);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);

	if (!__glClipReadPixels(gc, &spanInfo)) return;
	(*tex->readTexImage)(gc, &spanInfo, tex, level);
    }

    /* Might have just disabled texturing... */
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_CopyTexImage2D(GLenum target,
				 GLint level,
				 GLenum internalformat,
				 GLint x, GLint y,
				 GLsizei w, GLsizei h,
				 GLint border)
{
    __GLtexture *tex;
    __GLtextureBuffer *dest;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_PIXEL_OP();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = w + gc->pixel.adjustWidth2D;
    adjustedHeight = h + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture being changed */
    tex = CheckCopyTexImageArgs(gc, target, level, internalformat,
    			x, y, adjustedWidth, adjustedHeight, border, 2);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Allocate memory for the level data */
    dest = (*tex->createLevel)(gc, tex, level, internalformat,
    			adjustedWidth, adjustedHeight, 1+border*2, border, 2);

    /* Copy image data */
    if (dest) {

	__glInitReadImageSrcInfo(gc, &spanInfo, x, y, w, h);
	InitTexImageStore(gc, &spanInfo, tex, level);
	__glInitUnpacker(gc, &spanInfo);
	__glInitPacker(gc, &spanInfo);

	if (!__glClipReadPixels(gc, &spanInfo)) return;
	(*tex->readTexImage)(gc, &spanInfo, tex, level);
    }

    /* Might have just disabled texturing... */
    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_CopyTexSubImage1D(GLenum target,
				    GLint level,
				    GLint xoffset,
				    GLint x,
				    GLint y,
				    GLsizei width)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_PIXEL_OP();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = width + gc->pixel.adjustWidth1D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckCopyTexSubImageArgs(gc, target, level,
    				xoffset, 0, 0, x, y, adjustedWidth, 1, 1);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Copy sub-image data */
    __glInitReadImageSrcInfo(gc, &spanInfo, x, y, width, 1);
    InitTexSubImageStore(gc, &spanInfo, tex, level, xoffset, 0, 0);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    if (!__glClipReadPixels(gc, &spanInfo)) return;
    (*tex->readTexImage)(gc, &spanInfo, tex, level);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
}

void GLAPI __glim_CopyTexSubImage2D(GLenum target,
				    GLint level,
				    GLint xoffset,
				    GLint yoffset,
				    GLint x,
				    GLint y,
				    GLsizei width,
				    GLsizei height)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_PIXEL_OP();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = width + gc->pixel.adjustWidth2D;
    adjustedHeight = height + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckCopyTexSubImageArgs(gc, target, level,
				   xoffset, yoffset, 0,
				   x, y, adjustedWidth, adjustedHeight, 2);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Copy sub-image data */
    __glInitReadImageSrcInfo(gc, &spanInfo, x, y, width, height);
    InitTexSubImageStore(gc, &spanInfo, tex, level, xoffset, yoffset, 0);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    if (!__glClipReadPixels(gc, &spanInfo)) return;
    (*tex->readTexImage)(gc, &spanInfo, tex, level);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
}

void GLAPI __glim_CopyTexSubImage3D(GLenum target,
				    GLint level,
				    GLint xoffset,
				    GLint yoffset,
				    GLint zoffset,
				    GLint x,
				    GLint y,
				    GLsizei width,
				    GLsizei height)
{
    __GLtexture *tex;
    __GLpixelSpanInfo spanInfo;
    GLint adjustedWidth, adjustedHeight;

    /*
    ** Validate the pixel path for copyImage and to update
    ** the width and height adjustment.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_PIXEL_OP();

    /* Adjust level dimensions to account for pixel path operators */
    adjustedWidth = width + gc->pixel.adjustWidth2D;
    adjustedHeight = height + gc->pixel.adjustHeight2D;

    /* Check arguments and get the right texture level being changed */
    tex = CheckCopyTexSubImageArgs(gc, target, level,
				   xoffset, yoffset, zoffset,
				   x, y, adjustedWidth, adjustedHeight, 3);
    if (!tex) {
	return;
    }
    if (tex->refcount > 2) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /* Copy sub-image data */
    __glInitReadImageSrcInfo(gc, &spanInfo, x, y, width, height);
    InitTexSubImageStore(gc, &spanInfo, tex, level, xoffset, yoffset, zoffset);
    __glInitUnpacker(gc, &spanInfo);
    __glInitPacker(gc, &spanInfo);

    if (!__glClipReadPixels(gc, &spanInfo)) return;
    (*tex->readTexImage)(gc, &spanInfo, tex, level);

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_LIGHTING);
}

/************************************************************************/
/*
** Texture Object routines.
*/
/************************************************************************/

#define __GL_CHECK_VALID_N_PARAM(failStatement)				\
    if (n < 0) {							\
	__glSetError(GL_INVALID_VALUE);					\
    }									\
    if (n == 0) {							\
	failStatement;							\
    }									\


/*
 * Generic texture object free function; called from so_names.c
 */
GLvoid __glDisposeTexObj(__GLcontext *gc, void *pData)
{
    __GLtexture *tex = (__GLtexture *)pData;

    tex->refcount--;
    assert(tex->refcount >= 0);

    if (tex->refcount == 0) {
	(*tex->free)(gc, tex);
    }
}

GLvoid GLAPI __glim_GenTextures(GLsizei n, GLuint* textures)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_CHECK_VALID_N_PARAM(return);
    __GL_API_BLAND();

    if (NULL == textures) return;

    assert(NULL != gc->texture.namesArray);

    __glNamesGenNames(gc, gc->texture.namesArray, n, textures);

}

GLvoid GLAPI __glim_DeleteTextures(GLsizei n, const GLuint* textures)
{
    GLuint start, rangeVal;
    __GLnamesArray *array;
    int i, j, k;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_CHECK_VALID_N_PARAM(return);
    __GL_API_STATE();

    array = gc->texture.namesArray;

    /*
    ** Send the texture names in ranges to the names module to be
    ** deleted.  Ignore any references to default textures.
    ** If a texture that is being deleted is currently bound,
    ** bind the default texture to its target.
    ** The names routine ignores any names that don't refer to
    ** textures.
    */
    start = rangeVal = textures[0];
    for (i=0; i < n; i++, rangeVal++) {
	if (0 == textures[i]) {		/* skip default textures */
	    /* delete up to this one */
	    __glNamesDeleteRange(gc, array, start, rangeVal-start);

	    /* skip over this one by setting start to the next one */
	    start = textures[i+1];
	    rangeVal = start-1;		/* because it gets incremented later */
	    continue;
	}
	/*
	** If the texture is currently bound, bind the defaultTexture
	** to its target.  The problem here is identifying the target.
	** One way is to look up the texture obj with the name.  Another is
	** to look through all of the currently bound textures and
	** check each for the name.  It has been implemented with the
	** assumption that looking through the currently bound textures
	** is faster than retrieving the texture obj that corresponds to
	** the name.
	*/
	for (j=0; j < __GL_NUM_TEXTURE_BINDINGS; j++) {
	    for (k=0; k < __GL_NUM_TEXTURE_BINDINGS; k++) {
		__GLtexture *tex = gc->texture.unit[j].texture[k];

		/* Is the texture currently bound? */
		if (tex->state.name == textures[i]) {
		    /* if we don't unlock it, it won't get deleted */
		    __glNamesUnlockData(gc, tex);

		    /* bind the default texture to this target */
		    gc->texture.unit[j].texture[k] =
		    		gc->texture.defaultTexture[k];

		    /* Need to reset the current texture and such. */
		    __GL_DELAY_VALIDATE(gc);
		    break;
		}
	    }
	}
	if (textures[i] != rangeVal) {
	    /* delete up to this one */
	    __glNamesDeleteRange(gc, array, start, rangeVal-start);
	    start = rangeVal = textures[i];
	}
    }
    __glNamesDeleteRange(gc, array, start, rangeVal-start);
}

/*
** This routine is used by the pick routines to actually perform
** the bind.
*/
void __glBindTexture(__GLcontext *gc,
			GLint unit, GLuint targetIndex, GLuint texture)
{
    __GLtexture *tex;
    __GLtexture *boundTexture;

    assert(NULL != gc->texture.namesArray);

    /*
    ** Retrieve the texture object from the namesArray structure.
    */
    if (0 == texture) {
	tex = gc->texture.defaultTexture[targetIndex];
	assert(NULL != tex);
	assert(0 == tex->state.name);
    } else {
	tex = (__GLtexture *)
		__glNamesLockData(gc, gc->texture.namesArray, texture);
    }

    /*
    ** Is this the first time this name has been bound?
    ** If so, create a new texture object and initialize it.
    */
    if (NULL == tex) {
	GLenum target = __glTextureTargetFromIndex(targetIndex);

	tex = (*gc->texture.createTexture)(gc, texture, target);
	assert(NULL != tex);
	assert(texture == tex->state.name);
	__glNamesNewData(gc, gc->texture.namesArray, texture, tex);
	/*
	** Shortcut way to lock without doing another lookup.
	*/
	tex->refcount++;
    } else {
	GLint dim;
	/*
	** Retrieved an existing texture object.  Do some
	** sanity checks.
	*/
	assert(texture == tex->state.name);
	switch (targetIndex) {
	  case __GL_TEXTURE_1D_INDEX:
	    dim = 1;
	    break;
	  case __GL_TEXTURE_2D_INDEX:
	    dim = 2;
	    break;
	  case __GL_TEXTURE_3D_INDEX:
	    dim = 3;
	    break;
	}
	if (tex->dim != dim) {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    /*
    ** Release texture that is being unbound.
    */
    boundTexture = gc->texture.unit[unit].texture[targetIndex];
    if (boundTexture && (boundTexture->state.name != 0)) {
	__glNamesUnlockData(gc, boundTexture);
    }

    /*
    ** Install the new texture into the correct target in this context.
    */
    gc->state.texture.unit[unit].texture[targetIndex] = &tex->state;
    gc->texture.unit[unit].texture[targetIndex] = tex;
    tex->gc = gc;
}

GLvoid GLAPI __glim_BindTexture(GLenum target, GLuint texture)
{
    GLuint targetIndex;
    /*
    ** Need to validate in case a new texture was popped into
    ** the state immediately prior to this call.
    */
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    __GL_API_STATE();

    switch (target) {
    case GL_TEXTURE_1D:
	targetIndex = __GL_TEXTURE_1D_INDEX;
	break;
    case GL_TEXTURE_2D:
	targetIndex = __GL_TEXTURE_2D_INDEX;
	break;
    case GL_TEXTURE_3D:
	targetIndex = __GL_TEXTURE_3D_INDEX;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __glBindTexture(gc, gc->state.texture.activeTexture, targetIndex, texture);

    /* Need to reset the current texture and such. */
    __GL_DELAY_VALIDATE(gc);
}


GLvoid GLAPI __glim_PrioritizeTextures(GLsizei n,
			   const GLuint* textures,
			   const GLclampf* priorities)
{
    int i;
    __GLtexture *tex;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_CHECK_VALID_N_PARAM(return);
    __GL_API_STATE();

    for (i=0; i < n; i++) {
	/* silently ignore default texture */
	if (0 == textures[i]) continue;

	tex = (__GLtexture *)
	    __glNamesLockData(gc, gc->texture.namesArray, textures[i]);

	/* silently ignore non-texture */
	if (NULL == tex) continue;

	tex->state.params.priority = Clampf(priorities[i], __glZero, __glOne);

	__glNamesUnlockData(gc, tex);
    }

#if 0
    /* XXX reshuffle textures now */
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_TEXTURE_PRIORITIES);
#endif
}

static
GLboolean IsTextureResident(__GLcontext *gc, __GLtexture *tex)
{
    GLint i;
    int maxMipMapLevel = gc->constants.maxMipMapLevel;

    for (i=0; i < maxMipMapLevel; i++) {
	if (!tex->level[i]->buffer) {
	    return GL_FALSE;
	}
	/* If not-mipmapping, return after checking the first level. */
	switch (tex->state.params.minFilter) {
	  case GL_NEAREST:
	  case GL_LINEAR:
	    return GL_TRUE;
	  default:
	    break;
	}
	if (tex->level[i]->width == 1 && tex->level[i]->height == 1) break;
    }

    return GL_TRUE;
}

GLboolean GLAPI __glim_AreTexturesResident(GLsizei n,
			       const GLuint* textures,
			       GLboolean* residences)
{
    int i;
    __GLtexture *tex;
    GLboolean allResident = GL_TRUE;

    __GL_SETUP_NOT_IN_BEGIN2();
    __GL_CHECK_VALID_N_PARAM(return GL_FALSE);
    __GL_API_GET();

    for (i=0; i < n; i++) {
	/* Can't query a default texture. */
	if (0 == textures[i]) {
	    __glSetError(GL_INVALID_VALUE);
	    return GL_FALSE;
	}
	tex = (__GLtexture *)
	    __glNamesLockData(gc, gc->texture.namesArray, textures[i]);
	/*
	** Ensure that all of the names have corresponding textures.
	*/
	if (NULL == tex) {
	    __glSetError(GL_INVALID_VALUE);
	    return GL_FALSE;
	}
	if (!__glIsTextureConsistent(gc, tex) || !IsTextureResident(gc, tex)) {
	    allResident = GL_FALSE;
	    residences[i] = GL_FALSE;
	} else {
	    residences[i] = GL_TRUE;
	}
	__glNamesUnlockData(gc, tex);
    }

    return allResident;
}

GLboolean GLAPI __glim_IsTexture(GLuint texture)
{
    __GLtexture *tex;
    __GL_SETUP_NOT_IN_BEGIN2();
    __GL_API_GET();

    if (0 == texture) return GL_FALSE;

    tex = (__GLtexture *)
	    __glNamesLockData(gc, gc->texture.namesArray, texture);

    if (NULL == tex) return GL_FALSE;

    __glNamesUnlockData(gc, tex);

    return GL_TRUE;
}

/************************************************************************/

/* Reformat textures into framebuffer format for fast undithered REPLACE */
void __glCookSubTexture(__GLcontext *gc, __GLmipMapLevel *lp,
			int x, int y, int w, int h)
{
    GLint row, col, rs, gs, bs, as;
    GLubyte *srcPtr, *dstPtr;
    GLuint srcSkip, dstSkip;
    GLuint dstBpp;
    __GLcolorBuffer *cfb = gc->drawBuffer;

    rs = 8 - gc->modes.redBits;
    gs = 8 - gc->modes.greenBits;
    bs = 8 - gc->modes.blueBits;
    as = 8 - gc->modes.alphaBits;

    /* XXX: accessing drawableBuf outside of a lock */
    dstBpp = gc->drawBuffer->buf.drawableBuf->elementSize;

    srcPtr = ((GLubyte *)lp->buffer) + w*y + x;
    dstPtr = ((GLubyte *)lp->pixelBuffer) + (w*y + x) * dstBpp;

    srcSkip = lp->width - w;
    dstSkip = srcSkip * dstBpp;

    for (row = 0; row < h; row++) {
	for (col = 0; col < w; col++) {
	    GLuint pixel;
	    GLubyte l = 0, r = 0, g = 0, b = 0, a = 0;

	    switch (lp->internalFormat) {
	    case __GL_FORMAT_LUMINANCE8:
		l = *srcPtr++;
		r = l;
		g = l;
		b = l;
		a = 0; /* Uses fragment alpha */
		break;

	    case __GL_FORMAT_LUMINANCE_ALPHA8:
		l = srcPtr[0];
		r = l;
		g = l;
		b = l;
		a = srcPtr[1];
		srcPtr += 2;
		break;

	    case __GL_FORMAT_RGB8:
		r = srcPtr[0];
		g = srcPtr[1];
		b = srcPtr[2];
		a = 0; /* uses fragment alpha */
		srcPtr += 3;
		break;
	    case __GL_FORMAT_RGBA8:
		r = srcPtr[0];
		g = srcPtr[1];
		b = srcPtr[2];
		a = srcPtr[3];
		srcPtr += 4;
		break;
	    case __GL_FORMAT_INTENSITY8:
		l = *srcPtr++;
		r = l;
		g = l;
		b = l;
		a = l;
		break;
	    }

	    r >>= rs;
	    g >>= gs;
	    b >>= bs;
	    a >>= as;

	    pixel =
		(r << cfb->redShift) |
		(g << cfb->greenShift) |
		(b << cfb->blueShift) |
		(a << cfb->alphaShift);

	    switch (dstBpp) {
	    case 1:
		*((GLubyte*)dstPtr) = pixel;
		dstPtr += sizeof(GLubyte);
		break;
	    case 2:
		*((GLushort*)dstPtr) = pixel;
		dstPtr += sizeof(GLushort);
		break;
	    case 4:
		*((GLuint*)dstPtr) = pixel;
		dstPtr += sizeof(GLuint);
		break;
	    }
	}
	srcPtr += srcSkip;
	dstPtr += dstSkip;
    }
}

void __glCookTexture(__GLcontext *gc)
{
    __GLmipMapLevel *lp = gc->texture.active->currentTexture->level[0];
    GLuint pixelCount;
    GLuint bufferSize;
    __GLcolorBuffer *cfb = gc->drawBuffer;

    /* Already cooked! */
    if (lp->pixelBuffer)
	return;

    pixelCount = lp->width * lp->height;
    /* XXX: accessing drawableBuf outside of a lock */
    bufferSize = pixelCount * gc->drawBuffer->buf.drawableBuf->elementSize;

    lp->pixelBuffer = (GLvoid *)
	(*gc->imports.realloc)(gc, lp->pixelBuffer, (size_t) bufferSize);

    __glCookSubTexture(gc, lp, 0, 0, lp->width, lp->height);
}

/************************************************************************/

/* XXX Verify these texture object stubs properly alias 1.1 features! */
GLvoid GLAPI __glim_GenTexturesEXT(GLsizei n, GLuint* textures)
{
    __glim_GenTextures(n, textures);
}

GLvoid GLAPI __glim_DeleteTexturesEXT(GLsizei n, const GLuint* textures)
{
    __glim_DeleteTextures(n, textures);
}

GLboolean GLAPI __glim_AreTexturesResidentEXT(GLsizei n,
			       const GLuint* textures,
			       GLboolean* residences)
{
    return __glim_AreTexturesResident(n, textures, residences);
}

GLboolean GLAPI __glim_IsTextureEXT(GLuint texture)
{
    return __glim_IsTexture(texture);
}
