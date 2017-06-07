#ifndef __gl_render_h_
#define __gl_render_h_

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
#include "types.h"
#include "consts.h"
#include "texture.h"
#include "cpu.h"

/* 
** used to approximate zero (and avoid divide by zero errors)
** when doing polygon offset with dzdx = 0.
*/
#define __GL_PGON_OFFSET_NEAR_ZERO .00001

#if defined(DEBUG)
#define POLYGON_OFFSET_DEBUG
/* for the time being, don't debug polygon offset */
#undef POLYGON_OFFSET_DEBUG
#endif /* DEBUG */

typedef GLuint __GLstippleWord;

/*
** Constants for access shader and fragment colors / interpolators.
*/

enum {
    __GL_PRIMARY_COLOR = 0,
    __GL_SECONDARY_COLOR,
    __GL_NUM_FRAGMENT_COLORS
};

	

typedef struct __GLfragmentTextureRec {
    __GLfloat s, t, r, qw, rhow;
} __GLfragmentTexture;

/*
** A fragment is a collection of all the data needed after rasterization
** of a primitive has occured, but before the data is entered into various
** framebuffers.  The data contained in the fragment has been normalized
** into a form for immediate storage into the framebuffer.
*/
struct __GLfragmentRec {
    /* Screen x, y */
    GLint x, y;

    /* Z coordinate in form used by depth buffer */
    __GLzValue z;

    /*
    ** Colors of the fragment.  When in colorIndexMode only the r component
    ** is valid.
    */
    __GLcolor color[__GL_NUM_FRAGMENT_COLORS];

    /* Texture information for the fragment */
    __GLfragmentTexture texture[__GL_NUM_TEXTURE_UNITS];

    /* Fog information for the fragment */
    __GLfloat f;
};

/************************************************************************/

/*
** Interpolator record for color interpolators.  Used for primary and
** secondary colors;
*/

struct __GLcolorIteratorRec {
    __GLfloat rLittle, gLittle, bLittle, aLittle;
    __GLfloat rBig, gBig, bBig, aBig;
    __GLfloat drdx, dgdx, dbdx, dadx;
    __GLfloat drdy, dgdy, dbdy, dady;
};

struct __GLtextureIteratorRec {
    __GLfloat sLittle, tLittle, rLittle, qwLittle, rhowLittle;
    __GLfloat sBig, tBig, rBig, qwBig, rhowBig;
    __GLfloat dsdx, dtdx, drdx, dqwdx, drhowdx;
    __GLfloat dsdy, dtdy, drdy, dqwdy, drhowdy;
};

/*
** Shader record for iterated objects (lines/triangles).  This keeps
** track of all the various deltas needed to rasterize a triangle.
*/
struct __GLshadeRec {
    GLint dxLeftLittle, dxLeftBig;
    GLint dxLeftFrac;
    GLint ixLeft, ixLeftFrac;

    GLint dxRightLittle, dxRightBig;
    GLint dxRightFrac;
    GLint ixRight, ixRightFrac;

    __GLfloat area;
    __GLfloat dxAC, dxBC, dyAC, dyBC;

    GLboolean ccw;

    __GLfragment frag;
    GLint length;

    /*
    **  Color iterators for primary and secondary colors
    */
    __GLcolorIterator  colorIter[__GL_NUM_FRAGMENT_COLORS];
    
    GLint zLittle, zBig;
    GLint dzdx, dzdxBig;
    __GLfloat dzdyf, dzdxf;

    __GLtextureIterator texture[__GL_NUM_TEXTURE_UNITS];

    __GLfloat fLittle, fBig;
    __GLfloat dfdy, dfdx;

    GLuint modeFlags;

    __GLzValue *zbuf;
    GLint zbufBig, zbufLittle;

    __GLstencilCell *sbuf;
    GLint sbufBig, sbufLittle;

    __GLcolor *colors;
    __GLcolor *fbcolors;
    __GLstippleWord *stipplePat;
    GLboolean done;

    __GLcolorBuffer *cfb;

    /*
    ** For polygon offset
    */

    __GLfloat offsetZ;
    __GLzValue startZLimit;
    __GLzValue endZLimit;

    
    __GLfloat outBoundCount; /* steps from start of span to exit z range */
    __GLfloat rangeCount; /* steps needed to traverse entire z range */

    __GLfloat outCountBig; /* update outBoundCount per span */
    __GLfloat outCountLittle;
};

/*
** The distinction between __GL_SHADE_SMOOTH and __GL_SHADE_SMOOTH_LIGHT is
** simple.  __GL_SHADE_SMOOTH indicates if the polygon will be smoothly 
** shaded, and __GL_SHADE_SMOOTH_LIGHT indicates if the polygon will be 
** lit at each vertex.  Note that __GL_SHADE_SMOOTH might be set while
** __GL_SHADE_SMOOTH_LIGHT is not set if the lighting model is GL_FLAT, but
** the polygons are fogged.
*/
#define __GL_SHADE_RGB		        0x0001
#define __GL_SHADE_SMOOTH	        0x0002 /* smooth shaded polygons */
#define __GL_SHADE_DEPTH_TEST	        0x0004
#define __GL_SHADE_TEXTURE	        0x0008
#define __GL_SHADE_STIPPLE	        0x0010 /* polygon stipple */
#define __GL_SHADE_STENCIL_TEST	        0x0020
#define __GL_SHADE_DITHER	        0x0040
#define __GL_SHADE_LOGICOP	        0x0080
#define __GL_SHADE_BLEND	        0x0100
#define __GL_SHADE_ALPHA_TEST	        0x0200
#define __GL_SHADE_TWOSIDED	        0x0400
#define __GL_SHADE_MASK		        0x0800

/* Two kinds of fog... */
#define __GL_SHADE_SLOW_FOG	0x1000
#define __GL_SHADE_CHEAP_FOG	0x2000

/* do we iterate depth values in software */
#define __GL_SHADE_DEPTH_ITER	0x4000

