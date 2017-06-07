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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_upack.c#13 $
*/
#include "context.h"
#include "global.h"
#include "pixel.h"

/*
** This file contains routines to unpack data from the user's data space
** into a span of pixels which can then be rendered.
*/

/*
** Return the number of elements per group of a specified format
** Supports packed pixels as described in the specification, i.e.
** if the type is packed pixel, the number of elements per group
** is understood to be one.
*/
GLint __glElementsPerGroup(GLenum format, GLenum type)
{
    /*
    ** To make row length computation valid for image extraction,
    ** packed pixel types assume elements per group equals one. 
    */
    switch(type) {
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
	return 1;
      default:
	break;
    }

    switch(format) {
      case GL_RGB:
      case GL_BGR:
        return 3;
      case GL_LUMINANCE_ALPHA:
        return 2;
      case GL_RGBA:
      case GL_ABGR_EXT:
      case GL_BGRA:
        return 4;
      case __GL_RED_ALPHA:
	return 2;
      default:
        return 1;
    }
}

/*
** Return the number of bytes per element, based on the element type
** Supports packed pixels as described in the specification, i.e.
** if packed pixel type return bytes per group.
*/
GLint __glBytesPerElement(GLenum type)
{
    switch(type) {
      case GL_UNSIGNED_SHORT:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
      case __GL_UNSIGNED_SHORT_5_6_5:
      case __GL_UNSIGNED_SHORT_X_5_5_5:
      case __GL_UNSIGNED_SHORT_1_5_5_5:
      case __GL_UNSIGNED_SHORT_4_4_4_4_ARGB:
        return 2;
      case GL_UNSIGNED_BYTE:
      case GL_BYTE:
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
        return 1;
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
        return 4;
      case GL_BITMAP: /* XXX what should be done here? */
        return 1;
      default:
        return 0;
    }
}

/* ARGSUSED */
void __glInitUnpacker(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint alignment;
    GLint components;
    GLint element_size;
    GLint rowsize;
    GLint imageheight;
    GLint padding;
    GLint groupsize;
    GLint imagesize;
    GLint groups_per_line;
    GLint skip_images, skip_pixels, skip_lines;
    GLint swap_bytes;
    GLenum format, type;
    const GLvoid *pixels;

    format = spanInfo->srcFormat;
    type = spanInfo->srcType;
    pixels = spanInfo->srcImage;
    imageheight = spanInfo->srcImageHeight;
    skip_images = spanInfo->srcSkipImages;
    skip_pixels = spanInfo->srcSkipPixels;
    skip_lines = spanInfo->srcSkipLines;
    alignment = spanInfo->srcAlignment;
    swap_bytes = spanInfo->srcSwapBytes;

    components = __glElementsPerGroup(format, type);
    groups_per_line = spanInfo->srcLineLength;

    element_size = __glBytesPerElement(type);
    if (element_size == 1) swap_bytes = 0;
    groupsize = element_size * components;

    rowsize = groups_per_line * groupsize;
    if (type == GL_BITMAP) {
	rowsize = (groups_per_line + 7)/8;
    }
    padding = (rowsize % alignment);
    if (padding) {
	rowsize += alignment - padding;
    }

    imagesize = imageheight * rowsize;

    if (type == GL_BITMAP) {
	spanInfo->srcCurrent = (const GLvoid *) (((const GLubyte *) pixels) + 
		skip_lines * rowsize + skip_pixels / 8);
	spanInfo->srcStartBit = skip_pixels % 8;
    } else {
	spanInfo->srcCurrent = (const GLvoid *) (((const GLubyte *) pixels) + 
		skip_lines * rowsize +
		skip_pixels * groupsize +
		skip_images * imagesize);
    }
    spanInfo->srcRowIncrement = rowsize;
    spanInfo->srcGroupIncrement = groupsize;
    spanInfo->srcComponents = components;
    spanInfo->srcElementSize = element_size;
    spanInfo->srcImageIncrement = imagesize;

    /* Set defaults for general span modifier selection control */
    spanInfo->applySrcClamp = GL_TRUE;
    spanInfo->applyDstClamp = GL_TRUE;
    spanInfo->applyFbScale = GL_TRUE;
    spanInfo->applyPixelTransfer = GL_TRUE;
    spanInfo->applyGenericScaleBias = GL_FALSE;
    spanInfo->nonColorComp = GL_FALSE;
}

