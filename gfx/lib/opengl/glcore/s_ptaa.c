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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_ptaa.c#7 $
*/
#include "render.h"
#include "math.h"
#include "context.h"
#include "global.h"

/*
** This is a little wierd.  What it does is to dither "comp" into the high
** n-4 bits, and add 16 * antiAliasPercent.  Dithering of the low bits is
** left to the usual methods (the store and span procs, for example).
*/
__GLfloat __glBuildAntiAliasIndex(__GLfloat index, 
				  __GLfloat antiAliasPercent)
{
    GLint newlowbits;

    newlowbits = (GLint)((__GL_CI_ANTI_ALIAS_DIVISOR - 1) * antiAliasPercent + (__GLfloat) 0.5);
    return (((int) index) & ~(__GL_CI_ANTI_ALIAS_DIVISOR - 1)) | newlowbits;
}

/************************************************************************/

/*
** To anti-alias points the below code operates a simple algrorithim that
** sub-samples the bounding box of the pixel area covered by the point.
** At each sub-sample the distance from the sample to the center of the
** point is computed and compared against the distance from the edge of
** the circle to the center.  If the computed distance is <= the edge
** distance then the sample is inside the circle.  All the samples for a
** particular pixel center are summed up and then the resulting value is
** divided by the total samples in the pixel.  This gives us a coverage value
** to use to adjust the fragment alpha with before storing (there is
** an analagous affect when color index anti-aliasing is being done).
**
** The code below implements this simple algrorithim, but has been tuned
** so it might be difficult to translate.  Basically, every possible operation
** that could be moved out of the Coverage code (i.e., invariants across
** the coverage test) has been done.  Also, the minimal area is sampled
** over.
*/

/* Code below knows alot about these constants so beware */
#define	__GL_FILTER_SIZE	__glOne
#define __GL_HALF_FILTER_SIZE	__glHalf
#define __GL_SAMPLES		4
#define __GL_SAMPLE_HIT		((__GLfloat) 0.0625)	/* 1 / (4*4) */
#define __GL_SAMPLE_DELTA	((__GLfloat) 0.25)	/* 1 / 4 */
#define __GL_HALF_SAMPLE_DELTA	((__GLfloat) 0.125)
/* -halffilter + half delta */
#define __GL_COORD_ADJUST	((__GLfloat) -0.375)

/*
** Return an estimate of the pixel coverage using sub-sampling.
**
** NOTE: The subtraction of xCenter,yCenter has been moved into the
** caller to save time.  Consequently the starting coordinate may not be
** on a pixel center, but thats ok.
*/
static __GLfloat Coverage(__GLfloat xStart, __GLfloat yStart,
			  __GLfloat radiusSquared)
{
    GLint i;
    __GLfloat delta, yBottom, sampleX, sampleY;
    __GLfloat hits, hitsInc;

    /*
    ** Get starting sample x & y positions.  We take our starting
    ** coordinate, back off half a filter size then add half a delta to
    ** it.  This constrains the sampling to lie entirely within the
    ** pixel, never on the edges of the pixel.  The constants above
    ** have this adjustment pre-computed.
    */
    sampleX = xStart + __GL_COORD_ADJUST;
    yBottom = yStart + __GL_COORD_ADJUST;

    delta = __GL_SAMPLE_DELTA;
    hits = __glZero;
    hitsInc = __GL_SAMPLE_HIT;
    for (i = __GL_SAMPLES; --i >= 0; ) {
	__GLfloat check = radiusSquared - sampleX * sampleX;

	/* Unrolled inner loop - change this code if __GL_SAMPLES changes */
	sampleY = yBottom;
	if (sampleY * sampleY <= check) {
	    hits += hitsInc;
	}
	sampleY += delta;
	if (sampleY * sampleY <= check) {
	    hits += hitsInc;
	}
	sampleY += delta;
	if (sampleY * sampleY <= check) {
	    hits += hitsInc;
	}
	sampleY += delta;
	if (sampleY * sampleY <= check) {
	    hits += hitsInc;
	}

	sampleX += delta;
    }
    return hits;
}

