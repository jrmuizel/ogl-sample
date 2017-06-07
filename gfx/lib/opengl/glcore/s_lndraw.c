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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_lndraw.c#12 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "g_imports.h"
#include "fmacros.h"

#define __TWO_31 2147483648.0

/*
** Most line functions will start off by computing the information 
** computed by this routine.
**
** The excessive number of labels in this routine is partly due
** to the fact that it is used as a model for writing an assembly 
** equivalent.
*/
void __glInitLineData(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1)
{
    __GLfloat x0,y0,x1,y1;
    __GLfloat minorStart;
    GLint intMinorStart;
    __GLfloat dx, dy;
    __GLfloat offset;
    __GLfloat slope;
    __GLlineState *ls = &gc->state.line;
    __GLfloat halfWidth;
    __GLfloat x0frac, x1frac, y0frac, y1frac, half, totDist;
    GLint ix0, ix1, iy0, iy1;

    gc->line.options.v0 = v0;
    gc->line.options.v1 = v1;
    gc->line.options.width = ls->aliasedWidth;

    half = __glHalf;
    x0=v0->window.x;
    y0=v0->window.y;
    x1=v1->window.x;
    y1=v1->window.y;

    dx=x1-x0;
    dy=y1-y0;

    halfWidth = ls->aliasedWidth * half;

    /* Ugh.  This is slow.  Bummer. */
    /*    x0frac = x0 - ((GLint) x0);
     *    x1frac = x1 - ((GLint) x1);
     *    y0frac = y0 - ((GLint) y0);
     *    y1frac = y1 - ((GLint) y1);
     */
    Float2Int(x0, ix0);
    Float2Int(y0, iy0);
    Float2Int(x1, ix1);
    Float2Int(y1, iy1);
    x0frac = x0 - ix0--;
    y0frac = y0 - iy0--;
    x1frac = x1 - ix1--;
    y1frac = y1 - iy1--;

    if (dx > __glZero) {
	if (dy > __glZero) {
	    if (dx > dy) {	/* dx > dy > 0 */
		gc->line.options.yBig = 1;
posxmajor:			/* dx > |dy| >= 0 */
		gc->line.options.yLittle = 0;
		gc->line.options.xBig = 1;
		gc->line.options.xLittle = 1;
		slope = dy/dx;

		if (y0frac < 0) y0frac = -y0frac;

		totDist = y0frac + x0frac;
		if (totDist > half) ix0++;

		if (y1frac < 0) y1frac = -y1frac;

		totDist = y1frac + x1frac;
		if (totDist > half) ix1++;

		offset = ix0 + 1.0 - x0;

		gc->line.options.length = dx;
		gc->line.options.numPixels = ix1 - ix0;

xmajorfinish:
		gc->line.options.axis = __GL_X_MAJOR;
		gc->line.options.xStart = ix0;
		gc->line.options.offset = offset;
		minorStart = y0 + offset*slope - halfWidth;
		intMinorStart = (GLint) minorStart;
		minorStart -= intMinorStart;
		gc->line.options.yStart = intMinorStart;
		gc->line.options.dfraction = (GLint)(slope * __TWO_31);
		gc->line.options.fraction = (GLint)(minorStart * __TWO_31);
	    } else {		/* dy >= dx > 0 */
		gc->line.options.xBig = 1;
posymajor:			/* dy >= |dx| >= 0, dy != 0 */
		gc->line.options.xLittle = 0;
		gc->line.options.yBig = 1;
		gc->line.options.yLittle = 1;
		slope = dx/dy;

		if (x0frac < 0) x0frac = -x0frac;

		totDist = y0frac + x0frac;
		if (totDist > half) iy0++;

		if (x1frac < 0) x1frac = -x1frac;

		totDist = y1frac + x1frac;
		if (totDist > half) iy1++;

		offset = iy0 + 1.0 - y0;

		gc->line.options.length = dy;
		gc->line.options.numPixels = iy1 - iy0;

ymajorfinish:
		gc->line.options.axis = __GL_Y_MAJOR;
		gc->line.options.yStart = iy0;
		gc->line.options.offset = offset;
		minorStart = x0 + offset*slope - halfWidth;
		intMinorStart = (GLint) minorStart;
		minorStart -= intMinorStart;
		gc->line.options.xStart = intMinorStart;
		gc->line.options.dfraction = (GLint)(slope * __TWO_31);
		gc->line.options.fraction = (GLint)(minorStart * __TWO_31);
	    }
	} else {
	    if (dx > -dy) {	/* dx > -dy >= 0 */
		gc->line.options.yBig = -1;
		goto posxmajor;
	    } else {		/* -dy >= dx >= 0, dy != 0 */
		gc->line.options.xBig = 1;
negymajor:			/* -dy >= |dx| >= 0, dy != 0 */
		gc->line.options.xLittle = 0;
		gc->line.options.yBig = -1;
		gc->line.options.yLittle = -1;
		slope = dx/-dy;

		if (x0frac < 0) x0frac = -x0frac;

		totDist = x0frac - y0frac;
		if (totDist > half) iy0--;

		if (x1frac < 0) x1frac = -x1frac;

		totDist = x1frac - y1frac;
		if (totDist > half) iy1--;

		offset = y0 - iy0 - 1.0;

		gc->line.options.length = -dy;
		gc->line.options.numPixels = iy0 - iy1;
		goto ymajorfinish;
	    }
	}
    } else {
	if (dy > __glZero) {
	    if (-dx > dy) {	/* -dx > dy > 0 */
		gc->line.options.yBig = 1;
negxmajor:			/* -dx > |dy| >= 0 */
		gc->line.options.yLittle = 0;
		gc->line.options.xBig = -1;
		gc->line.options.xLittle = -1;
		slope = dy/-dx;

		if (y0frac < 0) y0frac = -y0frac;

		totDist = y0frac - x0frac;
		if (totDist > half) ix0--;

		if (y1frac < 0) y1frac = -y1frac;

		totDist = y1frac - x1frac;
		if (totDist > half) ix1--;

		offset = x0 - ix0 - 1.0;

		gc->line.options.length = -dx;
		gc->line.options.numPixels = ix0 - ix1;

		goto xmajorfinish;
	    } else {		/* dy >= -dx >= 0, dy != 0 */
		gc->line.options.xBig = -1;
		goto posymajor;
	    }
	} else {
	    if (dx < dy) {	/* -dx > -dy >= 0 */
		gc->line.options.yBig = -1;
		goto negxmajor;
	    } else {		/* -dy >= -dx >= 0 */
		if (dx == dy && dy == 0) {
		    gc->line.options.numPixels = 0;
		    return;
		}
		gc->line.options.xBig = -1;
		goto negymajor;
	    }
	}
    }
}

