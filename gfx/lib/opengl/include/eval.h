#ifndef	__gl_evaluator_h_
#define	__gl_evaluator_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/eval.h#6 $
*/
#include "types.h"

/* XXX Can this be changed? */
#define __GL_MAX_ORDER		40

/* Number of maps */
#define __GL_MAP_RANGE_COUNT	9

#define __GL_EVAL1D_INDEX(old)		((old) - GL_MAP1_COLOR_4)
#define __GL_EVAL2D_INDEX(old)		((old) - GL_MAP2_COLOR_4)

/* internal form of map range indexes */
#define __GL_C4		__GL_EVAL1D_INDEX(GL_MAP1_COLOR_4)
#define __GL_I		__GL_EVAL1D_INDEX(GL_MAP1_INDEX)
#define __GL_N3		__GL_EVAL1D_INDEX(GL_MAP1_NORMAL)
#define __GL_T1		__GL_EVAL1D_INDEX(GL_MAP1_TEXTURE_COORD_1)
#define __GL_T2		__GL_EVAL1D_INDEX(GL_MAP1_TEXTURE_COORD_2)
#define __GL_T3		__GL_EVAL1D_INDEX(GL_MAP1_TEXTURE_COORD_3)
#define __GL_T4		__GL_EVAL1D_INDEX(GL_MAP1_TEXTURE_COORD_4)
#define __GL_V3		__GL_EVAL1D_INDEX(GL_MAP1_VERTEX_3)
#define __GL_V4		__GL_EVAL1D_INDEX(GL_MAP1_VERTEX_4)

typedef struct {
    /*
    ** not strictly necessary since it can be inferred from the index,
    ** but it makes the code simpler.
    */
    GLint k;		

    /*
    ** Order of the polynomial + 1
    */
    GLint order;

    __GLfloat u1, u2;
} __GLevaluator1;

typedef struct {
    GLint k;
    GLint majorOrder, minorOrder;
    __GLfloat u1, u2;
    __GLfloat v1, v2;
} __GLevaluator2;

typedef struct {
    __GLfloat start;
    __GLfloat finish;
    __GLfloat step;
    GLint n;
} __GLevaluatorGrid;

typedef struct {
    __GLevaluatorGrid u1, u2, v2;
} __GLevaluatorState;

typedef struct {
    __GLevaluator1 eval1[__GL_MAP_RANGE_COUNT];
    __GLevaluator2 eval2[__GL_MAP_RANGE_COUNT];

    __GLfloat *eval1Data[__GL_MAP_RANGE_COUNT];
    __GLfloat *eval2Data[__GL_MAP_RANGE_COUNT];

    __GLfloat uvalue;
    __GLfloat vvalue;
    __GLfloat ucoeff[__GL_MAX_ORDER];
    __GLfloat vcoeff[__GL_MAX_ORDER];
    __GLfloat ucoeffDeriv[__GL_MAX_ORDER];
    __GLfloat vcoeffDeriv[__GL_MAX_ORDER];
    GLint uorder;
    GLint vorder;
    GLint utype;
    GLint vtype;
} __GLevaluatorMachine;

extern void __glCopyEvaluatorState(__GLcontext *gc, __GLattribute *dst,
				   const __GLattribute *src);

extern GLint __glEvalComputeK(GLenum target);

extern void __glFillMap1f(GLint k, GLint order, GLint stride,
			  const GLfloat *points, __GLfloat *data);
extern void __glFillMap1d(GLint k, GLint order, GLint stride,
			  const GLdouble *points, __GLfloat *data);
extern void __glFillMap2f(GLint k, GLint majorOrder, GLint minorOrder,
			  GLint majorStride, GLint minorStride,
			  const GLfloat *points, __GLfloat *data);
extern void __glFillMap2d(GLint k, GLint majorOrder, GLint minorOrder,
			  GLint majorStride, GLint minorStride,
			  const GLdouble *points, __GLfloat *data);

extern GLint __glMap1_size(GLint k, GLint order);
extern GLint __glMap2_size(GLint k, GLint majorOrder, GLint minorOrder);


extern __GLevaluator1 *__glSetUpMap1(__GLcontext *gc, GLenum type,
				     GLint order, __GLfloat u1, __GLfloat u2);
extern __GLevaluator2 *__glSetUpMap2(__GLcontext *gc, GLenum type,
				     GLint majorOrder, GLint minorOrder,
				     __GLfloat u1, __GLfloat u2,
				     __GLfloat v1, __GLfloat v2);

extern void __glDoEvalCoord1(__GLcontext *gc, __GLfloat u);
extern void __glDoEvalCoord2(__GLcontext *gc, __GLfloat u, __GLfloat v);

extern void __glEvalMesh1Line(__GLcontext *gc, GLint low, GLint high);
extern void __glEvalMesh1Point(__GLcontext *gc, GLint low, GLint high);
extern void __glEvalMesh2Fill(__GLcontext *gc, GLint lowU, GLint lowV,
			      GLint highU, GLint highV);
extern void __glEvalMesh2Line(__GLcontext *gc, GLint lowU, GLint lowV,
			      GLint highU, GLint highV);
extern void __glEvalMesh2Point(__GLcontext *gc, GLint lowU, GLint lowV,
			       GLint highU, GLint highV);

#endif /* __gl_evaluator_h_ */
