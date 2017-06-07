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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_paths.c#22 $
*/

#include <math.h>
#include "context.h"
#include "global.h"
#include "pixel.h"
#include "g_imports.h"

/*
** Determines if the given type is a packed_pixel format
*/
static GLboolean isPackedType(GLenum type)
{
    switch (type) {
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case __GL_UNSIGNED_SHORT_5_6_5:
    case __GL_UNSIGNED_SHORT_X_5_5_5:
    case __GL_UNSIGNED_SHORT_1_5_5_5:
    case __GL_UNSIGNED_SHORT_4_4_4_4_ARGB:
        return GL_TRUE;
    default:
        return GL_FALSE;
    }
}

/*
** Determines if a scale/bias is not set to an identity transformation.
*/
GLboolean __glNeedScaleBias(__GLcontext *gc, __GLcolor *scale, __GLcolor *bias)
{
    return ((scale->r != __glOne) ||
    	    (scale->g != __glOne) ||
	    (scale->b != __glOne) ||
	    (scale->a != __glOne) ||
	    (bias->r != __glZero) ||
	    (bias->g != __glZero) ||
	    (bias->b != __glZero) ||
	    (bias->a != __glZero));
}

/*
** This structure is used when configuring spanModifiers
*/
typedef struct __GLpixelSpanModInfo {
	GLint srcData;			/* location of source data */
	GLint dstData;			/* location of destination data */
	GLint pixelPath;		/* type of pixel path */

	GLboolean srcSwap, dstSwap;
	GLboolean srcAlign, dstAlign;
	GLboolean srcConvert, dstConvert;
	GLboolean srcExpand, dstReduce;
	GLboolean srcClamp, dstClamp;

	GLboolean zoomx1;		/* -1 <= zoomx <= 1? */
	GLboolean zoomx2;		/* zoomx <= -1 || zoomx >= 1 */

	GLboolean skipGroups;		/* decimating zoom */
	GLboolean modify;		/* core pixel transfer ops */
	GLboolean modify2;		/* ARB_imaging pixel transfer ops */
} __GLpixelSpanModInfo;

/* values for the pixelPath field */
#define __GL_PIXPATH_DRAWPIX	1
#define __GL_PIXPATH_READPIX	2
#define __GL_PIXPATH_COPYPIX	3
#define __GL_PIXPATH_READIMAGE	4
#define __GL_PIXPATH_COPYIMAGE	5

/* values for the srcData and dstData fields */
#define __GL_PIXDATA_FB  	1
#define __GL_PIXDATA_MEM 	2

/*
** Functions to pick `fast-path' spanModifiers for Copy/Read/Copy operations
*/

static GLboolean
PickFastCopyImageModifiers(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    /*
    ** At this point we can pick fastpaths to optimize trivial data copies,
    ** including conversion to packed internal formats.
    */
    if ((spanInfo->srcType != GL_UNSIGNED_BYTE) ||
        (spanInfo->dstType != GL_UNSIGNED_BYTE &&
		!isPackedType(spanInfo->dstType)) ||
        spanModInfo->skipGroups ||
	spanModInfo->modify || spanModInfo->modify2 ||
	spanModInfo->srcAlign || spanModInfo->dstAlign ||
	spanModInfo->srcClamp || spanModInfo->dstClamp) {
        return GL_FALSE;
    }

    /* Look for trivial memcpy case */
    if (spanInfo->srcType == spanInfo->dstType &&
		    !spanModInfo->srcExpand && !spanModInfo->dstReduce) {
	spanInfo->spanModifier[spanInfo->numSpanMods++] = __glSpanCopy;
	return GL_TRUE;
    }

    /*
    ** Optimized fastpaths for pack_pixel format downloads.
    ** Unsupported types fall through to the slow paths.
    */
    switch (spanInfo->dstType) {
    case __GL_UNSIGNED_SHORT_5_6_5:
	switch (spanInfo->srcComponents) {
	case 1:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackL8ToRGB565;
	    return GL_TRUE;
	case 3:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackRGB8ToRGB565;
	    return GL_TRUE;
	case 4:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackRGBA8ToRGB565;
	    return GL_TRUE;
	}
	break;
    case __GL_UNSIGNED_SHORT_X_5_5_5:
	switch (spanInfo->srcComponents) {
	case 1:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackL8ToXRGB1555;
	    return GL_TRUE;
	case 3:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackRGB8ToXRGB1555;
	    return GL_TRUE;
	case 4:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackRGBA8ToXRGB1555;
	    return GL_TRUE;
	}
	break;
    case __GL_UNSIGNED_SHORT_1_5_5_5:
	switch (spanInfo->srcComponents) {
	case 2:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackLA8ToARGB1555;
	    return GL_TRUE;
	case 4:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackRGBA8ToARGB1555;
	    return GL_TRUE;
	}
	break;
    case __GL_UNSIGNED_SHORT_4_4_4_4_ARGB:
	switch (spanInfo->srcComponents) {
	case 2:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackLA8ToARGB4;
	    return GL_TRUE;
	case 4:
	    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		__glSpanPackRGBA8ToARGB4;
	    return GL_TRUE;
	}
	break;
    }
    return GL_FALSE;
}

static GLboolean
PickFastReadPixelsModifiers(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    /*
    ** No fast paths for ARB_imaging operations
    */
    if (spanModInfo->modify2) {
        return GL_FALSE;
    }

    /*
    ** Optimization attempt for ReadPixels.
    **
    ** This was taken from the DrawPixels path and modified
    ** so the optimizations for DrawPixels will be symmetrical
    ** to the optimizations for ReadPixels.
    **
    ** There are some format, type combinations that are expected to be 
    ** common.  This code optimizes a few of those cases.  Specifically,
    ** these modes include:  (GL_UNSIGNED_BYTE, GL_RGB), 
    ** (GL_UNSIGNED_BYTE, GL_RGBA), (GL_UNSIGNED_BYTE, GL_COLOR_INDEX),
    ** (GL_UNSIGNED_BYTE, GL_STENCIL_INDEX), 
    ** (GL_UNSIGNED_SHORT, GL_COLOR_INDEX), 
    ** (GL_UNSIGNED_SHORT, GL_STENCIL_INDEX),
    ** (GL_UNSIGNED_INT, GL_DEPTH_COMPONENT)
    */
    switch(spanInfo->dstType) {
    case GL_UNSIGNED_BYTE:
	switch(spanInfo->dstFormat) {
	case GL_RGB:
	    break;
	case GL_RGBA:
	    break;
	case GL_STENCIL_INDEX:
	    break;
	case GL_COLOR_INDEX:
	    break;
	default:
	    break;
	}
	break;
    case GL_UNSIGNED_SHORT:
	switch(spanInfo->dstFormat) {
	case GL_STENCIL_INDEX:
	    break;
	case GL_COLOR_INDEX:
	    break;
	default:
	    break;
	}
	break;
    case GL_UNSIGNED_INT:
	switch(spanInfo->dstFormat) {
	case GL_DEPTH_COMPONENT:
	    if (!spanModInfo->modify) {
		/* XXX: accessing numFracBits without a lock */
		GLuint scale = gc->depthBuffer.scale 
		    >> __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;

		/*
		 ** we will be not converting to float if:
		 **  - scale will be of the form 2^n -1 after the
		 **    shift imposed by numFracBits
		 */
		if ( (scale & (scale+1)) == 0x00000000 ) {
		    spanInfo->spanReader = __glSpanReadDepthUint2;
		    return GL_TRUE;
		}
	    }
	    break;
	default:
	    break;
	}
	break;
    default:
	break;
    }
    return GL_FALSE;
}

static GLboolean
PickFastDrawPixelsModifiers(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    __GLpixelMachine *pm = &gc->pixel;
    GLboolean packedUserData;

    /*
    ** No fast paths for ARB_imaging operations
    */
    if (spanModInfo->modify2) {
        return GL_FALSE;
    }

    /*
    ** Optimization attempt for DrawPixels.
    **
    ** There are some format, type combinations that are expected to be 
    ** common.  This code optimizes a few of those cases.  Specifically,
    ** these modes include:  (GL_UNSIGNED_BYTE, GL_RGB), 
    ** (GL_UNSIGNED_BYTE, GL_RGBA), (GL_UNSIGNED_BYTE, GL_COLOR_INDEX),
    ** (GL_UNSIGNED_BYTE, GL_STENCIL_INDEX), 
    ** (GL_UNSIGNED_SHORT, GL_COLOR_INDEX), 
    ** (GL_UNSIGNED_SHORT, GL_STENCIL_INDEX),
    ** (GL_UNSIGNED_INT, GL_DEPTH_COMPONENT)
    ** (GL_UNSIGNED_INT, GL_COLOR_INDEX)
    */
    switch(spanInfo->srcType) {
    case GL_UNSIGNED_BYTE:
	packedUserData = spanModInfo->zoomx2 &&
	    (spanModInfo->srcSwap ? GL_FALSE : GL_TRUE);
	switch(spanInfo->srcFormat) {
	case GL_RGB:
	    if (packedUserData) {
		/* no span unpacking is necessary! */
	    } else {
		/* zoomx2 must not be true, or packedUserData would be set */
		assert(!spanModInfo->zoomx2);
		spanInfo->spanModifier[spanInfo->numSpanMods++] =
				__glSpanUnpackRGBubyte;
	    }
	    if (!spanModInfo->modify) {
		pm->redCurMap = pm->redMap;
		pm->greenCurMap = pm->greenMap;
		pm->blueCurMap = pm->blueMap;
		pm->alphaCurMap = pm->alphaMap;
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderRGBubyte2;
		} else {
		    spanInfo->spanRender = __glSpanRenderRGBubyte;
		}
	    } else {
		if (!pm->rgbaCurrent) {
		    __glBuildRGBAModifyTables(gc, pm);
		}
		pm->redCurMap = pm->redModMap;
		pm->greenCurMap = pm->greenModMap;
		pm->blueCurMap = pm->blueModMap;
		pm->alphaCurMap = pm->alphaModMap;
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderRGBubyte2;
		} else {
		    spanInfo->spanRender = __glSpanRenderRGBubyte;
		}
	    }
	    return GL_TRUE;
	case GL_BGR:	/* copied from gl_rgb above */
	    if (packedUserData) {
		/* no span unpacking is necessary! */
	    } else {
		/* zoomx2 must not be true, or packedUserData would be set */
		assert(!spanModInfo->zoomx2);
		spanInfo->spanModifier[spanInfo->numSpanMods++] =
				__glSpanUnpackRGBubyte; /* no need for bgr */
	    }
	    if (!spanModInfo->modify) {
		pm->redCurMap = pm->redMap;
		pm->greenCurMap = pm->greenMap;
		pm->blueCurMap = pm->blueMap;
		pm->alphaCurMap = pm->alphaMap;
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderBGRubyte2;
		} else {
		    spanInfo->spanRender = __glSpanRenderBGRubyte;
		}
	    } else {
		if (!pm->rgbaCurrent) {
		    __glBuildRGBAModifyTables(gc, pm);
		}
		pm->redCurMap = pm->redModMap;
		pm->greenCurMap = pm->greenModMap;
		pm->blueCurMap = pm->blueModMap;
		pm->alphaCurMap = pm->alphaModMap;
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderBGRubyte2;
		} else {
		    spanInfo->spanRender = __glSpanRenderBGRubyte;
		}
	    }
	    return GL_TRUE;
	case GL_RGBA:
	    if (packedUserData) {
		/* no span unpacking is necessary! */
	    } else {
		/* zoomx2 must not be true, or packedUserData would be set */
		assert(!spanModInfo->zoomx2);
		spanInfo->spanModifier[spanInfo->numSpanMods++] =
				__glSpanUnpackRGBAubyte;
	    }
	    if (!spanModInfo->modify) {
		pm->redCurMap = pm->redMap;
		pm->greenCurMap = pm->greenMap;
		pm->blueCurMap = pm->blueMap;
		pm->alphaCurMap = pm->alphaMap;
	    } else {
		if (!pm->rgbaCurrent) {
		    __glBuildRGBAModifyTables(gc, pm);
		}
		pm->redCurMap = pm->redModMap;
		pm->greenCurMap = pm->greenModMap;
		pm->blueCurMap = pm->blueModMap;
		pm->alphaCurMap = pm->alphaModMap;
	    }
	    if (spanModInfo->zoomx1) {
		spanInfo->spanRender = __glSpanRenderRGBAubyte2;
	    } else {
		spanInfo->spanRender = __glSpanRenderRGBAubyte;
	    }
	    return GL_TRUE;
	case GL_STENCIL_INDEX:
	    if (!spanModInfo->modify) {
		if (packedUserData) {
		    /* no span unpacking is necessary! */
		} else {
		    /* zoomx2 must not be true, or packedUserData 
		     * would be set 
		     */
		    assert(!spanModInfo->zoomx2);
		    spanInfo->spanModifier[spanInfo->numSpanMods++] =
		    			__glSpanUnpackIndexUbyte;
		}
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderStencilUbyte2;
		} else {
		    spanInfo->spanRender = __glSpanRenderStencilUbyte;
		}
		return GL_TRUE;
	    }
	    break;
	case GL_COLOR_INDEX:
	    if (packedUserData) {
		/* no span unpacking is necessary! */
	    } else {
		/* zoomx2 must not be true, or packedUserData would be set */
		assert(!spanModInfo->zoomx2);
		spanInfo->spanModifier[spanInfo->numSpanMods++] =
					__glSpanUnpackIndexUbyte;
	    }
	    if (!spanModInfo->modify) {
		pm->iCurMap = pm->iMap;
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderCIubyte2;
		} else {
		    spanInfo->spanRender = __glSpanRenderCIubyte;
		}
	    } else {
		if (gc->modes.rgbMode) {
		    if (!pm->iToRGBACurrent) {
			__glBuildItoRGBAModifyTables(gc, pm);
		    }
		    pm->redCurMap = pm->iToRMap;
		    pm->greenCurMap = pm->iToGMap;
		    pm->blueCurMap = pm->iToBMap;
		    pm->alphaCurMap = pm->iToAMap;
		    if (spanModInfo->zoomx1) {
			spanInfo->spanRender = __glSpanRenderCIubyte4;
		    } else {
			spanInfo->spanRender = __glSpanRenderCIubyte3;
		    }
		} else {
		    if (!pm->iToICurrent) {
			__glBuildItoIModifyTables(gc, pm);
		    }
		    pm->iCurMap = pm->iToIMap;
		    if (spanModInfo->zoomx1) {
			spanInfo->spanRender = __glSpanRenderCIubyte2;
		    } else {
			spanInfo->spanRender = __glSpanRenderCIubyte;
		    }
		}
	    }
	    return GL_TRUE;
	default:
	    break;
	}
	break;
    case GL_UNSIGNED_SHORT:
	switch(spanInfo->srcFormat) {
	case GL_STENCIL_INDEX:
	    if (!spanModInfo->modify) {
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderStencilUshort2;
		} else {
		    spanInfo->spanRender = __glSpanRenderStencilUshort;
		}
		return GL_TRUE;
	    }
	    break;
	case GL_COLOR_INDEX:
	    if (!spanModInfo->modify) {
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderCIushort2;
		} else {
		    spanInfo->spanRender = __glSpanRenderCIushort;
		}
		return GL_TRUE;
	    }
	    break;
	default:
	    break;
	}
	break;
    case GL_UNSIGNED_INT:
	switch(spanInfo->srcFormat) {
	case GL_DEPTH_COMPONENT:
	    if (!spanModInfo->modify) {
		GLuint writeMask = gc->depthBuffer.writeMask;
		/* XXX: accessing numFracBits without a lock */
		GLuint scale = gc->depthBuffer.scale 
		    >> __GL_DEPTH_DP(&gc->depthBuffer)->numFracBits;

		/*
		** we will be not converting to float if:
		**  - writeMask is of the form 2^n -1
		**  - scale will be of the form 2^n -1 after the
		**    shift imposed by numFracBits
		*/
		if( ( (writeMask & (writeMask+1)) == 0x00000000 ) &
		   ( (scale & (scale+1)) == 0x00000000 ) ) {

		    if (spanModInfo->zoomx1) {
			spanInfo->spanRender = __glSpanRenderDepthUint2;
		    } else {
			spanInfo->spanRender = __glSpanRenderDepthUint;
		    }
		    return GL_TRUE;
		}
	    }
	    break;
	case GL_COLOR_INDEX:
	    if (!spanModInfo->modify) {
		if (spanModInfo->zoomx1) {
		    spanInfo->spanRender = __glSpanRenderCIuint2;
		} else {
		    spanInfo->spanRender = __glSpanRenderCIuint;
		}
		return GL_TRUE;
	    }
	    break;
	default:
	    break;
	}
	break;
    default:
	break;
    }
    return GL_FALSE;
}

/*
** Functions to pick spanModifiers for each stage of span processing
*/

