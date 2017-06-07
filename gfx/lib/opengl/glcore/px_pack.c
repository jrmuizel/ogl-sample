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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_pack.c#10 $
*/

#include <math.h>
#include "context.h"
#include "global.h"
#include "pixel.h"
#include "g_imports.h"

/*
** This file contains span packers.  A span packer takes a span of source
** data, and packs its contents into the user's data space.
**
** The packer is expected to aquire information about store modes from
** the __GLpixelSpanInfo structure.
*/

/* ARGSUSED */
void __glInitPacker(__GLcontext *gc, __GLpixelSpanInfo *spanInfo)
{
    GLint alignment;
    GLint components;
    GLint element_size;
    GLint rowsize;
    GLint padding;
    GLint groupsize;
    GLint groups_per_line;
    GLint imagesize, imageheight;
    GLint skip_images, skip_pixels, skip_lines;
    GLint swap_bytes;
    GLenum format, type;
    GLvoid *pixels;
    
    format = spanInfo->dstFormat;
    type = spanInfo->dstType;
    pixels = spanInfo->dstImage;
    imageheight = spanInfo->dstImageHeight;
    skip_images = spanInfo->dstSkipImages;
    skip_pixels = spanInfo->dstSkipPixels;
    skip_lines = spanInfo->dstSkipLines;
    alignment = spanInfo->dstAlignment;
    swap_bytes = spanInfo->dstSwapBytes;

    components = __glElementsPerGroup(format, type);
    groups_per_line = spanInfo->dstLineLength;

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
	spanInfo->dstCurrent = (GLvoid *) (((GLubyte*) pixels) + 
		skip_lines * rowsize + skip_pixels / 8);
	spanInfo->dstStartBit = skip_pixels % 8;
    } else {
	spanInfo->dstCurrent = (GLvoid *) (((GLubyte*) pixels) + 
		skip_lines * rowsize +
		skip_pixels * groupsize +
		skip_images * imagesize);
    }
    spanInfo->dstRowIncrement = rowsize;
    spanInfo->dstGroupIncrement = groupsize;
    spanInfo->dstComponents = components;
    spanInfo->dstElementSize = element_size;
    spanInfo->dstImageIncrement = imagesize;

    /* Set defaults for general span modifier selection control */
    spanInfo->applySrcClamp = GL_TRUE;
    spanInfo->applyDstClamp = GL_TRUE;
    spanInfo->applyFbScale = GL_TRUE;
    spanInfo->applyPixelTransfer = GL_TRUE;
    spanInfo->applyGenericScaleBias = GL_FALSE;
    spanInfo->nonColorComp = GL_FALSE;
}

/*
** Reduces and unscales a RGBA, FLOAT span into a RED, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReduceRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;

    for (i=0; i<width; i++) {
	*outData++ = *inData * rs;
	inData += 4;
    }
}

/*
** Reduces and unscales a RGBA, FLOAT span into a GREEN, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReduceGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;

    inData++;	/* Skip first red */
    for (i=0; i<width; i++) {
	*outData++ = *inData * gs;
	inData += 4;
    }
}

/*
** Reduces and unscales a RGBA, FLOAT span into a BLUE, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReduceBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;

    inData += 2;	/* Skip first red, green */
    for (i=0; i<width; i++) {
	*outData++ = *inData * bs;
	inData += 4;
    }
}

/*
** Reduces and unscales a RGBA, FLOAT span into a ALPHA, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReduceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    inData += 3;	/* Skip first red, green, blue */
    for (i=0; i<width; i++) {
	*outData++ = *inData * as;
	inData += 4;
    }
}

