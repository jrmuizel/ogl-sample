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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_pgaa.c#10 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "g_imports.h"


/*
** Normal form of a line: Ax + By + C = 0.  When evaluated at a point P,
** the value is zero when P is on the line.  For points off the line,
** the sign of the value determines which side of the line P is on.
*/
typedef struct {
    __GLfloat a, b, c;

    /*
    ** The sign of an edge is determined by plugging the third vertex
    ** of the triangle into the line equation.  This flag is GL_TRUE when
    ** the sign is positive.
    */
    GLboolean edgeSign;
} __glLineEquation;

/*
** Machine state for rendering triangles.
*/
typedef struct {
    __GLfloat dyAB;
    __GLfloat dyBC;
    __glLineEquation ab;
    __glLineEquation bc;
    __glLineEquation ca;
    __GLfloat area;
    GLint areaSign;
} __glTriangleMachine;

/*
** Plane equation coefficients.  One plane equation exists for each of
** the parameters being computed across the surface of the triangle.
*/
typedef struct {
    __GLfloat a, b, c, d;
} __glPlaneEquation;

/*
** Cache for some of the coverage computation constants.
*/
typedef struct {
    __GLfloat dx, dy;
    GLint samples;
    GLint samplesSquared;
    __GLfloat samplesSquaredInv;
    GLboolean lastCoverageWasOne;
    __GLfloat leftDelta, rightDelta;
    __GLfloat bottomDelta, topDelta;
} __glCoverageStuff;

typedef struct {
    __glPlaneEquation sp, tp, rp, qwp, rhowp;
} __glTexturePlanes;

/*
** Cache for all the parameters being passed to FillAASubTriangle
*/
typedef struct {
    __glPlaneEquation rp, gp, bp, ap;	/* the color plane equations */
    __glPlaneEquation zp;		/* the z plane eq. */
    __glTexturePlanes texture[__GL_NUM_TEXTURE_UNITS];
    __glPlaneEquation ezp;		/* this is for the eye z */
    __glTriangleMachine tm;
    __glCoverageStuff cs;
    __GLcontext *gc;
} __glFillAASubTriangleInfo;

/*
** Compute the constants A, B and C for a line equation in the general
** form:  Ax + By + C = 0.  A given point at (x,y) can be plugged into
** the left side of the equation and yield a number which indiates whether
** or not the point is on the line.  If the result is zero, then the point
** is on the line.  The sign of the result determines which side of
** the line the point is on.  To handle tie cases properly we need a way
** to assign a point on the edge to only one triangle.  To do this, we
** look at the sign of the equation evaluated at "c".  For edges whose
** sign at "c" is positive, we allow points on the edge to be in the
** triangle.
*/
static void FindLineEquation(__glLineEquation *eq, const __GLcoord *a,
			    const __GLcoord *b, const __GLcoord *c)
{
    __GLfloat dy, dx, valueAtC;

    /*
    ** Sort a,b so that the ordering of the verticies is consistent,
    ** regardless of the order given to this procedure.
    */
    if (b->y < a->y) {
	const __GLcoord *temp = b;
	b = a;
	a = temp;
    } else if ((b->y == a->y) && (b->x < a->x)) {
	const __GLcoord *temp = b;
	b = a;
	a = temp;
    }

    dy = b->y - a->y;
    dx = b->x - a->x;
    eq->a = -dy;
    eq->b = dx;
    eq->c = dy * a->x - dx * a->y;

    valueAtC = eq->a * c->x + eq->b * c->y + eq->c;
    if (valueAtC > 0) {
	eq->edgeSign = GL_TRUE;
    } else {
	eq->edgeSign = GL_FALSE;
    }
}

/*
** Given three points in (x,y,p) find the plane equation coeffecients
** for the plane that contains the three points.  First find the cross
** product of two of the vectors defined by the three points, then
** use one of the points to find "d".
*/
static void FindPlaneEquation(__GLcontext *gc, __glPlaneEquation *eq,
			      const __GLvertex *a, const __GLvertex *b,
			      const __GLvertex *c, __GLfloat p1,
			      __GLfloat p2, __GLfloat p3)
{
    __GLfloat v1x, v1y, v1p;
    __GLfloat v2x, v2y, v2p;
    __GLfloat nx, ny, np, k;

    /* find vector v1 */
    v1x = b->window.x - a->window.x;
    v1y = b->window.y - a->window.y;
    v1p = p2 - p1;

    /* find vector v2 */
    v2x = c->window.x - a->window.x;
    v2y = c->window.y - a->window.y;
    v2p = p3 - p1;

    /* find the cross product (== normal) for the plane */
    nx = v1y*v2p - v1p*v2y;
    ny = v1p*v2x - v1x*v2p;
    np = v1x*v2y - v1y*v2x;

    /*
    ** V dot N = k.  Find k.  We can use any of the three points on
    ** the plane, so we use a.
    */
    k = (a->window.x - __glHalf)*nx + (a->window.y - __glHalf)*ny + p1*np;

    /*
    ** Finally, setup the plane equation coeffecients.  Force c to be one
    ** by dividing everything through by c.
    */
    if (np != __glZero) {
	eq->a = nx / np;
	eq->b = ny / np;
	eq->d = -k / np;
    } else {
	eq->a = __glZero;
	eq->b = __glZero;
	eq->d = __glZero;
    }
    eq->c = __glOne;
}

/*
** Solve for p in the plane equation.
*/
static __GLfloat FindP(__glPlaneEquation *eq, __GLfloat x, __GLfloat y)
{
    return -(eq->a * x + eq->b * y + eq->d);
}

/*
** See if a given point is on the same side of the edge as the other
** vertex in the triangle not part of this edge.  When the line
** equation evaluates to zero, make points which are on lines with
** a negative edge sign (edgeSign GL_FALSE) part of the triangle.
*/
#define In(eq,x,y) \
    (((eq)->a * (x) + (eq)->b * (y) + (eq)->c > 0) == (eq)->edgeSign)

/*
** Determine if the point x,y is in or out of the triangle.  Evaluate
** each line equation for the point and compare the sign of the result
** with the edgeSign flag.
*/
#define Inside(tm,x,y) \
    (In(&(tm)->ab, x, y) && In(&(tm)->bc, x, y) && In(&(tm)->ca, x, y))

#define	FILTER_WIDTH	((__GLfloat) 1.0)
#define	FILTER_HEIGHT	((__GLfloat) 1.0)

/*
** Precompute stuff that is constant for all coverage tests.
*/
static void ComputeCoverageStuff(__glCoverageStuff *cs, GLint samples)
{
    __GLfloat dx, dy, fs = samples;
    __GLfloat half = ((__GLfloat) 0.5);

    cs->dx = dx = FILTER_WIDTH / fs;
    cs->dy = dy = FILTER_HEIGHT / fs;
    cs->leftDelta = -(FILTER_WIDTH / 2) + dx * half;
    cs->rightDelta = (FILTER_WIDTH / 2) - dx * half;
    cs->bottomDelta = -(FILTER_HEIGHT / 2) + dy * half;
    cs->topDelta = (FILTER_HEIGHT / 2) - dy * half;
    cs->samplesSquared = samples * samples;
    cs->samplesSquaredInv = ((__GLfloat) 1.0) / cs->samplesSquared;
    cs->samples = samples;
}

