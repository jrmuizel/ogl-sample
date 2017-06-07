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
** $Header: //depot/main/gfx/lib/opengl/glcore/px_mod.c#10 $
*/

#include "context.h"
#include "global.h"
#include "pixel.h"

/*
** This file contains a bunch of routines that perform span modification.
** As a span of pixel data is being processed (for DrawPixels, ReadPixels
** or CopyPixels), it usually has to pass through one of these routines.
** Span modification consists of mapping colors through pixel maps provided 
** with glPixelMap*(), or scaling/biasing/shifting/offsetting colors with the
** values provided through glPixelTransfer*().
*/

/*
** Build lookup tables to perform automatic modification of RGBA when the
** type is UNSIGNED_BYTE.
*/
void __glBuildRGBAModifyTables(__GLcontext *gc, __GLpixelMachine *pm)
{
    GLfloat *redMap, *greenMap, *blueMap, *alphaMap;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0, aasizeMinusOne = 0;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL, *aamap = NULL;
    GLboolean mapColor;
    __GLfloat rbias, gbias, bbias, abias;
    GLint entry;
    __GLfloat rscale, gscale, bscale, ascale;
    GLint i;
    __GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;
    pm->rgbaCurrent = GL_TRUE;

    redMap = pm->redModMap;
    if (redMap == NULL) {
	/* First time allocation of these maps */
	redMap = pm->redModMap = (GLfloat*)
	    (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->greenModMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->blueModMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->alphaModMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
    }
    greenMap = pm->greenModMap;
    blueMap = pm->blueModMap;
    alphaMap = pm->alphaModMap;

    rbias = gc->state.pixel.transferMode.bias.r;
    gbias = gc->state.pixel.transferMode.bias.g;
    bbias = gc->state.pixel.transferMode.bias.b;
    abias = gc->state.pixel.transferMode.bias.a;
    rscale = gc->state.pixel.transferMode.scale.r;
    gscale = gc->state.pixel.transferMode.scale.g;
    bscale = gc->state.pixel.transferMode.scale.b;
    ascale = gc->state.pixel.transferMode.scale.a;
    if (mapColor) {
	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
	aamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	aasizeMinusOne = aamap->size - 1;
    }

    for (i=0; i<256; i++) {
	alpha = red = green = blue = i / 255.0f;

	red = red * rscale + rbias;
	green = green * gscale + gbias;
	blue = blue * bscale + bbias;
	alpha = alpha * ascale + abias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    red = rrmap->base.mapF[entry];

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    green = ggmap->base.mapF[entry];

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    blue = bbmap->base.mapF[entry];

	    entry = (GLint)(alpha * aasizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > aasizeMinusOne) entry = aasizeMinusOne;
	    alpha = aamap->base.mapF[entry];
	} else {
	    if (red > __glOne) red = __glOne;
	    else if (red < __glZero) red = __glZero;
	    if (green > __glOne) green = __glOne;
	    else if (green < __glZero) green = __glZero;
	    if (blue > __glOne) blue = __glOne;
	    else if (blue < __glZero) blue = __glZero;
	    if (alpha > __glOne) alpha = __glOne;
	    else if (alpha < __glZero) alpha = __glZero;
	}

	redMap[i] = red * gc->frontBuffer.redScale;
	greenMap[i] = green * gc->frontBuffer.greenScale;
	blueMap[i] = blue * gc->frontBuffer.blueScale;
	alphaMap[i] = alpha * gc->frontBuffer.alphaScale;
    }
}

/*
** Build lookup tables to perform automatic modification of color index to 
** color index when the type is UNSIGNED_BYTE.
*/
void __glBuildItoIModifyTables(__GLcontext *gc, __GLpixelMachine *pm)
{
    GLint indexOffset, indexShift;
    __GLfloat indexScale;
    __GLpixelMapHead *iimap = NULL;
    GLint iimask = 0;
    GLboolean mapColor;
    GLfloat *indexMap;
    GLint i;
    GLint entry;
    __GLfloat index;
    GLint mask;

    mapColor = gc->state.pixel.transferMode.mapColor;
    mask = gc->frontBuffer.redMax;
    pm->iToICurrent = GL_TRUE;

    indexMap = pm->iToIMap;
    if (indexMap == NULL) {
	indexMap = pm->iToIMap = (GLfloat*)
	    (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
    }

    indexOffset = gc->state.pixel.transferMode.indexOffset;
    indexShift = gc->state.pixel.transferMode.indexShift;
    if (indexShift >= 0) {
	indexScale = (__GLfloat) (1 << indexShift);
    } else {
	indexScale = __glOne/(GLuint) (1 << (-indexShift));
    }

    if (mapColor) {
	iimap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_I];
	iimask = iimap->size - 1;
    }

    for (i=0; i<256; i++) {
	index = i * indexScale + indexOffset;
	if (mapColor) {
            if (index < 0) {
                entry = (GLint)(index - __glHalf);
            } else {
                entry = (GLint)(index + __glHalf);
            }
	    index = (__GLfloat) (iimap->base.mapI[entry & iimask]);
	}
	indexMap[i] = (GLfloat) (((GLint) index) & mask);
    }
}