/*
** Reduces and unscales a RGBA, FLOAT span into a RGB, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReduceRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat red, green, blue;

    for (i=0; i<width; i++) {
	red = *inData++ * rs;
	green = *inData++ * gs;
	blue = *inData++ * bs;
	*outData++ = red;
	*outData++ = green;
	*outData++ = blue;
	inData++;
    }
}


/*
** Reduces and unscales a RGBA, FLOAT span into a BGR, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReduceBGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat red, green, blue;

    for (i=0; i<width; i++) {
	red = *inData++ * rs;
	green = *inData++ * gs;
	blue = *inData++ * bs;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
	inData++;
    }
}


/*
** Reorders and unscales a RGBA, FLOAT span into a ABGR, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReorderABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;
    GLfloat red, green, blue, alpha;

    for (i=0; i<width; i++) {
	red = *inData++ * rs;
	green = *inData++ * gs;
	blue = *inData++ * bs;
	alpha = *inData++ * as;
	*outData++ = alpha;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
    }
}

/*
** Reorders and unscales a RGBA, FLOAT span into a BGRA, FLOAT span, unscaling
** as it goes.
*/
void __glSpanReorderBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;
    GLfloat red, green, blue, alpha;

    for (i=0; i<width; i++) {
	red = *inData++ * rs;
	green = *inData++ * gs;
	blue = *inData++ * bs;
	alpha = *inData++ * as;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
	*outData++ = alpha;
    }
}

/*
** Reduces and unscales a RGBA, FLOAT span into a LUMINANCE, FLOAT span, 
** unscaling as it goes.
*/
void __glSpanReduceLuminance(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat l, one;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;

    one = __glOne;

    for (i=0; i<width; i++) {
	l = inData[0] * rs + inData[1] * gs + inData[2] * bs;
	if (l > one) l = one;
	*outData++ = l;
	inData += 4;
    }
}

/*
** Reduces and unscales a RGBA, FLOAT span into a LUMINANCE_ALPHA, FLOAT span,
** unscaling as it goes.
*/
void __glSpanReduceLuminanceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		                  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat l, one;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat bs = gc->frontBuffer.oneOverBlueScale;
    GLfloat gs = gc->frontBuffer.oneOverGreenScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    one = __glOne;

    for (i=0; i<width; i++) {
	l = inData[0] * rs + inData[1] * gs + inData[2] * bs;
	if (l > one) l = one;
	*outData++ = l; 
	inData += 3;
	*outData++ = *inData++ * as;
    }
}

/*
** Reduces and unscales a RGBA, FLOAT span into a __GL_RED_ALPHA, FLOAT span,
** unscaling as it goes.
*/
void __glSpanReduceRedAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat rs = gc->frontBuffer.oneOverRedScale;
    GLfloat as = gc->frontBuffer.oneOverAlphaScale;

    for (i=0; i<width; i++) {
	*outData++ = *inData++ * rs;
	inData += 2;
	*outData++ = *inData++ * as;
    }
}

/*
** Reduces a RGBA, FLOAT span into a RED, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReduceRedNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;

    for (i=0; i<width; i++) {
	*outData++ = *inData;
	inData += 4;
    }
}

/*
** Reduces a RGBA, FLOAT span into a GREEN, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReduceGreenNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;

    inData++;	/* Skip first red */
    for (i=0; i<width; i++) {
	*outData++ = *inData;
	inData += 4;
    }
}

/*
** Reduces a RGBA, FLOAT span into a BLUE, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReduceBlueNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;

    inData += 2;	/* Skip first red, green */
    for (i=0; i<width; i++) {
	*outData++ = *inData;
	inData += 4;
    }
}

/*
** Reduces a RGBA, FLOAT span into a ALPHA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReduceAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;

    inData += 3;	/* Skip first red, green, blue */
    for (i=0; i<width; i++) {
	*outData++ = *inData;
	inData += 4;
    }
}