static void
PickSpanUnpack(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    __GLpixelMachine *pm = &gc->pixel;
    GLint spanCount = spanInfo->numSpanMods;

    /* 
    ** Reformat data if it is not in a convenient format.
    **
    ** Reformatting can consist of:
    **  - aligning the data
    **  - skipping pixels if |xzoom| is < 1
    **  - swapping bytes if necessary
    */
    if (spanModInfo->srcSwap) {
	if (spanModInfo->skipGroups) {
	    if (spanInfo->srcElementSize == 2) {
		spanInfo->spanModifier[spanCount++] = __glSpanSwapAndSkipBytes2;
	    } else /* spanInfo->srcElementSize == 4 */ {
		spanInfo->spanModifier[spanCount++] = __glSpanSwapAndSkipBytes4;
	    }
	} else {
	    if (spanInfo->srcElementSize == 2) {
		spanInfo->spanModifier[spanCount++] = __glSpanSwapBytes2;
	    } else /* spanInfo->srcElementSize == 4 */ {
		spanInfo->spanModifier[spanCount++] = __glSpanSwapBytes4;
	    }
	}
    } else if (spanModInfo->srcAlign) {
	if (spanModInfo->skipGroups) {
	    if (spanInfo->srcElementSize == 2) {
		spanInfo->spanModifier[spanCount++] = __glSpanSlowSkipPixels2;
	    } else /* spanInfo->srcElementSize == 4 */ {
		spanInfo->spanModifier[spanCount++] = __glSpanSlowSkipPixels4;
	    }
	} else {
	    if (spanInfo->srcElementSize == 2) {
		spanInfo->spanModifier[spanCount++] = __glSpanAlignPixels2;
	    } else /* spanInfo->srcElementSize == 4 */ {
		spanInfo->spanModifier[spanCount++] = __glSpanAlignPixels4;
	    }
	}
    } else if (spanModInfo->skipGroups) {
	if (spanInfo->srcElementSize == 1) {
	    spanInfo->spanModifier[spanCount++] = __glSpanSkipPixels1;
	} else if (spanInfo->srcElementSize == 2) {
	    spanInfo->spanModifier[spanCount++] = __glSpanSkipPixels2;
	} else /* spanInfo->srcElementSize == 4 */ {
	    spanInfo->spanModifier[spanCount++] = __glSpanSkipPixels4;
	}
    }

    /* 
    ** Conversion to float.
    **
    ** All formats are converted into floating point (including GL_BITMAP).
    */
    if (spanModInfo->srcConvert) {
	if (spanInfo->srcFormat == GL_COLOR_INDEX ||
			spanInfo->srcFormat == GL_STENCIL_INDEX) {
	    /* Index conversion */
	    switch(spanInfo->srcType) {
	      case GL_BITMAP:
		if (spanModInfo->zoomx2) {
		    spanInfo->spanModifier[spanCount++] = __glSpanUnpackBitmap2;
		} else {
		    spanInfo->spanModifier[spanCount++] = __glSpanUnpackBitmap;
		}
		break;
	      case GL_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackByteI;
		break;
	      case GL_UNSIGNED_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackUbyteI;
		break;
	      case GL_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackShortI;
		break;
	      case GL_UNSIGNED_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackUshortI;
		break;
	      case GL_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackIntI;
		break;
	      case GL_UNSIGNED_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackUintI;
		break;
	    }
	} else {
	    /* Component conversion */
	    switch(spanInfo->srcType) {
	      case GL_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackByte;
		break;
	      case GL_UNSIGNED_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackUbyte;
		break;
	      case GL_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackShort;
		break;
	      case GL_UNSIGNED_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackUshort;
		break;
	      case GL_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackInt;
		break;
	      case GL_UNSIGNED_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackUint;
		break;
	      case GL_UNSIGNED_BYTE_3_3_2:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack332Ubyte;
		/*
		** So why the heck are we setting srcComponents here??
		** 
		** The packed pixel routines use the correct
		** number of components implicitly.  Other routines
		** that come after the packed pixel routines need
		** to know the number of components that were produced
		** when the packed pixels were unpacked.  Setting
		** srcComponents to its expanded value only
		** works because srcComponents is not used by the
		** packed pixel routines, which assume the number
		** of components is always one.
		**
		** We can't set the value at an earlier point, because 
		** the number of components needs to be 1 for the 
		** computation of row sizes, etc.  
		**
		** The same principle applies to the packing of 
		** packed pixels.  
		*/
		spanInfo->srcComponents = 3;
		break;
	      case GL_UNSIGNED_BYTE_2_3_3_REV:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack233RevUbyte;
		spanInfo->srcComponents = 3;
		break;
	      case GL_UNSIGNED_SHORT_5_6_5:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack565Ushort;
		spanInfo->srcComponents = 3;
		break;
	      case GL_UNSIGNED_SHORT_5_6_5_REV:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack565RevUshort;
		spanInfo->srcComponents = 3;
		break;
	      case GL_UNSIGNED_SHORT_4_4_4_4:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack4444Ushort;
		spanInfo->srcComponents = 4;
		break;
	      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack4444RevUshort;
		spanInfo->srcComponents = 4;
		break;
	      case __GL_UNSIGNED_SHORT_4_4_4_4_ARGB:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackARGB4444Ushort;
		spanInfo->srcComponents = 4;
		break;
	      case GL_UNSIGNED_SHORT_5_5_5_1:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack5551Ushort;
		spanInfo->srcComponents = 4;
		break;
	      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack1555RevUshort;
		spanInfo->srcComponents = 4;
		break;
	      case __GL_UNSIGNED_SHORT_X_5_5_5:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpackX555Ushort;
		spanInfo->srcComponents = 3;
		break;
	      case __GL_UNSIGNED_SHORT_1_5_5_5:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack1555Ushort;
		spanInfo->srcComponents = 4;
		break;
	      case __GL_UNSIGNED_SHORT_5_6_5:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack565Ushort;
		spanInfo->srcComponents = 3;
		break;
	      case GL_UNSIGNED_INT_8_8_8_8:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack8888Uint;
		spanInfo->srcComponents = 4;
		break;
	      case GL_UNSIGNED_INT_8_8_8_8_REV:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack8888RevUint;
		spanInfo->srcComponents = 4;
		break;
	      case GL_UNSIGNED_INT_10_10_10_2:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack_10_10_10_2_Uint;
		spanInfo->srcComponents = 4;
		break;
	      case GL_UNSIGNED_INT_2_10_10_10_REV:
		spanInfo->spanModifier[spanCount++] = __glSpanUnpack_2_10_10_10_RevUint;
		spanInfo->srcComponents = 4;
		break;
	    }
	}
    } else if (spanInfo->nonColorComp) {
	/* only needed for hgram so far; only srcType is Uint */
	spanInfo->spanModifier[spanCount++] = __glSpanUnpackNonCompUint;
    }

    /*
    ** Clamp components here if it allows us to avoid clamping later.
    */
    if (spanModInfo->srcClamp) {
	spanInfo->spanModifier[spanCount++] = __glSpanClampFloat;
    }

    /* 
    ** Expansion to RGBA, Modification and color scaling
    **
    ** Spans are modified if necessary (color biasing, maps, shift,
    ** scale), and RGBA colors are scaled.  Also, all RGBA derivative
    ** formats (RED, LUMINANCE, ALPHA, etc.) are converted to RGBA.
    ** The only four span formats that survive this stage are:
    **
    ** (COLOR_INDEX, FLOAT),
    ** (STENCIL_INDEX, FLOAT),
    ** (DEPTH_COMPONENT, FLOAT),
    ** (RGBA, FLOAT),
    */
    if (spanModInfo->srcExpand) {
	switch(spanInfo->srcFormat) {
	  case GL_RED:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyRed;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandRed : __glSpanExpandRedNS;
	    }
	    break;
	  case GL_GREEN:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyGreen;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandGreen : __glSpanExpandGreenNS;
	    }
	    break;
	  case GL_BLUE:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyBlue;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandBlue : __glSpanExpandBlueNS;
	    }
	    break;
	  case GL_ALPHA:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyAlpha;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandAlpha : __glSpanExpandAlphaNS;
	    }
	    break;
	  case GL_RGB:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyRGB;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandRGB : __glSpanExpandRGBNS;
	    }
	    break;
	  case GL_BGR:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyBGR;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandBGR : __glSpanExpandBGRNS;
	    }
	    break;
	  case GL_LUMINANCE:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyLuminance;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandLuminance : __glSpanExpandLuminanceNS;
	    }
	    break;
	  case GL_LUMINANCE_ALPHA:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyLuminanceAlpha;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandLuminanceAlpha : 
		    __glSpanExpandLuminanceAlphaNS;
	    }
	    break;
	  case __GL_RED_ALPHA:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyRedAlpha;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandRedAlpha : __glSpanExpandRedAlphaNS;
	    }
	    break;
	  case GL_RGBA:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyRGBA;
	    } else {
		if (spanInfo->applyFbScale) {
		    spanInfo->spanModifier[spanCount++] = __glSpanScaleRGBA;
		}
	    }
	    break;
	  case GL_DEPTH_COMPONENT:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyDepth;
	    } 
	    break;
	  case GL_STENCIL_INDEX:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyStencil;
	    } 
	    break;
	  case GL_COLOR_INDEX:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyCI;
	    } 
	    break;
	  case GL_ABGR_EXT:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyABGR;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanScaleABGR : __glSpanPreReorderABGR;
	    }
	    break;
	  case GL_BGRA:
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanModifyBGRA;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanScaleBGRA : __glSpanPreReorderBGRA;
	    }
	    break;
	  case GL_INTENSITY:
	    /* 
	    ** Modify should never be on, because this is an internal
	    ** format, and modify only happens when loading from user
	    ** memory.
	    */
	    spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanExpandIntensity : __glSpanExpandIntensityNS;
	    break;
	}
    }

    /*
    ** Apply generic scale and bias
    */
    if (spanInfo->applyGenericScaleBias) {
        spanInfo->spanModifier[spanCount++] = __glSpanGenericScaleBiasRGBA;
    }
    spanInfo->numSpanMods = spanCount;
}

static void
PickSpanRead(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    __GLpixelMachine *pm = &gc->pixel;
    GLint spanCount = spanInfo->numSpanMods;

    /*
    ** Read and modify a span.  RGBA spans are scaled when
    ** this step is finished.
    */
    switch(spanInfo->dstFormat) {
      case GL_RGB:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_ALPHA:
      case GL_RGBA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
      case GL_INTENSITY:
      case __GL_RED_ALPHA:
	if (gc->modes.rgbMode) {
	    if (spanModInfo->zoomx2) {
		spanInfo->spanReader = gc->procs.pixel.spanReadRGBA2;
	    } else {
		spanInfo->spanReader = gc->procs.pixel.spanReadRGBA;
	    }
	    if (spanModInfo->modify) {
		spanInfo->spanModifier[spanCount++] = __glSpanPreUnscaleRGBA;
		spanInfo->spanModifier[spanCount++] = __glSpanModifyRGBA;
	    }
	} else {
	    if (spanModInfo->zoomx2) {
		spanInfo->spanReader = gc->procs.pixel.spanReadCI2;
	    } else {
		spanInfo->spanReader = gc->procs.pixel.spanReadCI;
	    }
	    spanInfo->spanModifier[spanCount++] = __glSpanModifyCI;
	}
	break;
      case GL_DEPTH_COMPONENT:
	if (spanModInfo->zoomx2) {
	    spanInfo->spanReader = gc->procs.pixel.spanReadDepth2;
	} else {
	    spanInfo->spanReader = gc->procs.pixel.spanReadDepth;
	}
	if (spanModInfo->modify) {
	    spanInfo->spanModifier[spanCount++] = __glSpanModifyDepth;
	}
	break;
      case GL_STENCIL_INDEX:
	if (spanModInfo->zoomx2) {
	    spanInfo->spanReader = gc->procs.pixel.spanReadStencil2;
	} else {
	    spanInfo->spanReader = gc->procs.pixel.spanReadStencil;
	}
	if (spanModInfo->modify) {
	    spanInfo->spanModifier[spanCount++] = __glSpanModifyStencil;
	}
	break;
      case GL_COLOR_INDEX:
	if (spanModInfo->zoomx2) {
	    spanInfo->spanReader = gc->procs.pixel.spanReadCI2;
	} else {
	    spanInfo->spanReader = gc->procs.pixel.spanReadCI;
	}
	if (spanModInfo->modify) {
	    spanInfo->spanModifier[spanCount++] = __glSpanModifyCI;
	} 
	break;
      default:
	assert(0);
    }

    /*
    ** Apply generic scale and bias
    */
    if (spanInfo->applyGenericScaleBias) {
        spanInfo->spanModifier[spanCount++] = __glSpanGenericScaleBiasRGBA;
    }
    spanInfo->numSpanMods = spanCount;
}

static void
PickSpanColorTable(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    GLint spanCount = spanInfo->numSpanMods;
    GLuint pixelModeFlags = gc->pixel.pixelModeFlags;

    if (pixelModeFlags & __GL_PIXEL_MODE_COLOR_TABLE) {
	spanInfo->spanModifier[spanCount++] = __glSpanColorTableRGBA;
    }
    spanInfo->numSpanMods = spanCount;
}

static void
PickSpanConvolution(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    GLint spanCount = spanInfo->numSpanMods;
    GLuint pixelModeFlags = gc->pixel.pixelModeFlags;
    __GLconvolutionFilter *filter;

    /*
    ** Convolution
    */
    switch (spanInfo->dim) {
    case 1:
	filter = gc->pixel.currentFilter1D;
	break;
    case 2: /* FALLTHROUGH */
    case 3:
	filter = gc->pixel.currentFilter2D;
	break;
    default:
    	assert(0);	/* invalid dimension */
	return;
    }
    spanInfo->filter = filter;

    if (filter) {
	/*
	** Pick convolution row operator 
	*/
	switch (filter->baseFormat) {
	case GL_LUMINANCE:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReduceL;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReduceL;
		}
	        break;
	    case GL_CONSTANT_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableConstantL;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsConstantL;
		}
	        break;
	    case GL_REPLICATE_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReplicateL;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReplicateL;
		}
	        break;
	    default:
	        assert(0);	/* invalid border mode */
		return;
	    }
	    break;
	case GL_LUMINANCE_ALPHA:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReduceLA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReduceLA;
		}
	        break;
	    case GL_CONSTANT_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableConstantLA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsConstantLA;
		}
	        break;
	    case GL_REPLICATE_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReplicateLA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReplicateLA;
		}
	        break;
	    default:
	        assert(0);	/* invalid border mode */
		return;
	    }
	    break;
	case GL_ALPHA:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReduceA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReduceA;
		}
	        break;
	    case GL_CONSTANT_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableConstantA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsConstantA;
		}
	        break;
	    case GL_REPLICATE_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReplicateA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReplicateA;
		}
	        break;
	    default:
	        assert(0);	/* invalid border mode */
		return;
	    }
	    break;
	case GL_INTENSITY:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReduceI;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReduceI;
		}
	        break;
	    case GL_CONSTANT_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableConstantI;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsConstantI;
		}
	        break;
	    case GL_REPLICATE_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReplicateI;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReplicateI;
		}
	        break;
	    default:
	        assert(0);	/* invalid border mode */
		return;
	    }
	    break;
	case GL_RGB:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReduceRGB;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReduceRGB;
		}
	        break;
	    case GL_CONSTANT_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableConstantRGB;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsConstantRGB;
		}
	        break;
	    case GL_REPLICATE_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReplicateRGB;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReplicateRGB;
		}
	        break;
	    default:
	        assert(0);	/* invalid border mode */
		return;
	    }
	    break;
	case GL_RGBA:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReduceRGBA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReduceRGBA;
		}
	        break;
	    case GL_CONSTANT_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableConstantRGBA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsConstantRGBA;
		}
	        break;
	    case GL_REPLICATE_BORDER:
	        if (filter->target == GL_SEPARABLE_2D) {
		    spanInfo->convolveRows =
		    			__glConvolveRowsSeparableReplicateRGBA;
		} else {
		    spanInfo->convolveRows =
		    			__glConvolveRowsReplicateRGBA;
		}
	        break;
	    default:
	        assert(0);	/* invalid border mode */
		return;
	    }
	    break;
	default:
	    assert(0);		/* invalid filter internal format */
	    return;
	}

	/*
	** Pick convolution span operator
	*/
	spanInfo->convModifierIndex = spanCount;
	switch (spanInfo->dim) {
	case 1:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
		spanInfo->spanModifier[spanCount++] =
					__glSpanConvolution1DReduceRGBA;
		spanInfo->convInitialSpans =
		spanInfo->convFinalSpans = 0;
		break;
	    case GL_CONSTANT_BORDER:
	    case GL_REPLICATE_BORDER:
		spanInfo->spanModifier[spanCount++] =
					__glSpanConvolution1DBorderRGBA;
		spanInfo->convInitialSpans =
		spanInfo->convFinalSpans = 0;
		break;
	    }
	    break;
	case 2: /* FALLTHROUGH */
	case 3:
	    switch (filter->state.borderMode) {
	    case GL_REDUCE:
		spanInfo->spanModifier[spanCount++] =
					__glSpanConvolution2DReduceRGBA;
		spanInfo->convInitialSpans = filter->height-1;
		spanInfo->convFinalSpans = 0;
		break;
	    case GL_CONSTANT_BORDER:
	    case GL_REPLICATE_BORDER:
		spanInfo->spanModifier[spanCount++] =
					__glSpanConvolution2DBorderRGBA;
		spanInfo->convInitialSpans =
		spanInfo->convFinalSpans =
				    (filter->height-1) - filter->height/2;
		break;
	    }
	    break;
	}

	/*
	** Post Convolution Scale and Bias
	*/
	if (pixelModeFlags & __GL_PIXEL_MODE_POST_CONVOLUTION_SCALE_BIAS) {
	    spanInfo->spanModifier[spanCount++] = __glSpanPostConvScaleBiasRGBA;
	}
    }

    /*
    ** Post Convolution Color Table
    */
    if (pixelModeFlags & __GL_PIXEL_MODE_POST_CONVOLUTION_COLOR_TABLE) {
	spanInfo->spanModifier[spanCount++] =
		    __glSpanPostConvolutionColorTableRGBA;
    }
    spanInfo->numSpanMods = spanCount;
}