/*
** Build lookup tables to perform automatic modification of color index to 
** RGBA when the type is UNSIGNED_BYTE.
*/
void __glBuildItoRGBAModifyTables(__GLcontext *gc, __GLpixelMachine *pm)
{
    GLint indexOffset, indexShift;
    __GLfloat indexScale;
    __GLpixelMapHead *irmap, *igmap, *ibmap, *iamap;
    GLint irmask, igmask, ibmask, iamask;
    GLfloat *redMap, *greenMap, *blueMap, *alphaMap;
    __GLfloat index;
    GLint entry;
    GLint i;

    pm->iToRGBACurrent = GL_TRUE;

    redMap = pm->iToRMap;
    if (redMap == NULL) {
	/* First time allocation of these maps */
	redMap = pm->iToRMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->iToGMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->iToBMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
	pm->iToAMap =
	    (GLfloat*) (*gc->imports.malloc)(gc, 256 * sizeof(GLfloat));
    }
    greenMap = pm->iToGMap;
    blueMap = pm->iToBMap;
    alphaMap = pm->iToAMap;

    indexOffset = gc->state.pixel.transferMode.indexOffset;
    indexShift = gc->state.pixel.transferMode.indexShift;
    if (indexShift >= 0) {
	indexScale = (__GLfloat) ((GLuint) (1 << indexShift));
    } else {
	indexScale = __glOne/(GLuint) (1 << (-indexShift));
    }

    irmap = 
	&gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_R];
    irmask = irmap->size - 1;
    igmap = 
	&gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_G];
    igmask = igmap->size - 1;
    ibmap = 
	&gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_B];
    ibmask = ibmap->size - 1;
    iamap = 
	&gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_A];
    iamask = iamap->size - 1;

    for (i=0; i<256; i++) {
	index = i * indexScale + indexOffset;
        if (index < 0) {
            entry = (GLint)(index - __glHalf);
        } else {
            entry = (GLint)(index + __glHalf);
        }
	redMap[i] = irmap->base.mapF[entry & irmask] * 
		gc->frontBuffer.redScale;
	greenMap[i] = igmap->base.mapF[entry & igmask] *
		gc->frontBuffer.greenScale;
	blueMap[i] = ibmap->base.mapF[entry & ibmask] * 
		gc->frontBuffer.blueScale;
	alphaMap[i] = iamap->base.mapF[entry & iamask] *
		gc->frontBuffer.alphaScale;
    }
}

/*
** Modify a RGBA, FLOAT span.  On the way out, the RGBA span will have 
** been modified as needed, and also scaled by the color scaling factors.
*/
void __glSpanModifyRGBA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    __GLfloat rbias, gbias, bbias, abias;
    __GLfloat rscale, gscale, bscale, ascale;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL, *aamap = NULL;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0, aasizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	gbias = gc->state.pixel.transferMode.bias.g;
	bbias = gc->state.pixel.transferMode.bias.b;
	abias = gc->state.pixel.transferMode.bias.a;
	rscale = gc->state.pixel.transferMode.scale.r;
	gscale = gc->state.pixel.transferMode.scale.g;
	bscale = gc->state.pixel.transferMode.scale.b;
	ascale = gc->state.pixel.transferMode.scale.a;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
	aamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	aasizeMinusOne = aamap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	gbias = gc->state.pixel.transferMode.bias.g * 
		gc->frontBuffer.greenScale;
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	abias = gc->state.pixel.transferMode.bias.a *
		gc->frontBuffer.alphaScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
	ascale = gc->state.pixel.transferMode.scale.a *
		gc->frontBuffer.alphaScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	red = *oldData++ * rscale + rbias;
	green = *oldData++ * gscale + gbias;
	blue = *oldData++ * bscale + bbias;
	alpha = *oldData++ * ascale + abias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;

	    entry = (GLint)(alpha * aasizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > aasizeMinusOne) entry = aasizeMinusOne;
	    *newData++ = aamap->base.mapF[entry] * gc->frontBuffer.alphaScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    if (alpha > gc->frontBuffer.alphaScale) {
		alpha = gc->frontBuffer.alphaScale;
	    } else if (alpha < __glZero) alpha = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	    *newData++ = alpha;
	}
    }
}