#define __GL_SHADE_LINE_STIPPLE	0x8000

#define __GL_SHADE_CULL_FACE	0x00010000
#define __GL_SHADE_SMOOTH_LIGHT	0x00020000 /* smoothly lit polygons */

#define __GL_SHADE_POLYGON_OFFSET_POINT	0x00040000 /* point offset */
#define __GL_SHADE_POLYGON_OFFSET_LINE	0x00080000 /* line offset */
#define __GL_SHADE_POLYGON_OFFSET_FILL	0x00100000 /* polygon offset */

#define __GL_SHADE_OWNERSHIP_TEST	0x00200000

/* are texture coordinates projected before rasterization */
#define __GL_SHADE_PROJSCALED_TEXTURE	0x00400000

/* do we iterate scaled texture coordinates */
#define __GL_SHADE_UVSCALED_TEXTURE	0x00800000

/* do we iterate perspective correct texture coordinates */
#define __GL_SHADE_PERSP_TEXTURE	0x01000000

/* are we mipmapping */
#define __GL_SHADE_MIPMAP_TEXTURE	0x02000000

/* do we need source alpha (for blending, alpha testing, dest alpha) */
#define __GL_SHADE_SRC_ALPHA		0x04000000

#define __GL_SHADE_SECONDARY_COLOR      0x08000000

/************************************************************************/

/*
** __GL_STIPPLE_COUNT_BITS is the number of bits needed to represent a 
** stipple count (5 bits).
**
** __GL_STIPPLE_BITS is the number of bits in a stipple word (32 bits).
*/
#define __GL_STIPPLE_COUNT_BITS 5
#define __GL_STIPPLE_BITS (1 << __GL_STIPPLE_COUNT_BITS)

#ifdef __GL_STIPPLE_MSB
#define __GL_STIPPLE_SHIFT(i) (1 << (__GL_STIPPLE_BITS - 1 - (i)))
#else
#define __GL_STIPPLE_SHIFT(i) (1 << (i))
#endif

#define __GL_MAX_STIPPLE_WORDS \
    ((__GL_MAX_MAX_VIEWPORT + __GL_STIPPLE_BITS - 1) / __GL_STIPPLE_BITS)

/************************************************************************/

/*
** Accumulation buffer cells for each color component.  Note that these
** items needs to be at least 2 bits bigger than the color components
** that drive them, with 2 times being ideal.  This declaration assumes
** that the underlying color components are no more than 14 bits and
** hopefully 8.
*/
typedef struct __GLaccumCellRec {
    __GLaccumCellElement r, g, b, a;
} __GLaccumCell;

/************************************************************************/

struct __GLbitmapRec {
        GLsizei width;
        GLsizei height;
        GLfloat xorig;
        GLfloat yorig;
        GLfloat xmove;
        GLfloat ymove;
	GLint imageSize;		/* An optimization */
	GLint pad1;                     /* XXX -- dbg, make sure we're a multiple of 8 in size */
        /*      bitmap  */
};

extern void __glDrawBitmap(__GLcontext *gc, GLsizei width, GLsizei height,
			   GLfloat xOrig, GLfloat yOrig,
			   GLfloat xMove, GLfloat yMove,
			   const GLubyte bits[]);

extern __GLbitmap *__glAllocBitmap(__GLcontext *gc,
				   GLsizei width, GLsizei height,
				   GLfloat xOrig, GLfloat yOrig,
				   GLfloat xMove, GLfloat yMove);
				
extern void __glRenderBitmap(__GLcontext *gc, const __GLbitmap *bitmap, 
			     const GLubyte *bits);

/************************************************************************/

/* New AA line algorithm supports widths one or more.  Until that changes,
** don't change this minimum!
*/
#define __GL_POINT_SIZE_MINIMUM		 1.0
#define __GL_POINT_SIZE_MAXIMUM		10.0
#define __GL_POINT_SIZE_GRANULARITY	 0.125
#define __GL_ALIASED_POINT_SIZE_MINIMUM	 1
#define __GL_ALIASED_POINT_SIZE_MAXIMUM	64

extern void __glBeginPoints(__GLcontext *gc);
extern void __glEndPoints(__GLcontext *gc);

extern void __glPoint(__GLcontext *gc, __GLvertex *vx);
extern void __glPointLock(__GLcontext *gc, __GLvertex *vx);
extern void __glPointFast(__GLcontext *gc, __GLvertex *vx);
extern void __glPointFastLock(__GLcontext *gc, __GLvertex *vx);

/* Various point rendering implementations */
void __glShadeTextureFogPoint(__GLcontext *gc, __GLvertex *v);
void __glShadeTexturePoint(__GLcontext *gc, __GLvertex *v);
void __glShadeFogPoint(__GLcontext *gc, __GLvertex *v);
void __glShadePoint(__GLcontext *gc, __GLvertex *v);
void __glRenderAliasedPointN(__GLcontext *gc, __GLvertex *v, __GLfragment *f);
void __glRenderAliasedPoint1(__GLcontext *gc, __GLvertex *v, __GLfragment *f);
void __glRenderAntiAliasedRGBPoint(__GLcontext *gc, __GLvertex *v);
void __glRenderAntiAliasedCIPoint(__GLcontext *gc, __GLvertex *v);

/************************************************************************/

#define __GL_LINE_WIDTH_MINIMUM		 0.5 
#define __GL_LINE_WIDTH_MAXIMUM		10.0
#define __GL_LINE_WIDTH_GRANULARITY	 0.125
#define __GL_ALIASED_LINE_WIDTH_MINIMUM	 1
#define __GL_ALIASED_LINE_WIDTH_MAXIMUM	64

/*
** Don't change these constants without fixing LIGHT/rex_linespan.ma which
** currently assumes that __GL_X_MAJOR is 0.
*/
#define __GL_X_MAJOR    0
#define __GL_Y_MAJOR    1