/*
** Return an estimate of the pixel coverage using sub-sampling.
*/
static __GLfloat Coverage(__glTriangleMachine *tm, __GLfloat *xs,
			  __GLfloat *ys, __glCoverageStuff *cs)
{
    GLint xx, yy, hits, samples;
    __GLfloat dx, dy, yBottom, px, py;
    __GLfloat minX, minY, maxX, maxY;

    hits = 0;
    samples = cs->samples;
    dx = cs->dx;
    dy = cs->dy;
    px = *xs + cs->leftDelta;
    yBottom = *ys + cs->bottomDelta;

    /*
    ** If the last coverage was one (the pixel to the left in x from us),
    ** then if the upper right and lower right sample positions are
    ** also in then this entire pixel must be in.
    */
    if (cs->lastCoverageWasOne) {
	__GLfloat urx, ury;
	urx = *xs + cs->rightDelta;
	ury = *ys + cs->topDelta;
	if (Inside(tm, urx, ury) && Inside(tm, urx, yBottom)) {
	    return ((__GLfloat) 1.0);
	}
    }

    /*
    ** Setup minimum and maximum x,y coordinates.  The min and max values
    ** are used to find a "good" point that is actually within the
    ** triangle so that parameter values can be computed correctly.
    */
    minX = 999999;
    maxX = __glMinusOne;
    minY = 999999;
    maxY = __glMinusOne;
    for (xx = 0; xx < samples; xx++) {
	py = yBottom;
	for (yy = 0; yy < samples; yy++) {
	    if (Inside(tm, px, py)) {
		if (px < minX) minX = px;
		if (px > maxX) maxX = px;
		if (py < minY) minY = py;
		if (py > maxY) maxY = py;
		hits++;
	    }
	    py += dy;
	}
	px += dx;
    }
    if (hits) {
	/*
	** Return the average of the two coordinates which is guaranteed
	** to be in the triangle.
	*/
	*xs = (minX + maxX) * ((__GLfloat) 0.5);
	*ys = (minY + maxY) * ((__GLfloat) 0.5);
	if (hits == cs->samplesSquared) {
	    /* Keep track when the last coverage was one */
	    cs->lastCoverageWasOne = GL_TRUE;
	    return ((__GLfloat) 1.0);
	}
    }
    cs->lastCoverageWasOne = GL_FALSE;
    return hits * cs->samplesSquaredInv;
}

/* 
 * macro to process an AA pixel.  
 * a subroutine is avoided for speed.
 */
#define PROCESS_PIXEL \
	    /* \
	     * we are done with coordinate check.\
	     * Let's get serious and start rendering.\
	     */\
	    if( modeFlags & __GL_SHADE_STIPPLE ) {\
		/* check against stipple pattern */\
		GLint row, col;\
		if( yInverted ) {\
		    row = (windowHeight - (iyBottom - viewYAdjust) - 1)\
			& (__GL_STIPPLE_BITS-1);\
	        } else {\
		    row = iyBottom & (__GL_STIPPLE_BITS-1);\
		}\
		col = x & (__GL_STIPPLE_BITS-1);\
		if( (gc->polygon.stipple[row] & __GL_STIPPLE_SHIFT(col)) == 0 ) {\
		    /* we hit a void.  try next pixel */\
		    continue;\
		}\
	    }\
\
	    /* calculate the fragment now */\
\
	    /*\
	     ** Fill in fragment for rendering.  First compute the color\
	     ** of the fragment.\
	     */\
	    if (modeFlags & __GL_SHADE_SMOOTH) {\
		frag.color[__GL_PRIMARY_COLOR].r = FindP(&info->rp, xs, ys);\
		if (rgbMode) {\
		    frag.color[__GL_PRIMARY_COLOR].g = FindP(&info->gp, xs, ys);\
		    frag.color[__GL_PRIMARY_COLOR].b = FindP(&info->bp, xs, ys);\
		    frag.color[__GL_PRIMARY_COLOR].a = FindP(&info->ap, xs, ys);\
		}\
	    } else {\
		frag.color[__GL_PRIMARY_COLOR].r = flatColor->r;\
		if (rgbMode) {\
		    frag.color[__GL_PRIMARY_COLOR].g = flatColor->g;\
		    frag.color[__GL_PRIMARY_COLOR].b = flatColor->b;\
		    frag.color[__GL_PRIMARY_COLOR].a = flatColor->a;\
		}\
	    }\
\
	    /*\
	     ** Texture the fragment.\
	     */\
	    if (modeFlags & __GL_SHADE_TEXTURE) {\
		__GLcoord strq[__GL_NUM_TEXTURE_UNITS];\
		__GLfloat rhow[__GL_NUM_TEXTURE_UNITS];\
		GLint i;\
		for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {\
		    strq[i].x = FindP(&info->texture[i].sp, xs, ys);\
		    strq[i].y = FindP(&info->texture[i].tp, xs, ys);\
		    strq[i].z = FindP(&info->texture[i].rp, xs, ys);\
		    strq[i].w = FindP(&info->texture[i].qwp, xs, ys);\
		    rhow[i] = FindP(&info->texture[i].rhowp, xs, ys);\
		}\
		(*gc->procs.textureRaster)(gc,\
				&frag.color[__GL_PRIMARY_COLOR], strq, rhow);\
	    }\
\
	    /*\
	     ** Fog the resulting color.\
	     */\
	    if (modeFlags & __GL_SHADE_SLOW_FOG) {\
		__GLfloat eyeZ = FindP(&info->ezp, xs, ys);\
		__glFogFragmentSlow(gc, &frag, eyeZ);\
	    }\
\
	    /*\
	     ** Apply anti-aliasing effect\
	     */\
	    if (rgbMode) {\
		frag.color[__GL_PRIMARY_COLOR].a *= coverage;\
	    } else {\
		frag.color[__GL_PRIMARY_COLOR].r =\
		    __glBuildAntiAliasIndex(frag.color[__GL_PRIMARY_COLOR].r,\
					    coverage);\
	    }\
\
	    /*\
	     ** Finally, render the fragment.\
	     */\
	    frag.x = (GLint)xs;\
	    frag.y = (GLint)ys;\
	    if (modeFlags & __GL_SHADE_DEPTH_ITER) {\
		frag.z = (__GLzValue)FindP(&info->zp, xs, ys);\
	    }\
	    (*gc->procs.store)(gc->drawBuffer, &frag);\


/* This routine sets gc->polygon.shader.cfb to gc->drawBuffer */

