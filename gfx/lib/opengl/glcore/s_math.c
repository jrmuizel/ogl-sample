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
*/
/*
** Mathematical subroutines needed by the GL.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/s_math.c#8 $
*/
#include "global.h"
#include "context.h"
#include "xform.h"
#include "glmath.h"
#include "g_imports.h"


/*
** Copy src to dst
*/
void __glCopyMatrix(__GLmatrix *dst, const __GLmatrix *src)
{
    dst->matrixType = src->matrixType;
    dst->matrix[0][0] = src->matrix[0][0];
    dst->matrix[0][1] = src->matrix[0][1];
    dst->matrix[0][2] = src->matrix[0][2];
    dst->matrix[0][3] = src->matrix[0][3];

    dst->matrix[1][0] = src->matrix[1][0];
    dst->matrix[1][1] = src->matrix[1][1];
    dst->matrix[1][2] = src->matrix[1][2];
    dst->matrix[1][3] = src->matrix[1][3];

    dst->matrix[2][0] = src->matrix[2][0];
    dst->matrix[2][1] = src->matrix[2][1];
    dst->matrix[2][2] = src->matrix[2][2];
    dst->matrix[2][3] = src->matrix[2][3];

    dst->matrix[3][0] = src->matrix[3][0];
    dst->matrix[3][1] = src->matrix[3][1];
    dst->matrix[3][2] = src->matrix[3][2];
    dst->matrix[3][3] = src->matrix[3][3];
}

/*
** Make m an identity matrix
*/
void __glMakeIdentity(__GLmatrix *m)
{
    __GLfloat zer = __glZero;
    __GLfloat one = ((__GLfloat) 1.0);;
    m->matrix[0][0] = one; m->matrix[0][1] = zer;
        m->matrix[0][2] = zer; m->matrix[0][3] = zer;
    m->matrix[1][0] = zer; m->matrix[1][1] = one;
        m->matrix[1][2] = zer; m->matrix[1][3] = zer;
    m->matrix[2][0] = zer; m->matrix[2][1] = zer;
        m->matrix[2][2] = one; m->matrix[2][3] = zer;
    m->matrix[3][0] = zer; m->matrix[3][1] = zer;
        m->matrix[3][2] = zer; m->matrix[3][3] = one;
    m->matrixType = __GL_MT_IDENTITY;
}

/*
** Compute r = a * b, where r can equal b.
*/
void __glMultMatrixNoOverwrite(__GLmatrix *r, const __GLmatrix *a, const __GLmatrix *b)
{
    GLint i;

    for (i = 0; i < 4; i++) {
	r->matrix[i][0] = a->matrix[i][0]*b->matrix[0][0] +
			  a->matrix[i][1]*b->matrix[1][0] +
			  a->matrix[i][2]*b->matrix[2][0] +
			  a->matrix[i][3]*b->matrix[3][0];

	r->matrix[i][1] = a->matrix[i][0]*b->matrix[0][1] +
			  a->matrix[i][1]*b->matrix[1][1] +
			  a->matrix[i][2]*b->matrix[2][1] +
			  a->matrix[i][3]*b->matrix[3][1];

	r->matrix[i][2] = a->matrix[i][0]*b->matrix[0][2] +
			  a->matrix[i][1]*b->matrix[1][2] +
			  a->matrix[i][2]*b->matrix[2][2] +
			  a->matrix[i][3]*b->matrix[3][2];

	r->matrix[i][3] = a->matrix[i][0]*b->matrix[0][3] +
			  a->matrix[i][1]*b->matrix[1][3] +
			  a->matrix[i][2]*b->matrix[2][3] +
			  a->matrix[i][3]*b->matrix[3][3];
    }
}

