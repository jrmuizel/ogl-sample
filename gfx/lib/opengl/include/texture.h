#ifndef __gl_texture_h_
#define __gl_texture_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/texture.h#18 $
*/
#include "types.h"
#include "ctable.h"
#include "texfmt.h"

/*
** Number of texture units.  Each texture unit includes a complete
** set of texture bindings and a complete set of environment bindings.
*/
#define __GL_NUM_TEXTURE_UNITS		4

/*
** Number of texture bindings supported by each texture unit.
*/
#define __GL_NUM_TEXTURE_BINDINGS	3
#define __GL_TEXTURE_1D_INDEX		0
#define __GL_TEXTURE_2D_INDEX		1
#define __GL_TEXTURE_3D_INDEX		2

/*
** Number of environment bindings supported by each texture unit.
*/
#define __GL_NUM_TEXTURE_ENV_BINDINGS	1

/************************************************************************/

/*
** State set with glTexGen
*/
typedef struct __GLtextureGenStateRec {
    /* How coordinates are being generated */
    GLenum mode;

    /* eye plane equation (used iff mode == GL_EYE_LINEAR) */
    __GLcoord eyePlaneEquation;

    /* object plane equation (used iff mode == GL_OBJECT_LINEAR) */
    __GLcoord objectPlaneEquation;
} __GLtextureGenState;

/*
** State set with glTexEnv
*/
typedef struct __GLtextureEnvStateRec {
    /* environment "blend" function */
    GLenum mode;

    /* environment color */
    __GLcolor color;
} __GLtextureEnvState;

/*
** State set with glTexParameter
*/
typedef struct __GLtextureParamStateRec {
    /* wrap modes */
    GLenum sWrapMode;
    GLenum tWrapMode;
    GLenum rWrapMode;

    /* min and mag filter */
    GLenum minFilter;
    GLenum magFilter;

    /* border color */
    __GLcolor borderColor;	/* Unscaled! */

    /* priority */
    GLfloat priority;

    /* level of detail controls */
    GLint baseLevel;
    GLint maxLevel;
    GLfloat minLOD;
    GLfloat maxLOD;
} __GLtextureParamState;

/*
** Stackable texture binding state
*/
typedef struct __GLtextureBindingStateRec {
    /* Texture name */
    GLuint name;

    /* Parameter state */
    __GLtextureParamState params;
} __GLtextureBindingState;

/*
** Stackable texture state for each texture unit
*/
typedef struct __GLtextureUnitStateRec {
    /* Per coordinate texture state (set with glTexGen) */
    __GLtextureGenState s;
    __GLtextureGenState t;
    __GLtextureGenState r;
    __GLtextureGenState q;

    /* Per texture object binding state (points to bound texture object) */
    __GLtextureBindingState *texture[__GL_NUM_TEXTURE_BINDINGS];

    /* Per texture environment binding state */
    __GLtextureEnvState env[__GL_NUM_TEXTURE_ENV_BINDINGS];
} __GLtextureUnitState;

/*
** Stackable texture state. This does not include
** the mipmaps, or level dependent state.  Only state which is
** stackable via glPushAttrib/glPopAttrib is here.  The rest of the
** state is in the machine structure below.
*/
typedef struct __GLtextureStateRec {
    /* Current active texture unit (stored as an index not an enum) */
    GLint activeTexture;

    /* Pointer to current active texture unit */
    __GLtextureUnitState *active;

    /* Current state for all texture units */
    __GLtextureUnitState unit[__GL_NUM_TEXTURE_UNITS];
} __GLtextureState;

/*
** Stackable client texture state.  This is the texture state which
** is stackable via glPushClientAttrib/glPopClientAttrib.
*/
typedef struct __GLclientTextureStateRec {
    /* Current client active texture unit */
    GLenum activeTexture;
} __GLclientTextureState;

/************************************************************************/

typedef GLubyte __GLtextureBuffer;

struct __GLtexelRec {
    GLubyte r, g, b, a;
};

/************************************************************************/

/*
** Per Mipmap Level state.
*/
struct __GLmipMapLevelRec {
    /* These are the texels for this mipmap level */
    __GLtextureBuffer *buffer;

    /* Alternate representation in pixel format */
    GLvoid *pixelBuffer;

    /* Image dimensions, including border */
    GLint width, height, depth, imageSize;

    /* Image dimensions, doesn't include border */
    GLint width2, height2, depth2;
    __GLfloat width2f, height2f, depth2f;

