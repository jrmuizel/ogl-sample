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
** $Header: /hosts/vertov.engr/usr/people/dyu/trees/linuxogl/main/gfx/lib/opengl/glcore/RCS/s_texextr.c,v 1.1 1999/09/23 05:37:33 dyu Exp dyu $
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

/************************************************************************/

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexelL8(__GLmipMapLevel *level, __GLtexture *tex,
		        GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	luminance = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = level->buffer + (row << level->widthLog2) + col;
	luminance = image[0];
    }
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
}

/*
** Get a texture element out of the two component texture buffer
** with no border.
*/
void __glExtractTexelLA8(__GLmipMapLevel *level, __GLtexture *tex,
		         GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance, alpha;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	luminance = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	alpha = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = level->buffer + ((row << level->widthLog2) + col) * 2;
	luminance = image[0];
	alpha = image[1];
    }
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
    result->a = alpha;
}

/*
** Get a texture element out of the three component texture buffer
** with no border.
*/
void __glExtractTexelRGB8(__GLmipMapLevel *level, __GLtexture *tex,
		          GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = level->buffer + ((row << level->widthLog2) + col) * 3;
	result->r = image[0];
	result->g = image[1];
	result->b = image[2];
    }
}

/*
** Get a texture element out of the four component texture buffer
** with no border.
*/
void __glExtractTexelRGBA8(__GLmipMapLevel *level, __GLtexture *tex,
		           GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = level->buffer + ((row << level->widthLog2) + col) * 4;
	result->r = image[0];
	result->g = image[1];
	result->b = image[2];
	result->a = image[3];
    }
}

void __glExtractTexelA8(__GLmipMapLevel *level, __GLtexture *tex,
		        GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = level->buffer + (row << level->widthLog2) + col;
	result->a = image[0];
    }
}

void __glExtractTexelI8(__GLmipMapLevel *level, __GLtexture *tex,
		        GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte intensity;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	intensity = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = level->buffer + (row << level->widthLog2) + col;
	intensity = image[0];
    }
    result->r = intensity;
    result->g = intensity;
    result->b = intensity;
    result->a = intensity;
}

void __glExtractTexelXRGB1555(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = (GLushort *)level->buffer + (row << level->widthLog2) + col;

	texel = image[0];
	result->r = (GLubyte) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
	result->g = (GLubyte) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
	result->b = (GLubyte) ((texel	   ) & 0x1F) * (255.0F/31.0F);
    }
}

void __glExtractTexelRGB332(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = (GLubyte *)level->buffer + (row << level->widthLog2) + col;

	texel = image[0];
	result->r = (GLubyte) ((texel >> 5) & 0x7) * (255.0F/7.0F);
	result->g = (GLubyte) ((texel >> 2) & 0x7) * (255.0F/7.0F);
	result->b = (GLubyte) ((texel	  ) & 0x3) * (255.0F/3.0F);
    }
}

void __glExtractTexelRGB565(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = (GLushort *)level->buffer + (row << level->widthLog2) + col;

	texel = image[0];
	result->r = (GLubyte) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
	result->g = (GLubyte) ((texel >>  5) & 0x3F) * (255.0F/63.0F);
	result->b = (GLubyte) ((texel	   ) & 0x1F) * (255.0F/31.0F);
    }
}

void __glExtractTexelRGBA4(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
	result->g = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
	result->b = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
	result->a = (GLubyte) ((texel >>  0) & 0x0F) * (255.0F/15.0F);
    }
}

void __glExtractTexelARGB4(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
	result->g = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
	result->b = (GLubyte) ((texel	   ) & 0x0F) * (255.0F/15.0F);
	result->a = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
    }
}

void __glExtractTexelRGBA5551(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (__GLfloat) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
	result->g = (__GLfloat) ((texel >>  6) & 0x1F) * (255.0F/31.0F);
	result->b = (__GLfloat) ((texel >>  1) & 0x1F) * (255.0F/31.0F);
	result->a = (__GLfloat) ((texel >>  0) & 0x01) * 255.0F;
    }
}