/*
** Modify a ABGR, FLOAT span.  The span is converted to RGBA. 
** On the way out, the RGBA span will have 
** been modified as needed, and also scaled by the color scaling factors.
*/
void __glSpanModifyABGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    __GLfloat rbias, gbias, bbias, abias;
    __GLfloat rscale, gscale, bscale, ascale;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL, *aamap = NULL;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0, aasizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	gbias = gc->state.pixel.transferMode.bias.g;
	bbias = gc->state.pixel.transferMode.bias.b;
	abias = gc->state.pixel.transferMode.bias.a;
	rscale = gc->state.pixel.transferMode.scale.r;
	gscale = gc->state.pixel.transferMode.scale.g;
	bscale = gc->state.pixel.transferMode.scale.b;
	ascale = gc->state.pixel.transferMode.scale.a;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
	aamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	aasizeMinusOne = aamap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	gbias = gc->state.pixel.transferMode.bias.g * 
		gc->frontBuffer.greenScale;
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	abias = gc->state.pixel.transferMode.bias.a *
		gc->frontBuffer.alphaScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
	ascale = gc->state.pixel.transferMode.scale.a *
		gc->frontBuffer.alphaScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	alpha = *oldData++ * ascale + abias;
	blue = *oldData++ * bscale + bbias;
	green = *oldData++ * gscale + gbias;
	red = *oldData++ * rscale + rbias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;

	    entry = (GLint)(alpha * aasizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > aasizeMinusOne) entry = aasizeMinusOne;
	    *newData++ = aamap->base.mapF[entry] * gc->frontBuffer.alphaScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    if (alpha > gc->frontBuffer.alphaScale) {
		alpha = gc->frontBuffer.alphaScale;
	    } else if (alpha < __glZero) alpha = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	    *newData++ = alpha;
	}
    }
}

/*
** Modify a BGRA, FLOAT span.  The span is converted to RGBA. 
** On the way out, the RGBA span will have 
** been modified as needed, and also scaled by the color scaling factors.
*/
void __glSpanModifyBGRA(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    __GLfloat rbias, gbias, bbias, abias;
    __GLfloat rscale, gscale, bscale, ascale;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL, *aamap = NULL;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0, aasizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	gbias = gc->state.pixel.transferMode.bias.g;
	bbias = gc->state.pixel.transferMode.bias.b;
	abias = gc->state.pixel.transferMode.bias.a;
	rscale = gc->state.pixel.transferMode.scale.r;
	gscale = gc->state.pixel.transferMode.scale.g;
	bscale = gc->state.pixel.transferMode.scale.b;
	ascale = gc->state.pixel.transferMode.scale.a;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
	aamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	aasizeMinusOne = aamap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	gbias = gc->state.pixel.transferMode.bias.g * 
		gc->frontBuffer.greenScale;
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	abias = gc->state.pixel.transferMode.bias.a *
		gc->frontBuffer.alphaScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
	ascale = gc->state.pixel.transferMode.scale.a *
		gc->frontBuffer.alphaScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	blue = *oldData++ * bscale + bbias;
	green = *oldData++ * gscale + gbias;
	red = *oldData++ * rscale + rbias;
	alpha = *oldData++ * ascale + abias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;

	    entry = (GLint)(alpha * aasizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > aasizeMinusOne) entry = aasizeMinusOne;
	    *newData++ = aamap->base.mapF[entry] * gc->frontBuffer.alphaScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    if (alpha > gc->frontBuffer.alphaScale) {
		alpha = gc->frontBuffer.alphaScale;
	    } else if (alpha < __glZero) alpha = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	    *newData++ = alpha;
	}
    }
}

/*
** Modify a RED, FLOAT span.  On the way out, the RED span will have been
** converted into a RGBA span, modified as needed, and also scaled by the 
** color scaling factors.
*/
void __glSpanModifyRed(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat rbias;
    __GLfloat rscale;
    __GLpixelMapHead *rrmap = NULL;
    GLint rrsizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    pm = &(gc->pixel);
    green = pm->green0Mod;
    blue = pm->blue0Mod;
    alpha = pm->alpha1Mod;
    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	rscale = gc->state.pixel.transferMode.scale.r;
	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r *
		gc->frontBuffer.redScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	red = *oldData++ * rscale + rbias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;

	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    *newData++ = red;
	}

	*newData++ = green;
	*newData++ = blue;
	*newData++ = alpha;
    }
}