/*
** Use a fixed point notation of 15.17
**
** This should support screen sizes up to 4K x 4K, with 5 subpixel bits
** for 4K x 4K screens.
*/
#define __GL_LINE_FRACBITS              17
#define __GL_LINE_INT_TO_FIXED(x)       ((x) << __GL_LINE_FRACBITS)
#define __GL_LINE_FLOAT_TO_FIXED(x)     ((x) * (1 << __GL_LINE_FRACBITS))
#define __GL_LINE_FIXED_ONE             (1 << __GL_LINE_FRACBITS)
#define __GL_LINE_FIXED_HALF            (1 << (__GL_LINE_FRACBITS-1))
#define __GL_LINE_FIXED_TO_FLOAT(x) 	(((GLfloat) (x)) / __GL_LINE_FIXED_ONE)
#define __GL_LINE_FIXED_TO_INT(x)       (((unsigned int) (x)) >> __GL_LINE_FRACBITS)

/*
** Contains variables needed to draw all line options.
*/
struct __GLlineOptionsRec {
    GLint axis, numPixels;
    __GLfloat offset, length;
    GLint xStart, yStart;
    GLint xLittle, xBig, yLittle, yBig;
    GLint fraction, dfraction;
    __GLfloat curF, curR, curG, curB, curA, curS, curT, curQW;
    __GLzValue curZ;
    __GLfloat antiAliasPercent;
    __GLfloat f0;
    GLint width;
    const __GLvertex *v0, *v1;

    /* Anti-aliased line only info */
    __GLfloat realLength;
    __GLfloat dldx, dldy;
    __GLfloat dddx, dddy;
    __GLfloat dlLittle, dlBig;
    __GLfloat ddLittle, ddBig;
    __GLfloat plength, pwidth;

    /* Anti-aliased stippled lines only */
    __GLfloat stippleOffset;
    __GLfloat oneOverStippleRepeat;
};

/*
** Line state.  Contains all the line specific state, as well as
** procedure pointers used during rendering operations.
*/
typedef struct {
    /*
    ** stipplePosition indicates which bit in mask is being examined
    ** for the next pixel in the line to be rendered.  It is also used
    ** by feedback lines to determine if they are the first of a connected
    ** loop.
    */
    GLint stipplePosition;

    /*
    ** Repeat factor.  After repeat is reduced to zero the
    ** stipplePosition is updated.
    */
    GLint repeat;

    /*
    ** Set to FALSE when the stipple needs to be reset.
    */
    GLboolean notResetStipple;

    __GLlineOptions options;
} __GLlineMachine;

void __glBeginLStrip(__GLcontext *gc);
void __glEndLStrip(__GLcontext *gc);
void __glBeginLLoop(__GLcontext *gc);
void __glEndLLoop(__GLcontext *gc);
void __glBeginLines(__GLcontext *gc);
void __glEndLines(__GLcontext *gc);

void __glClipLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);
void __glFastClipSmoothLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);
void __glFastClipFlatLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);

void __glRenderAliasLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);
void __glRenderFlatFogLine(__GLcontext *gc, __GLvertex *v0, 
			   __GLvertex *v1);
void __glInitLineData(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);
void __glRenderAntiAliasLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);

void __glClipAndRenderLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);

/* 
** assembly depth-test routines
*/
GLboolean __glDT_NEVER( __GLzValue, __GLzValue * );
GLboolean __glDT_LEQUAL( __GLzValue, __GLzValue * );
GLboolean __glDT_LESS( __GLzValue, __GLzValue * );
GLboolean __glDT_EQUAL( __GLzValue, __GLzValue * );
GLboolean __glDT_GREATER( __GLzValue, __GLzValue * );
GLboolean __glDT_NOTEQUAL( __GLzValue, __GLzValue * );
GLboolean __glDT_GEQUAL( __GLzValue, __GLzValue * );
GLboolean __glDT_ALWAYS( __GLzValue, __GLzValue * );

GLboolean __glDT_LEQUAL_M( __GLzValue, __GLzValue * );
GLboolean __glDT_LESS_M( __GLzValue, __GLzValue * );
GLboolean __glDT_EQUAL_M( __GLzValue, __GLzValue * );
GLboolean __glDT_GREATER_M( __GLzValue, __GLzValue * );
GLboolean __glDT_NOTEQUAL_M( __GLzValue, __GLzValue * );
GLboolean __glDT_GEQUAL_M( __GLzValue, __GLzValue * );
GLboolean __glDT_ALWAYS_M( __GLzValue, __GLzValue * );

GLboolean __glDT_NEVER16( __GLzValue, __GLzValue * );
GLboolean __glDT_LEQUAL16( __GLzValue, __GLzValue * );
GLboolean __glDT_LESS16( __GLzValue, __GLzValue * );
GLboolean __glDT_EQUAL16( __GLzValue, __GLzValue * );
GLboolean __glDT_GREATER16( __GLzValue, __GLzValue * );
GLboolean __glDT_NOTEQUAL16( __GLzValue, __GLzValue * );
GLboolean __glDT_GEQUAL16( __GLzValue, __GLzValue * );
GLboolean __glDT_ALWAYS16( __GLzValue, __GLzValue * );

GLboolean __glDT_LEQUAL16_M( __GLzValue, __GLzValue * );
GLboolean __glDT_LESS16_M( __GLzValue, __GLzValue * );
GLboolean __glDT_EQUAL16_M( __GLzValue, __GLzValue * );
GLboolean __glDT_GREATER16_M( __GLzValue, __GLzValue * );
GLboolean __glDT_NOTEQUAL16_M( __GLzValue, __GLzValue * );
GLboolean __glDT_GEQUAL16_M( __GLzValue, __GLzValue * );
GLboolean __glDT_ALWAYS16_M( __GLzValue, __GLzValue * );

extern GLboolean (*__glCDTPixel[32])(__GLzValue, __GLzValue * );

