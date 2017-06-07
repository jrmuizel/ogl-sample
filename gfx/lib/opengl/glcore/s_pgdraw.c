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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_pgdraw.c#13 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "g_imports.h"

/* This routine sets gc->polygon.shader.cfb to gc->drawBuffer */

static void FillSubTriangle(__GLcontext *gc, GLint iyBottom, GLint iyTop)
{
    GLint i, n;
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
	spanWidth = ixRight - ixLeft;
	/*
	** Only render spans that have non-zero width and which are
	** not scissored out vertically.
	*/
	if ((spanWidth > 0) && (iyBottom >= clipY0) && (iyBottom < clipY1)) {
	    gc->polygon.shader.frag.x = ixLeft;
	    gc->polygon.shader.frag.y = iyBottom;
	    gc->polygon.shader.length = spanWidth;
	    (*gc->procs.span.processSpan)(gc);
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

	n = __GL_PRIMARY_COLOR;
	if ( modeFlags & __GL_SHADE_SECONDARY_COLOR )
	    n = __GL_SECONDARY_COLOR;
	
	iyBottom++;
	ixLeftFrac += dxLeftFrac;
	if (ixLeftFrac < 0) {
	    /* Carry/Borrow'd.  Use large step */
	    ixLeft += dxLeftBig;
	    ixLeftFrac &= ~0x80000000;

	    if (modeFlags & __GL_SHADE_RGB) {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    __GLshade *sh = &gc->polygon.shader;
		    for ( i = 0; i <= n; i++ ) {
			sh->frag.color[i].r += sh->colorIter[i].rBig;
			sh->frag.color[i].g += sh->colorIter[i].gBig;
			sh->frag.color[i].b += sh->colorIter[i].bBig;
			sh->frag.color[i].a += sh->colorIter[i].aBig;
		    }
		}
	    } else {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r +=
			gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].rBig;
		    if (modeFlags & __GL_SHADE_SECONDARY_COLOR) 
			gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].r +=
			    gc->polygon.shader.colorIter[__GL_SECONDARY_COLOR].rBig;
		}
	    }
	    if (modeFlags & __GL_SHADE_TEXTURE) {
		for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
		    gc->polygon.shader.frag.texture[i].s +=
					gc->polygon.shader.texture[i].sBig;
		    gc->polygon.shader.frag.texture[i].t +=
					gc->polygon.shader.texture[i].tBig;
		    gc->polygon.shader.frag.texture[i].r +=
					gc->polygon.shader.texture[i].rBig;
		    gc->polygon.shader.frag.texture[i].qw +=
					gc->polygon.shader.texture[i].qwBig;
		    gc->polygon.shader.frag.texture[i].rhow +=
					gc->polygon.shader.texture[i].rhowBig;
		}
	    }
	    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
		/* The implicit multiply is taken out of the loop */
		gc->polygon.shader.sbuf = (__GLstencilCell*)
		    ((GLubyte*) gc->polygon.shader.sbuf
		     + gc->polygon.shader.sbufBig);
	    }
	    if (modeFlags & __GL_SHADE_DEPTH_ITER) {
		gc->polygon.shader.frag.z += gc->polygon.shader.zBig;
	    }

	    if (modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
	      gc->polygon.shader.offsetZ += gc->polygon.shader.zBig;
		gc->polygon.shader.outBoundCount += 
		  gc->polygon.shader.outCountBig;
#if defined(POLYGON_OFFSET_DEBUG)
	      if(gc->polygon.shader.outBoundCount > 0. &&
		 (gc->polygon.shader.outBoundCount < 
		  gc->polygon.shader.rangeCount))
		assert(gc->polygon.shader.offsetZ >= 0. &&
		       gc->polygon.shader.offsetZ <= gc->depthBuffer.scale);
#endif /* POLYGON_OFFSET_DEBUG */
	    }

	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		/* The implicit multiply is taken out of the loop */
		gc->polygon.shader.zbuf = (__GLzValue*)
		    ((GLubyte*) gc->polygon.shader.zbuf
		     + gc->polygon.shader.zbufBig);
	    }
	    if (modeFlags & __GL_SHADE_SLOW_FOG) {
		gc->polygon.shader.frag.f += gc->polygon.shader.fBig;
	    }
	} else {
	    /* Use small step */
	    ixLeft += dxLeftLittle;
	    if (modeFlags & __GL_SHADE_RGB) {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    __GLshade *sh = &gc->polygon.shader;
		    for ( i = 0; i <= n; i++ ) {
			sh->frag.color[i].r += sh->colorIter[i].rLittle;
			sh->frag.color[i].g += sh->colorIter[i].gLittle;
			sh->frag.color[i].b += sh->colorIter[i].bLittle;
			sh->frag.color[i].a += sh->colorIter[i].aLittle;
		    }
		}
	    } else {
		if (modeFlags & __GL_SHADE_SMOOTH) {
		    gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR].r +=
			gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].rLittle;
		    if ( modeFlags & __GL_SHADE_SECONDARY_COLOR ) 
			gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR].r +=
			    gc->polygon.shader.colorIter[__GL_SECONDARY_COLOR].rLittle;
		}
	    }
	    if (modeFlags & __GL_SHADE_TEXTURE) {
		for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
		    gc->polygon.shader.frag.texture[i].s +=
					gc->polygon.shader.texture[i].sLittle;
		    gc->polygon.shader.frag.texture[i].t +=
					gc->polygon.shader.texture[i].tLittle;
		    gc->polygon.shader.frag.texture[i].r +=
					gc->polygon.shader.texture[i].rLittle;
		    gc->polygon.shader.frag.texture[i].qw +=
					gc->polygon.shader.texture[i].qwLittle;
		    gc->polygon.shader.frag.texture[i].rhow +=
					gc->polygon.shader.texture[i].rhowLittle;
		}
	    }
	    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
		/* The implicit multiply is taken out of the loop */
		gc->polygon.shader.sbuf = (__GLstencilCell*)
		    ((GLubyte*) gc->polygon.shader.sbuf
		     + gc->polygon.shader.sbufLittle);
	    }
	    if (modeFlags & __GL_SHADE_DEPTH_ITER) {
		gc->polygon.shader.frag.z += gc->polygon.shader.zLittle;
	    }

	    if (modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
	        gc->polygon.shader.offsetZ += gc->polygon.shader.zLittle;
		gc->polygon.shader.outBoundCount += 
		  gc->polygon.shader.outCountLittle;
#if defined(POLYGON_OFFSET_DEBUG)
	      if(gc->polygon.shader.outBoundCount > 0. &&
		 (gc->polygon.shader.outBoundCount < 
		  gc->polygon.shader.rangeCount))
		assert(gc->polygon.shader.offsetZ >= 0. &&
		       gc->polygon.shader.offsetZ <= gc->depthBuffer.scale);
#endif /* POLYGON_OFFSET_DEBUG */
	    }
	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		/* The implicit multiply is taken out of the loop */
		gc->polygon.shader.zbuf = (__GLzValue*)
		    ((GLubyte*) gc->polygon.shader.zbuf
		     + gc->polygon.shader.zbufLittle);
	    }
	    if (modeFlags & __GL_SHADE_SLOW_FOG) {
		gc->polygon.shader.frag.f += gc->polygon.shader.fLittle;
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

void __glSnapXLeft(__GLcontext *gc, __GLfloat xLeft, __GLfloat dxdyLeft)
{
    __GLfloat little, dx;
    GLint ixLeft, ixLeftFrac, frac, lineBytes, elementSize, ilittle, ibig;

    ixLeft = (GLint) xLeft;
    dx = xLeft - ixLeft;
    __FRACTION(ixLeftFrac,dx);

    gc->polygon.shader.ixLeft = ixLeft + (((GLuint) ixLeftFrac) >> 31);
    gc->polygon.shader.ixLeftFrac = ixLeftFrac & ~0x80000000;

    /* Compute big and little steps */
    ilittle = (GLint) dxdyLeft;
    little = (__GLfloat) ilittle;
    if (dxdyLeft < 0) {
	ibig = ilittle - 1;
	dx = little - dxdyLeft;
	__FRACTION(frac,dx);
	gc->polygon.shader.dxLeftFrac = -frac;
    } else {
	ibig = ilittle + 1;
	dx = dxdyLeft - little;
	__FRACTION(frac,dx);
	gc->polygon.shader.dxLeftFrac = frac;
    }
    if (gc->polygon.shader.modeFlags & __GL_SHADE_STENCIL_TEST) {
	/*
	** Compute the big and little stencil buffer steps.  We walk the
	** memory pointers for the stencil buffer along the edge of the
	** triangle as we walk the edge.  This way we don't have to
	** recompute the buffer address as we go.
	*/
	elementSize = gc->stencilBuffer.buf.drawableBuf->elementSize;
	lineBytes = elementSize * gc->stencilBuffer.buf.drawableBuf->outerWidth;
	gc->polygon.shader.sbufLittle = lineBytes + ilittle * elementSize;
	gc->polygon.shader.sbufBig = lineBytes + ibig * elementSize;
    }
    if (gc->polygon.shader.modeFlags & __GL_SHADE_DEPTH_TEST) {
	/*
	** Compute the big and little depth buffer steps.  We walk the
	** memory pointers for the depth buffer along the edge of the
	** triangle as we walk the edge.  This way we don't have to
	** recompute the buffer address as we go.
	*/
	elementSize = gc->depthBuffer.buf.drawableBuf->elementSize;
	lineBytes = elementSize * gc->depthBuffer.buf.drawableBuf->outerWidth;
	gc->polygon.shader.zbufLittle = lineBytes + ilittle * elementSize;
	gc->polygon.shader.zbufBig = lineBytes + ibig * elementSize;
    }
    gc->polygon.shader.dxLeftLittle = ilittle;
    gc->polygon.shader.dxLeftBig = ibig;
}

void __glSnapXRight(__GLshade *sh, __GLfloat xRight, __GLfloat dxdyRight)
{
    __GLfloat little, big, dx;
    GLint ixRight, ixRightFrac, frac;

    ixRight = (GLint) xRight;
    dx = xRight - ixRight;
    __FRACTION(ixRightFrac,dx);

    sh->ixRight = ixRight + (((GLuint) ixRightFrac) >> 31);
    sh->ixRightFrac = ixRightFrac & ~0x80000000;

    /* Compute big and little steps */
    little = (__GLfloat) ((GLint) dxdyRight);
    if (dxdyRight < 0) {
	big = little - 1;
	dx = little - dxdyRight;
	__FRACTION(frac,dx);
	sh->dxRightFrac = -frac;
    } else {
	big = little + 1;
	dx = dxdyRight - little;
	__FRACTION(frac,dx);
	sh->dxRightFrac = frac;
    }
    sh->dxRightLittle = (GLint) little;
    sh->dxRightBig = (GLint) big;
}

__GLzValue __glPolygonOffsetZ(__GLcontext *gc, const __GLvertex *a, 
			      __GLfloat dx, __GLfloat dy)
{

    __GLshade *sh = &gc->polygon.shader;
    __GLfloat factor;
    __GLfloat maxdZ;
    __GLfloat bias;
    __GLfloat newZ;

    /*
    ** Calculate factor and bias
    */
    factor = gc->state.polygon.factor;
    bias = gc->state.polygon.units * gc->depthBuffer.minResolution;

    /*
    ** find the maximum Z slope with respect to X and Y
    */
    if(__GL_ABSF(sh->dzdxf) > __GL_ABSF(sh->dzdyf))
        maxdZ = __GL_ABSF(sh->dzdxf);
    else
        maxdZ = __GL_ABSF(sh->dzdyf);

    newZ = (a->window.z + dx*sh->dzdxf + dy*sh->dzdyf) +
           factor * maxdZ + 
	   bias;

    /*
    ** Order the limits that might be crossed
    */
    if((GLint)sh->dzdx < 0) {
      sh->startZLimit = gc->depthBuffer.scale;
      sh->endZLimit = 0;
    } else {
      sh->startZLimit = 0;
      sh->endZLimit = gc->depthBuffer.scale;
    }

    sh->offsetZ = newZ;

    if(sh->dzdxf > 0.0) { /* bound is max z value */
      sh->outBoundCount = (gc->depthBuffer.scale - newZ)/sh->dzdxf;
      sh->rangeCount = (GLdouble)gc->depthBuffer.scale/sh->dzdxf;
    } else if(sh->dzdxf < 0.0) { /* bound is min z value */
      sh->outBoundCount = -newZ/sh->dzdxf;
      sh->rangeCount = gc->depthBuffer.scale/-sh->dzdxf;
    } else { /* dzdxf == 0; use a near-zero number, (no dz for entire span) */
      sh->outBoundCount = (gc->depthBuffer.scale - newZ)/
	                  __GL_PGON_OFFSET_NEAR_ZERO;
      sh->rangeCount = gc->depthBuffer.scale/__GL_PGON_OFFSET_NEAR_ZERO;
    }

    return((__GLzValue)newZ);
}


static void SetInitialParameters(__GLcontext *gc, const __GLvertex *a,
				 __GLfloat aFog, __GLfloat aRhow,
				 __GLfloat dx, __GLfloat dy)
{
    int i, n;
    __GLshade *sh = &gc->polygon.shader;
    __GLfloat big = sh->dxLeftBig;
    __GLfloat little = sh->dxLeftLittle;
    __GLcolor *vertexColor;
    __GLcolor *fragColor;
    __GLcolorIterator *iter;
    GLuint modeFlags = sh->modeFlags;
    
    n = __GL_PRIMARY_COLOR;
    if (modeFlags & __GL_SHADE_SECONDARY_COLOR)
	n = __GL_SECONDARY_COLOR;
    
    if (big > little) {
	if (modeFlags & __GL_SHADE_RGB) {
	    if (modeFlags & __GL_SHADE_SMOOTH) {

		for( i = __GL_PRIMARY_COLOR; i <= n; i++ ) {
		    /*
		    **  XXX Pointer arithmetic that depends on the
		    **   order of the __GL_{FRONT,BACK)FACE_SPECULAR
		    **   elements in the __GLvertex's color array
		    */
		    vertexColor = a->color + 2*i;
		    fragColor = &sh->frag.color[i];
		    iter = &sh->colorIter[i];

		    fragColor->r = vertexColor->r + dx*iter->drdx +
			dy*iter->drdy;
		    iter->rLittle = iter->drdy + little * iter->drdx;
		    iter->rBig = iter->rLittle + iter->drdx;

		    fragColor->g = vertexColor->g + dx*iter->dgdx +
			dy*iter->dgdy;
		    iter->gLittle = iter->dgdy + little * iter->dgdx;
		    iter->gBig = iter->gLittle + iter->dgdx;

		    fragColor->b = vertexColor->b + dx*iter->dbdx +
			dy*iter->dbdy;
		    iter->bLittle = iter->dbdy + little * iter->dbdx;
		    iter->bBig = iter->bLittle + iter->dbdx;

		    fragColor->a = vertexColor->a + dx*iter->dadx +
			dy*iter->dady;
		    iter->aLittle = iter->dady + little * iter->dadx;
		    iter->aBig =iter->aLittle + iter->dadx;
		}
	    }
	} else {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		
		for( i = __GL_PRIMARY_COLOR; i <= n; i++ ) {
		    /*
		    **  XXX Pointer arithmetic that depends on the
		    **   order of the __GL_{FRONT,BACK)FACE_SPECULAR
		    **   elements in the __GLvertex's color array
		    */
		    vertexColor = a->color + 2*i;
		    fragColor = &sh->frag.color[i];
		    iter = &sh->colorIter[i];

		    fragColor->r = vertexColor->r + dx*iter->drdx +
			dy*iter->drdy;
		    iter->rLittle = iter->drdy + little * iter->drdx;
		    iter->rBig = iter->rLittle + iter->drdx;
		}
	    }
	
	}
	if (modeFlags & __GL_SHADE_TEXTURE) {
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

	    zLittle = sh->dzdyf + little * sh->dzdxf;
	    sh->zLittle = (GLint)zLittle;
	    sh->zBig = (GLint)(zLittle + sh->dzdxf);

	    if(gc->state.enables.general & __GL_POLYGON_OFFSET_FILL_ENABLE) {
	        sh->frag.z = __glPolygonOffsetZ(gc, a, dx, dy);
		if(sh->dzdxf == 0.) { /* approximate dzdxf == 0. */
		    sh->outCountBig = -sh->zBig/__GL_PGON_OFFSET_NEAR_ZERO;
		    sh->outCountLittle = -sh->zLittle/__GL_PGON_OFFSET_NEAR_ZERO;
		} else {
		    sh->outCountBig = -sh->zBig/sh->dzdxf;
		    sh->outCountLittle = -sh->zLittle/sh->dzdxf;
		}
	    } else {
	        sh->frag.z = (__GLzValue)
		    (a->window.z + dx*sh->dzdxf + dy*sh->dzdyf);
	    }
	}
	if (modeFlags & __GL_SHADE_SLOW_FOG) {
	    sh->frag.f = aFog + dx*sh->dfdx + dy*sh->dfdy;
	    sh->fLittle = sh->dfdy + little * sh->dfdx;
	    sh->fBig = sh->fLittle + sh->dfdx;
	}
    } else {	
	if (modeFlags & __GL_SHADE_RGB) {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		for( i = __GL_PRIMARY_COLOR; i <= n; i++ ) {
		    /*
		    **  XXX Pointer arithmetic that depends on the
		    **   order of the __GL_{FRONT,BACK)FACE_SPECULAR
		    **   elements in the __GLvertex's color array
		    */
		    vertexColor = a->color + 2*i;
		    fragColor = &sh->frag.color[i];
		    iter = &sh->colorIter[i];

		    fragColor->r = vertexColor->r + dx*iter->drdx +
			dy*iter->drdy;
		    iter->rLittle = iter->drdy + little * iter->drdx;
		    iter->rBig = iter->rLittle - iter->drdx;

		    fragColor->g = vertexColor->g + dx*iter->dgdx +
			dy*iter->dgdy;
		    iter->gLittle = iter->dgdy + little * iter->dgdx;
		    iter->gBig = iter->gLittle - iter->dgdx;

		    fragColor->b = vertexColor->b + dx*iter->dbdx +
			dy*iter->dbdy;
		    iter->bLittle = iter->dbdy + little * iter->dbdx;
		    iter->bBig = iter->bLittle - iter->dbdx;

		    fragColor->a = vertexColor->a + dx*iter->dadx +
			dy*iter->dady;
		    iter->aLittle = iter->dady + little * iter->dadx;
		    iter->aBig =iter->aLittle - iter->dadx;
		}
	    }
	} else {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		for( i = __GL_PRIMARY_COLOR; i <= n; i++ ) {
		    /*
		    **  XXX Pointer arithmetic that depends on the
		    **   order of the __GL_{FRONT,BACK)FACE_SPECULAR
		    **   elements in the __GLvertex's color array
		    */
		    vertexColor = a->color + 2*i;
		    fragColor = &sh->frag.color[i];
		    iter = &sh->colorIter[i];

		    fragColor->r = vertexColor->r + dx*iter->drdx +
			dy*iter->drdy;
		    iter->rLittle = iter->drdy + little * iter->drdx;
		    iter->rBig = iter->rLittle - iter->drdx;
		}
	    }
	}
	if (modeFlags & __GL_SHADE_TEXTURE) {
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
	    zLittle = sh->dzdyf + little * sh->dzdxf;
	    sh->zLittle = (GLint)zLittle;
	    sh->zBig = (GLint)(zLittle - sh->dzdxf);
	    if(gc->state.enables.general & __GL_POLYGON_OFFSET_FILL_ENABLE) {
	        sh->frag.z = __glPolygonOffsetZ(gc, a, dx, dy);
		if(sh->dzdxf == 0.) { /* approximate dzdxf == 0. */
		  sh->outCountBig = -sh->zBig/__GL_PGON_OFFSET_NEAR_ZERO;
		  sh->outCountLittle = -sh->zLittle/__GL_PGON_OFFSET_NEAR_ZERO;
		} else {
		  sh->outCountBig = -sh->zBig/sh->dzdxf;
		  sh->outCountLittle = -sh->zLittle/sh->dzdxf;
		}
	    } else {
	        sh->frag.z = (__GLzValue)
		             (a->window.z + dx*sh->dzdxf + dy*sh->dzdyf);
	    }
	}
	if (modeFlags & __GL_SHADE_SLOW_FOG) {
	    sh->frag.f = aFog + dx*sh->dfdx + dy*sh->dfdy;
	    sh->fLittle = sh->dfdy + little * sh->dfdx;
	    sh->fBig = sh->fLittle - sh->dfdx;
	}
    }
}