/*
** Reduces a RGBA, FLOAT span into a RGB, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReduceRGBNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat red, green, blue;

    for (i=0; i<width; i++) {
	red = *inData++;
	green = *inData++;
	blue = *inData++;
	*outData++ = red;
	*outData++ = green;
	*outData++ = blue;
	inData++;
    }
}


/*
** Reduces a RGBA, FLOAT span into a BGR, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReduceBGRNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat red, green, blue;

    for (i=0; i<width; i++) {
	red = *inData++;
	green = *inData++;
	blue = *inData++;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
	inData++;
    }
}


/*
** Reorders a RGBA, FLOAT span into a ABGR, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReorderABGRNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat red, green, blue, alpha;

    for (i=0; i<width; i++) {
	red = *inData++;
	green = *inData++;
	blue = *inData++;
	alpha = *inData++;
	*outData++ = alpha;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
    }
}

/*
** Reorders a RGBA, FLOAT span into a BGRA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReorderBGRANS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLfloat red, green, blue, alpha;

    for (i=0; i<width; i++) {
	red = *inData++;
	green = *inData++;
	blue = *inData++;
	alpha = *inData++;
	*outData++ = blue;
	*outData++ = green;
	*outData++ = red;
	*outData++ = alpha;
    }
}

/*
** Reduces a RGBA, FLOAT span into a __GL_RED_ALPHA, FLOAT span.
*/
/*ARGSUSED*/
void __glSpanReduceRedAlphaNS(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    GLfloat *outData = (GLfloat *) outspan;
    const GLfloat *inData = (const GLfloat *) inspan;

    for (i=0; i<width; i++) {
	*outData++ = *inData++;
	inData += 2;
	*outData++ = *inData++;
    }
}

/*
** Packs to any component of type UNSIGNED_BYTE from a span of the same
** format of type FLOAT.
*/
void __glSpanPackUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	               const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_FLOAT_TO_UB(*inData++);
    }
}

/*
** Packs to any component of type BYTE from a span of the same
** format of type FLOAT.
*/
void __glSpanPackByte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	              const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLbyte *outData = (GLbyte *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_FLOAT_TO_B(*inData++);
    }
}

/*
** Packs to any component of type UNSIGNED_SHORT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_FLOAT_TO_US(*inData++);
    }
}

/*
** Packs to any component of type SHORT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackShort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	               const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLshort *outData = (GLshort *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_FLOAT_TO_S(*inData++);
    }
}

/*
** Packs to any component of type UNSIGNED_INT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	              const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLuint *outData = (GLuint *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_FLOAT_TO_UI(*inData++);
    }
}

/*
** Packs to any component of type INT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackInt(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	             const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLint *outData = (GLint *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = __GL_FLOAT_TO_I(*inData++);
    }
}


/*
** Packs from a three component span of type FLOAT to type 332Ubyte.
*/
void __glSpanPack332Ubyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per byte */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLubyte)(invalue * 7) <<5) & 0xE0;
	invalue = *inData++;
	*outData |= ((GLubyte)(invalue * 7) <<2) & 0x1C;
	invalue = *inData++;
	*outData |= ((GLubyte)(invalue * 3)) & 0x03;
	outData++;
    }
}

/*
** Packs from a three component span of type FLOAT to type 233RevUbyte.
*/
/*ARGSUSED*/
void __glSpanPack233RevUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per byte */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLubyte)(invalue * 7)) & 0x07;
	invalue = *inData++;
	*outData |= ((GLubyte)(invalue * 7) <<3) & 0x38;
	invalue = *inData++;
	*outData |= ((GLubyte)(invalue * 3) <<6) & 0xC0;
	outData++;
    }
}

/*
** Packs from a three component span of type FLOAT to type 565Ushort.
*/
/*ARGSUSED*/
void __glSpanPack565Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 31) <<11) & 0xF800;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 63) <<5) & 0x07E0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31)) & 0x001F;
	outData++;
    }
}

/*
** Packs from a three component span of type FLOAT to type 565RevUshort.
*/
/*ARGSUSED*/
void __glSpanPack565RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 31)       ) & 0x001F;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 63) <<  5) & 0x07E0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) << 11) & 0xF800;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 4444Ushort.
*/
void __glSpanPack4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 15) <<12) & 0xF000;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15) <<8) & 0x0F00;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15) <<4) & 0x00F0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15)) & 0x000F;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 4444RevUshort.
*/
/*ARGSUSED*/
void __glSpanPack4444RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 15)       ) & 0x000F;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15) <<  4) & 0x00F0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15) <<  8) & 0x0F00;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15) << 12) & 0xF000;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 4444Ushort.
*/
void __glSpanPackARGB4444Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
				const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 15) <<8) & 0x0F00;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15) <<4) & 0x00F0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15)) & 0x000F;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 15) <<12) & 0xF000;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 5551Ushort.