    /* log2 of width2 & height2 */
    GLint widthLog2, heightLog2, depthLog2;

    /* Border size */
    GLint border;

    /* Requested internal format */
    GLenum requestedFormat;

    /* Base internal format */
    GLenum baseFormat;

    /* Actual internal format */
    GLenum internalFormat;

    /* Component resolution */
    GLint redSize;
    GLint greenSize;
    GLint blueSize;
    GLint alphaSize;
    GLint luminanceSize;
    GLint intensitySize;

    const __GLtextureFormat *texFormat;

    /* Extract function for this mipmap level */
    void (*extract)(__GLmipMapLevel *level, __GLtexture *tex,
		    GLint img, GLint row, GLint col, __GLtexel *result);
};

/*
** Per Texture Object state.
** refcount field MUST be first in the structure.
*/
struct __GLtextureRec {
    /* reference count: create with 1; delete when 0 */
    GLint refcount;

    /* Back pointer to context (XXX could be bound to multiple contexts) */
    __GLcontext *gc;

    /* State for this texture object */
    __GLtextureBindingState state;

    /* Dimension of this texture (1, 2 or 3) */
    GLint dim;

    /* Level information */
    GLint numLevels;
    __GLmipMapLevel **level;

    GLenum residence;

    GLint onebyoneLevel;
    __GLfloat effectiveMaxLOD;
    __GLfloat adjustedMinLOD, adjustedMaxLOD;

    /* Min/Mag switchover point */
    __GLfloat c;

    /* Color table for EXT_paletted_texture */
    __GLcolorTable CT;

    /* Format of texel (after palette (if any) has been applied) */
    GLenum baseFormat;

    /* 
     * This should only be called by the texture object when it is 
     * deleted. It calls the texture managers delete textureobj func
     */
    void (*free)(__GLcontext *gc, __GLtexture *tex);
	       
    /* Make texture resident.  Return GL_TRUE if success */
    GLenum (*makeResident)(__GLcontext *gc, __GLtexture *tex, 
			   GLfloat priority);

    /* Unmake texture resident.  Return GL_TRUE if success */
    GLboolean (*unmakeResident)(__GLcontext *gc, __GLtexture *tex);
      
    /* Create a new mipmap level for this texture */
    __GLtextureBuffer * (*createLevel)(__GLcontext *gc, __GLtexture *tex,
				       GLint lod, GLint components,
				       GLsizei w, GLsizei h, GLsizei d,
				       GLint border, GLint dim);

    /* Release storage for a mipmap level */
    void (*releaseLevel)(__GLcontext *gc, __GLtexture *tex, GLint lod);

    /* Copy data to / Read data from texture memory */
    void (*copyTexImage)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 __GLtexture *tex, GLint lod);
    void (*readTexImage)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 __GLtexture *tex, GLint lod);

    /* Texturing function for this texture */
    void (*textureFunc)(__GLcontext *gc, __GLtexture *tex, __GLcolor *color,
			__GLfloat s, __GLfloat t, __GLfloat r, __GLfloat rho);

    /* Apply current environment function to fragment */
    void (*env)(__GLcontext *gc, __GLcolor *color, __GLtexel *texel);

    /* Magnification routine for this texture */
    void (*magnify)(__GLtexture *tex, __GLfloat lod,
		    __GLfloat s, __GLfloat t, __GLfloat r, __GLtexel *result);

    /* Minification routine for this texture */
    void (*minnify)(__GLtexture *tex, __GLfloat lod,
		    __GLfloat s, __GLfloat t, __GLfloat r, __GLtexel *result);

    /* Linear filter for this texture */
    void (*linear)(__GLtexture *tex, __GLmipMapLevel *lp,
		   __GLfloat s, __GLfloat t, __GLfloat r, __GLtexel *result);

    /* Nearest filter for this texture */
    void (*nearest)(__GLtexture *tex, __GLmipMapLevel *lp,
		    __GLfloat s, __GLfloat t, __GLfloat r, __GLtexel *result);
};

/*
** Per Texture Unit rendering state.
*/
typedef struct __GLtextureUnitMachineRec {
    /* Array of currently bound texture objects */
    __GLtexture *texture[__GL_NUM_TEXTURE_BINDINGS];

    /* Current enabled texture (NULL or one of the currently bound textures) */
    __GLtexture *currentTexture;

    /* The OR of all texture enable bits for this unit */
    GLboolean textureEnabled;

    /* GLubyte version of texture environment color */
    GLubyte envColor[4];
} __GLtextureUnitMachine;

