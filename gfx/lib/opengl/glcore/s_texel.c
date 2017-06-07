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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_texel.c#34 $
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

/*
** Some math routines that are optimized in assembly
*/
#if defined(__GL_USE_ASMCODE) && defined(mips)
#define __GL_FRAC(f)	__glFrac(f)
#else /* __GL_USE_ASMCODE && mips */
#define __GL_FRAC(f)	((f) - __GL_FLOORF(f))
#endif /* __GL_USE_ASMCODE && mips */


/************************************************************************/

/*
** Return texel nearest the s coordinate.  s is converted to u
** implicitly during this step.
*/
/* ARGSUSED */
void __glNearestFilter1(__GLtexture *tex, __GLmipMapLevel *lp,
			__GLfloat s, __GLfloat t, __GLfloat r,
			__GLtexel *result)
{
    GLint col = (GLint) s;
    GLint w2 = lp->width2;

    /* Find texel index */
    if (tex->state.params.sWrapMode == GL_REPEAT) {
	col &= w2-1;
    } else {
	if (col < 0) col = 0;
	else if (col >= w2) col = w2 - 1;
    }

    /* Lookup texel */
    (*lp->extract)(lp, tex, 0, 0, col, result);
}

/*
** Return texel nearest the s,t coordinates.  s,t are converted to u,v
** implicitly during this step.
*/
/* ARGSUSED */
void __glNearestFilter2(__GLtexture *tex, __GLmipMapLevel *lp,
			__GLfloat s, __GLfloat t, __GLfloat r,
			__GLtexel *result)
{
    GLint row = (GLint) t;
    GLint col = (GLint) s;
    GLint w2 = lp->width2;
    GLint h2 = lp->height2;

    /* Find texel column address */
    if (tex->state.params.sWrapMode == GL_REPEAT) {
	col &= w2-1;
    } else {
	if (col < 0) col = 0;
	else if (col >= w2) col = w2 - 1;
    }

    /* Find texel row address */
    if (tex->state.params.tWrapMode == GL_REPEAT) {
	row &= h2-1;
    } else {
	if (row < 0) row = 0;
	else if (row >= h2) row = h2 - 1;
    }

    /* Lookup texel */
    (*lp->extract)(lp, tex, 0, row, col, result);
}

/*
** Return texel nearest the s,t,r coordinates.  s,t,r are converted to u,v,w
** implicitly during this step.
*/
void __glNearestFilter3(__GLtexture *tex, __GLmipMapLevel *lp,
			__GLfloat s, __GLfloat t, __GLfloat r,
			__GLtexel *result)
{
    GLint row = (GLint) t;
    GLint col = (GLint) s;
    GLint img = (GLint) r;
    GLint w2 = lp->width2;
    GLint h2 = lp->height2;
    GLint d2 = lp->depth2;

    /* Find texel column address */
    if (tex->state.params.sWrapMode == GL_REPEAT) {
	col &= w2-1;
    } else {
	if (col < 0) col = 0;
	else if (col >= w2) col = w2 - 1;
    }

    /* Find texel row address */
    if (tex->state.params.tWrapMode == GL_REPEAT) {
	row &= h2-1;
    } else {
	if (row < 0) row = 0;
	else if (row >= h2) row = h2 - 1;
    }

    /* Find texel image address */
    if (tex->state.params.rWrapMode == GL_REPEAT) {
	img &= d2-1;
    } else {
	if (img < 0) img = 0;
	else if (img >= d2) img = d2 - 1;
    }

    /* Lookup texel */
    (*lp->extract)(lp, tex, img, row, col, result);
}

/*
** Return texel which is a linear combination of texels near s.
*/
/* ARGSUSED */
void __glLinearFilter1(__GLtexture *tex, __GLmipMapLevel *lp,
		       __GLfloat s, __GLfloat t, __GLfloat r,
		       __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    __GLfloat u, alpha, omalpha;
    GLint col0, col1;
    __GLtexel t0, t1;

    /* Find col0 and col1 */
    u = s;
    if (tex->state.params.sWrapMode == GL_REPEAT) {
	u -= __glHalf;
	col0 = ((GLint) __glFloor(u)) & (lp->width2 - 1);
	col1 = (col0 + 1) & (lp->width2 - 1);
    } else {
	if (u < __glZero) u = __glZero;
	else if (u > lp->width2) u = lp->width2;
	u -= __glHalf;
	col0 = (GLint) __glFloor(u);
	col1 = col0 + 1;
	if (tex->state.params.sWrapMode == GL_CLAMP_TO_EDGE) {
	   if (col0 < __glZero) col0 = __glZero;
	   if (col1 >= lp->width2) col1 = lp->width2 - 1;
	}
    }

    /* Compute alpha and beta */
    alpha = __GL_FRAC(u);

    /* Calculate the final texel value as a combination of the two texels */
    (*lp->extract)(lp, tex, 0, 0, col0, &t0);
    (*lp->extract)(lp, tex, 0, 0, col1, &t1);

    omalpha = __glOne - alpha;

    switch (tex->baseFormat) {
      case GL_LUMINANCE_ALPHA:
	result->a = omalpha * t0.a + alpha * t1.a;
	/* FALLTHROUGH */
      case GL_LUMINANCE:
	result->r = omalpha * t0.r + alpha * t1.r;
	break;
      case GL_RGBA:
	result->a = omalpha * t0.a + alpha * t1.a;
	/* FALLTHROUGH */
      case GL_RGB:
	result->r = omalpha * t0.r + alpha * t1.r;
	result->g = omalpha * t0.g + alpha * t1.g;
	result->b = omalpha * t0.b + alpha * t1.b;
	break;
      case GL_ALPHA:
	result->a = omalpha * t0.a + alpha * t1.a;
	break;
      case GL_INTENSITY:
	result->r = omalpha * t0.r + alpha * t1.r;
	break;
    }
}