*/
void __glSpanPack5551Ushort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 31) <<11) & 0xF800;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) <<6) & 0x07C0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) <<1) & 0x003E;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue)) & 0x0001;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 1555RevUshort.
*/
/*ARGSUSED*/
void __glSpanPack1555RevUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 31)       ) & 0x001F;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) <<  5) & 0x03E0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) << 10) & 0x7C00;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue)      << 15) & 0x8000;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 8888Uint.
*/
void __glSpanPack8888Uint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLuint *outData = (GLuint *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLuint)(invalue * 255) <<24) & 0xFF000000;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 255) <<16) & 0x00FF0000;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 255) <<8) & 0x0000FF00;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 255)) & 0x000000FF;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 8888RevUint.
*/
/*ARGSUSED*/
void __glSpanPack8888RevUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLuint *outData = (GLuint *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLuint)(invalue * 255)       ) & 0x000000FF;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 255) <<  8) & 0x0000FF00;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 255) << 16) & 0x00FF0000;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 255) << 24) & 0xFF000000;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 10_10_10_2_Uint.
*/
void __glSpanPack_10_10_10_2_Uint(__GLcontext *gc, 
				  __GLpixelSpanInfo *spanInfo, 
				  const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLuint *outData = (GLuint *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLuint)(invalue * 1023) <<22) & 0xFFC00000;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 1023) <<12) & 0x003FF000;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 1023) <<2) & 0x00000FFC;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 3)) & 0x00000003;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 2_10_10_10_RevUint.
*/
/*ARGSUSED*/
void __glSpanPack_2_10_10_10_RevUint(__GLcontext *gc, 
				     __GLpixelSpanInfo *spanInfo, 
				     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLuint *outData = (GLuint *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLuint)(invalue * 1023)       ) & 0x000003FF;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 1023) << 10) & 0x000FFC00;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 1023) << 20) & 0x3FF00000;
	invalue = *inData++;
	*outData |= ((GLuint)(invalue * 3)    << 30) & 0xC0000000;
	outData++;
    }
}

/*
** Packs from a three component span of type FLOAT to type X555Ushort.
*/
void __glSpanPackX555Ushort(__GLcontext *gc, 
			    __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData = 0x8000;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) <<10) & 0x7C00;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) <<5) & 0x03E0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31)) & 0x001F;
	outData++;
    }
}

/*
** Packs from a four component span of type FLOAT to type 1555Ushort.
*/
void __glSpanPack1555Ushort(__GLcontext *gc, 
			    __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint totalSize = width; /* one group (i.e. pixel) per type unit */
    GLfloat invalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	invalue = *inData++;
	*outData = ((GLushort)(invalue * 31) <<10) & 0x7C00;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31) <<5) & 0x03E0;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue * 31)) & 0x001F;
	invalue = *inData++;
	*outData |= ((GLushort)(invalue) <<15) & 0x8000;
	outData++;
    }
}

/*
** Packs to any index of type UNSIGNED_BYTE from a span of the same
** format of type FLOAT.
*/
void __glSpanPackUbyteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLubyte *outData = (GLubyte *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = ((GLint) *inData++) & 0xff;
    }
}

/*
** Packs to any index of type BYTE from a span of the same
** format of type FLOAT.
*/
void __glSpanPackByteI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	               const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLbyte *outData = (GLbyte *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = ((GLint) *inData++) & 0x7f;
    }
}

/*
** Packs to any index of type UNSIGNED_SHORT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackUshortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = ((GLint) *inData++) & 0xffff;
    }
}

/*
** Packs to any index of type SHORT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackShortI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLshort *outData = (GLshort *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = ((GLint) *inData++) & 0x7fff;
    }
}

/*
** Packs to any index of type UNSIGNED_INT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackUintI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	               const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLuint *outData = (GLuint *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLuint) *inData++;
    }
}

/*
** Packs to any index of type INT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackIntI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
	              const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint totalSize = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLint *outData = (GLint *) outspan;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = ((GLint) *inData++) & 0x7fffffff;
    }
}


/*
** Packs to any values of type UNSIGNED_BYTE from a span of the same
** format of type FLOAT.
** All of the nonColorComp routines do not convert to the 0..1 float
** range.
*/
void __glSpanPackNonCompUbyte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLubyte *outData = (GLubyte *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLubyte)(*inData++);
    }
}