/*
** An unpacker that unpacks from BITMAP source data, into FLOAT spans.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanUnpackBitmap(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width;
    const GLvoid *userData;
    GLfloat *spanData;
    GLint lsbFirst;
    GLint startBit;
    GLint bit;
    GLubyte ubyte;
    GLshort *pixelArray;
    GLint skipCount;
    __GLfloat zero, one;

    userData = inspan;
    spanData = (GLfloat *) outspan;
    pixelArray = spanInfo->pixelArray;

    width = spanInfo->width;
    lsbFirst = spanInfo->srcLsbFirst;
    startBit = spanInfo->srcStartBit;

    i = width;
    bit = startBit;
    ubyte = *(const GLubyte *) userData;

    zero = __glZero;
    one = __glOne;

    skipCount = 1;
    if (lsbFirst) {
	switch(bit) {
	  case 1:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x02) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 2:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x04) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 3:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x08) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 4:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x10) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 5:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x20) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 6:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x40) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 7:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x80) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    i--;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	  case 0:
	    break;
	}
	while (i >= 8) {
	    ubyte = *(const GLubyte *) userData;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	    i -= 8;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x01) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x02) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x04) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x08) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x10) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x20) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x40) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x80) *spanData++ = one;
		else *spanData++ = zero;
	    }
	}
	if (i) {
	    ubyte = *(const GLubyte *) userData;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x01) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x02) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x04) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x08) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x10) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x20) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x40) *spanData++ = one;
		else *spanData++ = zero;
	    }
	}
    } else {
	switch(bit) {
	  case 1:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x40) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 2:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x20) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 3:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x10) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 4:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x08) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 5:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x04) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 6:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x02) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) break;
	  case 7:
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x01) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    i--;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	  case 0:
	    break;
	}
	while (i >= 8) {
	    ubyte = *(const GLubyte *) userData;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	    i -= 8;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x80) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x40) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x20) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x10) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x08) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x04) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x02) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x01) *spanData++ = one;
		else *spanData++ = zero;
	    }
	}
	if (i) {
	    ubyte = *(const GLubyte *) userData;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x80) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x40) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x20) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x10) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x08) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x04) *spanData++ = one;
		else *spanData++ = zero;
	    }
	    if (--i == 0) return;
	    if (--skipCount == 0) {
		skipCount = *pixelArray++;
		if (ubyte & 0x02) *spanData++ = one;
		else *spanData++ = zero;
	    }
	}
    }
}

/*
** An unpacker that unpacks from BITMAP source data, into FLOAT spans.
**
** zoomx is assumed to be less than or equal to -1.0 or greater than or
** equal to 1.0.
*/
void __glSpanUnpackBitmap2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width;
    const GLvoid *userData;
    GLfloat *spanData;
    GLint lsbFirst;
    GLint startBit;
    GLint bit;
    GLubyte ubyte;

    width = spanInfo->width;
    userData = inspan;
    spanData = (GLfloat *) outspan;

    lsbFirst = spanInfo->srcLsbFirst;
    startBit = spanInfo->srcStartBit;

    i = width;
    bit = startBit;
    ubyte = *(const GLubyte *) userData;

    if (lsbFirst) {
	switch(bit) {
	  case 1:
	    if (ubyte & 0x02) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 2:
	    if (ubyte & 0x04) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 3:
	    if (ubyte & 0x08) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 4:
	    if (ubyte & 0x10) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 5:
	    if (ubyte & 0x20) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 6:
	    if (ubyte & 0x40) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 7:
	    if (ubyte & 0x80) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    i--;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	  case 0:
	    break;
	}
	while (i >= 8) {
	    ubyte = *(const GLubyte *) userData;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	    i -= 8;
	    if (ubyte & 0x01) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x02) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x04) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x08) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x10) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x20) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x40) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x80) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	}
	if (i) {
	    ubyte = *(const GLubyte *) userData;
	    if (ubyte & 0x01) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x02) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x04) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x08) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x10) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x20) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x40) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	}
    } else {
	switch(bit) {
	  case 1:
	    if (ubyte & 0x40) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 2:
	    if (ubyte & 0x20) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 3:
	    if (ubyte & 0x10) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 4:
	    if (ubyte & 0x08) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 5:
	    if (ubyte & 0x04) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 6:
	    if (ubyte & 0x02) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) break;
	  case 7:
	    if (ubyte & 0x01) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    i--;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	  case 0:
	    break;
	}
	while (i >= 8) {
	    ubyte = *(const GLubyte *) userData;
	    userData = (const GLvoid *) ((const GLubyte *) userData + 1);
	    i -= 8;
	    if (ubyte & 0x80) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x40) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x20) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x10) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x08) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x04) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x02) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (ubyte & 0x01) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	}
	if (i) {
	    ubyte = *(const GLubyte *) userData;
	    if (ubyte & 0x80) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x40) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x20) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x10) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x08) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x04) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	    if (--i == 0) return;
	    if (ubyte & 0x02) *spanData++ = __glOne;
	    else *spanData++ = __glZero;
	}
    }
}

/*
** An unpacker that unpacks from RGB, UNSIGNED_BYTE source data, into 
** RGB, UNSIGNED_BYTE spans.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanUnpackRGBubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    const GLubyte *userData;
    GLubyte *spanData;
    GLint width;
    GLshort *pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    width = spanInfo->width;
    userData = (const GLubyte *) inspan;
    spanData = (GLubyte *) outspan;

    pixelArray = spanInfo->pixelArray;

    i = 0;
    do {
	spanData[0] = userData[0];
	spanData[1] = userData[1];
	spanData[2] = userData[2];
	spanData += 3;

	skipCount = *pixelArray++;
	userData = (const GLubyte *) ((const GLubyte *) userData + 3 * skipCount);
	i++;
    } while (i<width);
}

/*
** An unpacker that unpacks from either index, UNSIGNED_BYTE source data, 
** into UNSIGNED_BYTE spans.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanUnpackIndexUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    const GLubyte *userData;
    GLubyte *spanData;
    GLint width;
    GLshort *pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    width = spanInfo->width;
    userData = (const GLubyte *) inspan;
    spanData = (GLubyte *) outspan;
    
    pixelArray = spanInfo->pixelArray;

    i = 0;
    do {
	*spanData = *userData;
	spanData++;

	skipCount = *pixelArray++;
	userData = (const GLubyte *) ((const GLubyte *) userData + skipCount);
	i++;
    } while (i<width);
}

/*
** An unpacker that unpacks from RGBA, UNSIGNED_BYTE source data, into 
** RGBA, UNSIGNED_BYTE spans.
**
** This could be faster if we could assume that the first ubyte (red)
** was aligned on a word boundary.  Then we could just use unsigned int
** pointers to copy the user's data.  This might be a reasonable future
** optimization.
**
** zoomx is assumed to be less than 1.0 and greater than -1.0.
*/
void __glSpanUnpackRGBAubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    const GLubyte *userData;
    GLubyte *spanData;
    GLint width;
    GLshort *pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    width = spanInfo->width;
    userData = (const GLubyte *) inspan;
    spanData = (GLubyte *) outspan;

    pixelArray = spanInfo->pixelArray;

    i = 0;
    do {
	spanData[0] = userData[0];
	spanData[1] = userData[1];
	spanData[2] = userData[2];
	spanData[3] = userData[3];
	spanData += 4;

	skipCount = *pixelArray++;
	userData = (const GLubyte *) ((const GLubyte *) userData + (skipCount << 2));
	i++;
    } while (i<width);
}

/*
** Swaps bytes from an incoming span of two byte objects to an outgoing span.
** No pixel skipping is performed.
*/
void __glSpanSwapBytes2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	a = inData[0];
	b = inData[1];
	outData[0] = b;
	outData[1] = a;
	outData += 2;
	inData += 2;
    }
}

/*
** Swaps bytes from an incoming span of two byte objects to an outgoing span.
** No pixel skipping is performed.  This version is for swapping to the 
** desination image.
*/
void __glSpanSwapBytes2Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		           const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	a = inData[0];
	b = inData[1];
	outData[0] = b;
	outData[1] = a;
	outData += 2;
	inData += 2;
    }
}