void __glRenderAliasLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1)
{
    GLint i, n;
    __GLfloat invDelta;
    __GLfloat r;
    __GLcolor *cp;
    __GLfloat offset;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    __GL_ADJUST_LINE_VERTS(gc, v0, v1);

    __glInitLineData(gc, v0, v1);
    if (gc->line.options.numPixels == 0) {
	__GL_RESTORE_LINE_VERTS(gc, v0, v1);
	return;
    }

    offset = gc->line.options.offset;

    /*
    ** Set up increments for any enabled line options.
    */
    invDelta = __glOne / gc->line.options.length;
    if (modeFlags & __GL_SHADE_DEPTH_ITER) {
	__GLfloat dzdx;

	/*
	** Calculate window z coordinate increment and starting position.
	*/
	dzdx = (v1->window.z - v0->window.z) * invDelta;
	gc->polygon.shader.frag.z = (__GLzValue)(v0->window.z + dzdx * offset);
	gc->polygon.shader.dzdx = (GLint)dzdx;
	gc->polygon.shader.dzdxBig = 
	    gc->polygon.shader.dzdx << __GL_STIPPLE_COUNT_BITS;
    }

    if (modeFlags & __GL_SHADE_LINE_STIPPLE) {
	if (!gc->line.notResetStipple) {
	    gc->line.stipplePosition = 0;
	    gc->line.repeat = 0;
	    gc->line.notResetStipple = GL_TRUE;
	}
    }

    if (modeFlags & __GL_SHADE_SLOW_FOG) {
	__GLfloat f1, f0;
	__GLfloat dfdx;

	/*
	** Calculate eye z coordinate increment.
	*/
	if (gc->state.hints.fog == GL_NICEST) {
	    gc->line.options.f0 = f0 = v0->eye.z;
	    gc->polygon.shader.dfdx = dfdx = 
		    (v1->eye.z - v0->eye.z) * invDelta;
	} else {
	    f0 = v0->fog;
	    f1 = v1->fog;
	    gc->line.options.f0 = f0;
	    gc->polygon.shader.dfdx = dfdx = (f1 - f0) * invDelta;
	}
	gc->polygon.shader.frag.f = f0 + dfdx * offset;
    }

    if (modeFlags & __GL_SHADE_TEXTURE) {

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    __GLfloat v0S, v1S, v0T, v1T, v0R, v1R, v0QW, v1QW;
	    __GLfloat dS, dT, dR, dQWdX;
	    __GLfloat v0rhow, v1rhow, drhow;

	    if (!gc->texture.unit[i].currentTexture) { continue; }

	    v0S = v0->texture[i].x;
	    v1S = v1->texture[i].x;
	    v0T = v0->texture[i].y;
	    v1T = v1->texture[i].y;
	    v0R = v0->texture[i].z;
	    v1R = v1->texture[i].z;
	    v0QW = v0->texture[i].w;
	    v1QW = v1->texture[i].w;

	    if ((modeFlags & __GL_SHADE_PERSP_TEXTURE) &&
		!(modeFlags & __GL_SHADE_PROJSCALED_TEXTURE))
	    {
		v0S *= v0->window.w;
		v1S *= v1->window.w;
		v0T *= v0->window.w;
		v1T *= v1->window.w;
		v0R *= v0->window.w;
		v1R *= v1->window.w;
		v0QW *= v0->window.w;
		v1QW *= v1->window.w;
	    }

	    /*
	    ** Calculate texture s, t and r value increments.
	    */
	    dS = (v1S - v0S) * invDelta;
	    dT = (v1T - v0T) * invDelta;
	    dR = (v1R - v0R) * invDelta;
	    gc->polygon.shader.texture[i].dsdx = dS;
	    gc->polygon.shader.texture[i].dtdx = dT;
	    gc->polygon.shader.texture[i].drdx = dR;
	    gc->polygon.shader.texture[i].dqwdx = dQWdX = (v1QW - v0QW) * invDelta;
	    gc->polygon.shader.frag.texture[i].s = v0S + dS * offset;
	    gc->polygon.shader.frag.texture[i].t = v0T + dT * offset;
	    gc->polygon.shader.frag.texture[i].r = v0R + dR * offset;
	    gc->polygon.shader.frag.texture[i].qw = v0QW + dQWdX * offset;

	    v0rhow = (*gc->procs.calcLineRho)(gc, i, v0S, v0T, v0R, v0QW)*v0QW;

	    v1rhow = (*gc->procs.calcLineRho)(gc, i, v1S, v1T, v1R, v1QW)*v1QW;

	    drhow = (v1rhow - v0rhow) * invDelta;
	    gc->polygon.shader.texture[i].drhowdx = drhow;
	    gc->polygon.shader.frag.texture[i].rhow = v0rhow + drhow * offset;
	}
    } 

    n = __GL_PRIMARY_COLOR;
    if ( modeFlags & __GL_SHADE_SECONDARY_COLOR )
	n = __GL_SECONDARY_COLOR;

    if (modeFlags & __GL_SHADE_SMOOTH) {

	for ( i = __GL_PRIMARY_COLOR; i <= n; ++i ) {
	    /*
	    **  XXX Pointer arithmetic that depends on the
	    **   order of the __GL_{FRONT,BACK}FACE_SPECULAR
	    **   elements in the __GLvertex's color array
	    */
	    __GLcolor *c0 = v0->color + 2*i;
	    __GLcolor *c1 = v1->color + 2*i;
	    __GLfloat drdx, dgdx, dbdx, dadx;

	    /*
	    ** Calculate red, green, blue and alpha value increments.
	    */
	    drdx = (c1->r - c0->r) * invDelta;
	    gc->polygon.shader.colorIter[i].drdx = drdx;
	    if (gc->modes.rgbMode) {
		dgdx = (c1->g - c0->g) * invDelta;
		dbdx = (c1->b - c0->b) * invDelta;
		dadx = (c1->a - c0->a) * invDelta;
		gc->polygon.shader.colorIter[i].dgdx = dgdx;
		gc->polygon.shader.colorIter[i].dbdx = dbdx;
		gc->polygon.shader.colorIter[i].dadx = dadx;
	    }
	}
	cp = v0->color;
    } else {
	cp = v1->color;
    }

    if (modeFlags & __GL_SHADE_RGB) {
	__GLfloat   r, g,b,a;

	for ( i = __GL_PRIMARY_COLOR; i <= n; ++i ) {
	    cp += 2*i;

	    r = cp->r;
	    gc->polygon.shader.frag.color[i].r = r;
	    if ( gc->modes.rgbMode ) {
		g = cp->g;
		b = cp->b;
		a = cp->a;
		gc->polygon.shader.frag.color[i].g = g;
		gc->polygon.shader.frag.color[i].b = b;
		gc->polygon.shader.frag.color[i].a = a;
	    }
	}
    }

    gc->polygon.shader.length = gc->line.options.numPixels;
    (*gc->procs.line.processLine)(gc);

    __GL_RESTORE_LINE_VERTS(gc, v0, v1);

}