/*
** Packs to any value of type BYTE from a span of the same
** format of type FLOAT.
*/
void __glSpanPackNonCompByte(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLbyte *outData = (GLbyte *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLbyte)(*inData++);
    }
}

/*
** Packs to any value of type UNSIGNED_SHORT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackNonCompUshort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLushort)(*inData++);
    }
}

/*
** Packs to any value of type SHORT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackNonCompShort(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLshort *outData = (GLshort *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLshort)(*inData++);
    }
}

/*
** Packs to any value of type UNSIGNED_INT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackNonCompUint(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLuint *outData = (GLuint *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLuint)(*inData++);
    }
}

/*
** Packs to any value of type INT from a span of the same
** format of type FLOAT.
*/
void __glSpanPackNonCompInt(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLfloat *inData = (const GLfloat *) inspan;
    GLint *outData = (GLint *) outspan;
    GLint components = spanInfo->dstComponents;
    GLint totalSize = width * components;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<totalSize; i++) {
	*outData++ = (GLint)(*inData++);
    }
}

/* ARGSUSED */
void __glSpanCopy(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		  const GLvoid *inspan, GLvoid *outspan)
{
    GLint totalSize = spanInfo->width * spanInfo->srcComponents *
	spanInfo->srcElementSize;

    __GL_MEMCOPY(outspan, inspan, totalSize);
}