/*
** Swaps bytes from an incoming span of two byte objects to an outgoing span.
** Pixel skipping is performed.
*/
void __glSpanSwapAndSkipBytes2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		               const GLvoid *inspan, GLvoid *outspan)
{
    GLint i,j;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b;
    GLint components = spanInfo->srcComponents;
    GLint groupInc = spanInfo->srcGroupIncrement;
    GLshort *pixelArray = spanInfo->pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++) {
	for (j=0; j<components; j++) {
	    a = inData[0];
	    b = inData[1];
	    outData[0] = b;
	    outData[1] = a;
	    outData += 2;
	    inData += 2;
	}

	skipCount = (*pixelArray++) - 1;
	inData = (const GLubyte *) ((const GLubyte *) inData + (skipCount * groupInc));
    }
}

/*
** Swaps bytes from an incoming span of four byte objects to an outgoing span.
** No pixel skipping is performed.
*/
void __glSpanSwapBytes4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b,c,d;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	c = inData[2];
	d = inData[3];
	a = inData[0];
	b = inData[1];
	outData[0] = d;
	outData[1] = c;
	outData[2] = b;
	outData[3] = a;
	outData += 4;
	inData += 4;
    }
}

/*
** Swaps bytes from an incoming span of four byte objects to an outgoing span.
** No pixel skipping is performed.  This version is for swapping to the 
** destination image.
*/
void __glSpanSwapBytes4Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		           const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b,c,d;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	c = inData[2];
	d = inData[3];
	a = inData[0];
	b = inData[1];
	outData[0] = d;
	outData[1] = c;
	outData[2] = b;
	outData[3] = a;
	outData += 4;
	inData += 4;
    }
}

/*
** Swaps bytes from an incoming span of four byte objects to an outgoing span.
** Pixel skipping is performed.
*/
void __glSpanSwapAndSkipBytes4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		               const GLvoid *inspan, GLvoid *outspan)
{
    GLint i,j;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b,c,d;
    GLint components = spanInfo->srcComponents;
    GLint groupInc = spanInfo->srcGroupIncrement;
    GLshort *pixelArray = spanInfo->pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++) {
	for (j=0; j<components; j++) {
	    c = inData[2];
	    d = inData[3];
	    outData[0] = d;
	    outData[1] = c;
	    a = inData[0];
	    b = inData[1];
	    outData[2] = b;
	    outData[3] = a;
	    outData += 4;
	    inData += 4;
	}

	skipCount = (*pixelArray++) - 1;
	inData = (const GLubyte *) ((const GLubyte *) inData + (skipCount * groupInc));
    }
}

/*
** A span modifier that skips pixels according to the pixel skip array.
** Components are assumed to be 1 byte in size.
*/
void __glSpanSkipPixels1(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i,j;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint groupInc = spanInfo->srcGroupIncrement;
    GLshort *pixelArray = spanInfo->pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++) {
	for (j=0; j<components; j++) {
	    *outData++ = *inData++;
	}

	skipCount = (*pixelArray++) - 1;
	inData = (const GLubyte *) ((const GLubyte *) inData + (skipCount * groupInc));
    }
}

/*
** A span modifier that skips pixels according to the pixel skip array.
** Components are assumed to be 2 bytes in size, and aligned on a half
** word boundary.
*/
void __glSpanSkipPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i,j;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint groupInc = spanInfo->srcGroupIncrement;
    GLshort *pixelArray = spanInfo->pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++) {
	for (j=0; j<components; j++) {
	    *outData++ = *inData++;
	}

	skipCount = (*pixelArray++) - 1;
	inData = (const GLushort *) ((const GLubyte *) inData + (skipCount * groupInc));
    }
}

/*
** A span modifier that skips pixels according to the pixel skip array.
** Components are assumed to be 4 bytes in size, and aligned on a word
** boundary.
*/
void __glSpanSkipPixels4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i,j;
    GLint width = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLuint *outData = (GLuint *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint groupInc = spanInfo->srcGroupIncrement;
    GLshort *pixelArray = spanInfo->pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++) {
	for (j=0; j<components; j++) {
	    *outData++ = *inData++;
	}

	skipCount = (*pixelArray++) - 1;
	inData = (const GLuint *) ((const GLubyte *) inData + (skipCount * groupInc));
    }
}

/*
** A span modifier that skips pixels according to the pixel skip array.
** Components are assumed to be 2 bytes in size.  No alignment is assumed,
** so misaligned data should use this path.
*/
void __glSpanSlowSkipPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *inspan, GLvoid *outspan)
{
    GLint i,j;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b;
    GLint components = spanInfo->srcComponents;
    GLint groupInc = spanInfo->srcGroupIncrement;
    GLshort *pixelArray = spanInfo->pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++) {
	for (j=0; j<components; j++) {
	    a = inData[0];
	    b = inData[1];
	    outData[0] = a;
	    outData[1] = b;
	    outData += 2;
	    inData += 2;
	}

	skipCount = (*pixelArray++) - 1;
	inData = ((const GLubyte *) inData + (skipCount * groupInc));
    }
}

/*
** A span modifier that skips pixels according to the pixel skip array.
** Components are assumed to be 4 bytes in size.  No alignment is assumed,
** so misaligned data should use this path.
*/
void __glSpanSlowSkipPixels4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *inspan, GLvoid *outspan)
{
    GLint i,j;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b,c,d;
    GLint components = spanInfo->srcComponents;
    GLint groupInc = spanInfo->srcGroupIncrement;
    GLshort *pixelArray = spanInfo->pixelArray;
    GLint skipCount;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++) {
	for (j=0; j<components; j++) {
	    a = inData[0];
	    b = inData[1];
	    c = inData[2];
	    d = inData[3];
	    outData[0] = a;
	    outData[1] = b;
	    outData[2] = c;
	    outData[3] = d;
	    outData += 4;
	    inData += 4;
	}

	skipCount = (*pixelArray++) - 1;
	inData = ((const GLubyte *) inData + (skipCount * groupInc));
    }
}

/*
** A span modifier that aligns pixels 2 bytes in size.  No alignment is 
** assumed, so misaligned data should use this path.
*/
void __glSpanAlignPixels2(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif

    for (i=0; i<totalSize; i++) {
	a = inData[0];
	b = inData[1];
	outData[0] = a;
	outData[1] = b;
	outData += 2;
	inData += 2;
    }
}

/*
** A span modifier that aligns pixels 2 bytes in size.  No alignment is 
** assumed, so misaligned data should use this path.  This version is for
** aligning to the destination image.
*/
void __glSpanAlignPixels2Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif

    for (i=0; i<totalSize; i++) {
	a = inData[0];
	b = inData[1];
	outData[0] = a;
	outData[1] = b;
	outData += 2;
	inData += 2;
    }
}

