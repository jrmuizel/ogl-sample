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
*/
/*
** Lighting and coloring code.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/s_light.c#15 $
*/
#include "context.h"
#include "global.h"
#include "g_imports.h"
#if defined(__GL_USE_ASMCODE) && defined(i386)
#include "i386.h"
#if defined(__GL_USE_K3DCODE)
#include "i386/k3d.h"
#endif
#endif

/*
** Scale an incoming color from the user.
*/
void __glScaleColorf(__GLcontext *gc, __GLcolor *dst, const GLfloat src[4])
{
    dst->r = src[0] * gc->constants.redScale;
    dst->g = src[1] * gc->constants.greenScale;
    dst->b = src[2] * gc->constants.blueScale;
    dst->a = src[3] * gc->constants.alphaScale;
}

/*
** Clamp and scale an incoming color from the user.
** Called via (*gc->procs.applyColor)
*/
void __glClampAndScaleColorf(__GLcontext *gc, __GLcolor *d, const GLfloat s[4])
{
    __GLfloat zero = __glZero;

    d->r = s[0] * gc->constants.redScale;
    if (d->r < zero) d->r = zero;
    if (d->r > gc->constants.redScale) d->r = gc->constants.redScale;

    d->g = s[1] * gc->constants.greenScale;
    if (d->g < zero) d->g = zero;
    if (d->g > gc->constants.greenScale) d->g = gc->constants.greenScale;

    d->b = s[2] * gc->constants.blueScale;
    if (d->b < zero) d->b = zero;
    if (d->b > gc->constants.blueScale) d->b = gc->constants.blueScale;

    d->a = s[3] * gc->constants.alphaScale;
    if (d->a < zero) d->a = zero;
    if (d->a > gc->constants.alphaScale) d->a = gc->constants.alphaScale;
}

/*
** Clamp an incoming color from the user.
*/
void __glClampColorf(__GLcontext *gc, __GLcolor *d, const GLfloat s[4])
{
    __GLfloat zero = __glZero;
    __GLfloat one = __glOne;
    __GLfloat r,g,b,a;

    r = s[0];
    g = s[1];
    b = s[2];
    a = s[3];

    if (r < zero) d->r = zero;
    else if (r > one) d->r = one;
    else d->r = r;

    if (g < zero) d->g = zero;
    else if (g > one) d->g = one;
    else d->g = g;

    if (b < zero) d->b = zero;
    else if (b > one) d->b = one;
    else d->b = b;

    if (a < zero) d->a = zero;
    else if (a > one) d->a = one;
    else d->a = a;
}


/*
** Clamp an already scaled color.
*/
static void ClampScaledColorf(__GLcontext *gc, __GLcolor *d, 
			      const GLfloat s[4])
{
    __GLfloat zero = __glZero;

    d->r = s[0];
    if (d->r < zero) d->r = zero;
    if (d->r > gc->constants.redScale) d->r = gc->constants.redScale;

    d->g = s[1];
    if (d->g < zero) d->g = zero;
    if (d->g > gc->constants.greenScale) d->g = gc->constants.greenScale;

    d->b = s[2];
    if (d->b < zero) d->b = zero;
    if (d->b > gc->constants.blueScale) d->b = gc->constants.blueScale;

    d->a = s[3];
    if (d->a < zero) d->a = zero;
    if (d->a > gc->constants.alphaScale) d->a = gc->constants.alphaScale;
}

/*
** Scale an incoming color from the user.  This also converts the incoming
** color to floating point.
*/
void __glScaleColori(__GLcontext *gc, __GLcolor *dst, const GLint src[4])
{
    dst->r = __GL_I_TO_FLOAT(src[0]) * gc->constants.redScale;
    dst->g = __GL_I_TO_FLOAT(src[1]) * gc->constants.greenScale;
    dst->b = __GL_I_TO_FLOAT(src[2]) * gc->constants.blueScale;
    dst->a = __GL_I_TO_FLOAT(src[3]) * gc->constants.alphaScale;
}

/*
** Clamp and scale an incoming color from the user.
*/
void __glClampAndScaleColori(__GLcontext *gc, __GLcolor *d, const GLint s[4])
{
    __GLfloat zero = __glZero;

    d->r = __GL_I_TO_FLOAT(s[0]) * gc->constants.redScale;
    if (d->r < zero) d->r = zero;
    if (d->r > gc->constants.redScale) d->r = gc->constants.redScale;

    d->g = __GL_I_TO_FLOAT(s[1]) * gc->constants.greenScale;
    if (d->g < zero) d->g = zero;
    if (d->g > gc->constants.greenScale) d->g = gc->constants.greenScale;

    d->b = __GL_I_TO_FLOAT(s[2]) * gc->constants.blueScale;
    if (d->b < zero) d->b = zero;
    if (d->b > gc->constants.blueScale) d->b = gc->constants.blueScale;

    d->a = __GL_I_TO_FLOAT(s[3]) * gc->constants.alphaScale;
    if (d->a < zero) d->a = zero;
    if (d->a > gc->constants.alphaScale) d->a = gc->constants.alphaScale;
}

/*
** Clamp an incoming color from the user.
*/
void __glClampColori(__GLcontext *gc, __GLcolor *d, const GLint s[4])
{
    __GLfloat zero = __glZero;
    __GLfloat one = __glOne;
    __GLfloat r,g,b,a;

    r = __GL_I_TO_FLOAT(s[0]);
    g = __GL_I_TO_FLOAT(s[1]);
    b = __GL_I_TO_FLOAT(s[2]);
    a = __GL_I_TO_FLOAT(s[3]);

    if (r < zero) d->r = zero;
    else if (r > one) d->r = one;
    else d->r = r;

    if (g < zero) d->g = zero;
    else if (g > one) d->g = one;
    else d->g = g;

    if (b < zero) d->b = zero;
    else if (b > one) d->b = one;
    else d->b = b;

    if (a < zero) d->a = zero;
    else if (a > one) d->a = one;
    else d->a = a;
}

/*
** Reverse the scaling back to the users original
*/
void __glUnScaleColorf(__GLcontext *gc, GLfloat dst[4], const __GLcolor* src)
{
    dst[0] = src->r * gc->constants.oneOverRedScale;
    dst[1] = src->g * gc->constants.oneOverGreenScale;
    dst[2] = src->b * gc->constants.oneOverBlueScale;
    dst[3] = src->a * gc->constants.oneOverAlphaScale;
}

/*
** Reverse the scaling back to the users original
*/
void __glUnScaleColori(__GLcontext *gc, GLint dst[4], const __GLcolor* src)
{
    dst[0] = __GL_FLOAT_TO_I(src->r * gc->constants.oneOverRedScale);
    dst[1] = __GL_FLOAT_TO_I(src->g * gc->constants.oneOverGreenScale);
    dst[2] = __GL_FLOAT_TO_I(src->b * gc->constants.oneOverBlueScale);
    dst[3] = __GL_FLOAT_TO_I(src->a * gc->constants.oneOverAlphaScale);
}

/*
** Clamp an already scaled RGB color.
*/
void __glClampRGBColor(__GLcontext *gc, __GLcolor *dst, const __GLcolor *src)
{
    __GLfloat zero = __glZero;
    __GLfloat r, g, b, a;
    __GLfloat rl, gl, bl, al;

    r = src->r; rl = gc->constants.redScale;
    if (r <= zero) {
	dst->r = zero;
    } else {
	if (r >= rl) {
	    dst->r = rl;
	} else {
	    dst->r = r;
	}
    }
    g = src->g; gl = gc->constants.greenScale;
    if (g <= zero) {
	dst->g = zero;
    } else {
	if (g >= gl) {
	    dst->g = gl;
	} else {
	    dst->g = g;
	}
    }
    b = src->b; bl = gc->constants.blueScale;
    if (b <= zero) {
	dst->b = zero;
    } else {
	if (b >= bl) {
	    dst->b = bl;
	} else {
	    dst->b = b;
	}
    }
    a = src->a; al = gc->constants.alphaScale;
    if (a <= zero) {
	dst->a = zero;
    } else {
	if (a >= al) {
	    dst->a = al;
	} else {
	    dst->a = a;
	}
    }
}

/************************************************************************/

static void Vvec(__GLcoord *r, const __GLcoord *p1, const __GLcoord *p2)
{
   const int i = *(const int *)&p1->w;
   const int j = *(const int *)&p2->w;
   
   /*
   * Test for +/-0.0
   */
   if (i << 1) {
      if (j << 1) {
      /*
	     * Cross multiply instead of dividing, since
        * result always gets normalized.
         */
         __GLfloat w1, w2;
         w1 = p1->w;
         w2 = p2->w;
         r->x = p2->x * w1 - p1->x * w2;
         r->y = p2->y * w1 - p1->y * w2;
         r->z = p2->z * w1 - p1->z * w2;
      } else {
      /*
	     * w2 == 0
         */
         r->x = p2->x;
         r->y = p2->y;
         r->z = p2->z;
      }
   } else if (j << 1) {
   /*
   * w1 == 0
	     */
      r->x = -p1->x;
      r->y = -p1->y;
      r->z = -p1->z;
   } else {
   /*
   * w1 == 0 & w2 == 0
	     */
      r->x = p2->x - p1->x;
      r->y = p2->y - p1->y;
      r->z = p2->z - p1->z;
   }
}