/*
** Line procs
*/
GLboolean __glProcessLine(__GLcontext *);
GLboolean __glProcessLine3NW(__GLcontext *);
GLboolean __glWideLineRep(__GLcontext *);
GLboolean __glDrawBothLine(__GLcontext *);
GLboolean __glScissorLine(__GLcontext *);
GLboolean __glStippleLine(__GLcontext *);
GLboolean __glStencilTestLine(__GLcontext *);
GLboolean __glDepthTestStencilLine(__GLcontext *);
GLboolean __glDepthPassLine(__GLcontext *);
GLboolean __glDitherCILine(__GLcontext *);
GLboolean __glDitherRGBALine(__GLcontext *);
GLboolean __glStoreLine(__GLcontext *);
GLboolean __glAntiAliasLine(__GLcontext *);

/* Optimized line store procs */
GLboolean __glStoreLine_CI_8_Flat(__GLcontext *gc);
GLboolean __glStoreLine_CI_8_Smooth(__GLcontext *gc);
GLboolean __glStoreLine_CI_8_Flat_Dither(__GLcontext *gc);
GLboolean __glStoreLine_CI_8_Smooth_Dither(__GLcontext *gc);
GLboolean __glStoreLine_RGB_16_Flat(__GLcontext *gc);
GLboolean __glStoreLine_RGB_16_Smooth(__GLcontext *gc);
GLboolean __glStoreLine_RGB_16_Dither(__GLcontext *gc);

GLboolean __glStoreStippledLine_CI_8_Flat(__GLcontext *gc);
GLboolean __glStoreStippledLine_CI_8_Smooth(__GLcontext *gc);
GLboolean __glStoreStippledLine_CI_8_Flat_Dither(__GLcontext *gc);
GLboolean __glStoreStippledLine_CI_8_Smooth_Dither(__GLcontext *gc);
GLboolean __glStoreStippledLine_RGB_16_Flat(__GLcontext *gc);
GLboolean __glStoreStippledLine_RGB_16_Smooth(__GLcontext *gc);
GLboolean __glStoreStippledLine_RGB_16_Dither(__GLcontext *gc);

#ifdef __GL_USE_MIPSASMCODE
/* Assembly routines */
void __glDTP_NEVER(void);
void __glDTP_LEQUAL(void);
void __glDTP_LESS(void);
void __glDTP_GREATER(void);
void __glDTP_GEQUAL(void);
void __glDTP_EQUAL(void);
void __glDTP_NOTEQUAL(void);
void __glDTP_ALWAYS(void);
void __glDTP_LEQUAL_M(void);
void __glDTP_LESS_M(void);
void __glDTP_GREATER_M(void);
void __glDTP_GEQUAL_M(void);
void __glDTP_EQUAL_M(void);
void __glDTP_NOTEQUAL_M(void);
void __glDTP_ALWAYS_M(void);
void __glDTP_NEVER16(void);
void __glDTP_LEQUAL16(void);
void __glDTP_LESS16(void);
void __glDTP_GREATER16(void);
void __glDTP_GEQUAL16(void);
void __glDTP_EQUAL16(void);
void __glDTP_NOTEQUAL16(void);
void __glDTP_ALWAYS16(void);
void __glDTP_LEQUAL16_M(void);
void __glDTP_LESS16_M(void);
void __glDTP_GREATER16_M(void);
void __glDTP_GEQUAL16_M(void);
void __glDTP_EQUAL16_M(void);
void __glDTP_NOTEQUAL16_M(void);
void __glDTP_ALWAYS16_M(void);
GLboolean __glDepthTestLine_asm(__GLcontext *gc);
extern void (*__glPDepthTestPixel[32])(void);
#else
GLboolean __glDepthTestLine(__GLcontext *);
#endif


/*
** Line stippled procs
*/
GLboolean __glScissorStippledLine(__GLcontext *);
GLboolean __glWideStippleLineRep(__GLcontext *);
GLboolean __glDrawBothStippledLine(__GLcontext *);
GLboolean __glStencilTestStippledLine(__GLcontext *);
GLboolean __glDepthTestStippledLine(__GLcontext *);
GLboolean __glDepthTestStencilStippledLine(__GLcontext *);
GLboolean __glDepthPassStippledLine(__GLcontext *);
GLboolean __glDitherCIStippledLine(__GLcontext *);
GLboolean __glDitherRGBAStippledLine(__GLcontext *);
GLboolean __glStoreStippledLine(__GLcontext *);
GLboolean __glAntiAliasStippledLine(__GLcontext *);


/************************************************************************/

/*
** Polygon machine state.  Contains all the polygon specific state,
** as well as procedure pointers used during rendering operations.
*/
typedef struct __GLpolygonMachineRec {
    /*
    ** Internal form of users stipple.  Users stipple is always
    ** normalized to stippleWord sized with the LSB of each word mapping
    ** to the left x coordinate.
    */
    __GLstippleWord stipple[32];

    /*
    ** Polygon (triangle really) shading state.  Used by polygon fillers
    ** and span routines.
    */
    __GLshade shader;

    /*
    ** Lookup table that returns the face (0=front, 1=back) when indexed
    ** by a flag which is zero for CW and 1 for CCW.  If FrontFace is CW:
    ** 	face[0] = 0
    ** 	face[1] = 1
    ** else
    ** 	face[0] = 1
    ** 	face[1] = 0
    */
    GLubyte face[2];

    /*
    ** Internal form of polygon mode for each face
    */
    GLubyte mode[2];

    /*
    ** Culling flag.  0 when culling the front face, 1 when culling the
    ** back face and 2 when not culling.
    */
    GLubyte cullFace;
} __GLpolygonMachine;

/* defines for above cullFlag */
#define __GL_CULL_FLAG_FRONT	__GL_FRONTFACE
#define __GL_CULL_FLAG_BACK	__GL_BACKFACE
#define __GL_CULL_FLAG_DONT	2

/* Indicies for face[] array in polygonMachine above */
#define __GL_CW		0
#define __GL_CCW	1

