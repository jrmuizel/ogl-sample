#ifndef __gl_attrib_h_
#define __gl_attrib_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/attrib.h#14 $
*/

#include "lighting.h"
#include "texture.h"
#include "pixel.h"
#include "eval.h"
#include "vertex.h"
#include "ctable.h"
#include "varray.h"

/*
** Current state.
*/
typedef struct __GLcurrentStateRec {
    __GLvertex rasterPos;

    /*
    ** Raster pos valid bit.
    */
    GLboolean validRasterPos;

    /*
    ** Edge tag state.
    */
    GLuint edgeTag;

    /*
    ** Current color and colorIndex.  These variables are also used for
    ** the current rasterPos color and colorIndex as set by the user.
    */
    __GLcolor userColor;
    __GLfloat userColorIndex;

    /*
    ** The current operating color.  The r component is the color
    ** index in color index mode.  This is a clamped copy of the
    ** users RGB color when in RGB mode.  This is computed during
    ** the applyColor step of color loading.
    */
    __GLcolor color;

    __GLcoord normal;
    __GLcoord texture[__GL_NUM_TEXTURE_UNITS];
} __GLcurrentState;

/************************************************************************/

/*
** Point state.
*/
typedef struct __GLpointStateRec {
    __GLfloat requestedSize;
    __GLfloat smoothSize;
    GLint aliasedSize;
} __GLpointState;

/************************************************************************/

/*
** Line state.  Contains all the client controllable line state.
*/
typedef struct __GLlineStateRec {
    __GLfloat requestedWidth;
    __GLfloat smoothWidth;
    GLint aliasedWidth;
    GLushort stipple;
    GLshort stippleRepeat;
} __GLlineState;

/************************************************************************/

/*
** Polygon state.  Contains all the user controllable polygon state
** except for the stipple state.
*/
typedef struct __GLpolygonStateRec {
    GLenum frontMode;
    GLenum backMode;

    /*
    ** Culling state.  Culling can be enabled/disabled and set to cull
    ** front or back faces.  The FrontFace call determines whether clockwise
    ** or counter-clockwise oriented vertices are front facing.
    */
    GLenum cull;
    GLenum frontFaceDirection;

    /*
    ** Polygon offset state. 
    */
    __GLfloat factor;
    __GLfloat units;

} __GLpolygonState;

/************************************************************************/

/*
** Polygon stipple state.
*/
typedef struct __GLpolygonStippleStateRec {
    GLubyte stipple[4*32];
} __GLpolygonStippleState;

/************************************************************************/

/*
** Fog state.
*/
typedef struct __GLfogStateRec {
    GLenum mode;
    __GLcolor color;
    __GLfloat density, start, end;
    __GLfloat oneOverEMinusS;
    __GLfloat index;
    __GLcolor bufColor;
    GLubyte r, g, b, i;	/* scaled ubyte copies of color, index */
} __GLfogState;

/************************************************************************/

/*
** Depth state.  Contains all the user settable depth state.
*/
typedef struct __GLdepthStateRec {
    /*
    ** Depth buffer test function.  The z value is compared using zFunction
    ** against the current value in the zbuffer.  If the comparison
    ** succeeds the new z value is written into the z buffer masked
    ** by the z write mask.
    */
    GLenum testFunc;

    /*
    ** Writemask enable.  When GL_TRUE writing to the depth buffer is
    ** allowed.
    */
    GLboolean writeEnable;

    /*
    ** Value used to clear the z buffer when glClear is called.
    */
    GLdouble clear;
} __GLdepthState;

/************************************************************************/

/*
** Accum state.
*/
typedef struct __GLaccumStateRec {
    __GLcolor clear;
} __GLaccumState;

/************************************************************************/

/*
** Stencil state.  Contains all the user settable stencil state.
*/
typedef struct __GLstencilStateRec {
    /*
    ** Stencil test function.  When the stencil is enabled this
    ** function is applied to the reference value and the stored stencil
    ** value as follows:
    **		result = ref comparision (mask & stencilBuffer[x][y])
    ** If the test fails then the fail op is applied and rendering of
    ** the pixel stops.
    */
    GLenum testFunc;

    /*
    ** Stencil clear value.  Used by glClear.
    */
    GLshort clear;

    /*
    ** Reference stencil value.
    */
    GLshort reference;

    /*
    ** Stencil mask.  This is anded against the contents of the stencil
    ** buffer during comparisons.
    */
    GLshort mask;

    /*
    ** Stencil write mask
    */
    GLshort writeMask;

    /*
    ** When the stencil comparison fails this operation is applied to
    ** the stencil buffer.
    */
    GLenum fail;

    /*
    ** When the stencil comparison passes and the depth test
    ** fails this operation is applied to the stencil buffer.
    */
    GLenum depthFail;

    /*
    ** When both the stencil comparison passes and the depth test
    ** passes this operation is applied to the stencil buffer.
    */
    GLenum depthPass;
} __GLstencilState;