/*
 * The following vector normalization macros are adapted from the
 * __glNormalize() function in s_math.c.  The major difference is that
 * only one iteration of Newton-Raphson is used.  This appears to be
 * sufficient for the purpose of lighting with 8-bit color channels.
 * XXX the lighting conformance tests appear to require more precision.
 */

/*
 * Normalize the vector <X,Y,Z> in place.
 */
#define NORMALIZE_3ARG( X, Y, Z )			\
{							\
    __GLfloat zero = __glZero;				\
    __GLfloat len = (X)*(X) + (Y)*(Y) + (Z)*(Z);	\
    if (len <= zero) {					\
	X = zero;					\
	Y = zero;					\
	Z = zero;					\
    }							\
    else if (len != 1.0F) {				\
        union {						\
            unsigned int i;				\
            float f;					\
        } seed;						\
        float xy, subexp;				\
        seed.f = len;					\
        seed.i = 0x5f375a00u - (seed.i >> 1);		\
	xy = len * seed.f * seed.f;			\
	subexp = 3.f - xy;				\
	len = .0625f * seed.f * subexp * (12.f - xy * subexp * subexp); \
	X *= len;					\
	Y *= len;					\
	Z *= len;					\
    }							\
}

/*
 * Normalize the vector <X,Y,Z> returning <NX,NY,NZ>.
 */
#define NORMALIZE_6ARG( NX, NY, NZ, X, Y, Z )		\
{							\
    __GLfloat zero = __glZero;				\
    __GLfloat len = (X)*(X) + (Y)*(Y) + (Z)*(Z);	\
    if (len <= zero) {					\
	NX = zero;					\
	NY = zero;					\
	NZ = zero;					\
    }							\
    else if (len == 1.0F) {				\
	NX = X;						\
	NY = Y;						\
	NZ = Z;						\
    }							\
    else {						\
        union {						\
            unsigned int i;				\
            float f;					\
        } seed;						\
        float xy, subexp;				\
        seed.f = len;					\
        seed.i = 0x5f375a00u - (seed.i >> 1);		\
	xy = len * seed.f * seed.f;			\
	subexp = 3.f - xy;				\
	len = .0625f * seed.f * subexp * (12.f - xy * subexp * subexp); \
	NX = X * len;					\
	NY = Y * len;					\
	NZ = Z * len;					\
    }							\
}

/*
** BWH:
** this is a pure C version of the NORMALIZE_nARG macro
** I wrote this so that I could step through the code with
** a debugger.
*/
#if 0
static void NormalizeCoord_C( __GLcoord *dst, const __GLcoord *src )
{
   __GLfloat len = src->x * src->x + src->y * src->y + src->z * src->z;

   if ( len <= 0 )
   {
      dst->x = dst->y = dst->z = 0.0F;
   }
   else if ( len == 1.0F )
   {
      dst->x = src->x;
      dst->y = src->y;
      dst->z = src->z;
   }
   else
   {
      float xy, subexp;
      union {						
         unsigned int i;				
         float f;					
      } seed;

      seed.f = len;
      seed.i = 0x5f375a00u - (seed.i >> 1);
      xy     = len * seed.f * seed.f;
      subexp = 3.f - xy;
      len    = .0625f * seed.f * subexp * (12.f - xy * subexp * subexp);
      dst->x = src->x * len;
      dst->y = src->y * len;
      dst->z = src->z * len;
   }
}
#endif


void __glCalcRGBColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLlightSourceMachine *lsm;
#if USE_LSPMM
    __GLlightSourcePerMaterialMachine *lspmm;