/*
** Modify a GREEN, FLOAT span.  On the way out, the GREEN span will have been
** converted into a RGBA span, modified as needed, and also scaled by the 
** color scaling factors.
*/
void __glSpanModifyGreen(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat gbias;
    __GLfloat gscale;
    __GLpixelMapHead *ggmap = NULL;
    GLint ggsizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    pm = &(gc->pixel);
    red = pm->red0Mod;
    blue = pm->blue0Mod;
    alpha = pm->alpha1Mod;
    if (mapColor) {
	gbias = gc->state.pixel.transferMode.bias.g;
	gscale = gc->state.pixel.transferMode.scale.g;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
    } else {
	gbias = gc->state.pixel.transferMode.bias.g *
		gc->frontBuffer.greenScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	green = *oldData++ * gscale + gbias;
	*newData++ = red;
	if (mapColor) {
	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;
	} else {
	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    *newData++ = green;
	}

	*newData++ = blue;
	*newData++ = alpha;
    }
}

/*
** Modify a BLUE, FLOAT span.  On the way out, the BLUE span will have been
** converted into a RGBA span, modified as needed, and also scaled by the 
** color scaling factors.
*/
void __glSpanModifyBlue(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		        const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat bbias;
    __GLfloat bscale;
    __GLpixelMapHead *bbmap = NULL;
    GLint bbsizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    pm = &(gc->pixel);
    red = pm->red0Mod;
    green = pm->green0Mod;
    alpha = pm->alpha1Mod;
    if (mapColor) {
	bbias = gc->state.pixel.transferMode.bias.b;
	bscale = gc->state.pixel.transferMode.scale.b;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
    } else {
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	blue = *oldData++ * bscale + bbias;
	*newData++ = red;
	*newData++ = green;
	if (mapColor) {
	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;
	} else {
	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    *newData++ = blue;
	}

	*newData++ = alpha;
    }
}

/*
** Modify an ALPHA, FLOAT span.  On the way out, the ALPHA span will have been
** converted into a RGBA span, modified as needed, and also scaled by the 
** color scaling factors.
*/
void __glSpanModifyAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat abias;
    __GLfloat ascale;
    __GLpixelMapHead *aamap = NULL;
    GLint aasizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    pm = &(gc->pixel);
    red = pm->red0Mod;
    green = pm->green0Mod;
    blue = pm->blue0Mod;
    if (mapColor) {
	abias = gc->state.pixel.transferMode.bias.a;
	ascale = gc->state.pixel.transferMode.scale.a;
	aamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	aasizeMinusOne = aamap->size - 1;
    } else {
	abias = gc->state.pixel.transferMode.bias.a *
		gc->frontBuffer.alphaScale;
	ascale = gc->state.pixel.transferMode.scale.a *
		gc->frontBuffer.alphaScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	alpha = *oldData++ * ascale + abias;
	*newData++ = red;
	*newData++ = green;
	*newData++ = blue;
	if (mapColor) {
	    entry = (GLint)(alpha * aasizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > aasizeMinusOne) entry = aasizeMinusOne;
	    *newData++ = aamap->base.mapF[entry] * gc->frontBuffer.alphaScale;
	} else {
	    if (alpha > gc->frontBuffer.alphaScale) {
		alpha = gc->frontBuffer.alphaScale;
	    } else if (alpha < __glZero) alpha = __glZero;

	    *newData++ = alpha;
	}
    }
}

/*
** Modify a RGB, FLOAT span.  On the way out, the RGB span will have been
** converted into a RGBA span, modified as needed, and also scaled by the 
** color scaling factors.
*/
void __glSpanModifyRGB(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat rbias, gbias, bbias;
    __GLfloat rscale, gscale, bscale;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    pm = &(gc->pixel);
    mapColor = gc->state.pixel.transferMode.mapColor;

    alpha = pm->alpha1Mod;

    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	gbias = gc->state.pixel.transferMode.bias.g;
	bbias = gc->state.pixel.transferMode.bias.b;
	rscale = gc->state.pixel.transferMode.scale.r;
	gscale = gc->state.pixel.transferMode.scale.g;
	bscale = gc->state.pixel.transferMode.scale.b;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	gbias = gc->state.pixel.transferMode.bias.g * 
		gc->frontBuffer.greenScale;
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	red = *oldData++ * rscale + rbias;
	green = *oldData++ * gscale + gbias;
	blue = *oldData++ * bscale + bbias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	}

	*newData++ = alpha;
    }
}

/*
** Modify a BGR, FLOAT span.  On the way out, the BGR span will have been
** converted into a RGBA span, modified as needed, and also scaled by the 
** color scaling factors.
*/
void __glSpanModifyBGR(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		       const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat rbias, gbias, bbias;
    __GLfloat rscale, gscale, bscale;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    pm = &(gc->pixel);
    mapColor = gc->state.pixel.transferMode.mapColor;

    alpha = pm->alpha1Mod;

    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	gbias = gc->state.pixel.transferMode.bias.g;
	bbias = gc->state.pixel.transferMode.bias.b;
	rscale = gc->state.pixel.transferMode.scale.r;
	gscale = gc->state.pixel.transferMode.scale.g;
	bscale = gc->state.pixel.transferMode.scale.b;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	gbias = gc->state.pixel.transferMode.bias.g * 
		gc->frontBuffer.greenScale;
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	blue = *oldData++ * bscale + bbias;
	green = *oldData++ * gscale + gbias;
	red = *oldData++ * rscale + rbias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	}

	*newData++ = alpha;
    }
}