/************************************************************************/

/*
** Viewport state.
*/
typedef struct __GLviewportRec {
    /*
    ** Viewport parameters from user, as integers.
    */
    GLint x, y;
    GLsizei width, height;

    /*
    ** Depthrange parameters from user.
    */
    GLdouble zNear, zFar;

/*XXX move me */
    /*
    ** Internal form of viewport and depth range used to compute
    ** window coordinates from clip coordinates.
    */
    __GLfloat xScale, xCenter;
    __GLfloat yScale, yCenter;
    __GLfloat zScale, zCenter;
} __GLviewport;

/************************************************************************/

/*
** Transform state.
*/
typedef struct __GLtransformStateRec {
    /*
    ** Current mode of the matrix stack.  This determines what effect
    ** the various matrix operations (load, mult, scale) apply to.
    */
    GLenum matrixMode;

    /*
    ** User clipping planes in eye space.  These are the user clip planes
    ** projected into eye space.  
    */
/* XXX BUG! stacking of eyeClipPlanes is busted! */
    __GLcoord *eyeClipPlanes;

    /*
    ** EXT_cull_vertex attributes.
    */
    __GLcoord eyePos;		/* eye space eye location */
    __GLcoord eyePosObj;	/* object space eye location */
    GLboolean objEyeSpecified;	/* true if user specified obj space */
} __GLtransformState;

/************************************************************************/

/*
** Enable structures.  Anything that can be glEnable'd or glDisable'd is
** contained in this structure.  The enables are kept as single bits
** in a couple of bitfields.
*/

/* Bits in "general" enable word */
#define __GL_ALPHA_TEST_ENABLE			(1 <<  0)
#define __GL_BLEND_ENABLE			(1 <<  1)
#define __GL_INDEX_LOGIC_OP_ENABLE		(1 <<  2)
#define __GL_DITHER_ENABLE			(1 <<  3)
#define __GL_DEPTH_TEST_ENABLE			(1 <<  4)
#define __GL_FOG_ENABLE				(1 <<  5)
#define __GL_LIGHTING_ENABLE			(1 <<  6)
#define __GL_COLOR_MATERIAL_ENABLE		(1 <<  7)
#define __GL_LINE_STIPPLE_ENABLE		(1 <<  8)
#define __GL_LINE_SMOOTH_ENABLE			(1 <<  9)
#define __GL_POINT_SMOOTH_ENABLE		(1 << 10)
#define __GL_POLYGON_SMOOTH_ENABLE		(1 << 11)
#define __GL_CULL_FACE_ENABLE			(1 << 12)
#define __GL_POLYGON_STIPPLE_ENABLE		(1 << 13)
#define __GL_SCISSOR_TEST_ENABLE		(1 << 14)
#define __GL_STENCIL_TEST_ENABLE		(1 << 15)
#define __GL_NORMALIZE_ENABLE			(1 << 16)
#define __GL_AUTO_NORMAL_ENABLE			(1 << 17)
#define __GL_POLYGON_OFFSET_POINT_ENABLE	(1 << 18)
#define __GL_POLYGON_OFFSET_LINE_ENABLE		(1 << 19)
#define __GL_POLYGON_OFFSET_FILL_ENABLE		(1 << 20)
#define __GL_COLOR_LOGIC_OP_ENABLE		(1 << 21)
#define __GL_CULL_VERTEX_ENABLE			(1 << 22)
#define __GL_RESCALE_NORMAL_ENABLE		(1 << 23)

#define __GL_POLYGON_OFFSET_ENABLES \
    (__GL_POLYGON_OFFSET_POINT_ENABLE | \
     __GL_POLYGON_OFFSET_LINE_ENABLE | \
     __GL_POLYGON_OFFSET_FILL_ENABLE)

/* Bits in "texture" enable word */
#define __GL_TEXTURE_1D_ENABLE			(1 << 0)
#define __GL_TEXTURE_2D_ENABLE			(1 << 1)
#define __GL_TEXTURE_3D_ENABLE			(1 << 2)
#define __GL_TEXTURE_GEN_S_ENABLE		(1 << 3)
#define __GL_TEXTURE_GEN_T_ENABLE		(1 << 4)
#define __GL_TEXTURE_GEN_R_ENABLE		(1 << 5)
#define __GL_TEXTURE_GEN_Q_ENABLE		(1 << 6)

