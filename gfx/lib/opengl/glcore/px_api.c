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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_api.c#10 $
*/

#include "context.h"
#include "global.h"
#include "pixel.h"
#include "g_imfncs.h"

/*
** Initialize pixel map with default size and value.
*/
void __glInitDefaultPixelMap(__GLcontext *gc, GLenum map)
{
    __GLpixelState *ps = &gc->state.pixel;
    __GLpixelMapHead *pMap = gc->pixel.pixelMap;
    GLint index = map - GL_PIXEL_MAP_I_TO_I;
    
    switch (map) {
      case GL_PIXEL_MAP_I_TO_I:
      case GL_PIXEL_MAP_S_TO_S:
	/*
	** Allocate single-entry map for index type.
	*/
	if (!(pMap[index].base.mapI = (GLint*)
	      (*gc->imports.malloc)(gc, sizeof(GLint)))) {
	    return;
	} else {
	    pMap[index].base.mapI[0] = 0;
	    pMap[index].size = 1;
	}
	break;
      case GL_PIXEL_MAP_I_TO_R: case GL_PIXEL_MAP_I_TO_G:
      case GL_PIXEL_MAP_I_TO_B: case GL_PIXEL_MAP_I_TO_A:
      case GL_PIXEL_MAP_R_TO_R: case GL_PIXEL_MAP_G_TO_G:
      case GL_PIXEL_MAP_B_TO_B: case GL_PIXEL_MAP_A_TO_A:
	/*
	** Allocate single-entry map for component type.
	*/
	if (!(pMap[index].base.mapF = (__GLfloat*)
	      (*gc->imports.malloc)(gc, sizeof(__GLfloat)))) {
	    return;
	} else {
	    pMap[index].base.mapF[0] = __glZero;
	    pMap[index].size = 1;
	}
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void __glPixelSetColorScales(__GLcontext *gc)
{
    __GLpixelMachine *pm = &gc->pixel;
    GLfloat redScale, greenScale, blueScale, alphaScale;
    int i;
    GLint mask;

    if (pm->redMap == NULL) {
	/* First time allocation of these maps */

	/* 
	** These lookup tables are for type UNSIGNED_BYTE, so they are sized
	** to 256 entries.  They map from UNSIGNED_BYTE to internal scaled 
	** floating point colors.
	*/
	pm->redMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->greenMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->blueMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->alphaMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->iMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
    }

    redScale = gc->frontBuffer.redScale / 255;
    greenScale = gc->frontBuffer.greenScale / 255;
    blueScale = gc->frontBuffer.blueScale / 255;
    alphaScale = gc->frontBuffer.alphaScale / 255;
    mask = gc->frontBuffer.redMax;
    for (i=0; i<256; i++) {
	pm->redMap[i] = i * redScale;
	pm->greenMap[i] = i * greenScale;
	pm->blueMap[i] = i * blueScale;
	pm->alphaMap[i] = i * alphaScale;
	pm->iMap[i] = (GLfloat) (i & mask);
    }

    /*
    ** Invalidate the RGBA modify tables so that they will be
    ** recomputed using the current color buffer scales.
    */
    pm->rgbaCurrent = GL_FALSE;
}

/************************************************************************/

void __glEarlyInitPixelState(__GLcontext *gc)
{
    __glEarlyInitColorTableState(gc);
    __glEarlyInitConvolutionState(gc);
}

void __glFreePixelState(__GLcontext *gc)
{
    __GLpixelState *ps = &gc->state.pixel;
    __GLpixelMachine *pm = &gc->pixel;
    __GLpixelMapHead *pMap = pm->pixelMap;
    GLenum m;
    GLint i;

    /*
    ** Free memory allocated to pixel maps.
    */
    for (m = GL_PIXEL_MAP_I_TO_I; m <= GL_PIXEL_MAP_A_TO_A; m++) {
	i = m - GL_PIXEL_MAP_I_TO_I;
	if (pMap[i].base.mapI) {
	    (*gc->imports.free)(gc, pMap[i].base.mapI);
	    pMap[i].base.mapI = 0;
	}
    }
    (*gc->imports.free)(gc, pm->redMap);
    (*gc->imports.free)(gc, pm->greenMap);
    (*gc->imports.free)(gc, pm->blueMap);
    (*gc->imports.free)(gc, pm->alphaMap);
    (*gc->imports.free)(gc, pm->iMap);
    if (pm->redModMap) {
	(*gc->imports.free)(gc, pm->redModMap);
	(*gc->imports.free)(gc, pm->greenModMap);
	(*gc->imports.free)(gc, pm->blueModMap);
	(*gc->imports.free)(gc, pm->alphaModMap);
    }
    if (pm->iToRMap) {
	(*gc->imports.free)(gc, pm->iToRMap);
	(*gc->imports.free)(gc, pm->iToGMap);
	(*gc->imports.free)(gc, pm->iToBMap);
	(*gc->imports.free)(gc, pm->iToAMap);
    }
    if (pm->iToIMap) {
	(*gc->imports.free)(gc, pm->iToIMap);
    }

    __glFreeColorTableState(gc);
    __glFreeConvolutionState(gc);
    __glFreeHistogramState(gc);
    __glFreeMinmaxState(gc);
}

void __glInitPixelState(__GLcontext *gc)
{
    __GLpixelState *ps = &gc->state.pixel;
    GLenum m;

    /*
    ** Initialize transfer mode.
    */
    ps->transferMode.scale.r = __glOne;
    ps->transferMode.scale.g = __glOne;
    ps->transferMode.scale.b = __glOne;
    ps->transferMode.scale.a = __glOne;
    ps->transferMode.bias.r = __glZero;
    ps->transferMode.bias.g = __glZero;
    ps->transferMode.bias.b = __glZero;
    ps->transferMode.bias.a = __glZero;

    ps->transferMode.depthScale = __glOne;
    ps->transferMode.depthBias = __glZero;

    ps->transferMode.postConvolutionScale.r = __glOne;
    ps->transferMode.postConvolutionScale.g = __glOne;
    ps->transferMode.postConvolutionScale.b = __glOne;
    ps->transferMode.postConvolutionScale.a = __glOne;
    ps->transferMode.postConvolutionBias.r = __glZero;
    ps->transferMode.postConvolutionBias.g = __glZero;
    ps->transferMode.postConvolutionBias.b = __glZero;
    ps->transferMode.postConvolutionBias.a = __glZero;

    ps->transferMode.postColorMatrixScale.r = __glOne;
    ps->transferMode.postColorMatrixScale.g = __glOne;
    ps->transferMode.postColorMatrixScale.b = __glOne;
    ps->transferMode.postColorMatrixScale.a = __glOne;
    ps->transferMode.postColorMatrixBias.r = __glZero;
    ps->transferMode.postColorMatrixBias.g = __glZero;
    ps->transferMode.postColorMatrixBias.b = __glZero;
    ps->transferMode.postColorMatrixBias.a = __glZero;
    
    ps->transferMode.zoomX = __glOne;
    ps->transferMode.zoomY = __glOne;

    /*
    ** Initialize pixel maps with default sizes and values.
    */
    for (m = GL_PIXEL_MAP_I_TO_I; m <= GL_PIXEL_MAP_A_TO_A; m++) {
	__glInitDefaultPixelMap(gc, m);
    }

    /*
    ** Initialize store mode.
    */
    gc->clientPixel.packModes.alignment = 4;
    gc->clientPixel.unpackModes.alignment = 4;

    /* Setup to use the correct read buffer */
    if (gc->modes.doubleBufferMode) {
	ps->readBuffer = GL_BACK;
    } else {
	ps->readBuffer = GL_FRONT;
    }
    ps->readBufferReturn = ps->readBuffer;

    __glInitColorTableState(gc);
    __glInitConvolutionState(gc);
    __glInitHistogramState(gc);
    __glInitMinmaxState(gc);

    __glPixelSetColorScales(gc);
}

/************************************************************************/

/*
** Specify modes that control the storage format of pixel arrays.
*/
void GLAPI __glim_PixelStoref(GLenum mode, GLfloat value)
{
    switch (mode) {
      case GL_PACK_ROW_LENGTH:
      case GL_PACK_IMAGE_HEIGHT:
      case GL_PACK_SKIP_ROWS:
      case GL_PACK_SKIP_PIXELS:
      case GL_PACK_SKIP_IMAGES:
      case GL_PACK_ALIGNMENT:
      case GL_UNPACK_ROW_LENGTH:
      case GL_UNPACK_IMAGE_HEIGHT:
      case GL_UNPACK_SKIP_ROWS:
      case GL_UNPACK_SKIP_PIXELS:
      case GL_UNPACK_SKIP_IMAGES:
      case GL_UNPACK_ALIGNMENT:
	/* Round */
	if (value < 0) {
	    __glim_PixelStorei(mode, (GLint) (value - (__GLfloat) 0.5)); 
	} else {
	    __glim_PixelStorei(mode, (GLint) (value + (__GLfloat) 0.5));  
	}
	break;
      case GL_PACK_SWAP_BYTES:
      case GL_PACK_LSB_FIRST:
      case GL_UNPACK_SWAP_BYTES:
      case GL_UNPACK_LSB_FIRST:
	if (value == __glZero) {
	    __glim_PixelStorei(mode, GL_FALSE);
	} else {
	    __glim_PixelStorei(mode, GL_TRUE);
	}
	break;
      default:
	__glim_PixelStorei(mode, (GLint) value);
	break;
    }
}

void GLAPI __glim_PixelStorei(GLenum mode, GLint value)
{
    __GLclientPixelState *ps;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    ps = &gc->clientPixel;

    switch (mode) {
      case GL_PACK_ROW_LENGTH:
	if (value < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	ps->packModes.lineLength = value;
	break;
      case GL_PACK_IMAGE_HEIGHT:
        if (value < 0) {
            __glSetError(GL_INVALID_VALUE);
            return;
        }
        ps->packModes.imageHeight = value;
        break;
      case GL_PACK_SKIP_ROWS:
	if (value < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	ps->packModes.skipLines = value;
	break;
      case GL_PACK_SKIP_PIXELS:
	if (value < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	ps->packModes.skipPixels = value;
	break;
      case GL_PACK_SKIP_IMAGES:
        if (value < 0) {
            __glSetError(GL_INVALID_VALUE);
            return;
        }
        ps->packModes.skipImages = value;
        break;
      case GL_PACK_ALIGNMENT:
	switch (value) {
	  case 1: case 2: case 4: case 8:
	    ps->packModes.alignment = value;
	    break;
	  default:
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	break;
      case GL_PACK_SWAP_BYTES:
	ps->packModes.swapEndian = (value != 0);
	break;
      case GL_PACK_LSB_FIRST:
	ps->packModes.lsbFirst = (value != 0);
	break;

      case GL_UNPACK_ROW_LENGTH:
	if (value < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	ps->unpackModes.lineLength = value;
	break;
      case GL_UNPACK_IMAGE_HEIGHT:
        if (value < 0) {
            __glSetError(GL_INVALID_VALUE);
            return;
        }
        ps->unpackModes.imageHeight = value;
        break;
      case GL_UNPACK_SKIP_ROWS:
	if (value < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	ps->unpackModes.skipLines = value;
	break;
      case GL_UNPACK_SKIP_PIXELS:
	if (value < 0) {
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	ps->unpackModes.skipPixels = value;
	break;
      case GL_UNPACK_SKIP_IMAGES:
        if (value < 0) {
            __glSetError(GL_INVALID_VALUE);
            return;
        }
        ps->unpackModes.skipImages = value;
        break;
      case GL_UNPACK_ALIGNMENT:
	switch (value) {
	  case 1: case 2: case 4: case 8:
	    ps->unpackModes.alignment = value;
	    break;
	  default:
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	break;
      case GL_UNPACK_SWAP_BYTES:
	ps->unpackModes.swapEndian = (value != 0);
	break;
      case GL_UNPACK_LSB_FIRST:
	ps->unpackModes.lsbFirst = (value != 0);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

/*
** Specify zoom factor that affects drawing and copying of pixel arrays
*/
void GLAPI __glim_PixelZoom(GLfloat xfactor, GLfloat yfactor)
{
    __GLpixelState *ps;
    GLfloat xtemp, ytemp;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    ps = &gc->state.pixel;

    /* Round xfactor and yfactor to fixed point accuracy. */
    if (xfactor > 0) {
	xtemp = (xfactor / gc->constants.viewportEpsilon) + __glHalf;
    } else {
	xtemp = (xfactor / gc->constants.viewportEpsilon) - __glHalf;
    }
    if (yfactor > 0) {
	ytemp = (yfactor / gc->constants.viewportEpsilon) + __glHalf;
    } else {
	ytemp = (yfactor / gc->constants.viewportEpsilon) - __glHalf;
    }
    xfactor = xtemp * gc->constants.viewportEpsilon;
    yfactor = ytemp * gc->constants.viewportEpsilon;

    ps->transferMode.zoomX = xfactor;
    ps->transferMode.zoomY = yfactor;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

/*
** Specify modes that control the transfer of pixel arrays.
*/
void GLAPI __glim_PixelTransferf(GLenum mode, GLfloat value)
{
    __GLpixelState *ps;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    ps = &gc->state.pixel;

    switch (mode) {
      case GL_RED_SCALE:
	ps->transferMode.scale.r = value;
	break;
      case GL_GREEN_SCALE:
	ps->transferMode.scale.g = value;
	break;
      case GL_BLUE_SCALE:
	ps->transferMode.scale.b = value;
	break;
      case GL_ALPHA_SCALE:
	ps->transferMode.scale.a = value;
	break;
      case GL_RED_BIAS:
	ps->transferMode.bias.r = value;
	break;
      case GL_GREEN_BIAS:
	ps->transferMode.bias.g = value;
	break;
      case GL_BLUE_BIAS:
	ps->transferMode.bias.b = value;
	break;
      case GL_ALPHA_BIAS:
	ps->transferMode.bias.a = value;
	break;
      case GL_DEPTH_SCALE:
	ps->transferMode.depthScale = value;
	break;
      case GL_DEPTH_BIAS:
	ps->transferMode.depthBias = value;
	break;
      case GL_POST_CONVOLUTION_RED_SCALE:
	ps->transferMode.postConvolutionScale.r = value;
	break;
      case GL_POST_CONVOLUTION_GREEN_SCALE:
	ps->transferMode.postConvolutionScale.g = value;
	break;
      case GL_POST_CONVOLUTION_BLUE_SCALE:
	ps->transferMode.postConvolutionScale.b = value;
	break;
      case GL_POST_CONVOLUTION_ALPHA_SCALE:
	ps->transferMode.postConvolutionScale.a = value;
	break;
      case GL_POST_CONVOLUTION_RED_BIAS:
	ps->transferMode.postConvolutionBias.r = value;
	break;
      case GL_POST_CONVOLUTION_GREEN_BIAS:
	ps->transferMode.postConvolutionBias.g = value;
	break;
      case GL_POST_CONVOLUTION_BLUE_BIAS:
	ps->transferMode.postConvolutionBias.b = value;
	break;
      case GL_POST_CONVOLUTION_ALPHA_BIAS:
	ps->transferMode.postConvolutionBias.a = value;
	break;
      case GL_POST_COLOR_MATRIX_RED_SCALE:
	ps->transferMode.postColorMatrixScale.r = value;
	break;
      case GL_POST_COLOR_MATRIX_GREEN_SCALE:
	ps->transferMode.postColorMatrixScale.g = value;
	break;
      case GL_POST_COLOR_MATRIX_BLUE_SCALE:
	ps->transferMode.postColorMatrixScale.b = value;
	break;
      case GL_POST_COLOR_MATRIX_ALPHA_SCALE:
	ps->transferMode.postColorMatrixScale.a = value;
	break;
      case GL_POST_COLOR_MATRIX_RED_BIAS:
	ps->transferMode.postColorMatrixBias.r = value;
	break;
      case GL_POST_COLOR_MATRIX_GREEN_BIAS:
	ps->transferMode.postColorMatrixBias.g = value;
	break;
      case GL_POST_COLOR_MATRIX_BLUE_BIAS:
	ps->transferMode.postColorMatrixBias.b = value;
	break;
      case GL_POST_COLOR_MATRIX_ALPHA_BIAS:
	ps->transferMode.postColorMatrixBias.a = value;
	break;
      case GL_INDEX_SHIFT:
	/* Round */
        if (value > 0) {
	    ps->transferMode.indexShift = (GLint) (value + __glHalf);
	} else {
	    ps->transferMode.indexShift = (GLint) (value - __glHalf);
	}
	break;
      case GL_INDEX_OFFSET:
        /* Round */
	if (value > 0) {
	    ps->transferMode.indexOffset = (GLint) (value + __glHalf);
	} else {
	    ps->transferMode.indexOffset = (GLint) (value - __glHalf);
	}
	break;
      case GL_MAP_COLOR:
	ps->transferMode.mapColor = (value != __glZero);
	break;
      case GL_MAP_STENCIL:
	ps->transferMode.mapStencil = (value != __glZero);
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void GLAPI __glim_PixelTransferi( GLenum mode, GLint value)
{
    __glim_PixelTransferf(mode, (GLfloat) value);
}

/************************************************************************/

/*
** Functions to specify mapping of pixel colors and stencil values.
*/
void GLAPI __glim_PixelMapfv(GLenum map, GLint mapSize,
		       const GLfloat values[])
{
    __GLpixelState *ps;
    __GLpixelMapHead *pMap;
    GLint index = map - GL_PIXEL_MAP_I_TO_I;
    GLfloat value;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    ps = &gc->state.pixel;
    pMap = gc->pixel.pixelMap;

    switch (map) {
      case GL_PIXEL_MAP_I_TO_I:
      case GL_PIXEL_MAP_S_TO_S:
	if (mapSize <= 0 || (mapSize & (mapSize - 1))) {
	    /*
	    ** Maps indexed by color or stencil index must be sized
	    ** to a power of two.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	if (pMap[index].base.mapI) {
	    (*gc->imports.free)(gc, pMap[index].base.mapI);
	    pMap[index].base.mapI = 0;
	}
	pMap[index].base.mapI = (GLint*)
	    (*gc->imports.malloc)(gc, (size_t) (mapSize * sizeof(GLint)));
	if (!pMap[index].base.mapI) {
	    pMap[index].size = 0;
	    return;
	}
	pMap[index].size = mapSize;
	while (--mapSize >= 0) {
	    value = values[mapSize];
	    if (value > 0) {		/* round! */
		pMap[index].base.mapI[mapSize] =
		    (GLint)(value + __glHalf);
	    } else {
		pMap[index].base.mapI[mapSize] =
		    (GLint)(value - __glHalf);
	    }
	}
	break;
      case GL_PIXEL_MAP_I_TO_R:
      case GL_PIXEL_MAP_I_TO_G:
      case GL_PIXEL_MAP_I_TO_B:
      case GL_PIXEL_MAP_I_TO_A:
	if (mapSize <= 0 || (mapSize & (mapSize - 1))) {
	    /*
	    ** Maps indexed by color or stencil index must be sized
	    ** to a power of two.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
      case GL_PIXEL_MAP_R_TO_R:
      case GL_PIXEL_MAP_G_TO_G:
      case GL_PIXEL_MAP_B_TO_B:
      case GL_PIXEL_MAP_A_TO_A:
	if (mapSize < 0) {
	    /*
	    ** Maps indexed by color component must not have negative size.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	if (pMap[index].base.mapF) {
	    (*gc->imports.free)(gc, pMap[index].base.mapF);
	    pMap[index].base.mapF = 0;
	}
	if (mapSize == 0) {
	    __glInitDefaultPixelMap(gc, map);
	} else {
	    pMap[index].base.mapF = (__GLfloat*)
		(*gc->imports.malloc)(gc,
				      (size_t) (mapSize * sizeof(__GLfloat)));
	    if (!pMap[index].base.mapF) {
		pMap[index].size = 0;
		return;
	    }
	    pMap[index].size = mapSize;
	    while (--mapSize >= 0) {
		value = values[mapSize];
		if (value < __glZero) value = __glZero;
		else if (value > __glOne) value = __glOne;
		pMap[index].base.mapF[mapSize] = value;
	    }
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_PixelMapuiv(GLenum map, GLint mapSize,
			const GLuint values[])
{
    __GLpixelState *ps;
    __GLpixelMapHead *pMap;
    GLint index = map - GL_PIXEL_MAP_I_TO_I;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    ps = &gc->state.pixel;
    pMap = gc->pixel.pixelMap;

    switch (map) {
      case GL_PIXEL_MAP_I_TO_I:
      case GL_PIXEL_MAP_S_TO_S:
	if (mapSize <= 0 || (mapSize & (mapSize - 1))) {
	    /*
	    ** Maps indexed by color or stencil index must be sized
	    ** to a power of two.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	if (pMap[index].base.mapI) {
	    (*gc->imports.free)(gc, pMap[index].base.mapI);
	    pMap[index].base.mapI = 0;
	}
	pMap[index].base.mapI = (GLint*)
	    (*gc->imports.malloc)(gc, (size_t) (mapSize * sizeof(GLint)));
	if (!pMap[index].base.mapI) {
	    pMap[index].size = 0;
	    return;
	}
	pMap[index].size = mapSize;
	while (--mapSize >= 0) {
	    pMap[index].base.mapI[mapSize] = values[mapSize];
	}
	break;
      case GL_PIXEL_MAP_I_TO_R:
      case GL_PIXEL_MAP_I_TO_G:
      case GL_PIXEL_MAP_I_TO_B:
      case GL_PIXEL_MAP_I_TO_A:
	if (mapSize <= 0 || (mapSize & (mapSize - 1))) {
	    /*
	    ** Maps indexed by color or stencil index must be sized
	    ** to a power of two.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
      case GL_PIXEL_MAP_R_TO_R:
      case GL_PIXEL_MAP_G_TO_G:
      case GL_PIXEL_MAP_B_TO_B:
      case GL_PIXEL_MAP_A_TO_A:
	if (mapSize < 0) {
	    /*
	    ** Maps indexed by color component must not have negative size.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	if (pMap[index].base.mapF) {
	    (*gc->imports.free)(gc, pMap[index].base.mapF);
	    pMap[index].base.mapF = 0;
	}
	if (mapSize == 0) {
	    __glInitDefaultPixelMap(gc, map);
	} else {
	    pMap[index].base.mapF = (__GLfloat*)
		(*gc->imports.malloc)(gc, (size_t) (mapSize * sizeof(GLfloat)));
	    if (!pMap[index].base.mapF) {
		pMap[index].size = 0;
		return;
	    }
	    pMap[index].size = mapSize;
	    while (--mapSize >= 0) {
		pMap[index].base.mapF[mapSize] = 
			__GL_UI_TO_FLOAT(values[mapSize]);
	    }
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

void GLAPI __glim_PixelMapusv(GLenum map, GLint mapSize,
			const GLushort values[])
{
    __GLpixelState *ps;
    __GLpixelMapHead *pMap;
    GLint index = map - GL_PIXEL_MAP_I_TO_I;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    ps = &gc->state.pixel;
    pMap = gc->pixel.pixelMap;

    switch (map) {
      case GL_PIXEL_MAP_I_TO_I:
      case GL_PIXEL_MAP_S_TO_S:
	if (mapSize <= 0 || (mapSize & (mapSize - 1))) {
	    /*
	    ** Maps indexed by color or stencil index must be sized
	    ** to a power of two.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	if (pMap[index].base.mapI) {
	    (*gc->imports.free)(gc, pMap[index].base.mapI);
	    pMap[index].base.mapI = 0;
	}
	pMap[index].base.mapI = (GLint*)
	    (*gc->imports.malloc)(gc, (size_t) (mapSize * sizeof(GLint)));
	if (!pMap[index].base.mapI) {
	    pMap[index].size = 0;
	    return;
	}
	pMap[index].size = mapSize;
	while (--mapSize >= 0) {
	    pMap[index].base.mapI[mapSize] = values[mapSize];
	}
	break;
      case GL_PIXEL_MAP_I_TO_R:
      case GL_PIXEL_MAP_I_TO_G:
      case GL_PIXEL_MAP_I_TO_B:
      case GL_PIXEL_MAP_I_TO_A:
	if (mapSize <= 0 || (mapSize & (mapSize - 1))) {
	    /*
	    ** Maps indexed by color or stencil index must be sized
	    ** to a power of two.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
      case GL_PIXEL_MAP_R_TO_R:
      case GL_PIXEL_MAP_G_TO_G:
      case GL_PIXEL_MAP_B_TO_B:
      case GL_PIXEL_MAP_A_TO_A:
	if (mapSize < 0) {
	    /*
	    ** Maps indexed by color component must not have negative size.
	    */
	    __glSetError(GL_INVALID_VALUE);
	    return;
	}
	if (pMap[index].base.mapF) {
	    (*gc->imports.free)(gc, pMap[index].base.mapF);
	    pMap[index].base.mapF = 0;
	}
	if (mapSize == 0) {
	    __glInitDefaultPixelMap(gc, map);
	} else {
	    pMap[index].base.mapF = (__GLfloat*)
		(*gc->imports.malloc)(gc, (size_t) (mapSize * sizeof(GLfloat)));
	    if (!pMap[index].base.mapF) {
		pMap[index].size = 0;
		return;
	    }
	    pMap[index].size = mapSize;
	    while (--mapSize >= 0) {
		pMap[index].base.mapF[mapSize] = 
			__GL_US_TO_FLOAT(values[mapSize]);
	    }
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
}

/*
** Specify buffer from which pixels are read (another transfer mode).
*/
void GLAPI __glim_ReadBuffer(GLenum mode)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    switch (mode) {
      case GL_FRONT:
      case GL_LEFT:
      case GL_FRONT_LEFT:
	gc->state.pixel.readBuffer = GL_FRONT;
	break;
      case GL_BACK:
      case GL_BACK_LEFT:
	if (!gc->modes.doubleBufferMode) {
	    goto invalid_buffer;
	}
	gc->state.pixel.readBuffer = GL_BACK;
	break;
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3:
aux_4_through_246:
	{
	    GLint i = mode - GL_AUX0;
	    if (i >= gc->modes.numAuxBuffers) {
		goto invalid_buffer;
	    }
	    gc->state.pixel.readBuffer = mode;
	}
	break;
      case GL_FRONT_RIGHT:
      case GL_BACK_RIGHT:
      case GL_RIGHT:
invalid_buffer:
	__glSetError(GL_INVALID_OPERATION);
	return;
      default:
	if (mode > GL_AUX3 && mode < GL_AUX0+247) {
	    goto aux_4_through_246;
	}
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.pixel.readBufferReturn = mode;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_GENERIC | __GL_DIRTY_PIXEL);
}

GLboolean __glCheckDrawPixelArgs(__GLcontext *gc, GLsizei width, GLsizei height,
				 GLenum format, GLenum type)
{
    GLboolean index;

    if ((width < 0) || (height < 0)) {
	__glSetError(GL_INVALID_VALUE);
	return GL_FALSE;
    }
    switch (format) {
      case GL_STENCIL_INDEX:
	if (!gc->modes.haveStencilBuffer) {
	    __glSetError(GL_INVALID_OPERATION);
	    return GL_FALSE;
	}
      case GL_COLOR_INDEX:
	index = GL_TRUE;
	break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_RGBA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
	if (gc->modes.colorIndexMode) {
	    /* Can't convert RGB to color index */
	    __glSetError(GL_INVALID_OPERATION);
	    return GL_FALSE;
	}
      case GL_DEPTH_COMPONENT:
	index = GL_FALSE;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
    }

    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __glSetError(GL_INVALID_ENUM);
	    return GL_FALSE;
	}
	break;
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
	    return GL_FALSE;
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
	    return GL_FALSE;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean __glCheckReadPixelArgs(__GLcontext *gc, GLsizei width, GLsizei height,
				 GLenum format, GLenum type)
{
    if ((width < 0) || (height < 0)) {
	__glSetError(GL_INVALID_VALUE);
	return GL_FALSE;
    }
    switch (format) {
      case GL_STENCIL_INDEX:
	if (!gc->modes.haveStencilBuffer) {
	    __glSetError(GL_INVALID_OPERATION);
	    return GL_FALSE;
	}
	break;
      case GL_COLOR_INDEX:
	if (gc->modes.rgbMode) {
	    /* Can't convert RGB to color index */
	    __glSetError(GL_INVALID_OPERATION);
	    return GL_FALSE;
	}
	break;
      case GL_DEPTH_COMPONENT:
	if (!gc->modes.haveDepthBuffer) {
	    __glSetError(GL_INVALID_OPERATION);
	    return GL_FALSE;
	}
	break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_RGBA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
    }
    switch (type) {
      case GL_BITMAP:
	if (format != GL_STENCIL_INDEX && format != GL_COLOR_INDEX) {
	    __glSetError(GL_INVALID_OPERATION);
	    return GL_FALSE;
	}
	break;
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
	    return GL_FALSE;
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
	    return GL_FALSE;
	}
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return GL_FALSE;
    }
    return GL_TRUE;
}

void GLAPI __glim_DrawPixels(GLsizei width, GLsizei height, GLenum format,
		       GLenum type, const GLvoid *pixels)
{
    GLuint beginMode;
    __GL_SETUP();

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	    glDrawPixels(width,height,format,type,pixels);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    if (!__glCheckDrawPixelArgs(gc, width, height, format, type)) return;
    if (!gc->state.current.validRasterPos) {
	return;
    }

    if (gc->renderMode == GL_FEEDBACK) {
	__glFeedbackDrawPixels(gc, &gc->state.current.rasterPos);
	return;
    }

    if (gc->renderMode != GL_RENDER) return;

    __GL_API_PIXEL_OP();
    (*gc->procs.drawPixels)(gc, width, height, format, type, pixels, GL_FALSE);
}

void GLAPI __glim_ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, 
		       GLenum format, GLenum type, GLvoid *buf)
{
    GLuint beginMode;
    __GL_SETUP();

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	    glReadPixels(x,y,width,height,format,type,buf);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    if (!__glCheckReadPixelArgs(gc, width, height, format, type)) return;

    __GL_API_PIXEL_OP();
    (*gc->procs.readPixels)(gc, x, y, width, height, format, type, buf);
}

void GLAPI __glim_CopyPixels(GLint x, GLint y, GLsizei width, GLsizei height,
		       GLenum type)
{
    GLenum format;
    GLuint beginMode;
    __GL_SETUP();

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	    glCopyPixels(x,y,width,height,type);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    if ((width < 0) || (height < 0)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    switch (type) {
      case GL_STENCIL:
	if (!gc->modes.haveStencilBuffer) {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	format = GL_STENCIL_INDEX;
	break;
      case GL_COLOR:
	if (gc->modes.rgbMode) {
	    format = GL_RGBA;
	} else {
	    format = GL_COLOR_INDEX;
	}
	break;
      case GL_DEPTH:
	if (!gc->modes.haveDepthBuffer) {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	format = GL_DEPTH_COMPONENT;
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if (!gc->state.current.validRasterPos) {
	return;
    }

    if (gc->renderMode == GL_FEEDBACK) {
	__glFeedbackCopyPixels(gc, &gc->state.current.rasterPos);
	return;
    }

    if (gc->renderMode != GL_RENDER) return;

    __GL_API_PIXEL_OP();
    (*gc->procs.copyPixels)(gc, x, y, width, height, format);
}