/*
** Modify a LUMINANCE, FLOAT span.  On the way out, the LUMINANCE span will 
** have been converted into a RGBA span, modified as needed, and also scaled 
** by the color scaling factors.
*/
void __glSpanModifyLuminance(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		             const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat rbias, gbias, bbias;
    __GLfloat rscale, gscale, bscale;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    pm = &(gc->pixel);
    mapColor = gc->state.pixel.transferMode.mapColor;

    alpha = pm->alpha1Mod;

    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	gbias = gc->state.pixel.transferMode.bias.g;
	bbias = gc->state.pixel.transferMode.bias.b;
	rscale = gc->state.pixel.transferMode.scale.r;
	gscale = gc->state.pixel.transferMode.scale.g;
	bscale = gc->state.pixel.transferMode.scale.b;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	gbias = gc->state.pixel.transferMode.bias.g * 
		gc->frontBuffer.greenScale;
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	red = *oldData * rscale + rbias;
	green = *oldData * gscale + gbias;
	blue = *oldData++ * bscale + bbias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	}

	*newData++ = alpha;
    }
}

/*
** Modify a LUMINANCE_ALPHA, FLOAT span.  On the way out, the LUMINANCE_ALPHA 
** span will have been converted into a RGBA span, modified as needed, and 
** also scaled by the color scaling factors.
*/
void __glSpanModifyLuminanceAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		                  const GLvoid *inspan, GLvoid *outspan)
{
    __GLfloat rbias, gbias, bbias, abias;
    __GLfloat rscale, gscale, bscale, ascale;
    __GLpixelMapHead *rrmap = NULL, *ggmap = NULL, 
                     *bbmap = NULL, *aamap = NULL;
    GLint rrsizeMinusOne = 0, ggsizeMinusOne = 0, 
          bbsizeMinusOne = 0, aasizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	gbias = gc->state.pixel.transferMode.bias.g;
	bbias = gc->state.pixel.transferMode.bias.b;
	abias = gc->state.pixel.transferMode.bias.a;
	rscale = gc->state.pixel.transferMode.scale.r;
	gscale = gc->state.pixel.transferMode.scale.g;
	bscale = gc->state.pixel.transferMode.scale.b;
	ascale = gc->state.pixel.transferMode.scale.a;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	ggmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	ggsizeMinusOne = ggmap->size - 1;
	bbmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	bbsizeMinusOne = bbmap->size - 1;
	aamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	aasizeMinusOne = aamap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	gbias = gc->state.pixel.transferMode.bias.g * 
		gc->frontBuffer.greenScale;
	bbias = gc->state.pixel.transferMode.bias.b *
		gc->frontBuffer.blueScale;
	abias = gc->state.pixel.transferMode.bias.a *
		gc->frontBuffer.alphaScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	gscale = gc->state.pixel.transferMode.scale.g *
		gc->frontBuffer.greenScale;
	bscale = gc->state.pixel.transferMode.scale.b *
		gc->frontBuffer.blueScale;
	ascale = gc->state.pixel.transferMode.scale.a *
		gc->frontBuffer.alphaScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	red = *oldData * rscale + rbias;
	green = *oldData * gscale + gbias;
	blue = *oldData++ * bscale + bbias;
	alpha = *oldData++ * ascale + abias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * ggsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > ggsizeMinusOne) entry = ggsizeMinusOne;
	    *newData++ = ggmap->base.mapF[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bbsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > bbsizeMinusOne) entry = bbsizeMinusOne;
	    *newData++ = bbmap->base.mapF[entry] * gc->frontBuffer.blueScale;

	    entry = (GLint)(alpha * aasizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > aasizeMinusOne) entry = aasizeMinusOne;
	    *newData++ = aamap->base.mapF[entry] * gc->frontBuffer.alphaScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (green > gc->frontBuffer.greenScale) {
		green = gc->frontBuffer.greenScale;
	    } else if (green < __glZero) green = __glZero;

	    if (blue > gc->frontBuffer.blueScale) {
		blue = gc->frontBuffer.blueScale;
	    } else if (blue < __glZero) blue = __glZero;

	    if (alpha > gc->frontBuffer.alphaScale) {
		alpha = gc->frontBuffer.alphaScale;
	    } else if (alpha < __glZero) alpha = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	    *newData++ = alpha;
	}
    }
}