/*
** A span modifier that aligns pixels 4 bytes in size.  No alignment is 
** assumed, so misaligned data should use this path.
*/
void __glSpanAlignPixels4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b,c,d;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif

    for (i=0; i<totalSize; i++) {
	a = inData[0];
	b = inData[1];
	c = inData[2];
	d = inData[3];
	outData[0] = a;
	outData[1] = b;
	outData[2] = c;
	outData[3] = d;
	outData += 4;
	inData += 4;
    }
}

/*
** A span modifier that aligns pixels 4 bytes in size.  No alignment is 
** assumed, so misaligned data should use this path.  This version is 
** for swapping to the destination image.
*/
void __glSpanAlignPixels4Dst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLubyte a,b,c,d;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif

    for (i=0; i<totalSize; i++) {
	a = inData[0];
	b = inData[1];
	c = inData[2];
	d = inData[3];
	outData[0] = a;
	outData[1] = b;
	outData[2] = c;
	outData[3] = d;
	outData += 4;
	inData += 4;
    }
}

/*
** Unpacks from any component of type UNSIGNED_BYTE to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_UB_TO_FLOAT(*inData++);
    }
}

/*
** Unpacks from any component of type BYTE to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackByte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLbyte *inData = (const GLbyte *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_B_TO_FLOAT(*inData++);
    }
}

/*
** Unpacks from any component of type UNSIGNED_SHORT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_US_TO_FLOAT(*inData++);
    }
}

/*
** Unpacks from any component of type SHORT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackShort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLshort *inData = (const GLshort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_S_TO_FLOAT(*inData++);
    }
}

/*
** Unpacks from any component of type UNSIGNED_INT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_UI_TO_FLOAT(*inData++);
    }
}

/*
** Unpacks from any component of type INT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackInt(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	               const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLint *inData = (const GLint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_I_TO_FLOAT(*inData++);
    }
}

/*
** Unpacks from type 332Ubyte to a three component span of type FLOAT.
*/
void __glSpanUnpack332Ubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per byte */
    GLubyte inbyte;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	inbyte = (*inData++);
	*outData++ = (float)((inbyte & 0xE0) >> 5) / 7.0;
	*outData++ = (float)((inbyte & 0x1C) >> 2) / 7.0; 	/* 7 = 2^3-1 */
	*outData++ = (float)(inbyte & 0x03) / 3.0; 		/* 3 = 2^2-1 */
    }
}

/*
** Unpacks from type 233RevUbyte to a three component span of type FLOAT.
*/
/*ARGSUSED*/
void __glSpanUnpack233RevUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per byte */
    GLubyte inbyte;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	inbyte = (*inData++);
	*outData++ = (float)( inbyte & 0x07      ) / 7.0;
	*outData++ = (float)((inbyte & 0x38) >> 3) / 7.0; 	/* 7 = 2^3-1 */
	*outData++ = (float)((inbyte & 0xC0) >> 6) / 3.0; 	/* 3 = 2^2-1 */
    }
}

/*
** Unpacks from type 565Ushort to a three component span of type FLOAT.
*/
/*ARGSUSED*/
void __glSpanUnpack565Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++);
	*outData++ = (float)((invalue & 0xF800) >> 11) / 31.0; /* 31 = 2^5-1 */
	*outData++ = (float)((invalue & 0x07E0) >> 5) / 63.0; /* 63 = 2^6-1 */
	*outData++ = (float)(invalue & 0x001F) / 31.0; 		
    }
}

/*
** Unpacks from type 565RevUshort to a three component span of type FLOAT.
*/
/*ARGSUSED*/
void __glSpanUnpack565RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++);
	*outData++ = (float)((invalue & 0x001F)      ) / 31.0; /* 31 = 2^5-1 */
	*outData++ = (float)((invalue & 0x07E0) >>  5) / 63.0; /* 63 = 2^6-1 */
	*outData++ = (float)((invalue & 0xF800) >> 11) / 31.0; 		
    }
}

/*
** Unpacks from type 4444Ushort to a four component span of type FLOAT.
*/
void __glSpanUnpack4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++);
	*outData++ = (float)((invalue & 0xF000) >> 12) / 15.0;  /* 15 = 2^4-1 */
	*outData++ = (float)((invalue & 0x0F00) >> 8) / 15.0;
	*outData++ = (float)((invalue & 0x00F0) >> 4) / 15.0;
	*outData++ = (float)(invalue & 0x000F) / 15.0;
    }
}

/*
** Unpacks from type 4444RevUshort to a four component span of type FLOAT.
*/
/*ARGSUSED*/
void __glSpanUnpack4444RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++);
	*outData++ = (float)((invalue & 0x000F)      ) / 15.0; /* 15 = 2^4-1 */
	*outData++ = (float)((invalue & 0x00F0) >>  4) / 15.0;
	*outData++ = (float)((invalue & 0x0F00) >>  8) / 15.0;
	*outData++ = (float)((invalue & 0xF000) >> 12) / 15.0;
    }
}

/*
** Unpacks from type 4444Ushort to a four component span of type FLOAT.
*/
void __glSpanUnpackARGB4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++);
	*outData++ = (float)((invalue & 0x0F00) >> 8) / 15.0;
	*outData++ = (float)((invalue & 0x00F0) >> 4) / 15.0;
	*outData++ = (float)(invalue & 0x000F) / 15.0;
	*outData++ = (float)((invalue & 0xF000) >> 12) / 15.0;  /* 15 = 2^4-1 */
    }
}

/*
** Unpacks from type 5551Ushort to a four component span of type FLOAT.
*/
void __glSpanUnpack5551Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++);
	*outData++ = (float)((invalue & 0xF800) >> 11) / 31.0;  /* 31 = 2^5-1 */
	*outData++ = (float)((invalue & 0x07C0) >> 6) / 31.0;
	*outData++ = (float)((invalue & 0x003E) >> 1) / 31.0;
	*outData++ = (float)(invalue & 0x0001);
    }
}

/*
** Unpacks from type 1555RevUshort to a four component span of type FLOAT.
*/
/*ARGSUSED*/
void __glSpanUnpack1555RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++);
	*outData++ = (float)((invalue & 0x001F)      ) / 31.0; /* 31 = 2^5-1 */
	*outData++ = (float)((invalue & 0x03E0) >>  5) / 31.0;
	*outData++ = (float)((invalue & 0x7C00) >> 10) / 31.0;
	*outData++ = (float)((invalue & 0x8000) >> 15); 
    }
}