/*
** Return texel which is a linear combination of texels near s,t.
*/
/* ARGSUSED */
void __glLinearFilter2(__GLtexture *tex, __GLmipMapLevel *lp,
		       __GLfloat s, __GLfloat t, __GLfloat r,
		       __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    __GLfloat u, v, alpha, beta, half;
    GLint col0, row0, col1, row1, w2f, h2f;
    __GLtexel t00, t01, t10, t11;
    __GLfloat omalpha, ombeta, m00, m01, m10, m11;

    /* Find col0, col1 */
    w2f = (GLint)lp->width2f;
    u = s;
    half = __glHalf;
    if (tex->state.params.sWrapMode == GL_REPEAT) {
	GLint w2mask = lp->width2 - 1;
	u -= half;
	col0 = ((GLint) __glFloor(u)) & w2mask;
	col1 = (col0 + 1) & w2mask;
    } else {
	if (u < __glZero) u = __glZero;
	else if (u > w2f) u = w2f;
	u -= half;
	col0 = (GLint) __glFloor(u);
	col1 = col0 + 1;
	if (tex->state.params.sWrapMode == GL_CLAMP_TO_EDGE) {
	   if (col0 < __glZero) col0 = __glZero;
	   if (col1 >= lp->width2) col1 = lp->width2 - 1;
	}
    }

    /* Find row0, row1 */
    h2f = (GLint)lp->height2f;
    v = t;
    if (tex->state.params.tWrapMode == GL_REPEAT) {
	GLint h2mask = lp->height2 - 1;
	v -= half;
	row0 = ((GLint) __glFloor(v)) & h2mask;
	row1 = (row0 + 1) & h2mask;
    } else {
	if (v < __glZero) v = __glZero;
	else if (v > h2f) v = h2f;
	v -= half;
	row0 = (GLint) __glFloor(v);
	row1 = row0 + 1;
	if (tex->state.params.tWrapMode == GL_CLAMP_TO_EDGE) {
	   if (row0 < __glZero) row0 = __glZero;
	   if (row1 >= lp->height2) row1 = lp->height2 - 1;
	}
    }

    /* Compute alpha and beta */
    alpha = __GL_FRAC(u);
    beta = __GL_FRAC(v);

    /* Calculate the final texel value as a combination of the square chosen */
    (*lp->extract)(lp, tex, 0, row0, col0, &t00);
    (*lp->extract)(lp, tex, 0, row0, col1, &t10);
    (*lp->extract)(lp, tex, 0, row1, col0, &t01);
    (*lp->extract)(lp, tex, 0, row1, col1, &t11);

    omalpha = __glOne - alpha;
    ombeta = __glOne - beta;

    m00 = omalpha * ombeta;
    m10 = alpha * ombeta;
    m01 = omalpha * beta;
    m11 = alpha * beta;

    switch (tex->baseFormat) {
      case GL_LUMINANCE_ALPHA:
	result->a = m00*t00.a + m10*t10.a + m01*t01.a
	    + m11*t11.a;
	/* FALLTHROUGH */
      case GL_LUMINANCE:
	result->r = m00*t00.r + m10*t10.r
	    + m01*t01.r + m11*t11.r;
	break;
      case GL_RGBA:
	result->a = m00*t00.a + m10*t10.a + m01*t01.a
	    + m11*t11.a;
	/* FALLTHROUGH */
      case GL_RGB:
	result->r = m00*t00.r + m10*t10.r + m01*t01.r + m11*t11.r;
	result->g = m00*t00.g + m10*t10.g + m01*t01.g + m11*t11.g;
	result->b = m00*t00.b + m10*t10.b + m01*t01.b + m11*t11.b;
	break;
      case GL_ALPHA:
	result->a = m00*t00.a + m10*t10.a + m01*t01.a
	    + m11*t11.a;
	break;
      case GL_INTENSITY:
	result->r = m00*t00.r + m10*t10.r
	    + m01*t01.r + m11*t11.r;
	break;
    }
}

