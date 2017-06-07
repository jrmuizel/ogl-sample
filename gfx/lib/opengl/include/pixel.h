#ifndef __gl_pixel_h_
#define __gl_pixel_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/pixel.h#24 $
*/
#include "types.h"
#include "vertex.h"
#include "consts.h"
#include "ctable.h"

/* Not much for elegance, but it works. */
#define __GL_N_PIXEL_MAPS       (GL_PIXEL_MAP_A_TO_A - GL_PIXEL_MAP_I_TO_I + 1)
#define __GL_REMAP_PM(x)        ((x) - GL_PIXEL_MAP_I_TO_I)
#define __GL_PIXEL_MAP_I_TO_I   __GL_REMAP_PM(GL_PIXEL_MAP_I_TO_I)
#define __GL_PIXEL_MAP_S_TO_S   __GL_REMAP_PM(GL_PIXEL_MAP_S_TO_S)
#define __GL_PIXEL_MAP_I_TO_R   __GL_REMAP_PM(GL_PIXEL_MAP_I_TO_R)
#define __GL_PIXEL_MAP_I_TO_G   __GL_REMAP_PM(GL_PIXEL_MAP_I_TO_G)
#define __GL_PIXEL_MAP_I_TO_B   __GL_REMAP_PM(GL_PIXEL_MAP_I_TO_B)
#define __GL_PIXEL_MAP_I_TO_A   __GL_REMAP_PM(GL_PIXEL_MAP_I_TO_A)
#define __GL_PIXEL_MAP_R_TO_R   __GL_REMAP_PM(GL_PIXEL_MAP_R_TO_R)
#define __GL_PIXEL_MAP_G_TO_G   __GL_REMAP_PM(GL_PIXEL_MAP_G_TO_G)
#define __GL_PIXEL_MAP_B_TO_B   __GL_REMAP_PM(GL_PIXEL_MAP_B_TO_B)
#define __GL_PIXEL_MAP_A_TO_A   __GL_REMAP_PM(GL_PIXEL_MAP_A_TO_A)

/*
** Pixel format not explicitly defined by the spec, but implictly suggested.
** (for processing luminance alpha texture images)
*/
#define __GL_RED_ALPHA		1

/*
** Packed pixel types used for storing texture images
*/
enum {
    __GL_UNSIGNED_SHORT_5_6_5,
    __GL_UNSIGNED_SHORT_X_5_5_5,
    __GL_UNSIGNED_SHORT_1_5_5_5,
    __GL_UNSIGNED_SHORT_4_4_4_4_ARGB
};

/*
** State associated with a pixel map (state settable by glPixelMap*())
*/
typedef struct __GLpixelMapHeadRec {
    GLint size;
    GLint tableId;
    union {
        GLint *mapI;		/* access index (integral) entries */
        __GLfloat *mapF;	/* access component (float) entries */
    } base;
} __GLpixelMapHead;

/*
** State settable by glPixelTransfer*() and glPixelZoom()
*/
typedef struct __GLpixelTransferModeRec {
    __GLcolor scale;          /* Pixel scale and bias */
    __GLcolor bias;

    __GLfloat depthScale;     /* Depth scale and bias */
    __GLfloat depthBias;
    
    __GLcolor postConvolutionScale;
    __GLcolor postConvolutionBias;

    __GLcolor postColorMatrixScale;
    __GLcolor postColorMatrixBias;

    __GLfloat zoomX;
    __GLfloat zoomY;

    GLint indexShift;
    GLint indexOffset;

    GLboolean mapColor;
    GLboolean mapStencil;
} __GLpixelTransferMode;

/*
** State settable with glPixelStore*()
*/
typedef struct __GLpixelPackModeRec {
    GLboolean swapEndian;
    GLboolean lsbFirst;

    GLuint lineLength;
    GLuint imageHeight;
    GLuint skipImages;
    GLuint skipLines;
    GLuint skipPixels;
    GLuint alignment;
} __GLpixelPackMode;

typedef struct __GLpixelUnpackModeRec {
    GLboolean swapEndian;
    GLboolean lsbFirst;

    GLuint lineLength;
    GLuint imageHeight;
    GLuint skipImages;
    GLuint skipLines;
    GLuint skipPixels;
    GLuint alignment;
} __GLpixelUnpackMode;