/*
** Modify a RED_ALPHA, FLOAT span.  On the way out, the RED_ALPHA span will 
** have been converted into a RGBA span, modified as needed, and also scaled 
** by the color scaling factors.
**
** A RED_ALPHA span comes from a two component texture (where the spec 
** takes the first component from RED for some reason rather than the more
** typical recombination of r, g and b, as is done in ReadPixels).
*/
void __glSpanModifyRedAlpha(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			    const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMachine *pm;
    __GLfloat rbias, abias;
    __GLfloat rscale, ascale;
    __GLpixelMapHead *rrmap = NULL, *aamap = NULL;
    GLint rrsizeMinusOne = 0, aasizeMinusOne = 0;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLfloat red, green, blue, alpha;

    mapColor = gc->state.pixel.transferMode.mapColor;

    pm = &(gc->pixel);

    green = pm->green0Mod;
    blue = pm->blue0Mod;
    if (mapColor) {
	rbias = gc->state.pixel.transferMode.bias.r;
	abias = gc->state.pixel.transferMode.bias.a;
	rscale = gc->state.pixel.transferMode.scale.r;
	ascale = gc->state.pixel.transferMode.scale.a;

	rrmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	rrsizeMinusOne = rrmap->size - 1;
	aamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	aasizeMinusOne = aamap->size - 1;
    } else {
	rbias = gc->state.pixel.transferMode.bias.r * 
		gc->frontBuffer.redScale;
	abias = gc->state.pixel.transferMode.bias.a *
		gc->frontBuffer.alphaScale;
	rscale = gc->state.pixel.transferMode.scale.r *
		gc->frontBuffer.redScale;
	ascale = gc->state.pixel.transferMode.scale.a *
		gc->frontBuffer.alphaScale;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	red = *oldData * rscale + rbias;
	alpha = *oldData++ * ascale + abias;
	if (mapColor) {
	    entry = (GLint)(red * rrsizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > rrsizeMinusOne) entry = rrsizeMinusOne;
	    *newData++ = rrmap->base.mapF[entry] * gc->frontBuffer.redScale;

	    *newData++ = green;
	    *newData++ = blue;

	    entry = (GLint)(alpha * aasizeMinusOne + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > aasizeMinusOne) entry = aasizeMinusOne;
	    *newData++ = aamap->base.mapF[entry] * gc->frontBuffer.alphaScale;
	} else {
	    if (red > gc->frontBuffer.redScale) {
		red = gc->frontBuffer.redScale;
	    } else if (red < __glZero) red = __glZero;

	    if (alpha > gc->frontBuffer.alphaScale) {
		alpha = gc->frontBuffer.alphaScale;
	    } else if (alpha < __glZero) alpha = __glZero;

	    *newData++ = red;
	    *newData++ = green;
	    *newData++ = blue;
	    *newData++ = alpha;
	}
    }
}

/*
** Modify a DEPTH, FLOAT span.  On the way out, the DEPTH span will have been
** modified as needed.
*/
void __glSpanModifyDepth(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		         const GLvoid *inspan, GLvoid *outspan)
{
    __GLfloat dbias;
    __GLfloat dscale;
    const GLfloat *oldData;
    GLfloat *newData;
    GLfloat d;
    GLfloat one, zero;
    GLint i;
    GLint width;

    dbias = gc->state.pixel.transferMode.depthBias;
    dscale = gc->state.pixel.transferMode.depthScale;
    one = __glOne;
    zero = __glZero;

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	d = *oldData++ * dscale + dbias;
	if (d < zero) d = zero;
	else if (d > one) d = one;
	*newData++ = d;
    }
}