#endif
    __GLmaterialMachine *msm;
    __GLmaterialState   *ms;
    __GLcolor *c, *sc;
    __GLfloat r, g, b, zero, nx, ny, nz;
    GLboolean eyeWIsZero, localViewer, separateSpecular;
    static __GLcoord Pe = { 0, 0, 0, 1 };

    /* Pick material to use */
    if (face == __GL_FRONTFACE) {
	c = &vx->colors[__GL_FRONTFACE];
	sc = &vx->colors[__GL_FRONTFACE_SPECULAR];
	msm = &gc->light.front;
	ms  = &gc->state.light.front;
	nx = vx->normal.x;
	ny = vx->normal.y;
	nz = vx->normal.z;
    } else {
	c = &vx->colors[__GL_BACKFACE];
	sc = &vx->colors[__GL_BACKFACE_SPECULAR];
	msm = &gc->light.back;
	ms  = &gc->state.light.back;
	nx = -vx->normal.x;
	ny = -vx->normal.y;
	nz = -vx->normal.z;
    }

    /* Compute the color */
    zero = __glZero;
    r = msm->sceneColor.r;
    g = msm->sceneColor.g;
    b = msm->sceneColor.b;
    eyeWIsZero = vx->eye.w == zero;
    localViewer = gc->state.light.model.localViewer;
    separateSpecular = ( gc->state.light.model.colorControl ==
			 GL_SEPARATE_SPECULAR_COLOR );
    lsm = gc->light.sources;
    while (lsm) {
        __GLlightSourceState *lss = lsm->state;

#if USE_LSPMM
	lspmm = &lsm->front + face;
#endif
	if (lsm->slowPath || eyeWIsZero) {
	    __GLcoord hHat, vPli, vPliHat;
	    __GLfloat att, attSpot;
	    __GLfloat hv_x, hv_y, hv_z;
    
	    /* Compute unit h[i] */
	    Vvec(&vPli, &vx->eye, &lsm->position);
            NORMALIZE_6ARG( vPliHat.x, vPliHat.y, vPliHat.z, 
                            vPli.x, vPli.y, vPli.z );
	    if (localViewer) {
                __GLfloat vPeHat_x, vPeHat_y, vPeHat_z;
                /* vPeHat = vector from vx->eye to Pe (Pe is a constant) */
                vPeHat_x = -vx->eye.x;
                vPeHat_y = -vx->eye.y;
                vPeHat_z = -vx->eye.z;
                NORMALIZE_3ARG( vPeHat_x, vPeHat_y, vPeHat_z );
		hv_x = vPliHat.x + vPeHat_x;
		hv_y = vPliHat.y + vPeHat_y;
		hv_z = vPliHat.z + vPeHat_z;
	    } else {
		hv_x = vPliHat.x;
		hv_y = vPliHat.y;
		hv_z = vPliHat.z + __glOne;
	    }
            /* hHat = normalized(hv) */
            NORMALIZE_6ARG( hHat.x, hHat.y, hHat.z, hv_x, hv_y, hv_z );

	    /* Compute attenuation */
	    if (lsm->position.w != zero) {
		__GLfloat k0, k1, k2, dist;

		k0 = lsm->constantAttenuation;
		k1 = lsm->linearAttenuation;
		k2 = lsm->quadraticAttenuation;
		if ((k1 == zero) && (k2 == zero)) {
		    /* Use pre-computed 1/k0 */
		    att = lsm->attenuation;
		} else {
		    dist = __GL_SQRTF(vPli.x*vPli.x + vPli.y*vPli.y
				      + vPli.z*vPli.z);
		    att = k0 + (k1 + k2 * dist) * dist;
		    att = (att) ? __glOne / att : __glZero;
		}
	    } else {
		att = __glOne;
	    }

	    /* Compute spot effect if light is a spot light */
	    attSpot = att;
	    if (lsm->isSpot) {
		__GLfloat dot, px, py, pz;

		px = -vPliHat.x;
		py = -vPliHat.y;
		pz = -vPliHat.z;
		dot = px * lsm->direction.x + py * lsm->direction.y
		    + pz * lsm->direction.z;
		if ((dot >= lsm->threshold) && (dot >= lsm->cosCutOffAngle)) {
		    GLuint ix = (GLuint)((dot - lsm->threshold) * lsm->scale 
				+ __glHalf);
		    if (ix < __GL_SPOT_LOOKUP_TABLE_SIZE) {
			attSpot = att * lsm->spotTable[ix];
		    }
		} else {
		    attSpot = zero;
		}
	    }

	    /* Add in remaining effect of light, if any */
	    if (attSpot) {
		__GLfloat n1;
		__GLcolor sum;

#if USE_LSPMM
		sum.r = lspmm->ambient.r;
		sum.g = lspmm->ambient.g;
		sum.b = lspmm->ambient.b;
#else
		sum.r = lss->ambient.r * ms->ambient.r;
		sum.g = lss->ambient.g * ms->ambient.g;
		sum.b = lss->ambient.b * ms->ambient.b;
#endif

		n1 = nx * vPliHat.x + ny * vPliHat.y + nz * vPliHat.z;
		if (n1 > zero) {
                    __GLfloat n2;
		    n2 = nx * hHat.x + ny * hHat.y + nz * hHat.z;
		    n2 -= msm->threshold;
		    if (n2 >= zero) {
			GLuint ix = (GLuint)(n2 * msm->scale + __glHalf);
			if (ix < __GL_SPEC_LOOKUP_TABLE_SIZE) {
			    n2 = msm->specTable[ix];
#if USE_LSPMM
			    if ( separateSpecular ) {
				sc->r = n2 * lspmm->specular.r;
				sc->g = n2 * lspmm->specular.g;
				sc->b = n2 * lspmm->specular.r;
			    } else {
				sum.g += n2 * lspmm->specular.g;
				sum.b += n2 * lspmm->specular.b;
				sum.b += n2 * lspmm->specular.b;
			    }
#else
			    if ( separateSpecular ) {
				sc->r = n2 * lss->specular.r * ms->specular.r;
				sc->g = n2 * lss->specular.g * ms->specular.g;
				sc->b = n2 * lss->specular.b * ms->specular.b;
			    } else {
				sum.r += n2 * lss->specular.r * ms->specular.r;
				sum.g += n2 * lss->specular.g * ms->specular.g;
				sum.b += n2 * lss->specular.b * ms->specular.b;
			    }
#endif
			} else {
                            /* n2 = __glOne; */
#if USE_LSPMM
			    if ( separateSpecular ) {
				sc->r = lspmm->specular.r;
				sc->g = lspmm->specular.g;
				sc->b = lspmm->specular.b;
			    } else {
				sum.r += lspmm->specular.r;
				sum.g += lspmm->specular.g;
				sum.b += lspmm->specular.b;
			    }
#else
			    if ( separateSpecular ) {
				sc->r = lss->specular.r * ms->specular.r;
				sc->g = lss->specular.g * ms->specular.g;
				sc->b = lss->specular.b * ms->specular.b;
			    } else { 
				sum.r += lss->specular.r * ms->specular.r;
				sum.g += lss->specular.g * ms->specular.g;
				sum.b += lss->specular.b * ms->specular.b;
			    }
#endif
                        }
		    }
#if USE_LSPMM
		    sum.r += n1 * lspmm->diffuse.r;
		    sum.g += n1 * lspmm->diffuse.g;
		    sum.b += n1 * lspmm->diffuse.b;
#else
		    sum.r += n1 * ms->diffuse.r * lss->diffuse.r;
		    sum.g += n1 * ms->diffuse.g * lss->diffuse.g;
		    sum.b += n1 * ms->diffuse.b * lss->diffuse.b;
#endif
		}

		r += attSpot * sum.r;
		g += attSpot * sum.g;
		b += attSpot * sum.b;

		if (separateSpecular) {
		    sc->r *= attSpot;
		    sc->g *= attSpot;
		    sc->b *= attSpot;
		}
	    }
	} else {
            /* Fast lighting */
	    __GLfloat n1;

#if USE_LSPMM
	    r += lspmm->ambient.r;
	    g += lspmm->ambient.g;
	    b += lspmm->ambient.b;
#else
		r += lss->ambient.r * ms->ambient.r;
		g += lss->ambient.g * ms->ambient.g;
		b += lss->ambient.b * ms->ambient.b;
#endif

	    /* Add in specular and diffuse effect of light, if any */
	    n1 = nx * lsm->unitVPpli.x + ny * lsm->unitVPpli.y +
		nz * lsm->unitVPpli.z;
	    if (n1 > zero) {
                __GLfloat n2;
		n2 = nx * lsm->hHat.x + ny * lsm->hHat.y + nz * lsm->hHat.z;
		n2 -= msm->threshold;
		if (n2 >= zero) {
		    GLuint ix = (GLuint)(n2 * msm->scale + __glHalf);
		    if (ix < __GL_SPEC_LOOKUP_TABLE_SIZE) {
			n2 = msm->specTable[ix];
#if USE_LSPMM
			if ( separateSpecular ) {
			    sc->r = n2 * lspmm->specular.r;
			    sc->g = n2 * lspmm->specular.g;
			    sc->b = n2 * lspmm->specular.b;
			} else {
			    r += n2 * lspmm->specular.r;
			    g += n2 * lspmm->specular.g;
			    b += n2 * lspmm->specular.b;
			}
#else
			if ( separateSpecular ) {
			    sc->r = n2 * lss->specular.r * ms->specular.r;
			    sc->g = n2 * lss->specular.g * ms->specular.g;
			    sc->b = n2 * lss->specular.b * ms->specular.b;
			} else {
			    r += n2 * lss->specular.r * ms->specular.r;
			    g += n2 * lss->specular.g * ms->specular.g;
			    b += n2 * lss->specular.b * ms->specular.b;
			}
#endif
		    } else {
                        /* n2 = __glOne;*/
#if USE_LSPMM
			if ( separateSpecular ) {
			    sc->r = lspmm->specular.r;
			    sc->g = lspmm->specular.g;
			    sc->b = lspmm->specular.b;
			} else {
			    r += lspmm->specular.r;
			    g += lspmm->specular.g;
			    b += lspmm->specular.b;
			}
#else
			if ( separateSpecular ) {
			    sc->r = lss->specular.r * ms->specular.r;
			    sc->g = lss->specular.g * ms->specular.g;
			    sc->b = lss->specular.b * ms->specular.b;
			} else {
			    r += lss->specular.r * ms->specular.r;
			    g += lss->specular.g * ms->specular.g;
			    b += lss->specular.b * ms->specular.b;
			}
#endif
                    }
		}
#if USE_LSPMM
		r += n1 * lspmm->diffuse.r;
		g += n1 * lspmm->diffuse.g;
		b += n1 * lspmm->diffuse.b;
#else
		r += n1 * lss->diffuse.r * ms->diffuse.r;
		g += n1 * lss->diffuse.g * ms->diffuse.g;
		b += n1 * lss->diffuse.b * ms->diffuse.b;
#endif
	    }
	}
	lsm = lsm->next;
    }

    /* Clamp the computed color */
    {
	__GLfloat rl, gl, bl;
	rl = gc->constants.redScale;
	gl = gc->constants.greenScale;
	bl = gc->constants.blueScale;

	if (r <= zero)  r = zero;
	else if (r >= rl) r = rl;

	if (g <= zero)  g = zero;
	else if (g >= gl) g = gl;

	if (b <= zero)  b = zero;
	else if (b >= bl) b = bl;

	c->r = r;
	c->g = g;
	c->b = b;
	c->a = msm->alpha;

	if (separateSpecular) {
	    if (sc->r <= zero)  sc->r = zero;
	    else if (sc->r >= rl) sc->r = rl;

	    if (sc->g <= zero)  sc->g = zero;
	    else if (sc->g >= gl) sc->g = gl;

	    if (sc->b <= zero)  sc->b = zero;
	    else if (sc->b >= bl) sc->b = bl;

	    sc->a = gc->constants.alphaScale;
	}

    }
}

#if !(defined(__GL_USE_ASMCODE) && defined(mips))

#define LSPMM_AMBIENT_R 0x000
#define LSPMM_AMBIENT_G 0x004
#define LSPMM_AMBIENT_B 0x008
#define LSPMM_AMBIENT_A 0x00C

#define LSPMM_DIFFUSE_R 0x010
#define LSPMM_DIFFUSE_G 0x014
#define LSPMM_DIFFUSE_B 0x018
#define LSPMM_DIFFUSE_A 0x01C

#define LSPMM_SPECULAR_R 0x020
#define LSPMM_SPECULAR_G 0x024
#define LSPMM_SPECULAR_B 0x028
#define LSPMM_SPECULAR_A 0x02C

#define LSM_UNITVPPLI_X  0x0B0
#define LSM_UNITVPPLI_Y  0x0B4
#define LSM_UNITVPPLI_Z  0x0B8

#define LSM_HHAT_X       0x0A0
#define LSM_HHAT_Y       0x0A4
#define LSM_HHAT_Z       0x0A8

#define LSM_FRONT        0x000
#define LSM_NEXT         0x0C8

#define MSM_SPECTABLE    0x044
#define MSM_THRESHOLD    0x048
#define MSM_SCALE        0x04C

#define SIZEOF_LSM       0xe0
#define SIZEOF_LSPMM     0x30