/*
** Return texel which is a linear combination of texels near s,t,r.
*/
void __glLinearFilter3(__GLtexture *tex, __GLmipMapLevel *lp,
		       __GLfloat s, __GLfloat t, __GLfloat r,
		       __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    __GLfloat u, v, w, alpha, beta, theta, half;
    GLint col0, row0, img0, col1, row1, img1, w2f, h2f, d2f;
    __GLtexel t000, t010, t100, t110, t001, t011, t101, t111;
    __GLfloat omalpha, ombeta, omtheta;
    __GLfloat m000, m010, m100, m110, m001, m011, m101, m111;

    /* Find col0, col1 */
    w2f = (GLint)lp->width2f;
    u = s;
    half = __glHalf;
    if (tex->state.params.sWrapMode == GL_REPEAT) {
	GLint w2mask = lp->width2 - 1;
	u -= half;
	col0 = ((GLint) __glFloor(u)) & w2mask;
	col1 = (col0 + 1) & w2mask;
    } else {
	if (u < __glZero) u = __glZero;
	else if (u > w2f) u = w2f;
	u -= half;
	col0 = (GLint) __glFloor(u);
	col1 = col0 + 1;
	if (tex->state.params.sWrapMode == GL_CLAMP_TO_EDGE) {
	   if (col0 < __glZero) col0 = __glZero;
	   if (col1 >= lp->width2) col1 = lp->width2 - 1;
	}
    }

    /* Find row0, row1 */
    h2f = (GLint)lp->height2f;
    v = t;
    if (tex->state.params.tWrapMode == GL_REPEAT) {
	GLint h2mask = lp->height2 - 1;
	v -= half;
	row0 = ((GLint) __glFloor(v)) & h2mask;
	row1 = (row0 + 1) & h2mask;
    } else {
	if (v < __glZero) v = __glZero;
	else if (v > h2f) v = h2f;
	v -= half;
	row0 = (GLint) __glFloor(v);
	row1 = row0 + 1;
	if (tex->state.params.tWrapMode == GL_CLAMP_TO_EDGE) {
	   if (row0 < __glZero) row0 = __glZero;
	   if (row1 >= lp->height2) row1 = lp->height2 - 1;
	}
    }

    /* Find img0, img1 */
    d2f = (GLint)lp->depth2f;
    w = r;
    if (tex->state.params.rWrapMode == GL_REPEAT) {
	GLint d2mask = lp->depth2 - 1;
	w -= half;
	img0 = ((GLint) __glFloor(w)) & d2mask;
	img1 = (img0 + 1) & d2mask;
    } else {
	if (w < __glZero) w = __glZero;
	else if (w > d2f) w = d2f;
	w -= half;
	img0 = (GLint) __glFloor(w);
	img1 = img0 + 1;
	if (tex->state.params.rWrapMode == GL_CLAMP_TO_EDGE) {
	   if (img0 < __glZero) img0 = __glZero;
	   if (img1 >= lp->depth2) img1 = lp->depth2 - 1;
	}
    }

    /* Compute alpha and beta */
    alpha = __GL_FRAC(u);
    beta = __GL_FRAC(v);
    theta = __GL_FRAC(w);

    /* Calculate the final texel value as a combination of a 2x2 cube */
    (*lp->extract)(lp, tex, img0, row0, col0, &t000);
    (*lp->extract)(lp, tex, img0, row0, col1, &t100);
    (*lp->extract)(lp, tex, img0, row1, col0, &t010);
    (*lp->extract)(lp, tex, img0, row1, col1, &t110);

    (*lp->extract)(lp, tex, img1, row0, col0, &t001);
    (*lp->extract)(lp, tex, img1, row0, col1, &t101);
    (*lp->extract)(lp, tex, img1, row1, col0, &t011);
    (*lp->extract)(lp, tex, img1, row1, col1, &t111);

    omalpha = __glOne - alpha;
    ombeta = __glOne - beta;
    omtheta = __glOne - theta;

    m000 = omalpha * ombeta * omtheta;
    m100 = alpha * ombeta * omtheta;
    m010 = omalpha * beta * omtheta;
    m110 = alpha * beta * omtheta;

    m001 = omalpha * ombeta * theta;
    m101 = alpha * ombeta * theta;
    m011 = omalpha * beta * theta;
    m111 = alpha * beta * theta;

    switch (tex->baseFormat) {
      case GL_LUMINANCE_ALPHA:
	result->a = m000*t000.a + m100*t100.a +
		    m010*t010.a + m110*t110.a +
		    m001*t001.a + m101*t101.a +
		    m011*t011.a + m111*t111.a;
	/* FALLTHROUGH */
      case GL_LUMINANCE:
	result->r = m000*t000.r + m100*t100.r +
		    m010*t010.r + m110*t110.r +
		    m001*t001.r + m101*t101.r +
		    m011*t011.r + m111*t111.r;
	break;
      case GL_RGBA:
	result->a = m000*t000.a + m100*t100.a +
		    m010*t010.a + m110*t110.a +
		    m001*t001.a + m101*t101.a +
		    m011*t011.a + m111*t111.a;
	/* FALLTHROUGH */
      case GL_RGB:
	result->r = m000*t000.r + m100*t100.r + m010*t010.r + m110*t110.r +
		    m001*t001.r + m101*t101.r + m011*t011.r + m111*t111.r;
	result->g = m000*t000.g + m100*t100.g + m010*t010.g + m110*t110.g +
		    m001*t001.g + m101*t101.g + m011*t011.g + m111*t111.g;
	result->b = m000*t000.b + m100*t100.b + m010*t010.b + m110*t110.b +
		    m001*t001.b + m101*t101.b + m011*t011.b + m111*t111.b;
	break;
      case GL_ALPHA:
	result->a = m000*t000.a + m100*t100.a +
		    m010*t010.a + m110*t110.a +
		    m001*t001.a + m101*t101.a +
		    m011*t011.a + m111*t111.a;
	break;
    }
}

/*
** Linear min/mag filter
*/
void __glLinearFilterUVScaled(__GLtexture *tex, __GLfloat lod,
		      __GLfloat s, __GLfloat t, __GLfloat r,
		      __GLtexel *result)
{
#if defined(__GL_LINT)
    lod = lod;
#endif
    (*tex->linear)(tex, tex->level[tex->state.params.baseLevel], s, t, r, result);
}