static void FillAASubTriangle(GLint iyBottom, GLint iyTop,
			      __glFillAASubTriangleInfo *info )
{
    GLint ixLeft, ixRight;
    GLint ixLeftFrac, ixRightFrac;
    GLint dxLeftFrac, dxRightFrac;
    GLint dxLeftLittle, dxRightLittle;
    GLint dxLeftBig, dxRightBig;
    GLint spanWidth, clipY0, clipY1;
    GLuint modeFlags;
#if !defined(__GL_ALLOCATE_SCANLINE_ARRAYS)
    __GLcolor colors[__GL_MAX_MAX_VIEWPORT];/*XXX oink */
    __GLcolor fbcolors[__GL_MAX_MAX_VIEWPORT];/*XXX oink */
    __GLstippleWord words[__GL_MAX_STIPPLE_WORDS];
#endif /* __GL_ALLOCATE_SCANLINE_ARRAYS */

    /* some caches */
    GLboolean rgbMode;
    __GLcolor *flatColor;
    __GLcontext *gc = info->gc;
    __glTriangleMachine *tm = &(info->tm);
    __glCoverageStuff *cs = &(info->cs);
    GLboolean yInverted = gc->constants.yInverted;
    GLint windowHeight = gc->constants.height;
    GLint viewYAdjust = gc->constants.viewportYAdjust;

    /* used for antialiasing */
    __GLfloat zero = __glZero;
    __GLfloat half = __glHalf;
    __GLfloat one = __glOne;
    GLint leftDiff;

    __GLshade *sh = &gc->polygon.shader;

    ixLeft = gc->polygon.shader.ixLeft;
    ixLeftFrac = gc->polygon.shader.ixLeftFrac;
    ixRight = gc->polygon.shader.ixRight;
    ixRightFrac = gc->polygon.shader.ixRightFrac;
    clipY0 = gc->transform.clipY0;
    clipY1 = gc->transform.clipY1;
    dxLeftFrac = gc->polygon.shader.dxLeftFrac;
    dxLeftBig = gc->polygon.shader.dxLeftBig;
    dxLeftLittle = gc->polygon.shader.dxLeftLittle;
    dxRightFrac = gc->polygon.shader.dxRightFrac;
    dxRightBig = gc->polygon.shader.dxRightBig;
    dxRightLittle = gc->polygon.shader.dxRightLittle;
    modeFlags = gc->polygon.shader.modeFlags;

    /* for AA */
    rgbMode = modeFlags & __GL_SHADE_RGB;
    flatColor = gc->vertex.provoking->color;

#if !defined(__GL_ALLOCATE_SCANLINE_ARRAYS)
    gc->polygon.shader.colors = colors;/*XXX*/
    gc->polygon.shader.fbcolors = fbcolors;/*XXX*/
    gc->polygon.shader.stipplePat = words;
#endif /* __GL_ALLOCATE_SCANLINE_ARRAYS */

    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
	__GLdrawableBuffer *dBuf = gc->stencilBuffer.buf.drawableBuf;
	gc->polygon.shader.sbuf =
	    __GL_STENCIL_ADDR(gc, &gc->stencilBuffer, dBuf, 
			      (__GLstencilCell*), ixLeft, iyBottom);
    }
    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
	__GLdrawableBuffer *dBuf = gc->depthBuffer.buf.drawableBuf;
	gc->polygon.shader.zbuf =
	    __GL_DEPTH_ADDR(gc, &gc->depthBuffer, dBuf, 
			    (__GLzValue*), ixLeft, iyBottom);
    }
    gc->polygon.shader.cfb = gc->drawBuffer;

    while (iyBottom < iyTop) {
	/*
	 * before we play around filling spans, let's figure out
	 * which pixels are partially covered due to anti-aliasing.
	 *
	 * The following algorithm makes the assumption that since
	 * ixLeft and ixRight are on the edge, their coverage is not 0.
	 * There is a guard statement however to avoid filling infinite spans.
	 * Starting from ixLeft we go left processing pixels till we
	 * hit coverage==0.  Then we jump back to ixLeft going to the right
	 * processing pixels till we hit coverage==1 or coverage==0.  If
	 * we hit coverage==0, then there are no internal pixels, so we are
	 * done with this scanline.  If we hit coverage==1, mark this pixel
	 * as the beginning of the scanline.  Perform the symmetric operation
	 * for ixRight.  Finally, if there is a span of pixels with coverage==1
	 * render them using span.processSpan, ottherwise go ahead as usual
	 * to calculate the parameters for the next scanline.
	 */
	GLint x;
	__GLfloat xs, ys;
	__GLfloat coverage;
	GLint xSpanLeft = 0, xSpanRight = 0;
	__GLfragment frag;
	GLboolean doRightEdge = GL_TRUE;

	/* fix left edge first */
	for( x = ixLeft;; x-- ) {
	    xs = x + half;
	    ys = iyBottom + half;
	    cs->lastCoverageWasOne = GL_FALSE;
	    coverage = Coverage( tm, &xs, &ys, cs );
	    if( coverage == zero ) break;

	    PROCESS_PIXEL;
	}
	for( x = ixLeft+1;; x++ ) {
	    xs = x + half;
	    ys = iyBottom + half;
	    cs->lastCoverageWasOne = GL_FALSE;
	    coverage = Coverage( tm, &xs, &ys, cs );
	    if( coverage == zero ) {
		doRightEdge = GL_FALSE;	/* don't do right edge */
		xSpanLeft = -1;
		break;
	    }
	    if( coverage == one ) {
		xSpanLeft = x;
		break;
	    }

	    PROCESS_PIXEL;
	}

	/* exit in case of an emergency */
	if( xSpanLeft > ixRight ) { /* something's fishy */
	    xSpanLeft = -1;
	    doRightEdge = GL_FALSE;
	}

	/* now fix right edge */
	if( doRightEdge == GL_TRUE ) {
	    for( x = ixRight;; x++ ) {
		xs = x + half;
		ys = iyBottom + half;
		cs->lastCoverageWasOne = GL_FALSE;
		coverage = Coverage( tm, &xs, &ys, cs );
		if( coverage == zero ) break;

		PROCESS_PIXEL;
	    }
	    for( x = ixRight-1;; x-- ) {
		xs = x + half;
		ys = iyBottom + half;
		cs->lastCoverageWasOne = GL_FALSE;
		coverage = Coverage( tm, &xs, &ys, cs );
		if( coverage == zero ) {
		    xSpanRight = -1;
		    break;
		}
		if( coverage == one ) {
		    xSpanRight = x;
		    break;
		}

		PROCESS_PIXEL;
	    }
	}

	/* after all these pixels are done, fix the iteration values */
	if( xSpanLeft != -1 ) {
	    leftDiff = xSpanLeft - ixLeft;

	    if( rgbMode ) {
		if( modeFlags & __GL_SHADE_SMOOTH ) {
		    sh->frag.color[__GL_PRIMARY_COLOR].r += leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].g += leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dgdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].b += leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dbdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].a += leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dadx;
		}
		if( modeFlags & __GL_SHADE_TEXTURE ) {
		    GLint i;

		    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
			sh->frag.texture[i].s +=
				leftDiff * sh->texture[i].dsdx;
			sh->frag.texture[i].t +=
				leftDiff * sh->texture[i].dtdx;
			sh->frag.texture[i].r +=
				leftDiff * sh->texture[i].drdx;
			sh->frag.texture[i].qw +=
				leftDiff * sh->texture[i].dqwdx;
			sh->frag.texture[i].rhow +=
				leftDiff * sh->texture[i].drhowdx;
		    }
		}
	    } else {
		if( modeFlags & __GL_SHADE_SMOOTH ) {
		    sh->frag.color[__GL_PRIMARY_COLOR].r += leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].drdy;
		}
	    }
	    if( modeFlags & __GL_SHADE_DEPTH_ITER ) {
		sh->frag.z += leftDiff * sh->dzdx;
	    }
	    if( modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
	        sh->offsetZ += leftDiff * sh->dzdx;
		if(sh->dzdxf != 0.)
		  gc->polygon.shader.outBoundCount += 
		    -(leftDiff * sh->dzdx)/sh->dzdxf;
	    }
	    if( modeFlags & __GL_SHADE_DEPTH_TEST ) {
		sh->zbuf += leftDiff;
	    }
	    if( modeFlags & __GL_SHADE_STENCIL_TEST ) {
		sh->sbuf += leftDiff;
	    }
	    if( modeFlags & __GL_SHADE_SLOW_FOG ) {
		sh->frag.f += leftDiff * sh->dfdx;
	    }
	} else {
	    leftDiff = 0;
	}
	
	/* render span if it exists */
	if( (xSpanLeft != -1) && (xSpanRight != -1) ) {
	    spanWidth = xSpanRight - xSpanLeft + 1;
	    /*
	     ** Only render spans that have non-zero width and which are
	     ** not scissored out vertically.
	     */
	    if ((spanWidth > 0) && (iyBottom >= clipY0) && 
		(iyBottom < clipY1)) {
		gc->polygon.shader.frag.x = xSpanLeft;
		gc->polygon.shader.frag.y = iyBottom;
		gc->polygon.shader.length = spanWidth;
		(*gc->procs.span.processSpan)(gc);
	    }
	}


	/* Advance right edge fixed point, adjusting for carry */
	ixRightFrac += dxRightFrac;
	if (ixRightFrac < 0) {
	    /* Carry/Borrow'd. Use large step */
	    ixRight += dxRightBig;
	    ixRightFrac &= ~0x80000000;
	} else {
	    ixRight += dxRightLittle;
	}

	iyBottom++;
	ixLeftFrac += dxLeftFrac;
	if (ixLeftFrac < 0) {
	    /* Carry/Borrow'd.  Use large step */
	    ixLeft += dxLeftBig;
	    ixLeftFrac &= ~0x80000000;

	    if (modeFlags & __GL_SHADE_RGB) {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    sh->frag.color[__GL_PRIMARY_COLOR].r += sh->colorIter[__GL_PRIMARY_COLOR].rBig - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].g += sh->colorIter[__GL_PRIMARY_COLOR].gBig - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dgdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].b += sh->colorIter[__GL_PRIMARY_COLOR].bBig - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dbdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].a += sh->colorIter[__GL_PRIMARY_COLOR].aBig - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dadx;
		}
		if (modeFlags & __GL_SHADE_TEXTURE) {
		    GLint i;

		    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
			sh->frag.texture[i].s +=
				sh->texture[i].sBig - leftDiff * sh->texture[i].dsdx;
			sh->frag.texture[i].t +=
				sh->texture[i].tBig - leftDiff * sh->texture[i].dtdx;
			sh->frag.texture[i].r +=
				sh->texture[i].rBig - leftDiff * sh->texture[i].drdx;
			sh->frag.texture[i].qw +=
				sh->texture[i].qwBig - leftDiff * sh->texture[i].dqwdx;
			sh->frag.texture[i].rhow +=
				sh->texture[i].rhowBig - leftDiff * sh->texture[i].drhowdx;
		    }
		}
	    } else {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    sh->frag.color[__GL_PRIMARY_COLOR].r += sh->colorIter[__GL_PRIMARY_COLOR].rBig - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		}
	    }
	    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
		/* The implicit multiply is taken out of the loop */
		sh->sbuf = (__GLstencilCell*)
		    ((GLubyte*) sh->sbuf + sh->sbufBig);
	    }
	    if (modeFlags & __GL_SHADE_DEPTH_ITER) {
		sh->frag.z += sh->zBig - leftDiff * sh->dzdx;
	    }
	    if( modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
	        sh->offsetZ += sh->zBig - leftDiff * sh->dzdx;
		if(sh->dzdxf == 0.) /* approximate 0 dzdxf */
		  gc->polygon.shader.outBoundCount += 
		    -sh->zBig/__GL_PGON_OFFSET_NEAR_ZERO;
		    else
		  gc->polygon.shader.outBoundCount += 
		    -(sh->zBig - leftDiff * sh->dzdx)/sh->dzdxf;
	    }
	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		sh->zbuf -= leftDiff;
		/* The implicit multiply is taken out of the loop */
		sh->zbuf = (__GLzValue*)
		    ((GLubyte*) sh->zbuf + sh->zbufBig);
	    }
	    if (modeFlags & __GL_SHADE_SLOW_FOG) {
		sh->frag.f += sh->fBig - leftDiff * sh->dfdx;
	    }
	} else {
	    /* Use small step */
	    ixLeft += dxLeftLittle;
	    if (modeFlags & __GL_SHADE_RGB) {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    sh->frag.color[__GL_PRIMARY_COLOR].r += sh->colorIter[__GL_PRIMARY_COLOR].rLittle - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].g += sh->colorIter[__GL_PRIMARY_COLOR].gLittle - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dgdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].b += sh->colorIter[__GL_PRIMARY_COLOR].bLittle - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dbdx;
		    sh->frag.color[__GL_PRIMARY_COLOR].a += sh->colorIter[__GL_PRIMARY_COLOR].aLittle - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].dadx;
		}
		if (modeFlags & __GL_SHADE_TEXTURE) {
		    GLint i;

		    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
			sh->frag.texture[i].s +=
				sh->texture[i].sLittle - leftDiff * sh->texture[i].dsdx;
			sh->frag.texture[i].t +=
				sh->texture[i].tLittle - leftDiff * sh->texture[i].dtdx;
			sh->frag.texture[i].r +=
				sh->texture[i].rLittle - leftDiff * sh->texture[i].drdx;
			sh->frag.texture[i].qw +=
				sh->texture[i].qwLittle - leftDiff * sh->texture[i].dqwdx;
			sh->frag.texture[i].rhow +=
				sh->texture[i].rhowLittle - leftDiff * sh->texture[i].drhowdx;
		    }
		}
	    } else {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    sh->frag.color[__GL_PRIMARY_COLOR].r += sh->colorIter[__GL_PRIMARY_COLOR].rLittle - leftDiff * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		}
	    }
	    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
		/* The implicit multiply is taken out of the loop */
		gc->polygon.shader.sbuf = (__GLstencilCell*)
		    ((GLubyte*) gc->polygon.shader.sbuf
		     + gc->polygon.shader.sbufLittle);
	    }
	    if (modeFlags & __GL_SHADE_DEPTH_ITER) {
		sh->frag.z += sh->zLittle - leftDiff * sh->dzdx;
	    }
	    if( modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
	        sh->offsetZ += sh->zLittle - leftDiff * sh->dzdx;
		if(sh->dzdxf == 0.) /* approximate dzdx = 0 */
		  sh->outBoundCount += -sh->zLittle/__GL_PGON_OFFSET_NEAR_ZERO;
		else
		  sh->outBoundCount += 
		    -(sh->zLittle - leftDiff * sh->dzdx)/sh->dzdxf;
	    }
	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		sh->zbuf -= leftDiff;
		/* The implicit multiply is taken out of the loop */
		sh->zbuf = (__GLzValue*)
		    ((GLubyte*) sh->zbuf + sh->zbufLittle);
	    }
	    if (modeFlags & __GL_SHADE_SLOW_FOG) {
		sh->frag.f += sh->fLittle - leftDiff * sh->dfdx;
	    }
	}
    }
    gc->polygon.shader.ixLeft = ixLeft;
    gc->polygon.shader.ixLeftFrac = ixLeftFrac;
    gc->polygon.shader.ixRight = ixRight;
    gc->polygon.shader.ixRightFrac = ixRightFrac;
}

