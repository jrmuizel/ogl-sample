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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_texmgr.c#18 $
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
#include "texfmt.h"
#include "texmgr.h"
#include "texture.h"
#include <memory.h>


/* The texture manager */
static __GLtextureManager __glTextureManager =
{
    0,
    __glFreeTextureManager,
    __glInvalidateAllTextures,
};


void __glInitTextureManager(__GLcontext *gc)
{
    __GLtextureManager *texmgr;

    texmgr = (*gc->imports.malloc)(gc, sizeof(__GLtextureManager));
    *texmgr = __glTextureManager;
    
    texmgr->refcount++;
    gc->texture.textureManager = texmgr;

    gc->texture.createTexture = __glCreateTexture;
}

void __glFreeTextureManager(__GLcontext *gc)
{
    __GLtextureManager *texmgr = gc->texture.textureManager;
    texmgr->refcount--;
    if (0 == texmgr->refcount) {
	/* Delete allocated resources */
	(*gc->imports.free)(gc, texmgr);
    }
    gc->texture.textureManager = NULL;
}

/* Tell the texture manager to invalidate all active textures */
GLboolean __glInvalidateAllTextures(__GLcontext *gc, 
				    __GLtextureManager *texmgr)
{
    /* in this implementation, just say OK */
    return GL_TRUE;
}

void __glShareTextureManager(__GLcontext *dst, __GLcontext *src)
{
    /* First we free the texture manager of the dst context */
    __glFreeTextureManager(dst);

    /* and set the dst texture manager to be the src one */
    dst->texture.textureManager = src->texture.textureManager;
    dst->texture.textureManager->refcount++;
}

/*******************************************************************/
/*                   Texture object management                     */
/*******************************************************************/


__GLtexture *
__glCreateTexture(__GLcontext *gc, GLuint name, GLenum target)
{
    __GLtexture *tex;
    GLint level, maxMipMapLevel;
    __GLmipMapLevel *lp;

    tex = (__GLtexture *) (*gc->imports.calloc)(gc, 1, sizeof(__GLtexture));
    assert(NULL != tex);

    tex->refcount = 1;

    /* Set default parameter state */
    tex->state.params.sWrapMode = GL_REPEAT;
    tex->state.params.tWrapMode = GL_REPEAT;
    tex->state.params.rWrapMode = GL_REPEAT;
    tex->state.params.minFilter = GL_NEAREST_MIPMAP_LINEAR;
    tex->state.params.magFilter = GL_LINEAR;
    tex->state.params.priority = 1.0F;
    tex->state.params.baseLevel = 0;
    tex->state.params.maxLevel = 1000;
    tex->state.params.minLOD = -1000.0F;
    tex->state.params.maxLOD = 1000.0F;

    tex->state.name = name;

    tex->gc = gc;
    tex->residence = __GL_TEXTURE_NOT_RESIDENT;
    tex->free = __glFreeTexture;
    tex->makeResident = __glMakeResidentTexture;
    tex->unmakeResident = __glUnmakeResidentTexture;
    tex->copyTexImage = __glCopyTexImage;
    tex->readTexImage = __glReadTexImage;
    tex->CT.format = GL_RGBA;
    tex->adjustedMinLOD = -__GL_MAX_FLOAT;
    tex->adjustedMaxLOD = __GL_MAX_FLOAT;

    switch (target) {
      case GL_TEXTURE_1D:
	tex->dim = 1;
	tex->createLevel  = __glTextureCreateLevel;
	tex->CT.target = GL_TEXTURE_1D;
	break;
      case GL_TEXTURE_2D:
	tex->dim = 2;
	tex->createLevel  = __glTextureCreateLevel;
	tex->CT.target = GL_TEXTURE_2D;
	break;
      case GL_TEXTURE_3D:
	tex->dim = 3;
	tex->createLevel  = __glTextureCreateLevel;
	tex->CT.target = GL_TEXTURE_3D;
	break;
      case GL_PROXY_TEXTURE_1D:
	tex->dim = 1;
	tex->createLevel  = __glTextureCreateProxyLevel;
	break;
      case GL_PROXY_TEXTURE_2D:
	tex->dim = 2;
	tex->createLevel  = __glTextureCreateProxyLevel;
	break;
      case GL_PROXY_TEXTURE_3D:
	tex->dim = 3;
	tex->createLevel  = __glTextureCreateProxyLevel;
	break;
      default:
	break;
    }
    tex->releaseLevel = __glTextureDeleteLevel;

    maxMipMapLevel = gc->constants.maxMipMapLevel;
    tex->level = (__GLmipMapLevel**)
	(*gc->imports.calloc)(gc, (size_t) maxMipMapLevel,
			      sizeof(__GLmipMapLevel*));

    tex->level[0] = (__GLmipMapLevel*)
	(*gc->imports.calloc)(gc, (size_t) maxMipMapLevel,
			      sizeof(__GLmipMapLevel));

    /* Init each texture level */
    for (level = 0, lp = tex->level[0];
	 level < maxMipMapLevel; level++, lp++) {
	tex->level[level] = lp;
	tex->level[level]->requestedFormat = 1;
    }

    return (__GLtexture *)tex;
}