/*
** Nearest min/mag filter
*/
void __glNearestFilterUVScaled(__GLtexture *tex, __GLfloat lod,
		       __GLfloat s, __GLfloat t, __GLfloat r,
		       __GLtexel *result)
{
#if defined(__GL_LINT)
    lod = lod;
#endif
    (*tex->nearest)(tex, tex->level[tex->state.params.baseLevel], s, t, r, result);
}

/*
** Linear min/mag filter
*/
void __glLinearFilter(__GLtexture *tex, __GLfloat lod,
		      __GLfloat s, __GLfloat t, __GLfloat r,
		      __GLtexel *result)
{
#if defined(__GL_LINT)
    lod = lod;
#endif
    __GLmipMapLevel *lp = tex->level[tex->state.params.baseLevel];
    s *= lp->width2f;
    t *= lp->height2f;
    r *= lp->depth2f;
    (*tex->linear)(tex, lp, s, t, r, result);
}

/*
** Nearest min/mag filter
*/
void __glNearestFilter(__GLtexture *tex, __GLfloat lod,
		       __GLfloat s, __GLfloat t, __GLfloat r,
		       __GLtexel *result)
{
#if defined(__GL_LINT)
    lod = lod;
#endif
    __GLmipMapLevel *lp = tex->level[tex->state.params.baseLevel];
    s *= lp->width2f;
    t *= lp->height2f;
    r *= lp->depth2f;
    (*tex->nearest)(tex, lp, s, t, r, result);
}

/*
** Apply minification rules to find the texel value.
*/
void __glNMNFilter(__GLtexture *tex, __GLfloat lod,
		   __GLfloat s, __GLfloat t, __GLfloat r, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    __GLmipMapLevel *lp;
    GLint level;

    if (lod <= __glHalf) {
	level = tex->state.params.baseLevel;
    } else {
	lod += tex->state.params.baseLevel;
	if (lod > tex->effectiveMaxLOD + __glHalf) {
	    level = (GLint)tex->effectiveMaxLOD;
	} else {
	    level = (GLint)__GL_CEILF(lod + __glHalf) - 1;
	}
    }
    lp = tex->level[level];
    s *= lp->width2f;
    t *= lp->height2f;
    r *= lp->depth2f;
    (*tex->nearest)(tex, lp, s, t, r, result);
}

/*
** Apply minification rules to find the texel value.
*/
void __glLMNFilter(__GLtexture *tex, __GLfloat lod,
		   __GLfloat s, __GLfloat t, __GLfloat r, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    __GLmipMapLevel *lp;
    GLint level;

    if (lod <= __glHalf) {
	level = tex->state.params.baseLevel;
    } else {
	lod += tex->state.params.baseLevel;
	if (lod > tex->effectiveMaxLOD + __glHalf) {
	    level = (GLint)tex->effectiveMaxLOD;
	} else {
	    level = (GLint)__GL_CEILF(lod + __glHalf) - 1;
	}
    }
    lp = tex->level[level];
    s *= lp->width2f;
    t *= lp->height2f;
    r *= lp->depth2f;
    (*tex->linear)(tex, lp, s, t, r, result);
}

/*
** Apply minification rules to find the texel value.
*/
void __glNMLFilter(__GLtexture *tex, __GLfloat lod,
		   __GLfloat s, __GLfloat t, __GLfloat r, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    __GLmipMapLevel *lp;
    GLint level;
    __GLtexel td, td1;
    __GLfloat f, omf;

    lod += tex->state.params.baseLevel;
    if (lod >= tex->effectiveMaxLOD) {
	lp = tex->level[(GLint)tex->effectiveMaxLOD];
	s *= lp->width2f;
	t *= lp->height2f;
	r *= lp->depth2f;
	(*tex->nearest)(tex, lp, s, t, r, result);
    } else {
	__GLfloat s1, t1, r1;

	level = (GLint)__GL_FLOORF(lod);

	lp = tex->level[level];
	s1 = s * lp->width2f;
	t1 = t * lp->height2f;
	r1 = r * lp->depth2f;
	(*tex->nearest)(tex, lp, s1, t1, r1, &td);

	lp = tex->level[level+1];
	s1 = s * lp->width2f;
	t1 = t * lp->height2f;
	r1 = r * lp->depth2f;
	(*tex->nearest)(tex, lp, s1, t1, r1, &td1);

	f = __GL_FRAC(lod);
	omf = __glOne - f;
	switch (tex->baseFormat) {
	  case GL_LUMINANCE_ALPHA:
	    result->a = omf * td1.a + f * td.a;
	    /* FALLTHROUGH */
	  case GL_LUMINANCE:
	    result->r = omf * td1.r + f * td.r;
	    break;
	  case GL_RGBA:
	    result->a = omf * td1.a + f * td.a;
	    /* FALLTHROUGH */
	  case GL_RGB:
	    result->r = omf * td1.r + f * td.r;
	    result->g = omf * td1.g + f * td.g;
	    result->b = omf * td1.b + f * td.b;
	    break;
	  case GL_ALPHA:
	    result->a = omf * td1.a + f * td.a;
	    break;
	  case GL_INTENSITY:
	    result->r = omf * td1.r + f * td.r;
	    break;
	}
    }
}