void __glExtractTexelARGB1555(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (__GLfloat) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
	result->g = (__GLfloat) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
	result->b = (__GLfloat) ((texel      ) & 0x1F) * (255.0F/31.0F);
	result->a = (__GLfloat) ((texel >> 15) & 0x01) * 255.0F;
    }
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexelCI8(__GLmipMapLevel *level, __GLtexture *tex,
			 GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	index = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = (GLubyte *)level->buffer + (row << level->widthLog2) + col;
	index = image[0];
    }

    if (tex->baseFormat == GL_COLOR_INDEX) {
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexelCI16(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	index = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = (GLushort *)level->buffer + (row << level->widthLog2) + col;
	index = image[0];
    }

    if (tex->baseFormat == GL_COLOR_INDEX) {
	/* Warning: this will truncate to GLubyte */
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}

/**************************************************************************/

/*
** Get a texture element out of the one component texture buffer
** with a border.
*/
void __glExtractTexelL8_B(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance;

    row++;
    col++;
    image = level->buffer + (row * level->width) + col;
    luminance = image[0];
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
}

/*
** Get a texture element out of the two component texture buffer
** with a border.
*/
void __glExtractTexelLA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			   GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance, alpha;

    row++;
    col++;
    image = level->buffer + (row * level->width + col) * 2;
    luminance = image[0];
    alpha = image[1];
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
    result->a = alpha;
}

/*
** Get a texture element out of the three component texture buffer
** with a border.
*/
void __glExtractTexelRGB8_B(__GLmipMapLevel *level, __GLtexture *tex,
			    GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    row++;
    col++;
    image = level->buffer + (row * level->width + col) * 3;
    result->r = image[0];
    result->g = image[1];
    result->b = image[2];
}

/*
** Get a texture element out of the four component texture buffer
** with a border.
*/
void __glExtractTexelRGBA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			     GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    row++;
    col++;
    image = level->buffer + (row * level->width + col) * 4;
    result->r = image[0];
    result->g = image[1];
    result->b = image[2];
    result->a = image[3];
}

void __glExtractTexelA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    row++;
    col++;
    image = level->buffer + (row << level->widthLog2) + col;
    result->a = image[0];
}

void __glExtractTexelI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte intensity;

    row++;
    col++;
    image = level->buffer + (row << level->widthLog2) + col;
    intensity = image[0];
    result->r = intensity;
    result->g = intensity;
    result->b = intensity;
    result->a = intensity;
}

void __glExtractTexelXRGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    image = (GLushort *)level->buffer + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
    result->b = (GLubyte) ((texel      ) & 0x1F) * (255.0F/31.0F);
}

void __glExtractTexelRGB332_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image, texel;

    row++;
    col++;
    image = (GLubyte *)level->buffer + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 5) & 0x7) * (255.0F/7.0F);
    result->g = (GLubyte) ((texel >> 2) & 0x7) * (255.0F/7.0F);
    result->b = (GLubyte) ((texel     ) & 0x3) * (255.0F/3.0F);
}

void __glExtractTexelRGB565_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    image = (GLushort *)level->buffer + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  5) & 0x3F) * (255.0F/63.0F);
    result->b = (GLubyte) ((texel      ) & 0x1F) * (255.0F/31.0F);
}

void __glExtractTexelRGBA4_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    GLushort *image, texel;

    row++;
    col++;
    image = (GLushort *)level->buffer + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
    result->g = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
    result->b = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
    result->a = (GLubyte) ((texel >>  0) & 0x0F) * (255.0F/15.0F);
}

void __glExtractTexelARGB4_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    GLushort *image, texel;

    row++;
    col++;
    image = (GLushort *)level->buffer + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
    result->g = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
    result->b = (GLubyte) ((texel      ) & 0x0F) * (255.0F/15.0F);
    result->a = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
}

void __glExtractTexelRGBA5551_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    image = (GLushort *)level->buffer + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  6) & 0x1F) * (255.0F/31.0F);
    result->b = (GLubyte) ((texel >>  1) & 0x1F) * (255.0F/31.0F);
    result->a = (GLubyte) ((texel >>  0) & 0x01) * 255.0F;
}