/*
** Unpacks from type 8888Uint to a four component span of type FLOAT.
*/
void __glSpanUnpack8888Uint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLuint invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++); 
							/* 255 = 2^8-1 */
	*outData++ = (float)((invalue & 0xFF000000) >> 24) / 255.0;
	*outData++ = (float)((invalue & 0x00FF0000) >> 16) / 255.0;
	*outData++ = (float)((invalue & 0x0000FF00) >> 8) / 255.0;
	*outData++ = (float)(invalue & 0x000000FF) / 255.0;
    }
}

/*
** Unpacks from type 8888RevUint to a four component span of type FLOAT.
*/
/*ARGSUSED*/
void __glSpanUnpack8888RevUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLuint invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++); 
							/* 255 = 2^8-1 */
	*outData++ = (float)((invalue & 0x000000FF)      ) / 255.0;
	*outData++ = (float)((invalue & 0x0000FF00) >>  8) / 255.0;
	*outData++ = (float)((invalue & 0x00FF0000) >> 16) / 255.0;
	*outData++ = (float)((invalue & 0xFF000000) >> 24) / 255.0;
    }
}

/*
** Unpacks from type 10_10_10_2_Uint to a four component span of type FLOAT.
*/
void __glSpanUnpack_10_10_10_2_Uint(__GLcontext *gc, 
				    __GLpixelSpanInfo *spanInfo, 
				    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLuint invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++); 
							/* 1023 = 2^10-1 */
	*outData++ = (float)((invalue & 0xFFC00000) >> 22) / 1023.0;
	*outData++ = (float)((invalue & 0x003FF000) >> 12) / 1023.0;
	*outData++ = (float)((invalue & 0x00000FFC) >> 2) / 1023.0;
	*outData++ = (float)(invalue & 0x00000003) / 3.0;
    }
}

/*
** Unpacks from type 2_10_10_10_RevUint to a four component span of type FLOAT.
*/
/*ARGSUSED*/
void __glSpanUnpack_2_10_10_10_RevUint(__GLcontext *gc, 
				       __GLpixelSpanInfo *spanInfo, 
				       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLuint invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++); 
							    /* 1023 = 2^10-1 */
	*outData++ = (float)((invalue & 0x000003FF)      ) / 1023.0;
	*outData++ = (float)((invalue & 0x000FFC00) >> 10) / 1023.0;
	*outData++ = (float)((invalue & 0x3FF00000) >> 20) / 1023.0;
	*outData++ = (float)((invalue & 0xC0000000) >> 30) / 3.0;
	                                                     /* 3.0 = 2^2-1 */ 
    }
}

/*
** Unpacks from type X555Ushort to a three component span of type FLOAT.
*/
void __glSpanUnpackX555Ushort(__GLcontext *gc, 
			      __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++); 
							/* 31 = 2^5-1 */
	*outData++ = (float)((invalue & 0x7C00) >> 10) / 31.0;
	*outData++ = (float)((invalue & 0x03E0) >> 5) / 31.0;
	*outData++ = (float)(invalue & 0x001F) / 31.0;
    }
}

/*
** Unpacks from type 1555Ushort to a four component span of type FLOAT.
*/
void __glSpanUnpack1555Ushort(__GLcontext *gc, 
			      __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLushort invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = (*inData++); 
							/* 31 = 2^5-1 */
	*outData++ = (float)((invalue & 0x7C00) >> 10) / 31.0;
	*outData++ = (float)((invalue & 0x03E0) >> 5) / 31.0;
	*outData++ = (float)(invalue & 0x001F) / 31.0;
	*outData++ = (float)((invalue & 0x8000) >> 15);
    }
}

/*
** Unpacks from any index of type UNSIGNED_BYTE to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackUbyteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLfloat *outData = (GLfloat *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = *inData++;
    }
}

/*
** Unpacks from any index of type BYTE to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackByteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLbyte *inData = (const GLbyte *) inspan;
    GLfloat *outData = (GLfloat *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = *inData++;
    }
}

/*
** Unpacks from any index of type UNSIGNED_SHORT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackUshortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLushort *inData = (const GLushort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = *inData++;
    }
}

/*
** Unpacks from any index of type SHORT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackShortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLshort *inData = (const GLshort *) inspan;
    GLfloat *outData = (GLfloat *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = *inData++;
    }
}

/*
** Unpacks from any index of type UNSIGNED_INT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackUintI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = *inData++;
    }
}

/*
** Unpacks from any index of type INT to a span of the same
** format of type FLOAT.
*/
void __glSpanUnpackIntI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLint *inData = (const GLint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = *inData++;
    }
}

/*
** Unpacks from any values of type UNSIGNED_INT to a span of the same
** format of type FLOAT.  This one is used for nonColorComp (ie non-color
** values, eg hgram), so the scale to 0..1 range is not done.
*/
void __glSpanUnpackNonCompUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLuint *inData = (const GLuint *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLfloat)(*inData++);
    }
}

/*
** Clamps from any type FLOAT to a span of the same format of type FLOAT.
*/
void __glSpanClampFloat(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLint components = spanInfo->srcComponents;
    GLint totalSize = width * components;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat r, one, zero;

    one = __glOne;
    zero = __glZero;
    for (i=0; i<totalSize; i++) {
	r = *inData++;
	if (r > one) r = one;
	else if (r < zero) r = zero;
	*outData++ = r;
    }
}

void __glSpanClampRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat r, rs, zero;

    rs = gc->frontBuffer.redScale;
    zero = __glZero;
    for (i=0; i<width; i++) {
	r = *inData++;
	if (r > rs) r = rs; /* assumes red scale is positive */
	else if (r < zero) r = zero;
	*outData++ = r;
	*outData++ = *inData++;
	*outData++ = *inData++;
	*outData++ = *inData++;
    }
}

void __glSpanClampGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat g, gs, zero;

    gs = gc->frontBuffer.greenScale;
    zero = __glZero;
    for (i=0; i<width; i++) {
	*outData++ = *inData++;
	g = *inData++;
	if (g > gs) g = gs; /* assumes green scale is positive */
	else if (g < zero) g = zero;
	*outData++ = g;
	*outData++ = *inData++;
	*outData++ = *inData++;
    }
}