void __glRenderFlatFogLine(__GLcontext *gc, __GLvertex *v0, 
			   __GLvertex *v1)
{
    __GLcolor v0col, v1col;
    __GLcolor *v0ocp, *v1ocp;

    (*gc->procs.fogColor)(gc, &v0col, v1->color, v0->fog);
    (*gc->procs.fogColor)(gc, &v1col, v1->color, v1->fog);
    v0ocp = v0->color;
    v1ocp = v1->color;
    v0->color = &v0col;
    v1->color = &v1col;

    (*gc->procs.renderLine2)(gc, v0, v1);
    
    v0->color = v0ocp;
    v1->color = v1ocp;
}


/************************************************************************/

/*
** Most line functions will start off by computing the information 
** computed by this routine.
**
** The excessive number of labels in this routine is partly due
** to the fact that it is used as a model for writing an assembly 
** equivalent.
*/
static void InitAALineData(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1)
{
    GLint start;
    __GLfloat width;
    __GLfloat x0,y0,x1,y1;
    __GLfloat minorStart;
    GLint intMinorStart;
    __GLfloat dx, dy;
    __GLfloat offset;
    __GLfloat slope;
    __GLfloat halfWidth;
    __GLfloat realLength, oneOverRealLength;
    __GLfloat dldx, dldy;
    __GLfloat dddx, dddy;

    gc->line.options.v0 = v0;
    gc->line.options.v1 = v1;

    x0=v0->window.x;
    y0=v0->window.y;
    x1=v1->window.x;
    y1=v1->window.y;

    x0 -= __glHalf;
    y0 -= __glHalf;
    x1 -= __glHalf;
    y1 -= __glHalf;

    dx=x1-x0;
    dy=y1-y0;
    realLength = __GL_SQRTF(dx*dx+dy*dy);
    oneOverRealLength = __glOne / realLength;
    gc->line.options.realLength = realLength;
    gc->line.options.dldx = dldx = dx * oneOverRealLength;
    gc->line.options.dldy = dldy = dy * oneOverRealLength;
    gc->line.options.dddx = dddx = -dldy;
    gc->line.options.dddy = dddy = dldx;

    if (dx > __glZero) {
	if (dy > __glZero) {	/* dx > 0, dy > 0 */
	    gc->line.options.dlBig = dldx + dldy;
	    gc->line.options.ddBig = dddx + dddy;
	    if (dx > dy) {	/* dx > dy > 0 */
		gc->line.options.yBig = 1;
posxmajor:			/* dx > |dy| >= 0 */
		gc->line.options.yLittle = 0;
		gc->line.options.xBig = 1;
		gc->line.options.xLittle = 1;
		gc->line.options.dlLittle = dldx;
		gc->line.options.ddLittle = dddx;
		slope = dy/dx;
		start = (GLint) x0;
		offset = start + __glHalf - x0;

		gc->line.options.length = dx;
		gc->line.options.numPixels = (GLint)(__GL_CEILF(x1 - x0) + 1);

		width = __GL_CEILF(gc->state.line.smoothWidth * 
			realLength / dx);
xmajorfinish:
		gc->line.options.width = (GLint)width + 1;
		halfWidth = width * __glHalf;

		gc->line.options.axis = __GL_X_MAJOR;
		gc->line.options.xStart = start;
		gc->line.options.offset = offset;
		minorStart = y0 + offset*slope - halfWidth;
		intMinorStart = (GLint) minorStart;
		minorStart -= intMinorStart;
		gc->line.options.yStart = intMinorStart;
		gc->line.options.dfraction = (GLint)(slope * __TWO_31);
		gc->line.options.fraction = (GLint)(minorStart * __TWO_31);
	    } else {		/* dy >= dx > 0 */
		gc->line.options.xBig = 1;
posymajor:			/* dy >= |dx| >= 0, dy != 0 */
		gc->line.options.xLittle = 0;
		gc->line.options.yBig = 1;
		gc->line.options.yLittle = 1;
		gc->line.options.dlLittle = dldy;
		gc->line.options.ddLittle = dddy;
		slope = dx/dy;
		start = (GLint) y0;
		offset = start + __glHalf - y0;

		gc->line.options.length = dy;
		gc->line.options.numPixels = (GLint)__GL_CEILF(y1 - y0) + 1;

		width = __GL_CEILF(gc->state.line.smoothWidth * 
			realLength / dy);
ymajorfinish:
		gc->line.options.width = (GLint)width + 1;
		halfWidth = width * __glHalf;

		gc->line.options.axis = __GL_Y_MAJOR;
		gc->line.options.yStart = start;
		gc->line.options.offset = offset;
		minorStart = x0 + offset*slope - halfWidth;
		intMinorStart = (GLint) minorStart;
		minorStart -= intMinorStart;
		gc->line.options.xStart = intMinorStart;
		gc->line.options.dfraction = (GLint)(slope * __TWO_31);
		gc->line.options.fraction = (GLint)(minorStart * __TWO_31);
	    }
	} else {		/* dx > 0, dy <= 0 */
	    gc->line.options.dlBig = dldx - dldy;
	    gc->line.options.ddBig = dddx - dddy;
	    if (dx > -dy) {	/* dx > -dy >= 0 */
		gc->line.options.yBig = -1;
		goto posxmajor;
	    } else {		/* -dy >= dx >= 0, dy != 0 */
		gc->line.options.xBig = 1;
negymajor:			/* -dy >= |dx| >= 0, dy != 0 */
		gc->line.options.xLittle = 0;
		gc->line.options.yBig = -1;
		gc->line.options.yLittle = -1;
		gc->line.options.dlLittle = -dldy;
		gc->line.options.ddLittle = -dddy;
		slope = dx/-dy;
		start = (GLint) y0;
		offset = y0 - (start + __glHalf);

		gc->line.options.length = -dy;
		gc->line.options.numPixels = (GLint)__GL_CEILF(y0 - y1) + 1;

		width = __GL_CEILF(-gc->state.line.smoothWidth * 
			realLength / dy);

		goto ymajorfinish;
	    }
	}
    } else {
	if (dy > __glZero) {	/* dx <= 0, dy > 0 */
	    gc->line.options.dlBig = dldy - dldx;
	    gc->line.options.ddBig = dddy - dddx;
	    if (-dx > dy) {	/* -dx > dy > 0 */
		gc->line.options.yBig = 1;
negxmajor:			/* -dx > |dy| >= 0 */
		gc->line.options.yLittle = 0;
		gc->line.options.xBig = -1;
		gc->line.options.xLittle = -1;
		gc->line.options.dlLittle = -dldx;
		gc->line.options.ddLittle = -dddx;
		slope = dy/-dx;
		start = (GLint) x0;
		offset = x0 - (start + __glHalf);

		gc->line.options.length = -dx;
		gc->line.options.numPixels = (GLint)__GL_CEILF(x0 - x1) + 1;

		width = __GL_CEILF(-gc->state.line.smoothWidth * 
			realLength / dx);

		goto xmajorfinish;
	    } else {		/* dy >= -dx >= 0, dy != 0 */
		gc->line.options.xBig = -1;
		goto posymajor;
	    }
	} else {		/* dx <= 0, dy <= 0 */
	    gc->line.options.dlBig = -dldx - dldy;
	    gc->line.options.ddBig = -dddx - dddy;
	    if (dx < dy) {	/* -dx > -dy >= 0 */
		gc->line.options.yBig = -1;
		goto negxmajor;
	    } else {		/* -dy >= -dx >= 0 */
		if (dx == dy && dy == 0) {
		    gc->line.options.length = 0;
		    return;
		}
		gc->line.options.xBig = -1;
		goto negymajor;
	    }
	}
    }
}