/*
** Number of convolution filter targets
*/
#define __GL_NUM_CONVOLUTION_TARGETS	3
#define __GL_CONVOLUTION_1D_INDEX	0
#define __GL_CONVOLUTION_2D_INDEX	1
#define __GL_SEPARABLE_2D_INDEX		2

/*
** Stackable per convolution filter state
*/
typedef struct __GLconvolutionFilterStateRec {
    __GLcolor scale;
    __GLcolor bias;

    GLenum borderMode;
    __GLcolor borderColor;
} __GLconvolutionFilterState;

/*
** Convolution filter state
*/
typedef struct __GLconvolutionFilterRec {
    GLenum target;

    __GLfloat *filter;

    GLsizei width;
    GLsizei height;

    GLenum format;			/* actual internal format */
    GLenum formatReturn;		/* requested internal format */
    GLenum baseFormat;			/* internal format w/o size info */
    GLenum type;

    __GLconvolutionFilterState state;	/* stackable state */
} __GLconvolutionFilter;

/*
** Histogram buffer state
*/
typedef struct __GLhistogramRec {
    GLuint *array;

    GLsizei width;

    GLenum format;			/* actual internal format */
    GLenum formatReturn;		/* requested internal format */
    GLenum baseFormat;			/* internal format w/o size info */
    GLenum type;

    GLsizei redSize;
    GLsizei greenSize;
    GLsizei blueSize;
    GLsizei alphaSize;
    GLsizei luminanceSize;

    GLboolean sink;			/* toss pixels after histogram? */
} __GLhistogram;

/*
** Minmax buffer state
*/
typedef struct __GLminmaxRec {
    __GLfloat array[8];

    GLenum format;			/* actual internal format */
    GLenum formatReturn;		/* requested internal format */
    GLenum baseFormat;			/* internal format w/o size info */
    GLenum type;

    GLboolean sink;			/* toss pixels after minmax? */
} __GLminmax;

/*
** Stackable pixel state
*/
typedef struct __GLpixelStateRec {
    __GLpixelTransferMode transferMode;

    /*
    ** Read buffer.  Where pixel reads come from.
    */
    GLenum readBuffer;

    /*
    ** Read buffer specified by user.  May be different from readBuffer
    ** above.  If the user specifies GL_FRONT_LEFT, for example, then 
    ** readBuffer is set to GL_FRONT, and readBufferReturn to 
    ** GL_FRONT_LEFT if the context does not support stereo.
    */
    GLenum readBufferReturn;

    __GLcolorTableState *colorTable[__GL_NUM_COLOR_TABLE_TARGETS];
    __GLconvolutionFilterState *convolutionFilter[__GL_NUM_CONVOLUTION_TARGETS];
} __GLpixelState;

/*
** Stackable client side pixel state
*/
typedef struct __GLclientPixelStateRec {
    __GLpixelPackMode packModes;
    __GLpixelUnpackMode unpackModes;
} __GLclientPixelState;

/*
** Per context rendering state.
*/
typedef struct __GLpixelMachineRec {
    __GLpixelMapHead pixelMap[__GL_N_PIXEL_MAPS];

    __GLcolorTable colorTable[__GL_NUM_COLOR_TABLE_TARGETS];
    __GLcolorTable proxyColorTable[__GL_NUM_COLOR_TABLE_TARGETS];

    __GLconvolutionFilter convolutionFilter[__GL_NUM_CONVOLUTION_TARGETS];

    __GLconvolutionFilter *currentFilter1D;
    GLint adjustWidth1D;

    __GLconvolutionFilter *currentFilter2D;
    GLint adjustWidth2D;
    GLint adjustHeight2D;

    __GLhistogram histogram;
    __GLhistogram proxyHistogram;

    __GLminmax minmax;

    GLuint pixelModeFlags;	/* Operations affecting pixel rectangles */

    GLboolean modifyRGBA;	/* Is the RGBA path being modified? */
    GLboolean modifyCI;
    GLboolean modifyDepth;
    GLboolean modifyStencil;

    GLboolean fastRGBA;		/* are fragment operations disabled? */

    /* scaled values indicating what a red of 0 maps to, an alpha of 1 ... */
    GLfloat red0Mod, green0Mod, blue0Mod, alpha1Mod;
    GLfloat *redMap;		/* Lookup tables with no modification */
    GLfloat *greenMap;
    GLfloat *blueMap;
    GLfloat *alphaMap;
    GLfloat *iMap;
    GLvoid *iCurMap, *redCurMap, *greenCurMap, *blueCurMap, *alphaCurMap;
    GLboolean rgbaCurrent;	
    GLfloat *redModMap;		/* Lookup tables for modification path */
    GLfloat *greenModMap;
    GLfloat *blueModMap;
    GLfloat *alphaModMap;
    GLboolean iToICurrent;	/* Lookup table for modification of CI */
    GLfloat *iToIMap;
    GLboolean iToRGBACurrent;	/* Lookup tables from CI to RGBA */
    GLfloat *iToRMap;
    GLfloat *iToGMap;
    GLfloat *iToBMap;
    GLfloat *iToAMap;
} __GLpixelMachine;