void __glFastCalcRGBColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLlightSourceMachine *lsm;
    __GLmaterialMachine    *msm;
    __GLmaterialState      *ms;
    __GLcolor *c;
    __GLfloat r, g, b;
    __GLfloat nx, ny, nz;
    __GLfloat zero;

    zero = __glZero;

    /* Pick material to use */
    if (face == __GL_FRONTFACE) {
        c = &vx->colors[__GL_FRONTFACE];
        msm = &gc->light.front;
		ms  = &gc->state.light.front;
        nx = vx->normal.x;
        ny = vx->normal.y;
        nz = vx->normal.z;
    } else {
        c = &vx->colors[__GL_BACKFACE];
        msm = &gc->light.back;
		ms  = &gc->state.light.back;
        nx = -vx->normal.x;
        ny = -vx->normal.y;
        nz = -vx->normal.z;
    }

    /* Compute the color */
    r = msm->sceneColor.r;
    g = msm->sceneColor.g;
    b = msm->sceneColor.b;
    lsm = gc->light.sources;

    while (lsm) {
        __GLfloat n1, n2;
        __GLlightSourceState *lss;
#if USE_LSPMM
        __GLlightSourcePerMaterialMachine *lspmm;

        lspmm = &lsm->front + face;
#endif

		lss   = lsm->state;
        
#if USE_LSPMM
        r += lspmm->ambient.r;
        g += lspmm->ambient.g;
        b += lspmm->ambient.b;
#else
        r += lss->ambient.r * ms->ambient.r;
        g += lss->ambient.g * ms->ambient.g;
        b += lss->ambient.b * ms->ambient.b;
#endif

        /* Add in specular and diffuse effect of light, if any */
        n1 = nx * lsm->unitVPpli.x + 
             ny * lsm->unitVPpli.y +
             nz * lsm->unitVPpli.z;

        if (n1 > zero) {
            n2 = nx * lsm->hHat.x + 
                 ny * lsm->hHat.y + 
                 nz * lsm->hHat.z;
            n2 -= msm->threshold;
            if (n2 >= zero) {
                GLuint ix = (GLuint)(n2 * msm->scale + __glHalf);
                if (ix < __GL_SPEC_LOOKUP_TABLE_SIZE) {
                    n2 = msm->specTable[ix];
                } 
                else {
                    n2 = __glOne;
                }
#if USE_LSPMM
                r += n2 * lspmm->specular.r;
                g += n2 * lspmm->specular.g;
                b += n2 * lspmm->specular.b;
#else
                r += n2 * ms->specular.r * lss->specular.r;
                g += n2 * ms->specular.g * lss->specular.g;
                b += n2 * ms->specular.b * lss->specular.b;
#endif
            }
#if USE_LSPMM
            r += n1 * lspmm->diffuse.r;
            g += n1 * lspmm->diffuse.g;
            b += n1 * lspmm->diffuse.b;
#else
            r += n1 * ms->diffuse.r * lss->diffuse.r;
            g += n1 * ms->diffuse.g * lss->diffuse.g;
            b += n1 * ms->diffuse.b * lss->diffuse.b;
#endif
        }
        lsm = lsm->next;
    }

    {

    /* Clamp the computed color */
    {
	__GLfloat rl, gl, bl;
	rl = gc->constants.redScale;
	gl = gc->constants.greenScale;
	bl = gc->constants.blueScale;
	if (r <= zero) {
	    r = zero;
	} else {
	    if (r >= rl) {
		r = rl;
	    }
	}
	if (g <= zero) {
	    g = zero;
	} else {
	    if (g >= gl) {
		g = gl;
	    }
	}
	if (b <= zero) {
	    b = zero;
	} else {
	    if (b >= bl) {
		b = bl;
	    }
	}
	c->r = r;
	c->g = g;
	c->b = b;
	c->a = msm->alpha;
    }
    }
}
#endif  /* !(__GL_USE_ASMCODE && mips) */

static void FogRGBColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLfloat fog, oneMinusFog;
    __GLcolor *cp;

    /* Get the vertex fog value */
    fog = vx->fog;
    oneMinusFog = __glOne - fog;

    /* Now whack the color */
    cp = &vx->colors[face];
    cp->r = fog * cp->r + oneMinusFog * gc->state.fog.color.r;
    cp->g = fog * cp->g + oneMinusFog * gc->state.fog.color.g;
    cp->b = fog * cp->b + oneMinusFog * gc->state.fog.color.b;
}

static void FogLitRGBColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLfloat fog, oneMinusFog;
    __GLcolor *cp;

    /* First compute vertex color */
    (*gc->procs.calcColor2)(gc, face, vx);

    /* Get the vertex fog value */
    fog = vx->fog;
    oneMinusFog = __glOne - fog;

    /* Now whack the color */
    cp = &vx->colors[face];
    cp->r = fog * cp->r + oneMinusFog * gc->state.fog.color.r;
    cp->g = fog * cp->g + oneMinusFog * gc->state.fog.color.g;
    cp->b = fog * cp->b + oneMinusFog * gc->state.fog.color.b;
}

void __glCalcCIColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLmaterialMachine *msm;
    __GLmaterialState *ms;
    __GLlightSourceMachine *lsm;
    __GLcolor *c;
    __GLfloat s, d, zero, nx, ny, nz;
    GLboolean eyeWIsZero, localViewer;
    static __GLcoord Pe = { 0, 0, 0, 1 };

    /* Pick material to use */
    if (face == __GL_FRONTFACE) {
	c = &vx->colors[__GL_FRONTFACE];
	ms = &gc->state.light.front;
	msm = &gc->light.front;
	nx = vx->normal.x;
	ny = vx->normal.y;
	nz = vx->normal.z;
    } else {
	c = &vx->colors[__GL_BACKFACE];
	ms = &gc->state.light.back;
	msm = &gc->light.back;
	nx = -vx->normal.x;
	ny = -vx->normal.y;
	nz = -vx->normal.z;
    }

    zero = __glZero;
    s = zero;
    d = zero;
    lsm = gc->light.sources;
    eyeWIsZero = vx->eye.w == zero;
    localViewer = gc->state.light.model.localViewer;
    while (lsm) {
	if (lsm->slowPath || eyeWIsZero) {
	    __GLfloat att, attSpot;
	    __GLcoord vPliHat, vPli, hHat;
	    __GLfloat hv_x, hv_y, hv_z;

            /* vPli = vector from vx->eye to lsm->position */
	    Vvec(&vPli, &vx->eye, &lsm->position);
            /* vPliHat = normalized(vPli) */
            NORMALIZE_6ARG( vPliHat.x, vPliHat.y, vPliHat.z,
                            vPli.x, vPli.y, vPli.z );
	    if (localViewer) {
                /* vPeHat = vector from vx_eye to Pe, Pe is constant */
		__GLfloat vPeHat_x, vPeHat_y, vPeHat_z;
                vPeHat_x = -vx->eye.x;
                vPeHat_y = -vx->eye.y;
                vPeHat_z = -vx->eye.z;
                /* vPeHat = normalized(vPeHat) */
		NORMALIZE_3ARG( vPeHat_x, vPeHat_y, vPeHat_z );
		hv_x = vPliHat.x + vPeHat_x;
		hv_y = vPliHat.y + vPeHat_y;
		hv_z = vPliHat.z + vPeHat_z;
	    } else {
		hv_x = vPliHat.x;
		hv_y = vPliHat.y;
		hv_z = vPliHat.z + __glOne;
	    }
            /* hHat = normalized(hv) */
            NORMALIZE_6ARG( hHat.x, hHat.y, hHat.z, hv_x, hv_y, hv_z );

	    /* Compute attenuation */
	    if (lsm->position.w != zero) {
		__GLfloat k0, k1, k2, dist;

		k0 = lsm->constantAttenuation;
		k1 = lsm->linearAttenuation;
		k2 = lsm->quadraticAttenuation;
		if ((k1 == zero) && (k2 == zero)) {
		    /* Use pre-computed 1/k0 */
		    att = lsm->attenuation;
		} else {
		    dist = __GL_SQRTF(vPli.x*vPli.x + vPli.y*vPli.y
				      + vPli.z*vPli.z);
		    att = k0 + (k1 + k2 * dist) * dist;
                    att = (att) ? __glOne / att : __glZero;
		}
	    } else {
		att = __glOne;
	    }

	    /* Compute spot effect if light is a spot light */
	    attSpot = att;
	    if (lsm->isSpot) {
		__GLfloat dot, px, py, pz;

		px = -vPliHat.x;
		py = -vPliHat.y;
		pz = -vPliHat.z;
		dot = px * lsm->direction.x + py * lsm->direction.y
		    + pz * lsm->direction.z;
		if ((dot >= lsm->threshold) && (dot >= lsm->cosCutOffAngle)) {
		    GLuint ix = (GLuint)((dot - lsm->threshold) * lsm->scale 
				+ __glHalf);
		    if (ix < __GL_SPOT_LOOKUP_TABLE_SIZE) {
			attSpot = att * lsm->spotTable[ix];
		    }
		} else {
		    attSpot = zero;
		}
	    }

	    /* Add in remaining effect of light, if any */
	    if (attSpot) {
                __GLfloat n1;
		n1 = nx * vPliHat.x + ny * vPliHat.y + nz * vPliHat.z;
		if (n1 > zero) {
		    d += attSpot * n1 * lsm->dli;
                    /*if (lsm->sli!=0.0F)*/ {
                        __GLfloat n2;
                        n2 = nx * hHat.x + ny * hHat.y + nz * hHat.z;
                        n2 -= msm->threshold;
                        if (n2 >= zero) {
                            GLuint ix = (GLuint)(n2 * msm->scale + __glHalf);
                            if (ix < __GL_SPEC_LOOKUP_TABLE_SIZE) {
                                n2 = msm->specTable[ix];
                                s += attSpot * n2 * lsm->sli;
                            }
                            else {
                                /* n2 = __glOne; */
                                s += attSpot * lsm->sli;
                            }
                        }
		    }
		}
	    }
	} else {
            /* Fast lighting calculation */
	    __GLfloat n1;

	    /* Compute specular contribution */
	    n1 = nx * lsm->unitVPpli.x + ny * lsm->unitVPpli.y +
		nz * lsm->unitVPpli.z;
	    if (n1 > zero) {
		d += n1 * lsm->dli;
                /*if (lsm->sli!=0.0F)*/ {
                    __GLfloat n2;
                    n2 = nx * lsm->hHat.x + ny * lsm->hHat.y + nz * lsm->hHat.z;
                    n2 -= msm->threshold;
                    if (n2 >= zero) {
                        GLuint ix = (GLuint)(n2 * msm->scale + __glHalf);
                        if (ix < __GL_SPEC_LOOKUP_TABLE_SIZE) {
                            n2 = msm->specTable[ix];
                            s += n2 * lsm->sli;
                        } else {
                            /* n2 = __glOne; */
                            s += lsm->sli;
                        }
                    }
		}
	    }
	}
	lsm = lsm->next;
    }

    /* Compute final color */
    if (s >= __glOne) {
        c->r = ms->cmaps;
    } else {
        __GLfloat ci;
        ci = ms->cmapa + (__glOne - s) * d * msm->cmapd_minus_cmapa
            + s * msm->cmaps_minus_cmapa;
        if (ci > ms->cmaps) {
            ci = ms->cmaps;
        }
        c->r = ci;
    }
}