/*
 * Texture Object methods
 */
void __glFreeTexture(__GLcontext *gc, __GLtexture *tex)
{
    GLint level, maxLevel;

    assert(tex->refcount == 0);

    maxLevel = gc->constants.maxMipMapLevel;
    for (level = 0; level < maxLevel; level++) {
	(*tex->releaseLevel)(gc, tex, level);
    }
    (*gc->imports.free)(gc, tex->level[0]);
    (*gc->imports.free)(gc, tex->level);
    (*gc->imports.free)(gc, tex);
}


/*ARGSUSED*/
GLvoid __glCopyTexImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			__GLtexture *tex, GLint lod)
{
    (*gc->procs.copyImage)(gc, spanInfo);

    /* resident texture is out of date.  remove it */
    (*tex->unmakeResident)(gc, tex);
}


/*ARGSUSED*/
GLvoid __glReadTexImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			__GLtexture *tex, GLint lod)
{
    __GL_LOCK_PXL_READ_BUFFER(gc, 0);
    (*gc->procs.readImage)(gc, tex->level[lod], spanInfo);
    __GL_UNLOCK_PXL_READ_BUFFER(gc, 0);

    /* resident texture is out of date.  remove it */
    (*tex->unmakeResident)(gc, tex);
}


GLenum
__glMakeResidentTexture(__GLcontext *gc, __GLtexture *tex, float priority)
{
    return __GL_TEXTURE_RESIDENCE_LOADED;
}

GLboolean
__glUnmakeResidentTexture(__GLcontext *gc, __GLtexture *tex)
{
    return GL_TRUE;
}


__GLtextureBuffer *
__glTextureCreateLevel(__GLcontext *gc, __GLtexture *tex,
		       GLint lod, GLint components,
		       GLsizei w, GLsizei h, GLsizei d,
		       GLint border, GLint dim)
{
    __GLmipMapLevel   *lp = tex->level[lod];
    const __GLtextureFormat *texFormat;
    GLenum baseFormat;
    GLint baseWidth  = (w-border*2) * (1 << lod);
    GLint baseHeight = (h-border*2) * (1 << lod);
    GLint baseDepth = (d-border*2) * (1 << lod);
    GLint bufferSize;
    GLint maxDim;

    if (baseWidth > baseHeight) {
         maxDim = baseWidth;
    } else {
	maxDim = baseHeight;
    }
    if (baseDepth > maxDim) {
	maxDim = baseDepth;
    }

    tex->numLevels = __glFloorLog2(maxDim)+1;

    texFormat = (const __GLtextureFormat *)
	(*gc->texture.lookupTextureFormat)(components, &baseFormat);
    assert(NULL != texFormat);
    /*
    ** XXX: We allocate extra space to work aroung an issue with the
    ** OG texel extraction code when using RGB8 textures.
    */
    bufferSize = w * h * d * ((texFormat->bitsPerTexel+7) >> 3) + 1;

    if (baseWidth > gc->constants.maxTextureSize ||
	baseHeight > gc->constants.maxTextureSize ||
	baseDepth > gc->constants.maxTextureSize) {
	/* Texture allocation failed */
	__glSetError(GL_INVALID_VALUE);
	return 0;
    } else if (bufferSize) {
	/* Texture allocation succeeded, fill in new level info */
	lp->buffer = (__GLtextureBuffer*)
	    (*gc->imports.realloc)(gc, lp->buffer, (size_t) bufferSize);
	
	if (lp->buffer == NULL) {
	    __glSetError(GL_OUT_OF_MEMORY);
	    return 0;
	}

	/* This is allocated lazily */
	if (lp->pixelBuffer != NULL) {
	    (*gc->imports.free)(gc, lp->pixelBuffer);
	    lp->pixelBuffer = NULL;
	}

	lp->width = w;
	lp->height = h;
	lp->depth = d;
	lp->imageSize = w * h;
	lp->width2 = w - border*2;
	lp->widthLog2 = __glFloorLog2(lp->width2);
	lp->height2 = h - border*2;
	lp->heightLog2 =__glFloorLog2(lp->height2);
	lp->depth2 = d - border*2;
	lp->depthLog2 =__glFloorLog2(lp->depth2);
	lp->width2f = lp->width2;
	lp->height2f = lp->height2;
	lp->depth2f = lp->depth2;
	lp->border = border;
	lp->texFormat = texFormat;
	lp->requestedFormat = (GLenum) components;
	lp->baseFormat = baseFormat;
	lp->internalFormat = texFormat->internalFormat;
	lp->redSize = texFormat->redSize;
	lp->greenSize = texFormat->greenSize;
	lp->blueSize = texFormat->blueSize;
	lp->alphaSize = texFormat->alphaSize;
	lp->luminanceSize = texFormat->luminanceSize;
	lp->intensitySize = texFormat->intensitySize;
	if (dim < 3) {
	    if (border) {
		lp->extract = texFormat->extractTexelBorder;
	    } else {
		lp->extract = texFormat->extractTexel;
	    }
	} else {
	    if (border) {
		lp->extract = texFormat->extractTexelBorder3D;
	    } else {
		lp->extract = texFormat->extractTexel3D;
	    }
	}
    } else {
	/* The texture level is being freed */
	if (lp->buffer != NULL) {
	    (*gc->imports.free)(gc, lp->buffer);
	    lp->buffer = NULL;
	}
	if (lp->pixelBuffer != NULL) {
	    (*gc->imports.free)(gc, lp->pixelBuffer);
	    lp->pixelBuffer = NULL;
	}

	lp->width = 0;
	lp->height = 0;
	lp->depth = 0;
	lp->imageSize = 0;
	lp->width2 = 0;
	lp->height2 = 0;
	lp->depth2 = 0;
	lp->widthLog2 = 0;
	lp->heightLog2 = 0;
	lp->depthLog2 = 0;
	lp->border = 0;
	lp->texFormat = 0;
	lp->requestedFormat = 1;
	lp->baseFormat = 0;
	lp->internalFormat = 0;
	lp->redSize = 0;
	lp->greenSize = 0;
	lp->blueSize = 0;
	lp->alphaSize = 0;
	lp->luminanceSize = 0;
	lp->intensitySize = 0;
	lp->extract = (void (*)(__GLmipMapLevel *, __GLtexture *,
				GLint, GLint, GLint, __GLtexel *)) __glNop6;
    }

    return lp->buffer;
}