/* Internal numbering for polymode values */
#define __GL_POLYGON_MODE_FILL	(GL_FILL & 0xf)
#define __GL_POLYGON_MODE_LINE	(GL_LINE & 0xf)
#define __GL_POLYGON_MODE_POINT	(GL_POINT & 0xf)

extern void __glBeginPolygon(__GLcontext *gc);
extern void __glBeginQStrip(__GLcontext *gc);
extern void __glBeginQuads(__GLcontext *gc);
extern void __glBeginTFan(__GLcontext *gc);
extern void __glBeginTriangles(__GLcontext *gc);
extern void __glBeginTStrip(__GLcontext *gc);

extern void __glClipTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			     __GLvertex *c, GLuint orClipCodes);
extern void __glClipPolygon(__GLcontext *gc, __GLvertex *v0, GLint nv);
extern void __glDoPolygonClip(__GLcontext *gc, __GLvertex **vp, GLint nv,
			      GLuint orClipCodes);
extern void __glFrustumClipPolygon(__GLcontext *gc, __GLvertex *v0, GLint nv);

extern void __glConvertStipple(__GLcontext *gc);

/* Rectangle processing proc */
extern void __glRect(__GLcontext *gc, __GLfloat x0, __GLfloat y0, 
	             __GLfloat x1, __GLfloat y1);

/*
** Triangle render proc that handles culling, twosided lighting and
** polygon mode.
*/
extern void __glRenderTriangle(__GLcontext *gc, __GLvertex *a,
			       __GLvertex *b, __GLvertex *c);
extern void __glRenderFlatTriangle(__GLcontext *gc, __GLvertex *a,
				   __GLvertex *b, __GLvertex *c);
extern void __glRenderSmoothTriangle(__GLcontext *gc, __GLvertex *a,
				     __GLvertex *b, __GLvertex *c);
extern void __glDontRenderTriangle(__GLcontext *gc, __GLvertex *a,
				   __GLvertex *b, __GLvertex *c);
extern void __glClipAndRenderTriangle(__GLcontext *gc, __GLvertex *v0,
				      __GLvertex *v1, __GLvertex *v2);

extern void __glRenderTriangleLineLock(__GLcontext *gc, __GLvertex *a,
				       __GLvertex *b, __GLvertex *c);
extern void __glRenderTriangleLine(__GLcontext *gc, __GLvertex *a,
				   __GLvertex *b, __GLvertex *c);
extern void __glRenderTrianglePointLock(__GLcontext *gc, __GLvertex *a,
					__GLvertex *b, __GLvertex *c);
extern void __glRenderTrianglePoint(__GLcontext *gc, __GLvertex *a,
				    __GLvertex *b, __GLvertex *c);

/*
** Triangle filling procs for each polygon smooth mode
*/
extern void __glFillTriangle(__GLcontext *gc, __GLvertex *a,
			     __GLvertex *b, __GLvertex *c);
extern void __glFillFlatFogTriangle(__GLcontext *gc, __GLvertex *a,
				    __GLvertex *b, __GLvertex *c);
extern void __glFillAntiAliasedTriangle(__GLcontext *gc, __GLvertex *a,
					__GLvertex *b, __GLvertex *c);

/*
** triangle fill utility functions
*/
void __glSnapXLeft(__GLcontext *gc, __GLfloat xLeft, __GLfloat dxdyLeft);
void __glSnapXRight(__GLshade *sh, __GLfloat xRight, __GLfloat dxdyRight);

/*
** triangle alteration functions
*/
__GLzValue __glPolygonOffsetZ(__GLcontext *gc, const __GLvertex *a,
			__GLfloat dx, __GLfloat dy);

/*
** Span procs
*/
extern GLboolean __glProcessSpan(__GLcontext *);
extern GLboolean __glProcessReplicateSpan(__GLcontext *);
extern GLboolean __glClipSpan(__GLcontext *);
extern GLboolean __glStippleSpan(__GLcontext *);
extern GLboolean __glAlphaTestSpan(__GLcontext *);

#ifdef __GL_USE_MIPSASMCODE
/* Assembly routines */
void __glDTS_NEVER(void);
void __glDTS_LEQUAL(void);
void __glDTS_EQUAL(void);
void __glDTS_GREATER(void);
void __glDTS_NOTEQUAL(void);
void __glDTS_GEQUAL(void);
void __glDTS_ALWAYS(void);
void __glDTS_LESS(void);
void __glDTS_LEQUAL_M(void);
void __glDTS_EQUAL_M(void);
void __glDTS_GREATER_M(void);
void __glDTS_NOTEQUAL_M(void);
void __glDTS_GEQUAL_M(void);
void __glDTS_ALWAYS_M(void);
void __glDTS_LESS_M(void);
void __glDTS_NEVER16(void);
void __glDTS_LEQUAL16(void);
void __glDTS_EQUAL16(void);
void __glDTS_GREATER16(void);
void __glDTS_NOTEQUAL16(void);
void __glDTS_GEQUAL16(void);
void __glDTS_ALWAYS16(void);
void __glDTS_LESS16(void);
void __glDTS_LEQUAL16_M(void);
void __glDTS_EQUAL16_M(void);
void __glDTS_GREATER16_M(void);
void __glDTS_NOTEQUAL16_M(void);
void __glDTS_GEQUAL16_M(void);
void __glDTS_ALWAYS16_M(void);
void __glDTS_LESS16_M(void);
extern GLboolean __glStencilTestSpan_asm(__GLcontext *);
extern GLboolean __glDepthTestSpan_asm(__GLcontext *);
extern GLboolean __glDepthTestStippledSpan_asm(__GLcontext *);
extern GLboolean __glDepthTestStencilSpan_asm(__GLcontext *);
extern GLboolean __glDepthTestStencilStippledSpan_asm(__GLcontext *);
extern void (*__glSDepthTestPixel[32])(void);
#else /* __GL_USE_MIPSASMCODE */
extern GLboolean __glStencilTestSpan(__GLcontext *);
extern GLboolean __glDepthTestSpan(__GLcontext *);
#endif /* __GL_USE_MIPSASMCODE */