static void
PickSpanColorMatrix(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    GLint spanCount = spanInfo->numSpanMods;
    GLuint pixelModeFlags = gc->pixel.pixelModeFlags;

    if (pixelModeFlags & __GL_PIXEL_MODE_COLOR_MATRIX) {
	if (pixelModeFlags & __GL_PIXEL_MODE_POST_COLOR_MATRIX_SCALE_BIAS) {
	    spanInfo->spanModifier[spanCount++] =
	    			__glSpanColorMatrixGeneralRGBA;
	} else {
	    spanInfo->spanModifier[spanCount++] =
	    			__glSpanColorMatrixNoScaleBiasRGBA;
	}
    } else if (pixelModeFlags & __GL_PIXEL_MODE_POST_COLOR_MATRIX_SCALE_BIAS) {
	spanInfo->spanModifier[spanCount++] =
				__glSpanColorMatrixOnlyScaleBiasRGBA;
    }

    /*
    ** Post Color Matrix Color Table
    */
    if (pixelModeFlags & __GL_PIXEL_MODE_POST_COLOR_MATRIX_COLOR_TABLE) {
	spanInfo->spanModifier[spanCount++] =
		    __glSpanPostColorMatrixColorTableRGBA;
    }
    spanInfo->numSpanMods = spanCount;
}

static GLboolean
PickSpanHistogramMinmax(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    GLint spanCount = spanInfo->numSpanMods;
    GLuint pixelModeFlags = gc->pixel.pixelModeFlags;
    GLboolean sink = GL_FALSE;

    if (pixelModeFlags & __GL_PIXEL_HISTOGRAM) {
	if (gc->pixel.histogram.sink) {
	    sink = GL_TRUE;
	    switch (gc->pixel.histogram.baseFormat) {
	    case GL_ALPHA:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanHistogramSinkA;
		break;
	    case GL_LUMINANCE:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanHistogramSinkL;
		break;
	    case GL_LUMINANCE_ALPHA:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanHistogramSinkLA;
		break;
	    case GL_RGB:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanHistogramSinkRGB;
		break;
	    case GL_RGBA:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanHistogramSinkRGBA;
		break;
	    default:
		assert(0);
	    }
	} else {
	    switch (gc->pixel.histogram.baseFormat) {
	    case GL_ALPHA:
		spanInfo->spanModifier[spanCount++] = __glSpanHistogramA;
		break;
	    case GL_LUMINANCE:
		spanInfo->spanModifier[spanCount++] = __glSpanHistogramL;
		break;
	    case GL_LUMINANCE_ALPHA:
		spanInfo->spanModifier[spanCount++] = __glSpanHistogramLA;
		break;
	    case GL_RGB:
		spanInfo->spanModifier[spanCount++] = __glSpanHistogramRGB;
		break;
	    case GL_RGBA:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanHistogramRGBA;
		break;
	    default:
		assert(0);
	    }
	}
    }

    /*
    ** Find minimum and maximum component values.
    ** Only do this if minmax has been enabled, and the array
    ** has been intialized.
    */
    if (!sink && (pixelModeFlags & __GL_PIXEL_MINMAX)) {
	if (gc->pixel.minmax.sink) {
	    sink = GL_TRUE;
	    switch (gc->pixel.minmax.baseFormat) {
	    case GL_ALPHA:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxSinkA;
		break;
	    case GL_LUMINANCE:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxSinkL;
		break;
	    case GL_LUMINANCE_ALPHA:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxSinkLA;
		break;
	    case GL_RGB:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanMinmaxSinkRGB;
		break;
	    case GL_RGBA:
		spanInfo->spanModifier[spanCount++] = 
		    __glSpanMinmaxSinkRGBA;
		break;
	    default:
		assert(0);
	    }
	} else {
	    switch (gc->pixel.minmax.baseFormat) {
	    case GL_ALPHA:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxA;
		break;
	    case GL_LUMINANCE:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxL;
		break;
	    case GL_LUMINANCE_ALPHA:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxLA;
		break;
	    case GL_RGB:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxRGB;
		break;
	    case GL_RGBA:
		spanInfo->spanModifier[spanCount++] = __glSpanMinmaxRGBA;
		break;
	    default:
		assert(0);
	    }
	}
    }
    spanInfo->numSpanMods = spanCount;
    return sink;
}

static void
PickSpanPack(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    GLint spanCount = spanInfo->numSpanMods;

    /*
    ** Reduce RGBA spans to appropriate derivative (RED, 
    ** LUMINANCE, ALPHA, etc.).
    */
    if (spanModInfo->dstReduce) {
	switch(spanInfo->dstFormat) {
	  case GL_RGB:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceRGB : __glSpanReduceRGBNS;
	    break;
	  case GL_BGR:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceBGR : __glSpanReduceBGRNS;
	    break;
	  case GL_RED:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceRed : __glSpanReduceRedNS;
	    break;
	  case GL_GREEN:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceGreen : __glSpanReduceGreenNS;
	    break;
	  case GL_BLUE:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceBlue : __glSpanReduceBlueNS;
	    break;
	  case GL_LUMINANCE:
	    if (spanModInfo->pixelPath == __GL_PIXPATH_READPIX) {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanReduceLuminance : 
		    __glSpanReduceRedNS;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanReduceRed : __glSpanReduceRedNS;
	    }
	    break;
	  case GL_LUMINANCE_ALPHA:
	    if (spanModInfo->pixelPath == __GL_PIXPATH_READPIX) {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanReduceLuminanceAlpha : 
		    __glSpanReduceRedAlphaNS;
	    } else {
		spanInfo->spanModifier[spanCount++] = 
		    spanInfo->applyFbScale ? 
		    __glSpanReduceRedAlpha : 
		    __glSpanReduceRedAlphaNS;
	    }
	    break;
	  case __GL_RED_ALPHA:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceRedAlpha : __glSpanReduceRedAlphaNS;
	    break;
	  case GL_ALPHA:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceAlpha : __glSpanReduceAlphaNS;
	    break;
	  case GL_RGBA:
	    if (spanInfo->applyFbScale) {
		spanInfo->spanModifier[spanCount++] = 
					__glSpanPostUnscaleRGBA;
	    }
	    break;
	  case GL_ABGR_EXT:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReorderABGR : __glSpanReorderABGRNS;
	    break;
	  case GL_BGRA:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReorderBGRA : __glSpanReorderBGRANS;
	    break;
	  case GL_INTENSITY:
	    spanInfo->spanModifier[spanCount++] = 
		spanInfo->applyFbScale ? 
		__glSpanReduceRed : __glSpanReduceRedNS;
	    break;
	}
    }

    /*
    ** Clamp if necessary.
    */
    if (spanModInfo->dstClamp) {
	switch(spanInfo->srcType) {
	  case GL_BYTE:
	  case GL_SHORT:
	  case GL_INT:
	    spanInfo->spanModifier[spanCount++] = __glSpanClampSignedDst;
	    break;
	  case GL_FLOAT:
	    spanInfo->spanModifier[spanCount++] = __glSpanClampFloatDst;
	    break;
	}
    }

    /*
    ** Conversion from FLOAT to user requested type.
    */
    if (spanModInfo->dstConvert) {
	if (spanInfo->dstFormat == GL_COLOR_INDEX ||
			spanInfo->dstFormat == GL_STENCIL_INDEX) {
	    switch(spanInfo->dstType) {
	      case GL_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanPackByteI;
		break;
	      case GL_UNSIGNED_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanPackUbyteI;
		break;
	      case GL_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackShortI;
		break;
	      case GL_UNSIGNED_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackUshortI;
		break;
	      case GL_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackIntI;
		break;
	      case GL_UNSIGNED_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackUintI;
		break;
	      case GL_BITMAP:
		spanInfo->spanModifier[spanCount++] = __glSpanPackBitmap;
		break;
	    }
	} else {
	    switch(spanInfo->dstType) {
	      case GL_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanPackByte;
		break;
	      case GL_UNSIGNED_BYTE:
		spanInfo->spanModifier[spanCount++] = __glSpanPackUbyte;
		break;
	      case GL_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackShort;
		break;
	      case GL_UNSIGNED_SHORT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackUshort;
		break;
	      case GL_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackInt;
		break;
	      case GL_UNSIGNED_INT:
		spanInfo->spanModifier[spanCount++] = __glSpanPackUint;
		break;
	      case GL_UNSIGNED_BYTE_3_3_2:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack332Ubyte;
		spanInfo->dstComponents = 3;
		break;
	      case GL_UNSIGNED_BYTE_2_3_3_REV:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack233RevUbyte;
		spanInfo->dstComponents = 3;
		break;
	      case GL_UNSIGNED_SHORT_5_6_5:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack565Ushort;
		spanInfo->dstComponents = 3;
		break;
	      case GL_UNSIGNED_SHORT_5_6_5_REV:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack565RevUshort;
		spanInfo->srcComponents = 3;
		break;
	      case GL_UNSIGNED_SHORT_4_4_4_4:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack4444Ushort;
		spanInfo->dstComponents = 4;
		break;
	      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack4444RevUshort;
		spanInfo->dstComponents = 4;
		break;
	      case __GL_UNSIGNED_SHORT_4_4_4_4_ARGB:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPackARGB4444Ushort;
		spanInfo->dstComponents = 4;
		break;
	      case GL_UNSIGNED_SHORT_5_5_5_1:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack5551Ushort;
		spanInfo->dstComponents = 4;
		break;
	      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack1555RevUshort;
		spanInfo->dstComponents = 4;
		break;
	      case __GL_UNSIGNED_SHORT_X_5_5_5:
		spanInfo->spanModifier[spanCount++] =
					    __glSpanPackX555Ushort;
		spanInfo->dstComponents = 3;
		break;
	      case __GL_UNSIGNED_SHORT_1_5_5_5:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack1555Ushort;
		spanInfo->dstComponents = 4;
		break;
	      case __GL_UNSIGNED_SHORT_5_6_5:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack565Ushort;
		spanInfo->dstComponents = 3;
		break;
	      case GL_UNSIGNED_INT_8_8_8_8:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack8888Uint;
		spanInfo->dstComponents = 4;
		break;
	      case GL_UNSIGNED_INT_8_8_8_8_REV:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack8888RevUint;
		spanInfo->dstComponents = 4;
		break;
	      case GL_UNSIGNED_INT_10_10_10_2:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack_10_10_10_2_Uint;
		spanInfo->dstComponents = 4;
		break;
	      case GL_UNSIGNED_INT_2_10_10_10_REV:
		spanInfo->spanModifier[spanCount++] = 
					    __glSpanPack_2_10_10_10_RevUint;
		spanInfo->dstComponents = 4;
		break;
	      default:
		assert(0);
	    }
	}
    } else if (spanInfo->nonColorComp) {
	/* only needed for hgram so far; these are the hgram dest types */
	switch(spanInfo->dstType) {
	case GL_BYTE:
	    spanInfo->spanModifier[spanCount++] = __glSpanPackNonCompByte;
	    break;
	case GL_UNSIGNED_BYTE:
	    spanInfo->spanModifier[spanCount++] = __glSpanPackNonCompUbyte;
	    break;
	case GL_SHORT:
	    spanInfo->spanModifier[spanCount++] = __glSpanPackNonCompShort;
	    break;
	case GL_UNSIGNED_SHORT:
	    spanInfo->spanModifier[spanCount++] = __glSpanPackNonCompUshort;
	    break;
	case GL_INT:
	    spanInfo->spanModifier[spanCount++] = __glSpanPackNonCompInt;
	    break;
	case GL_UNSIGNED_INT:
	    spanInfo->spanModifier[spanCount++] = __glSpanPackNonCompUint;
	    break;
	}
    }

    /*
    ** Mis-align data as needed, and perform byte swapping
    ** if requested by the user.
    */
    if (spanModInfo->dstSwap) {
	if (spanInfo->dstElementSize == 2) {
	    spanInfo->spanModifier[spanCount++] = __glSpanSwapBytes2Dst;
	} else if (spanInfo->dstElementSize == 4) {
	    spanInfo->spanModifier[spanCount++] = __glSpanSwapBytes4Dst;
	}
    } else if (spanModInfo->dstAlign) {
	if (spanInfo->dstElementSize == 2) {
	    spanInfo->spanModifier[spanCount++] = __glSpanAlignPixels2Dst;
	} else if (spanInfo->dstElementSize == 4) {
	    spanInfo->spanModifier[spanCount++] = __glSpanAlignPixels4Dst;
	}
    }
    spanInfo->numSpanMods = spanCount;
}

static void
PickSpanRender(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, __GLpixelSpanModInfo *spanModInfo)
{
    __GLpixelMachine *pm = &gc->pixel;
    GLint spanCount = spanInfo->numSpanMods;

    /*
    **  Rendering
    **
    ** The spans are rendered.  If |xzoom| > 1, then the span renderer
    ** is responsible for pixel replication.
    */
    switch(spanInfo->dstFormat) {
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
      case GL_INTENSITY:
	if (spanModInfo->zoomx1) {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderRGBA2;
	} else {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderRGBA;
	}
	break;
      case GL_DEPTH_COMPONENT:
	if (spanModInfo->zoomx1) {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderDepth2;
	} else {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderDepth;
	}
	break;
      case GL_COLOR_INDEX:
	if (spanModInfo->zoomx1) {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderCI2;
	} else {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderCI;
	}
	break;
      case GL_STENCIL_INDEX:
	if (spanModInfo->zoomx1) {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderStencil2;
	} else {
	    spanInfo->spanRender = gc->procs.pixel.spanRenderStencil;
	}
	break;
      default:
	assert(0);
    }
    spanInfo->numSpanMods = spanCount;
}

static void NopSpanRender(__GLcontext *gc,
		__GLpixelSpanInfo *spanInfo, const GLvoid *inspan)
{
}