void __glExtractTexelARGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    image = (GLushort *)level->buffer + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
    result->b = (GLubyte) ((texel      ) & 0x1F) * (255.0F/31.0F);
    result->a = (GLubyte) ((texel >> 15) & 0x01) * 255.0F;
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexelCI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			   GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    row++;
    col++;
    image = (GLubyte *)level->buffer + (row << level->widthLog2) + col;
    index = image[0];

    if (tex->baseFormat == GL_COLOR_INDEX) {
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexelCI16_B(__GLmipMapLevel *level, __GLtexture *tex,
			    GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    row++;
    col++;
    image = (GLushort *)level->buffer + (row << level->widthLog2) + col;
    index = image[0];

    if (tex->baseFormat == GL_COLOR_INDEX) {
	/* Warning: this will truncate to GLubyte */
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}

/************************************************************************/

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexel3DL8(__GLmipMapLevel *level, __GLtexture *tex,
		          GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	luminance = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	luminance = image[0];
    }
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
}

/*
** Get a texture element out of the two component texture buffer
** with no border.
*/
void __glExtractTexel3DLA8(__GLmipMapLevel *level, __GLtexture *tex,
		           GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance, alpha;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	luminance = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	alpha = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = level->buffer +
		(img * level->imageSize + (row << level->widthLog2) + col) * 2;
	luminance = image[0];
	alpha = image[1];
    }
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
    result->a = alpha;
}

/*
** Get a texture element out of the three component texture buffer
** with no border.
*/
void __glExtractTexel3DRGB8(__GLmipMapLevel *level, __GLtexture *tex,
		            GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = level->buffer +
		(img * level->imageSize + (row << level->widthLog2) + col) * 3;
	result->r = image[0];
	result->g = image[1];
	result->b = image[2];
    }
}

/*
** Get a texture element out of the four component texture buffer
** with no border.
*/
void __glExtractTexel3DRGBA8(__GLmipMapLevel *level, __GLtexture *tex,
		             GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = level->buffer +
		(img * level->imageSize + (row << level->widthLog2) + col) * 4;
	result->r = image[0];
	result->g = image[1];
	result->b = image[2];
	result->a = image[3];
    }
}

void __glExtractTexel3DA8(__GLmipMapLevel *level, __GLtexture *tex,
		          GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	result->a = image[0];
    }
}

void __glExtractTexel3DI8(__GLmipMapLevel *level, __GLtexture *tex,
		          GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte intensity;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	intensity = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	intensity = image[0];
    }
    result->r = intensity;
    result->g = intensity;
    result->b = intensity;
    result->a = intensity;
}

void __glExtractTexel3DXRGB1555(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = (GLushort *)level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;

	texel = image[0];
	result->r = (GLubyte) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
	result->g = (GLubyte) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
	result->b = (GLubyte) ((texel	   ) & 0x1F) * (255.0F/31.0F);
    }
}

void __glExtractTexel3DRGB332(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = (GLubyte *)level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;

	texel = image[0];
	result->r = (GLubyte) ((texel >> 5) & 0x7) * (255.0F/7.0F);
	result->g = (GLubyte) ((texel >> 2) & 0x7) * (255.0F/7.0F);
	result->b = (GLubyte) ((texel	  ) & 0x3) * (255.0F/3.0F);
    }
}

void __glExtractTexel3DRGB565(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
    } else {
	image = (GLushort *)level->buffer + 
		(img * level->imageSize) + (row << level->widthLog2) + col;

	texel = image[0];
	result->r = (GLubyte) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
	result->g = (GLubyte) ((texel >>  5) & 0x3F) * (255.0F/63.0F);
	result->b = (GLubyte) ((texel	   ) & 0x1F) * (255.0F/31.0F);
    }
}

void __glExtractTexel3DRGBA4(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
	result->g = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
	result->b = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
	result->a = (GLubyte) ((texel >>  0) & 0x0F) * (255.0F/15.0F);
    }
}

void __glExtractTexel3DARGB4(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer + 
		(img * level->imageSize) + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
	result->g = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
	result->b = (GLubyte) ((texel	   ) & 0x0F) * (255.0F/15.0F);
	result->a = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
    }
}

void __glExtractTexel3DRGBA5551(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (__GLfloat) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
	result->g = (__GLfloat) ((texel >>  6) & 0x1F) * (255.0F/31.0F);
	result->b = (__GLfloat) ((texel >>  1) & 0x1F) * (255.0F/31.0F);
	result->a = (__GLfloat) ((texel >>  0) & 0x01) * 255.0F;
    }
}