/* EXT Extensions */

extern GLboolean __glDepthTestOffsetSpan(__GLcontext *);
extern GLboolean __glDepthTestStencilOffsetSpan(__GLcontext *);

extern GLboolean __glDepthTestStencilSpan(__GLcontext *);
extern GLboolean __glDepthPassSpan(__GLcontext *);
extern GLboolean __glColorSpan1(__GLcontext *);
extern GLboolean __glColorSpan2(__GLcontext *);
extern GLboolean __glColorSpan3(__GLcontext *);
extern GLboolean __glFlatRGBASpan(__GLcontext *);
extern GLboolean __glFlatSecondaryRGBASpan(__GLcontext *);
extern GLboolean __glShadeRGBASpan(__GLcontext *);
extern GLboolean __glShadeSecondaryRGBASpan(__GLcontext *);
extern GLboolean __glFlatCISpan(__GLcontext *);
extern GLboolean __glShadeCISpan(__GLcontext *);
extern GLboolean __glTextureSpan(__GLcontext *);
extern GLboolean __glFogSpan(__GLcontext *);
extern GLboolean __glFogSpanSlow(__GLcontext *);
extern GLboolean __glDrawBothSpan(__GLcontext *);
extern GLboolean __glBlendSpan(__GLcontext *);
extern GLboolean __glBlendSpan_SA_MSA(__GLcontext *);
extern GLboolean __glBlendSpan_MSA_SA(__GLcontext *);
extern GLboolean __glBlendSpan_SA_ZERO(__GLcontext *);
extern GLboolean __glBlendSpan_SA_ONE(__GLcontext *);
extern GLboolean __glDitherRGBASpan(__GLcontext *);
extern GLboolean __glDitherCISpan(__GLcontext *);
extern GLboolean __glRoundRGBASpan(__GLcontext *);
extern GLboolean __glRoundCISpan(__GLcontext*);
extern GLboolean __glLogicOpSpan(__GLcontext *);
extern GLboolean __glMaskRGBASpan(__GLcontext *);
extern GLboolean __glMaskCISpan(__GLcontext *);

/*
** Stippled span procs
*/
extern GLboolean __glStippleStippledSpan(__GLcontext *);
extern GLboolean __glAlphaTestStippledSpan(__GLcontext *);
extern GLboolean __glStencilTestStippledSpan(__GLcontext *);
extern GLboolean __glDepthTestStippledSpan(__GLcontext *);
extern GLboolean __glDepthTestStencilStippledSpan(__GLcontext *);
extern GLboolean __glDepthPassStippledSpan(__GLcontext *);
extern GLboolean __glColorStippledSpan1(__GLcontext *);
extern GLboolean __glColorStippledSpan2(__GLcontext *);
extern GLboolean __glColorStippledSpan3(__GLcontext *);
extern GLboolean __glTextureStippledSpan(__GLcontext *);
extern GLboolean __glFogStippledSpan(__GLcontext *);
extern GLboolean __glFogStippledSpanSlow(__GLcontext *);
extern GLboolean __glDrawBothStippledSpan(__GLcontext *);
extern GLboolean __glBlendStippledSpan(__GLcontext *);
extern GLboolean __glDitherRGBAStippledSpan(__GLcontext *);
extern GLboolean __glDitherCIStippledSpan(__GLcontext *);
extern GLboolean __glRoundRGBAStippledSpan(__GLcontext *);
extern GLboolean __glRoundCIStippledSpan(__GLcontext *);
extern GLboolean __glLogicOpStippledSpan(__GLcontext *);

/*
** optimized texture span processing routines
*/
extern GLboolean __glTextureRGB_L_NML_Span(__GLcontext *);
extern GLboolean __glTextureRGB_L_NML_StippledSpan(__GLcontext *);
extern GLboolean __glTextureRGB_N_N_Span(__GLcontext *);
extern GLboolean __glTextureRGB_N_N_StippledSpan(__GLcontext *);
extern GLboolean __glTextureRGB_N_NMN_Span(__GLcontext *);
extern GLboolean __glTextureRGB_N_NMN_StippledSpan(__GLcontext *);
extern GLboolean __glTextureRGB_L_LML_Span(__GLcontext *);
extern GLboolean __glTextureRGB_L_LML_StippledSpan(__GLcontext *);
extern GLboolean __glTextureCI8_N_N_RGB_Span(__GLcontext *);
extern GLboolean __glTextureCI8_N_N_RGB_StippledSpan(__GLcontext *);
extern GLboolean __glTextureCI8_N_N_RGBA_Span(__GLcontext *);
extern GLboolean __glTextureCI8_N_N_RGBA_StippledSpan(__GLcontext *);

/* EXT Extensions */

extern GLboolean __glDepthTestStippledOffsetSpan(__GLcontext *);
extern GLboolean __glDepthTestStencilStippledOffsetSpan(__GLcontext *);


/************************************************************************/

/* trap rendering routines used for switching between sw/hw */

#if defined(__GL_HW_NEEDS_SYNC)
extern void __glRenderPointSWTrap(__GLcontext *gc, __GLvertex *v);
extern void __glRenderPointHWTrap(__GLcontext *gc, __GLvertex *v);
extern void __glRenderLineSWTrap(__GLcontext *gc, __GLvertex *v0, 
				 __GLvertex *v1);
extern void __glRenderLineHWTrap(__GLcontext *gc, __GLvertex *v0, 
				 __GLvertex *v1);
extern void __glRenderTriangleSWTrap(__GLcontext *gc, __GLvertex *v0,
				     __GLvertex *v1, __GLvertex *v2);
extern void __glRenderTriangleHWTrap(__GLcontext *gc, __GLvertex *v0,
				     __GLvertex *v1, __GLvertex *v2);