/*
** Modify a STENCIL_INDEX, FLOAT span.  On the way out, the STENCIL_INDEX span 
** will have been modified as needed.
*/
void __glSpanModifyStencil(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		           const GLvoid *inspan, GLvoid *outspan)
{
    __GLpixelMapHead *ssmap = NULL;
    GLint ssmask = 0;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;
    GLboolean mapStencil;
    GLint indexOffset, indexShift;

    indexOffset = gc->state.pixel.transferMode.indexOffset;
    indexShift = gc->state.pixel.transferMode.indexShift;
    mapStencil = gc->state.pixel.transferMode.mapStencil;
    if (mapStencil) {
	ssmap = &gc->pixel.pixelMap[__GL_PIXEL_MAP_S_TO_S];
	ssmask = ssmap->size - 1;
    }

    /*
    ** Large positive shifts cause accuracy problems when casting to
    ** GLfloat, so shift beyond the reach of the mask when appropriate.
    */
    if (indexShift > gc->modes.stencilBits) {
	indexShift = indexShift % (sizeof(GLint)*8);
	if (indexShift > gc->modes.stencilBits)
	    indexShift = gc->modes.stencilBits;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;

    /* Do a separate loop for each case to avoid tests inside the loop */
    if (indexShift >= 0) {
	if (mapStencil) {
	    for (i=0; i<width; i++) {
		entry = ((GLint)*oldData++ << indexShift) + indexOffset;
		*newData++ = (GLfloat) (ssmap->base.mapI[entry & ssmask]);
	    }
        } else /* mapping off */ {
	    for (i=0; i<width; i++) {
		*newData++ = (GLfloat) (((GLint)*oldData++ << indexShift) + indexOffset);
	    }
	}
    } else /* negative shift */ {
        if (mapStencil) {
	    for (i=0; i<width; i++) {
		entry = ((GLint)*oldData++ >> -indexShift) + indexOffset;
		*newData++ = (GLfloat) (ssmap->base.mapI[entry & ssmask]);
	    }
        } else /* mapping off */ {
	    for (i=0; i<width; i++) {
		*newData++ = (GLfloat) (((GLint)*oldData++ >> -indexShift) + indexOffset);
	    }
        }
    }
}

/*
** Modify a COLOR_INDEX, FLOAT span.  On the way out, the COLOR_INDEX span 
** will have been modified as needed.
*/
void __glSpanModifyCI(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		      const GLvoid *inspan, GLvoid *outspan)
{
    GLint indexOffset, indexShift;
    __GLpixelMapHead *iimap = NULL, *irmap = NULL, 
                     *igmap = NULL, *ibmap = NULL, 
                     *iamap = NULL;
    GLint iimask = 0, irmask = 0, igmask = 0, 
          ibmask = 0, iamask = 0;
    GLfloat rscale = 0.0f, gscale = 0.0f, 
            bscale = 0.0f, ascale = 0.0f;
    GLboolean mapColor;
    const GLfloat *oldData;
    GLfloat *newData;
    GLint i;
    GLint width;
    GLint entry;

    mapColor = gc->state.pixel.transferMode.mapColor;

    indexOffset = gc->state.pixel.transferMode.indexOffset;
    indexShift = gc->state.pixel.transferMode.indexShift;
    if (spanInfo->dstFormat != GL_COLOR_INDEX) {
	irmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_R];
	irmask = irmap->size - 1;
	igmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_G];
	igmask = igmap->size - 1;
	ibmap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_B];
	ibmask = ibmap->size - 1;
	iamap = 
	    &gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_A];
	iamask = iamap->size - 1;
	rscale = gc->frontBuffer.redScale;
	gscale = gc->frontBuffer.greenScale;
	bscale = gc->frontBuffer.blueScale;
	ascale = gc->frontBuffer.alphaScale;
    } else {
	if (mapColor) {
	    iimap = &gc->pixel.pixelMap[__GL_PIXEL_MAP_I_TO_I];
	    iimask = iimap->size - 1;
	}
    }

    /*
    ** Large positive shifts cause accuracy problems when casting to
    ** GLfloat, so shift beyond the reach of the mask when appropriate.
    */
    if (indexShift > gc->modes.indexBits) {
	indexShift = indexShift % (sizeof(GLint)*8);
	if (indexShift > gc->modes.indexBits) indexShift = gc->modes.indexBits;
    }

    oldData = (const GLfloat*) inspan;
    newData = (GLfloat*) outspan;
    width = spanInfo->width;
    for (i=0; i<width; i++) {
	if (indexShift >= 0) {
	    entry = ((GLint)*oldData++ << indexShift) + indexOffset;
	} else {
	    entry = ((GLint)*oldData++ >> -indexShift) + indexOffset;
	}
	if (spanInfo->dstFormat != GL_COLOR_INDEX) {
	    *newData++ = irmap->base.mapF[entry & irmask] * rscale;
	    *newData++ = igmap->base.mapF[entry & igmask] * gscale;
	    *newData++ = ibmap->base.mapF[entry & ibmask] * bscale;
	    *newData++ = iamap->base.mapF[entry & iamask] * ascale;
	} else if (mapColor) {
	    *newData++ = (GLfloat) (iimap->base.mapI[entry & iimask]);
	} else {
	    *newData++ = (GLfloat) entry;
	}
    }
}

/*
** Apply generic scale and bias to RGBA float spans
*/
/*ARGSUSED*/
void __glSpanGenericScaleBiasRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    GLint width = spanInfo->width; 
    const GLfloat *in = (const GLfloat *) inspan;
    GLfloat *out = (GLfloat *) outspan;
    GLfloat rs, gs, bs, as;
    GLfloat rb, gb, bb, ab;

    rs = spanInfo->scale.r;
    gs = spanInfo->scale.g;
    bs = spanInfo->scale.b;
    as = spanInfo->scale.a;

    rb = spanInfo->bias.r;
    gb = spanInfo->bias.g;
    bb = spanInfo->bias.b;
    ab = spanInfo->bias.a;

    while ( width-- > 0 ) {
	*out++ = *in++ * rs + rb;
	*out++ = *in++ * gs + gb;
	*out++ = *in++ * bs + bb;
	*out++ = *in++ * as + ab;
    }
}