void __glExtractTexel3DARGB1555(__GLmipMapLevel *level, __GLtexture *tex,
		       GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	result->r = (GLubyte) (255.0F * tex->state.params.borderColor.r);
	result->g = (GLubyte) (255.0F * tex->state.params.borderColor.g);
	result->b = (GLubyte) (255.0F * tex->state.params.borderColor.b);
	result->a = (GLubyte) (255.0F * tex->state.params.borderColor.a);
    } else {
	image = (GLushort *)level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	texel = image[0];
	result->r = (__GLfloat) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
	result->g = (__GLfloat) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
	result->b = (__GLfloat) ((texel      ) & 0x1F) * (255.0F/31.0F);
	result->a = (__GLfloat) ((texel >> 15) & 0x01) * 255.0F;
    }
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexel3DCI8(__GLmipMapLevel *level, __GLtexture *tex,
			 GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	index = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = (GLubyte *)level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	index = image[0];
    }

    if (tex->baseFormat == GL_COLOR_INDEX) {
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexel3DCI16(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    if ((row < 0) || (row >= level->height2) ||
	(col < 0) || (col >= level->width2) ||
	(img < 0) || (img >= level->depth2)) {
	/*
	** Use border color when the texture supplies no border.
	*/
	index = (GLubyte) (255.0F * tex->state.params.borderColor.r);
    } else {
	image = (GLushort *)level->buffer +
		(img * level->imageSize) + (row << level->widthLog2) + col;
	index = image[0];
    }

    if (tex->baseFormat == GL_COLOR_INDEX) {
	/* Warning: this will truncate to GLubyte */
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}

/**************************************************************************/

/*
** Get a texture element out of the one component texture buffer
** with a border.
*/
void __glExtractTexel3DL8_B(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance;

    row++;
    col++;
    img++;
    image = level->buffer + (img * level->imageSize + row * level->width + col);
    luminance = image[0];
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
}

/*
** Get a texture element out of the two component texture buffer
** with a border.
*/
void __glExtractTexel3DLA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			   GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte luminance, alpha;

    row++;
    col++;
    img++;
    image = level->buffer +
	    (img * level->imageSize + row * level->width + col) * 2;
    luminance = image[0];
    alpha = image[1];
    result->r = luminance;
    result->g = luminance;
    result->b = luminance;
    result->a = alpha;
}

/*
** Get a texture element out of the three component texture buffer
** with a border.
*/
void __glExtractTexel3DRGB8_B(__GLmipMapLevel *level, __GLtexture *tex,
			    GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    row++;
    col++;
    img++;
    image = level->buffer +
	    (img * level->imageSize + row * level->width + col) * 3;
    result->r = image[0];
    result->g = image[1];
    result->b = image[2];
}

/*
** Get a texture element out of the four component texture buffer
** with a border.
*/
void __glExtractTexel3DRGBA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			     GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    row++;
    col++;
    img++;
    image = level->buffer +
	    (img * level->imageSize + row * level->width + col) * 4;
    result->r = image[0];
    result->g = image[1];
    result->b = image[2];
    result->a = image[3];
}

void __glExtractTexel3DA8_B(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;

    row++;
    col++;
    img++;
    image = level->buffer +
	    (img * level->imageSize) + (row << level->widthLog2) + col;
    result->a = image[0];
}

void __glExtractTexel3DI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			  GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLtextureBuffer *image;
    GLubyte intensity;

    row++;
    col++;
    img++;
    image = level->buffer +
	    (img * level->imageSize) + (row << level->widthLog2) + col;
    intensity = image[0];
    result->r = intensity;
    result->g = intensity;
    result->b = intensity;
    result->a = intensity;
}

void __glExtractTexel3DXRGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    img++;
    image = (GLushort *)level->buffer +
	    (img * level->imageSize) + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
    result->b = (GLubyte) ((texel      ) & 0x1F) * (255.0F/31.0F);
}

void __glExtractTexel3DRGB332_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image, texel;

    row++;
    col++;
    img++;
    image = (GLubyte *)level->buffer +
	    (img * level->imageSize) + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 5) & 0x7) * (255.0F/7.0F);
    result->g = (GLubyte) ((texel >> 2) & 0x7) * (255.0F/7.0F);
    result->b = (GLubyte) ((texel     ) & 0x3) * (255.0F/3.0F);
}