/*
** Composities of the above bits for each glPushAttrib group that has
** multiple enables, except for those defined below
*/
#define __GL_COLOR_BUFFER_ENABLES				\
    (__GL_ALPHA_TEST_ENABLE | __GL_BLEND_ENABLE			\
     | __GL_INDEX_LOGIC_OP_ENABLE | __GL_COLOR_LOGIC_OP_ENABLE  \
     | __GL_DITHER_ENABLE)

#define __GL_LIGHTING_ENABLES \
    (__GL_LIGHTING_ENABLE | __GL_COLOR_MATERIAL_ENABLE)

#define __GL_LINE_ENABLES \
    (__GL_LINE_STIPPLE_ENABLE | __GL_LINE_SMOOTH_ENABLE)

#define __GL_POLYGON_ENABLES				\
    (__GL_POLYGON_SMOOTH_ENABLE | __GL_CULL_FACE_ENABLE	\
     | __GL_POLYGON_STIPPLE_ENABLE | __GL_POLYGON_OFFSET_ENABLES)

/* Bits in "eval1" enable word */
#define __GL_MAP1_VERTEX_3_ENABLE		(1 << __GL_V3)
#define __GL_MAP1_VERTEX_4_ENABLE		(1 << __GL_V4)
#define __GL_MAP1_COLOR_4_ENABLE		(1 << __GL_C4)
#define __GL_MAP1_INDEX_ENABLE			(1 << __GL_I)
#define __GL_MAP1_NORMAL_ENABLE			(1 << __GL_N3)
#define __GL_MAP1_TEXTURE_COORD_1_ENABLE	(1 << __GL_T1)
#define __GL_MAP1_TEXTURE_COORD_2_ENABLE	(1 << __GL_T2)
#define __GL_MAP1_TEXTURE_COORD_3_ENABLE	(1 << __GL_T3)
#define __GL_MAP1_TEXTURE_COORD_4_ENABLE	(1 << __GL_T4)

/* Bits in "eval2" enable word */
#define __GL_MAP2_VERTEX_3_ENABLE		(1 << __GL_V3)
#define __GL_MAP2_VERTEX_4_ENABLE		(1 << __GL_V4)
#define __GL_MAP2_COLOR_4_ENABLE		(1 << __GL_C4)
#define __GL_MAP2_INDEX_ENABLE			(1 << __GL_I)
#define __GL_MAP2_NORMAL_ENABLE			(1 << __GL_N3)
#define __GL_MAP2_TEXTURE_COORD_1_ENABLE	(1 << __GL_T1)
#define __GL_MAP2_TEXTURE_COORD_2_ENABLE	(1 << __GL_T2)
#define __GL_MAP2_TEXTURE_COORD_3_ENABLE	(1 << __GL_T3)
#define __GL_MAP2_TEXTURE_COORD_4_ENABLE	(1 << __GL_T4)

/* Bits in "clipPlanes" enable word */
#define __GL_CLIP_PLANE0_ENABLE			(1 << 0)
#define __GL_CLIP_PLANE1_ENABLE			(1 << 1)
#define __GL_CLIP_PLANE2_ENABLE			(1 << 2)
#define __GL_CLIP_PLANE3_ENABLE			(1 << 3)
#define __GL_CLIP_PLANE4_ENABLE			(1 << 4)
#define __GL_CLIP_PLANE5_ENABLE			(1 << 5)

/* Bits in "lights" enable word */
#define __GL_LIGHT0_ENABLE			(1 << 0)
#define __GL_LIGHT1_ENABLE			(1 << 1)
#define __GL_LIGHT2_ENABLE			(1 << 2)
#define __GL_LIGHT3_ENABLE			(1 << 3)
#define __GL_LIGHT4_ENABLE			(1 << 4)
#define __GL_LIGHT5_ENABLE			(1 << 5)
#define __GL_LIGHT6_ENABLE			(1 << 6)
#define __GL_LIGHT7_ENABLE			(1 << 7)

/* Bits in the "pixelPath" enable word */
#define __GL_COLOR_TABLE_ENABLE				(1 << 0)
#define __GL_POST_CONVOLUTION_COLOR_TABLE_ENABLE	(1 << 1)
#define __GL_POST_COLOR_MATRIX_COLOR_TABLE_ENABLE	(1 << 2)
#define __GL_CONVOLUTION_1D_ENABLE			(1 << 3)
#define __GL_CONVOLUTION_2D_ENABLE			(1 << 4)
#define __GL_SEPARABLE_2D_ENABLE			(1 << 5)
#define __GL_HISTOGRAM_ENABLE				(1 << 6)
#define __GL_MINMAX_ENABLE				(1 << 7)