static
void PickSpanModifiers(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    __GLpixelSpanModInfo *spanModInfo)
{
    __GLpixelMachine *pm = &gc->pixel;
    GLuint pixelModeFlags = gc->pixel.pixelModeFlags;

    /*
    ** Examine pixel zoom factors if the framebuffer is the destination
    */
    if (spanModInfo->dstData == __GL_PIXDATA_FB) {
	__GLfloat zoomx = gc->state.pixel.transferMode.zoomX;

	if (zoomx >= (__GLfloat) -1.0 && zoomx <= __glOne) {
	    spanModInfo->zoomx1 = GL_TRUE;
	} else {
	    spanModInfo->zoomx1 = GL_FALSE;
	}
	if (zoomx <= (__GLfloat) -1.0 || zoomx >= __glOne) {
	    spanModInfo->zoomx2 = GL_TRUE;
	} else {
	    spanModInfo->zoomx2 = GL_FALSE;
	}
	if (spanModInfo->zoomx2 || spanInfo->srcType == GL_BITMAP) {
	    spanModInfo->skipGroups = GL_FALSE;
	} else {
	    spanModInfo->skipGroups = GL_TRUE;
	}
    } else {
	spanModInfo->zoomx1 = GL_TRUE;
	spanModInfo->zoomx2 = GL_TRUE;
	spanModInfo->skipGroups = GL_FALSE;
    }

    /*
    ** Byte swapping
    */
    if (spanInfo->srcSwapBytes && spanInfo->srcElementSize > 1) {
	spanModInfo->srcSwap = GL_TRUE;
    } else {
	spanModInfo->srcSwap = GL_FALSE;
    }
    if (spanInfo->dstSwapBytes && spanInfo->dstElementSize > 1) {
	spanModInfo->dstSwap = GL_TRUE;
    } else {
	spanModInfo->dstSwap = GL_FALSE;
    }

    /*
    ** Handling mis-aligned data access
    */
    if (spanInfo->srcType != GL_BITMAP &&
	(((unsigned long) (spanInfo->srcImage)) & /* XXXportability */
	    (spanInfo->srcElementSize - 1))) {
	spanModInfo->srcAlign = GL_TRUE;
    } else {
	spanModInfo->srcAlign = GL_FALSE;
    }
    if (spanInfo->dstType != GL_BITMAP &&
	(((unsigned long) (spanInfo->dstImage)) & /* XXXportability */
	    (spanInfo->dstElementSize - 1))) {
	spanModInfo->dstAlign = GL_TRUE;
    } else {
	spanModInfo->dstAlign = GL_FALSE;
    }

    /*
    ** Conversion to/from floating point
    */
    if (spanInfo->srcType == GL_FLOAT ||  spanInfo->nonColorComp) {
	spanModInfo->srcConvert = GL_FALSE;
    } else {
	spanModInfo->srcConvert = GL_TRUE;
    }
    if (spanInfo->dstType == GL_FLOAT || spanInfo->nonColorComp) {
	spanModInfo->dstConvert = GL_FALSE;
    } else {
	spanModInfo->dstConvert = GL_TRUE;
    }

    /*
    ** Extract pixel transfer enables from pixelModeFlags if we're
    ** applying pixel transfer operations.
    */
    if (spanInfo->applyPixelTransfer) {
	switch(spanInfo->srcFormat) {
	  case GL_RED:
	  case GL_GREEN:
	  case GL_BLUE:
	  case GL_ALPHA:
	  case GL_RGB:
	  case GL_LUMINANCE:
	  case GL_LUMINANCE_ALPHA:
	  case __GL_RED_ALPHA:
	  case GL_RGBA:
	  case GL_ABGR_EXT:
	  case GL_BGR:
	  case GL_BGRA:
	    spanModInfo->modify = 0 != (pixelModeFlags &
	    	(__GL_PIXEL_MODE_RGBA_LOOKUP |
		 __GL_PIXEL_MODE_RGBA_SCALE_BIAS));
	    spanModInfo->modify2 = 0 != (pixelModeFlags &
	    	(__GL_PIXEL_MODE_COLOR_TABLE |
		 __GL_PIXEL_MODE_CONVOLUTION |
		 __GL_PIXEL_MODE_POST_CONVOLUTION_SCALE_BIAS |
		 __GL_PIXEL_MODE_POST_CONVOLUTION_COLOR_TABLE |
		 __GL_PIXEL_MODE_COLOR_MATRIX |
		 __GL_PIXEL_MODE_POST_COLOR_MATRIX_SCALE_BIAS |
		 __GL_PIXEL_MODE_POST_COLOR_MATRIX_COLOR_TABLE |
		 __GL_PIXEL_HISTOGRAM |
		 __GL_PIXEL_MINMAX));
	    break;
	  case GL_DEPTH_COMPONENT:
	    spanModInfo->modify = pm->modifyDepth;
	    spanModInfo->modify2 = 0;
	    break;
	  case GL_STENCIL_INDEX:
	    spanModInfo->modify = pm->modifyStencil;
	    spanModInfo->modify2 = 0;
	    break;
	  case GL_COLOR_INDEX:
	    spanModInfo->modify = pm->modifyCI;
	    spanModInfo->modify2 = 0;
	    break;
	}
    } else {
        spanModInfo->modify = GL_FALSE;
        spanModInfo->modify2 = GL_FALSE;
    }

    /*
    ** Expansion to RGBA
    */
    if ((spanModInfo->pixelPath != __GL_PIXPATH_COPYIMAGE) ||
        spanModInfo->modify || spanModInfo->modify2 ||
	(spanInfo->srcFormat != spanInfo->dstFormat) || 
	(spanInfo->srcFormat != GL_LUMINANCE_ALPHA &&
		spanInfo->dstFormat != __GL_RED_ALPHA) ||
	(spanInfo->srcFormat != __GL_RED_ALPHA &&
		spanInfo->dstFormat != GL_LUMINANCE_ALPHA) ||
	(spanInfo->srcFormat != GL_LUMINANCE &&
		spanInfo->dstFormat != GL_RED) ||
	(spanInfo->srcFormat != GL_RED &&
		spanInfo->dstFormat != GL_LUMINANCE)) {
	spanModInfo->srcExpand = GL_TRUE;
	spanModInfo->dstReduce = GL_TRUE;
    } else {
	spanModInfo->srcExpand = GL_FALSE;
	spanModInfo->dstReduce = GL_FALSE;
    }

    /*
    ** Clamp before applying pixel transfer operations (if any)
    **
    ** Clamp types only if not float or not modifying (because converting
    ** float types means clamping, and that is only done if not modifying),
    ** and only if they might need clamping (UNSIGNED types never do).
    */
    if ((!spanInfo->applySrcClamp) ||
        (spanModInfo->srcData == __GL_PIXDATA_FB) ||
	(spanInfo->srcType == GL_BITMAP) ||
	(spanInfo->srcType == GL_UNSIGNED_BYTE) || 
	(spanInfo->srcType == GL_UNSIGNED_SHORT) ||
	(spanInfo->srcType == GL_UNSIGNED_INT) ||
	(spanInfo->srcFormat == GL_COLOR_INDEX) ||
	(spanInfo->srcFormat == GL_STENCIL_INDEX) ||
	(spanInfo->srcFormat == GL_DEPTH_COMPONENT && pm->modifyDepth) ||
	(spanInfo->srcFormat != GL_DEPTH_COMPONENT && pm->modifyRGBA)) {
	spanModInfo->srcClamp = GL_FALSE;
    } else {
	spanModInfo->srcClamp = GL_TRUE;
    }

    /*
    ** Clamp after applying pixel transfer operations
    */
    if ((!spanInfo->applyDstClamp) ||
        (spanInfo->dstFormat == GL_COLOR_INDEX) ||
        (spanInfo->dstFormat == GL_STENCIL_INDEX) ||
	(spanModInfo->modify)) {
	spanModInfo->dstClamp = GL_FALSE;
    } else {
	spanModInfo->dstClamp = GL_TRUE;
    }

    spanInfo->filter = NULL;

    /*
    ** Some transfers are handled as special cases.
    */
    switch (spanModInfo->pixelPath) {
      case __GL_PIXPATH_DRAWPIX:
        if (PickFastDrawPixelsModifiers(gc, spanInfo, spanModInfo)) {
	    return;
	}
        break;
      case __GL_PIXPATH_READPIX:
        if (PickFastReadPixelsModifiers(gc, spanInfo, spanModInfo)) {
	    return;
	}
        break;
      case __GL_PIXPATH_COPYIMAGE:
        if (PickFastCopyImageModifiers(gc, spanInfo, spanModInfo)) {
	    return;
	}
        break;
    }

    /*
    ** Pick span procs for Unpack/Read and core transfer operations.
    */
    switch(spanModInfo->srcData) {
      case __GL_PIXDATA_MEM:
	PickSpanUnpack(gc, spanInfo, spanModInfo);
	break;
      case __GL_PIXDATA_FB:
	PickSpanRead(gc, spanInfo, spanModInfo);
	break;
    }

    /*
    ** Pick span procs for ARB_imaging operations.
    */
    if (spanModInfo->modify2) {
	PickSpanColorTable(gc, spanInfo, spanModInfo);
	PickSpanConvolution(gc, spanInfo, spanModInfo);
	PickSpanColorMatrix(gc, spanInfo, spanModInfo);
	if (PickSpanHistogramMinmax(gc, spanInfo, spanModInfo)) {
	    /*
	    ** Either histogram or minmax are specified to be a `sink'
	    ** for pixel operations.  So don't pick any more spanModifiers
	    ** and make sure that we don't render anything.
	    */
	    spanInfo->spanRender = NopSpanRender;
	    return;
	}
    }

    /*
    ** Pick span procs for Pack/Render.
    */
    switch(spanModInfo->dstData) {
      case __GL_PIXDATA_MEM:
	PickSpanPack(gc, spanInfo, spanModInfo);
	break;
      case __GL_PIXDATA_FB:
	PickSpanRender(gc, spanInfo, spanModInfo);
	break;
    }

    /*
    ** Sanity check:  If we have zero span routines, then CopyImage
    ** isn't going to work.  We need to at least copy the data.
    */
    if ((spanModInfo->pixelPath == __GL_PIXPATH_COPYIMAGE) &&
    		spanInfo->numSpanMods == 0) {
	spanInfo->spanModifier[spanInfo->numSpanMods++] = __glSpanCopy;
    }
}

/*
** This routine clips a draw pixels box, and sets up a bunch of 
** variables required for drawing the box.  These are some of them:
**
** startCol   - The first column that will be drawn.
** x          - Effective raster position.  This will be set up so that 
**		every time zoomx is added, a change in the integer portion
**		of x indicates that a pixel should rendered (unpacked).
** columns    - The total number of columns that will be rendered.
**
** Others are startRow, y, rows.
**
** Yet other variables may be modified, such as width, height, skipPixels,
** skipLines.
**
** The clipping routine is written very carefully so that a fragment will
** be rasterized by a pixel if it's center falls within the range
** [x, x+zoomx) x [y, y+zoomy).
*/
GLboolean __glClipDrawPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint skipPixels;
    GLint skipRows;
    GLint width, height;
    GLint tempint;
    GLint endCol, endRow;
    __GLfloat x,y,x2,y2;
    __GLfloat zoomx, zoomy;
    __GLfloat clipLeft, clipRight, clipBottom, clipTop;

    zoomx = spanInfo->zoomx;
    zoomy = spanInfo->zoomy;
    if (zoomx == __glZero || zoomy == __glZero) {
	return GL_FALSE;
    }

    skipPixels = skipRows = 0;
    width = spanInfo->width;
    height = spanInfo->height;
    clipLeft = gc->transform.clipX0 + __glHalf;
    clipBottom = gc->transform.clipY0 + __glHalf;
    clipRight = gc->transform.clipX1 - gc->constants.viewportAlmostHalf;
    clipTop = gc->transform.clipY1 - gc->constants.viewportAlmostHalf;

#if defined(__GL_HALF_PIXEL_OFFSET)
    clipLeft -= __glHalf;
    clipBottom -= __glHalf;
    clipRight -= __glHalf;
    clipTop -= __glHalf;
#endif

    /* first corner of destination rectangle */
    x = spanInfo->x;
    y = spanInfo->y;

    /* opposite corner of destination rectangle */
    x2 = x + zoomx * (width + gc->pixel.adjustWidth2D);
    y2 = y + zoomy * (height + gc->pixel.adjustHeight2D);

    if (zoomx > 0) {
	/* Zoomx is positive, clip the left edge */
	if (x > clipLeft) {
	    /* Clip to the first fragment that will be produced */
	    clipLeft = __GL_FLOORF(x + gc->constants.viewportAlmostHalf);
	    clipLeft += __glHalf;
	}
	skipPixels = (GLint) ((clipLeft-x) / zoomx);
	if (skipPixels >= width) return GL_FALSE;

	width -= skipPixels;
	spanInfo->startCol = (GLint) clipLeft;
	x = x + skipPixels * zoomx;
	spanInfo->x = x + gc->constants.viewportAlmostHalf;
	spanInfo->srcSkipPixels += skipPixels;

	/* Zoomx is positive, clip the right edge */
	if (x2 < clipRight) {
	    /* Clip to the last fragment that will be produced */
	    clipRight = __GL_FLOORF(x2 + gc->constants.viewportAlmostHalf);
	    clipRight -= gc->constants.viewportAlmostHalf;
	}
	tempint = (GLint) ((x2-clipRight) / zoomx);
	if (tempint >= width) return GL_FALSE;

	width -= tempint;
	endCol = (GLint) clipRight + 1;
	spanInfo->endCol = endCol;
	spanInfo->columns = endCol - spanInfo->startCol;
    } else /* zoomx < 0 */ {
	/* Zoomx is negative, clip the right edge */
	if (x < clipRight) {
	    /* Clip to the first fragment that will be produced */
	    clipRight = __GL_FLOORF(x + gc->constants.viewportAlmostHalf);
	    clipRight -= gc->constants.viewportAlmostHalf;
	}
	skipPixels = (GLint) ((clipRight-x) / zoomx);
	if (skipPixels >= width) return GL_FALSE;

	width -= skipPixels;
	spanInfo->startCol = (GLint) clipRight;
	x = x + skipPixels * zoomx;
	spanInfo->x = x + gc->constants.viewportAlmostHalf - __glOne;
	spanInfo->srcSkipPixels += skipPixels;

	/* Zoomx is negative, clip the left edge */
	if (x2 > clipLeft) {
	    clipLeft = __GL_FLOORF(x2 + gc->constants.viewportAlmostHalf);
	    clipLeft += __glHalf;
	}
	tempint = (GLint) ((x2-clipLeft) / zoomx);
	if (tempint >= width) return GL_FALSE;

	width -= tempint;
	endCol = (GLint) clipLeft - 1;
	spanInfo->endCol = endCol;
	spanInfo->columns = spanInfo->startCol - endCol;
    }

    if (zoomy > 0) {
	/* Zoomy is positive, clip the bottom edge */
	if (y > clipBottom) {
	    /* Clip to the first row that will be produced */
	    clipBottom = __GL_FLOORF(y + gc->constants.viewportAlmostHalf);
	    clipBottom += __glHalf;
	}
	skipRows = (GLint) ((clipBottom-y) / zoomy);
	if (skipRows >= height) return GL_FALSE;

	height -= skipRows;
	spanInfo->startRow = (GLint) clipBottom;
	y = y + skipRows * zoomy;
	spanInfo->y = y + gc->constants.viewportAlmostHalf;
	spanInfo->srcSkipLines += skipRows;

	/* Zoomy is positive, clip the top edge */
	if (y2 < clipTop) {
	    /* Clip to the last row that will be produced */
	    clipTop = __GL_FLOORF(y2 + gc->constants.viewportAlmostHalf);
	    clipTop -= gc->constants.viewportAlmostHalf;
	}
	tempint = (GLint) ((y2-clipTop) / zoomy);
	if (tempint >= height) return GL_FALSE;

	height -= tempint;
	endRow = (GLint) clipTop + 1;
	spanInfo->rows = endRow - spanInfo->startRow;
    } else /* zoomy < 0 */ {
	/* Zoomy is negative, clip the top edge */
	if (y < clipTop) {
	    /* Clip to the first row that will be produced */
	    clipTop = __GL_FLOORF(y + gc->constants.viewportAlmostHalf);
	    clipTop -= gc->constants.viewportAlmostHalf;
	}
	skipRows = (GLint) ((clipTop-y) / zoomy);
	if (skipRows >= height) return GL_FALSE;

	height -= skipRows;
	spanInfo->startRow = (GLint) clipTop;
	y = y + skipRows * zoomy;
	/* spanInfo->y = y - __glHalf; */
	spanInfo->y = y + gc->constants.viewportAlmostHalf - __glOne;
	spanInfo->srcSkipLines += skipRows;

	/* Zoomy is negative, clip the bottom edge */
	if (y2 > clipBottom) {
	    clipBottom = __GL_FLOORF(y2 + gc->constants.viewportAlmostHalf);
	    clipBottom += __glHalf;
	}
	tempint = (GLint) ((y2-clipBottom) / zoomy);
	if (tempint >= height) return GL_FALSE;

	height -= tempint;
	endRow = (GLint) clipBottom - 1;
	spanInfo->rows = spanInfo->startRow - endRow;
    }

    spanInfo->width = width;
    spanInfo->height = height;

    return GL_TRUE;
}

/*
** This routine computes spanInfo->pixelArray if needed.
**
** If |zoomx| > 1.0, this array contains counts for how many times to 
** replicate a given pixel.  For example, if zoomx is 2.0, this array will
** contain all 2's.  If zoomx is 1.5, then every other entry will contain 
** a 2, and every other entry will contain a 1.
**
** if |zoomx| < 1.0, this array contains counts for how many pixels to 
** skip.  For example, if zoomx is 0.5, every entry in the array will contain
** a 2 (indicating to skip forward two pixels [only past one]).  If zoomx is
** .666, then every other entry will be a 2, and every other entry will be 
** a 1.
*/
void __glComputeSpanPixelArray(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint width, intx;
    __GLfloat zoomx, oldx, newx;
    GLint i;
    GLshort *array;
    
    zoomx = spanInfo->zoomx;
    if (zoomx > (__GLfloat) -1.0 && zoomx < __glOne) {
	GLint lasti;

	/* Build pixel skip array */
	width = spanInfo->width;
	oldx = spanInfo->x;
	array = spanInfo->pixelArray;

	intx = (GLint) oldx;
	newx = oldx;

	lasti = 0;
	for (i=0; i<width; i++) {
	    /* Skip groups which will not be rasterized */
	    newx += zoomx;
	    while ((GLint) newx == intx && i<width) {
		newx += zoomx;
		i++;
	    }
	    assert (i != width);
	    if (i != 0) {
		*array++ = (GLshort) (i - lasti);
	    }
	    lasti = i;
	    intx = (GLint) newx;
	}
	*array++ = 1;
    } else if (zoomx < (__GLfloat) -1.0 || zoomx > __glOne) {
	__GLfloat right;
	GLint iright;
	GLint coladd, column;
	GLint startCol;

	/* Build pixel replication array */
	width = spanInfo->width - 1;
	startCol = spanInfo->startCol;
	column = startCol;
	coladd = spanInfo->coladd;
	array = spanInfo->pixelArray;
	right = spanInfo->x;
	for (i=0; i<width; i++) {
	    right = right + zoomx;
	    iright = (GLint) right;
	    *array++ = (GLshort) (iright - column);
	    column = iright;
	}
	if (coladd == 1) {
	    *array++ = (GLshort) (spanInfo->columns - (column - startCol));
	} else {
	    *array++ = (GLshort) ((startCol - column) - spanInfo->columns);
	}
    }
}

/*
** Initialize the spanInfo structure.  If "packed" is true, the structure
** is initialized for unpacking data from a display list.  If "packed" is 
** false, it is initialized for unpacking data from the user's data space.
*/
void __glLoadUnpackModes(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			 GLboolean packed)
{

    if (packed) {
	/*
	** Data came from a display list.
	*/

	spanInfo->srcAlignment = 1;
	spanInfo->srcSkipPixels = 0;
	spanInfo->srcSkipLines = 0;
	spanInfo->srcSkipImages = 0;
	spanInfo->srcLsbFirst = GL_FALSE;
	spanInfo->srcSwapBytes = GL_FALSE;
	spanInfo->srcLineLength = spanInfo->width;
	spanInfo->srcImageHeight = spanInfo->height;
    } else {
	GLint lineLength, imageHeight;

	/*
	** Data came straight from the application.
	*/

	spanInfo->srcAlignment = gc->clientPixel.unpackModes.alignment;
	spanInfo->srcSkipPixels = gc->clientPixel.unpackModes.skipPixels;
	spanInfo->srcSkipLines = gc->clientPixel.unpackModes.skipLines;
	spanInfo->srcSkipImages = gc->clientPixel.unpackModes.skipImages;
	spanInfo->srcLsbFirst = gc->clientPixel.unpackModes.lsbFirst;
	spanInfo->srcSwapBytes = gc->clientPixel.unpackModes.swapEndian;

	lineLength = gc->clientPixel.unpackModes.lineLength;
	if (lineLength <= 0) lineLength = spanInfo->width;
	spanInfo->srcLineLength = lineLength;

	imageHeight = gc->clientPixel.unpackModes.imageHeight;
	if (imageHeight <= 0) imageHeight = spanInfo->height;
	spanInfo->srcImageHeight = imageHeight;
    }
}

