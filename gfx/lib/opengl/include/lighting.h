#ifndef __gl_lighting_h_
#define __gl_lighting_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/lighting.h#7 $
*/
#include "types.h"

/*
** Light state.  Contains all the user controllable lighting state.
** Most of the colors kept in user state are scaled to match the
** drawing surfaces color resolution.
*/

struct __GLmaterialStateRec {
    __GLcolor ambient;			/* unscaled */
    __GLcolor diffuse;			/* unscaled */
    __GLcolor specular;			/* unscaled */
    __GLcolor emissive;			/* scaled */
    __GLfloat specularExponent;
    __GLfloat cmapa, cmaps, cmapd;
};

struct __GLlightModelStateRec {
    __GLcolor ambient;			/* scaled */
    GLboolean localViewer;
    GLboolean twoSided;
    GLenum    colorControl;
};

typedef struct {
    __GLcolor ambient;			/* scaled */
    __GLcolor diffuse;			/* scaled */
    __GLcolor specular;			/* scaled */
    __GLcoord position;
    __GLcoord positionEye;
    __GLcoord direction;
    __GLfloat spotLightExponent;
    __GLfloat spotLightCutOffAngle;
    __GLfloat constantAttenuation;
    __GLfloat linearAttenuation;
    __GLfloat quadraticAttenuation;
} __GLlightSourceState;

typedef struct {
    GLenum colorMaterialFace;
    GLenum colorMaterialParam;
    GLenum shadingModel;
    __GLlightModelState model;
    __GLmaterialState front;
    __GLmaterialState back;
    __GLlightSourceState *source;
} __GLlightState;

/************************************************************************/

/*
** What bits are affected by color index anti-aliasing.  This isn't a
** really a changeable parameter (it is defined by the spec), but it
** is useful for documentation instead of a mysterious 4 or 16 sitting
** around in the code.
*/
#define __GL_CI_ANTI_ALIAS_BITS		4
#define __GL_CI_ANTI_ALIAS_DIVISOR	(1 << __GL_CI_ANTI_ALIAS_BITS)

/************************************************************************/

/*
** These macros are used to convert incoming color values into the
** abstract color range from 0.0 to 1.0
*/
#define __GL_B_TO_FLOAT(b)	((((b)<<1) + 1) * gc->constants.oneOver255)
#define __GL_UB_TO_FLOAT(ub)	(gc->constants.uByteToFloat[ub])
#define __GL_S_TO_FLOAT(s)	((((s)<<1) + 1) * gc->constants.oneOver65535)
#define __GL_US_TO_FLOAT(us)	((us) * gc->constants.oneOver65535)

/*
** Not quite 2^31-1 because of possible floating point errors.  4294965000
** is a much safer number to use.
*/
#define __GL_I_TO_FLOAT(i) \
	((((__GLfloat)(i) * (__GLfloat) 2.0) + 1) * \
	    gc->constants.oneOver4294965000)
#define __GL_UI_TO_FLOAT(ui) \
	((__GLfloat)(ui) * gc->constants.oneOver4294965000)

/*
** Bloody "round towards 0" convention.  We could avoid these floor() calls
** were it not for that!
*/
#define __GL_FLOAT_TO_B(f) \
	((GLbyte) __GL_FLOORF(((f) * gc->constants.val255) * __glHalf))
#define __GL_FLOAT_TO_UB(f) \
	((GLubyte) ((f) * gc->constants.val255 + __glHalf))
#define __GL_FLOAT_TO_S(f) \
	((GLshort) __GL_FLOORF(((f) * gc->constants.val65535) * __glHalf))
#define __GL_FLOAT_TO_US(f) \
	((GLushort) ((f) * gc->constants.val65535 + __glHalf))

/*
** Not quite 2^31-1 because of possible floating point errors.  4294965000
** is a much safer number to use.
*/
#define __GL_FLOAT_TO_I(f) \
    ((GLint) __GL_FLOORF(((f) * gc->constants.val4294965000) * __glHalf))
#define __GL_FLOAT_TO_UI(f) \
    ((GLuint) ((f) * gc->constants.val4294965000 + __glHalf))