/*
** Apply a ColorTable to a FLOAT RGBA span.  
*/
static void ApplyColorTableRGBA(__GLcontext *gc, __GLcolorTable *ct,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    const GLfloat *src = (const GLfloat *)inspan;
    GLfloat *dst = (GLfloat *)outspan;
    GLint width = spanInfo->width;
    GLfloat *table = (GLfloat *) ct->table;
    GLint maxEntry = ct->width - 1;
    GLfloat rScale = (GLfloat) maxEntry * gc->frontBuffer.oneOverRedScale;
    GLfloat gScale = (GLfloat) maxEntry * gc->frontBuffer.oneOverGreenScale;
    GLfloat bScale = (GLfloat) maxEntry * gc->frontBuffer.oneOverBlueScale;
    GLfloat aScale = (GLfloat) maxEntry * gc->frontBuffer.oneOverAlphaScale;

    while ( width-- > 0 ) {
	GLfloat red = *src++;
	GLfloat green = *src++;
	GLfloat blue = *src++;
	GLfloat alpha = *src++;
	GLint entry;

	/*
	** To compute index into color table: scale to [0, width-1],
	** round, clamp to [0, width-1], adjust index to number
	** of components in each table entry.
	*/
	/*
	** XXX If components are clamped when they get here,
	** we can skip clamping.
	*/
	switch (ct->baseFormat) {
	case GL_ALPHA:
	    entry = (GLint)(alpha * aScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    alpha = table[entry] * gc->frontBuffer.alphaScale;
	    break;
	case GL_LUMINANCE:
	    entry = (GLint)(red * rScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    red = table[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * gScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    green = table[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    blue = table[entry] * gc->frontBuffer.blueScale;
	    break;
	case GL_LUMINANCE_ALPHA:
	    entry = (GLint)(red * rScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    red = table[2*entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * gScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    green = table[2*entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    blue = table[2*entry] * gc->frontBuffer.blueScale;

	    entry = (GLint)(alpha * aScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    alpha = table[2*entry+1] * gc->frontBuffer.alphaScale;
	    break;
	case GL_INTENSITY:
	    entry = (GLint)(red * rScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    red = table[entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * gScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    green = table[entry] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    blue = table[entry] * gc->frontBuffer.blueScale;

	    entry = (GLint)(alpha * aScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    alpha = table[entry] * gc->frontBuffer.alphaScale;
	    break;
	case GL_RGB:
	    entry = (GLint)(red * rScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    red = table[3*entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * gScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    green = table[3*entry+1] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    blue = table[3*entry+2] * gc->frontBuffer.blueScale;
	    break;
	case GL_RGBA:
	    entry = (GLint)(red * rScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    red = table[4*entry] * gc->frontBuffer.redScale;

	    entry = (GLint)(green * gScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    green = table[4*entry+1] * gc->frontBuffer.greenScale;

	    entry = (GLint)(blue * bScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    blue = table[4*entry+2] * gc->frontBuffer.blueScale;

	    entry = (GLint)(alpha * aScale + __glHalf);
	    if (entry < 0) {
		entry = 0;
	    } else if (entry > maxEntry) {
		entry = maxEntry;
	    }
	    alpha = table[4*entry+3] * gc->frontBuffer.alphaScale;
	    break;
	default:
	    assert(0); /* should be one of above types */
	    break;
	}

	*dst++ = red;
	*dst++ = green;
	*dst++ = blue;
	*dst++ = alpha;
    }
}

void __glSpanColorTableRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    ApplyColorTableRGBA(gc,
	&gc->pixel.colorTable[__GL_COLOR_TABLE_INDEX],
	spanInfo, inspan, outspan);
}

void __glSpanPostConvolutionColorTableRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    ApplyColorTableRGBA(gc,
	&gc->pixel.colorTable[__GL_POST_CONVOLUTION_COLOR_TABLE_INDEX],
	spanInfo, inspan, outspan);
}

void __glSpanPostColorMatrixColorTableRGBA(__GLcontext *gc,
	    __GLpixelSpanInfo *spanInfo, const GLvoid *inspan, GLvoid *outspan)
{
    ApplyColorTableRGBA(gc,
	&gc->pixel.colorTable[__GL_POST_COLOR_MATRIX_COLOR_TABLE_INDEX],
	spanInfo, inspan, outspan);
}