void __glInitDrawPixelsInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		            GLint width, GLint height, GLenum format, 
			    GLenum type, const GLvoid *pixels)
{
    __GLfloat x,y;
    __GLfloat zoomx, zoomy;

    x = gc->state.current.rasterPos.window.x;
    y = gc->state.current.rasterPos.window.y;

#if defined(__GL_HALF_PIXEL_OFFSET)
    x -= __glHalf;
    y -= __glHalf;
#endif

    spanInfo->x = x;
    spanInfo->y = y;
    spanInfo->fragz = (__GLzValue) (gc->state.current.rasterPos.window.z *
						gc->constants.depthRescale);
    zoomx = gc->state.pixel.transferMode.zoomX;
    if (zoomx > __glZero) {
	if (zoomx < __glOne) {
	    spanInfo->rendZoomx = __glOne;
	} else {
	    spanInfo->rendZoomx = zoomx;
	}
	spanInfo->coladd = 1;
    } else {
	if (zoomx > (GLfloat) -1.0) {
	    spanInfo->rendZoomx = (GLfloat) -1.0;
	} else {
	    spanInfo->rendZoomx = zoomx;
	}
	spanInfo->coladd = -1;
    }
    spanInfo->zoomx = zoomx;
    zoomy = gc->state.pixel.transferMode.zoomY;
    if (gc->constants.yInverted) {
	zoomy = -zoomy;
    } else {
	spanInfo->y += gc->constants.viewportEpsilon;
    }
    if (zoomy > __glZero) {
	spanInfo->rowadd = 1;
    } else {
	spanInfo->rowadd = -1;
    }
    spanInfo->zoomy = zoomy;
    spanInfo->width = width;
    spanInfo->height = height;
    if (format == GL_COLOR_INDEX && gc->modes.rgbMode) {
	spanInfo->dstFormat = GL_RGBA;
    } else {
	spanInfo->dstFormat = format;
    }
    spanInfo->srcFormat = format;
    spanInfo->srcType = type;
    spanInfo->srcImage = pixels;

    /*
    ** These aren't needed for DrawPixels, but they are read by the
    ** generic pixel path setup code, so something needs to be set
    ** in them.
    */
    spanInfo->dstType = GL_FLOAT;
    spanInfo->dstElementSize = 4;
    spanInfo->dstSwapBytes = GL_FALSE;
    spanInfo->dstImage = NULL;
    spanInfo->dim = 2;
}

void __glDrawPixelsConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    int i;
    __GLfloat zoomy, newy;
    GLint inty, width, height;
    GLint firstSpanMod, lastSpanMod;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint convModifierIndex = spanInfo->convModifierIndex;
    GLint convInitialSpans = spanInfo->convInitialSpans;
    GLint convFinalSpans = spanInfo->convFinalSpans;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;

    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];
    GLfloat spanCacheData[16][__GL_MAX_SPAN_SIZE];
    GLfloat *spanCache[16];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    for (i=0; i<16; ++i) {
        spanCache[i] = &spanCacheData[i][0];
    }
    spanInfo->spanCache = (GLvoid **) &spanCache;

    spanModifier = spanInfo->spanModifier;
    render = spanInfo->spanRender;

    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    width = spanInfo->width;
    height = spanInfo->height;

    for (i=0; i<height+convFinalSpans; i++) {
	/*
	** Process each span of the source image
	*/
	spanInfo->spanCount = i;
	spanInfo->width = width;

	if (i < convInitialSpans) {
	    firstSpanMod = 0;
	    lastSpanMod = convModifierIndex;
	} else if (i >= height) {
	    firstSpanMod = convModifierIndex;
	    lastSpanMod = numSpanMods-1;
	} else {
	    firstSpanMod = 0;
	    lastSpanMod = numSpanMods-1;
	}

	(*spanModifier[firstSpanMod])(gc, spanInfo,
					spanInfo->srcCurrent, spanData1);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum=firstSpanMod+1; spanModNum<=lastSpanMod; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}

	if (i < convInitialSpans) {
	    continue;
	}

	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	spanInfo->y = newy;
	newy += zoomy;
	if ((GLint) newy != inty) {
	    (*render)(gc, spanInfo, spanDataIn);
	    inty = (GLint) newy;
	}
    }
}

void __glDrawPixelsN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    int i;
    __GLfloat zoomy, newy;
    GLint inty, height;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;

    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    spanModifier = spanInfo->spanModifier;
    render = spanInfo->spanRender;

    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	while ((GLint) newy == inty && i<height) {
	    spanInfo->y = newy;
	    spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		    spanInfo->srcRowIncrement;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;

	(*spanModifier[0])(gc, spanInfo, spanInfo->srcCurrent, spanData1);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum = 1; spanModNum < numSpanMods; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}
	(*render)(gc, spanInfo, spanDataIn);
    }
}

/*
** This is the generic DrawPixels routine.  It applies two span modification
** routines followed by a span rendering routine.
*/
void __glDrawPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    int i;
    __GLfloat zoomy, newy;
    GLint inty, height;
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*span2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		  const GLvoid *inspan, GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    span1 = spanInfo->spanModifier[0];
    span2 = spanInfo->spanModifier[1];
    render = spanInfo->spanRender;

    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	while ((GLint) newy == inty && i<height) {
	    spanInfo->y = newy;
	    spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		    spanInfo->srcRowIncrement;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;
	(*span1)(gc, spanInfo, spanInfo->srcCurrent, spanData1);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	(*span2)(gc, spanInfo, spanData1, spanData2);
	(*render)(gc, spanInfo, spanData2);
    }
}

/* 
** Draw pixels with only one span modification routine.
*/
void __glDrawPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    int i;
    __GLfloat zoomy, newy;
    GLint inty, height;
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		  const GLvoid *inspan, GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE];
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    span1 = spanInfo->spanModifier[0];
    render = spanInfo->spanRender;

    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	while ((GLint) newy == inty && i<height) {
	    spanInfo->y = newy;
	    spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		    spanInfo->srcRowIncrement;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;
	(*span1)(gc, spanInfo, spanInfo->srcCurrent, spanData1);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	(*render)(gc, spanInfo, spanData1);
    }
}

/* 
** Draw pixels with no span modification routines.
*/
void __glDrawPixels0(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    int i;
    __GLfloat zoomy, newy;
    GLint inty, height;
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   const GLvoid *inspan);
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    render = spanInfo->spanRender;

    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	while ((GLint) newy == inty && i<height) {
	    spanInfo->y = newy;
	    spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		    spanInfo->srcRowIncrement;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;
	(*render)(gc, spanInfo, spanInfo->srcCurrent);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
    }
}

/*
** Generic implementation of a DrawPixels picker.  Any machine specific
** implementation should provide their own.
*/
void __glSlowPickDrawPixels(__GLcontext *gc, GLint width, GLint height,
		            GLenum format, GLenum type, const GLvoid *pixels,
			    GLboolean packed)
{
    __GLpixelSpanInfo spanInfo;
    
    __glInitDrawPixelsInfo(gc, &spanInfo, width, height, format, type, pixels);
    __glLoadUnpackModes(gc, &spanInfo, packed);

    if(!__glClipDrawPixels(gc, &spanInfo)) return;

    __glInitUnpacker(gc, &spanInfo);

    __GL_LOCK_PXL_DRAW_BUFFER(gc, format);

    __glGenericPickDrawPixels(gc, &spanInfo);

    __GL_UNLOCK_PXL_DRAW_BUFFER(gc, format);
}

/*
** Generic picker for DrawPixels.  This should be called if no machine
** specific path is provided for this specific version of DrawPixels.
*/
void __glGenericPickDrawPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    void (*dpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
    __GLpixelSpanModInfo spanModInfo;

    spanModInfo.pixelPath = __GL_PIXPATH_DRAWPIX;
    spanModInfo.srcData = __GL_PIXDATA_MEM;
    spanModInfo.dstData = __GL_PIXDATA_FB;

    spanInfo->numSpanMods = 0;
    PickSpanModifiers(gc, spanInfo, &spanModInfo);

    /*
    ** Pick a DrawPixels function that applies the correct number of 
    ** span modifiers.
    */

    if (spanInfo->filter) {
	dpfn = __glDrawPixelsConvolution;
    } else {
	switch(spanInfo->numSpanMods) {
	case 0:
	    dpfn = __glDrawPixels0;
	    break;
	case 1:
	    dpfn = __glDrawPixels1;
	    break;
	case 2:
	    dpfn = __glDrawPixels2;
	    break;
	default:
	    dpfn = __glDrawPixelsN;
	    break;
	}
    }

    (*dpfn)(gc, spanInfo);
}

/*
** This routine clips ReadPixels calls so that only fragments which are
** owned by this context will be read and copied into the user's data.
** Parts of the ReadPixels rectangle lying outside of the window will
** be ignored.
*/
GLboolean __glClipReadPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint clipLeft, clipRight, clipTop, clipBottom;
    GLint x,y,x2,y2;
    GLint skipPixels, skipRows;
    GLint width, height;
    GLint tempint;

    width = spanInfo->width;
    height = spanInfo->height;
    x = (GLint) spanInfo->readX;
    y = (GLint) spanInfo->readY;
    x2 = x + spanInfo->width;
    if (gc->constants.yInverted) {
	y2 = y - spanInfo->height;
    } else {
	y2 = y + spanInfo->height;
    }
#if defined(__GL_ALIGNED_BUFFERS)
    clipLeft = gc->constants.viewportXAdjustOffset;
    clipRight = gc->constants.width + gc->constants.viewportXAdjustOffset;
    clipBottom = gc->constants.viewportYAdjustOffset;
    clipTop = gc->constants.height + gc->constants.viewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
    clipLeft = gc->constants.viewportXAdjust;
    clipRight = gc->constants.width + gc->constants.viewportXAdjust;
    clipBottom = gc->constants.viewportYAdjust;
    clipTop = gc->constants.height + gc->constants.viewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
    skipPixels = 0;
    skipRows = 0;
    if (x < clipLeft) {
	skipPixels = clipLeft - x;
	if (skipPixels > width) return GL_FALSE;

	width -= skipPixels;
	x = clipLeft;
	spanInfo->dstSkipPixels += skipPixels;
	spanInfo->readX = (__GLfloat) x;
    }
    if (x2 > clipRight) {
	tempint = x2 - clipRight;
	if (tempint > width) return GL_FALSE;

	width -= tempint;
    }
    if (gc->constants.yInverted) {
	if (y >= clipTop) {
	    skipRows = y - clipTop + 1;
	    if (skipRows > height) return GL_FALSE;

	    height -= skipRows;
	    y = clipTop - 1;
	    spanInfo->dstSkipLines += skipRows;
	    spanInfo->readY = (__GLfloat) y;
	}
	if (y2 < clipBottom - 1) {
	    tempint = clipBottom - y2 - 1;
	    if (tempint > height) return GL_FALSE;

	    height -= tempint;
	}
    } else {
	if (y < clipBottom) {
	    skipRows = clipBottom - y;
	    if (skipRows > height) return GL_FALSE;

	    height -= skipRows;
	    y = clipBottom;
	    spanInfo->dstSkipLines += skipRows;
	    spanInfo->readY = (__GLfloat) y;
	}
	if (y2 > clipTop) {
	    tempint = y2 - clipTop;
	    if (tempint > height) return GL_FALSE;

	    height -= tempint;
	}
    }

    spanInfo->width = width;
    spanInfo->height = height;

    return GL_TRUE;
}

/*
** Initialize the spanInfo structure for packing data into the user's data
** space.
*/
void __glLoadPackModes(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint lineLength = gc->clientPixel.packModes.lineLength;
    GLint imageHeight = gc->clientPixel.packModes.imageHeight;

    spanInfo->dstAlignment = gc->clientPixel.packModes.alignment;
    spanInfo->dstSkipPixels = gc->clientPixel.packModes.skipPixels;
    spanInfo->dstSkipLines = gc->clientPixel.packModes.skipLines;
    spanInfo->dstSkipImages = gc->clientPixel.packModes.skipImages;
    spanInfo->dstLsbFirst = gc->clientPixel.packModes.lsbFirst;
    spanInfo->dstSwapBytes = gc->clientPixel.packModes.swapEndian;
    if (lineLength <= 0) lineLength = spanInfo->width;
    spanInfo->dstLineLength = lineLength;
    if (imageHeight <= 0) imageHeight = spanInfo->height;
    spanInfo->dstImageHeight = imageHeight;
}

void __glInitReadPixelsInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		            GLint x, GLint y, GLint width, GLint height, 
			    GLenum format, GLenum type, GLvoid *pixels)
{
#if defined(__GL_ALIGNED_BUFFERS)
    spanInfo->readX = (__GLfloat) (x + gc->constants.viewportXAdjustOffset);
    if (gc->constants.yInverted) {
	spanInfo->readY = (__GLfloat) ((gc->constants.height - y - 1) + 
		gc->constants.viewportYAdjustOffset);
    } else {
	spanInfo->readY = (__GLfloat) (y + gc->constants.viewportYAdjustOffset);
    }
#else /* __GL_ALIGNED_BUFFERS */
    spanInfo->readX = (__GLfloat) (x + gc->constants.viewportXAdjust);
    if (gc->constants.yInverted) {
	spanInfo->readY = (__GLfloat) ((gc->constants.height - y - 1) + 
		gc->constants.viewportYAdjust);
    } else {
	spanInfo->readY = (__GLfloat) (y + gc->constants.viewportYAdjust);
    }
#endif /* __GL_ALIGNED_BUFFERS */
    spanInfo->width = width;
    spanInfo->height = height;
    spanInfo->dstFormat = format;
    spanInfo->dstType = type;
    spanInfo->dstImage = pixels;
    spanInfo->zoomx = __glOne;
    spanInfo->x = __glZero;
    __glLoadPackModes(gc, spanInfo);
    if (gc->clientPixel.packModes.lineLength <= 0) {
	spanInfo->dstLineLength = spanInfo->width + gc->pixel.adjustWidth2D;
    }
    /*
    ** These aren't needed for ReadPixels, but they are read by the
    ** generic pixel path setup code, so something needs to be set
    ** in them.
    */
    spanInfo->srcType = GL_FLOAT;
    spanInfo->srcElementSize = 4;
    spanInfo->srcSwapBytes = GL_FALSE;
    spanInfo->srcImage = NULL;
    spanInfo->srcFormat = gc->modes.rgbMode ? GL_RGBA : GL_COLOR_INDEX;
    spanInfo->dim = 2;
}

void __glReadPixelsConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i, ySign;
    GLint width, height;
    GLint firstSpanMod, lastSpanMod;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint convModifierIndex = spanInfo->convModifierIndex;
    GLint convInitialSpans = spanInfo->convInitialSpans;
    GLint convFinalSpans = spanInfo->convFinalSpans;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;
    GLfloat spanCacheData[16][__GL_MAX_SPAN_SIZE];
    GLfloat *spanCache[16];

    for (i=0; i<16; ++i) {
        spanCache[i] = &spanCacheData[i][0];
    }
    spanInfo->spanCache = (GLvoid **) &spanCache;

    reader = spanInfo->spanReader;

    ySign = gc->constants.ySign;
    width = spanInfo->width;
    height = spanInfo->height;
    spanModifier = spanInfo->spanModifier;

    for (i=0; i<height+convFinalSpans; i++) {
	/*
	** Process each span of the source image
	*/
	spanInfo->spanCount = i;
	spanInfo->width = width;

	if (i < convInitialSpans) {
	    firstSpanMod = 0;
	    lastSpanMod = convModifierIndex;
	    (*reader)(gc, spanInfo, spanData1);
	} else if (i >= height) {
	    firstSpanMod = convModifierIndex;
	    lastSpanMod = numSpanMods-1;
	} else {
	    firstSpanMod = 0;
	    lastSpanMod = numSpanMods-1;
	    (*reader)(gc, spanInfo, spanData1);
	}

	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum=firstSpanMod; spanModNum<=lastSpanMod-1; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}
	(*spanModifier[spanModNum])(gc, spanInfo,
					spanDataIn, spanInfo->dstCurrent);

	spanInfo->readY += ySign;

	if (i < convInitialSpans) {
	    continue;
	}

	spanInfo->dstCurrent = (GLvoid *) ((GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement);
    }
}

void __glReadPixelsN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i, ySign;
    GLint height;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;

    reader = spanInfo->spanReader;

    ySign = gc->constants.ySign;
    height = spanInfo->height;
    spanModifier = spanInfo->spanModifier;

    for (i=0; i<height; i++) {
	(*reader)(gc, spanInfo, spanData1);
	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum = 0; spanModNum < numSpanMods-1; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}
	(*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanInfo->dstCurrent);

	spanInfo->dstCurrent = (GLvoid *) ((GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement);
	spanInfo->readY += ySign;
    }
}

/*
** A simple generic ReadPixels routine with two span modifiers.
*/
void __glReadPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i, ySign;
    GLint height;
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*span2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		  const GLvoid *inspan, GLvoid *outspan);
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];

    span1 = spanInfo->spanModifier[0];
    span2 = spanInfo->spanModifier[1];
    reader = spanInfo->spanReader;

    ySign = gc->constants.ySign;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	(*reader)(gc, spanInfo, spanData1);
	(*span1)(gc, spanInfo, spanData1, spanData2);
	(*span2)(gc, spanInfo, spanData2, spanInfo->dstCurrent);
	spanInfo->dstCurrent = (GLvoid *) ((GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement);
	spanInfo->readY += ySign;
    }
}

/*
** A simple generic ReadPixels routine with one span modifier.
*/
void __glReadPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i, ySign;
    GLint height;
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE];

    span1 = spanInfo->spanModifier[0];
    reader = spanInfo->spanReader;

    ySign = gc->constants.ySign;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	(*reader)(gc, spanInfo, spanData1);
	(*span1)(gc, spanInfo, spanData1, spanInfo->dstCurrent);
	spanInfo->dstCurrent = (GLvoid *) ((GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement);
	spanInfo->readY += ySign;
    }
}

/*
** A simple generic ReadPixels routine with no span modifiers.
*/
void __glReadPixels0(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i, ySign;
    GLint height;
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);

    reader = spanInfo->spanReader;

    ySign = gc->constants.ySign;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	(*reader)(gc, spanInfo, spanInfo->dstCurrent);
	spanInfo->dstCurrent = (GLvoid *) ((GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement);
	spanInfo->readY += ySign;
    }
}