void __glFillTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
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

    __GL_ADJUST_TRIANGLE_VERTS(gc, oa, ob, oc, gc->vertex.provoking);

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

    /* Compute signed area of the triangle */
    dxAC = a->window.x - c->window.x;
    dxBC = b->window.x - c->window.x;
    dyAC = a->window.y - c->window.y;
    dyBC = b->window.y - c->window.y;
    area = dxAC * dyBC - dxBC * dyAC;
    ccw = !(*(int *)&area >> 31);

    /* Pre-compute one over polygon area */
    oneOverArea = area ? __glOne / area : __glZero;

    /* Fetch some stuff we are going to reuse */
    modeFlags = gc->polygon.shader.modeFlags;
    ac = a->color;
    bc = b->color;

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
	    int i, n;
	    __GLfloat drAC, dgAC, dbAC, daAC;
	    __GLfloat drBC, dgBC, dbBC, daBC;
	    __GLcolor *acp, *bcp, *ccp;
	    __GLcolorIterator *iter;

	    /*
	    **  XXX This really sucks; its chuck full of assumptions
	    **   because of the "color" pointer in the vertex structure.
	    **   Use of the color pointer assumes that the separate
	    **   specular color (__GL_SECONDARY_COLOR) is in the second
	    **   __GLcolor after vertexPointer->color.
	    */
	    n = __GL_PRIMARY_COLOR;
	    if (modeFlags & __GL_SHADE_SECONDARY_COLOR)
		n = __GL_SECONDARY_COLOR;

	    for ( i = __GL_PRIMARY_COLOR; i <= n; i++ ) {
		iter = &gc->polygon.shader.colorIter[i];
		/*
		**  XXX Pointer arithmetic that depends on the
		**   order of the __GL_{FRONT,BACK)FACE_SPECULAR
		**   elements in the __GLvertex's color array
		*/
		acp = a->color + 2*i;
		bcp = b->color + 2*i;
		ccp = c->color + 2*i;

		drAC = acp->r - ccp->r;
		drBC = bcp->r - ccp->r;
		iter->drdx = drAC * t2 - drBC * t1;
		iter->drdy = drBC * t3 - drAC * t4;
		dgAC = acp->g - ccp->g;
		dgBC = bcp->g - ccp->g;
		iter->dgdx = dgAC * t2 - dgBC * t1;
		iter->dgdy = dgBC * t3 - dgAC * t4;
		dbAC = acp->b - ccp->b;
		dbBC = bcp->b - ccp->b;
		iter->dbdx = dbAC * t2 - dbBC * t1;
		iter->dbdy = dbBC * t3 - dbAC * t4;
		daAC = acp->a - ccp->a;
		daBC = bcp->a - ccp->a;
		iter->dadx = daAC * t2 - daBC * t1;
		iter->dady = daBC * t3 - daAC * t4;
	    }
	} else {
	    __GLcolor *flatColor;
	    __GLcolor *fragColor;

	    flatColor = gc->vertex.provoking->color;
	    fragColor = &gc->polygon.shader.frag.color[__GL_PRIMARY_COLOR];
	    fragColor->r = flatColor->r;
	    fragColor->g = flatColor->g;
	    fragColor->b = flatColor->b;
	    fragColor->a = flatColor->a;

	    if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
		/*
		** XXX this assmes that the second vertex color is in the
		**  second location following whatever the current face color
		**  is.  See vertex.h for more info.
		*/
		flatColor = gc->vertex.provoking->color + 2;
		fragColor =
		    &gc->polygon.shader.frag.color[__GL_SECONDARY_COLOR];
		fragColor->r = flatColor->r;
		fragColor->g = flatColor->g;
		fragColor->b = flatColor->b;
		fragColor->a = flatColor->a;
	    }
	}
    } else {
	if (modeFlags & __GL_SHADE_SMOOTH) {
	    __GLfloat drAC;
	    __GLfloat drBC;
	    __GLcolor *cc;

	    cc = c->color;
	    drAC = ac->r - cc->r;
	    drBC = bc->r - cc->r;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdx =
		drAC * t2 - drBC * t1;
	    gc->polygon.shader.colorIter[__GL_PRIMARY_COLOR].drdy =
		drBC * t3 - drAC * t4;
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

	    if (!gc->texture.unit[i].currentTexture) { continue; }

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
	SetInitialParameters(gc, a, aFog, aRhow, dx, dy);
	if (aIY != bIY) {
		dxdyRight = dyAB ? dxAB / dyAB : __glZero;
	    __glSnapXRight(&gc->polygon.shader, a->window.x + dy*dxdyRight,
		       dxdyRight);
	    FillSubTriangle(gc, aIY, bIY);
	}

	if (bIY != cIY) {
	    dxdyRight = dyBC ? dxBC / dyBC : __glZero;
	    dy = (bIY + __glOne) - b->window.y;
	    __glSnapXRight(&gc->polygon.shader, b->window.x + dy*dxdyRight,
		       dxdyRight);
	    FillSubTriangle(gc, bIY, cIY);
	}
    } else {
	dxdyRight = dyAC ? dxAC / dyAC : __glZero;
	dy = (aIY + __glOne) - a->window.y;
	__glSnapXRight(&gc->polygon.shader, a->window.x + dy*dxdyRight, dxdyRight);
	if (aIY != bIY) {
	    dxdyLeft = dyAB ? dxAB / dyAB : __glZero;
	    __glSnapXLeft(gc, a->window.x + dy*dxdyLeft, dxdyLeft);
	    dx = (gc->polygon.shader.ixLeft + __glOne) - a->window.x;
	    SetInitialParameters(gc, a, aFog, aRhow, dx, dy);
	    FillSubTriangle(gc, aIY, bIY);
	}

	if (bIY != cIY) {
	    dxdyLeft = dyBC ? dxBC / dyBC : __glZero;
	    dy = (bIY + __glOne) - b->window.y;
	    __glSnapXLeft(gc, b->window.x + dy*dxdyLeft, dxdyLeft);
	    dx = (gc->polygon.shader.ixLeft + __glOne) - b->window.x;
	    SetInitialParameters(gc, b, bFog, bRhow, dx, dy);
	    FillSubTriangle(gc, bIY, cIY);
	}
    }

    __GL_RESTORE_TRIANGLE_VERTS(gc, oa, ob, oc, gc->vertex.provoking);
}

void __glFillFlatFogTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
		             __GLvertex *c)
{
    __GLcolor acol, bcol, ccol;
    __GLcolor *aocp, *bocp, *cocp;
    __GLvertex *pv;

    pv = gc->vertex.provoking;
    (*gc->procs.fogColor)(gc, &acol, pv->color, a->fog);
    (*gc->procs.fogColor)(gc, &bcol, pv->color, b->fog);
    (*gc->procs.fogColor)(gc, &ccol, pv->color, c->fog);
    aocp = a->color;
    bocp = b->color;
    cocp = c->color;
    a->color = &acol;
    b->color = &bcol;
    c->color = &ccol;

    (*gc->procs.fillTriangle2)(gc, a, b, c);

    a->color = aocp;
    b->color = bocp;
    c->color = cocp;
}