/*
** Apply minification rules to find the texel value.
*/
void __glLMLFilter(__GLtexture *tex, __GLfloat lod,
		   __GLfloat s, __GLfloat t,  __GLfloat r, __GLtexel *result)
{
    __GLcontext *gc = tex->gc;
    __GLmipMapLevel *lp;
    GLint level;
    __GLtexel td, td1;
    __GLfloat f, omf;

    lod += tex->state.params.baseLevel;
    if (lod >= tex->effectiveMaxLOD) {
	lp = tex->level[(GLint)tex->effectiveMaxLOD];
	s *= lp->width2f;
	t *= lp->height2f;
	r *= lp->depth2f;
	(*tex->linear)(tex, lp, s, t, r, result);
    } else {
	__GLfloat s1, t1, r1;

	level = (GLint)__GL_FLOORF(lod);

	lp = tex->level[level];
	s1 = s * lp->width2f;
	t1 = t * lp->height2f;
	r1 = r * lp->depth2f;
	(*tex->linear)(tex, lp, s1, t1, r1, &td);

	lp = tex->level[level+1];
	s1 = s * lp->width2f;
	t1 = t * lp->height2f;
	r1 = r * lp->depth2f;
	(*tex->linear)(tex, lp, s1, t1, r1, &td1);

	f = __GL_FRAC(lod);
	omf = __glOne - f;
	switch (tex->baseFormat) {
	  case GL_LUMINANCE_ALPHA:
	    result->a = omf * td1.a + f * td.a;
	    /* FALLTHROUGH */
	  case GL_LUMINANCE:
	    result->r = omf * td1.r + f * td.r;
	    break;
	  case GL_RGBA:
	    result->a = omf * td1.a + f * td.a;
	    /* FALLTHROUGH */
	  case GL_RGB:
	    result->r = omf * td1.r + f * td.r;
	    result->g = omf * td1.g + f * td.g;
	    result->b = omf * td1.b + f * td.b;
	    break;
	  case GL_ALPHA:
	    result->a = omf * td1.a + f * td.a;
	    break;
	  case GL_INTENSITY:
	    result->r = omf * td1.r + f * td.r;
	    break;
	}
    }
}

/***********************************************************************/

/* 1 Component modulate */
void __glTextureModulateL(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat luminance = __GL_UB_TO_FLOAT(texel->r);

    color->r = luminance * color->r;
    color->g = luminance * color->g;
    color->b = luminance * color->b;
}

/* 2 Component modulate */
void __glTextureModulateLA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat luminance = __GL_UB_TO_FLOAT(texel->r);
    __GLfloat alpha = __GL_UB_TO_FLOAT(texel->a);

    color->r = luminance * color->r;
    color->g = luminance * color->g;
    color->b = luminance * color->b;
    color->a = alpha * color->a;
}

/* 3 Component modulate */
void __glTextureModulateRGB(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->r = __GL_UB_TO_FLOAT(texel->r) * color->r;
    color->g = __GL_UB_TO_FLOAT(texel->g) * color->g;
    color->b = __GL_UB_TO_FLOAT(texel->b) * color->b;
}

/* 4 Component modulate */
void __glTextureModulateRGBA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->r = __GL_UB_TO_FLOAT(texel->r) * color->r;
    color->g = __GL_UB_TO_FLOAT(texel->g) * color->g;
    color->b = __GL_UB_TO_FLOAT(texel->b) * color->b;
    color->a = __GL_UB_TO_FLOAT(texel->a) * color->a;
}

/* Alpha modulate */
void __glTextureModulateA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->a = __GL_UB_TO_FLOAT(texel->a) * color->a;
}

/* Intensity modulate */
void __glTextureModulateI(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat intensity = __GL_UB_TO_FLOAT(texel->r);

    color->r = intensity * color->r;
    color->g = intensity * color->g;
    color->b = intensity * color->b;
    color->a = intensity * color->a;
}

/* Color Index modulate */
void __glTextureModulateCI(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat index = __GL_UB_TO_FLOAT(texel->r);

    color->r = index * color->r;
}

/***********************************************************************/

/* 3 Component decal */
void __glTextureDecalRGB(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->r = __GL_UB_TO_FLOAT(texel->r) * gc->frontBuffer.redScale;
    color->g = __GL_UB_TO_FLOAT(texel->g) * gc->frontBuffer.greenScale;
    color->b = __GL_UB_TO_FLOAT(texel->b) * gc->frontBuffer.blueScale;
}

/* 4 Component decal */
void __glTextureDecalRGBA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat a = __GL_UB_TO_FLOAT(texel->a);
    __GLfloat oma = __glOne - a;

    color->r = oma * color->r
	+ a * __GL_UB_TO_FLOAT(texel->r) * gc->frontBuffer.redScale;
    color->g = oma * color->g
	+ a * __GL_UB_TO_FLOAT(texel->g) * gc->frontBuffer.greenScale;
    color->b = oma * color->b
	+ a * __GL_UB_TO_FLOAT(texel->b) * gc->frontBuffer.blueScale;
}

/***********************************************************************/

/* 1 Component blend */
void __glTextureBlendL(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat l = __GL_UB_TO_FLOAT(texel->r);
    __GLfloat oml = __glOne - l;
    __GLcolor *cc = &gc->state.texture.active->env[0].color;

    color->r = oml * color->r + l * cc->r;
    color->g = oml * color->g + l * cc->g;
    color->b = oml * color->b + l * cc->b;
}