/*
** Generic implementation of a ReadPixels picker.  Any machine specific
** implementation should provide their own.
*/
void __glSlowPickReadPixels(__GLcontext *gc, GLint x, GLint y,
		            GLsizei width, GLsizei height,
		            GLenum format, GLenum type, GLvoid *pixels)
{
    __GLpixelSpanInfo spanInfo;

    __glInitReadPixelsInfo(gc, &spanInfo, x, y, width, height, format, 
	    type, pixels);
    if (!__glClipReadPixels(gc, &spanInfo)) return;

    __glInitPacker(gc, &spanInfo);

    __GL_LOCK_PXL_READ_BUFFER(gc, format);

    __glGenericPickReadPixels(gc, &spanInfo);

    __GL_UNLOCK_PXL_READ_BUFFER(gc, format);
}

/*
** Generic picker for ReadPixels.  This should be called if no machine
** specific path is provided for this specific version of ReadPixels.
*/
void __glGenericPickReadPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    void (*rpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
    __GLpixelSpanModInfo spanModInfo;

    spanModInfo.pixelPath = __GL_PIXPATH_READPIX;
    spanModInfo.srcData = __GL_PIXDATA_FB;
    spanModInfo.dstData = __GL_PIXDATA_MEM;

    spanInfo->numSpanMods = 0;
    PickSpanModifiers(gc, spanInfo, &spanModInfo);

    /*
    ** Pick a ReadPixels routine that uses the right number of span 
    ** modifiers.
    */

    if (spanInfo->filter) {
	rpfn = __glReadPixelsConvolution;
    } else {
	switch(spanInfo->numSpanMods) {
	case 0:
	    rpfn = __glReadPixels0;
	    break;
	case 1:
	    rpfn = __glReadPixels1;
	    break;
	case 2:
	    rpfn = __glReadPixels2;
	    break;
	default:
	    rpfn = __glReadPixelsN;
	    break;
	}
    }

    (*rpfn)(gc, spanInfo);
}

/*
** This routine does two clips.  It clips like the DrawPixel clipper so 
** that if you try to copy to off window pixels, nothing will be done, and it 
** also clips like the ReadPixel clipper so that if you try to copy from
** off window pixels, nothing will be done.
*/
GLboolean __glClipCopyPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    __GLfloat num, den;
    __GLfloat rpyUp, rpyDown;
    GLint rowsUp, rowsDown, startUp, startDown;
    __GLfloat midPoint;
    GLint intMidPoint, rowCount;
    GLint width, height;
    GLint readX, readY;
    __GLfloat zoomx, zoomy;
    __GLfloat rpx, rpy;
    __GLfloat rx1, rx2, ry1, ry2, wx1, wx2, wy1, wy2;
    __GLfloat abszoomy;
    GLint readUp, readDown;

    /*
    ** NOTE:
    ** A "nice" thing we could do for our application writers would be 
    ** to copy white when they try to copy from off window memory.  This
    ** would alert them to a bug in their program which they could then
    ** fix.
    **
    ** However, that seems like unnecessary code which would never be used
    ** anyway (no reason to bloat unnecessarily).
    */

    /*
    ** We take the easy approach, and just call the DrawPixels and ReadPixels
    ** clippers directly.
    */
    spanInfo->dstSkipLines = 0;
    spanInfo->dstSkipPixels = 0;
    if (!__glClipReadPixels(gc, spanInfo)) return GL_FALSE;
    spanInfo->x += spanInfo->dstSkipPixels * spanInfo->zoomx;
    spanInfo->y += spanInfo->dstSkipLines * spanInfo->zoomy;

    spanInfo->srcSkipLines = 0;
    spanInfo->srcSkipPixels = 0;
    if (!__glClipDrawPixels(gc, spanInfo)) return GL_FALSE;
    spanInfo->readX += spanInfo->srcSkipPixels;
    if (gc->constants.yInverted) {
	spanInfo->readY -= spanInfo->srcSkipLines;
    } else {
	spanInfo->readY += spanInfo->srcSkipLines;
    }

    /*
    ** Now for the incredibly tricky part!
    **
    ** This code attempts to deal with overlapping CopyPixels regions.
    ** It is a very difficult problem given that zoomy may be negative.
    ** The IrisGL used a cheap hack to solve this problem, which is 
    ** to read in the entire source image, and then write the destination
    ** image.  The problem with this approach, of course, is that it 
    ** requires a large amount of memory.
    **
    ** If zoomy can only be positive, then any image can be copied by
    ** copying a single span at a time, as long as you are careful about
    ** what order you process the spans.  However, since zoomy may be
    ** negative, the worst case images require copying two spans at 
    ** a time.  This means reading both spans, possibly modifying them,
    ** and then writing them back out. 
    **
    ** An example of this can be seen as follows:  Suppose an image
    ** covering 4 spans is copied onto itself with a zoomy of -1.  This
    ** means that the first row will be copied to the fourth row,
    ** and the fourth row will be copied to the first row.  In order 
    ** to accomplish both of these copies, they must be performed 
    ** simultaneously (after all, if you copy the first row to
    ** the fourth row first, then you have just destroyed the data 
    ** on the fourth row, and you can no longer copy it!).
    **
    ** In the most general case, any rectangular image can be copied
    ** by simultaneously iterating two spans over the source image
    ** and copying as you go.  Sometimes these spans will start at 
    ** the outside of the image and move their way inwards meeting 
    ** in the middle, and sometimes they will start in the middle 
    ** and work their way outward.
    **
    ** The middle point where the spans both start or end depends
    ** upon how the source and destination images overlap.  This point
    ** may be exactly in the middle, or at either end.  This means 
    ** that you may only end up with just a single span iterating over the 
    ** entire image (starting at one end and moving to the other).
    **
    ** The code that follows computes if the images overlap, and if they
    ** do, how two spans can be used to iterate over the source image
    ** so that it can be successfully copied to the destination image.
    **
    ** The following fields in the spanInfo record will be set in the 
    ** process of making these calculations:
    **
    ** overlap - set to GL_TRUE if the regions overlap at all.  Set to
    **		 GL_FALSE otherwise.
    **
    ** rowsUp, rowsDown - The number of rows of the source image that
    ** 			  need to be dealt with by the span that moves up
    **			  over the source image and the one that moves down
    **			  over the source image.  For example, if rowsUp is
    **			  equal to 10 and rowsDown is 0, then all 10 rows of 
    **			  the image should be copied by the up moving span
    **			  (the one that starts at readY and works it's way
    **			  up to readY+height).
    **
    ** startUp, startDown - At what relative points in time the spans should
    **			    start iterating.  For example, if startUp is 0
    **			    and startDown is 2, then the up moving span 
    **			    should be started first, and after it has 
    **			    iterated over 2 rows of the source image then
    **			    the down moving span should be started.
    **
    ** rpyUp, rpyDown - The starting raster positions for the two spans.
    **			These numbers are not exactly what they claim to
    **			be, but they are close.  They should be used by
    **			the span iterators in the following manner:  When
    **			the up moving span starts, it starts iterating 
    **			the float "rp_y" at rpyUp.  After reading and
    **			modifying a span, the span is written to rows
    **           	floor(rp_y) through floor(rp_y+zoomy) of the
    **			screen (not-inclusive of floor(rp_y+zoomy)).
    **			rp_y is then incremented by zoomy.  The same 
    **			algorithm is applied to the down moving span except
    **			that zoomy is subtracted from rp_y instead of
    **			being added.
    **
    ** readUp, readDown - The spans that are to be used for reading from
    **			  the source image.  The up moving span should start
    **			  reading at line "readUp", and the down moving span
    **			  should start at "readDown". 
    **
    ** Remember that the up moving and down moving spans must be iterated
    ** over the image simultaneously such that both spans are read before
    ** either one is written.
    **
    ** The actual algorithm applied here took many many hours of scratch 
    ** paper, and graph diagrams to derive.  It is very complicated, and
    ** hard to understand.  Do not attempt to change it without first
    ** understanding what it does completely.
    **
    ** In a nutshell, it first computes what span of the source image 
    ** will be copied onto itself (if any), and if |zoomy| < 1 it starts the
    ** up and down moving spans there and moves them outwards, or if 
    ** |zoomy| >= 1 it starts the spans at the outside of the image 
    ** and moves them inward so that they meet at the computed point.
    **
    ** Computing what span of the source image copies onto itself is 
    ** relatively easy.  For any span j of the source image from 0 through
    ** height, the span is read from row "readY + j" and written to
    ** any row centers falling within the range "rp_y + j * zoomy" through 
    ** "rp_y + (j+1) * zoomy".  If you set these equations equal to 
    ** each other (and subtract 0.5 from the raster position -- effectively
    ** moving the row centers from X.5 to X.0), you can determine that for 
    ** j = (readY - (rpy - 0.5)) / (zoomy-1) the source image concides with
    ** the destination image.  This is a floating point solution to a discrete
    ** problem, meaning that it is not a complete solution, but that is 
    ** the general idea.  Explaining this algorithm in any more detail would
    ** take another 1000 lines of comments, so I will leave it at that.
    */

    width = spanInfo->width;
    height = spanInfo->height;
    rpx = spanInfo->x;
    rpy = spanInfo->y;
    readX = (GLint) spanInfo->readX;
    readY = (GLint) spanInfo->readY;
    zoomx = spanInfo->zoomx;
    zoomy = spanInfo->zoomy;

    /* First check if the regions overlap at all */
    if (gc->constants.yInverted) {
	ry1 = readY - height + __glHalf;
	ry2 = readY - gc->constants.viewportAlmostHalf;
    } else {
	ry1 = readY + __glHalf;
	ry2 = readY + height - gc->constants.viewportAlmostHalf;
    }
    rx1 = readX + __glHalf;
    rx2 = readX + width - gc->constants.viewportAlmostHalf;
    if (zoomx > 0) {
	/* Undo some math done by ClipDrawPixels */
	rpx = rpx - gc->constants.viewportAlmostHalf;
	wx1 = rpx;
	wx2 = rpx + zoomx * (width + gc->pixel.adjustWidth2D);
    } else {
	/* Undo some math done by ClipDrawPixels */
	rpx = rpx - gc->constants.viewportAlmostHalf + __glOne;
	wx1 = rpx + zoomx * (width + gc->pixel.adjustHeight2D);
	wx2 = rpx;
    }
    if (zoomy > 0) {
	/* Undo some math done by ClipDrawPixels */
	rpy = rpy - gc->constants.viewportAlmostHalf;
	abszoomy = zoomy;
	wy1 = rpy;
	wy2 = rpy + zoomy * (height + gc->pixel.adjustWidth2D);
    } else {
	/* Undo some math done by ClipDrawPixels */
	rpy = rpy - gc->constants.viewportAlmostHalf + __glOne;
	abszoomy = -zoomy;
	wy1 = rpy + zoomy * (height + gc->pixel.adjustHeight2D);
	wy2 = rpy;
    }

    if (rx2 < wx1 || wx2 < rx1 || ry2 < wy1 || wy2 < ry1) {
	/* No overlap! */
	spanInfo->overlap = GL_FALSE;
	spanInfo->rowsUp = height;
	spanInfo->rowsDown = 0;
	spanInfo->startUp = 0;
	spanInfo->startDown = 0;
	spanInfo->rpyUp = rpy;
	spanInfo->rpyDown = rpy;
	return GL_TRUE;
    }

    spanInfo->overlap = GL_TRUE;

    /* Time to compute how we should set up our spans */
    if (gc->constants.yInverted) {
	num = (rpy - 0.5f) - readY;
	den = -zoomy - 1;
    } else {
	num = readY - (rpy - 0.5f);
	den = zoomy - 1;
    }
    startDown = startUp = 0;
    rowsUp = rowsDown = 0;
    rpyUp = rpy;
    rpyDown = rpy + zoomy*height;
    readUp = readY;
    if (gc->constants.yInverted) {
	readDown = readY - height + 1;
    } else {
	readDown = readY + height - 1;
    }

    if (den == __glZero) {
	/* Better not divide! */
	if (num > 0) {
	    midPoint = (__GLfloat) height;
	} else {
	    midPoint = __glZero;
	}
    } else {
	midPoint = num/den;
	if (midPoint < __glZero) {
	    midPoint = __glZero;
	} else if (midPoint > height) {
	    midPoint = (__GLfloat) height;
	}
    }
    if (midPoint == __glZero) {
	/* Only one span needed */
	if (abszoomy < __glOne) {
	    rowsUp = height;
	} else {
	    rowsDown = height;
	}
    } else if (midPoint == height) {
	/* Only one span needed */
	if (abszoomy < __glOne) {
	    rowsDown = height;
	} else {
	    rowsUp = height;
	}
    } else {
	/* Almost definitely need two spans to copy this image! */
	intMidPoint = (GLint) __GL_CEILF(midPoint);

	rowCount = height - intMidPoint;
	if (intMidPoint > rowCount) {
	    rowCount = intMidPoint;
	}

	if (abszoomy > __glOne) {
	    GLint temp;

	    /* Move from outside of image inward */
	    startUp = rowCount - intMidPoint;
	    startDown = rowCount - (height - intMidPoint);
	    rowsUp = intMidPoint;
	    rowsDown = height - rowsUp;

	    if (gc->constants.yInverted) {
		temp = readY - intMidPoint + 1;
	    } else {
		temp = readY + intMidPoint - 1;
	    }

	    if (__GL_FLOORF( (temp - 
		    (rpy-__glHalf-gc->constants.viewportEpsilon)) 
		    / zoomy) == intMidPoint-1) {
		/* 
		** row "intMidPoint-1" copies exactly onto itself.  Let's 
		** make it the midpoint which we converge to.
		*/
		if (startDown) {
		    startDown--;
		} else {
		    startUp++;
		}
	    }
	} else {
	    /* Move from inside of image outward */
	    rowsDown = intMidPoint;
	    rowsUp = height - rowsDown;
	    rpyUp = rpyDown = rpy + zoomy * intMidPoint;
	    if (gc->constants.yInverted) {
		readUp = readY - intMidPoint;
		readDown = readY - intMidPoint + 1;
	    } else {
		readUp = readY + intMidPoint;
		readDown = readY + intMidPoint - 1;
	    }

	    if (__GL_FLOORF( (readDown - 
		    (rpy-__glHalf-gc->constants.viewportEpsilon))
		    / zoomy) == intMidPoint-1) {
		/* 
		** row "intMidPoint-1" copies exactly onto itself.  Let's
		** make it the midpoint which we diverge from.
		*/
		startUp = 1;
	    }
	}
    }

    /* 
    ** Adjust rpyUp and rpyDown so that they will change integer values 
    ** when fragments should be produced.  This basically takes the 0.5
    ** out of the inner loop when these spans are actually iterated.
    */
    if (zoomy > 0) {
	spanInfo->rpyUp = rpyUp + gc->constants.viewportAlmostHalf;
	spanInfo->rpyDown = rpyDown + gc->constants.viewportAlmostHalf - 
		__glOne;
    } else {
	spanInfo->rpyUp = rpyUp + gc->constants.viewportAlmostHalf - __glOne;
	spanInfo->rpyDown = rpyDown + gc->constants.viewportAlmostHalf;
    }
    spanInfo->startUp = startUp;
    spanInfo->startDown = startDown;
    spanInfo->rowsUp = rowsUp;
    spanInfo->rowsDown = rowsDown;
    spanInfo->readUp = readUp;
    spanInfo->readDown = readDown;

    return GL_TRUE;
}

void __glInitCopyPixelsInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    GLint x, GLint y, GLint width, GLint height, 
			    GLenum format)
{
    __GLfloat rpx, rpy;
    __GLfloat zoomx, zoomy;

    rpx = gc->state.current.rasterPos.window.x;
    rpy = gc->state.current.rasterPos.window.y;

#if defined(__GL_HALF_PIXEL_OFFSET)
    rpx -= __glHalf;
    rpy -= __glHalf;
#endif

    spanInfo->x = rpx;
    spanInfo->y = rpy;
    spanInfo->fragz = (__GLzValue) (gc->state.current.rasterPos.window.z *
						gc->constants.depthRescale);
    zoomx = gc->state.pixel.transferMode.zoomX;
    if (zoomx > __glZero) {
	if (zoomx < __glOne) {
	    spanInfo->rendZoomx = __glOne;
	} else {
	    spanInfo->rendZoomx = zoomx;
	}
	spanInfo->coladd = 1;
    } else {
	if (zoomx > (GLfloat) -1.0) {
	    spanInfo->rendZoomx = (GLfloat) -1.0;
	} else {
	    spanInfo->rendZoomx = zoomx;
	}
	spanInfo->coladd = -1;
    }
    spanInfo->zoomx = zoomx;
    zoomy = gc->state.pixel.transferMode.zoomY;
    if (gc->constants.yInverted) {
	zoomy = -zoomy;
    } else {
	spanInfo->y += gc->constants.viewportEpsilon;
    }
    if (zoomy > __glZero) {
	spanInfo->rowadd = 1;
    } else {
	spanInfo->rowadd = -1;
    }
    spanInfo->zoomy = zoomy;
#if defined(__GL_ALIGNED_BUFFERS)
    spanInfo->readX = (__GLfloat) (x + gc->constants.viewportXAdjustOffset);
    if (gc->constants.yInverted) {
	spanInfo->readY = (__GLfloat) ((gc->constants.height - y - 1) + 
		gc->constants.viewportYAdjustOffset);
    } else {
	spanInfo->readY = (__GLfloat) (y + gc->constants.viewportYAdjustOffset);
    }
#else /* __GL_ALIGNED_BUFFERS */
    spanInfo->readX = (__GLfloat) (x + gc->constants.viewportXAdjust);
    if (gc->constants.yInverted) {
	spanInfo->readY = (__GLfloat) ((gc->constants.height - y - 1) + 
		gc->constants.viewportYAdjust);
    } else {
	spanInfo->readY = (__GLfloat) (y + gc->constants.viewportYAdjust);
    }
#endif /* __GL_ALIGNED_BUFFERS */
    spanInfo->dstFormat = spanInfo->srcFormat = format;
    spanInfo->width = width;
    spanInfo->height = height;
    /*
    ** These aren't needed for CopyPixels, but they are read by the
    ** generic pixel path setup code, so something needs to be set
    ** in them.
    */
    spanInfo->srcType = GL_FLOAT;
    spanInfo->srcElementSize = 4;
    spanInfo->srcSwapBytes = GL_FALSE;
    spanInfo->srcImage = NULL;
    spanInfo->srcFormat = gc->modes.rgbMode ? GL_RGBA : GL_COLOR_INDEX;
    spanInfo->dstType = GL_FLOAT;
    spanInfo->dstElementSize = 4;
    spanInfo->dstSwapBytes = GL_FALSE;
    spanInfo->dstImage = NULL;
    spanInfo->applySrcClamp = GL_TRUE;
    spanInfo->applyDstClamp = GL_TRUE;
    spanInfo->applyFbScale = GL_TRUE;
    spanInfo->applyPixelTransfer = GL_TRUE;
    spanInfo->applyGenericScaleBias = GL_FALSE;
    spanInfo->nonColorComp = GL_FALSE;
    spanInfo->dim = 2;
}