void __glExtractTexel3DRGB565_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    img++;
    image = (GLushort *)level->buffer +
	    (img * level->imageSize) + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  5) & 0x3F) * (255.0F/63.0F);
    result->b = (GLubyte) ((texel      ) & 0x1F) * (255.0F/31.0F);
}

void __glExtractTexel3DRGBA4_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    GLushort *image, texel;

    row++;
    col++;
    img++;
    image = (GLushort *)level->buffer +
	    (img * level->imageSize) + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
    result->g = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
    result->b = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
    result->a = (GLubyte) ((texel >>  0) & 0x0F) * (255.0F/15.0F);
}

void __glExtractTexel3DARGB4_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    GLushort *image, texel;

    row++;
    col++;
    img++;
    image = (GLushort *)level->buffer +
	    (img * level->imageSize) + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >>  8) & 0x0F) * (255.0F/15.0F);
    result->g = (GLubyte) ((texel >>  4) & 0x0F) * (255.0F/15.0F);
    result->b = (GLubyte) ((texel      ) & 0x0F) * (255.0F/15.0F);
    result->a = (GLubyte) ((texel >> 12) & 0x0F) * (255.0F/15.0F);
}

void __glExtractTexel3DRGBA5551_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    img++;
    image = (GLushort *)level->buffer +
	    (img * level->imageSize) + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 11) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  6) & 0x1F) * (255.0F/31.0F);
    result->b = (GLubyte) ((texel >>  1) & 0x1F) * (255.0F/31.0F);
    result->a = (GLubyte) ((texel >>  0) & 0x01) * 255.0F;
}

void __glExtractTexel3DARGB1555_B(__GLmipMapLevel *level, __GLtexture *tex,
			GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image, texel;

    row++;
    col++;
    img++;
    image = (GLushort *)level->buffer +
	    (img * level->imageSize) + (row * level->width) + col;
    texel = image[0];
    result->r = (GLubyte) ((texel >> 10) & 0x1F) * (255.0F/31.0F);
    result->g = (GLubyte) ((texel >>  5) & 0x1F) * (255.0F/31.0F);
    result->b = (GLubyte) ((texel      ) & 0x1F) * (255.0F/31.0F);
    result->a = (GLubyte) ((texel >> 15) & 0x01) * 255.0F;
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexel3DCI8_B(__GLmipMapLevel *level, __GLtexture *tex,
			   GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLubyte *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    row++;
    col++;
    img++;
    image = (GLubyte *)level->buffer +
	    (img * level->imageSize) + (row << level->widthLog2) + col;
    index = image[0];

    if (tex->baseFormat == GL_COLOR_INDEX) {
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}

/*
** Get a texture element out of the one component texture buffer
** with no border.
*/
void __glExtractTexel3DCI16_B(__GLmipMapLevel *level, __GLtexture *tex,
			    GLint img, GLint row, GLint col, __GLtexel *result)
{
    GLushort *image;
    GLuint index;
    __GLcolorTable *ct = &tex->CT;
    GLubyte *table = (GLubyte *) ct->table;
    GLubyte luminance, alpha, intensity;

    row++;
    col++;
    img++;
    image = (GLushort *)level->buffer +
	    (img * level->imageSize) + (row << level->widthLog2) + col;
    index = image[0];

    if (tex->baseFormat == GL_COLOR_INDEX) {
	/* Warning: this will truncate to GLubyte */
	result->r = index;
    } else {
	switch (ct->baseFormat) {
	case GL_LUMINANCE:
	    luminance = table[index];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    break;
	case GL_LUMINANCE_ALPHA:
	    index <<= 1;
	    luminance = table[index];
	    alpha = table[index+1];
	    result->r = luminance;
	    result->g = luminance;
	    result->b = luminance;
	    result->a = alpha;
	    break;
	case GL_RGB:
	    index += index<<1;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    break;
	case GL_RGBA:
	    index <<= 2;
	    result->r = table[index];
	    result->g = table[index+1];
	    result->b = table[index+2];
	    result->a = table[index+3];
	    break;
	case GL_ALPHA:
	    result->a = table[index];
	    break;
	case GL_INTENSITY:
	    intensity = table[index];
	    result->r = intensity;
	    result->g = intensity;
	    result->b = intensity;
	    result->a = intensity;
	    break;
	default:
	    assert(0);
	}
    }
}