/*
** Per Context rendering state.
*/
typedef struct __GLtextureMachineRec {
    /* Texture objects for the default textures */
    __GLtexture *defaultTexture[__GL_NUM_TEXTURE_BINDINGS];

    /* Texture objects for proxy textures */
    __GLtexture *proxyTexture[__GL_NUM_TEXTURE_BINDINGS];

    /* Pointer to current active texture unit rendering state */
    __GLtextureUnitMachine *active;

    /* Rendering state for each texture unit */
    __GLtextureUnitMachine unit[__GL_NUM_TEXTURE_UNITS];

    /* The OR of all texture enable bits for this unit */
    GLboolean textureEnabled;

    /* Stores pointers to texture objects, retrieved by name */
    __GLnamesArray *namesArray;

    struct __GLtextureManagerRec *textureManager;

    /* Early initialization function for texture manager */
    void (*initTextureManager)(__GLcontext *gc);

    /* Lookup a texture format */
    const __GLtextureFormat *(*lookupTextureFormat)(const GLenum components, 
						    GLenum *baseFormat);

    /* Create a texture with the given name and target */
    __GLtexture *(*createTexture)(__GLcontext *gc, GLuint name,
				  GLuint targetIndex);
} __GLtextureMachine;

/************************************************************************/

/* Check for texture consistency before enabling texturing */
extern GLboolean __glIsTextureConsistent(__GLcontext *gc, __GLtexture *tex);

/* Fragment texturing routines */
extern void __glFastTextureFragment(__GLcontext *gc, __GLtexture *tex,
				__GLcolor *color,
				__GLfloat s, __GLfloat t, __GLfloat r,
				__GLfloat rho);
extern void __glTextureFragment(__GLcontext *gc, __GLtexture *tex,
				__GLcolor *color,
				__GLfloat s, __GLfloat t, __GLfloat r,
				__GLfloat rho);
extern void __glMipMapFragment(__GLcontext *gc, __GLtexture *tex,
				__GLcolor *color,
				__GLfloat s, __GLfloat t, __GLfloat r,
				__GLfloat rho);
extern void __glMultiTextureFragment(__GLcontext *gc, __GLcolor *color,
				__GLcoord strq[__GL_NUM_TEXTURE_UNITS],
				__GLfloat rhow[__GL_NUM_TEXTURE_UNITS]);
extern void __glMultiTextureFragmentUVScale(__GLcontext *gc, __GLcolor *color,
				__GLcoord strq[__GL_NUM_TEXTURE_UNITS],
				__GLfloat rhow[__GL_NUM_TEXTURE_UNITS]);

/* Texturing routines */
extern void __glLinearFilterUVScaled(__GLtexture *tex, __GLfloat lod,
			     __GLfloat s, __GLfloat t, __GLfloat r,
			     __GLtexel *result);
extern void __glNearestFilterUVScaled(__GLtexture *tex, __GLfloat lod,
			      __GLfloat s, __GLfloat t, __GLfloat r,
			      __GLtexel *result);
extern void __glLinearFilter(__GLtexture *tex, __GLfloat lod,
			     __GLfloat s, __GLfloat t, __GLfloat r,
			     __GLtexel *result);
extern void __glNearestFilter(__GLtexture *tex, __GLfloat lod,
			      __GLfloat s, __GLfloat t, __GLfloat r,
			      __GLtexel *result);
extern void __glNMNFilter(__GLtexture *tex, __GLfloat lod,
			  __GLfloat s, __GLfloat t, __GLfloat r,
			  __GLtexel *result);
extern void __glLMNFilter(__GLtexture *tex, __GLfloat lod,
			  __GLfloat s, __GLfloat t, __GLfloat r,
			  __GLtexel *result);
extern void __glNMLFilter(__GLtexture *tex, __GLfloat lod,
			  __GLfloat s, __GLfloat t, __GLfloat r,
			  __GLtexel *result);
extern void __glLMLFilter(__GLtexture *tex, __GLfloat lod,
			  __GLfloat s, __GLfloat t, __GLfloat r,
			  __GLtexel *result);

/* Filter routines */
extern void __glLinearFilter1(__GLtexture *tex, __GLmipMapLevel *lp,
			      __GLfloat s, __GLfloat t, __GLfloat r,
			      __GLtexel *result);