extern void __glInitDefaultPixelMap(__GLcontext *gc, GLenum map);

/************************************************************************/

/* Bitmasks for pixelModeFlags */
#define __GL_PIXEL_MODE_INDEX_LOOKUP			(1 <<  0)
#define __GL_PIXEL_MODE_STENCIL_LOOKUP			(1 <<  1)
#define __GL_PIXEL_MODE_INDEX_SHIFT_OFFSET		(1 <<  2)
#define __GL_PIXEL_MODE_RGBA_LOOKUP			(1 <<  3)
#define __GL_PIXEL_MODE_RGBA_SCALE_BIAS			(1 <<  4)
#define __GL_PIXEL_MODE_DEPTH_SCALE_BIAS		(1 <<  5)
#define __GL_PIXEL_MODE_COLOR_TABLE			(1 <<  6)
#define __GL_PIXEL_MODE_CONVOLUTION			(1 <<  7)
#define __GL_PIXEL_MODE_POST_CONVOLUTION_SCALE_BIAS	(1 <<  8)
#define __GL_PIXEL_MODE_POST_CONVOLUTION_COLOR_TABLE	(1 <<  9)
#define __GL_PIXEL_MODE_COLOR_MATRIX			(1 << 10)
#define __GL_PIXEL_MODE_POST_COLOR_MATRIX_SCALE_BIAS	(1 << 11)
#define __GL_PIXEL_MODE_POST_COLOR_MATRIX_COLOR_TABLE	(1 << 12)
#define __GL_PIXEL_HISTOGRAM				(1 << 13)
#define __GL_PIXEL_MINMAX				(1 << 14)

/************************************************************************/

#define __GL_MAX_SPAN_WIDTH	(__GL_MAX_MAX_VIEWPORT)
#define __GL_MAX_SPAN_SIZE	(__GL_MAX_SPAN_WIDTH * 4 * sizeof(GLfloat))

struct __GLpixelSpanInfoRec {
    GLenum srcFormat, srcType;	/* Form of source image */
    const GLvoid *srcImage;	/* The source image */
    const GLvoid *srcCurrent;	/* The current pointer into the source data */
    GLint srcRowIncrement;	/* Add this much to get to the next row */
    GLint srcGroupIncrement;	/* Add this much to get to the next group */
    GLint srcImageIncrement;	/* Add this much to get to the next image */
    GLint srcComponents;	/* (4 for RGBA, 1 for ALPHA, etc.) */
    GLint srcElementSize;	/* Size of one element (1 for BYTE) */
    GLint srcSwapBytes;		
    GLint srcLsbFirst;
    GLint srcSkipPixels, srcSkipLines, srcSkipImages;
    GLint srcLineLength, srcImageHeight;
    GLint srcAlignment;
    GLint srcStartBit;		/* After applying skipPixels */

    GLenum dstFormat, dstType;	/* Form of destination image */
    GLvoid *dstImage;		/* The destination image */
    GLvoid *dstCurrent;		/* The current pointer into the dest data */
    GLint dstRowIncrement;	/* Add this much to get to the next row */
    GLint dstGroupIncrement;	/* Add this much to get to the next group */
    GLint dstImageIncrement;	/* Add this much to get to the next image */
    GLint dstComponents;	/* (4 for RGBA, 1 for ALPHA, etc.) */
    GLint dstElementSize;	/* Size of one element (1 for BYTE) */
    GLint dstSwapBytes;		
    GLint dstLsbFirst;
    GLint dstSkipPixels, dstSkipLines, dstSkipImages;
    GLint dstLineLength, dstImageHeight;
    GLint dstAlignment;
    GLint dstStartBit;		/* After applying skipPixels */