#endif /* __GL_HW_NEEDS_SYNC */

/************************************************************************/

extern void __glValidateAlphaTest(__GLcontext *gc);

/************************************************************************/

extern void __glValidateStencil(__GLcontext *gc);

#define __GL_STENCIL_RANGE	(1 << (gc->modes.stencilBits))
#define __GL_MAX_STENCIL_VALUE	(__GL_STENCIL_RANGE - 1)

/************************************************************************/

void __glFogFragmentSlow(__GLcontext *gc, __GLfragment *fr, __GLfloat f);
__GLfloat __glFogVertex(__GLcontext *gc, __GLvertex *fr);
__GLfloat __glFogVertexLinear(__GLcontext *gc, __GLvertex *fr);
void __glFogColorSlow(__GLcontext *gc, __GLcolor *out, __GLcolor *in,
		      __GLfloat fog);

/************************************************************************/

/* color index anti-alias support function */
extern __GLfloat __glBuildAntiAliasIndex(__GLfloat idx,
				         __GLfloat antiAliasPercent);

/************************************************************************/

/*
** Dithering implementation stuff.
*/
#define	__GL_DITHER_BITS 4
#define	__GL_DITHER_PRECISION (1 << __GL_DITHER_BITS)
#define	__GL_DITHER_INDEX(x,y) (((x) & 3) + (((y) & 3) << 2))

extern GLbyte __glDitherTable[16];
extern __GLfloat __glDitherTableFloat[16];

/************************************************************************/

/*
** Deal with differences between HW and SW rasterization when
** falling back to software.
*/
#if !defined(__GL_HALF_PIXEL_OFFSET) || \
     defined(__GL_DEVICE_COLOR_SCALE) || \
     defined(__GL_DEVICE_DEPTH_SCALE)

typedef struct __GLadjustPointDataRec {
#if !defined(__GL_HALF_PIXEL_OFFSET) || defined(__GL_DEVICE_DEPTH_SCALE)
    __GLcoord   window;
#endif
#if defined(__GL_DEVICE_COLOR_SCALE)
    __GLcolor   color;
#endif
} __GLadjustPointData;

typedef struct __GLadjustLineDataRec {
#if !defined(__GL_HALF_PIXEL_OFFSET) || defined(__GL_DEVICE_DEPTH_SCALE)
    __GLcoord   window[2];
#endif
#if defined(__GL_DEVICE_COLOR_SCALE)
    __GLcolor   color[2];
#endif
} __GLadjustLineData;

typedef struct __GLadjustTriangleDataRec {
#if !defined(__GL_HALF_PIXEL_OFFSET) || defined(__GL_DEVICE_DEPTH_SCALE)
    __GLcoord   window[3];
#endif
#if defined(__GL_DEVICE_COLOR_SCALE)
    __GLcolor   color[4], *pvcolor;
#endif
} __GLadjustTriangleData;

#if !defined(__GL_HALF_PIXEL_OFFSET)
#define __GL_ADJUST_VERTEX_XY(gc, v, save) \
		(save).x = (v)->window.x; \
		(save).y = (v)->window.y; \
		(v)->window.x += 0.5F; \
		(v)->window.y += 0.5F

#define __GL_RESTORE_VERTEX_XY(gc, v, save) \
		(v)->window.x = (save).x; \
		(v)->window.y = (save).y
#else
#define __GL_ADJUST_VERTEX_XY(gc, v, save)
#define __GL_RESTORE_VERTEX_XY(gc, v, save)
#endif

#if defined(__GL_DEVICE_DEPTH_SCALE)
#define __GL_ADJUST_VERTEX_Z(gc, v, save) \
		(save).z = (v)->window.z; \
		(v)->window.z *= (gc)->constants.depthRescale

#define __GL_RESTORE_VERTEX_Z(gc, v, save) \
		(v)->window.z = (save).z
#else
#define __GL_ADJUST_VERTEX_Z(gc, v, save)
#define __GL_RESTORE_VERTEX_Z(gc, v, save)
#endif

#if defined(__GL_DEVICE_COLOR_SCALE)
#define __GL_ADJUST_VERTEX_RGBA(gc, v, save) \
		(save) = (v)->color[0]; \
		(v)->color->r *= (gc)->constants.redRescale; \
		(v)->color->g *= (gc)->constants.greenRescale; \
		(v)->color->b *= (gc)->constants.blueRescale; \
		(v)->color->a *= (gc)->constants.alphaRescale

#define __GL_RESTORE_VERTEX_RGBA(gc, v, save) \
		(v)->color->r = (save).r; \
		(v)->color->g = (save).g; \
		(v)->color->b = (save).b; \
		(v)->color->a = (save).a
#else
#define __GL_ADJUST_VERTEX_RGBA(gc, v, save)
#define __GL_RESTORE_VERTEX_RGBA(gc, v, save)
#endif

#if defined(__GL_DEVICE_COLOR_SCALE)
#define __GL_ADJUST_LINE_VERTEX_COLOR(gc, v0, v1, save) \
		__GL_ADJUST_VERTEX_RGBA(gc, v0, save.color[0]); \
		if ((v1)->color != (v0)->color) { \
		    __GL_ADJUST_VERTEX_RGBA(gc, v1, save.color[1]); \
		}

#define __GL_RESTORE_LINE_VERTEX_COLOR(gc, v0, v1, save) \
		__GL_RESTORE_VERTEX_RGBA(gc, v0, save.color[0]); \
		if ((v1)->color != (v0)->color) { \
		    __GL_RESTORE_VERTEX_RGBA(gc, v1, save.color[1]); \
		}