/*
** No slow lights version.
*/
void __glFastCalcCIColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLmaterialMachine *msm;
    __GLmaterialState *ms;
    __GLlightSourceMachine *lsm;
    __GLcolor *c;
    __GLfloat s, d, zero, nx, ny, nz;

    zero = __glZero;
    #if 0 
    /*
    ** This shouldn't be necessary.  The fast lighting happens for
    ** only the case where there are no local lights and the viewer
    ** is non-local.  In that case, the object eye is not used.
    ** This check negates all fast lighting, because it checks for
    ** zero eye.w exactly when HAS_EYE has been left off.
    */
    if (vx->eye.w == zero) {
	/* Have to use slow path to deal with zero eye w coordinate */
	__glCalcCIColor(gc, face, vx);
	return;
    }
    #endif

    /* Pick material to use */
    if (face == __GL_FRONTFACE) {
	c = &vx->colors[__GL_FRONTFACE];
	ms = &gc->state.light.front;
	msm = &gc->light.front;
	nx = vx->normal.x;
	ny = vx->normal.y;
	nz = vx->normal.z;
    } else {
	c = &vx->colors[__GL_BACKFACE];
	ms = &gc->state.light.back;
	msm = &gc->light.back;
	nx = -vx->normal.x;
	ny = -vx->normal.y;
	nz = -vx->normal.z;
    }

    s = zero;
    d = zero;
    lsm = gc->light.sources;
    while (lsm) {
	__GLfloat n1;

	/* Compute specular contribution */
	n1 = nx * lsm->unitVPpli.x + ny * lsm->unitVPpli.y +
	    nz * lsm->unitVPpli.z;
	if (n1 > zero) {
	    d += n1 * lsm->dli;
            /*if (lsm->sli!=0.0F)*/ {
                __GLfloat n2;
                n2 = nx * lsm->hHat.x + ny * lsm->hHat.y + nz * lsm->hHat.z;
                n2 -= msm->threshold;
                if (n2 >= zero) {
                    GLuint ix = (GLuint)(n2 * msm->scale + __glHalf);
                    if (ix < __GL_SPEC_LOOKUP_TABLE_SIZE) {
                        n2 = msm->specTable[ix];
                        s += n2 * lsm->sli;
                    } else {
                        /* n2 = __glOne; */
                        s += lsm->sli;
                    }
                }
            }
	}
	lsm = lsm->next;
    }

    /* Compute final color */
    if (s >= __glOne) {
        c->r = ms->cmaps;
    } else {
        __GLfloat ci;
        ci = ms->cmapa + (__glOne - s) * d * msm->cmapd_minus_cmapa
            + s * msm->cmaps_minus_cmapa;
        if (ci > ms->cmaps) {
            ci = ms->cmaps;
        }
        c->r = ci;
    }
}


static void FogCIColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLfloat fog, oneMinusFog, maxR;
    __GLcolor *cp;

    /* Get the vertex fog value */
    fog = vx->fog;
    oneMinusFog = __glOne - fog;

    /* Now whack the color */
    cp = &vx->colors[face];
    cp->r = cp->r + oneMinusFog * gc->state.fog.index;
    maxR = (1 << gc->modes.indexBits) - 1;
    if (cp->r > maxR) {
	cp->r = maxR;
    }
}

static void FogLitCIColor(__GLcontext *gc, GLint face, __GLvertex *vx)
{
    __GLfloat fog, oneMinusFog, maxR;
    __GLcolor *cp;

    /* First compute vertex color */
    (*gc->procs.calcColor2)(gc, face, vx);

    /* Get the vertex fog value */
    fog = vx->fog;
    oneMinusFog = __glOne - fog;

    /* Now whack the color */
    cp = &vx->colors[face];
    cp->r = cp->r + oneMinusFog * gc->state.fog.index;
    maxR = (1 << gc->modes.indexBits) - 1;
    if (cp->r > maxR) {
	cp->r = maxR;
    }
}

/************************************************************************/

/*
** gc->procs.applyColor procs.  These are used to apply the current color
** change to either a material color, or to current.color (when not
** lighting), preparing the color for copying into the vertex.
*/

#if !(defined(__GL_USE_ASMCODE) && defined(mips))
/*
** Clamp and scale the current color (range 0.0 to 1.0) using the color
** buffer scales.  From here on out the colors in the vertex are in their
** final form.
*/
void __glClampAndScaleColor(__GLcontext *gc)
{
    __GLfloat one = __glOne;
    __GLfloat zero = __glZero;
    __GLfloat r, g, b, a;

    r = gc->state.current.userColor.r;
    g = gc->state.current.userColor.g;
    b = gc->state.current.userColor.b;
    a = gc->state.current.userColor.a;

    if (r <= zero) {
	gc->state.current.color.r = zero;
    } else
    if (r >= one) {
	gc->state.current.color.r = gc->constants.redScale;
    } else {
	gc->state.current.color.r = r * gc->constants.redScale;
    }

    if (g <= zero) {
	gc->state.current.color.g = zero;
    } else
    if (g >= one) {
	gc->state.current.color.g = gc->constants.greenScale;
    } else {
	gc->state.current.color.g = g * gc->constants.greenScale;
    }

    if (b <= zero) {
	gc->state.current.color.b = zero;
    } else
    if (b >= one) {
	gc->state.current.color.b = gc->constants.blueScale;
    } else {
	gc->state.current.color.b = b * gc->constants.blueScale;
    }

    if (a <= zero) {
	gc->state.current.color.a = zero;
    } else
    if (a >= one) {
	gc->state.current.color.a = gc->constants.alphaScale;
    } else {
	gc->state.current.color.a = a * gc->constants.alphaScale;
    }
}
#endif /* !(__GL_USE_ASMCODE && mips) */

static void ChangeMaterialEmission(__GLcontext *gc, __GLmaterialState *ms,
				   __GLmaterialMachine *msm)
{
    __GLlightSourcePerMaterialMachine *lspmm;
    __GLlightSourceMachine *lsm;
    GLboolean isBack;
    __GLfloat r, g, b;
    __GLcolor basecolor;

    r = gc->state.current.userColor.r * gc->constants.redScale;
    g = gc->state.current.userColor.g * gc->constants.greenScale;
    b = gc->state.current.userColor.b * gc->constants.blueScale;

    ms->emissive.r = r;
    ms->emissive.g = g;
    ms->emissive.b = b;
    ms->emissive.a = gc->state.current.userColor.a * gc->constants.alphaScale;

    msm->sceneColor.r = r + ms->ambient.r * gc->state.light.model.ambient.r;
    msm->sceneColor.g = g + ms->ambient.g * gc->state.light.model.ambient.g;
    msm->sceneColor.b = b + ms->ambient.b * gc->state.light.model.ambient.b;

    /*
    ** Update per-light-source state that depends on material emissive
    ** state.
    */
    isBack = msm == &gc->light.back;
    basecolor = msm->sceneColor;
    for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	lspmm = &lsm->front + isBack;

	basecolor.r += lspmm->ambient.r;
	basecolor.g += lspmm->ambient.g;
	basecolor.b += lspmm->ambient.b;
    }
    if (!isBack) {
	ClampScaledColorf(gc, &gc->light.baseColor, &basecolor.r);
    }
}