    __GLfloat zoomx, zoomy;
    GLint width, height, depth;	/* Size of image */
    GLint dim;			/* dimensionality of calling routine */

    __GLfloat readX, readY;	/* Reading coords (CopyPixels, ReadPixels) */
    __GLfloat x, y;		/* Effective raster coordinates */
    GLint startCol, startRow;	/* First actual pixel goes here */
    GLint endCol;		/* Last column rendered (minus coladd) */
    GLint columns, rows;	/* Taking zoomx, zoomy into account */
    GLboolean overlap;		/* Do CopyPixels src/dest regions overlap? */
    GLint rowsUp, rowsDown;	/* Stuff for overlapping CopyPixels regions */
    GLint rowadd, coladd;	/* Adders for incrementing the col or row */
    __GLfloat rendZoomx;	/* effective zoomx for render span */
    __GLzValue fragz;		/* save this computation in the span walker */
    __GLfloat rpyUp, rpyDown;
    GLint startUp, startDown;
    GLint readUp, readDown;
    GLvoid *redMap, *greenMap, *blueMap, *alphaMap;
    GLvoid *indexMap;
    GLshort *pixelArray;	/* Array of pixel replication counts (if    */
				/* zoomx < -1 or zoomx > 1) or pixels to   */
				/* skip (if zoomx < 1 and zoomx > -1).     */

    __GLcolor scale;		/* generic scale values */
    __GLcolor bias;		/* generic bias values */

    /*
    ** A pile of span routines used by the DrawPixels, ReadPixels, and 
    ** CopyPixels functions.
    */
    GLint numSpanMods; 		/* number of span modifiers current in use */
    void (*spanReader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       GLvoid *outspan);
    void (*(spanModifier[13]))(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		              const GLvoid *inspan, GLvoid *outspan);
    void (*spanRender)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan);

    /*
    ** These fields are used only for selecting spanModifiers.  The
    ** span modifier routines themselves do not use them.  They are
    ** in this record because they need to be passed through copyImage.
    */
    GLboolean applySrcClamp;		/* clamp source data */
    GLboolean applyDstClamp;		/* clamp destination data */
    GLboolean applyFbScale;		/* apply frame buffer scale/unscale */
    GLboolean applyPixelTransfer;	/* apply pixel transfer operations */
    GLboolean applyGenericScaleBias;	/* apply non pix xfer scale and bias */
    GLboolean nonColorComp;		/* not color components (eg hgram) */

    /*
    ** These fields are used by convolution operators
    */
    GLint spanCount;		/* running counter */
    GLint convInitialSpans;
    GLint convFinalSpans;
    GLint convModifierIndex;	/* index of the convolution span Modifier */

    __GLconvolutionFilter *filter;
    GLvoid **spanCache;		/* cache of spans for convolution */
    void (*convolveRows)(__GLcontext *gc, GLint spanCount,
    		__GLconvolutionFilter *filter, GLint firstRow, GLint lastRow,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
};

/* px_api.c */
extern GLboolean __glCheckDrawPixelArgs(__GLcontext *gc, GLsizei width, 
					GLsizei height, GLenum format, 
					GLenum type);
extern GLboolean __glCheckReadPixelArgs(__GLcontext *gc, GLsizei width,
					GLsizei height, GLenum format,
					GLenum type);
void __glPixelSetColorScales(__GLcontext *);

/* px_paths.c */
GLboolean __glNeedScaleBias(__GLcontext *gc, __GLcolor *scale, __GLcolor *bias);
GLboolean __glClipDrawPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glComputeSpanPixelArray(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glLoadUnpackModes(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 GLboolean packed);
void __glInitDrawPixelsInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            GLint width, GLint height, GLenum format, 
                            GLenum type, const GLvoid *pixels);

void __glOptPickDrawPixels(__GLcontext *gc, GLint width, GLint height,
		            GLenum format, GLenum type, const GLvoid *pixels,
			    GLboolean packed);