#define __TWO_31 2147483648.0

#define __FRACTION(result,f) \
    result = (GLint) ((f) * __TWO_31)


static void SetInitialParameters(__GLcontext *gc, const __GLvertex *a,
				 const __GLcolor *ac,
				 __GLfloat aFog, __GLfloat aRhow,
				 __GLfloat dx, __GLfloat dy)
{
    __GLshade *sh = &gc->polygon.shader;
    __GLfloat little = sh->dxLeftLittle;
    __GLfloat big = sh->dxLeftBig;
    GLuint modeFlags = sh->modeFlags;

    if (big > little) {
	if (modeFlags & __GL_SHADE_RGB) {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		sh->frag.color[__GL_PRIMARY_COLOR].r = ac->r + dx*sh->colorIter[__GL_PRIMARY_COLOR].drdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].drdy;
		sh->colorIter[__GL_PRIMARY_COLOR].rLittle = sh->colorIter[__GL_PRIMARY_COLOR].drdy + little * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		sh->colorIter[__GL_PRIMARY_COLOR].rBig = sh->colorIter[__GL_PRIMARY_COLOR].rLittle + sh->colorIter[__GL_PRIMARY_COLOR].drdx;

		sh->frag.color[__GL_PRIMARY_COLOR].g = ac->g + dx*sh->colorIter[__GL_PRIMARY_COLOR].dgdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].dgdy;
		sh->colorIter[__GL_PRIMARY_COLOR].gLittle = sh->colorIter[__GL_PRIMARY_COLOR].dgdy + little * sh->colorIter[__GL_PRIMARY_COLOR].dgdx;
		sh->colorIter[__GL_PRIMARY_COLOR].gBig = sh->colorIter[__GL_PRIMARY_COLOR].gLittle + sh->colorIter[__GL_PRIMARY_COLOR].dgdx;

		sh->frag.color[__GL_PRIMARY_COLOR].b = ac->b + dx*sh->colorIter[__GL_PRIMARY_COLOR].dbdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].dbdy;
		sh->colorIter[__GL_PRIMARY_COLOR].bLittle = sh->colorIter[__GL_PRIMARY_COLOR].dbdy + little * sh->colorIter[__GL_PRIMARY_COLOR].dbdx;
		sh->colorIter[__GL_PRIMARY_COLOR].bBig = sh->colorIter[__GL_PRIMARY_COLOR].bLittle + sh->colorIter[__GL_PRIMARY_COLOR].dbdx;

		sh->frag.color[__GL_PRIMARY_COLOR].a = ac->a + dx*sh->colorIter[__GL_PRIMARY_COLOR].dadx + dy*sh->colorIter[__GL_PRIMARY_COLOR].dady;
		sh->colorIter[__GL_PRIMARY_COLOR].aLittle = sh->colorIter[__GL_PRIMARY_COLOR].dady + little * sh->colorIter[__GL_PRIMARY_COLOR].dadx;
		sh->colorIter[__GL_PRIMARY_COLOR].aBig =sh->colorIter[__GL_PRIMARY_COLOR].aLittle + sh->colorIter[__GL_PRIMARY_COLOR].dadx;
	    }
	} else {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		sh->frag.color[__GL_PRIMARY_COLOR].r = ac->r + dx*sh->colorIter[__GL_PRIMARY_COLOR].drdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].drdy;
		sh->colorIter[__GL_PRIMARY_COLOR].rLittle = sh->colorIter[__GL_PRIMARY_COLOR].drdy + little * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		sh->colorIter[__GL_PRIMARY_COLOR].rBig = sh->colorIter[__GL_PRIMARY_COLOR].rLittle + sh->colorIter[__GL_PRIMARY_COLOR].drdx;
	    }
	}
	if (modeFlags & __GL_SHADE_TEXTURE) {
	    GLint i;

	    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
		__GLfloat s = a->texture[i].x;
		__GLfloat t = a->texture[i].y;
		__GLfloat r = a->texture[i].z;
		__GLfloat q = a->texture[i].w;

		if ((modeFlags & __GL_SHADE_PERSP_TEXTURE) &&
		    !(modeFlags & __GL_SHADE_PROJSCALED_TEXTURE))
		{
		    __GLfloat w = a->window.w;

		    s *= w;
		    t *= w;
		    r *= w;
		    q *= w;
		}

		sh->frag.texture[i].s =
			s + dx*sh->texture[i].dsdx + dy*sh->texture[i].dsdy;
		sh->texture[i].sLittle =
			sh->texture[i].dsdy + little * sh->texture[i].dsdx;
		sh->texture[i].sBig =
			sh->texture[i].sLittle + sh->texture[i].dsdx;

		sh->frag.texture[i].t =
			t + dx*sh->texture[i].dtdx + dy*sh->texture[i].dtdy;
		sh->texture[i].tLittle =
			sh->texture[i].dtdy + little * sh->texture[i].dtdx;
		sh->texture[i].tBig =
			sh->texture[i].tLittle + sh->texture[i].dtdx;

		sh->frag.texture[i].r =
			r + dx*sh->texture[i].drdx + dy*sh->texture[i].drdy;
		sh->texture[i].rLittle =
			sh->texture[i].drdy + little * sh->texture[i].drdx;
		sh->texture[i].rBig =
			sh->texture[i].rLittle + sh->texture[i].drdx;

		sh->frag.texture[i].qw =
			q + dx*sh->texture[i].dqwdx + dy*sh->texture[i].dqwdy;
		sh->texture[i].qwLittle =
			sh->texture[i].dqwdy + little * sh->texture[i].dqwdx;
		sh->texture[i].qwBig =
			sh->texture[i].qwLittle + sh->texture[i].dqwdx;

		sh->frag.texture[i].rhow =
			aRhow + dx*sh->texture[i].drhowdx + dy*sh->texture[i].drhowdy;
		sh->texture[i].rhowLittle =
			sh->texture[i].drhowdy + little*sh->texture[i].drhowdx;
		sh->texture[i].rhowBig =
			sh->texture[i].rhowLittle + sh->texture[i].drhowdx;
	    }
	}
	if (modeFlags & __GL_SHADE_DEPTH_ITER) {
	    __GLfloat zLittle;
	    if(gc->state.enables.general & __GL_POLYGON_OFFSET_FILL_ENABLE) {
	        sh->frag.z = __glPolygonOffsetZ(gc, a, dx, dy);
	    } else {
	        sh->frag.z = (__GLzValue)
		             (a->window.z + dx*sh->dzdxf + dy*sh->dzdyf);
	    }
	    zLittle = sh->dzdyf + little * sh->dzdxf;
	    sh->zLittle = (GLint)zLittle;
	    sh->zBig = (GLint)(zLittle + sh->dzdxf);
	}
	if (modeFlags & __GL_SHADE_SLOW_FOG) {
	    sh->frag.f = aFog + dx*sh->dfdx + dy*sh->dfdy;
	    sh->fLittle = sh->dfdy + little * sh->dfdx;
	    sh->fBig = sh->fLittle + sh->dfdx;
	}
    } else {	
	if (modeFlags & __GL_SHADE_RGB) {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		sh->frag.color[__GL_PRIMARY_COLOR].r = ac->r + dx*sh->colorIter[__GL_PRIMARY_COLOR].drdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].drdy;
		sh->colorIter[__GL_PRIMARY_COLOR].rLittle = sh->colorIter[__GL_PRIMARY_COLOR].drdy + little * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		sh->colorIter[__GL_PRIMARY_COLOR].rBig = sh->colorIter[__GL_PRIMARY_COLOR].rLittle - sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		sh->frag.color[__GL_PRIMARY_COLOR].g = ac->g + dx*sh->colorIter[__GL_PRIMARY_COLOR].dgdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].dgdy;
		sh->colorIter[__GL_PRIMARY_COLOR].gLittle = sh->colorIter[__GL_PRIMARY_COLOR].dgdy + little * sh->colorIter[__GL_PRIMARY_COLOR].dgdx;
		sh->colorIter[__GL_PRIMARY_COLOR].gBig = sh->colorIter[__GL_PRIMARY_COLOR].gLittle - sh->colorIter[__GL_PRIMARY_COLOR].dgdx;

		sh->frag.color[__GL_PRIMARY_COLOR].b = ac->b + dx*sh->colorIter[__GL_PRIMARY_COLOR].dbdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].dbdy;
		sh->colorIter[__GL_PRIMARY_COLOR].bLittle = sh->colorIter[__GL_PRIMARY_COLOR].dbdy + little * sh->colorIter[__GL_PRIMARY_COLOR].dbdx;
		sh->colorIter[__GL_PRIMARY_COLOR].bBig = sh->colorIter[__GL_PRIMARY_COLOR].bLittle - sh->colorIter[__GL_PRIMARY_COLOR].dbdx;

		sh->frag.color[__GL_PRIMARY_COLOR].a = ac->a + dx*sh->colorIter[__GL_PRIMARY_COLOR].dadx + dy*sh->colorIter[__GL_PRIMARY_COLOR].dady;
		sh->colorIter[__GL_PRIMARY_COLOR].aLittle = sh->colorIter[__GL_PRIMARY_COLOR].dady + little * sh->colorIter[__GL_PRIMARY_COLOR].dadx;
		sh->colorIter[__GL_PRIMARY_COLOR].aBig =sh->colorIter[__GL_PRIMARY_COLOR].aLittle - sh->colorIter[__GL_PRIMARY_COLOR].dadx;
	    }
	} else {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		sh->frag.color[__GL_PRIMARY_COLOR].r = ac->r + dx*sh->colorIter[__GL_PRIMARY_COLOR].drdx + dy*sh->colorIter[__GL_PRIMARY_COLOR].drdy;
		sh->colorIter[__GL_PRIMARY_COLOR].rLittle = sh->colorIter[__GL_PRIMARY_COLOR].drdy + little * sh->colorIter[__GL_PRIMARY_COLOR].drdx;
		sh->colorIter[__GL_PRIMARY_COLOR].rBig = sh->colorIter[__GL_PRIMARY_COLOR].rLittle - sh->colorIter[__GL_PRIMARY_COLOR].drdx;
	    }
	}
	if (modeFlags & __GL_SHADE_TEXTURE) {
	    GLint i;

	    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
		__GLfloat s = a->texture[i].x;
		__GLfloat t = a->texture[i].y;
		__GLfloat r = a->texture[i].z;
		__GLfloat q = a->texture[i].w;

		if ((modeFlags & __GL_SHADE_PERSP_TEXTURE) &&
		    !(modeFlags & __GL_SHADE_PROJSCALED_TEXTURE))
		{
		    __GLfloat w = a->window.w;

		    s *= w;
		    t *= w;
		    r *= w;
		    q *= w;
		}

		sh->frag.texture[i].s =
			s + dx*sh->texture[i].dsdx + dy*sh->texture[i].dsdy;
		sh->texture[i].sLittle =
			sh->texture[i].dsdy + little * sh->texture[i].dsdx;
		sh->texture[i].sBig =
			sh->texture[i].sLittle - sh->texture[i].dsdx;

		sh->frag.texture[i].t =
			t + dx*sh->texture[i].dtdx + dy*sh->texture[i].dtdy;
		sh->texture[i].tLittle =
			sh->texture[i].dtdy + little * sh->texture[i].dtdx;
		sh->texture[i].tBig =
			sh->texture[i].tLittle - sh->texture[i].dtdx;

		sh->frag.texture[i].r =
			r + dx*sh->texture[i].drdx + dy*sh->texture[i].drdy;
		sh->texture[i].rLittle =
			sh->texture[i].drdy + little * sh->texture[i].drdx;
		sh->texture[i].rBig =
			sh->texture[i].rLittle - sh->texture[i].drdx;

		sh->frag.texture[i].qw =
			q + dx*sh->texture[i].dqwdx + dy*sh->texture[i].dqwdy;
		sh->texture[i].qwLittle =
			sh->texture[i].dqwdy + little * sh->texture[i].dqwdx;
		sh->texture[i].qwBig =
			sh->texture[i].qwLittle - sh->texture[i].dqwdx;

		sh->frag.texture[i].rhow =
			aRhow + dx*sh->texture[i].drhowdx + dy*sh->texture[i].drhowdy;
		sh->texture[i].rhowLittle =
			sh->texture[i].drhowdy + little*sh->texture[i].drhowdx;
		sh->texture[i].rhowBig =
			sh->texture[i].rhowLittle - sh->texture[i].drhowdx;
	    }
	}
	if (modeFlags & __GL_SHADE_DEPTH_ITER) {
	    __GLfloat zLittle;
	    if(gc->state.enables.general & __GL_POLYGON_OFFSET_FILL_ENABLE) {
	        sh->frag.z = __glPolygonOffsetZ(gc, a, dx, dy);
	    } else {
	        sh->frag.z = (__GLzValue)
		             (a->window.z + dx*sh->dzdxf + dy*sh->dzdyf);
	    }
	    zLittle = sh->dzdyf + little * sh->dzdxf;
	    sh->zLittle = (GLint)zLittle;
	    sh->zBig = (GLint)(zLittle - sh->dzdxf);
	}
	if (modeFlags & __GL_SHADE_SLOW_FOG) {
	    sh->frag.f = aFog + dx*sh->dfdx + dy*sh->dfdy;
	    sh->fLittle = sh->dfdy + little * sh->dfdx;
	    sh->fBig = sh->fLittle - sh->dfdx;
	}
    }
}