static void ChangeMaterialSpecular(__GLcontext *gc, __GLmaterialState *ms,
				   __GLmaterialMachine *msm)
{
    __GLlightSourcePerMaterialMachine *lspmm;
    __GLlightSourceMachine *lsm;
    __GLlightSourceState *lss;
    GLboolean isBack;
    __GLfloat r, g, b;

    r = gc->state.current.userColor.r;
    g = gc->state.current.userColor.g;
    b = gc->state.current.userColor.b;

    ms->specular.r = r;
    ms->specular.g = g;
    ms->specular.b = b;
    ms->specular.a = gc->state.current.userColor.a;

    /*
    ** Update per-light-source state that depends on material specular
    ** state
    */
    isBack = msm == &gc->light.back;
    for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	lspmm = &lsm->front + isBack;
	lss = lsm->state;

	/* Recompute per-light per-material cached specular */
	lspmm->specular.r = r * lss->specular.r;
	lspmm->specular.g = g * lss->specular.g;
	lspmm->specular.b = b * lss->specular.b;
    }
}

static void ChangeMaterialAmbient(__GLcontext *gc, __GLmaterialState *ms,
				  __GLmaterialMachine *msm)
{
    __GLlightSourcePerMaterialMachine *lspmm;
    __GLlightSourceMachine *lsm;
    __GLlightSourceState *lss;
    GLboolean isBack;
    __GLfloat r, g, b;
    __GLcolor basecolor;

    r = gc->state.current.userColor.r;
    g = gc->state.current.userColor.g;
    b = gc->state.current.userColor.b;

    ms->ambient.r = r;
    ms->ambient.g = g;
    ms->ambient.b = b;
    ms->ambient.a = gc->state.current.userColor.a;

    msm->sceneColor.r = ms->emissive.r + r * gc->state.light.model.ambient.r;
    msm->sceneColor.g = ms->emissive.g + g * gc->state.light.model.ambient.g;
    msm->sceneColor.b = ms->emissive.b + b * gc->state.light.model.ambient.b;

    /*
    ** Update per-light-source state that depends on material ambient
    ** state.
    */
    isBack = msm == &gc->light.back;
    basecolor = msm->sceneColor;
    for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	lspmm = &lsm->front + isBack;
	lss = lsm->state;

	/* Recompute per-light per-material cached ambient */
	lspmm->ambient.r = r * lss->ambient.r;
	lspmm->ambient.g = g * lss->ambient.g;
	lspmm->ambient.b = b * lss->ambient.b;

	basecolor.r += lspmm->ambient.r;
	basecolor.g += lspmm->ambient.g;
	basecolor.b += lspmm->ambient.b;
    }
    if (!isBack) {
	ClampScaledColorf(gc, &gc->light.baseColor, &basecolor.r);
    }
}

static void ChangeMaterialDiffuse(__GLcontext *gc, __GLmaterialState *ms,
				  __GLmaterialMachine *msm)
{
    __GLlightSourcePerMaterialMachine *lspmm;
    __GLlightSourceMachine *lsm;
    __GLlightSourceState *lss;
    GLboolean isBack;
    __GLfloat r, g, b, a;

    r = gc->state.current.userColor.r;
    g = gc->state.current.userColor.g;
    b = gc->state.current.userColor.b;
    a = gc->state.current.userColor.a;

    ms->diffuse.r = r;
    ms->diffuse.g = g;
    ms->diffuse.b = b;
    ms->diffuse.a = a;

    if (a < __glZero) {
	a = __glZero;
    } else if (a > __glOne) {
	a = __glOne;
    }
    msm->alpha = a * gc->constants.alphaScale;

    /*
    ** Update per-light-source state that depends on material diffuse
    ** state.
    */
    isBack = msm == &gc->light.back;
    for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	lspmm = &lsm->front + isBack;
	lss = lsm->state;

	/* Recompute per-light per-material cached diffuse */
	lspmm->diffuse.r = r * lss->diffuse.r;
	lspmm->diffuse.g = g * lss->diffuse.g;
	lspmm->diffuse.b = b * lss->diffuse.b;
    }
}

static void ChangeMaterialAmbientAndDiffuse(__GLcontext *gc,
					    __GLmaterialState *ms,
					    __GLmaterialMachine *msm)
{
    __GLlightSourcePerMaterialMachine *lspmm;
    __GLlightSourceMachine *lsm;
    __GLlightSourceState *lss;
    GLboolean isBack;
    __GLfloat r, g, b, a;
    __GLcolor basecolor;

    r = gc->state.current.userColor.r;
    g = gc->state.current.userColor.g;
    b = gc->state.current.userColor.b;
    a = gc->state.current.userColor.a;

    ms->ambient.r = r;
    ms->ambient.g = g;
    ms->ambient.b = b;
    ms->ambient.a = a;

    ms->diffuse.r = r;
    ms->diffuse.g = g;
    ms->diffuse.b = b;
    ms->diffuse.a = a;

    msm->sceneColor.r = ms->emissive.r + r * gc->state.light.model.ambient.r;
    msm->sceneColor.g = ms->emissive.g + g * gc->state.light.model.ambient.g;
    msm->sceneColor.b = ms->emissive.b + b * gc->state.light.model.ambient.b;

    if (a < __glZero) {
	a = __glZero;
    } else if (a > __glOne) {
	a = __glOne;
    }
    msm->alpha = a * gc->constants.alphaScale;

    /*
    ** Update per-light-source state that depends on per-material state.
    */
    isBack = msm == &gc->light.back;
    basecolor = msm->sceneColor;
    for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	lspmm = &lsm->front + isBack;
	lss = lsm->state;

	/* Recompute per-light per-material cached ambient */
	lspmm->ambient.r = r * lss->ambient.r;
	lspmm->ambient.g = g * lss->ambient.g;
	lspmm->ambient.b = b * lss->ambient.b;

	/* Recompute per-light per-material cached diffuse */
	lspmm->diffuse.r = r * lss->diffuse.r;
	lspmm->diffuse.g = g * lss->diffuse.g;
	lspmm->diffuse.b = b * lss->diffuse.b;

	basecolor.r += lspmm->ambient.r;
	basecolor.g += lspmm->ambient.g;
	basecolor.b += lspmm->ambient.b;
    }
    if (!isBack) {
	ClampScaledColorf(gc, &gc->light.baseColor, &basecolor.r);
    }
}

/*
** Called via (*gc->procs.applyColor)
*/
void __glChangeOneMaterialColor(__GLcontext *gc)
{

    /*
    ** If we are in the middle of contructing a primitive, we possibly
    ** need to validate the front and back colors of the vertices which 
    ** we have queued.
    */
    if (__GL_IN_BEGIN() && gc->vertex.materialNeeds) {
	(*gc->procs.matValidate)(gc);
    }

    (*gc->procs.changeMaterial)(gc, gc->light.cm, gc->light.cmm);

    if (!(gc->state.enables.general & __GL_LIGHTING_ENABLE)) {
	/*
	** If colormaterial is enabled but lighting isn't we have to
	** clamp the current.color after the material has been updated.
	*/
	__glClampAndScaleColor(gc);
    }
}

/*
** Called via (*gc->procs.applyColor)
*/
void __glChangeBothMaterialColors(__GLcontext *gc)
{
    /*
    ** If we are in the middle of contructing a primitive, we possibly
    ** need to validate the front and back colors of the vertices which 
    ** we have queued.
    */
    if (__GL_IN_BEGIN() && gc->vertex.materialNeeds) {
	(*gc->procs.matValidate)(gc);
    }

    (*gc->procs.changeMaterial)(gc, &gc->state.light.front, &gc->light.front);
    (*gc->procs.changeMaterial)(gc, &gc->state.light.back, &gc->light.back);

    if (!(gc->state.enables.general & __GL_LIGHTING_ENABLE)) {
	/*
	** If colormaterial is enabled but lighting isn't we have to
	** clamp the current.color after the material has been updated.
	*/
	__glClampAndScaleColor(gc);
    }
}

void __glChangeVertexMaterialColor(__GLcontext *gc, __GLcolor *col)
{
    __GLcolor saveColor = gc->state.current.userColor;

    gc->state.current.userColor = *col;
    (*gc->procs.applyColor)(gc);
    gc->state.current.userColor = saveColor;
}

/************************************************************************/

/*
** DEPENDENCIES:
**
** Material 	EMISSIVE, AMBIENT, DIFFUSE, SHININESS
** Light Model 	AMBIENT
*/