/* 2 Component blend */
void __glTextureBlendLA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat l = __GL_UB_TO_FLOAT(texel->r);
    __GLfloat oml = __glOne - l;
    __GLcolor *cc = &gc->state.texture.active->env[0].color;

    color->r = oml * color->r + l * cc->r;
    color->g = oml * color->g + l * cc->g;
    color->b = oml * color->b + l * cc->b;
    color->a = __GL_UB_TO_FLOAT(texel->a) * color->a;
}

/* 3 Component blend */
void __glTextureBlendRGB(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat r = __GL_UB_TO_FLOAT(texel->r);
    __GLfloat g = __GL_UB_TO_FLOAT(texel->g);
    __GLfloat b = __GL_UB_TO_FLOAT(texel->b);
    __GLcolor *cc = &gc->state.texture.active->env[0].color;

    color->r = (__glOne - r) * color->r + r * cc->r;
    color->g = (__glOne - g) * color->g + g * cc->g;
    color->b = (__glOne - b) * color->b + b * cc->b;
}

/* 4 Component blend */
void __glTextureBlendRGBA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat r = __GL_UB_TO_FLOAT(texel->r);
    __GLfloat g = __GL_UB_TO_FLOAT(texel->g);
    __GLfloat b = __GL_UB_TO_FLOAT(texel->b);
    __GLcolor *cc = &gc->state.texture.active->env[0].color;

    color->r = (__glOne - r) * color->r + r * cc->r;
    color->g = (__glOne - g) * color->g + g * cc->g;
    color->b = (__glOne - b) * color->b + b * cc->b;
    color->a = __GL_UB_TO_FLOAT(texel->a) * color->a;
}

/* Alpha blend */
void __glTextureBlendA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->a = __GL_UB_TO_FLOAT(texel->a) * color->a;
}

/* Intensity blend */
void __glTextureBlendI(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat i = __GL_UB_TO_FLOAT(texel->r);
    __GLfloat omi = __glOne - i;
    __GLcolor *cc = &gc->state.texture.active->env[0].color;

    color->r = omi * color->r + i * cc->r;
    color->g = omi * color->g + i * cc->g;
    color->b = omi * color->b + i * cc->b;
    color->a = omi * color->a + i * cc->a;
}

/***********************************************************************/

/* 1 Component replace */
void __glTextureReplaceL(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat luminance = __GL_UB_TO_FLOAT(texel->r);

    color->r = luminance * gc->frontBuffer.redScale;
    color->g = luminance * gc->frontBuffer.greenScale;
    color->b = luminance * gc->frontBuffer.blueScale;
}

/* 2 Component replace */
void __glTextureReplaceLA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat luminance = __GL_UB_TO_FLOAT(texel->r);
    __GLfloat alpha = __GL_UB_TO_FLOAT(texel->a);

    color->r = luminance * gc->frontBuffer.redScale;
    color->g = luminance * gc->frontBuffer.greenScale;
    color->b = luminance * gc->frontBuffer.blueScale;
    color->a = alpha * gc->frontBuffer.alphaScale;
}

/* 3 Component replace */
void __glTextureReplaceRGB(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->r = __GL_UB_TO_FLOAT(texel->r) * gc->frontBuffer.redScale;
    color->g = __GL_UB_TO_FLOAT(texel->g) * gc->frontBuffer.greenScale;
    color->b = __GL_UB_TO_FLOAT(texel->b) * gc->frontBuffer.blueScale;
}

/* 4 Component replace */
void __glTextureReplaceRGBA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->r = __GL_UB_TO_FLOAT(texel->r) * gc->frontBuffer.redScale;
    color->g = __GL_UB_TO_FLOAT(texel->g) * gc->frontBuffer.greenScale;
    color->b = __GL_UB_TO_FLOAT(texel->b) * gc->frontBuffer.blueScale;
    color->a = __GL_UB_TO_FLOAT(texel->a) * gc->frontBuffer.alphaScale;
}

/* Alpha replace */
void __glTextureReplaceA(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->a = __GL_UB_TO_FLOAT(texel->a) * gc->frontBuffer.alphaScale;
}

/* Intensity replace */
void __glTextureReplaceI(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    __GLfloat intensity = __GL_UB_TO_FLOAT(texel->r);

    color->r = intensity * gc->frontBuffer.redScale;
    color->g = intensity * gc->frontBuffer.greenScale;
    color->b = intensity * gc->frontBuffer.blueScale;
    color->a = intensity * gc->frontBuffer.alphaScale;
}

/* Color Index replace */
void __glTextureReplaceCI(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->r = __GL_UB_TO_FLOAT(texel->r);
}

/***********************************************************************/

/* Color Index add */
void __glTextureAddCI(__GLcontext *gc, __GLcolor *color, __GLtexel *texel)
{
    color->r = __GL_UB_TO_FLOAT(texel->r) + color->r;
}

/***********************************************************************/

/* ARGSUSED */
__GLfloat __glNopPolygonRho(__GLcontext *gc, GLint unit,
			    const __GLshade *sh,
			    __GLfloat s, __GLfloat t, __GLfloat r,
			    __GLfloat winv)
{
    return __glZero;
}