typedef struct __GLenableStateRec {
    GLuint general;
    GLuint texture[__GL_NUM_TEXTURE_UNITS];
    GLuint lights;
    GLuint clipPlanes;
    GLuint pixelPath;
    GLushort eval1, eval2;
} __GLenableState;

/************************************************************************/

/*
** Raster state.
*/
typedef struct __GLrasterStateRec {
    /*
    ** Alpha function.  The alpha function is applied to the alpha color
    ** value and the reference value.  If it fails then the pixel is
    ** not rendered.
    */
    GLenum alphaFunction;
    __GLfloat alphaReference;

    /*
    ** Alpha blending source and destination factors, blend op,
    ** and blend color.
    */
    GLenum blendSrc;
    GLenum blendDst;

    /*
    ** Constant blending color.
    */
    __GLcolor blendColor; 

    /*
    ** Blending equation.
    */
    GLenum blendMode;

    /*
    ** Logic op.
    */
    GLenum logicOp;

    /*
    ** Color to fill the color portion of the framebuffer when clear
    ** is called.
    */
    __GLcolor clear;
    __GLfloat clearIndex;

    /*
    ** Color index write mask.  The color values are masked with this
    ** value when writing to the frame buffer so that only the bits set
    ** in the mask are changed in the frame buffer.
    */
    GLint writeMask;

    /*
    ** RGB write masks.  These booleans enable or disable writing of
    ** the r, g, b, and a components.
    */
    GLboolean rMask, gMask, bMask, aMask;

    /*
    ** This state variable tracks which buffer(s) is being drawn into.
    */
    GLenum drawBuffer;

    /*
    ** Draw buffer specified by user.  May be different from drawBuffer
    ** above.  If the user specifies GL_FRONT_LEFT, for example, then 
    ** drawBuffer is set to GL_FRONT, and drawBufferReturn to 
    ** GL_FRONT_LEFT.
    */
    GLenum drawBufferReturn;
} __GLrasterState;

/************************************************************************/

/*
** Hint state.  Contains all the user controllable hint state.
*/
typedef struct {
    GLenum perspectiveCorrection;
    GLenum pointSmooth;
    GLenum lineSmooth;
    GLenum polygonSmooth;
    GLenum fog;
    GLenum textureBuffer;
} __GLhintState;

/************************************************************************/

/*
** All stackable list state.
*/
typedef struct __GLdlistStateRec {
    GLuint listBase;
} __GLdlistState;

/************************************************************************/

/*
** Scissor state from user.
*/
typedef struct __GLscissorRec {
    GLint scissorX, scissorY;
    GLuint scissorWidth, scissorHeight;
} __GLscissor;

/************************************************************************/

struct __GLattributeRec {
    /*
    ** Mask of which fields in this structure are valid.
    */
    GLuint mask;

    __GLcurrentState current;
    __GLpointState point;
    __GLlineState line;
    __GLpolygonState polygon;
    __GLpolygonStippleState polygonStipple;
    __GLpixelState pixel;
    __GLlightState light;
    __GLfogState fog;
    __GLdepthState depth;
    __GLaccumState accum;
    __GLstencilState stencil;
    __GLviewport viewport;
    __GLtransformState transform;
    __GLenableState enables;
    __GLrasterState raster;
    __GLhintState hints;
    __GLevaluatorState evaluator;
    __GLdlistState list;
    __GLtextureState texture;
    __GLscissor scissor;
    __GLcolorTableState colorTable;
};

struct __GLclientAttributeRec {
    GLuint mask;

    __GLclientPixelState clientPixel;
    __GLclientTextureState clientTexture;
    __GLclientVertArrayState vertexArray;
};

/************************************************************************/

/*
** Attribution machine state.  This manages the stack of attributes.
*/
typedef struct {
    /*
    ** Attribute stack.  The attribute stack keeps track of the
    ** attributes that have been pushed.
    */
    __GLattribute **stack;
    __GLclientAttribute **clientStack;

    /*
    ** Attribute stack pointer.
    */
    __GLattribute **stackPointer;
    __GLclientAttribute **clientStackPointer;
} __GLattributeMachine;

extern void __glFreeAttributeState(__GLcontext *gc);
extern GLboolean __glCopyContext(__GLcontext *dst, const __GLcontext *src,
				 GLuint mask);
extern GLenum __glErrorCheckMaterial(GLenum face, GLenum p, GLfloat pv0);

#endif /* __gl_attrib_h_ */