void __glRenderAntiAliasLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1)
{
    GLint  i, n;
    __GLfloat invDelta;
    __GLcolor *cp;
    __GLfloat offset;
    GLint lineRep;
    GLint x, y, xBig, xLittle, yBig, yLittle;
    GLint fraction, dfraction;
    __GLfloat dlLittle, dlBig;
    __GLfloat ddLittle, ddBig;
    __GLfloat length, width;
    __GLfloat dx, dy;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    __GL_ADJUST_LINE_VERTS(gc, v0, v1);

    InitAALineData(gc, v0, v1);
    if (gc->line.options.length == 0) {
	__GL_RESTORE_LINE_VERTS(gc, v0, v1);
	return;
    }

    offset = gc->line.options.offset;

    /*
    ** Set up increments for any enabled line options.
    */
    invDelta = __glOne / gc->line.options.length;
    if (modeFlags & __GL_SHADE_DEPTH_ITER) {
	/*
	** Calculate window z coordinate increment and starting position.
	*/
	gc->polygon.shader.dzdx = (GLint)((v1->window.z - v0->window.z) * 
			invDelta);
	gc->polygon.shader.dzdxBig =
	    gc->polygon.shader.dzdx << __GL_STIPPLE_COUNT_BITS;
	gc->polygon.shader.frag.z = (GLint)(v0->window.z + 
		gc->polygon.shader.dzdx * offset);
    } 

    if (modeFlags & __GL_SHADE_LINE_STIPPLE) {
	if (!gc->line.notResetStipple) {
	    gc->line.stipplePosition = 0;
	    gc->line.repeat = 0;
	    gc->line.notResetStipple = GL_TRUE;
	}
    }

    if (modeFlags & __GL_SHADE_SLOW_FOG) {
	/*
	** Calculate eye z coordinate increment.
	*/
	if (gc->state.hints.fog == GL_NICEST) {
	    gc->line.options.f0 = v0->eye.z;
	    gc->polygon.shader.dfdx = (v1->eye.z - v0->eye.z) * invDelta;
	} else {
	    __GLfloat f1, f0;

	    f0 = v0->fog;
	    f1 = v1->fog;
	    gc->line.options.f0 = f0;
	    gc->polygon.shader.dfdx = (f1 - f0) * invDelta;
	}
	gc->polygon.shader.frag.f = gc->line.options.f0 + 
		gc->polygon.shader.dfdx * offset;
    } 

    n = __GL_PRIMARY_COLOR;
    if ( modeFlags & __GL_SHADE_SECONDARY_COLOR )
	n = __GL_SECONDARY_COLOR;
    
    if (modeFlags & __GL_SHADE_SMOOTH) {

	for ( i = 0; i <= n; ++i ) {
	    __GLcolor *c0 = v0->color + 2*i;
	    __GLcolor *c1 = v1->color + 2*i;

	    /*
	    ** Calculate red, green, blue and alpha value increments.
	    */
	    gc->polygon.shader.colorIter[i].drdx = (c1->r - c0->r) * invDelta;
	    if (gc->modes.rgbMode) {
		gc->polygon.shader.colorIter[i].dgdx = (c1->g - c0->g) * invDelta;
		gc->polygon.shader.colorIter[i].dbdx = (c1->b - c0->b) * invDelta;
		gc->polygon.shader.colorIter[i].dadx = (c1->a - c0->a) * invDelta;
	    }
	}
	cp = v0->color;
    } else {
	cp = v1->color;
    }
    
    for ( i = __GL_PRIMARY_COLOR; i <= n; ++i ) {
	cp += 2*i;
	gc->polygon.shader.frag.color[i].r = cp->r;
	if (gc->modes.rgbMode) {
	    gc->polygon.shader.frag.color[i].g = cp->g;
	    gc->polygon.shader.frag.color[i].b = cp->b;
	    gc->polygon.shader.frag.color[i].a = cp->a;
	}
    }
    
    if (modeFlags & __GL_SHADE_TEXTURE) {

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    __GLfloat v0S, v1S, v0T, v1T, v0R, v1R, v0QW, v1QW;
	    __GLfloat dS, dT, dR, dQWdX;
	    __GLfloat v0rhow, v1rhow, drhow;

	    if (gc->procs.textureUnit[i] == 0)
		continue;
	    
	    v0S = v0->texture[i].x;
	    v1S = v1->texture[i].x;
	    v0T = v0->texture[i].y;
	    v1T = v1->texture[i].y;
	    v0R = v0->texture[i].z;
	    v1R = v1->texture[i].z;
	    v0QW = v0->texture[i].w;
	    v1QW = v1->texture[i].w;

	    if ((modeFlags & __GL_SHADE_PERSP_TEXTURE) &&
		!(modeFlags & __GL_SHADE_PROJSCALED_TEXTURE))
	    {
		v0S *= v0->window.w;
		v1S *= v1->window.w;
		v0T *= v0->window.w;
		v1T *= v1->window.w;
		v0R *= v0->window.w;
		v1R *= v1->window.w;
		v0QW *= v0->window.w;
		v1QW *= v1->window.w;
	    }

	    /*
	    ** Calculate texture s, t and r value increments.
	    */
	    dS = (v1S - v0S) * invDelta;
	    dT = (v1T - v0T) * invDelta;
	    dR = (v1R - v0R) * invDelta;
	    gc->polygon.shader.texture[i].dsdx = dS;
	    gc->polygon.shader.texture[i].dtdx = dT;
	    gc->polygon.shader.texture[i].drdx = dR;
	    gc->polygon.shader.texture[i].dqwdx = dQWdX = (v1QW - v0QW) * invDelta;
	    gc->polygon.shader.frag.texture[i].s = v0S + dS * offset;
	    gc->polygon.shader.frag.texture[i].t = v0T + dT * offset;
	    gc->polygon.shader.frag.texture[i].r = v0R + dR * offset;
	    gc->polygon.shader.frag.texture[i].qw = v0QW + dQWdX * offset;

	    v0rhow = (*gc->procs.calcLineRho)(gc, i, v0S, v0T, v0R, v0QW)*v0QW;

	    v1rhow = (*gc->procs.calcLineRho)(gc, i, v1S, v1T, v1R, v1QW)*v1QW;

	    drhow = (v1rhow - v0rhow) * invDelta;
	    gc->polygon.shader.texture[i].drhowdx = drhow;
	    gc->polygon.shader.frag.texture[i].rhow = v0rhow + drhow * offset;
	}
    } 

    lineRep = gc->line.options.width;

    fraction = gc->line.options.fraction;
    dfraction = gc->line.options.dfraction;

    x = gc->line.options.xStart;
    y = gc->line.options.yStart;
    xBig = gc->line.options.xBig;
    yBig = gc->line.options.yBig;
    xLittle = gc->line.options.xLittle;
    yLittle = gc->line.options.yLittle;

    dlLittle = gc->line.options.dlLittle;
    dlBig = gc->line.options.dlBig;
    ddLittle = gc->line.options.ddLittle;
    ddBig = gc->line.options.ddBig;

    dx = x + __glHalf - v0->window.x;
    dy = y + __glHalf - v0->window.y;
    length = dx * gc->line.options.dldx +
	    dy * gc->line.options.dldy;
    width = dx * gc->line.options.dddx +
	    dy * gc->line.options.dddy;

    if (modeFlags & __GL_SHADE_LINE_STIPPLE) {
	gc->line.options.stippleOffset = 
		gc->line.stipplePosition * gc->state.line.stippleRepeat +
		gc->line.repeat - __glHalf;

	/* XXX Move to a validation routine? */
	gc->line.options.oneOverStippleRepeat = 
		__glOne / gc->state.line.stippleRepeat;
    }

    while (--lineRep >= 0) {
	/* Trace the line backwards as needed */
	while (length > -__glHalf) {
	    fraction -= dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		length -= dlBig;
		width -= ddBig;
		x -= xBig;
		y -= yBig;
	    } else {
		length -= dlLittle;
		width -= ddLittle;
		x -= xLittle;
		y -= yLittle;
	    }
	}

	/* Trace line forwards to correct */
	while (length <= -__glHalf) {
	    fraction += dfraction;
	    if (fraction < 0) {
		fraction &= ~0x80000000;
		length += dlBig;
		width += ddBig;
		x += xBig;
		y += yBig;
	    } else {
		length += dlLittle;
		width += ddLittle;
		x += xLittle;
		y += yLittle;
	    }
	}

	/* Save new fraction/dfraction */
	gc->line.options.plength = length;
	gc->line.options.pwidth = width;
	gc->line.options.fraction = fraction;
	gc->line.options.dfraction = dfraction;
	gc->line.options.xStart = x;
	gc->line.options.yStart = y;
    
	gc->polygon.shader.length = gc->line.options.numPixels;
	(*gc->procs.line.processLine)(gc);

	if (gc->line.options.axis == __GL_X_MAJOR) {
	    y++;
	    length += gc->line.options.dldy;
	    width += gc->line.options.dddy;
	} else {
	    x++;
	    length += gc->line.options.dldx;
	    width += gc->line.options.dddx;
	}
    }

    if (modeFlags & __GL_SHADE_LINE_STIPPLE) {
	/* Update stipple.  Ugh. */
	int increase;
	int posInc;

	/* Shift stipple by 'increase' bits */
	increase = (GLint)__GL_CEILF(gc->line.options.realLength);

	posInc = increase / gc->state.line.stippleRepeat;

	gc->line.stipplePosition = (gc->line.stipplePosition + posInc) & 0xf;
	gc->line.repeat = (gc->line.repeat + increase) % gc->state.line.stippleRepeat;
    }

    __GL_RESTORE_LINE_VERTS(gc, v0, v1);
}