/* 
** A CopyPixels with convolution.
*/
void __glCopyPixelsConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    __GLfloat newy;
    __GLfloat zoomy;
    GLint inty, i, ySign;
    GLint width, height;
    GLint firstSpanMod, lastSpanMod;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint convModifierIndex = spanInfo->convModifierIndex;
    GLint convInitialSpans = spanInfo->convInitialSpans;
    GLint convFinalSpans = spanInfo->convFinalSpans;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];
    GLfloat spanCacheData[16][__GL_MAX_SPAN_SIZE];
    GLfloat *spanCache[16];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    for (i=0; i<16; ++i) {
        spanCache[i] = &spanCacheData[i][0];
    }
    spanInfo->spanCache = (GLvoid **) &spanCache;

    reader = spanInfo->spanReader;
    render = spanInfo->spanRender;

    if (spanInfo->overlap) {
#if 0 /* XXXimaging_changes */
	__glCopyPixelsOverlapping(gc, spanInfo, numSpanMods);
#endif
	return;
    }

    ySign = gc->constants.ySign;
    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    width = spanInfo->width;
    height = spanInfo->height;
    spanModifier = spanInfo->spanModifier;

    for (i=0; i<height+convFinalSpans; i++) {
	/*
	** Process each span of the source image
	*/
	spanInfo->spanCount = i;
	spanInfo->width = width;

	if (i < convInitialSpans) {
	    firstSpanMod = 0;
	    lastSpanMod = convModifierIndex;
	    (*reader)(gc, spanInfo, spanData1);
	} else if (i >= height) {
	    firstSpanMod = convModifierIndex;
	    lastSpanMod = numSpanMods-1;
	} else {
	    firstSpanMod = 0;
	    lastSpanMod = numSpanMods-1;
	    (*reader)(gc, spanInfo, spanData1);
	}

	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum=firstSpanMod; spanModNum<=lastSpanMod; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}

	spanInfo->readY += ySign;

	if (i < convInitialSpans) {
	    continue;
	}

	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	spanInfo->y = newy;
	newy += zoomy;
	if ((GLint) newy != inty) {
	    (*render)(gc, spanInfo, spanDataIn);
	    inty = (GLint) newy;
	}
    }
}

/* 
** A CopyPixels with N span modifiers.
*/
void __glCopyPixelsN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    __GLfloat newy;
    __GLfloat zoomy;
    GLint inty, i, ySign;
    GLint height;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    if (spanInfo->overlap) {
	__glCopyPixelsOverlapping(gc, spanInfo, numSpanMods);
	return;
    }

    reader = spanInfo->spanReader;
    render = spanInfo->spanRender;

    ySign = gc->constants.ySign;
    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;
    spanModifier = spanInfo->spanModifier;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	while ((GLint) newy == inty && i<height) {
	    spanInfo->readY += ySign;
	    spanInfo->y = newy;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;

	(*reader)(gc, spanInfo, spanData1);
	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum = 0; spanModNum < numSpanMods; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}
	(*render)(gc, spanInfo, spanDataIn);

	spanInfo->readY += ySign;
    }
}


/* 
** A CopyPixels with two span modifiers.
*/
void __glCopyPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    __GLfloat newy;
    __GLfloat zoomy;
    GLint inty, i, ySign;
    GLint height;
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*span2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    if (spanInfo->overlap) {
	__glCopyPixelsOverlapping(gc, spanInfo, 2);
	return;
    }

    reader = spanInfo->spanReader;
    span1 = spanInfo->spanModifier[0];
    span2 = spanInfo->spanModifier[1];
    render = spanInfo->spanRender;

    ySign = gc->constants.ySign;
    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	while ((GLint) newy == inty && i<height) {
	    spanInfo->readY += ySign;
	    spanInfo->y = newy;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;
	(*reader)(gc, spanInfo, spanData1);
	(*span1)(gc, spanInfo, spanData1, spanData2);
	(*span2)(gc, spanInfo, spanData2, spanData1);
	(*render)(gc, spanInfo, spanData1);
	spanInfo->readY += ySign;
    }
}

/* 
** A CopyPixels with one span modifier.
*/
void __glCopyPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    __GLfloat newy;
    __GLfloat zoomy;
    GLint inty, i, ySign;
    GLint height;
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    if (spanInfo->overlap) {
	__glCopyPixelsOverlapping(gc, spanInfo, 1);
	return;
    }

    reader = spanInfo->spanReader;
    span1 = spanInfo->spanModifier[0];
    render = spanInfo->spanRender;

    ySign = gc->constants.ySign;
    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	/*
	** Skip over spans when zoomy is in the range (-1,1)
	*/
	while ((GLint) newy == inty && i<height) {
	    spanInfo->readY += ySign;
	    spanInfo->y = newy;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;
	(*reader)(gc, spanInfo, spanData1);
	(*span1)(gc, spanInfo, spanData1, spanData2);
	(*render)(gc, spanInfo, spanData2);
	spanInfo->readY += ySign;
    }
}

/* 
** Copy pixels with no span modifiers.
*/
void __glCopyPixels0(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    __GLfloat newy;
    __GLfloat zoomy;
    GLint inty, i, ySign;
    GLint height;
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		   const GLvoid *inspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE];
    GLshort pixelArray[__GL_MAX_MAX_VIEWPORT];

    spanInfo->pixelArray = pixelArray;
    __glComputeSpanPixelArray(gc, spanInfo);

    if (spanInfo->overlap) {
	__glCopyPixelsOverlapping(gc, spanInfo, 0);
	return;
    }

    reader = spanInfo->spanReader;
    render = spanInfo->spanRender;

    ySign = gc->constants.ySign;
    zoomy = spanInfo->zoomy;
    inty = (GLint) spanInfo->y;
    newy = spanInfo->y;
    height = spanInfo->height;

    for (i=0; i<height; i++) {
	spanInfo->y = newy;
	newy += zoomy;
	while ((GLint) newy == inty && i<height) {
	    spanInfo->readY += ySign;
	    spanInfo->y = newy;
	    newy += zoomy;
	    i++;
	    assert (i != height);
	}
	inty = (GLint) newy;
	(*reader)(gc, spanInfo, spanData1);
	(*render)(gc, spanInfo, spanData1);
	spanInfo->readY += ySign;
    }
}

/*
** Yick!  
**
** This routine is provided to perform CopyPixels when the source and
** destination images overlap.  
**
** It is not designed to go particularly fast, but then overlapping
** copies is probably not too common, and this routine is not typically a 
** large part of the execution overhead anyway.
**
** For more information on copying an image which overlaps its destination,
** check out the hairy comment within the __glClipCopyPixels function.
*/
void __glCopyPixelsOverlapping(__GLcontext *gc, 
			       __GLpixelSpanInfo *spanInfo, GLint modifiers)
{
    GLint i;
    __GLfloat zoomy, newy;
    GLint inty, ySign;
    GLubyte spanData1[__GL_MAX_SPAN_SIZE], spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte spanData3[__GL_MAX_SPAN_SIZE];
    GLubyte *outSpan1, *outSpan2;
    GLint rowsUp, rowsDown;
    GLint startUp, startDown;
    __GLfloat rpyUp, rpyDown;
    GLint readUp, readDown;
    GLint gotUp, gotDown;
    __GLpixelSpanInfo downSpanInfo;
    GLint clipLow, clipHigh;
    GLint startRow, endRow;
    void (*reader)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		   GLvoid *outspan);
    void (*render)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		   const GLvoid *inspan);

    reader = spanInfo->spanReader;
    render = spanInfo->spanRender;

    if (modifiers & 1) {
	outSpan1 = outSpan2 = spanData3;
    } else {
	outSpan1 = spanData1;
	outSpan2 = spanData2;
    }

    zoomy = spanInfo->zoomy;
    rowsUp = spanInfo->rowsUp;
    rowsDown = spanInfo->rowsDown;
    startUp = spanInfo->startUp;
    startDown = spanInfo->startDown;
    rpyUp = spanInfo->rpyUp;
    rpyDown = spanInfo->rpyDown;
    readUp = spanInfo->readUp;
    readDown = spanInfo->readDown;
    downSpanInfo = *spanInfo;
    downSpanInfo.rowadd = -spanInfo->rowadd;
    downSpanInfo.zoomy = -zoomy;
    spanInfo->y = rpyUp;
    downSpanInfo.y = rpyDown;
    spanInfo->readY = (__GLfloat) readUp;
    downSpanInfo.readY = (__GLfloat) readDown;
    gotUp = gotDown = 0;
    ySign = gc->constants.ySign;

    /* Clip upgoing and downgoing spans */
    if (zoomy > 0) {
	clipLow = spanInfo->startRow;
	clipHigh = spanInfo->startRow + spanInfo->rows - 1;

	/* Clip down span first */
	startRow = (GLint) rpyDown;
	endRow = (GLint) (rpyDown - zoomy*rowsDown) + 1;
	if (startRow > clipHigh) startRow = clipHigh;
	if (endRow < clipLow) endRow = clipLow;
	downSpanInfo.startRow = startRow;
	downSpanInfo.rows = startRow - endRow + 1;

	/* Now clip up span */
	startRow = (GLint) rpyUp;
	endRow = (GLint) (rpyUp + zoomy*rowsUp) - 1;
	if (startRow < clipLow) startRow = clipLow;
	if (endRow > clipHigh) endRow = clipHigh;
	spanInfo->startRow = startRow;
	spanInfo->rows = endRow - startRow + 1;
    } else /* zoomy < 0 */ {
	clipHigh = spanInfo->startRow;
	clipLow = spanInfo->startRow - spanInfo->rows + 1;

	/* Clip down span first */
	startRow = (GLint) rpyDown;
	endRow = (GLint) (rpyDown - zoomy*rowsDown) - 1;
	if (startRow < clipLow) startRow = clipLow;
	if (endRow > clipHigh) endRow = clipHigh;
	downSpanInfo.startRow = startRow;
	downSpanInfo.rows = endRow - startRow + 1;

	/* Now clip up span */
	startRow = (GLint) rpyUp;
	endRow = (GLint) (rpyUp + zoomy*rowsUp) + 1;
	if (startRow > clipHigh) startRow = clipHigh;
	if (endRow < clipLow) endRow = clipLow;
	spanInfo->startRow = startRow;
	spanInfo->rows = startRow - endRow + 1;
    }

    while (rowsUp && rowsDown) {
	if (startUp) {
	    startUp--;
	} else {
	    gotUp = 1;
	    rowsUp--;
	    spanInfo->y = rpyUp;
	    newy = rpyUp + zoomy;
	    inty = (GLint) rpyUp;
	    while (rowsUp && (GLint) newy == inty) {
		spanInfo->y = newy;
		newy += zoomy;
		rowsUp--;
		spanInfo->readY += ySign;
	    }
	    if (inty == (GLint) newy) break;
	    rpyUp = newy;
	    (*reader)(gc, spanInfo, spanData1);
	    spanInfo->readY += ySign;
	}
	if (startDown) {
	    startDown--;
	} else {
	    gotDown = 1;
	    rowsDown--;
	    downSpanInfo.y = rpyDown;
	    newy = rpyDown - zoomy;
	    inty = (GLint) rpyDown;
	    while (rowsDown && (GLint) newy == inty) {
		downSpanInfo.y = newy;
		newy -= zoomy;
		rowsDown--;
		downSpanInfo.readY -= ySign;
	    }
	    if (inty == (GLint) newy) {
		if (gotUp) {
		    for (i=0; i<modifiers; i++) {
			if (i & 1) {
			    (*(spanInfo->spanModifier[i]))(gc, spanInfo, 
				    spanData3, spanData1);
			} else {
			    (*(spanInfo->spanModifier[i]))(gc, spanInfo, 
				    spanData1, spanData3);
			}
		    }
		    (*render)(gc, spanInfo, outSpan1);
		}
		break;
	    }
	    rpyDown = newy;
	    (*reader)(gc, &downSpanInfo, spanData2);
	    downSpanInfo.readY -= ySign;
	}

	if (gotUp) {
	    for (i=0; i<modifiers; i++) {
		if (i & 1) {
		    (*(spanInfo->spanModifier[i]))(gc, spanInfo, 
			    spanData3, spanData1);
		} else {
		    (*(spanInfo->spanModifier[i]))(gc, spanInfo, 
			    spanData1, spanData3);
		}
	    }
	    (*render)(gc, spanInfo, outSpan1);
	}

	if (gotDown) {
	    for (i=0; i<modifiers; i++) {
		if (i & 1) {
		    (*(spanInfo->spanModifier[i]))(gc, &downSpanInfo, 
			    spanData3, spanData2);
		} else {
		    (*(spanInfo->spanModifier[i]))(gc, &downSpanInfo, 
			    spanData2, spanData3);
		}
	    }
	    (*render)(gc, &downSpanInfo, outSpan2);
	}
    }

    /*
    ** Only one of the spanners is left to iterate.
    */

    while (rowsUp) {
	/* Do what is left of up spans */
	rowsUp--;
	spanInfo->y = rpyUp;
	newy = rpyUp + zoomy;
	inty = (GLint) rpyUp;
	while (rowsUp && (GLint) newy == inty) {
	    spanInfo->y = newy;
	    newy += zoomy;
	    rowsUp--;
	    spanInfo->readY += ySign;
	}
	if (inty == (GLint) newy) break;
	rpyUp = newy;

	(*reader)(gc, spanInfo, spanData1);
	for (i=0; i<modifiers; i++) {
	    if (i & 1) {
		(*(spanInfo->spanModifier[i]))(gc, spanInfo, 
			spanData3, spanData1);
	    } else {
		(*(spanInfo->spanModifier[i]))(gc, spanInfo, 
			spanData1, spanData3);
	    }
	}
	(*render)(gc, spanInfo, outSpan1);

	spanInfo->readY += ySign;
    }

    while (rowsDown) {
	/* Do what is left of down spans */
	rowsDown--;
	downSpanInfo.y = rpyDown;
	newy = rpyDown - zoomy;
	inty = (GLint) rpyDown;
	while (rowsDown && (GLint) newy == inty) {
	    downSpanInfo.y = newy;
	    newy -= zoomy;
	    rowsDown--;
	    downSpanInfo.readY -= ySign;
	}
	if (inty == (GLint) newy) break;
	rpyDown = newy;

	(*reader)(gc, &downSpanInfo, spanData2);
	for (i=0; i<modifiers; i++) {
	    if (i & 1) {
		(*(spanInfo->spanModifier[i]))(gc, &downSpanInfo, 
			spanData3, spanData2);
	    } else {
		(*(spanInfo->spanModifier[i]))(gc, &downSpanInfo, 
			spanData2, spanData3);
	    }
	}
	(*render)(gc, &downSpanInfo, outSpan2);

	downSpanInfo.readY -= ySign;
    }
}

/*
** Generic implementation of a CopyPixels picker.  Any machine specific
** implementation should provide their own.
*/
void __glSlowPickCopyPixels(__GLcontext *gc, GLint x, GLint y, GLint width,
		            GLint height, GLenum type)
{
    __GLpixelSpanInfo spanInfo;

    __glInitCopyPixelsInfo(gc, &spanInfo, x, y, width, height, type);
    if (!__glClipCopyPixels(gc, &spanInfo)) return;

    __GL_LOCK_PXL_COPY_BUFFERS(gc, type);

    __glGenericPickCopyPixels(gc, &spanInfo);

    __GL_UNLOCK_PXL_COPY_BUFFERS(gc, type);
}

/*
** Generic picker for CopyPixels.  This should be called if no machine
** specific path is provided for this specific version of CopyPixels.
*/
void __glGenericPickCopyPixels(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    void (*cpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
    __GLpixelSpanModInfo spanModInfo;

    spanModInfo.pixelPath = __GL_PIXPATH_COPYPIX;
    spanModInfo.srcData = __GL_PIXDATA_FB;
    spanModInfo.dstData = __GL_PIXDATA_FB;

    spanInfo->numSpanMods = 0;
    PickSpanModifiers(gc, spanInfo, &spanModInfo);

    if (spanInfo->filter) {
        cpfn = __glCopyPixelsConvolution;
    } else {
	switch(spanInfo->numSpanMods) {
	case 0:
	    cpfn = __glCopyPixels0;
	    break;
	case 1:
	    cpfn = __glCopyPixels1;
	    break;
	case 2:
	    cpfn = __glCopyPixels2;
	    break;
	default:
	    cpfn = __glCopyPixelsN;
	    break;
	}
    }

    (*cpfn)(gc, spanInfo);
}

/*
** A simple image copying routine with one span modifier.
*/
void __glCopyImage1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i;
    GLint height;
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);

    height = spanInfo->height;
    span1  = spanInfo->spanModifier[0];
    for (i=0; i<height; i++) {
	(*span1)(gc, spanInfo, spanInfo->srcCurrent, spanInfo->dstCurrent);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	spanInfo->dstCurrent = (GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement;
    }
}

