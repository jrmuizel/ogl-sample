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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_blend.c#8 $
*/
#include "context.h"
#include "global.h"
#include <math.h>

/*
** The following blendOp is called from the Dest
** blend functions.  They assume the src result is already stored
** in result.
*/
/* ARGSUSED */
static void BlendAdd(__GLcontext *gc, const __GLcolor *source,
		     const __GLcolor *dest, __GLcolor *result)
{
    result->r += dest->r;
    result->g += dest->g;
    result->b += dest->b;
    result->a += dest->a;
}

static void BlendSubtract(__GLcontext *gc, const __GLcolor *source,
			  const __GLcolor *dest, __GLcolor *result)
{
    result->r -= dest->r;
    result->g -= dest->g;
    result->b -= dest->b;
    result->a -= dest->a;
}

static void BlendReverseSubtract(__GLcontext *gc, const __GLcolor *source,
				 const __GLcolor *dest, __GLcolor *result)
{
    result->r = dest->r - result->r;
    result->g = dest->g - result->g;
    result->b = dest->b - result->b;
    result->a = dest->a - result->a;
}

static void DoBlendSourceZERO(__GLcontext *gc, const __GLcolor *source,
			      const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat zero = __glZero;

#if defined(__GL_LINT)
    gc = gc;
    source = source;
    dest = dest;
#endif /* __GL_LINT */
    result->r = zero;
    result->g = zero;
    result->b = zero;
    result->a = zero;
}