void __glDrawPixelsConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glDrawPixelsN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glDrawPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glDrawPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glDrawPixels0(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

void __glSlowPickDrawPixels(__GLcontext *gc, GLint width, GLint height,
                            GLenum format, GLenum type, const GLvoid *pixels,
			    GLboolean packed);
void __glGenericPickDrawPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
GLboolean __glClipReadPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glLoadPackModes(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glInitReadPixelsInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            GLint x, GLint y, GLint width, GLint height, 
                            GLenum format, GLenum type, GLvoid *pixels);

void __glReadPixelsConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glReadPixelsN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glReadPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glReadPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glReadPixels0(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

void __glSlowPickReadPixels(__GLcontext *gc, GLint x, GLint y,
                            GLsizei width, GLsizei height,
                            GLenum format, GLenum type, GLvoid *pixels);
void __glGenericPickReadPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

GLboolean __glClipCopyPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glInitCopyPixelsInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                            GLint x, GLint y, GLint width, GLint height, 
                            GLenum format);

void __glCopyPixelsConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyPixelsN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyPixels0(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyPixelsOverlapping(__GLcontext *gc, 
                               __GLpixelSpanInfo *spanInfo, GLint modifiers);

void __glSlowPickCopyPixels(__GLcontext *gc, GLint x, GLint y, GLint width,
                            GLint height, GLenum type);
void __glGenericPickCopyPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

void __glCopyImageConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyImageN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyImage2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyImage1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glCopyImage3D(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		void (*cpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo));

void __glGenericPickCopyImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

void __glGenericPickStoreTexture(__GLcontext *gc, __GLmipMapLevel *lp,
                                 __GLpixelSpanInfo *spanInfo);

void __glGenericPickGetTexture(__GLcontext *gc, __GLmipMapLevel *lp,
                               __GLpixelSpanInfo *spanInfo);

void __glInitReadImageSrcInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			      GLint x, GLint y,
			      GLsizei width, GLsizei height);
void __glGenericPickReadImage(__GLcontext *gc, __GLmipMapLevel *lp, 
                              __GLpixelSpanInfo *spanInfo);
void __glInitImagePack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       GLint width, GLint height, GLenum format, GLenum type,
                       GLvoid *buf);
void __glInitMemGet(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			GLsizei width, GLsizei height, GLenum internalFormat,
			const GLvoid *buf);
void __glInitMemUnpack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			GLsizei width, GLsizei height, GLsizei depth,
			GLenum format, GLenum type, const GLvoid *buf,
			GLboolean dltex);
void __glInitMemPack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			GLsizei width, GLsizei height, GLsizei depth,
			GLenum format, GLenum type, GLvoid *buf);
void __glInitMemLoad(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		     GLenum internalFormat, GLvoid *buf);
void __glLateScissorRender(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   GLvoid *inspan);


/* px_modify.c */
void __glBuildRGBAModifyTables(__GLcontext *gc, __GLpixelMachine *pm);
void __glBuildItoIModifyTables(__GLcontext *gc, __GLpixelMachine *pm);
void __glBuildItoRGBAModifyTables(__GLcontext *gc, 
				  __GLpixelMachine *pm);
void __glSpanModifyRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		        const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		        const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		        const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		        const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyBGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyLuminance(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyLuminanceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                                  const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyRedAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyDepth(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyStencil(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                           const GLvoid *inspan, GLvoid *outspan);
void __glSpanModifyCI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                      const GLvoid *inspan, GLvoid *outspan);

void __glSpanGenericScaleBiasRGBA(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo,
		const GLvoid *inspan, GLvoid *outspan);

void __glSpanColorTableRGBA(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo,
		const GLvoid *inspan, GLvoid *outspan);
void __glSpanPostConvolutionColorTableRGBA(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo,
		const GLvoid *inspan, GLvoid *outspan);
void __glSpanPostColorMatrixColorTableRGBA(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo,
		const GLvoid *inspan, GLvoid *outspan);

/* px_pack.c */
void __glInitPacker(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glSpanReduceRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceBGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceLuminance(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceLuminanceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                                  const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceRedAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *inspan, GLvoid *outspan);
void __glSpanReorderABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanReorderBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceRedNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceGreenNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceBlueNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceRGBNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceBGRNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanReduceRedAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanReorderABGRNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan);
void __glSpanReorderBGRANS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan);