extern void __glLinearFilter2(__GLtexture *tex, __GLmipMapLevel *lp,
			      __GLfloat s, __GLfloat t, __GLfloat r,
			      __GLtexel *result);
extern void __glLinearFilter3(__GLtexture *tex, __GLmipMapLevel *lp,
			      __GLfloat s, __GLfloat t, __GLfloat r,
			      __GLtexel *result);
extern void __glNearestFilter1(__GLtexture *tex, __GLmipMapLevel *lp,
			       __GLfloat s, __GLfloat t, __GLfloat r,
			       __GLtexel *result);
extern void __glNearestFilter2(__GLtexture *tex, __GLmipMapLevel *lp,
			       __GLfloat s, __GLfloat t, __GLfloat r,
			       __GLtexel *result);
extern void __glNearestFilter3(__GLtexture *tex, __GLmipMapLevel *lp,
			       __GLfloat s, __GLfloat t, __GLfloat r,
			       __GLtexel *result);


/* Texture generation procs */
extern void __glCalcEyeLinear(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcObjectLinear(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcSphereMap(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcMixedTexture(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcTexture(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcTexturePersp(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcTextureUVScale(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcTexturePerspUVScale(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glFastCalcTexturePersp(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glFastCalcTextureUVScale(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glFastCalcTexturePerspUVScale(__GLcontext *gc, GLint unit, __GLvertex *v);
extern void __glCalcMultiTexture(__GLcontext *gc, __GLvertex *v);
extern void __glValidateTextureModelView(__GLcontext *gc);

/* Texture environment functions */
extern void __glTextureModulateL(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureModulateLA(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureModulateRGB(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureModulateRGBA(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureModulateA(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureModulateI(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureModulateCI(__GLcontext *gc, __GLcolor *color,
				  __GLtexel *tx);

extern void __glTextureDecalRGB(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);
extern void __glTextureDecalRGBA(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);

extern void __glTextureBlendL(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);
extern void __glTextureBlendLA(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);
extern void __glTextureBlendRGB(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);
extern void __glTextureBlendRGBA(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);
extern void __glTextureBlendA(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);
extern void __glTextureBlendI(__GLcontext *gc, __GLcolor *color,
			      __GLtexel *tx);

extern void __glTextureReplaceL(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureReplaceLA(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureReplaceRGB(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureReplaceRGBA(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureReplaceA(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureReplaceI(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *tx);
extern void __glTextureReplaceCI(__GLcontext *gc, __GLcolor *color,
				 __GLtexel *texel);
extern void __glTextureAddCI(__GLcontext *gc, __GLcolor *color,
			     __GLtexel *texel);

/* Extract a texel from a texture level (no border) */
extern void __glExtractTexelL8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelLA8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGB8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGBA8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelA8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelI8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelXRGB1555(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGB332(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGB565(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGBA4(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelARGB4(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGBA5551(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelARGB1555(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelCI8(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelCI16(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);

/* Extract a texel from a texture level (the texture has a border) */
extern void __glExtractTexelL8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelLA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGB8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGBA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelXRGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGB332_B(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGB565_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGBA4_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelARGB4_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelRGBA5551_B(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelARGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelCI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexelCI16_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);

/* Extract a 3D texel from a texture level (no border) */
extern void __glExtractTexel3DL8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DLA8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGB8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGBA8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DA8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DI8(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DXRGB1555(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGB332(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGB565(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGBA4(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DARGB4(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGBA5551(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DARGB1555(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DCI8(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DCI16(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);

/* Extract a 3D texel from a texture level (the texture has a border) */
extern void __glExtractTexel3DL8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DLA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGB8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGBA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DXRGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGB332_B(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGB565_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGBA4_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DARGB4_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DRGBA5551_B(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DARGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			      GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DCI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);
extern void __glExtractTexel3DCI16_B(__GLmipMapLevel *level, __GLtexture *tex,
			       GLint img, GLint row, GLint col, __GLtexel *res);

/* For getting texture images */
extern void __glInitTexDestPack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				GLsizei width, GLsizei height, GLsizei depth,
				GLenum format, GLenum type,
				GLvoid *buf);
extern void __glInitTexImageGet(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				__GLtexture *tex, GLint lod);

extern GLvoid __glCopyTexImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       __GLtexture *tex, GLint lod);
extern GLvoid __glReadTexImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       __GLtexture *tex, GLint lod);

/* List execution texture image code */
extern void __gllei_TexImage1D(__GLcontext *gc, GLenum target, GLint lod,
			       GLint components,
			       GLsizei length, GLint border,
			       GLenum format, GLenum type,
			       const GLubyte *image);
extern void __gllei_TexImage2D(__GLcontext *gc, GLenum target, GLint lod,
			       GLint components,
			       GLsizei w, GLsizei h, GLint border,
			       GLenum format, GLenum type,
			       const GLubyte *image);
extern void __gllei_TexImage3D(__GLcontext *gc, GLenum target, GLint lod,
			       GLint components,
			       GLsizei w, GLsizei h, GLsizei d, GLint border,
			       GLenum format, GLenum type,
			       const GLubyte *image);


extern void __gllei_TexSubImage1D(__GLcontext *gc, GLenum target, GLint lod,
				     GLint xoffset, GLsizei length,
				     GLenum format, GLenum type,
				     const GLubyte *image);
extern void __gllei_TexSubImage2D(__GLcontext *gc, GLenum target, GLint lod, 
				     GLint xoffset, GLint yoffset,
				     GLsizei w, GLsizei h,
				     GLenum format, GLenum type,
				     const GLubyte *image);
extern void __gllei_TexSubImage3D(__GLcontext *gc, GLenum target, GLint lod, 
				     GLint xoffset, GLint yoffset,
				     GLint zoffset, GLsizei w, GLsizei h,
				     GLsizei d, GLenum format, GLenum type,
				     const GLubyte *image);

/* Rho calculation routines */
extern __GLfloat __glComputeLineRho(__GLcontext *gc, GLint unit,
				    __GLfloat s, __GLfloat t, __GLfloat r,
				    __GLfloat wInv);
extern __GLfloat __glNopLineRho(__GLcontext *gc, GLint unit,
				__GLfloat s, __GLfloat t, __GLfloat r,
				__GLfloat wInv);
extern __GLfloat __glComputePolygonRho(__GLcontext *gc, GLint unit,
				       const __GLshade *sh,
				       __GLfloat s, __GLfloat t, __GLfloat r,
				       __GLfloat winv);
extern __GLfloat __glNopPolygonRho(__GLcontext *gc, GLint unit,
				   const __GLshade *sh,
				   __GLfloat s, __GLfloat t, __GLfloat r,
				   __GLfloat winv);

extern __GLtexture *__glCheckTexImage1DArgs(__GLcontext *gc, GLenum target,
					    GLint lod, GLint components,
					    GLsizei length, GLint border,
					    GLenum format, GLenum type);

extern __GLtexture *__glCheckTexImage2DArgs(__GLcontext *gc, GLenum target,
					    GLint lod, GLint components,
					    GLsizei w, GLsizei h, GLint border,
					    GLenum format, GLenum type);

extern __GLtexture *__glCheckTexImage3DArgs(__GLcontext *gc, GLenum target,
					    GLint lod, GLint components,
					    GLsizei w, GLsizei h, GLsizei d,
					    GLint border, GLenum format,
					    GLenum type);

/* Texture Lookup */
GLenum __glTextureTargetFromIndex(int index);
GLenum __glProxyTextureTargetFromIndex(int index);
extern __GLtextureParamState *__glLookupActiveTextureParams(__GLcontext *gc,
						      GLenum target);
extern __GLtexture *__glLookupActiveTexture(__GLcontext *gc, GLenum target);
extern __GLtextureParamState *__glLookupTextureParams(__GLcontext *gc,
						      GLint unit,
						      GLenum target);
extern __GLtexture *__glLookupTexture(__GLcontext *gc,
				      GLint unit,
				      GLenum target);

/* Texture Initialization */
extern void __glEarlyInitTextureState(__GLcontext *gc);
/* Used to share texture objects. */
extern void __glShareTextureObjects(__GLcontext *gc, __GLcontext *shareMe);

/* Texture Object management */
extern GLvoid __glFreeTexObj(__GLtexture *texobj);
extern GLvoid __glDisposeTexObj(__GLcontext *gc, void *pData);

/* Bind Texture used by pop or entry point. */
extern void __glBindTexture(__GLcontext *gc,
				GLint unit, GLuint targetIndex, GLuint name);

/* Create specially formatted texture to match framebuffer format */
void __glCookTexture(__GLcontext *gc);
void __glCookSubTexture(__GLcontext *gc, __GLmipMapLevel *lp,
			int x, int y, int w, int h);

#endif /* __gl_texture_h_ */