/*
** Compute r = a * b, where r can equal b.
*/
void __glMultMatrix(__GLmatrix *r, const __GLmatrix *a, const __GLmatrix *b)
{
    __GLfloat b00, b01, b02, b03;
    __GLfloat b10, b11, b12, b13;
    __GLfloat b20, b21, b22, b23;
    __GLfloat b30, b31, b32, b33;
    GLint i;

    b00 = b->matrix[0][0]; b01 = b->matrix[0][1];
        b02 = b->matrix[0][2]; b03 = b->matrix[0][3];
    b10 = b->matrix[1][0]; b11 = b->matrix[1][1];
        b12 = b->matrix[1][2]; b13 = b->matrix[1][3];
    b20 = b->matrix[2][0]; b21 = b->matrix[2][1];
        b22 = b->matrix[2][2]; b23 = b->matrix[2][3];
    b30 = b->matrix[3][0]; b31 = b->matrix[3][1];
        b32 = b->matrix[3][2]; b33 = b->matrix[3][3];

    for (i = 0; i < 4; i++) {
	r->matrix[i][0] = a->matrix[i][0]*b00 + a->matrix[i][1]*b10
	    + a->matrix[i][2]*b20 + a->matrix[i][3]*b30;
	r->matrix[i][1] = a->matrix[i][0]*b01 + a->matrix[i][1]*b11
	    + a->matrix[i][2]*b21 + a->matrix[i][3]*b31;
	r->matrix[i][2] = a->matrix[i][0]*b02 + a->matrix[i][1]*b12
	    + a->matrix[i][2]*b22 + a->matrix[i][3]*b32;
	r->matrix[i][3] = a->matrix[i][0]*b03 + a->matrix[i][1]*b13
	    + a->matrix[i][2]*b23 + a->matrix[i][3]*b33;
    }
}

#if !(defined(mips) && defined(__GL_USE_ASMCODE))
/*
** Normalize v into vout.
*/
void __glNormalize(__GLfloat vout[3], const __GLfloat v[3])
{
    __GLfloat len, zero = __glZero;

    len = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (len <= zero) {
	vout[0] = zero;
	vout[1] = zero;
	vout[2] = zero;
	return;
    }
    if (len == 1.0F) {
	vout[0] = v[0];
	vout[1] = v[1];
	vout[2] = v[2];
	return;
    }
    else {
#if 1
	union {
	    unsigned int i;
	    float f;
	} seed;
	float xy, subexp;

	/*
	** This code calculates a reciprocal square root accurate to well over
	** 16 bits using Newton-Raphson approximation.
	**
	** To calculate the seed, the shift compresses the floating-point
	** range just as sqrt() does, and the subtract inverts the range
	** like reciprocation does.  The constant was chosen by trial-and-error
	** to minimize the maximum error of the iterated result for all values
	** over the range .5 to 2.
	*/
	seed.f = len;
	seed.i = 0x5f375a00u - (seed.i >> 1);

	/*
	** The Newton-Raphson iteration to approximate X = 1/sqrt(Y) is:
	**
	**	X[1] = .5*X[0]*(3 - Y*X[0]^2)
	**
	** A double iteration is:
	**
	**	X[2] = .0625*X[0]*(3 - Y*X[0]^2)*[12 - (Y*X[0]^2)*(3 - Y*X[0]^2)^2]
	**
	*/
	xy = len * seed.f * seed.f;
	subexp = 3.f - xy;
	len = .0625f * seed.f * subexp * (12.f - xy * subexp * subexp);
#else
	len = ((__GLfloat) 1.0) / __GL_SQRTF(len);
#endif
	vout[0] = v[0] * len;
	vout[1] = v[1] * len;
	vout[2] = v[2] * len;
	return;
    }
}
#endif /* !(mips && __GL_USE_ASMCODE) */