/*
** Mask the incoming color index (in floating point) against the
** maximum color index value for the color buffers.  Keep 4 bits
** of fractional precision.
*/
#define __GL_MASK_INDEXF(gc, val)			       \
    (((__GLfloat) (((GLint) ((val) * 16))		       \
		   & (((gc)->frontBuffer.redMax << 4) | 0xf))) \
     / (__GLfloat)16.0)

#define __GL_MASK_INDEXI(gc, val)			       \
    ((val) & (gc)->frontBuffer.redMax)

/************************************************************************/

/* 
** These two must be the same size, because they cache their tables in the
** same arena.
*/
#define __GL_SPEC_LOOKUP_TABLE_SIZE	256
#define __GL_SPOT_LOOKUP_TABLE_SIZE	__GL_SPEC_LOOKUP_TABLE_SIZE

typedef struct {
    GLint refcount;
    __GLfloat threshold, scale, exp;
    __GLfloat table[__GL_SPEC_LOOKUP_TABLE_SIZE];
} __GLspecLUTEntry;

__GLspecLUTEntry *__glCreateSpecLUT(__GLcontext *gc, __GLfloat exp);
void __glFreeSpecLUT(__GLcontext *gc, __GLspecLUTEntry *lut);
void __glInitLUTCache(__GLcontext *gc);
void __glFreeLUTCache(__GLcontext *gc);

#define __GL_LIGHT_UPDATE_FRONT_MATERIAL_AMBIENT

/*
** Per light source per material computed state.
*/
typedef struct __GLlightSourcePerMaterialMachineRec {
    __GLcolor ambient;		/* light ambient times material ambient */
    __GLcolor diffuse;		/* light diffuse times material diffuse */
    __GLcolor specular;		/* light specular times material specular */
} __GLlightSourcePerMaterialMachine;

/*
** Per light source computed state.
*/
struct __GLlightSourceMachineRec {
    /*
    ** ambient, diffuse and specular are each pre-multiplied by the
    ** material ambient, material diffuse and material specular.
    ** We use the face being lit to pick between the two sets.
    */
    __GLlightSourcePerMaterialMachine front, back;

    __GLlightSourceState *state;

    __GLfloat constantAttenuation;
    __GLfloat linearAttenuation;
    __GLfloat quadraticAttenuation;
    __GLfloat spotLightExponent;

    /* Position of the light source in eye coordinates */
    __GLcoord position;

    /* Direction of the light source in eye coordinates, normalize */
    __GLcoord direction;

    /* Cosine of the spot light cutoff angle */
    __GLfloat cosCutOffAngle;

    /* Precomputed attenuation, only when k1 and k2 are zero */
    __GLfloat attenuation;

    /* This will be set when the cut off angle != 180 */
    GLboolean isSpot;

    /* When possible, the normalized "h" value from the spec is pre-computed */
    __GLcoord hHat;

    /* Unit vector VPpli pre-computed (only when light is at infinity) */
    __GLcoord unitVPpli;

    /* As above, but in model coordinates */
    __GLcoord hHatModel;
    __GLcoord unitVPpliModel;

    /* sli and dli values pre-computed (color index mode only) */
    __GLfloat sli, dli;

    /* Link to next active light */
    __GLlightSourceMachine *next;

    /* Spot light exponent lookup table */
    __GLfloat *spotTable;

    /* Values used to avoid pow function during spot computations */
    __GLfloat threshold, scale;

    /* cache entry where this data came from */
    __GLspecLUTEntry *cache;

    /* Set to GL_TRUE if slow processing path is needed */
    GLboolean slowPath;
};

/*
** Per material computed state.
*/
struct __GLmaterialMachineRec {
    /*
    ** Sum of:
    **	material emissive color
    **  material ambient color * scene ambient color
    **
    ** This sum is carefully kept scaled.
    */
    __GLcolor sceneColor;

    /*
    ** Combination of the current material and first light source
    ** One light being optimized. one for specular/diffuse & ambient
    */
    __GLcolor firstLightMatS;
    __GLcolor firstLightMatD;
    __GLcolor firstLightMatA;