void __glSpanClampBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat b, bs, zero;

    bs = gc->frontBuffer.blueScale;
    zero = __glZero;
    for (i=0; i<width; i++) {
	*outData++ = *inData++;
	*outData++ = *inData++;
	b = *inData++;
	if (b > bs) b = bs; /* assumes blue scale is positive */
	else if (b < zero) b = zero;
	*outData++ = b;
	*outData++ = *inData++;
    }
}

void __glSpanClampAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat a, as, zero;

    as = gc->frontBuffer.alphaScale;
    zero = __glZero;
    for (i=0; i<width; i++) {
	*outData++ = *inData++;
	*outData++ = *inData++;
	*outData++ = *inData++;
	a = *inData++;
	if (a > as) a = as; /* assumes alpha scale is positive */
	else if (a < zero) a = zero;
	*outData++ = a;
    }
}


void __glSpanClampRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat r, g, b, rs, gs, bs, zero;

    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;

    zero = __glZero;
    for (i=0; i<width; i++) {
	r = *inData++;
	if (r > rs) r = rs; /* assumes red scale is positive */
	else if (r < zero) r = zero;
	*outData++ = r;

	g = *inData++;
	if (g > gs) g = gs; /* assumes green scale is positive */
	else if (g < zero) g = zero;
	*outData++ = g;

	b = *inData++;
	if (b > bs) b = bs; /* assumes blue scale is positive */
	else if (b < zero) b = zero;
	*outData++ = b;
	*outData++ = *inData++;
    }
}


void __glSpanClampRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData;
    GLfloat *outData;
    GLfloat r, g, b, a, rs, gs, bs, as, zero;

    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;
    as = gc->frontBuffer.alphaScale;

    zero = __glZero;
    for (i=0; i<width; i++) {
	r = *inData++;
	if (r > rs) r = rs; /* assumes red scale is positive */
	else if (r < zero) r = zero;
	*outData++ = r;

	g = *inData++;
	if (g > gs) g = gs; /* assumes green scale is positive */
	else if (g < zero) g = zero;
	*outData++ = g;

	b = *inData++;
	if (b > bs) b = bs; /* assumes blue scale is positive */
	else if (b < zero) b = zero;
	*outData++ = b;

	a = *inData++;
	if (a > as) a = as; /* assumes alpha scale is positive */
	else if (a < zero) a = zero;
	*outData++ = a;
    }
}


/*
** Clamps from any type FLOAT to a span of the same format of type FLOAT.
*/
void __glSpanClampFloatDst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat r, one, zero;

    one = __glOne;
    zero = __glZero;
    for (i=0; i<totalSize; i++) {
	r = *inData++;
	if (r > one) r = one;
	else if (r < zero) r = zero;
	*outData++ = r;
    }
}


/*
** Clamps from a signed FLOAT [-1, 1] to a span of the same format of type 
** FLOAT.
*/
/* ARGSUSED */
void __glSpanClampSignedDst(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat r, zero;

    zero = __glZero;
    for (i=0; i<totalSize; i++) {
	r = *inData++;
	if (r < zero) r = zero;
	*outData++ = r;
    }
}


/*
** Expands and scales a RED, FLOAT span into a RGBA, FLOAT span.
*/
void __glSpanExpandRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat *outData = (GLfloat *) outspan;
    GLfloat zero = __glZero;
    GLfloat as = gc->frontBuffer.alphaScale;
    GLfloat rs = gc->frontBuffer.redScale;

    for (i=0; i<width; i++) {
	*outData++ = *inData++ * rs;	/* Red */
	*outData++ = zero;		/* Green */
	*outData++ = zero;		/* Blue */
	*outData++ = as;		/* Alpha */
    }
}

/*
** Expands and scales a GREEN, FLOAT span into a RGBA, FLOAT span.
*/
void __glSpanExpandGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat zero = __glZero;
    GLfloat gs = gc->frontBuffer.greenScale;
    GLfloat as = gc->frontBuffer.alphaScale;

    for (i=0; i<width; i++) {
	*outData++ = zero;		/* Red */
	*outData++ = *inData++ * gs;	/* Green */
	*outData++ = zero;		/* Blue */
	*outData++ = as;		/* Alpha */
    }
}

/*
** Expands and scales a BLUE, FLOAT span into a RGBA, FLOAT span.
*/
void __glSpanExpandBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat zero = __glZero;
    GLfloat bs = gc->frontBuffer.blueScale;
    GLfloat as = gc->frontBuffer.alphaScale;

    for (i=0; i<width; i++) {
	*outData++ = zero;		/* Red */
	*outData++ = zero;		/* Green */
	*outData++ = *inData++ * bs;	/* Blue */
	*outData++ = as;		/* Alpha */
    }
}

/*
** Expands and scales an ALPHA, FLOAT span into a RGBA, FLOAT span. 
*/
void __glSpanExpandAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat zero = __glZero;
    GLfloat as = gc->frontBuffer.alphaScale;

    for (i=0; i<width; i++) {
	*outData++ = zero;		/* Red */
	*outData++ = zero;		/* Green */
	*outData++ = zero;		/* Blue */
	*outData++ = *inData++ * as;	/* Alpha */
    }
}

/*
** Expands and scales an RGB, FLOAT span into a RGBA, FLOAT span. 
*/
void __glSpanExpandRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat r,g,b;
    GLfloat rs,gs,bs;
    GLfloat as = gc->frontBuffer.alphaScale;

    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;

    for (i=0; i<width; i++) {
	r = *inData++ * rs;
	g = *inData++ * gs;
	b = *inData++ * bs;
	*outData++ = r;
	*outData++ = g;
	*outData++ = b;
	*outData++ = as;		/* Alpha */
    }
}

/*
** Expands and scales a BGR, FLOAT span into a RGBA, FLOAT span. 
*/
void __glSpanExpandBGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat r,g,b;
    GLfloat rs,gs,bs;
    GLfloat as = gc->frontBuffer.alphaScale;

    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;

    for (i=0; i<width; i++) {
	b = *inData++ * bs;
	g = *inData++ * gs;
	r = *inData++ * rs;
	*outData++ = r;
	*outData++ = g;
	*outData++ = b;
	*outData++ = as;		/* Alpha */
    }
}

/*
** Expands and scales a LUMINANCE, FLOAT span into a RGBA, FLOAT span. 
*/
void __glSpanExpandLuminance(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;
    GLfloat rs,gs,bs;
    GLfloat as = gc->frontBuffer.alphaScale;

    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp * rs;		/* Red */
	*outData++ = comp * gs;		/* Green */
	*outData++ = comp * bs;		/* Blue */
	*outData++ = as;		/* Alpha */
    }
}