/*
** A simple image copying routine with two span modifiers.
*/
void __glCopyImage2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i;
    GLint height;
    GLubyte spanData1[__GL_MAX_SPAN_SIZE];
    void (*span1)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);
    void (*span2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	          const GLvoid *inspan, GLvoid *outspan);

    height = spanInfo->height;
    span1 = spanInfo->spanModifier[0];
    span2 = spanInfo->spanModifier[1];
    for (i=0; i<height; i++) {
	(*span1)(gc, spanInfo, spanInfo->srcCurrent, spanData1);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	(*span2)(gc, spanInfo, spanData1, spanInfo->dstCurrent);
	spanInfo->dstCurrent = (GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement;
    }
}

/*
** A simple image copying routine for N span modifiers.
*/
void __glCopyImageN(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i;
    GLint height;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE];
    GLubyte spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;

    height = spanInfo->height;
    spanModifier = spanInfo->spanModifier;
    for (i=0; i<height; i++) {

	(*spanModifier[0])(gc, spanInfo, spanInfo->srcCurrent, spanData1);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum = 1; spanModNum < numSpanMods-1; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}
	(*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanInfo->dstCurrent);
	spanInfo->dstCurrent = (GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement;
    }
}

/*
** A simple image copying routine for convolution
*/
void __glCopyImageConvolution(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint i;
    GLint width, height;
    GLint firstSpanMod, lastSpanMod;
    GLint numSpanMods = spanInfo->numSpanMods;
    GLint convModifierIndex = spanInfo->convModifierIndex;
    GLint convInitialSpans = spanInfo->convInitialSpans;
    GLint convFinalSpans = spanInfo->convFinalSpans;
    GLint spanModNum;
    void (**spanModifier)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan);
    GLubyte spanData1[__GL_MAX_SPAN_SIZE];
    GLubyte spanData2[__GL_MAX_SPAN_SIZE];
    GLubyte *tmpSpanData, *spanDataIn, *spanDataOut;
    GLfloat spanCacheData[16][__GL_MAX_SPAN_SIZE];
    GLfloat *spanCache[16];

    for (i=0; i<16; ++i) {
        spanCache[i] = &spanCacheData[i][0];
    }
    spanInfo->spanCache = (GLvoid **) &spanCache;

    width = spanInfo->width;
    height = spanInfo->height;
    spanModifier = spanInfo->spanModifier;
    for (i=0; i<height+convFinalSpans; i++) {
	/*
	** Process each span of the source image
	*/
	spanInfo->spanCount = i;
	spanInfo->width = width;

	if (i < convInitialSpans) {
	    firstSpanMod = 0;
	    lastSpanMod = convModifierIndex;
	} else if (i >= height) {
	    firstSpanMod = convModifierIndex;
	    lastSpanMod = numSpanMods-1;
	} else {
	    firstSpanMod = 0;
	    lastSpanMod = numSpanMods-1;
	}

	(*spanModifier[firstSpanMod])(gc, spanInfo,
					spanInfo->srcCurrent, spanData1);
	spanInfo->srcCurrent = (const GLubyte *) spanInfo->srcCurrent + 
		spanInfo->srcRowIncrement;
	spanDataIn = spanData1; spanDataOut = spanData2;
	for (spanModNum=firstSpanMod+1;
			spanModNum<=lastSpanMod-1; spanModNum++) {
	    (*spanModifier[spanModNum])(gc, spanInfo, spanDataIn, spanDataOut);
	    tmpSpanData = spanDataIn;
	    spanDataIn = spanDataOut;
	    spanDataOut = tmpSpanData;
	}
	(*spanModifier[spanModNum])(gc, spanInfo,
					spanDataIn, spanInfo->dstCurrent);
	if (i < convInitialSpans) {
	    continue;
	}

	spanInfo->dstCurrent = (GLubyte *) spanInfo->dstCurrent +
		spanInfo->dstRowIncrement;
    }
}

/*
** Used to transfer 3D images.  Calls the copy image proc to transfer
** each 2D layer of the 3D image.
*/
void __glCopyImage3D(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
                        void (*cpfn)(__GLcontext *gc,
			__GLpixelSpanInfo *spanInfo)){
    GLint i;
    GLint depth;
    const GLvoid *srcCurrent;
    GLvoid *dstCurrent;

    depth = spanInfo->depth;
    srcCurrent = spanInfo->srcCurrent;
    dstCurrent = spanInfo->dstCurrent;
    for (i=0; i<depth; i++) {
        (*cpfn)(gc, spanInfo);
        srcCurrent = (const GLubyte *) srcCurrent + spanInfo->srcImageIncrement;
        spanInfo->srcCurrent = srcCurrent;
        dstCurrent = (GLubyte *) dstCurrent + spanInfo->dstImageIncrement;
        spanInfo->dstCurrent = dstCurrent;
    }
}

/*
** Internal image processing routine.  Used by GetTexImage to transfer from
** internal texture image to the user.  Used by TexImage[12]D to transfer
** from the user to internal texture.  Used for display list optimization of
** textures and DrawPixels.
**
** This routine also supports the pixel format mode __GL_RED_ALPHA which is
** basically a 2 component texture.
*/
void __glGenericPickCopyImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    __GLpixelSpanModInfo spanModInfo;
    void (*cpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

    spanModInfo.pixelPath = __GL_PIXPATH_COPYIMAGE;
    spanModInfo.srcData = __GL_PIXDATA_MEM;
    spanModInfo.dstData = __GL_PIXDATA_MEM;

    spanInfo->numSpanMods = 0;
    PickSpanModifiers(gc, spanInfo, &spanModInfo);

    if (spanInfo->filter) {
	cpfn = __glCopyImageConvolution;
    } else {
	switch(spanInfo->numSpanMods) {
	case 0:
	    cpfn = __glCopyImage1;
	    break;
	case 1:
	    cpfn = __glCopyImage1;
	    break;
	case 2:
	    cpfn = __glCopyImage2;
	    break;
	default:
	    cpfn = __glCopyImageN;
	    break;
	}
    }
    if (spanInfo->depth > 0) {
        __glCopyImage3D(gc, spanInfo, cpfn);
    } else {
        (*cpfn)(gc, spanInfo);
    }
}

void __glGenericPickStoreTexture(__GLcontext *gc, 
                                 __GLmipMapLevel *lp,
				 __GLpixelSpanInfo *spanInfo)
{
    __GLpixelSpanModInfo spanModInfo;
    void (*cpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

    spanModInfo.pixelPath = __GL_PIXPATH_COPYIMAGE;
    spanModInfo.srcData = __GL_PIXDATA_MEM;
    spanModInfo.dstData = __GL_PIXDATA_MEM;

    spanInfo->numSpanMods = 0;
    PickSpanModifiers(gc, spanInfo, &spanModInfo);
    if (lp->texFormat->store) {
	spanInfo->spanModifier[spanInfo->numSpanMods++] = lp->texFormat->store;
    }

    if (spanInfo->filter) {
	cpfn = __glCopyImageConvolution;
    } else {
	switch(spanInfo->numSpanMods) {
	case 0:
	    assert(0);
	    break;
	case 1:
	    cpfn = __glCopyImage1;
	    break;
	case 2:
	    cpfn = __glCopyImage2;
	    break;
	default:
	    cpfn = __glCopyImageN;
	    break;
	}
    }

    if (spanInfo->depth > 0) {
        __glCopyImage3D(gc, spanInfo, cpfn);
    } else {
        (*cpfn)(gc, spanInfo);
    }
}

void __glGenericPickGetTexture(__GLcontext *gc,
			       __GLmipMapLevel *lp,
			       __GLpixelSpanInfo *spanInfo)
{
    __GLpixelSpanModInfo spanModInfo;
    void (*cpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);

    spanModInfo.pixelPath = __GL_PIXPATH_COPYIMAGE;
    spanModInfo.srcData = __GL_PIXDATA_MEM;
    spanModInfo.dstData = __GL_PIXDATA_MEM;

    spanInfo->numSpanMods = 0;
    if (lp->texFormat->fetch) {
	spanInfo->spanModifier[spanInfo->numSpanMods++] = lp->texFormat->fetch;
    }
    PickSpanModifiers(gc, spanInfo, &spanModInfo);

    switch(spanInfo->numSpanMods) {
    case 0:
	assert(0);
	break;
    case 1:
	cpfn = __glCopyImage1;
	break;
    case 2:
	cpfn = __glCopyImage2;
	break;
    default:
	cpfn = __glCopyImageN;
	break;
    }

    if (spanInfo->depth > 0) {
        __glCopyImage3D(gc, spanInfo, cpfn);
    } else {
        (*cpfn)(gc, spanInfo);
    }
}

/*
** Initializes the src info for ReadImage.  Src info initialization
** is separate from dest info initialization to allow for
** different types of destinations.
*/
void __glInitReadImageSrcInfo(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				GLint x,
				GLint y,
				GLsizei width,
				GLsizei height)
{
#if defined(__GL_ALIGNED_BUFFERS)
    spanInfo->readX = (__GLfloat) (x + gc->constants.viewportXAdjustOffset);
    if (gc->constants.yInverted) {
	spanInfo->readY = (__GLfloat) ((gc->constants.height - y - 1) + 
		gc->constants.viewportYAdjustOffset);
    } else {
	spanInfo->readY = (__GLfloat) (y + gc->constants.viewportYAdjustOffset);
    }
#else /* __GL_ALIGNED_BUFFERS */
    spanInfo->readX = (__GLfloat) (x + gc->constants.viewportXAdjust);
    if (gc->constants.yInverted) {
	spanInfo->readY = (__GLfloat) ((gc->constants.height - y - 1) + 
		gc->constants.viewportYAdjust);
    } else {
	spanInfo->readY = (__GLfloat) (y + gc->constants.viewportYAdjust);
    }
#endif /* __GL_ALIGNED_BUFFERS */
    spanInfo->srcFormat = gc->modes.rgbMode ? GL_RGBA : GL_COLOR_INDEX;
    spanInfo->srcType = GL_FLOAT;
    spanInfo->width = width;
    spanInfo->height = height;
    spanInfo->zoomx = __glOne;
    spanInfo->x = __glZero;

    spanInfo->srcAlignment = 1;
    spanInfo->srcSkipImages = 0;
    spanInfo->srcSkipPixels = 0;
    spanInfo->srcSkipLines = 0;
    spanInfo->srcLsbFirst = GL_FALSE;
    spanInfo->srcSwapBytes = GL_FALSE;
    spanInfo->srcLineLength = spanInfo->width;
    spanInfo->srcImageHeight = spanInfo->height;
    /*
    ** These aren't needed for ReadImage, but they are read by the
    ** generic pixel path setup code, so something needs to be set
    ** in them.
    */
    spanInfo->srcType = GL_FLOAT;
    spanInfo->srcElementSize = 4;
    spanInfo->srcSwapBytes = GL_FALSE;
    spanInfo->srcImage = NULL;
}

/*
** Generic picker for reading from the frame buffer into an internal
** buffer.  This should be called if no machine specific path is provided 
** for this specific version.  This is used by CopyTexImage.
*/
void __glGenericPickReadImage(__GLcontext *gc, __GLmipMapLevel *lp,
			      __GLpixelSpanInfo *spanInfo)
{
    void (*rpfn)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
    __GLpixelSpanModInfo spanModInfo;

    spanModInfo.pixelPath = __GL_PIXPATH_READIMAGE;
    spanModInfo.srcData = __GL_PIXDATA_FB;
    spanModInfo.dstData = __GL_PIXDATA_MEM;

    spanInfo->numSpanMods = 0;
    PickSpanModifiers(gc, spanInfo, &spanModInfo);
    if (lp && lp->texFormat->store) {
	spanInfo->spanModifier[spanInfo->numSpanMods++] = lp->texFormat->store;
    }

    /*
    ** Pick a ReadPixels routine that uses the right number of span 
    ** modifiers.
    */

    if (spanInfo->filter) {
	rpfn = __glReadPixelsConvolution;
    } else {
	switch(spanInfo->numSpanMods) {
	  case 0:
	    rpfn = __glReadPixels0;
	    break;
	  case 1:
	    rpfn = __glReadPixels1;
	    break;
	  case 2:
	    rpfn = __glReadPixels2;
	    break;
	  default:
	    rpfn = __glReadPixelsN;
	    break;
	}
    }

    (*rpfn)(gc, spanInfo);
}


/*
** Used to set up getting data from mem objects without doing any unpacking.
*/
/*ARGSUSED*/
void __glInitMemGet(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			GLsizei width, GLsizei height, GLenum internalFormat,
			const GLvoid *buf)
{
    spanInfo->srcImage = buf;
    spanInfo->srcSkipPixels = 0;
    spanInfo->srcSkipLines = 0;
    spanInfo->srcSkipImages = 0;
    spanInfo->srcSwapBytes = GL_FALSE;
    spanInfo->srcLsbFirst = GL_TRUE;
    spanInfo->srcLineLength = width;
    spanInfo->srcImageHeight = height;

    switch(internalFormat) {
      case GL_LUMINANCE:
	spanInfo->srcFormat = GL_RED;
	spanInfo->srcType = GL_FLOAT;
	spanInfo->srcAlignment = 4;
	break;
      case GL_LUMINANCE_ALPHA:
	spanInfo->srcFormat = __GL_RED_ALPHA;
	spanInfo->srcType = GL_FLOAT;
	spanInfo->srcAlignment = 4;
	break;
      case GL_RGB:
	spanInfo->srcFormat = GL_RGB;
	spanInfo->srcType = GL_FLOAT;
	spanInfo->srcAlignment = 4;
	break;
      case GL_RGBA:
	spanInfo->srcFormat = GL_RGBA;
	spanInfo->srcType = GL_FLOAT;
	spanInfo->srcAlignment = 4;
	break;
      case GL_ALPHA:
	spanInfo->srcFormat = GL_ALPHA;
	spanInfo->srcType = GL_FLOAT;
	spanInfo->srcAlignment = 4;
	break;
      case GL_INTENSITY:
	spanInfo->srcFormat = GL_RED;
	spanInfo->srcType = GL_FLOAT;
	spanInfo->srcAlignment = 4;
	break;
      default:
	assert(0);
	break;
    }
}

/*
** Used to set up the host memory source when loading a mem object.
** "dltex" is GL_TRUE if the mem object is a display-listed texture.
*/
void __glInitMemUnpack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   GLsizei width, GLsizei height, GLsizei depth,
			   GLenum format, GLenum type, const GLvoid *buf,
			   GLboolean dltex)
{
    spanInfo->x = __glZero;
    spanInfo->zoomx = __glOne;
    spanInfo->width = width;
    spanInfo->height = height;
    spanInfo->depth = depth;
    spanInfo->srcFormat = format;
    spanInfo->srcType = type;
    spanInfo->srcImage = buf;

    /* Set the remaining source modes according to the pixel state */
    __glLoadUnpackModes(gc, spanInfo, dltex);
}

/*
** Used to set up the host memory destination when getting a mem object.
*/
void __glInitMemPack(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 GLsizei width, GLsizei height, GLsizei depth,
			 GLenum format, GLenum type, GLvoid *buf)
{
    spanInfo->x = __glZero;
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

void __glInitMemLoad(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		     GLenum internalFormat, GLvoid *buf)
{
    spanInfo->scale.r = 1.0;
    spanInfo->scale.g = 1.0;
    spanInfo->scale.b = 1.0;
    spanInfo->scale.a = 1.0;
    spanInfo->bias.r = 0.0;
    spanInfo->bias.g = 0.0;
    spanInfo->bias.b = 0.0;
    spanInfo->bias.a = 0.0;

    spanInfo->dstImage = buf;
    spanInfo->dstSkipPixels = 0;
    spanInfo->dstSkipLines = 0;
    spanInfo->dstSkipImages = 0;
    spanInfo->dstSwapBytes = GL_FALSE;
    spanInfo->dstLsbFirst = GL_TRUE;
    spanInfo->dstLineLength = spanInfo->width;

    switch(internalFormat) {
      case GL_LUMINANCE:
	spanInfo->dstFormat = GL_RED;
	spanInfo->dstType = GL_FLOAT;
	spanInfo->dstAlignment = 4;
	break;
      case GL_LUMINANCE_ALPHA:
	spanInfo->dstFormat = __GL_RED_ALPHA;
	spanInfo->dstType = GL_FLOAT;
	spanInfo->dstAlignment = 4;
	break;
      case GL_RGB:
	spanInfo->dstFormat = GL_RGB;
	spanInfo->dstType = GL_FLOAT;
	spanInfo->dstAlignment = 4;
	break;
      case GL_RGBA:
	spanInfo->dstFormat = GL_RGBA;
	spanInfo->dstType = GL_FLOAT;
	spanInfo->dstAlignment = 4;
	break;
      case GL_ALPHA:
	spanInfo->dstFormat = GL_ALPHA;
	spanInfo->dstType = GL_FLOAT;
	spanInfo->dstAlignment = 4;
	break;
      case GL_INTENSITY:
	spanInfo->dstFormat = GL_RED;
	spanInfo->dstType = GL_FLOAT;
	spanInfo->dstAlignment = 4;
	break;
      default:
	assert(0);
	break;
    }
}