void __glSpanPackUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackByte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                      const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackShort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                      const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack332Ubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			  const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack233RevUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack565Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack565RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack4444RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackARGB4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack5551Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack1555RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack8888Uint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			  const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack8888RevUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack_10_10_10_2_Uint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				  const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack_2_10_10_10_RevUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				     const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackX555Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack1555Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPack565Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackInt(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                     const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackUbyteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackByteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackUshortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackShortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackUintI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackIntI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                      const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackNonCompByte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackNonCompUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackNonCompShort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackNonCompUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackNonCompInt(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackNonCompUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan);

void __glSpanCopy(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		  const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackBitmap(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	                const GLvoid *inspan, GLvoid *outspan);

/* px_read.c */
void __glSpanReadRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                      GLvoid *span);
void __glSpanReadRGBA2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       GLvoid *span);
void __glSpanReadCI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                    GLvoid *span);
void __glSpanReadCI2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                     GLvoid *span);
void __glSpanReadDepth(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       GLvoid *span);
void __glSpanReadDepth2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        GLvoid *span);
void __glSpanReadDepthUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   GLvoid *span);
void __glSpanReadDepthUint2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    GLvoid *span);
void __glSpanReadStencil(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         GLvoid *span);
void __glSpanReadStencil2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                          GLvoid *span);

/* px_render.c */
void __glSlowDrawPixelsStore(__GLcolorBuffer *cfb, const __GLfragment *frag);
void __glSpanRenderRGBubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *span);
void __glSpanRenderBGRubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *span);
void __glSpanRenderRGBubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *span);
void __glSpanRenderBGRubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *span);
void __glSpanRenderRGBAubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *span);
void __glSpanRenderRGBAubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                              const GLvoid *span);
void __glSpanRenderDepthUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			     const GLvoid *span);
void __glSpanRenderDepthUint2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			      const GLvoid *span);
void __glSpanRenderStencilUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                                 const GLvoid *span);
void __glSpanRenderStencilUshort2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                                  const GLvoid *span);
void __glSpanRenderStencilUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                                const GLvoid *span);
void __glSpanRenderStencilUbyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                                 const GLvoid *span);
void __glSpanRenderCIuint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *span);
void __glSpanRenderCIuint2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *span);
void __glSpanRenderCIushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *span);
void __glSpanRenderCIushort2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *span);
void __glSpanRenderCIubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                           const GLvoid *span);
void __glSpanRenderCIubyte2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *span);
void __glSpanRenderCIubyte3(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *span);
void __glSpanRenderCIubyte4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *span);
void __glSpanRenderRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                        const GLvoid *span);
void __glSpanRenderRGBA2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *span);
void __glSpanRenderDepth(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *span);
void __glSpanRenderDepth2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                          const GLvoid *span);
void __glSpanRenderCI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                      const GLvoid *span);
void __glSpanRenderCI2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *span);
void __glSpanRenderStencil(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                           const GLvoid *span);
void __glSpanRenderStencil2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *span);