    /* Specular exponent */
    __GLfloat specularExponent;

    /* Specular exponent lookup table */
    __GLfloat *specTable;

    /* Values used to avoid pow function during specular computations */
    __GLfloat threshold, scale;

    /* cache entry where this data came from */
    __GLspecLUTEntry *cache;

    /* Scaled and clamped form of material diffuse alpha */
    __GLfloat alpha;

    /* For color index mode lighting */
    __GLfloat cmapd_minus_cmapa;	/* = cmapd - cmapa */
    __GLfloat cmaps_minus_cmapa;	/* = cmaps - cmapa */
};

typedef struct {
    __GLlightSourceMachine *source;
    __GLmaterialMachine front, back;

    /* List of enabled light sources */
    __GLlightSourceMachine *sources;

    GLint numEnabledLights;
    GLint fastLightMode;

    /* Current material color material (iff one material is being updated) */
    __GLmaterialState *cm;
    __GLmaterialMachine *cmm;

    /* Cache of lookup tables for spot lights and specular highlights */
    struct __GLspecLUTCache_Rec *lutCache;

    /* Computed starting color for vertex lighting */
    __GLcolor baseColor;
} __GLlightMachine;

/* Values for cmParam */
#define __GL_EMISSION			0
#define __GL_AMBIENT			1
#define __GL_SPECULAR			2
#define __GL_AMBIENT_AND_DIFFUSE	3
#define __GL_DIFFUSE			4

extern void __glCopyCIColor(__GLcontext *gc, GLuint faceBit, __GLvertex *v);
extern void __glCopyRGBColor(__GLcontext *gc, GLuint faceBit, __GLvertex *v);

extern void __glClampRGBColor(__GLcontext *gc, __GLcolor *dst,
			      const __GLcolor *src);
extern void __glClampAndScaleColor(__GLcontext *gc);


/* Stuff for converting float colors */
extern void __glClampAndScaleColorf(__GLcontext *gc, __GLcolor *dst,
				    const GLfloat src[4]);
extern void __glClampColorf(__GLcontext *gc, __GLcolor *dst,
			    const GLfloat src[4]);
extern void __glScaleColorf(__GLcontext *gc, __GLcolor *dst,
			    const GLfloat src[4]);
extern void __glUnScaleColorf(__GLcontext *gc, GLfloat dst[4],
			      const __GLcolor *src);

/* Stuff for converting integer colors */
extern void __glClampAndScaleColori(__GLcontext *gc, __GLcolor *dst,
				    const GLint src[4]);
extern void __glClampColori(__GLcontext *gc, __GLcolor *dst,
			    const GLint src[4]);
extern void __glScaleColori(__GLcontext *gc, __GLcolor *dst,
			    const GLint src[4]);
extern void __glUnScaleColori(__GLcontext *gc, GLint dst[4],
			      const __GLcolor *src);

extern void __glTransformLightDirection(__GLcontext *gc,
					__GLlightSourceState *ls);

extern void __glValidateLighting(__GLcontext *gc);
extern void __glValidateMaterial(__GLcontext *gc, GLint front, GLint back);
extern void __glValidateModelViewForLighting(__GLcontext *gc);

/* Procs for handling color material changes */
extern void __glChangeOneMaterialColor(__GLcontext *gc);
extern void __glChangeBothMaterialColors(__GLcontext *gc);
extern void __glChangeVertexMaterialColor(__GLcontext *gc, __GLcolor *col);

/* Lighting procs */
extern void __glCalcRGBColor(__GLcontext *gc, GLint face, __GLvertex *vx);
extern void __glFastCalcRGBColor(__GLcontext *gc, GLint face, __GLvertex *vx);
extern void __glCalcCIColor(__GLcontext *gc, GLint face, __GLvertex *vx);
extern void __glFastCalcCIColor(__GLcontext *gc, GLint face, __GLvertex *vx);

extern void __GLvvec(__GLcoord *r, const __GLcoord *p1, const __GLcoord *p2);
extern void __glNormalizeTwo(const __GLfloat v[3], __GLfloat voa[3], __GLfloat vob[3]);

#endif /* __gl_lighting_h_ */