/*
** Normalize v into vout.
**   voa = normalize(v)
**   vob = normalize(voa + (0 0 1))
*/
void __glNormalizeTwo(const __GLfloat v[3], __GLfloat voa[3], __GLfloat vob[3])
{
    __GLfloat len, voaZ;

    len = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (len <= __glZero) {
	voa[0] = __glZero;
	voa[1] = __glZero;
	voa[2] = __glZero;
	vob[0] = __glZero;
	vob[1] = __glZero;
	vob[2] = 1.0F;
	return;
    }
    if (len == 1.0F) {
	voa[0] = v[0];
	voa[1] = v[1];
	voa[2] = v[2];
    }
    else {
#if 0
	union {
	    unsigned int i;
	    float f;
	} seed;
	float xy, subexp;

	seed.f = len;
	seed.i = 0x5f375a00u - (seed.i >> 1);

	xy = len * seed.f * seed.f;
	subexp = 3.f - xy;
	len = .0625f * seed.f * subexp * (12.f - xy * subexp * subexp);
#else
	len = ((__GLfloat) 1.0) / __GL_SQRTF(len);
#endif
	voa[0] = v[0] * len;
	voa[1] = v[1] * len;
	voa[2] = v[2] * len;
    }

    voaZ = voa[2] + 1.0F;
    len = 2.0 * voaZ;
    if (len <= __glZero) {
	vob[0] = __glZero;
	vob[1] = __glZero;
	vob[2] = __glZero;
	return;
    }
    if (len == 1.0F) {
	vob[0] = voa[0];
	vob[1] = voa[1];
	vob[2] = voaZ;
    }
    else {
#if 0
	union {
	    unsigned int i;
	    float f;
	} seed;
	float xy, subexp;

	seed.f = len;
	seed.i = 0x5f375a00u - (seed.i >> 1);

	xy = len * seed.f * seed.f;
	subexp = 3.f - xy;
	len = .0625f * seed.f * subexp * (12.f - xy * subexp * subexp);
#else
	len = ((__GLfloat) 1.0) / __GL_SQRTF(len);
#endif
	vob[0] = voa[0] * len;
	vob[1] = voa[1] * len;
	vob[2] = voaZ * len;
    }
}


/*
**  Scale a vector by a scalar constant. (Used presently for rescaling
**    normal vectors when GL_RESCALE_NORMAL is enabled).
*/

void __glScaleVector(__GLfloat vout[3], const __GLfloat c,
                     const __GLfloat vin[3] )
{
    vout[0] = c * vin[0];
    vout[1] = c * vin[1];
    vout[2] = c * vin[2];
}