/*
** Expands and scales a LUMINANCE_ALPHA, FLOAT span into a RGBA, FLOAT span. 
*/
void __glSpanExpandLuminanceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		                  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;
    GLfloat rs,gs,bs;
    GLfloat as = gc->frontBuffer.alphaScale;

    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp * rs;		/* Red */
	*outData++ = comp * gs;		/* Green */
	*outData++ = comp * bs;		/* Blue */
	*outData++ = *inData++ * as;	/* Alpha */
    }
}

/*
** Expands and scales a __GL_RED_ALPHA, FLOAT span into a RGBA, FLOAT span. 
*/
void __glSpanExpandRedAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;
    GLfloat zero;
    GLfloat rs;
    GLfloat as = gc->frontBuffer.alphaScale;

    rs = gc->frontBuffer.redScale;
    zero = __glZero;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp * rs;		/* Red */
	*outData++ = zero;
	*outData++ = zero;
	*outData++ = *inData++ * as;	/* Alpha */
    }
}

/*
** Expands and scales a INTENSITY, FLOAT span into a RGBA, FLOAT span. 
** This is only used when expanding from an internal format.
*/
void __glSpanExpandIntensity(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;
    GLfloat rs,gs,bs;
    GLfloat as = gc->frontBuffer.alphaScale;

    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp * rs;		/* Red */
	*outData++ = comp * gs;		/* Green */
	*outData++ = comp * bs;		/* Blue */
	*outData++ = comp * as;		/* Alpha */
    }
}

/*
** Expands and scales a RED, FLOAT span into a RGBA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanExpandRedNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat zero = __glZero;

    for (i=0; i<width; i++) {
	*outData++ = *inData++;		/* Red */
	*outData++ = zero;		/* Green */
	*outData++ = zero;		/* Blue */
	*outData++ = 1.0;		/* Alpha */
    }
}

/*
** Expands and scales a GREEN, FLOAT span into a RGBA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanExpandGreenNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat zero = __glZero;

    for (i=0; i<width; i++) {
	*outData++ = zero;		/* Red */
	*outData++ = *inData++;		/* Green */
	*outData++ = zero;		/* Blue */
	*outData++ = 1.0;		/* Alpha */
    }
}

/*
** Expands and scales a BLUE, FLOAT span into a RGBA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanExpandBlueNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat zero = __glZero;

    for (i=0; i<width; i++) {
	*outData++ = zero;		/* Red */
	*outData++ = zero;		/* Green */
	*outData++ = *inData++;		/* Blue */
	*outData++ = 1.0;		/* Alpha */
    }
}

/*
** Expands and scales an ALPHA, FLOAT span into a RGBA, FLOAT span. 
*/
/*ARGSUSED*/
void __glSpanExpandAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat zero = __glZero;

    for (i=0; i<width; i++) {
	*outData++ = zero;		/* Red */
	*outData++ = zero;		/* Green */
	*outData++ = zero;		/* Blue */
	*outData++ = *inData++;		/* Alpha */
    }
}

/*
** Expands and scales an RGB, FLOAT span into a RGBA, FLOAT span. 
*/
/*ARGSUSED*/
void __glSpanExpandRGBNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;

    for (i=0; i<width; i++) {
	*outData++ = *inData++;
	*outData++ = *inData++;
	*outData++ = *inData++;
	*outData++ = 1.0;		/* Alpha */
    }
}

/*
** Expands and scales an BGR, FLOAT span into a RGBA, FLOAT span. 
*/
/*ARGSUSED*/
void __glSpanExpandBGRNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat r,g,b;

    for (i=0; i<width; i++) {
	b = *inData++;
	g = *inData++;
	r = *inData++;
	*outData++ = r;
	*outData++ = g;
	*outData++ = b;
	*outData++ = 1.0;		/* Alpha */
    }
}

/*
** Expands and scales a LUMINANCE, FLOAT span into a RGBA, FLOAT span. 
*/
/*ARGSUSED*/
void __glSpanExpandLuminanceNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp;		/* Red */
	*outData++ = comp;		/* Green */
	*outData++ = comp;		/* Blue */
	*outData++ = 1.0;		/* Alpha */
    }
}

/*
** Expands and scales a LUMINANCE_ALPHA, FLOAT span into a RGBA, FLOAT span. 
*/
/*ARGSUSED*/
void __glSpanExpandLuminanceAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
				    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp;		/* Red */
	*outData++ = comp;		/* Green */
	*outData++ = comp;		/* Blue */
	*outData++ = *inData++;		/* Alpha */
    }
}

/*
** Expands and scales a __GL_RED_ALPHA, FLOAT span into a RGBA, FLOAT span. 
*/
/*ARGSUSED*/
void __glSpanExpandRedAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;
    GLfloat zero;

    zero = __glZero;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp;		/* Red */
	*outData++ = zero;
	*outData++ = zero;
	*outData++ = *inData++;		/* Alpha */
    }
}

/*
** Expands and scales a INTENSITY, FLOAT span into a RGBA, FLOAT span. 
** This is only used when expanding from an internal format.
*/
/*ARGSUSED*/
void __glSpanExpandIntensityNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat comp;

    for (i=0; i<width; i++) {
	comp = *inData++;
	*outData++ = comp;		/* Red */
	*outData++ = comp;		/* Green */
	*outData++ = comp;		/* Blue */
	*outData++ = comp;		/* Alpha */
    }
}

/*
** The only span format supported by this routine is GL_RGBA, GL_FLOAT.
** The span is simply scaled by the frame buffer scaling factors.
*/
void __glSpanScaleRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i, width;
    GLfloat rscale, gscale, bscale, ascale;
    GLfloat red, green, blue, alpha;
    const GLfloat *inData;
    GLfloat *outData;

    width = spanInfo->width;
    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rscale = gc->frontBuffer.redScale;
    gscale = gc->frontBuffer.greenScale;
    bscale = gc->frontBuffer.blueScale;
    ascale = gc->frontBuffer.alphaScale;
    for (i=0; i<width; i++) {
	red = *inData++ * rscale;
	green = *inData++ * gscale;
	*outData++ = red;
	*outData++ = green;
	blue = *inData++ * bscale;
	alpha = *inData++ * ascale;
	*outData++ = blue;
	*outData++ = alpha;
    }
}