/*
** Compute the "rho" (level of detail) parameter used by the texturing code.
** Instead of fully computing the derivatives compute nearby texture coordinates
** and discover the derivative.  The incoming s & t arguments have not
** been divided by winv yet.
*/
__GLfloat __glComputePolygonRho(__GLcontext *gc, GLint unit,
				const __GLshade *sh,
				__GLfloat s, __GLfloat t, __GLfloat r,
				__GLfloat qw)
{
    const GLuint modeFlags = gc->polygon.shader.modeFlags;
    const __GLtexture *tex = gc->texture.unit[unit].currentTexture;
    __GLfloat qw0, qw1, p0, p1;
    __GLfloat pupx, pupy, pvpx, pvpy, pwpx, pwpy;
    __GLfloat px, py;

    /* Compute partial of u with respect to x */
    qw0 = (qw - sh->texture[unit].dqwdx) ?
    		__glOne / (qw - sh->texture[unit].dqwdx) : __glZero;
    qw1 = (qw + sh->texture[unit].dqwdx) ?
    		__glOne / (qw + sh->texture[unit].dqwdx) : __glZero;
    p0 = (s - sh->texture[unit].dsdx) * qw0;
    p1 = (s + sh->texture[unit].dsdx) * qw1;
    pupx = p1 - p0;
    if (!(modeFlags & __GL_SHADE_UVSCALED_TEXTURE)) {
	pupx *= tex->level[0]->width2f;
    }

    /* Compute partial of v with respect to x */
    p0 = (t - sh->texture[unit].dtdx) * qw0;
    p1 = (t + sh->texture[unit].dtdx) * qw1;
    pvpx = p1 - p0;
    if (!(modeFlags & __GL_SHADE_UVSCALED_TEXTURE)) {
	pvpx *= tex->level[0]->height2f;
    }

    if (tex->dim > 2) {
        /* Compute partial of w with respect to x */
        p0 = (r - sh->texture[unit].drdx) * qw0;
        p1 = (r + sh->texture[unit].drdx) * qw1;
        pwpx = p1 - p0;
	if (!(modeFlags & __GL_SHADE_UVSCALED_TEXTURE)) {
	    pwpx *= tex->level[0]->depth2f;
	}
    }

    /* Compute partial of u with respect to y */
    qw0 = (qw - sh->texture[unit].dqwdy) ?
    		__glOne / (qw - sh->texture[unit].dqwdy) : __glZero;
    qw1 = (qw + sh->texture[unit].dqwdy) ?
    		__glOne / (qw + sh->texture[unit].dqwdy) : __glZero;
    p0 = (s - sh->texture[unit].dsdy) * qw0;
    p1 = (s + sh->texture[unit].dsdy) * qw1;
    pupy = p1 - p0;
    if (!(modeFlags & __GL_SHADE_UVSCALED_TEXTURE)) {
	pupy *= tex->level[0]->width2f;
    }

    /* Compute partial of v with respect to y */
    p0 = (t - sh->texture[unit].dtdy) * qw0;
    p1 = (t + sh->texture[unit].dtdy) * qw1;
    pvpy = p1 - p0;
    if (!(modeFlags & __GL_SHADE_UVSCALED_TEXTURE)) {
	pvpy *= tex->level[0]->height2f;
    }

    if (tex->dim > 2) {
        /* Compute partial of w with respect to y */
        p0 = (r - sh->texture[unit].drdy) * qw0;
        p1 = (r + sh->texture[unit].drdy) * qw1;
	pwpy = p1 - p0;
	if (!(modeFlags & __GL_SHADE_UVSCALED_TEXTURE)) {
	    pwpy *= tex->level[0]->depth2f;
	}

        /* Finally, figure sum of squares */
        px = pupx * pupx + pvpx * pvpx + pwpx * pwpx;
        py = pupy * pupy + pvpy * pvpy + pwpy * pwpy;
    } else {
        /* Finally, figure sum of squares */
        px = pupx * pupx + pvpx * pvpx;
        py = pupy * pupy + pvpy * pvpy;
    }

    /* Return largest value as the level of detail */
    if (px > py) {
	return px * ((__GLfloat) 0.25);
    } else {
	return py * ((__GLfloat) 0.25);
    }
}

/* ARGSUSED */
__GLfloat __glNopLineRho(__GLcontext *gc, GLint unit,
			 __GLfloat s, __GLfloat t, __GLfloat r,
			 __GLfloat qw)
{
    return __glZero;
}

__GLfloat __glComputeLineRho(__GLcontext *gc, GLint unit,
			     __GLfloat s, __GLfloat t, __GLfloat r,
			     __GLfloat qw)
{
    __GLfloat pspx, pspy, ptpx, ptpy;
    __GLfloat pupx, pupy, pvpx, pvpy;
    __GLfloat temp, pu, pv, p2;
    __GLfloat magnitude, invMag, invMag2;
    __GLfloat dx, dy, invqw;
    const __GLtexture *tex = gc->texture.unit[unit].currentTexture;
    const __GLvertex *v0 = gc->line.options.v0;
    const __GLvertex *v1 = gc->line.options.v1;

    /* Compute the length of the line (its magnitude) */
    dx = v1->window.x - v0->window.x;
    dy = v1->window.y - v0->window.y;
    magnitude = __GL_SQRTF(dx*dx + dy*dy);
    invMag = magnitude ? __glOne / magnitude : __glZero;
    invMag2 = invMag * invMag;

    invqw = qw ? __glOne / qw : __glZero;

    /* Compute s partials */
    temp = ((v1->texture[unit].x - v0->texture[unit].x) - s) * invqw;
    pspx = temp * dx * invMag2;
    pspy = temp * dy * invMag2;

    /* Compute t partials */
    temp = ((v1->texture[unit].y - v0->texture[unit].y) - t) * invqw;
    ptpx = temp * dx * invMag2;
    ptpy = temp * dy * invMag2;

    pupx = pspx * tex->level[0]->width2f;
    pupy = pspy * tex->level[0]->width2f;
    pvpx = ptpx * tex->level[0]->height2f;
    pvpy = ptpy * tex->level[0]->height2f;

    /* Now compute rho */
    pu = pupx * dx + pupy * dy;
    pv = pvpx * dx + pvpy * dy;
    p2 = pu * pu + pv * pv;

    if (tex->dim > 2) {
        __GLfloat prpx, prpy;
        __GLfloat pwpx, pwpy;
        __GLfloat pw;

        /* Compute r partials */
	temp = ((v1->texture[unit].z - v0->texture[unit].z) - r) * invqw;
        prpx = temp * dx * invMag2;
        prpy = temp * dy * invMag2;

        pwpx = prpx * tex->level[0]->depth2f;
        pwpy = prpy * tex->level[0]->depth2f;

        pw = pwpx * dx + pwpy * dy;
        p2 += pw * pw;
    }

    return (p2 * invMag2);
}