void __glRenderAntiAliasedRGBPoint(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat xCenter, yCenter, radius, radiusSquared, coverage, x, y;
    __GLfloat zero, one, oldAlpha, xStart;
    __GLfragment frag;
    GLint w, width, height, ixLeft, iyBottom;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    void (*store)(__GLcolorBuffer *, const __GLfragment *) = gc->procs.store;

    __GL_ADJUST_POINT_VERTEX(gc, vx);

    if (gc->procs.store == gc->procs.cfbStore) {
	/* We need ownership testing */
	store = __glDoStore;
    }

    /*
    ** Determine area to compute coverage over.  The area is bloated by
    ** the filter's width & height implicitly.  By truncating to integer
    ** (NOTE: the x,y coordinate is always positive here) we are
    ** guaranteed to find the lowest coordinate that needs examination
    ** because of the nature of circles.  Similarly, by truncating the
    ** ending coordinate and adding one we get the pixel just past the
    ** upper/right edge of the circle.
    */
    radius = gc->state.point.smoothSize * __glHalf;
    radiusSquared = radius * radius;
    xCenter = vx->window.x - __glHalf;
    yCenter = vx->window.y - __glHalf;

    /* Truncate down to get starting coordinate */
    ixLeft = (GLint) (xCenter - radius);
    iyBottom = (GLint) (yCenter - radius);

    /*
    ** Truncate down and add 1 to get the ending coordinate, then subtract
    ** out the start to get the width & height.
    */
    width = ((GLint) (xCenter + radius)) + 1 - ixLeft;
    height = ((GLint) (yCenter + radius)) + 1 - iyBottom;

    /*
    ** Setup fragment.  The fragment base color will be constant
    ** (approximately) across the entire pixel.  The only thing that will
    ** change is the alpha (for rgb) or the red component (for color
    ** index).
    */
    frag.z = (__GLzValue)vx->window.z;
    frag.color[__GL_PRIMARY_COLOR] = *vx->color;
    if (modeFlags & __GL_SHADE_TEXTURE) {
	__GLcoord strq[__GL_NUM_TEXTURE_UNITS];
	__GLfloat rhow[__GL_NUM_TEXTURE_UNITS];
	GLint i;

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    strq[i] = vx->texture[i];
	    rhow[i] = __glOne;
	}
	(*gc->procs.texture)(gc, &frag.color[__GL_PRIMARY_COLOR], strq, rhow);
    }
    if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
	__GLcolor  *sc = vx->color + 2;
	frag.color[__GL_PRIMARY_COLOR].r += sc->r;
	frag.color[__GL_PRIMARY_COLOR].g += sc->g;
	frag.color[__GL_PRIMARY_COLOR].b += sc->b;
	frag.color[__GL_PRIMARY_COLOR].a += sc->a;
    }
    if (modeFlags & __GL_SHADE_SLOW_FOG) {
	(*gc->procs.fogPoint)(gc, &frag, vx->eye.z);
    }

    /*
    ** Now render the circle centered on xCenter,yCenter.  Move the
    ** subtraction of xCenter,yCenter outside of the loop to doing
    ** it up front in xStart and y.  This way the coverage code can
    ** assume the incoming starting coordinate has been properly
    ** adjusted.
    */
    zero = __glZero;
    one = __glOne;
    oldAlpha = frag.color[__GL_PRIMARY_COLOR].a;
    xStart = ixLeft + __glOne - xCenter;
    y = iyBottom + __glOne - yCenter;
    frag.y = iyBottom;
    while (--height >= 0) {
	x = xStart;
	frag.x = ixLeft;
	for (w = width; --w >= 0; ) {
	    coverage = Coverage(x, y, radiusSquared);
	    if (coverage > zero) {
		frag.color[__GL_PRIMARY_COLOR].a = oldAlpha * coverage;
		(*store)(gc->drawBuffer, &frag);
	    }
	    x += one;
	    frag.x++;
	}
	y += one;
	frag.y++;
    }

    __GL_RESTORE_POINT_VERTEX(gc, vx);
}

void __glRenderAntiAliasedCIPoint(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat xCenter, yCenter, radius, radiusSquared, coverage, x, y;
    __GLfloat zero, one, oldIndex, xStart;
    __GLfragment frag;
    GLint w, width, height, ixLeft, iyBottom;
    void (*store)(__GLcolorBuffer *, const __GLfragment *) = gc->procs.store;

    __GL_ADJUST_POINT_VERTEX(gc, vx);

    if (gc->procs.store == gc->procs.cfbStore) {
	/* We need ownership testing */
	store = __glDoStore;
    }

    /*
    ** Determine area to compute coverage over.  The area is bloated by
    ** the filter's width & height implicitly.  By truncating to integer
    ** (NOTE: the x,y coordinate is always positive here) we are
    ** guaranteed to find the lowest coordinate that needs examination
    ** because of the nature of circles.  Similarly, by truncating the
    ** ending coordinate and adding one we get the pixel just past the
    ** upper/right edge of the circle.
    */
    radius = gc->state.point.smoothSize * __glHalf;
    radiusSquared = radius * radius;
    xCenter = vx->window.x - __glHalf;
    yCenter = vx->window.y - __glHalf;

    /* Truncate down to get starting coordinate */
    ixLeft = (GLint) (xCenter - radius);
    iyBottom = (GLint) (yCenter - radius);

    /*
    ** Truncate down and add 1 to get the ending coordinate, then subtract
    ** out the start to get the width & height.
    */
    width = ((GLint) (xCenter + radius)) + 1 - ixLeft;
    height = ((GLint) (yCenter + radius)) + 1 - iyBottom;

    /*
    ** Setup fragment.  The fragment base color will be constant
    ** (approximately) across the entire pixel.  The only thing that will
    ** change is the alpha (for rgb) or the red component (for color
    ** index).
    */
    frag.z = (__GLzValue)vx->window.z;
    frag.color[__GL_PRIMARY_COLOR].r = vx->color->r;
    if (gc->polygon.shader.modeFlags & __GL_SHADE_SLOW_FOG) {
	(*gc->procs.fogPoint)(gc, &frag, vx->eye.z);
    }

    /*
    ** Now render the circle centered on xCenter,yCenter.  Move the
    ** subtraction of xCenter,yCenter outside of the loop to doing
    ** it up front in xStart and y.  This way the coverage code can
    ** assume the incoming starting coordinate has been properly
    ** adjusted.
    */
    zero = __glZero;
    one = __glOne;
    oldIndex = frag.color[__GL_PRIMARY_COLOR].r;
    xStart = ixLeft + __glOne - xCenter;
    y = iyBottom + __glOne - yCenter;
    frag.y = iyBottom;
    while (--height >= 0) {
	x = xStart;
	frag.x = ixLeft;
	for (w = width; --w >= 0; ) {
	    coverage = Coverage(x, y, radiusSquared);
	    if (coverage > zero) {
		frag.color[__GL_PRIMARY_COLOR].r =
		    __glBuildAntiAliasIndex(oldIndex, coverage);
		(*store)(gc->drawBuffer, &frag);
	    }
	    x += one;
	    frag.x++;
	}
	y += one;
	frag.y++;
    }

    __GL_RESTORE_POINT_VERTEX(gc, vx);
}