/* ARGSUSED */
static void DoBlendDestZERO(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat zero = __glZero;
    __GLcolor tmpc;

    tmpc.r = zero;
    tmpc.g = zero;
    tmpc.b = zero;
    tmpc.a = zero;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceONE(__GLcontext *gc, const __GLcolor *source,
			     const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b, a;

#if defined(__GL_LINT)
    gc = gc;
    dest = dest;
#endif /* __GL_LINT */
    r = source->r;
    g = source->g;
    b = source->b;
    a = source->a;

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendDestONE(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
#if defined(__GL_LINT)
    gc = gc;
    source = source;
#endif /* __GL_LINT */

    gc->procs.blendEquation(gc, source, dest,  result);
}

static void DoBlendDestSC(__GLcontext *gc, const __GLcolor *source,
			  const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;

    tmpc.r = dest->r * source->r * gc->frontBuffer.oneOverRedScale;
    tmpc.g = dest->g * source->g * gc->frontBuffer.oneOverGreenScale;
    tmpc.b = dest->b * source->b * gc->frontBuffer.oneOverBlueScale;
    tmpc.a = dest->a * source->a * gc->frontBuffer.oneOverAlphaScale;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendDestMSC(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat one = __glOne;

    tmpc.r = dest->r * (one - source->r * gc->frontBuffer.oneOverRedScale);
    tmpc.g = dest->g * (one - source->g * gc->frontBuffer.oneOverGreenScale);
    tmpc.b = dest->b * (one - source->b * gc->frontBuffer.oneOverBlueScale);
    tmpc.a = dest->a * (one - source->a * gc->frontBuffer.oneOverAlphaScale);

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceDC(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b, a;

    r = source->r * dest->r * gc->frontBuffer.oneOverRedScale;
    g = source->g * dest->g * gc->frontBuffer.oneOverGreenScale;
    b = source->b * dest->b * gc->frontBuffer.oneOverBlueScale;
    a = source->a * dest->a * gc->frontBuffer.oneOverAlphaScale;

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendSourceMDC(__GLcontext *gc, const __GLcolor *source,
			     const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b, a;
    __GLfloat one = __glOne;

    r = source->r * (one - dest->r * gc->frontBuffer.oneOverRedScale);
    g = source->g * (one - dest->g * gc->frontBuffer.oneOverGreenScale);
    b = source->b * (one - dest->b * gc->frontBuffer.oneOverBlueScale);
    a = source->a * (one - dest->a * gc->frontBuffer.oneOverAlphaScale);

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendSourceSA(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b, a;

#if defined(__GL_LINT)
    dest = dest;
#endif /* __GL_LINT */
    a = source->a * gc->frontBuffer.oneOverAlphaScale;

    r = a * source->r;
    g = a * source->g;
    b = a * source->b;
    a = a * source->a;

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendDestSA(__GLcontext *gc, const __GLcolor *source,
			  const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat a;

    a = source->a * gc->frontBuffer.oneOverAlphaScale;

    tmpc.r = a * dest->r;
    tmpc.g = a * dest->g;
    tmpc.b = a * dest->b;
    tmpc.a = a * dest->a;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceMSA(__GLcontext *gc, const __GLcolor *source,
			     const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b, a;
    __GLfloat msa = __glOne - source->a * gc->frontBuffer.oneOverAlphaScale;

#if defined(__GL_LINT)
    dest = dest;
#endif /* __GL_LINT */
    r = msa * source->r;
    g = msa * source->g;
    b = msa * source->b;
    a = msa * source->a;

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendDestMSA(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat msa =
	    __glOne - source->a * gc->frontBuffer.oneOverAlphaScale;

    tmpc.r = msa * dest->r;
    tmpc.g = msa * dest->g;
    tmpc.b = msa * dest->b;
    tmpc.a = msa * dest->a;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceDA(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b;
    __GLfloat a = dest->a * gc->frontBuffer.oneOverAlphaScale;

    r = a * source->r;
    g = a * source->g;
    b = a * source->b;
    a = a * source->a;

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendDestDA(__GLcontext *gc, const __GLcolor *source,
			  const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat a = dest->a * gc->frontBuffer.oneOverAlphaScale;

    tmpc.r = a * dest->r;
    tmpc.g = a * dest->g;
    tmpc.b = a * dest->b;
    tmpc.a = a * dest->a;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceMDA(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b, a;
    __GLfloat mda;

    mda = __glOne - dest->a * gc->frontBuffer.oneOverAlphaScale;

    r = mda * source->r;
    g = mda * source->g;
    b = mda * source->b;
    a = mda * source->a;

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendDestMDA(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat mda;

    mda = __glOne - dest->a * gc->frontBuffer.oneOverAlphaScale;

    tmpc.r = mda * dest->r;
    tmpc.g = mda * dest->g;
    tmpc.b = mda * dest->b;
    tmpc.a = mda * dest->a;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceCC(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat r, g, b, a;

    r = source->r * gc->state.raster.blendColor.r *
	gc->frontBuffer.oneOverRedScale;
    g = source->g * gc->state.raster.blendColor.g *
	gc->frontBuffer.oneOverGreenScale;
    b = source->b * gc->state.raster.blendColor.b *
	gc->frontBuffer.oneOverBlueScale;
    a = source->a * gc->state.raster.blendColor.a *
	gc->frontBuffer.oneOverAlphaScale;

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendDestCC(__GLcontext *gc, const __GLcolor *source,
			  const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;

    tmpc.r = dest->r * gc->state.raster.blendColor.r *
	gc->frontBuffer.oneOverRedScale;
    tmpc.g = dest->g * gc->state.raster.blendColor.g *
	gc->frontBuffer.oneOverGreenScale;
    tmpc.b = dest->b * gc->state.raster.blendColor.b *
	gc->frontBuffer.oneOverBlueScale;
    tmpc.a = dest->a * gc->state.raster.blendColor.a *
	gc->frontBuffer.oneOverAlphaScale;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceMCC(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat r, g, b, a;
    __GLfloat one = __glOne;

    r = source->r * (one - gc->state.raster.blendColor.r *
	gc->frontBuffer.oneOverRedScale);
    g = source->g * (one - gc->state.raster.blendColor.g *
	gc->frontBuffer.oneOverGreenScale);
    b = source->b * (one - gc->state.raster.blendColor.b *
	gc->frontBuffer.oneOverBlueScale);
    a = source->a * (one - gc->state.raster.blendColor.a *
	gc->frontBuffer.oneOverAlphaScale);

    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
}

static void DoBlendDestMCC(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat one = __glOne;

    tmpc.r = dest->r * (one - gc->state.raster.blendColor.r *
	gc->frontBuffer.oneOverRedScale);
    tmpc.g = dest->g * (one - gc->state.raster.blendColor.g *
	gc->frontBuffer.oneOverGreenScale);
    tmpc.b = dest->b * (one - gc->state.raster.blendColor.b *
	gc->frontBuffer.oneOverBlueScale);
    tmpc.a = dest->a * (one - gc->state.raster.blendColor.a *
	gc->frontBuffer.oneOverAlphaScale);

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceCA(__GLcontext *gc, const __GLcolor *source,
			    const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat ca;

    ca = gc->state.raster.blendColor.a * gc->frontBuffer.oneOverAlphaScale;

    result->r = ca * source->r;
    result->b = ca * source->b;
    result->g = ca * source->g;
    result->a = ca * source->a;
}

static void DoBlendDestCA(__GLcontext *gc, const __GLcolor *source,
			  const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat ca;

    ca = gc->state.raster.blendColor.a * gc->frontBuffer.oneOverAlphaScale;

    tmpc.r = ca * dest->r;
    tmpc.b = ca * dest->b;
    tmpc.g = ca * dest->g;
    tmpc.a = ca * dest->a;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceMCA(__GLcontext *gc, const __GLcolor *source,
			     const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat mca;

    mca = __glOne - (gc->state.raster.blendColor.a *
		    gc->frontBuffer.oneOverAlphaScale);

    result->r = mca * source->r;
    result->b = mca * source->b;
    result->g = mca * source->g;
    result->a = mca * source->a;
}

static void DoBlendDestMCA(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    __GLcolor tmpc;
    __GLfloat mca;

    mca = __glOne - (gc->state.raster.blendColor.a *
		    gc->frontBuffer.oneOverAlphaScale);

    tmpc.r = mca * dest->r;
    tmpc.b = mca * dest->b;
    tmpc.g = mca * dest->g;
    tmpc.a = mca * dest->a;

    gc->procs.blendEquation(gc, source, &tmpc, result);
}

static void DoBlendSourceSAT(__GLcontext *gc, const __GLcolor *source,
			     const __GLcolor *dest, __GLcolor *result)
{
    /* Compiler hints */
    __GLfloat r, g, b;
    __GLfloat sa, mda;

    mda = __glOne - dest->a * gc->frontBuffer.oneOverAlphaScale;
    sa = source->a * gc->frontBuffer.oneOverAlphaScale;
    if (sa < mda) {
	r = sa * source->r;
	g = sa * source->g;
	b = sa * source->b;
    } else {
	r = mda * source->r;
	g = mda * source->g;
	b = mda * source->b;
    }
    result->r = r;
    result->g = g;
    result->b = b;
    result->a = source->a;
}

/************************************************************************/

static void Nop(__GLcontext *gc, const __GLcolor *source,
		const __GLcolor *dest, __GLcolor *result)
{
#if defined(__GL_LINT)
    gc = gc;
    source = source;
    dest = dest;
    result = result;
#endif /* __GL_LINT */
}

/*
** Generic blend not handled by cases below
*/
static void NoFetchBlend(__GLcontext *gc, __GLcolorBuffer *cfb,
			 const __GLfragment *frag, __GLcolor *result)
{
#if defined(__GL_LINT)
    cfb = cfb;
#endif /* __GL_LINT */
    (*gc->procs.blendColor)(gc, &(frag->color[__GL_PRIMARY_COLOR]),
			    NULL, result);
}

/*
** Generic blend not handled by cases below
*/
static void FetchBlend(__GLcontext *gc, __GLcolorBuffer *cfb,
		       const __GLfragment *frag, __GLcolor *result)
{
    __GLcolor dest;

    (*cfb->fetch)(cfb, frag->x, frag->y, &dest);

    (*gc->procs.blendColor)(gc, &(frag->color[__GL_PRIMARY_COLOR]),
			    &dest, result);
}

static void DoBlend(__GLcontext *gc, const __GLcolor *source,
		     const __GLcolor *dest, __GLcolor *result)
{
    (*gc->procs.blendSrc)(gc, source, dest, result);
    (*gc->procs.blendDst)(gc, source, dest, result);

    if (result->r > gc->frontBuffer.redScale) {
	result->r = gc->frontBuffer.redScale;
    }
    if (result->g > gc->frontBuffer.greenScale) {
	result->g = gc->frontBuffer.greenScale;
    }
    if (result->b > gc->frontBuffer.blueScale) {
	result->b = gc->frontBuffer.blueScale;
    }
    if (result->a > gc->frontBuffer.alphaScale) {
	result->a = gc->frontBuffer.alphaScale;
    }
}

static void DoBlendNoClamp(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    (*gc->procs.blendSrc)(gc, source, dest, result);
    (*gc->procs.blendDst)(gc, source, dest, result);
}

/*
** Source function == SRC_ALPHA and dest function == ZERO
*/
static void DoBlend_SA_ZERO(__GLcontext *gc, const __GLcolor *source,
			  const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat a;

#if defined(__GL_LINT)
    dest = dest;
#endif /* __GL_LINT */
    a = source->a * gc->frontBuffer.oneOverAlphaScale;
    result->r = source->r * a;
    result->g = source->g * a;
    result->b = source->b * a;
    result->a = source->a * a;
}

/*
** Source function == SRC_ALPHA, dest function == ONE and op == ADD
*/
static void DoBlend_SA_ONE(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat a, ra, rr, rg, rb;

    a = source->a * gc->frontBuffer.oneOverAlphaScale;
    rr = dest->r + source->r * a;
    if (rr > gc->frontBuffer.redScale) {
	rr = gc->frontBuffer.redScale;
    }
    rg = dest->g + source->g * a;
    if (rg > gc->frontBuffer.greenScale) {
	rg = gc->frontBuffer.greenScale;
    }
    rb = dest->b + source->b * a;
    if (rb > gc->frontBuffer.blueScale) {
	rb = gc->frontBuffer.blueScale;
    }
    ra = dest->a + source->a * a;
    if (ra > gc->frontBuffer.alphaScale) {
	ra = gc->frontBuffer.alphaScale;
    }

    result->r = rr;
    result->g = rg;
    result->b = rb;
    result->a = ra;
}

/*
** Source function == SRC_ALPHA, dest function == ONE_MINUS_SRC_ALPHA
** and op == ADD.
** No clamping is done, because the incoming colors should already be
** in range, and the math of x a + y (1 - a) should give a result from
** 0 to 1 if x and y are both from 0 to 1.
*/
static void DoBlend_SA_MSA(__GLcontext *gc, const __GLcolor *source,
			   const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat a, msa, rr, rg, rb, ra;

    a = source->a * gc->frontBuffer.oneOverAlphaScale;
    msa = __glOne - a;
    rr = source->r * a + dest->r * msa;
    rg = source->g * a + dest->g * msa;
    rb = source->b * a + dest->b * msa;
    ra = source->a * a + dest->a * msa;

    result->r = rr;
    result->g = rg;
    result->b = rb;
    result->a = ra;
}

/*
** op == MIN
** No clamping is done, because the incoming colors should already be
** in range, and we're just picking one of them.
*/
static void DoBlend_Min(__GLcontext *gc, const __GLcolor *source,
			const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat a, msa, rr, rg, rb, ra;

    rr = (source->r < dest->r) ? source->r : dest->r;
    rg = (source->g < dest->g) ? source->g : dest->g;
    rb = (source->b < dest->b) ? source->b : dest->b;
    ra = (source->a < dest->a) ? source->a : dest->a;

    result->r = rr;
    result->g = rg;
    result->b = rb;
    result->a = ra;
}

/*
** op == MAX
** No clamping is done, because the incoming colors should already be
** in range, and we're just picking one of them.
*/
static void DoBlend_Max(__GLcontext *gc, const __GLcolor *source,
			const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat a, msa, rr, rg, rb, ra;

    rr = (source->r > dest->r) ? source->r : dest->r;
    rg = (source->g > dest->g) ? source->g : dest->g;
    rb = (source->b > dest->b) ? source->b : dest->b;
    ra = (source->a > dest->a) ? source->a : dest->a;

    result->r = rr;
    result->g = rg;
    result->b = rb;
    result->a = ra;
}

/*
** Source function == ONE_MINUS_SRC_ALPHA, dest function == SRC_ALPHA
** and op == ADD.
** No clamping is done, because the incoming colors should already be
** in range, and the math of x a + y (1 - a) should give a result from
** 0 to 1 if x and y are both from 0 to 1.
*/
static void DoBlend_MSA_SA(__GLcontext *gc, const __GLcolor *source,
			 const __GLcolor *dest, __GLcolor *result)
{
    __GLfloat a, msa, rr, rg, rb, ra;

    a = source->a * gc->frontBuffer.oneOverAlphaScale;
    msa = __glOne - a;
    rr = source->r * msa + dest->r * a;
    rg = source->g * msa + dest->g * a;
    rb = source->b * msa + dest->b * a;
    ra = source->a * msa + dest->a * a;

    result->r = rr;
    result->g = rg;
    result->b = rb;
    result->a = ra;
}

void __glGenericPickBlendProcs(__GLcontext *gc)
{
    GLenum s = gc->state.raster.blendSrc;
    GLenum d = gc->state.raster.blendDst;
    GLenum eq = gc->state.raster.blendMode;

    if (gc->modes.colorIndexMode) {
	return;
    }

    /* Does the blending function need to fetch the dst color?
    ** MIN and MAX always require the fetch (blend factors are unused).
    */
    if (d == GL_ZERO && s != GL_DST_COLOR && s != GL_ONE_MINUS_DST_COLOR &&
	    s != GL_DST_ALPHA && s != GL_ONE_MINUS_DST_ALPHA &&
	    s != GL_SRC_ALPHA_SATURATE &&
	    eq != GL_MIN && eq != GL_MAX) {
	gc->procs.blend = NoFetchBlend;
    } else {
	gc->procs.blend = FetchBlend;
    }
    if (!(gc->state.enables.general & __GL_BLEND_ENABLE)) {
	gc->procs.blendColor = Nop;
    } else {
	/* Look for any fast paths (mostly for ADD blend function, for now) */
	if (eq == GL_FUNC_ADD) {
	    if (s == GL_SRC_ALPHA) {
		if (d == GL_ZERO) {
		    gc->procs.blendColor = DoBlend_SA_ZERO;
		    return;
		}
		if (d == GL_ONE) {
		    gc->procs.blendColor = DoBlend_SA_ONE;
		    return;
		}
		if (d == GL_ONE_MINUS_SRC_ALPHA) {
		    gc->procs.blendColor = DoBlend_SA_MSA;
		    return;
		}
	    } else if (s == GL_ONE_MINUS_SRC_ALPHA) {
		if (d == GL_SRC_ALPHA) {
		    gc->procs.blendColor = DoBlend_MSA_SA;
		    return;
		}
	    }
	} else if (eq == GL_MIN) {
	    gc->procs.blendColor = DoBlend_Min;
	    return;
	} else if (eq == GL_MAX) {
	    gc->procs.blendColor = DoBlend_Max;
	    return;
	}

	/* Use generic blend function */
	if (eq == GL_FUNC_ADD &&
	    ((s == GL_DST_ALPHA && d == GL_ONE_MINUS_DST_ALPHA) ||
	     (s == GL_ONE_MINUS_DST_ALPHA && d == GL_DST_ALPHA) ||
	     (d == GL_ONE_MINUS_SRC_COLOR) ||
	     (s == GL_ONE_MINUS_DST_COLOR) ||
	     (d == GL_ZERO) ||
	     (s == GL_ZERO))) {
	    gc->procs.blendColor = DoBlendNoClamp;
	} else {
	    gc->procs.blendColor = DoBlend;
	}

	switch (s) {
	  case GL_ZERO:
	    gc->procs.blendSrc = DoBlendSourceZERO;
	    break;
	  case GL_ONE:
	    gc->procs.blendSrc = DoBlendSourceONE;
	    break;
	  case GL_DST_COLOR:
	    gc->procs.blendSrc = DoBlendSourceDC;
	    break;
	  case GL_ONE_MINUS_DST_COLOR:
	    gc->procs.blendSrc = DoBlendSourceMDC;
	    break;
	  case GL_SRC_ALPHA:
	    gc->procs.blendSrc = DoBlendSourceSA;
	    break;
	  case GL_ONE_MINUS_SRC_ALPHA:
	    gc->procs.blendSrc = DoBlendSourceMSA;
	    break;
	  case GL_DST_ALPHA:
	    gc->procs.blendSrc = DoBlendSourceDA;
	    break;
	  case GL_ONE_MINUS_DST_ALPHA:
	    gc->procs.blendSrc = DoBlendSourceMDA;
	    break;
	  case GL_CONSTANT_COLOR:
	    gc->procs.blendSrc = DoBlendSourceCC;
	    break;
	  case GL_ONE_MINUS_CONSTANT_COLOR:
	    gc->procs.blendSrc = DoBlendSourceMCC;
	    break;
	  case GL_CONSTANT_ALPHA:
	    gc->procs.blendSrc = DoBlendSourceCA;
	    break;
	  case GL_ONE_MINUS_CONSTANT_ALPHA:
	    gc->procs.blendSrc = DoBlendSourceMCA;
	    break;
	  case GL_SRC_ALPHA_SATURATE:
	    gc->procs.blendSrc = DoBlendSourceSAT;
	    break;
	}
	switch (d) {
	  case GL_ZERO:
	    gc->procs.blendDst = DoBlendDestZERO;
	    break;
	  case GL_ONE:
	    gc->procs.blendDst = DoBlendDestONE;
	    break;
	  case GL_SRC_COLOR:
	    gc->procs.blendDst = DoBlendDestSC;
	    break;
	  case GL_ONE_MINUS_SRC_COLOR:
	    gc->procs.blendDst = DoBlendDestMSC;
	    break;
	  case GL_SRC_ALPHA:
	    gc->procs.blendDst = DoBlendDestSA;
	    break;
	  case GL_ONE_MINUS_SRC_ALPHA:
	    gc->procs.blendDst = DoBlendDestMSA;
	    break;
	  case GL_DST_ALPHA:
	    gc->procs.blendDst = DoBlendDestDA;
	    break;
	  case GL_ONE_MINUS_DST_ALPHA:
	    gc->procs.blendDst = DoBlendDestMDA;
	    break;
	  case GL_CONSTANT_COLOR:
	    gc->procs.blendDst = DoBlendDestCC;
	    break;
	  case GL_ONE_MINUS_CONSTANT_COLOR:
	    gc->procs.blendDst = DoBlendDestMCC;
	    break;
	  case GL_CONSTANT_ALPHA:
	    gc->procs.blendDst = DoBlendDestCA;
	    break;
	  case GL_ONE_MINUS_CONSTANT_ALPHA:
	    gc->procs.blendDst = DoBlendDestMCA;
	    break;
	}
    }

    /*
    ** The blend extension allows an operator to be specified.
    ** The default is Add. MIN and MAX are fast paths handled
    ** above.
    */
    switch (eq) {
      case GL_FUNC_ADD:
	gc->procs.blendEquation = BlendAdd;
	break;
      case GL_FUNC_SUBTRACT:
	gc->procs.blendEquation = BlendSubtract;
	break;
      case GL_FUNC_REVERSE_SUBTRACT:
	gc->procs.blendEquation = BlendReverseSubtract;
	break;
    }
}

/************************************************************************/

GLboolean __glBlendSpan(__GLcontext *gc)
{
    __GLcolor *cp, *fcp, temp;
    GLint w;

    w = gc->polygon.shader.length;

    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    while (--w >= 0) {
	(*gc->procs.blendColor)(gc, cp, fcp, &temp);
	*cp = temp;
	cp++;
	fcp++;
    }

    return GL_FALSE;
}

/*
** Source function == SRC_ALPHA and dest function == ONE_MINUS_SRC_ALPHA
** No clamping is done, because the incoming colors should already be
** in range, and the math of x a + y (1 - a) should give a result from
** 0 to 1 if x and y are both from 0 to 1.
*/
GLboolean __glBlendSpan_SA_MSA(__GLcontext *gc)
{
    __GLfloat a, msa, rr, rg, rb, ra, oneOverAlpha;
    __GLfloat one = __glOne;
    __GLcolor *cp, *fcp;
    GLint w;

    w = gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    oneOverAlpha = gc->frontBuffer.oneOverAlphaScale;
    while (--w >= 0) {
	a = cp->a * oneOverAlpha;
	msa = one - a;

	rr = cp->r * a + fcp->r * msa;
	rg = cp->g * a + fcp->g * msa;
	rb = cp->b * a + fcp->b * msa;
	ra = cp->a * a + fcp->a * msa;

	cp->r = rr;
	cp->g = rg;
	cp->b = rb;
	cp->a = ra;
	cp++;
	fcp++;
    }

    return GL_FALSE;
}

/*
** Source function == ONE_MINUS_SRC_ALPHA and dest function == SRC_ALPHA
** No clamping is done, because the incoming colors should already be
** in range, and the math of x a + y (1 - a) should give a result from
** 0 to 1 if x and y are both from 0 to 1.
*/
GLboolean __glBlendSpan_MSA_SA(__GLcontext *gc)
{
    __GLfloat a, msa, rr, rg, rb, ra, oneOverAlpha;
    __GLcolor *cp, *fcp;
    GLint w;

    w = gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    oneOverAlpha = gc->frontBuffer.oneOverAlphaScale;
    while (--w >= 0) {
	a = cp->a * oneOverAlpha;
	msa = __glOne - a;
	rr = cp->r * msa + fcp->r * a;
	rg = cp->g * msa + fcp->g * a;
	rb = cp->b * msa + fcp->b * a;
	ra = cp->a * msa + fcp->a * a;

	cp->r = rr;
	cp->g = rg;
	cp->b = rb;
	cp->a = ra;
	cp++;
	fcp++;
    }

    return GL_FALSE;
}

GLboolean __glBlendSpan_SA_ZERO(__GLcontext *gc)
{
    __GLfloat a, rr, rg, rb, ra, oneOverAlpha;
    __GLcolor *cp;
    GLint w;

    w = gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    oneOverAlpha = gc->frontBuffer.oneOverAlphaScale;
    while (--w >= 0) {
	a = cp->a * oneOverAlpha;
	rr = cp->r * a;
	rg = cp->g * a;
	rb = cp->b * a;
	ra = cp->a * a;

	cp->r = rr;
	cp->g = rg;
	cp->b = rb;
	cp->a = ra;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glBlendSpan_SA_ONE(__GLcontext *gc)
{
    __GLfloat a, rr, rg, rb, ra;
    __GLfloat rs, gs, bs, as;
    __GLcolor *cp, *fcp;
    GLint w;

    w = gc->polygon.shader.length;
    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    rs = gc->frontBuffer.redScale;
    gs = gc->frontBuffer.greenScale;
    bs = gc->frontBuffer.blueScale;
    as = gc->frontBuffer.alphaScale;
    while (--w >= 0) {
	a = cp->a * gc->frontBuffer.oneOverAlphaScale;
	rr = fcp->r + cp->r * a;
	rg = fcp->g + cp->g * a;
	if (rr > rs) {
	    rr = rs;
	}
	rb = fcp->b + cp->b * a;
	if (rg > gs) {
	    rg = gs;
	}
	ra = fcp->a + cp->a * a;
	if (rb > bs) {
	    rb = bs;
	}

	cp->r = rr;
	if (ra > as) {
	    ra = as;
	}
	cp->g = rg;
	cp->b = rb;
	cp->a = ra;
	cp++;
	fcp++;
    }

    return GL_FALSE;
}

GLboolean __glBlendStippledSpan(__GLcontext *gc)
{
    __GLstippleWord *sp, bit, inMask;
    __GLcolor *cp, *fcp, temp;
    GLint count;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    cp = gc->polygon.shader.colors;
    fcp = gc->polygon.shader.fbcolors;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		(*gc->procs.blendColor)(gc, cp, fcp, &temp);
		*cp = temp;
	    }
	    cp++;
	    fcp++;
#if defined(__GL_STIPPLE_MSB)
	    bit >>= 1;
#else  /* __GL_STIPPLE_MSB */
	    bit <<= 1;
#endif /* __GL_STIPPLE_MSB */
	}
    }

    return GL_FALSE;
}