/************************************************************************/

/*
** Fast texture a fragment assumes that rho is noise - this is true
** when no mipmapping is being done and the min and mag filters are
** the same.
*/
void __glFastTextureFragment(__GLcontext *gc, __GLtexture *tex,
			     __GLcolor *color,
			     __GLfloat s, __GLfloat t, __GLfloat r,
			     __GLfloat rho)
{
    __GLtexel texel;

#if defined(__GL_LINT)
    rho = rho;
#endif
    (*tex->magnify)(tex, __glZero, s, t, r, &texel);
    (*tex->env)(gc, color, &texel);
}

/*
** Non-mipmapping texturing function.
*/
void __glTextureFragment(__GLcontext *gc, __GLtexture *tex,
			 __GLcolor *color,
			 __GLfloat s, __GLfloat t, __GLfloat r,
			 __GLfloat rho)
{
    __GLtexel texel;

    /* Clamp rho between adjusted minLOD and maxLOD. */
    if (rho < tex->adjustedMinLOD) {
	rho = tex->adjustedMinLOD;
    } else if (rho > tex->adjustedMaxLOD){
	rho = tex->adjustedMaxLOD;
    }

    if (rho <= tex->c) {
	(*tex->magnify)(tex, __glZero, s, t, r, &texel);
    } else {
	(*tex->minnify)(tex, __glZero, s, t, r, &texel);
    }

    /* Now apply texture environment to get final color */
    (*tex->env)(gc, color, &texel);
}

void __glMipMapFragment(__GLcontext *gc, __GLtexture *tex,
			__GLcolor *color,
			__GLfloat s, __GLfloat t, __GLfloat r, __GLfloat rho)
{
    __GLtexel texel;
    __GLfloat lambda;

    /* Clamp rho between adjusted minLOD and maxLOD. */
    if (rho < tex->adjustedMinLOD) {
	rho = tex->adjustedMinLOD;
    } else if (rho > tex->adjustedMaxLOD){
	rho = tex->adjustedMaxLOD;
    }

    if (rho <= tex->c) {
	(*tex->magnify)(tex, __glZero, s, t, r, &texel);
    } else {
	/* Convert rho to lambda */
	if (rho) {
	    __GLfloat twotolev;
	    GLuint rhoi, lev;

	    /* this is an approximation of log base 2 */
	    rhoi = rho;
	    lev = 0;
	    while (rhoi >>= 1) lev++;
	    twotolev = 1 << lev;
	    lambda = (lev + ((rho - twotolev) / twotolev)) * 0.5;
	} else {
	    lambda = __glZero;
	}
	(*tex->minnify)(tex, lambda, s, t, r, &texel);
    }

    /* Now apply texture environment to get final color */
    (*tex->env)(gc, color, &texel);
}

void __glMultiTextureFragment(__GLcontext *gc, __GLcolor *color,
			__GLcoord strq[__GL_NUM_TEXTURE_UNITS],
			__GLfloat rhow[__GL_NUM_TEXTURE_UNITS])
{
    GLint i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	__GLtexture *tex = gc->texture.unit[i].currentTexture;

	if (tex) {
	    __GLfloat qwInv = (strq[i].w) ? __glOne / strq[i].w : __glZero;

	    (*gc->procs.textureUnit[i])(gc, tex, color,
				    strq[i].x * qwInv,
				    strq[i].y * qwInv,
				    strq[i].z * qwInv,
				    rhow[i] * qwInv);
	}
    }
}

void __glMultiTextureFragmentUVScale(__GLcontext *gc, __GLcolor *color,
			__GLcoord strq[__GL_NUM_TEXTURE_UNITS],
			__GLfloat rhow[__GL_NUM_TEXTURE_UNITS])
{
    GLint i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	__GLtexture *tex = gc->texture.unit[i].currentTexture;

	if (tex) {
	    __GLmipMapLevel *lp = tex->level[tex->state.params.baseLevel];
	    __GLfloat qwInv = (strq[i].w) ? __glOne / strq[i].w : __glZero;

	    (*gc->procs.textureUnit[i])(gc, tex, color,
				    strq[i].x * qwInv * lp->width2f,
				    strq[i].y * qwInv * lp->height2f,
				    strq[i].z * qwInv * lp->depth2f,
				    rhow[i] * qwInv);
	}
    }
}