/* px_unpack.c */
GLint __glElementsPerGroup(GLenum format, GLenum type);
GLint __glBytesPerElement(GLenum type);
void __glInitUnpacker(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
void __glSpanUnpackBitmap(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                          const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackBitmap2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                           const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackRGBubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                            const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackIndexUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                              const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackRGBAubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanSwapBytes2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanSwapBytes2Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                           const GLvoid *inspan, GLvoid *outspan);
void __glSpanSwapAndSkipBytes2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                               const GLvoid *inspan, GLvoid *outspan);
void __glSpanSwapBytes4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanSwapBytes4Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                           const GLvoid *inspan, GLvoid *outspan);
void __glSpanSwapAndSkipBytes4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                               const GLvoid *inspan, GLvoid *outspan);
void __glSpanSkipPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanSkipPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanSkipPixels4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanSlowSkipPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanSlowSkipPixels4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanAlignPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                          const GLvoid *inspan, GLvoid *outspan);
void __glSpanAlignPixels2Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanAlignPixels4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                          const GLvoid *inspan, GLvoid *outspan);
void __glSpanAlignPixels4Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackByte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                          const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackShort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackInt(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack332Ubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack233RevUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack565Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack565RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack4444RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				 const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackARGB4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack5551Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack1555RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				 const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack8888Uint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack8888RevUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack_10_10_10_2_Uint(__GLcontext *gc, 
		       __GLpixelSpanInfo *spanInfo, 
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack_2_10_10_10_RevUint(__GLcontext *gc, 
				       __GLpixelSpanInfo *spanInfo, 
				       const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackX555Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack1555Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpack565Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackUbyteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                          const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackByteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackUshortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                           const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackShortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                          const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackUintI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackIntI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnpackNonCompUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampFloat(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		      const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampFloatDst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanClampSignedDst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandBGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandLuminance(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                             const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandLuminanceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                                  const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandRedAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                            const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandRedNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandGreenNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandBlueNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                        const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                         const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandRGBNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandBGRNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandLuminanceNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandLuminanceAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				    const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandRedAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandIntensityNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       const GLvoid *inspan, GLvoid *outspan);

void __glSpanScaleRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanScaleABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanScaleBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPreReorderABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPreReorderBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPreUnscaleRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPostUnscaleRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			     const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnscaleABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanUnscaleBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanExpandIntensity(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan);

void __glSpanPackL8ToXRGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackRGB8ToXRGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                                const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackRGBA8ToXRGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                                 const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackL8ToRGB565(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackRGB8ToRGB565(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                              const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackRGBA8ToRGB565(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackLA8ToARGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackRGBA8ToARGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                                 const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackLA8ToARGB4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanPackRGBA8ToARGB4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan);

void __glSpanHistogramA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramL(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramLA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan);

void __glSpanHistogramSinkA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramSinkL(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramSinkLA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			     const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramSinkRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			      const GLvoid *inspan, GLvoid *outspan);
void __glSpanHistogramSinkRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       const GLvoid *inspan, GLvoid *outspan);

void __glSpanMinmaxA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		     const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxL(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		     const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxLA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		      const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan);

void __glSpanMinmaxSinkA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		     const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxSinkL(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		     const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxSinkLA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		      const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxSinkRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan);
void __glSpanMinmaxSinkRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan);

void __glSpanColorMatrixGeneralRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);
void __glSpanColorMatrixNoScaleBiasRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);
void __glSpanColorMatrixOnlyScaleBiasRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);

/* s_convol.c */
GLboolean __glCheckConvolutionFilterArgs(__GLcontext *gc, GLenum target,
					 GLsizei width, GLsizei height,
					 GLenum internalformat,
					 GLenum format, GLenum type);
void GLAPI __gllei_ConvolutionFilter1D(__GLcontext *gc, GLenum target,
			GLenum internalformat, GLsizei width,
			GLenum format, GLenum type, const GLvoid *image);
void GLAPI __gllei_ConvolutionFilter2D(__GLcontext *gc, GLenum target,
			GLenum internalformat, GLsizei width, GLsizei height,
			GLenum format, GLenum type, const GLvoid *image);
void GLAPI __gllei_SeparableFilter2D(__GLcontext *gc, GLenum target,
			GLenum internalformat, GLsizei width, GLsizei height,
			GLenum format, GLenum type,
			const GLvoid *row, const GLvoid *column);
void __glEarlyInitConvolutionState(__GLcontext *gc);
void __glInitConvolutionState(__GLcontext *gc);
void __glFreeConvolutionState(__GLcontext *gc);

/* s_hist.c */
void __glInitHistogramState(__GLcontext *gc);
void __glFreeHistogramState(__GLcontext *gc);

void __glInitMinmaxState(__GLcontext *gc);
void __glFreeMinmaxState(__GLcontext *gc);

/* px_convol.c */
void __glSpanPostConvScaleBiasRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);
void __glSpanConvolution1DReduceRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);
void __glSpanConvolution1DBorderRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);
void __glSpanConvolution2DReduceRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);
void __glSpanConvolution2DBorderRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan);

void __glConvolveRowsReduceL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReduceLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReduceA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReduceI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReduceRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReduceRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsConstantL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);

void __glConvolveRowsConstantLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsConstantA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsConstantI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsConstantRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsConstantRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);

void __glConvolveRowsReplicateL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReplicateLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReplicateA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReplicateI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReplicateRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsReplicateRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);

void __glConvolveRowsSeparableReduceL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReduceLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReduceA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReduceI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReduceRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReduceRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);

void __glConvolveRowsSeparableConstantL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableConstantLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableConstantA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableConstantI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableConstantRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableConstantRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);

void __glConvolveRowsSeparableReplicateL(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReplicateLA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReplicateA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReplicateI(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReplicateRGB(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);
void __glConvolveRowsSeparableReplicateRGBA(__GLcontext *gc, GLint spanCount,
		__GLconvolutionFilter *filter, GLint firstRow, GLint numRows,
		GLint width, GLint height, const GLfloat *inspan,
		GLint cacheRow, GLfloat **spanCache);

#endif /* __gl_pixel_h_ */