/*
** inverse = invert(transpose(src))

This code uses Cramer's Rule to calculate the matrix inverse.
In general, the inverse transpose has this form:

[          ] -t    [                                   ]
[          ]       [             -t             -t t   ]
[  Q    P  ]       [   S(SQ - PT)     -(SQ - PT)  T    ]
[          ]       [                                   ]
[          ]       [                                   ]
[          ]    =  [                                   ]
[          ]       [        -1  t                      ]
[          ]       [     -(Q  P)             1         ]
[  T    S  ]       [   -------------   -------------   ]
[          ]       [         -1  t t         -1  t t   ]
[          ]       [   S - (Q  P) T    S - (Q  P) T    ]

But in the usual case that P,S == [0, 0, 0, 1], this is enough:

[          ] -t    [                                   ]
[          ]       [         -t              -t t      ]
[  Q    0  ]       [        Q              -Q  T       ]
[          ]       [                                   ]
[          ]       [                                   ]
[          ]    =  [                                   ]
[          ]       [                                   ]
[          ]       [                                   ]
[  T    1  ]       [        0                1         ]
[          ]       [                                   ]
[          ]       [                                   ]

*/
void __glInvertTransposeMatrix(__GLmatrix *inverse, const __GLmatrix *src)
{
    __GLfloat x00, x01, x02;
    __GLfloat x10, x11, x12;
    __GLfloat x20, x21, x22;
    __GLfloat rcp;

    /* propagate matrix type & branch if general */
    inverse->matrixType = src->matrixType;
    if(inverse->matrixType) {
	__GLfloat z00, z01, z02;
	__GLfloat z10, z11, z12;
	__GLfloat z20, z21, z22;

	/* read 3x3 matrix into registers */
	x00 = src->matrix[0][0];
	x01 = src->matrix[0][1];
	x02 = src->matrix[0][2];
	x10 = src->matrix[1][0];
	x11 = src->matrix[1][1];
	x12 = src->matrix[1][2];
	x20 = src->matrix[2][0];
	x21 = src->matrix[2][1];
	x22 = src->matrix[2][2];

	/* compute first three 2x2 cofactors */
	z20 = x01*x12 - x11*x02;
	z10 = x21*x02 - x01*x22;
	z00 = x11*x22 - x12*x21;

	/* compute 3x3 determinant & its reciprocal */
	rcp = x20*z20 + x10*z10 + x00*z00;
	if (rcp == (float)0)
	    return;
	rcp = (float)1/rcp;

	/* compute other six 2x2 cofactors */
	z01 = x20*x12 - x10*x22;
	z02 = x10*x21 - x20*x11;
	z11 = x00*x22 - x20*x02;
	z12 = x20*x01 - x00*x21;
	z21 = x10*x02 - x00*x12;
	z22 = x00*x11 - x10*x01;

	/* multiply all cofactors by reciprocal */
	inverse->matrix[0][0] = z00*rcp;
	inverse->matrix[0][1] = z01*rcp;
	inverse->matrix[0][2] = z02*rcp;
	inverse->matrix[1][0] = z10*rcp;
	inverse->matrix[1][1] = z11*rcp;
	inverse->matrix[1][2] = z12*rcp;
	inverse->matrix[2][0] = z20*rcp;
	inverse->matrix[2][1] = z21*rcp;
	inverse->matrix[2][2] = z22*rcp;

	/* read translation vector & negate */
	x00 = -src->matrix[3][0];
	x01 = -src->matrix[3][1];
	x02 = -src->matrix[3][2];

	/* store bottom row of inverse transpose */
	inverse->matrix[3][0] = 0;
	inverse->matrix[3][1] = 0;
	inverse->matrix[3][2] = 0;
	inverse->matrix[3][3] = 1;

	/* finish by tranforming translation vector */
	inverse->matrix[0][3] = inverse->matrix[0][0]*x00 +
	    inverse->matrix[0][1]*x01 +
	    inverse->matrix[0][2]*x02;
	inverse->matrix[1][3] = inverse->matrix[1][0]*x00 +
	    inverse->matrix[1][1]*x01 +
	    inverse->matrix[1][2]*x02;
	inverse->matrix[2][3] = inverse->matrix[2][0]*x00 +
	    inverse->matrix[2][1]*x01 +
	    inverse->matrix[2][2]*x02;
    } else {
	__GLfloat x30, x31, x32;
	__GLfloat y01, y02, y03, y12, y13, y23;
	__GLfloat z02, z03, z12, z13, z22, z23, z32, z33;

#define x03 x01
#define x13 x11
#define x23 x21
#define x33 x31
#define z00 x02
#define z10 x12
#define z20 x22
#define z30 x32
#define z01 x03
#define z11 x13
#define z21 x23
#define z31 x33

	/* read 1st two columns of matrix into registers */
	x00 = src->matrix[0][0];
	x01 = src->matrix[0][1];
	x10 = src->matrix[1][0];
	x11 = src->matrix[1][1];
	x20 = src->matrix[2][0];
	x21 = src->matrix[2][1];
	x30 = src->matrix[3][0];
	x31 = src->matrix[3][1];

	/* compute all six 2x2 determinants of 1st two columns */
	y01 = x00*x11 - x10*x01;
	y02 = x00*x21 - x20*x01;
	y03 = x00*x31 - x30*x01;
	y12 = x10*x21 - x20*x11;
	y13 = x10*x31 - x30*x11;
	y23 = x20*x31 - x30*x21;

	/* read 2nd two columns of matrix into registers */
	x02 = src->matrix[0][2];
	x03 = src->matrix[0][3];
	x12 = src->matrix[1][2];
	x13 = src->matrix[1][3];
	x22 = src->matrix[2][2];
	x23 = src->matrix[2][3];
	x32 = src->matrix[3][2];
	x33 = src->matrix[3][3];

	/* compute all 3x3 cofactors for 2nd two columns */
	z33 = x02*y12 - x12*y02 + x22*y01;
	z23 = x12*y03 - x32*y01 - x02*y13;
	z13 = x02*y23 - x22*y03 + x32*y02;
	z03 = x22*y13 - x32*y12 - x12*y23;
	z32 = x13*y02 - x23*y01 - x03*y12;
	z22 = x03*y13 - x13*y03 + x33*y01;
	z12 = x23*y03 - x33*y02 - x03*y23;
	z02 = x13*y23 - x23*y13 + x33*y12;

	/* compute all six 2x2 determinants of 2nd two columns */
	y01 = x02*x13 - x12*x03;
	y02 = x02*x23 - x22*x03;
	y03 = x02*x33 - x32*x03;
	y12 = x12*x23 - x22*x13;
	y13 = x12*x33 - x32*x13;
	y23 = x22*x33 - x32*x23;

	/* read 1st two columns of matrix into registers */
	x00 = src->matrix[0][0];
	x01 = src->matrix[0][1];
	x10 = src->matrix[1][0];
	x11 = src->matrix[1][1];
	x20 = src->matrix[2][0];
	x21 = src->matrix[2][1];
	x30 = src->matrix[3][0];
	x31 = src->matrix[3][1];

	/* compute all 3x3 cofactors for 1st column */
	z30 = x11*y02 - x21*y01 - x01*y12;
	z20 = x01*y13 - x11*y03 + x31*y01;
	z10 = x21*y03 - x31*y02 - x01*y23;
	z00 = x11*y23 - x21*y13 + x31*y12;

	/* compute 4x4 determinant & its reciprocal */
	rcp = x30*z30 + x20*z20 + x10*z10 + x00*z00;
	if (rcp == (float)0)
	    return;
	rcp = (float)1/rcp;

	/* compute all 3x3 cofactors for 2nd column */
	z31 = x00*y12 - x10*y02 + x20*y01;
	z21 = x10*y03 - x30*y01 - x00*y13;
	z11 = x00*y23 - x20*y03 + x30*y02;
	z01 = x20*y13 - x30*y12 - x10*y23;

	/* multiply all 3x3 cofactors by reciprocal */
	inverse->matrix[0][0] = z00*rcp;
	inverse->matrix[0][1] = z01*rcp;
	inverse->matrix[1][0] = z10*rcp;
	inverse->matrix[0][2] = z02*rcp;
	inverse->matrix[2][0] = z20*rcp;
	inverse->matrix[0][3] = z03*rcp;
	inverse->matrix[3][0] = z30*rcp;
	inverse->matrix[1][1] = z11*rcp;
	inverse->matrix[1][2] = z12*rcp;
	inverse->matrix[2][1] = z21*rcp;
	inverse->matrix[1][3] = z13*rcp;
	inverse->matrix[3][1] = z31*rcp;
	inverse->matrix[2][2] = z22*rcp;
	inverse->matrix[2][3] = z23*rcp;
	inverse->matrix[3][2] = z32*rcp;
	inverse->matrix[3][3] = z33*rcp;
    }
}