/*
** Force f to have no more precision that the subpixel precision allows.
** Even though "f" is biased this still works and does not generate an overflow.
*/
#define	__GL_FIX_PRECISION(f)	\
	((__GLfloat)((GLint)((f) * (1 << gc->constants.subpixelBits))) \
	/ (1 << gc->constants.subpixelBits))

void __glFillAntiAliasedTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
		      __GLvertex *c)

{
    __GLvertex *oa = a, *ob = b, *oc = c;
    __GLfloat ccw, area, oneOverArea, t1, t2, t3, t4;
    __GLfloat dxAC, dxBC, dyAC, dyBC;
    __GLfloat aFog = 0.0f, bFog = 0.0f;
    __GLfloat aRhow = 0.0f, bRhow = 0.0f;
    __GLfloat dxAB, dyAB;
    __GLfloat dx, dy, dxdyLeft, dxdyRight;
    __GLcolor *ac, *bc;
    GLint aIY, bIY, cIY;
    GLuint modeFlags;
    __GLcoord asnap, bsnap, csnap;

    /* structure in order to communicate with FillAASubTriangle */
    __glFillAASubTriangleInfo info;

    /* used for antialiasing */
    GLint samples;

    __GL_ADJUST_TRIANGLE_VERTS(gc, oa, ob, oc, gc->vertex.provoking);

    info.gc = gc;

    /*
    ** Sort vertices in y.
    */
    if (*(int *)&a->window.y < *(int *)&b->window.y) {
        if (*(int *)&b->window.y < *(int *)&c->window.y) {
            /* Already sorted */
        } else {
            if (*(int *)&a->window.y < *(int *)&c->window.y) {
                __GLvertex *temp=b; b=c; c=temp;
            } else {
                __GLvertex *temp=a; a=c; c=b; b=temp;
            }
        }
    } else {
        if (*(int *)&b->window.y < *(int *)&c->window.y) {
            if (*(int *)&a->window.y < *(int *)&c->window.y) {
                __GLvertex *temp=a; a=b; b=temp;
            } else {
                __GLvertex *temp=a; a=b; b=c; c=temp;
            }
        } else {
            __GLvertex *temp=a; a=c; c=temp;
        }
    }

    /*
    ** figure out the area.
    **
    ** Compute the area of the triangle after constraining the incoming
    ** coordinates to the subpixel precision.  The viewport bias gives
    ** more precision (typically) than the subpixel precision.  Because of
    ** this the algorithim below can fail to reject an essentially empty
    ** triangle and instead fill a large area.  The scan converter fill
    ** routines (eg polydraw.c) don't have this trouble because of the
    ** very nature of edge walking.
    **
    ** NOTE: Notice that here as in other places, when the area calculation
    ** is done we are careful to do it as a series of subtractions followed by
    ** multiplications.  This is done to guarantee that no overflow will
    ** occur (remember that the coordinates are biased by a potentially large
    ** number, and that multiplying two biased numbers will square the bias).
    */
    {
	__GLfloat ax, bx, cx, ay, by, cy;
	__GLfloat zero = __glZero;

	asnap.x = ax = __GL_FIX_PRECISION(a->window.x);
	bsnap.x = bx = __GL_FIX_PRECISION(b->window.x);
	csnap.x = cx = __GL_FIX_PRECISION(c->window.x);
	asnap.y = ay = __GL_FIX_PRECISION(a->window.y);
	bsnap.y = by = __GL_FIX_PRECISION(b->window.y);
	csnap.y = cy = __GL_FIX_PRECISION(c->window.y);
	asnap.z = a->window.z;
	bsnap.z = b->window.z;
	csnap.z = c->window.z;
	dxAC = ax - cx;
	dxBC = bx - cx;
	dyAC = ay - cy;
	dyBC = by - cy;
	area = dxAC * dyBC - dxBC * dyAC;
	if (area == zero) {
	    __GL_RESTORE_TRIANGLE_VERTS(gc, oa, ob, oc, gc->vertex.provoking);
	    return;
	}
    }

    ccw = !(*(int *)&area >> 31);

    /* Pre-compute one over polygon area */
    oneOverArea = __glOne / area;

    /* Fetch some more stuff we are going to reuse */
    modeFlags = gc->polygon.shader.modeFlags;
    ac = a->color;
    bc = b->color;

    /*
     * precompute parameters used for antialiasing
     */
    FindLineEquation( &info.tm.ab, &asnap, &bsnap, &csnap );
    FindLineEquation( &info.tm.bc, &bsnap, &csnap, &asnap );
    FindLineEquation( &info.tm.ca, &csnap, &asnap, &bsnap );
    if( gc->state.hints.polygonSmooth == GL_NICEST ) {
	samples = 8;
    } else {
	samples = 4;
    }
    ComputeCoverageStuff( &info.cs, samples );

    /*
    ** Compute delta values for unit changes in x or y for each
    ** parameter.
    */
    t1 = dyAC * oneOverArea;
    t2 = dyBC * oneOverArea;
    t3 = dxAC * oneOverArea;
    t4 = dxBC * oneOverArea;
    if (modeFlags & __GL_SHADE_RGB) {
	if (modeFlags & __GL_SHADE_SMOOTH) {
	    __GLfloat drAC, dgAC, dbAC, daAC;
	    __GLfloat drBC, dgBC, dbBC, daBC;
	    __GLcolor *cc;

	    cc = c->color;
	    drAC = ac->r - cc->r;
	    drBC = bc->r - cc->r;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdx = drAC * t2 - drBC * t1;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdy = drBC * t3 - drAC * t4;
	    dgAC = ac->g - cc->g;
	    dgBC = bc->g - cc->g;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dgdx = dgAC * t2 - dgBC * t1;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dgdy = dgBC * t3 - dgAC * t4;
	    dbAC = ac->b - cc->b;
	    dbBC = bc->b - cc->b;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dbdx = dbAC * t2 - dbBC * t1;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dbdy = dbBC * t3 - dbAC * t4;
	    daAC = ac->a - cc->a;
	    daBC = bc->a - cc->a;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dadx = daAC * t2 - daBC * t1;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].dady = daBC * t3 - daAC * t4;

	    /* for AA */
	    FindPlaneEquation( gc, &info.rp, a, b, c, ac->r, bc->r, cc->r );
	    FindPlaneEquation( gc, &info.gp, a, b, c, ac->g, bc->g, cc->g );
	    FindPlaneEquation( gc, &info.bp, a, b, c, ac->b, bc->b, cc->b );
	    FindPlaneEquation( gc, &info.ap, a, b, c, ac->a, bc->a, cc->a );
	} else {
	    __GLcolor *flatColor = gc->vertex.provoking->color;
	    gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r = flatColor->r;
	    gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].g = flatColor->g;
	    gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].b = flatColor->b;
	    gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].a = flatColor->a;
	}
    } else {
	if (modeFlags & __GL_SHADE_SMOOTH) {
	    __GLfloat drAC;
	    __GLfloat drBC;
	    __GLcolor *cc;

	    cc = c->color;
	    drAC = ac->r - cc->r;
	    drBC = bc->r - cc->r;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdx = drAC * t2 - drBC * t1;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdy = drBC * t3 - drAC * t4;

	    /* for AA */
	    FindPlaneEquation( gc, &info.rp, a, b, c, ac->r, bc->r, cc->r );
	} else {
	    __GLcolor *flatColor = gc->vertex.provoking->color;
	    gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r = flatColor->r;
	}
    }
    if (modeFlags & __GL_SHADE_TEXTURE) {
	GLint i;

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    __GLfloat ax, ay, az, aw, bx, by, bz, bw, cx, cy, cz, cw;
	    __GLfloat dsAC, dsBC, dtAC, dtBC, drAC, drBC, dqwAC, dqwBC;
	    __GLfloat drhowAC, drhowBC, cRhow;

	    if (gc->procs.textureUnit[i] == 0)
		continue;
	    		    
	    ax = a->texture[i].x;
	    ay = a->texture[i].y;
	    az = a->texture[i].z;
	    aw = a->texture[i].w;

	    bx = b->texture[i].x;
	    by = b->texture[i].y;
	    bz = b->texture[i].z;
	    bw = b->texture[i].w;

	    cx = c->texture[i].x;
	    cy = c->texture[i].y;
	    cz = c->texture[i].z;
	    cw = c->texture[i].w;

	    if ((modeFlags & __GL_SHADE_PERSP_TEXTURE) &&
		!(modeFlags & __GL_SHADE_PROJSCALED_TEXTURE))
	    {
		ax *= a->window.w;
		ay *= a->window.w;
		az *= a->window.w;
		aw *= a->window.w;

		bx *= b->window.w;
		by *= b->window.w;
		bz *= b->window.w;
		bw *= b->window.w;

		cx *= c->window.w;
		cy *= c->window.w;
		cz *= c->window.w;
		cw *= c->window.w;
	    }

	    dsAC = ax - cx;
	    dsBC = bx - cx;
	    gc->polygon.shader.texture[i].dsdx = dsAC * t2 - dsBC * t1;
	    gc->polygon.shader.texture[i].dsdy = dsBC * t3 - dsAC * t4;
	    dtAC = ay - cy;
	    dtBC = by - cy;
	    gc->polygon.shader.texture[i].dtdx = dtAC * t2 - dtBC * t1;
	    gc->polygon.shader.texture[i].dtdy = dtBC * t3 - dtAC * t4;
	    drAC = az - cz;
	    drBC = bz - cz;
	    gc->polygon.shader.texture[i].drdx = drAC * t2 - drBC * t1;
	    gc->polygon.shader.texture[i].drdy = drBC * t3 - drAC * t4;
	    dqwAC = aw - cw;
	    dqwBC = bw - cw;
	    gc->polygon.shader.texture[i].dqwdx = dqwAC * t2 - dqwBC * t1;
	    gc->polygon.shader.texture[i].dqwdy = dqwBC * t3 - dqwAC * t4;


	    /* calculate rho/w at the vertices */
	    aRhow = (*gc->procs.calcPolygonRho)(gc, i, &gc->polygon.shader,
						ax, ay, az, aw) * aw;
	    bRhow = (*gc->procs.calcPolygonRho)(gc, i, &gc->polygon.shader,
						bx, by, bz, bw) * bw;
	    cRhow = (*gc->procs.calcPolygonRho)(gc, i, &gc->polygon.shader,
						cx, cy, cz, cw) * cw;

	    drhowAC = aRhow - cRhow;
	    drhowBC = bRhow - cRhow;
	    gc->polygon.shader.texture[i].drhowdx = drhowAC * t2 - drhowBC * t1;
	    gc->polygon.shader.texture[i].drhowdy = drhowBC * t3 - drhowAC * t4;

	    /* for AA */
	    {
		FindPlaneEquation(gc, &info.texture[i].sp, a, b, c, 
				  a->texture[i].x,
				  b->texture[i].x,
				  c->texture[i].x);
		FindPlaneEquation(gc, &info.texture[i].tp, a, b, c, 
				  a->texture[i].y,
				  b->texture[i].y,
				  c->texture[i].y);
		FindPlaneEquation(gc, &info.texture[i].rp, a, b, c, 
				  a->texture[i].z,
				  b->texture[i].z,
				  c->texture[i].z);
		FindPlaneEquation(gc, &info.texture[i].qwp, a, b, c, 
				  a->texture[i].w,
				  b->texture[i].w,
				  c->texture[i].w);
		FindPlaneEquation(gc, &info.texture[i].rhowp, a, b, c,
				  aRhow, bRhow, cRhow);
	    }
	}
    }
    if (modeFlags & __GL_SHADE_DEPTH_ITER) {
	__GLfloat dzAC, dzBC;

	dzAC = a->window.z - c->window.z;
	dzBC = b->window.z - c->window.z;
	gc->polygon.shader.dzdxf = dzAC * t2 - dzBC * t1;
	gc->polygon.shader.dzdyf = dzBC * t3 - dzAC * t4;
	gc->polygon.shader.dzdx = (GLint) gc->polygon.shader.dzdxf;
	gc->polygon.shader.dzdxBig =
	    gc->polygon.shader.dzdx << __GL_STIPPLE_COUNT_BITS;

	/* for AA */
	FindPlaneEquation( gc, &info.zp, a, b, c, 
			  a->window.z, b->window.z, c->window.z );
    }
    if (modeFlags & __GL_SHADE_SLOW_FOG) {
	__GLfloat dfAC, dfBC, cFog;

	if (gc->state.hints.fog == GL_NICEST) {
	    /* Use eyeZ for interpolation value */
	    aFog = a->eye.z;
	    bFog = b->eye.z;
	    cFog = c->eye.z;
	} else {
	    /* Use fog(eyeZ) for interpolation value */
	    aFog = a->fog;
	    bFog = b->fog;
	    cFog = c->fog;
	}
	dfAC = aFog - cFog;
	dfBC = bFog - cFog;
	gc->polygon.shader.dfdx = dfAC * t2 - dfBC * t1;
	gc->polygon.shader.dfdy = dfBC * t3 - dfAC * t4;

	/* for AA */
	FindPlaneEquation( gc, &info.ezp, a, b, c, a->eye.z, b->eye.z, c->eye.z );
    }

    /* Snap each y coordinate to its pixel center */
    aIY = (GLint) (a->window.y);
    bIY = (GLint) (b->window.y);
    cIY = (GLint) (c->window.y);

    /*
    ** This algorithim always fills from bottom to top, left to right.
    ** Because of this, ccw triangles are inherently faster because
    ** the parameter values need not be recomputed.
    */
    dxAB = a->window.x - b->window.x;
    dyAB = a->window.y - b->window.y;

    if (ccw) {
	dxdyLeft = dyAC ? dxAC / dyAC : __glZero;
	dy = (aIY + __glOne) - a->window.y;
	__glSnapXLeft(gc, a->window.x + dy*dxdyLeft, dxdyLeft);
	dx = (gc->polygon.shader.ixLeft + __glOne) - a->window.x;
	SetInitialParameters(gc, a, ac, aFog, aRhow, dx, dy);
	if (aIY != bIY) {
		dxdyRight = dyAB ? dxAB / dyAB : __glZero;
	    __glSnapXRight(&gc->polygon.shader, a->window.x + dy*dxdyRight,
		       dxdyRight);
	    FillAASubTriangle(aIY, bIY, &info );
	}

	if (bIY != cIY) {
	    dxdyRight = dyBC ? dxBC / dyBC : __glZero;
	    dy = (bIY + __glOne) - b->window.y;
	    __glSnapXRight(&gc->polygon.shader, b->window.x + dy*dxdyRight,
		       dxdyRight);
	    FillAASubTriangle(bIY, cIY, &info );
	}
    } else {
	dxdyRight = dyAC ? dxAC / dyAC : __glZero;
	dy = (aIY + __glOne) - a->window.y;
	__glSnapXRight(&gc->polygon.shader, a->window.x + dy*dxdyRight, dxdyRight);
	if (aIY != bIY) {
	    dxdyLeft = dyAB ? dxAB / dyAB : __glZero;
	    __glSnapXLeft(gc, a->window.x + dy*dxdyLeft, dxdyLeft);
	    dx = (gc->polygon.shader.ixLeft + __glOne) - a->window.x;
	    SetInitialParameters(gc, a, ac, aFog, aRhow, dx, dy);
	    FillAASubTriangle(aIY, bIY, &info );
	}

	if (bIY != cIY) {
	    dxdyLeft = dyBC ? dxBC / dyBC : __glZero;
	    dy = (bIY + __glOne) - b->window.y;
	    __glSnapXLeft(gc, b->window.x + dy*dxdyLeft, dxdyLeft);
	    dx = (gc->polygon.shader.ixLeft + __glOne) - b->window.x;
	    SetInitialParameters(gc, b, bc, bFog, bRhow, dx, dy);
	    FillAASubTriangle(bIY, cIY, &info );
	}
    }

    __GL_RESTORE_TRIANGLE_VERTS(gc, oa, ob, oc, gc->vertex.provoking);
}