/*
** Compute derived state for a material
*/
static void ComputeMaterialState(__GLcontext *gc, __GLmaterialState *ms,
				 __GLmaterialMachine *msm, GLint changeBits)
{
    GLdouble  exponent;
    __GLspecLUTEntry *lut;

    if ((changeBits & (__GL_MATERIAL_EMISSIVE | __GL_MATERIAL_AMBIENT | 
	    __GL_MATERIAL_DIFFUSE | __GL_MATERIAL_SHININESS |
            __GL_MATERIAL_COLORINDEXES)) == 0) {
	return;
    }
    /* Only compute specular lookup table when it changes */
    if (!msm->cache || (ms->specularExponent != msm->specularExponent)) {
	/*
	** Specular lookup table generation.  Instead of performing a
	** "pow" computation each time a vertex is lit, we generate a
	** lookup table which approximates the pow function:
	**
	** 	n2 = n circle-dot hHat[i]
	** 	if (n2 >= threshold) {
	** 		n2spec = specTable[n2 * scale];
	** 		...
	** 	}
	**
	** Remember that n2 is a value constrained to be between 0.0 and
	** 1.0, inclusive (n is the normalized normal; hHat[i] is the
	** unit h vector).  "threshold" is the threshold where incoming
	** n2 values become meaningful for a given exponent.  The larger
	** the specular exponent, the closer "threshold" will approach
	** 1.0.
	**
	** A simple linear mapping of the n2 value to a table index will
	** not suffice because in most cases the majority of the table
	** entries would be zero, while the useful non-zero values would
	** be compressed into a few table entries.  By setting up a
	** threshold, we can use the entire table to represent the useful
	** values beyond the threshold.  "scale" is computed based on
	** this threshold.
	*/
	exponent = msm->specularExponent = ms->specularExponent;

	__glFreeSpecLUT(gc, msm->cache);
	lut = msm->cache = __glCreateSpecLUT(gc, exponent);
	msm->threshold = lut->threshold;
	msm->scale = lut->scale;
	msm->specTable = lut->table;
    }

    /* Compute scene color */
    if (changeBits & (__GL_MATERIAL_EMISSIVE | __GL_MATERIAL_AMBIENT)) {
	msm->sceneColor.r = ms->emissive.r
	    + ms->ambient.r * gc->state.light.model.ambient.r;
	msm->sceneColor.g = ms->emissive.g
	    + ms->ambient.g * gc->state.light.model.ambient.g;
	msm->sceneColor.b = ms->emissive.b
	    + ms->ambient.b * gc->state.light.model.ambient.b;
    }

    /* Clamp material alpha */
    if (changeBits & __GL_MATERIAL_DIFFUSE) {
	msm->alpha = ms->diffuse.a * gc->constants.alphaScale;
	if (msm->alpha < __glZero) {
	    msm->alpha = __glZero;
	} else if (msm->alpha > gc->constants.alphaScale) {
	    msm->alpha = gc->constants.alphaScale;
	}
    }

    /* Precompute some CI lighting expressions */
    if (changeBits & __GL_MATERIAL_COLORINDEXES) {
        msm->cmapd_minus_cmapa = ms->cmapd - ms->cmapa;
        msm->cmaps_minus_cmapa = ms->cmaps - ms->cmapa;
    }
}

/*
** DEPENDENCIES:
**
** Derived state:
**
** Enables	LIGHTx
** Lightx	DIFFUSE, AMBIENT, SPECULAR, POSITION, SPOT_EXPONENT, 
**		SPOT_CUTOFF, CONSTANT_ATTENUATION, LINEAR_ATTENUATION,
**		QUADRATIC_ATTENUATION
** Light Model  LOCAL_VIEWER
*/

/*
** Compute any derived state for the enabled lights.
*/
static void ComputeLightState(__GLcontext *gc)
{
    __GLlightSourceState *lss;
    __GLlightSourceMachine *lsm, **lsmp;
    __GLfloat zero;
    GLuint enables;
    GLint i;
    __GLspecLUTEntry *lut;

    zero = __glZero;

    lss = &gc->state.light.source[0];
    lsm = &gc->light.source[0];
    lsmp = &gc->light.sources;
    enables = gc->state.enables.lights;
    for (i = 0; i < gc->constants.numberOfLights;
	    i++, lss++, lsm++, enables >>= 1) {
	if (!(enables & 1)) continue;

	/* Link this enabled light on to the list */
	*lsmp = lsm;
	lsm->state = lss;	/* Could be done once, elsewhere... */
	lsmp = &lsm->next;

	/*
	** Compute per-light derived state that wasn't already done
	** in the api handlers.
	*/
	lsm->position = lss->positionEye;
	lsm->isSpot = lss->spotLightCutOffAngle != 180;
	if (lsm->isSpot) {
	    lsm->cosCutOffAngle =
		__GL_COSF(lss->spotLightCutOffAngle * __glDegreesToRadians);
	}

	if (lsm->isSpot && (!lsm->cache ||
	        (lsm->spotLightExponent != lss->spotLightExponent))) {
	    GLdouble exponent;

	    /*
	    ** Compute spot light exponent lookup table, but only when
	    ** the exponent changes value and the light is a spot light.
	    */
	    exponent = lsm->spotLightExponent = lss->spotLightExponent;

	    if (lsm->cache) {
		__glFreeSpecLUT(gc, lsm->cache);
	    }
	    lut = lsm->cache = __glCreateSpecLUT(gc, exponent);
	    lsm->threshold = lut->threshold;
	    lsm->scale = lut->scale;
	    lsm->spotTable = lut->table;
	}

	lsm->constantAttenuation = lss->constantAttenuation;
	if (lsm->constantAttenuation) {
	    lsm->attenuation = __glOne / lss->constantAttenuation;
	}
	lsm->linearAttenuation = lss->linearAttenuation;
	lsm->quadraticAttenuation = lss->quadraticAttenuation;

	/*
	** Pick per-light calculation proc based on the state
	** of the light source
	*/
	if (gc->modes.colorIndexMode) {
	    lsm->sli = ((__GLfloat) 0.30) * lss->specular.r
		+ ((__GLfloat) 0.59) * lss->specular.g
		+ ((__GLfloat) 0.11) * lss->specular.b;
	    lsm->dli = ((__GLfloat) 0.30) * lss->diffuse.r
		+ ((__GLfloat) 0.59) * lss->diffuse.g
		+ ((__GLfloat) 0.11) * lss->diffuse.b;
	}
	if (!gc->state.light.model.localViewer && !lsm->isSpot &&
	    gc->state.light.model.colorControl == GL_SINGLE_COLOR
		&& (lsm->position.w == zero)) {
	    __GLfloat hv[3];

	    /* Compute unit h[i] (normalized) */
	    (*gc->procs.normalize)(hv, &lsm->position.x);
	    lsm->unitVPpli.x = hv[0];
	    lsm->unitVPpli.y = hv[1];
	    lsm->unitVPpli.z = hv[2];
	    hv[2] += __glOne;
	    (*gc->procs.normalize)(&lsm->hHat.x, hv);
	    lsm->slowPath = GL_FALSE;
	} else {
	    lsm->slowPath = GL_TRUE;
	}
    }
    *lsmp = 0;
}

/*
** DEPENDENCIES:
**
** Procs:
**
** Light Model	LOCAL_VIEWER
** Lightx	SPOT_CUTOFF, POSITION
** Enables	LIGHTING
** modeFlags	CHEAP_FOG
*/
static void ComputeLightProcs(__GLcontext *gc)
{
    GLboolean anySlow = GL_FALSE;
    __GLlightSourceMachine *lsm;

    for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	if (lsm->slowPath) {
	    anySlow = GL_TRUE;
	    break;
	}
    }

    if (gc->state.enables.general & __GL_LIGHTING_ENABLE) {
	if (gc->modes.colorIndexMode) {
	    if (!anySlow) {
		gc->procs.calcColor = __glFastCalcCIColor;
	    } else {
		gc->procs.calcColor = __glCalcCIColor;
	    }
	} else {
	    if (!anySlow) {
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
		if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
		    gc->procs.calcColor = __glK3DFastCalcRGBColor;
		} else
#endif /* __GL_USE_K3DCODE */
		{
		    gc->procs.calcColor = __glFastCalcRGBColor;
		}
#else /* __GL_USE_ASMCODE && i386 */
		gc->procs.calcColor = __glFastCalcRGBColor;
#endif /* __GL_USE_ASMCODE && i386 */
	    } else {
		if ( gc->state.light.model.colorControl == GL_SINGLE_COLOR )
		    gc->procs.calcColor = __glCalcRGBColor;
		else {
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
		if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
		    gc->procs.calcColor = __glK3DCalcRGBColor;
		} else
#endif /* __GL_USE_K3DCODE */
		{
		    gc->procs.calcColor = __glI386CalcRGBColor;
		}
#else /* __GL_USE_ASMCODE && i386 */
		gc->procs.calcColor = __glCalcRGBColor;
#endif /* __GL_USE_ASMCODE && i386 */
		}
	    }
	}
	gc->procs.calcRasterColor = gc->procs.calcColor;
	if ((gc->polygon.shader.modeFlags & __GL_SHADE_CHEAP_FOG) &&
		(gc->polygon.shader.modeFlags & __GL_SHADE_SMOOTH_LIGHT) &&
		gc->renderMode == GL_RENDER) {
	    gc->procs.calcColor2 = gc->procs.calcColor;
	    if (gc->modes.colorIndexMode) {
		gc->procs.calcColor = FogLitCIColor;
	    } else {
		gc->procs.calcColor = FogLitRGBColor;
	    }
	}
    } else {
	gc->procs.calcRasterColor =
	    (void (*)(__GLcontext*, GLint, __GLvertex*)) __glNop3;
	if ((gc->polygon.shader.modeFlags & __GL_SHADE_CHEAP_FOG) &&
		(gc->polygon.shader.modeFlags & __GL_SHADE_SMOOTH_LIGHT) &&
		gc->renderMode == GL_RENDER) {
	    if (gc->modes.colorIndexMode) {
		gc->procs.calcColor = FogCIColor;
	    } else {
		gc->procs.calcColor = FogRGBColor;
	    }
	} else {
	    gc->procs.calcColor =
		(void (*)(__GLcontext*, GLint, __GLvertex*)) __glNop3;
	}
    }
}

