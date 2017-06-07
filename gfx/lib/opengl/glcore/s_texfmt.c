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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_texel.c#17 $
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
#include <memory.h>

/*****************************************************************/
/*		    Texture internal formats			 */
/*****************************************************************/

const __GLtextureFormat __glTexFormatLuminance8 = {
    __GL_FORMAT_LUMINANCE8,	/* internalFormat */
    0,				/* indexSize */
    0,				/* redSize */
    0,				/* greenSize */
    0,				/* blueSize */
    0,				/* alphaSize */
    8,				/* luminanceSize */
    0,				/* intensitySize */
    8,				/* bitsPerTexel */
    GL_RED,			/* pxFormat */
    GL_UNSIGNED_BYTE,		/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelL8,		/* extractTexel */
    __glExtractTexelL8_B,	/* extractTexelBorder */
    __glExtractTexel3DL8,	/* extractTexel */
    __glExtractTexel3DL8_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatLuminanceAlpha8 = {
    __GL_FORMAT_LUMINANCE_ALPHA8, /* internalFormat */
    0,				/* indexSize */
    0,				/* redSize */
    0,				/* greenSize */
    0,				/* blueSize */
    8,				/* alphaSize */
    8,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    __GL_RED_ALPHA,		/* pxFormat */
    GL_UNSIGNED_BYTE,		/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelLA8,	/* extractTexel */
    __glExtractTexelLA8_B,	/* extractTexelBorder */
    __glExtractTexel3DLA8,	/* extractTexel */
    __glExtractTexel3DLA8_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatRGB8 = {
    __GL_FORMAT_RGB8,		/* internalFormat */
    0,				/* indexSize */
    8,				/* redSize */
    8,				/* greenSize */
    8,				/* blueSize */
    0,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    24,				/* bitsPerTexel */
    GL_RGB,			/* pxFormat */
    GL_UNSIGNED_BYTE,		/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelRGB8,	/* extractTexel */
    __glExtractTexelRGB8_B,	/* extractTexelBorder */
    __glExtractTexel3DRGB8,	/* extractTexel */
    __glExtractTexel3DRGB8_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatRGB332 = {
    __GL_FORMAT_RGB332,		/* internalFormat */
    0,				/* indexSize */
    3,				/* redSize */
    3,				/* greenSize */
    2,				/* blueSize */
    0,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    8,				/* bitsPerTexel */
    GL_RGB,			/* pxFormat */
    GL_UNSIGNED_BYTE_3_3_2,	/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelRGB332,	/* extractTexel */
    __glExtractTexelRGB332_B,	/* extractTexelBorder */
    __glExtractTexel3DRGB332,	/* extractTexel */
    __glExtractTexel3DRGB332_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatXRGB1555 = {
    __GL_FORMAT_XRGB1555,	/* internalFormat */
    0,				/* indexSize */
    5,				/* redSize */
    5,				/* greenSize */
    5,				/* blueSize */
    0,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    GL_RGB,			/* pxFormat */
    __GL_UNSIGNED_SHORT_X_5_5_5,/* pxType */
    2,				/* pxAlignment */
    __glExtractTexelXRGB1555,	/* extractTexel */
    __glExtractTexelXRGB1555_B, /* extractTexelBorder */
    __glExtractTexel3DXRGB1555,	/* extractTexel */
    __glExtractTexel3DXRGB1555_B, /* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatRGB565 = {
    __GL_FORMAT_RGB565,		/* internalFormat */
    0,				/* indexSize */
    5,				/* redSize */
    6,				/* greenSize */
    5,				/* blueSize */
    0,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    GL_RGB,			/* pxFormat */
    __GL_UNSIGNED_SHORT_5_6_5,	/* pxType */
    2,				/* pxAlignment */
    __glExtractTexelRGB565,	/* extractTexel */
    __glExtractTexelRGB565_B,	/* extractTexelBorder */
    __glExtractTexel3DRGB565,	/* extractTexel */
    __glExtractTexel3DRGB565_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatRGBA8 = {
    __GL_FORMAT_RGBA8,		/* internalFormat */
    0,				/* indexSize */
    8,				/* redSize */
    8,				/* greenSize */
    8,				/* blueSize */
    8,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    32,				/* bitsPerTexel */
    GL_RGBA,			/* pxFormat */
    GL_UNSIGNED_BYTE,		/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelRGBA8,	/* extractTexel */
    __glExtractTexelRGBA8_B,	/* extractTexelBorder */
    __glExtractTexel3DRGBA8,	/* extractTexel */
    __glExtractTexel3DRGBA8_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatRGBA4 = {
    __GL_FORMAT_RGBA4,		/* internalFormat */
    0,				/* indexSize */
    4,				/* redSize */
    4,				/* greenSize */
    4,				/* blueSize */
    4,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    GL_RGBA,			/* pxFormat */
    GL_UNSIGNED_SHORT_4_4_4_4,			/* pxType */
    2,				/* pxAlignment */
    __glExtractTexelRGBA4,	/* extractTexel */
    __glExtractTexelRGBA4_B,	/* extractTexelBorder */
    __glExtractTexel3DRGBA4,	/* extractTexel */
    __glExtractTexel3DRGBA4_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatARGB4 = {
    __GL_FORMAT_ARGB4,		/* internalFormat */
    0,				/* indexSize */
    4,				/* redSize */
    4,				/* greenSize */
    4,				/* blueSize */
    4,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    GL_RGBA,			/* pxFormat */
    __GL_UNSIGNED_SHORT_4_4_4_4_ARGB,		/* pxType */
    2,				/* pxAlignment */
    __glExtractTexelARGB4,	/* extractTexel */
    __glExtractTexelARGB4_B,	/* extractTexelBorder */
    __glExtractTexel3DARGB4,	/* extractTexel */
    __glExtractTexel3DARGB4_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatRGBA5551 = {
    __GL_FORMAT_RGBA5551,	/* internalFormat */
    0,				/* indexSize */
    5,				/* redSize */
    5,				/* greenSize */
    5,				/* blueSize */
    1,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    GL_RGBA,			/* pxFormat */
    GL_UNSIGNED_SHORT_5_5_5_1,			/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelRGBA5551,	/* extractTexel */
    __glExtractTexelRGBA5551_B, /* extractTexelBorder */
    __glExtractTexel3DRGBA5551,	/* extractTexel */
    __glExtractTexel3DRGBA5551_B, /* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatARGB1555 = {
    __GL_FORMAT_ARGB1555,	/* internalFormat */
    0,				/* indexSize */
    5,				/* redSize */
    5,				/* greenSize */
    5,				/* blueSize */
    1,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    GL_RGBA,			/* pxFormat */
    __GL_UNSIGNED_SHORT_1_5_5_5,/* pxType */
    2,				/* pxAlignment */
    __glExtractTexelARGB1555,	/* extractTexel */
    __glExtractTexelARGB1555_B, /* extractTexelBorder */
    __glExtractTexel3DARGB1555,	/* extractTexel */
    __glExtractTexel3DARGB1555_B,/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatAlpha8 = {
    __GL_FORMAT_ALPHA8,		/* internalFormat */
    0,				/* indexSize */
    0,				/* redSize */
    0,				/* greenSize */
    0,				/* blueSize */
    8,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    8,				/* bitsPerTexel */
    GL_ALPHA,			/* pxFormat */
    GL_UNSIGNED_BYTE,		/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelA8,		/* extractTexel */
    __glExtractTexelA8_B,	/* extractTexelBorder */
    __glExtractTexel3DA8,	/* extractTexel */
    __glExtractTexel3DA8_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatIntensity8 = {
    __GL_FORMAT_INTENSITY8,	/* internalFormat */
    0,				/* indexSize */
    0,				/* redSize */
    0,				/* greenSize */
    0,				/* blueSize */
    0,				/* alphaSize */
    0,				/* luminanceSize */
    8,				/* intensitySize */
    8,				/* bitsPerTexel */
    GL_RED,			/* pxFormat */
    GL_UNSIGNED_BYTE,		/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelI8,		/* extractTexel */
    __glExtractTexelI8_B,	/* extractTexelBorder */
    __glExtractTexel3DI8,	/* extractTexel */
    __glExtractTexel3DI8_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatColorIndex8 = {
    __GL_FORMAT_COLOR_INDEX8,	/* internalFormat */
    0,				/* indexSize */
    0,				/* redSize */
    0,				/* greenSize */
    0,				/* blueSize */
    0,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    8,				/* bitsPerTexel */
    GL_COLOR_INDEX,		/* pxFormat */
    GL_UNSIGNED_BYTE,		/* pxType */
    1,				/* pxAlignment */
    __glExtractTexelCI8,	/* extractTexel */
    __glExtractTexelCI8_B,	/* extractTexelBorder */
    __glExtractTexel3DCI8,	/* extractTexel */
    __glExtractTexel3DCI8_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

const __GLtextureFormat __glTexFormatColorIndex16 = {
    __GL_FORMAT_COLOR_INDEX16,	/* internalFormat */
    0,				/* indexSize */
    0,				/* redSize */
    0,				/* greenSize */
    0,				/* blueSize */
    0,				/* alphaSize */
    0,				/* luminanceSize */
    0,				/* intensitySize */
    16,				/* bitsPerTexel */
    GL_COLOR_INDEX,		/* pxFormat */
    GL_UNSIGNED_SHORT,		/* pxType */
    2,				/* pxAlignment */
    __glExtractTexelCI16,	/* extractTexel */
    __glExtractTexelCI16_B,	/* extractTexelBorder */
    __glExtractTexel3DCI16,	/* extractTexel */
    __glExtractTexel3DCI16_B,	/* extractTexelBorder */
    NULL,			/* store */
    NULL,			/* fetch */
    NULL,			/* storeRect */
    NULL,			/* fetchRect */
};

/* Given a requested internal format, select the appropriate storage format.
 * The baseFormat is returned as a side effect.
 */
const __GLtextureFormat *
__glLookupTextureFormat(const GLenum components, GLenum *baseFormat)
{
    switch (components) {
    case 1:
    case GL_LUMINANCE:
    case GL_LUMINANCE4:
    case GL_LUMINANCE8:
    case GL_LUMINANCE12:
    case GL_LUMINANCE16:
	*baseFormat = GL_LUMINANCE;
	return &__glTexFormatLuminance8;
    case 2:
    case GL_LUMINANCE_ALPHA:
    case GL_LUMINANCE4_ALPHA4:
    case GL_LUMINANCE6_ALPHA2:
    case GL_LUMINANCE8_ALPHA8:
    case GL_LUMINANCE12_ALPHA4:
    case GL_LUMINANCE12_ALPHA12:
    case GL_LUMINANCE16_ALPHA16:
	*baseFormat = GL_LUMINANCE_ALPHA;
	return &__glTexFormatLuminanceAlpha8;
    case 3:
    case GL_RGB:
    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
	*baseFormat = GL_RGB;
	return &__glTexFormatRGB8;
    case 4:
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGBA8:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGB5_A1:
    case GL_RGB10_A2:
	*baseFormat = GL_RGBA;
	return &__glTexFormatRGBA8;
    case GL_ALPHA:
    case GL_ALPHA4:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
	*baseFormat = GL_ALPHA;
	return &__glTexFormatAlpha8;
    case GL_INTENSITY:
    case GL_INTENSITY4:
    case GL_INTENSITY8:
    case GL_INTENSITY12:
    case GL_INTENSITY16:
	*baseFormat = GL_INTENSITY;
	return &__glTexFormatIntensity8;
#ifdef XXX_IS_COLOR_INDEX_FORMAT_SUPPORTED
    case GL_COLOR_INDEX1_EXT:
    case GL_COLOR_INDEX2_EXT:
    case GL_COLOR_INDEX4_EXT:
    case GL_COLOR_INDEX8_EXT:
	*baseFormat = GL_COLOR_INDEX;
	return &__glTexFormatColorIndex8;
    case GL_COLOR_INDEX12_EXT:
    case GL_COLOR_INDEX16_EXT:
	*baseFormat = GL_COLOR_INDEX;
	return &__glTexFormatColorIndex16;
#endif
    default:
	assert(0);
	return 0;
    }
}