void __glTransposeMatrix(__GLmatrix *transpose, const __GLmatrix *src)
{
    transpose->matrix[0][0] = src->matrix[0][0];
    transpose->matrix[0][1] = src->matrix[1][0];
    transpose->matrix[0][2] = src->matrix[2][0];
    transpose->matrix[0][3] = src->matrix[3][0];
    transpose->matrix[1][0] = src->matrix[0][1];
    transpose->matrix[1][1] = src->matrix[1][1];
    transpose->matrix[1][2] = src->matrix[2][1];
    transpose->matrix[1][3] = src->matrix[3][1];
    transpose->matrix[2][0] = src->matrix[0][2];
    transpose->matrix[2][1] = src->matrix[1][2];
    transpose->matrix[2][2] = src->matrix[2][2];
    transpose->matrix[2][3] = src->matrix[3][2];
    transpose->matrix[3][0] = src->matrix[0][3];
    transpose->matrix[3][1] = src->matrix[1][3];
    transpose->matrix[3][2] = src->matrix[2][3];
    transpose->matrix[3][3] = src->matrix[3][3];
    
}

/*
 * Compute the floor of the log base 2 of a unsigned integer (used mostly
 *  for computing log2(2^n)).
 */
GLuint
__glFloorLog2(GLuint n)
{
   int i = 1;

   while ((n >> i) > 0)
       i++;
   return i-1;
}

/*
 * floor outside of libc.
 */
__GLfloat
__glFloor(__GLfloat f)
{
    __GLfloat retf;

    retf = (int)f;
    if (f < 0.0f) {
	retf -= 1.0f;
    }

    return retf;
}