#define __GL_ADJUST_TRIANGLE_VERTEX_COLOR(gc, v0, v1, v2, pv, save) \
		(save).pvcolor = (pv != NULL) ? (pv)->color : NULL; \
		if ((save).pvcolor != NULL) { \
		    __GL_ADJUST_VERTEX_RGBA(gc, pv, save.color[3]); \
		} \
		if ((v0)->color != (save).pvcolor) { \
		    __GL_ADJUST_VERTEX_RGBA(gc, v0, save.color[0]); \
		} \
		if ((v1)->color != (save).pvcolor) { \
		    __GL_ADJUST_VERTEX_RGBA(gc, v1, save.color[1]); \
		} \
		if ((v2)->color != (save).pvcolor) { \
		    __GL_ADJUST_VERTEX_RGBA(gc, v2, save.color[2]); \
		}

#define __GL_RESTORE_TRIANGLE_VERTEX_COLOR(gc, v0, v1, v2, pv, save) \
		if ((save).pvcolor != NULL) { \
		    __GL_RESTORE_VERTEX_RGBA(gc, pv, save.color[3]); \
		} \
		if ((v0)->color != (save).pvcolor) { \
		    __GL_RESTORE_VERTEX_RGBA(gc, v0, save.color[0]); \
		} \
		if ((v1)->color != (save).pvcolor) { \
		    __GL_RESTORE_VERTEX_RGBA(gc, v1, save.color[1]); \
		} \
		if ((v2)->color != (save).pvcolor) { \
		    __GL_RESTORE_VERTEX_RGBA(gc, v2, save.color[2]); \
		}
#else
#define __GL_ADJUST_LINE_VERTEX_COLOR(gc, v0, v1, save)
#define __GL_RESTORE_LINE_VERTEX_COLOR(gc, v0, v1, save)
#define __GL_ADJUST_TRIANGLE_VERTEX_COLOR(gc, v0, v1, v2, pv, save)
#define __GL_RESTORE_TRIANGLE_VERTEX_COLOR(gc, v0, v1, v2, pv, save)
#endif

#define __GL_ADJUST_POINT_VERTEX(gc, v0) \
		__GLadjustPointData adjustData; \
		__GL_ADJUST_VERTEX_XY(gc, v0, (adjustData).window); \
		__GL_ADJUST_VERTEX_Z(gc, v0, (adjustData).window); \
		__GL_ADJUST_VERTEX_RGBA(gc, v0, (adjustData).color) \

#define __GL_RESTORE_POINT_VERTEX(gc, v0) \
		__GL_RESTORE_VERTEX_XY(gc, v0, (adjustData).window); \
		__GL_RESTORE_VERTEX_Z(gc, v0, (adjustData).window); \
		__GL_RESTORE_VERTEX_RGBA(gc, v0, (adjustData).color) \

#define __GL_ADJUST_LINE_VERTS(gc, v0, v1) \
		__GLadjustLineData adjustData; \
		__GL_ADJUST_VERTEX_XY(gc, v0, (adjustData).window[0]); \
		__GL_ADJUST_VERTEX_Z(gc, v0, (adjustData).window[0]); \
		__GL_ADJUST_VERTEX_XY(gc, v1, (adjustData).window[1]); \
		__GL_ADJUST_VERTEX_Z(gc, v1, (adjustData).window[1]); \
		__GL_ADJUST_LINE_VERTEX_COLOR(gc, v0, v1, adjustData)

#define __GL_RESTORE_LINE_VERTS(gc, v0, v1) \
		__GL_RESTORE_VERTEX_XY(gc, v0, (adjustData).window[0]); \
		__GL_RESTORE_VERTEX_Z(gc, v0, (adjustData).window[0]); \
		__GL_RESTORE_VERTEX_XY(gc, v1, (adjustData).window[1]); \
		__GL_RESTORE_VERTEX_Z(gc, v1, (adjustData).window[1]); \
		__GL_RESTORE_LINE_VERTEX_COLOR(gc, v0, v1, adjustData)

#define __GL_ADJUST_TRIANGLE_VERTS(gc, v0, v1, v2, pv) \
		__GLadjustTriangleData adjustData; \
		__GL_ADJUST_VERTEX_XY(gc, v0, (adjustData).window[0]); \
		__GL_ADJUST_VERTEX_Z(gc, v0, (adjustData).window[0]); \
		__GL_ADJUST_VERTEX_XY(gc, v1, (adjustData).window[1]); \
		__GL_ADJUST_VERTEX_Z(gc, v1, (adjustData).window[1]); \
		__GL_ADJUST_VERTEX_XY(gc, v2, (adjustData).window[2]); \
		__GL_ADJUST_VERTEX_Z(gc, v2, (adjustData).window[2]); \
		__GL_ADJUST_TRIANGLE_VERTEX_COLOR(gc, v0, v1, v2, pv, adjustData)

#define __GL_RESTORE_TRIANGLE_VERTS(gc, v0, v1, v2, pv) \
		__GL_RESTORE_VERTEX_XY(gc, v0, (adjustData).window[0]); \
		__GL_RESTORE_VERTEX_Z(gc, v0, (adjustData).window[0]); \
		__GL_RESTORE_VERTEX_XY(gc, v1, (adjustData).window[1]); \
		__GL_RESTORE_VERTEX_Z(gc, v1, (adjustData).window[1]); \
		__GL_RESTORE_VERTEX_XY(gc, v2, (adjustData).window[2]); \
		__GL_RESTORE_VERTEX_Z(gc, v2, (adjustData).window[2]); \
		__GL_RESTORE_TRIANGLE_VERTEX_COLOR(gc, v0, v1, v2, pv, adjustData)

#else
#define __GL_ADJUST_POINT_VERTEX(gc, v0)
#define __GL_RESTORE_POINT_VERTEX(gc, v0)
#define __GL_ADJUST_LINE_VERTS(gc, v0, v1)
#define __GL_RESTORE_LINE_VERTS(gc, v0, v1)
#define __GL_ADJUST_TRIANGLE_VERTS(gc, v0, v1, v2, pv)
#define __GL_RESTORE_TRIANGLE_VERTS(gc, v0, v1, v2, pv)
#endif

#endif /* __gl_render_h_ */