__GLtextureBuffer *
__glTextureCreateProxyLevel(__GLcontext *gc, __GLtexture *tex,
			    GLint lod, GLint components,
			    GLsizei w, GLsizei h, GLsizei d,
			    GLint border, GLint dim)
{
    __GLmipMapLevel *lp = tex->level[lod];
    const __GLtextureFormat *texFormat;
    GLenum baseFormat;
    GLint baseWidth= (w-border*2) * (1 << lod);
    GLint baseHeight= (h-border*2) * (1 << lod);
    GLint baseDepth= (d-border*2) * (1 << lod);

    texFormat = (const __GLtextureFormat *)
	(*gc->texture.lookupTextureFormat)(components, &baseFormat);
    assert(NULL != texFormat);

    if (baseWidth > gc->constants.maxTextureSize ||
	baseHeight > gc->constants.maxTextureSize ||
	baseDepth > gc->constants.maxTextureSize) {
	/* Proxy allocation failed */
	lp->width = 0;
	lp->height = 0;
	lp->depth = 0;
	lp->border = 0;
	lp->texFormat = 0;
	lp->requestedFormat = 0;
	lp->baseFormat = 0;
	lp->internalFormat = 0;
	lp->redSize = 0;
	lp->greenSize = 0;
	lp->blueSize = 0;
	lp->alphaSize = 0;
	lp->luminanceSize = 0;
	lp->intensitySize = 0;
    } else {
	/* Proxy allocation succeeded */
	lp->width = w;
	lp->height = h;
	lp->depth = d;
	lp->border = border;
	lp->texFormat = texFormat;
	lp->requestedFormat = (GLenum) components;
	lp->baseFormat = baseFormat;
	lp->internalFormat = texFormat->internalFormat;
	lp->redSize = texFormat->redSize;
	lp->greenSize = texFormat->greenSize;
	lp->blueSize = texFormat->blueSize;
	lp->alphaSize = texFormat->alphaSize;
	lp->luminanceSize = texFormat->luminanceSize;
	lp->intensitySize = texFormat->intensitySize;
    }
    lp->extract = (void (*)(__GLmipMapLevel *, __GLtexture *,
			GLint, GLint, GLint, __GLtexel *)) __glNop6;
    return 0;
}


void __glTextureDeleteLevel(__GLcontext *gc, __GLtexture *tex, GLint lod)
{
    __GLmipMapLevel *lp = tex->level[lod];

    if (lp->buffer) {
	(*gc->imports.free)(gc, lp->buffer);
	lp->buffer = NULL;
    }

    if (lp->pixelBuffer) {
	(*gc->imports.free)(gc, lp->pixelBuffer);
	lp->pixelBuffer = NULL;
    }
}