/*
** The span format of incoming data is GL_ABGR, GL_FLOAT.  The format
** is changed to GL_RGBA, GL_FLOAT.
** The span is scaled by the frame buffer scaling factors.
*/
void __glSpanScaleABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i, width;
    GLfloat rscale, gscale, bscale, ascale;
    GLfloat red, green, blue, alpha;
    const GLfloat *inData;
    GLfloat *outData;

    width = spanInfo->width;
    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rscale = gc->frontBuffer.redScale;
    gscale = gc->frontBuffer.greenScale;
    bscale = gc->frontBuffer.blueScale;
    ascale = gc->frontBuffer.alphaScale;
    for (i=0; i<width; i++) {
	alpha = *inData++ * ascale;
	blue = *inData++ * bscale;
	green = *inData++ * gscale;
	red = *inData++ * rscale;
	*outData++ = red;
	*outData++ = green;
	*outData++ = blue;
	*outData++ = alpha;
    }
}


/*
** The span format of incoming data is GL_BGRA, GL_FLOAT.  The format
** is changed to GL_RGBA, GL_FLOAT.
** The span is scaled by the frame buffer scaling factors.
*/
void __glSpanScaleBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i, width;
    GLfloat rscale, gscale, bscale, ascale;
    GLfloat red, green, blue, alpha;
    const GLfloat *inData;
    GLfloat *outData;

    width = spanInfo->width;
    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rscale = gc->frontBuffer.redScale;
    gscale = gc->frontBuffer.greenScale;
    bscale = gc->frontBuffer.blueScale;
    ascale = gc->frontBuffer.alphaScale;
    for (i=0; i<width; i++) {
	blue = *inData++ * bscale;
	green = *inData++ * gscale;
	red = *inData++ * rscale;
	alpha = *inData++ * ascale;
	*outData++ = red;
	*outData++ = green;
	*outData++ = blue;
	*outData++ = alpha;
    }
}


/*
** Reorders a ABGR, FLOAT span into a RGBA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanPreReorderABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat red, green, blue, alpha;

    for (i=0; i<width; i++) {
	alpha = *inData++;
	blue = *inData++;
	green = *inData++;
	red = *inData++;
	*outData++ = red;
	*outData++ = green;
	*outData++ = blue;
	*outData++ = alpha;
    }
}

/*
** Reorders a BGRA, FLOAT span into a RGBA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanPreReorderBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat red, green, blue, alpha;

    for (i=0; i<width; i++) {
	blue = *inData++;
	green = *inData++;
	red = *inData++;
	alpha = *inData++;
	*outData++ = red;
	*outData++ = green;
	*outData++ = blue;
	*outData++ = alpha;
    }
}

/*
** The only span format supported by this routine is GL_RGBA, GL_FLOAT.
** The span is simply unscaled by the frame buffer scaling factors.
*/
void __glSpanPreUnscaleRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i, width;
    GLfloat rscale, gscale, bscale, ascale;
    GLfloat red, green, blue, alpha;
    const GLfloat *inData;
    GLfloat *outData;

    width = spanInfo->width;
    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rscale = gc->frontBuffer.oneOverRedScale;
    gscale = gc->frontBuffer.oneOverGreenScale;
    bscale = gc->frontBuffer.oneOverBlueScale;
    ascale = gc->frontBuffer.oneOverAlphaScale;
    for (i=0; i<width; i++) {
	red = *inData++ * rscale;
	green = *inData++ * gscale;
	*outData++ = red;
	*outData++ = green;
	blue = *inData++ * bscale;
	alpha = *inData++ * ascale;
	*outData++ = blue;
	*outData++ = alpha;
    }
}


/*
** The only span format supported by this routine is GL_RGBA, GL_FLOAT.
** The span is simply unscaled by the frame buffer scaling factors.
*/
void __glSpanPostUnscaleRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i, width;
    GLfloat rscale, gscale, bscale, ascale;
    GLfloat red, green, blue, alpha;
    const GLfloat *inData;
    GLfloat *outData;

    width = spanInfo->width;
    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rscale = gc->frontBuffer.oneOverRedScale;
    gscale = gc->frontBuffer.oneOverGreenScale;
    bscale = gc->frontBuffer.oneOverBlueScale;
    ascale = gc->frontBuffer.oneOverAlphaScale;
    for (i=0; i<width; i++) {
	red = *inData++ * rscale;
	green = *inData++ * gscale;
	*outData++ = red;
	*outData++ = green;
	blue = *inData++ * bscale;
	alpha = *inData++ * ascale;
	*outData++ = blue;
	*outData++ = alpha;
    }
}

/*
** The span format of incoming data is GL_RGBA, GL_FLOAT.  The format
** is changed to GL_ABGR, GL_FLOAT.
** The span is simply unscaled by the frame buffer scaling factors.
*/
void __glSpanUnscaleABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i, width;
    GLfloat rscale, gscale, bscale, ascale;
    GLfloat red, green, blue, alpha;
    const GLfloat *inData;
    GLfloat *outData;

    width = spanInfo->width;
    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rscale = gc->frontBuffer.oneOverRedScale;
    gscale = gc->frontBuffer.oneOverGreenScale;
    bscale = gc->frontBuffer.oneOverBlueScale;
    ascale = gc->frontBuffer.oneOverAlphaScale;
    for (i=0; i<width; i++) {
	red = *inData++ * rscale;
	green = *inData++ * gscale;
	blue = *inData++ * bscale;
	alpha = *inData++ * ascale;
	*outData++ = alpha;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
    }
}


/*
** The span format of incoming data is GL_RGBA, GL_FLOAT.  The format
** is changed to GL_BGRA, GL_FLOAT.
** The span is simply unscaled by the frame buffer scaling factors.
*/
void __glSpanUnscaleBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i, width;
    GLfloat rscale, gscale, bscale, ascale;
    GLfloat red, green, blue, alpha;
    const GLfloat *inData;
    GLfloat *outData;

    width = spanInfo->width;
    inData = (const GLfloat *) inspan;
    outData = (GLfloat *) outspan;

    rscale = gc->frontBuffer.oneOverRedScale;
    gscale = gc->frontBuffer.oneOverGreenScale;
    bscale = gc->frontBuffer.oneOverBlueScale;
    ascale = gc->frontBuffer.oneOverAlphaScale;
    for (i=0; i<width; i++) {
	red = *inData++ * rscale;
	green = *inData++ * gscale;
	blue = *inData++ * bscale;
	alpha = *inData++ * ascale;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
	*outData++ = alpha;
    }
}