/*
** Packs to any index of type BITMAP from a span of the same
** format of type FLOAT.
*/
void __glSpanPackBitmap(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
	                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width;
    GLvoid *userData;
    const GLfloat *spanData;
    GLint lsbFirst;
    GLint startBit;
    GLint bit;
    GLubyte ubyte;

#ifdef __GL_LINT
    gc = gc;
#endif

    width = spanInfo->width;
    userData = outspan;
    spanData = (const GLfloat *) inspan;

    lsbFirst = spanInfo->dstLsbFirst;
    startBit = spanInfo->dstStartBit;

    i = width;
    bit = startBit;
    ubyte = *(GLubyte *) userData;

    if (lsbFirst) {
	if (bit) {
	    switch(bit) {
	      case 1:
		if (((GLint) *spanData++) & 1) ubyte |= 0x02;
		else ubyte &= ~0x02;
		if (--i == 0) break;
	      case 2:
		if (((GLint) *spanData++) & 1) ubyte |= 0x04;
		else ubyte &= ~0x04;
		if (--i == 0) break;
	      case 3:
		if (((GLint) *spanData++) & 1) ubyte |= 0x08;
		else ubyte &= ~0x08;
		if (--i == 0) break;
	      case 4:
		if (((GLint) *spanData++) & 1) ubyte |= 0x10;
		else ubyte &= ~0x10;
		if (--i == 0) break;
	      case 5:
		if (((GLint) *spanData++) & 1) ubyte |= 0x20;
		else ubyte &= ~0x20;
		if (--i == 0) break;
	      case 6:
		if (((GLint) *spanData++) & 1) ubyte |= 0x40;
		else ubyte &= ~0x40;
		if (--i == 0) break;
	      case 7:
		if (((GLint) *spanData++) & 1) ubyte |= 0x80;
		else ubyte &= ~0x80;
		i--;
	    }
	    *(GLubyte *) userData = ubyte;
	    userData = (GLvoid *) ((GLubyte *) userData + 1);
	}
	while (i >= 8) {
	    ubyte = 0;
	    i -= 8;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x01;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x02;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x04;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x08;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x10;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x20;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x40;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x80;
	    *(GLubyte *) userData = ubyte;
	    userData = (GLvoid *) ((GLubyte *) userData + 1);
	}
	if (i) {
	    ubyte = *(GLubyte *) userData;
	    switch(i) {
	      case 7:
		if (((GLint) *spanData++) & 1) ubyte |= 0x01;
		else ubyte &= ~0x01;
	      case 6:
		if (((GLint) *spanData++) & 1) ubyte |= 0x02;
		else ubyte &= ~0x02;
	      case 5:
		if (((GLint) *spanData++) & 1) ubyte |= 0x04;
		else ubyte &= ~0x04;
	      case 4:
		if (((GLint) *spanData++) & 1) ubyte |= 0x08;
		else ubyte &= ~0x08;
	      case 3:
		if (((GLint) *spanData++) & 1) ubyte |= 0x10;
		else ubyte &= ~0x10;
	      case 2:
		if (((GLint) *spanData++) & 1) ubyte |= 0x20;
		else ubyte &= ~0x20;
	      case 1:
		if (((GLint) *spanData++) & 1) ubyte |= 0x40;
		else ubyte &= ~0x40;
		break;
	    }
	    *(GLubyte *) userData = ubyte;
	}
    } else {
	if (bit) {
	    switch(bit) {
	      case 1:
		if (((GLint) *spanData++) & 1) ubyte |= 0x40;
		else ubyte &= ~0x40;
		if (--i == 0) break;
	      case 2:
		if (((GLint) *spanData++) & 1) ubyte |= 0x20;
		else ubyte &= ~0x20;
		if (--i == 0) break;
	      case 3:
		if (((GLint) *spanData++) & 1) ubyte |= 0x10;
		else ubyte &= ~0x10;
		if (--i == 0) break;
	      case 4:
		if (((GLint) *spanData++) & 1) ubyte |= 0x08;
		else ubyte &= ~0x08;
		if (--i == 0) break;
	      case 5:
		if (((GLint) *spanData++) & 1) ubyte |= 0x04;
		else ubyte &= ~0x04;
		if (--i == 0) break;
	      case 6:
		if (((GLint) *spanData++) & 1) ubyte |= 0x02;
		else ubyte &= ~0x02;
		if (--i == 0) break;
	      case 7:
		if (((GLint) *spanData++) & 1) ubyte |= 0x01;
		else ubyte &= ~0x01;
		i--;
	    }
	    *(GLubyte *) userData = ubyte;
	    userData = (GLvoid *) ((GLubyte *) userData + 1);
	}
	while (i >= 8) {
	    ubyte = 0;
	    i -= 8;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x80;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x40;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x20;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x10;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x08;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x04;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x02;
	    if (((GLint) *spanData++) & 1) ubyte |= 0x01;
	    *(GLubyte *) userData = ubyte;
	    userData = (GLvoid *) ((GLubyte *) userData + 1);
	}
	if (i) {
	    ubyte = *(GLubyte *) userData;
	    switch(i) {
	      case 7:
		if (((GLint) *spanData++) & 1) ubyte |= 0x80;
		else ubyte &= ~0x80;
	      case 6:
		if (((GLint) *spanData++) & 1) ubyte |= 0x40;
		else ubyte &= ~0x40;
	      case 5:
		if (((GLint) *spanData++) & 1) ubyte |= 0x20;
		else ubyte &= ~0x20;
	      case 4:
		if (((GLint) *spanData++) & 1) ubyte |= 0x10;
		else ubyte &= ~0x10;
	      case 3:
		if (((GLint) *spanData++) & 1) ubyte |= 0x08;
		else ubyte &= ~0x08;
	      case 2:
		if (((GLint) *spanData++) & 1) ubyte |= 0x04;
		else ubyte &= ~0x04;
	      case 1:
		if (((GLint) *spanData++) & 1) ubyte |= 0x02;
		else ubyte &= ~0x02;
		break;
	    }
	    *(GLubyte *) userData = ubyte;
	}
    }
}

/*************************************************************************
 * Optimized pixel spans
 */