/*
** DEPENDENCIES:
**
** Material	AMBIENT, DIFFUSE, SPECULAR, EMISSIVE
** Lightx	AMBIENT, DIFFUSE, SPECULAR
*/
static void ComputeLightMaterialState(__GLcontext *gc, GLint frontChange,
				      GLint backChange)
{
    __GLmaterialState *front, *back;
    __GLlightSourceMachine *lsm;
    __GLlightSourceState *lss;
    __GLfloat r, g, b;
    GLint allChange;
    __GLcolor basecolor;

    allChange = frontChange | backChange;
    if ((allChange & (__GL_MATERIAL_AMBIENT | __GL_MATERIAL_DIFFUSE | 
	    __GL_MATERIAL_SPECULAR | __GL_MATERIAL_EMISSIVE)) == 0) {
	return;
    }

    basecolor = gc->light.front.sceneColor;

    front = &gc->state.light.front;
    back = &gc->state.light.back;
    for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	lss = lsm->state;
	/*
	** Pre-multiply and the front & back ambient, diffuse and
	** specular colors
	*/
	if (allChange & __GL_MATERIAL_AMBIENT) {
	    r = lss->ambient.r;
	    g = lss->ambient.g;
	    b = lss->ambient.b;
	    if (frontChange & __GL_MATERIAL_AMBIENT) {
		lsm->front.ambient.r = front->ambient.r * r;
		lsm->front.ambient.g = front->ambient.g * g;
		lsm->front.ambient.b = front->ambient.b * b;

		basecolor.r += lsm->front.ambient.r;
		basecolor.g += lsm->front.ambient.g;
		basecolor.b += lsm->front.ambient.b;
	    }
	    if (backChange & __GL_MATERIAL_AMBIENT) {
		lsm->back.ambient.r = back->ambient.r * r;
		lsm->back.ambient.g = back->ambient.g * g;
		lsm->back.ambient.b = back->ambient.b * b;
	    }
	}

	if (allChange & __GL_MATERIAL_DIFFUSE) {
	    r = lss->diffuse.r;
	    g = lss->diffuse.g;
	    b = lss->diffuse.b;
	    if (frontChange & __GL_MATERIAL_DIFFUSE) {
		lsm->front.diffuse.r = front->diffuse.r * r;
		lsm->front.diffuse.g = front->diffuse.g * g;
		lsm->front.diffuse.b = front->diffuse.b * b;
	    }
	    if (backChange & __GL_MATERIAL_DIFFUSE) {
		lsm->back.diffuse.r = back->diffuse.r * r;
		lsm->back.diffuse.g = back->diffuse.g * g;
		lsm->back.diffuse.b = back->diffuse.b * b;
	    }
	}

	if (allChange & __GL_MATERIAL_SPECULAR) {
	    r = lss->specular.r;
	    g = lss->specular.g;
	    b = lss->specular.b;
	    if (frontChange & __GL_MATERIAL_SPECULAR) {
		lsm->front.specular.r = front->specular.r * r;
		lsm->front.specular.g = front->specular.g * g;
		lsm->front.specular.b = front->specular.b * b;
	    }
	    if (backChange & __GL_MATERIAL_SPECULAR) {
		lsm->back.specular.r = back->specular.r * r;
		lsm->back.specular.g = back->specular.g * g;
		lsm->back.specular.b = back->specular.b * b;
	    }
	}
    }

    if (frontChange & (__GL_MATERIAL_AMBIENT | __GL_MATERIAL_EMISSIVE)) {
	ClampScaledColorf(gc, &gc->light.baseColor, &basecolor.r);
    }
}


/*
** DEPENDENCIES:
**
** Material 	EMISSIVE, AMBIENT, DIFFUSE, SHININESS, SPECULAR
** Light Model 	AMBIENT
** Lightx	AMBIENT, DIFFUSE, SPECULAR
*/

/*
** Recompute light state based upon the material change indicated by 
** frontChange and backChange.
*/
void __glValidateMaterial(__GLcontext *gc, GLint frontChange, GLint backChange)
{
    ComputeMaterialState(gc, &gc->state.light.front, &gc->light.front, 
	    frontChange);
    ComputeMaterialState(gc, &gc->state.light.back, &gc->light.back, 
	    backChange);
    ComputeLightMaterialState(gc, frontChange, backChange);
}

/*
** DEPENDENCIES:
**
** Enables	LIGHTx, LIGHTING
** ( Material 	EMISSIVE, AMBIENT, DIFFUSE, SHININESS, SPECULAR )
** Light Model 	AMBIENT, LOCAL_VIEWER
** Lightx	DIFFUSE, AMBIENT, SPECULAR, POSITION, SPOT_EXPONENT, 
**		SPOT_CUTOFF, CONSTANT_ATTENUATION, LINEAR_ATTENUATION,
**		QUADRATIC_ATTENUATION
** modeFlags	CHEAP_FOG
*/

/*
** Pre-compute lighting state.
*/
void __glValidateLighting(__GLcontext *gc)
{
    if (gc->dirtyMask & __GL_DIRTY_LIGHTING) {
	ComputeLightState(gc);
	ComputeLightProcs(gc);
	__glValidateModelViewForLighting(gc);
	__glValidateMaterial(gc, __GL_MATERIAL_ALL, __GL_MATERIAL_ALL);
    } else {
	ComputeLightProcs(gc);
    }
}

void __glGenericPickColorMaterialProcs(__GLcontext *gc)
{
    if (gc->modes.rgbMode) {
	if (gc->state.enables.general & __GL_COLOR_MATERIAL_ENABLE) {
	    switch (gc->state.light.colorMaterialFace) {
	      case GL_FRONT_AND_BACK:
		gc->procs.applyColor = __glChangeBothMaterialColors;
		gc->light.cm = 0;
		gc->light.cmm = 0;
		break;
	      case GL_FRONT:
		gc->procs.applyColor = __glChangeOneMaterialColor;
		gc->light.cm = &gc->state.light.front;
		gc->light.cmm = &gc->light.front;
		break;
	      case GL_BACK:
		gc->procs.applyColor = __glChangeOneMaterialColor;
		gc->light.cm = &gc->state.light.back;
		gc->light.cmm = &gc->light.back;
		break;
	    }
	    switch (gc->state.light.colorMaterialParam) {
	      case GL_EMISSION:
		gc->procs.changeMaterial = ChangeMaterialEmission;
		break;
	      case GL_SPECULAR:
		gc->procs.changeMaterial = ChangeMaterialSpecular;
		break;
	      case GL_AMBIENT:
		gc->procs.changeMaterial = ChangeMaterialAmbient;
		break;
	      case GL_DIFFUSE:
		gc->procs.changeMaterial = ChangeMaterialDiffuse;
		break;
	      case GL_AMBIENT_AND_DIFFUSE:
		gc->procs.changeMaterial = ChangeMaterialAmbientAndDiffuse;
		break;
	    }
	} else {
	    if (gc->state.enables.general & __GL_LIGHTING_ENABLE) {
		/* When no color material, user color is not used */
		gc->procs.applyColor = (void (*)(__GLcontext*)) __glNop1;
	    } else {
		gc->procs.applyColor = __glClampAndScaleColor;
	    }
	}
    } else {
	/*
	** When in color index mode the value is copied from the
	** current.userColorIndex into the vertex
	*/
	gc->procs.applyColor = (void (*)(__GLcontext*)) __glNop1;
    }
}

void __glValidateModelViewForLighting(__GLcontext *gc)
{
    if (gc->state.enables.general & __GL_LIGHTING_ENABLE) {
	__GLtransform *tr = gc->transform.modelView; 
	__GLlightSourceMachine *lsm;

	if (tr->updateInverse) {
	    (*gc->procs.computeInverseTranspose)(gc, tr);
	}

	for (lsm = gc->light.sources; lsm; lsm = lsm->next) {
	    __glXForm4_Pre(&lsm->unitVPpliModel, &lsm->unitVPpli.x,
			   &tr->inverseTranspose);
	    __glXForm4_Pre(&lsm->hHatModel, &lsm->hHat.x,
			   &tr->inverseTranspose);
	}
    }
}