/*
** Packs from a one component span of type UNSIGNED_BYTE to type X555Ushort.
*/
void __glSpanPackL8ToXRGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData++, outData++) {
	invalue = (*inData) >> 3;
        outvalue = 0x8000 | (invalue<<10) | (invalue<<5) | invalue;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a three component span of type UNSIGNED_BYTE to type X555Ushort.
*/
void __glSpanPackRGB8ToXRGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                                const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=3, outData++) {
	invalue = inData[0];
        outvalue = 0x8000 | ((invalue>>3)<<10);
	invalue = inData[1];
        outvalue |= ((invalue>>3)<<5);
	invalue = inData[2];
        outvalue |= invalue>>3;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a four component span of type UNSIGNED_BYTE to type X555Ushort.
*/
void __glSpanPackRGBA8ToXRGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=4, outData++) {
	invalue = inData[0];
        outvalue = 0x8000 | ((invalue>>3)<<10);
	invalue = inData[1];
        outvalue |= ((invalue>>3)<<5);
	invalue = inData[2];
        outvalue |= invalue>>3;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a one component span of type UNSIGNED_BYTE to type 565Ushort.
*/
void __glSpanPackL8ToRGB565(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData++, outData++) {
	invalue = *inData;
        outvalue = ((invalue>>3)<<11) | ((invalue>>2)<<5) | (invalue>>3);
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a three component span of type UNSIGNED_BYTE to type 565Ushort.
*/
void __glSpanPackRGB8ToRGB565(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                              const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=3, outData++) {
	invalue = inData[0];
        outvalue = ((invalue>>3)<<11);
	invalue = inData[1];
        outvalue |= ((invalue>>2)<<5);
	invalue = inData[2];
        outvalue |= invalue>>3;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a four component span of type UNSIGNED_BYTE to type 565Ushort.
*/
void __glSpanPackRGBA8ToRGB565(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=4, outData++) {
	invalue = inData[0];
        outvalue = ((invalue>>3)<<11);
	invalue = inData[1];
        outvalue |= ((invalue>>2)<<5);
	invalue = inData[2];
        outvalue |= invalue>>3;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a two component span of type UNSIGNED_BYTE to type 1555Ushort.
*/
void __glSpanPackLA8ToARGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=2, outData++) {
	invalue = inData[0] >> 3;
        outvalue = (invalue<<10) | (invalue<<5) | invalue;
        invalue = inData[1];
        outvalue |= (invalue >> 7) << 15;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a four component span of type UNSIGNED_BYTE to type 1555Ushort.
*/
void __glSpanPackRGBA8ToARGB1555(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
                                 const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=4, outData++) {
	invalue = inData[0];
        outvalue = ((invalue>>3)<<10);
	invalue = inData[1];
        outvalue |= ((invalue>>3)<<5);
	invalue = inData[2];
        outvalue |= invalue>>3;
        invalue = inData[3];
        outvalue |= (invalue >> 7) << 15;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a two component span of type UNSIGNED_BYTE to type ARGB4.
*/
void __glSpanPackLA8ToARGB4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=2, outData++) {
	invalue = inData[0] >> 4;
        outvalue = (invalue<<8) | (invalue<<4) | invalue;
        invalue = inData[1];
        outvalue |= (invalue >> 4) << 12;
	*outData = (GLushort) outvalue;
    }
}

/*
** Packs from a four component span of type UNSIGNED_BYTE to type ARGB4.
*/
void __glSpanPackRGBA8ToARGB4(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *inspan, GLvoid *outspan)
{
    GLint i;
    GLint width = spanInfo->width;
    const GLubyte *inData = (const GLubyte *) inspan;
    GLushort *outData = (GLushort *) outspan;
    GLuint invalue, outvalue;

#ifdef __GL_LINT
    gc = gc;
#endif
    for (i=0; i<width; i++, inData+=4, outData++) {
	invalue = inData[0];
        outvalue = ((invalue>>4)<<8);
	invalue = inData[1];
        outvalue |= ((invalue>>4)<<4);
	invalue = inData[2];
        outvalue |= invalue>>4;
        invalue = inData[3];
        outvalue |= (invalue >> 4) << 12;
	*outData = (GLushort) outvalue;
    }
}

