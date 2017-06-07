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
** Transformation procedures.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/s_xform.c#16 $
*/
#include "context.h"
#include "g_imports.h"
#include "global.h"
#include "g_imfncs.h"

#if defined(__GL_USE_ASMCODE)
#if defined(mips)
#include "mips.h"
#endif /* mips */
#if defined(i386)
#include "i386.h"
#if defined(__GL_USE_K3DCODE)
#include "i386/k3d.h"
#endif /* __GL_USE_K3DCODE */
#endif /* i386 */
#endif /* __GL_USE_ASMCODE */

/*
** Assuming that a->matrixType and b->matrixType are already correct,
** and dst = a * b, then compute dst's matrix type.
*/
void __glPickMatrixType(__GLmatrix *dst, __GLmatrix *a, __GLmatrix *b)
{
    switch(a->matrixType) {
      case __GL_MT_GENERAL:
	dst->matrixType = a->matrixType;
	break;
      case __GL_MT_W0001:
	if (b->matrixType == __GL_MT_GENERAL) {
	    dst->matrixType = b->matrixType;
	} else {
	    dst->matrixType = a->matrixType;
	}
	break;
      case __GL_MT_IS2D:
	if (b->matrixType < __GL_MT_IS2D) {
	    dst->matrixType = b->matrixType;
	} else {
	    dst->matrixType = a->matrixType;
	}
        break;
      case __GL_MT_IS2DNR:
	if (b->matrixType < __GL_MT_IS2DNR) {
	    dst->matrixType = b->matrixType;
	} else {
	    dst->matrixType = a->matrixType;
	}
        break;
      case __GL_MT_IDENTITY:
	if (b->matrixType == __GL_MT_IS2DNRSC) {
	    dst->width = b->width;
	    dst->height = b->height;
	}
	dst->matrixType = b->matrixType;
	break;
      case __GL_MT_IS2DNRSC:
	if (b->matrixType == __GL_MT_IDENTITY) {
	    dst->matrixType = __GL_MT_IS2DNRSC;
	    dst->width = a->width;
	    dst->height = a->height;
	} else if (b->matrixType < __GL_MT_IS2DNR) {
	    dst->matrixType = b->matrixType;
	} else {
	    dst->matrixType = __GL_MT_IS2DNR;
	}
	break;
    }
}

/*
** Muliply the first matrix by the second one keeping track of the matrix
** type of the newly combined matrix.
*/
void __glMultiplyMatrix(__GLcontext *gc, __GLmatrix *m, void *data)
{
    __GLmatrix *tm;

    tm = data;
    (*gc->procs.matrix.mult)(m, tm, m);
    __glPickMatrixType(m, tm, m);
}

static void SetDepthRange(__GLcontext *gc, double zNear, double zFar)
{
    __GLviewport *vp = &gc->state.viewport;
    double scale, zero = __glZero, one = __glOne;

    /* Clamp depth range to legal values */
    if (zNear < zero) zNear = zero;
    if (zNear > one) zNear = one;
    if (zFar < zero) zFar = zero;
    if (zFar > one) zFar = one;
    vp->zNear = zNear;
    vp->zFar = zFar;

    if (gc->modes.haveDepthBuffer) {
	__GL_LOCK_DP(gc->drawablePrivate, gc);
	if (__GL_DEPTH_DP(&gc->depthBuffer)->invertZRange) {
	    zNear = one - zNear;
	    zFar = one - zFar;
	}
	__GL_UNLOCK_DP(gc->drawablePrivate);
    }

    /* Compute viewport values for the new depth range */
    scale = gc->constants.depthScale * __glHalf;
    gc->state.viewport.zScale =	(zFar - zNear) * scale;
    gc->state.viewport.zCenter = (zFar + zNear) * scale;
}

void __glUpdateDepthRange(__GLcontext *gc)
{
    __GLviewport *vp = &gc->state.viewport;

    SetDepthRange(gc, vp->zNear, vp->zFar);
}

void __glEarlyInitTransformState(__GLcontext *gc)
{
    GLint numClipPlanes;

    /* Allocate memory for clip planes */
    numClipPlanes = gc->constants.numberOfClipPlanes;
    gc->state.transform.eyeClipPlanes = (__GLcoord *)
	(*gc->imports.calloc)(gc, (size_t) numClipPlanes, sizeof(__GLcoord));
}

void __glInitTransformState(__GLcontext *gc)
{
    GLint i, numClipPlanes;
    __GLtransform *tr;
    __GLvertex *vx;

    numClipPlanes = gc->constants.numberOfClipPlanes;

    /*
    ** Memory for clip planes allocated in __glEarlyInitTranformState
    ** in case a copy context occurs before MakeCurrent.
    */

    /* Allocate memory for matrix stacks */
    gc->transform.modelViewStack = (__GLtransform *)
	(*gc->imports.calloc)(gc, (size_t)gc->constants.maxModelViewStackDepth,
			      sizeof(__GLtransform));
    gc->transform.projectionStack = (__GLtransform *)
	(*gc->imports.calloc)(gc, (size_t)gc->constants.maxProjectionStackDepth,
			      sizeof(__GLtransform));
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	gc->transform.textureStack[i] = (__GLtransform *)
	    (*gc->imports.calloc)(gc,
	    			  (size_t)gc->constants.maxTextureStackDepth,
				  sizeof(__GLtransform));
    }
    gc->transform.colorStack = (__GLtransform *)
	(*gc->imports.calloc)(gc, (size_t) gc->constants.maxColorStackDepth,
			      sizeof(__GLtransform));

    /* 
    ** Allocate memory for clipping temporaries.
    ** Each plane can potentially add two temporary vertices, even though
    ** the overall number of vertices can only increase by one per plane.
    */
    gc->transform.clipTemp = (__GLvertex*)
	(*gc->imports.calloc)(gc, (size_t) 2*(6 + numClipPlanes),
			      sizeof(__GLvertex));

    gc->state.transform.matrixMode = GL_MODELVIEW;
    SetDepthRange(gc, __glZero, __glOne);

    gc->transform.modelView = tr = &gc->transform.modelViewStack[0];
    (*gc->procs.matrix.makeIdentity)(&tr->matrix);
    (*gc->procs.matrix.makeIdentity)(&tr->inverseTranspose);
    (*gc->procs.matrix.makeIdentity)(&tr->mvp);
    (*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
    (*gc->procs.pickInvTransposeProcs)(gc, &tr->inverseTranspose);
    tr->updateInverse = GL_FALSE;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_MODELVIEW);

    gc->transform.projection = tr = &gc->transform.projectionStack[0];
    (*gc->procs.matrix.makeIdentity)(&tr->matrix);
    (*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
    (*gc->procs.pickMvpMatrixProcs)(gc, &gc->transform.modelView->mvp);

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	gc->transform.texture[i] = tr = &gc->transform.textureStack[i][0];
	(*gc->procs.matrix.makeIdentity)(&tr->matrix);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
    }

    gc->transform.color = tr = &gc->transform.colorStack[0];
    (*gc->procs.matrix.makeIdentity)(&tr->matrix);
    (*gc->procs.pickMatrixProcs)(gc, &tr->matrix);

    gc->state.transform.eyePos.z = __glOne;
    gc->state.transform.eyePosObj.z = __glOne;
    gc->state.transform.objEyeSpecified = GL_FALSE;

    vx = &gc->transform.clipTemp[0];
    for (i = 0; i < 2*(6 + numClipPlanes); i++, vx++) {/*XXX*/
	vx->color = &vx->colors[__GL_FRONTFACE];
	vx->flagBits = (vx->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
	    __GL_VERTEX_4D_SIZE;
    }

    gc->state.current.normal.z = __glOne;
}

void __glFreeTransformState(__GLcontext *gc)
{
    GLint i;

    /*
    ** Free state allocated during __glEarlyInitTransformState()
    */
    if (gc->state.transform.eyeClipPlanes)
    	(*gc->imports.free)(gc, gc->state.transform.eyeClipPlanes);

    /*
    ** Free state allocated during __glInitTransformState()
    */
    if (gc->transform.modelViewStack)
    	(*gc->imports.free)(gc, gc->transform.modelViewStack);
    if (gc->transform.projectionStack)
    	(*gc->imports.free)(gc, gc->transform.projectionStack);
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	if(gc->transform.textureStack[i])
		(*gc->imports.free)(gc, gc->transform.textureStack[i]);
    }
    if (gc->transform.colorStack)
    	(*gc->imports.free)(gc, gc->transform.colorStack);
    if (gc->transform.clipTemp)
    	(*gc->imports.free)(gc, gc->transform.clipTemp);
}

/*
** An amazing thing has happened.  More than 2^32 changes to the projection
** matrix has occured.  Run through the modelView and projection stacks
** and reset the sequence numbers to force a revalidate on next usage.
*/
void __glInvalidateSequenceNumbers(__GLcontext *gc)
{
    __GLtransform *tr, *lasttr;
    GLuint s;

    /* Make all mvp matricies refer to sequence number zero */
    s = 0;
    tr = &gc->transform.modelViewStack[0];
    lasttr = tr + gc->constants.maxModelViewStackDepth;
    while (tr < lasttr) {
	tr->sequence = s;
	tr++;
    }

    /* Make all projection matricies sequence up starting at one */
    s = 1;
    tr = &gc->transform.projectionStack[0];
    lasttr = tr + gc->constants.maxProjectionStackDepth;
    while (tr < lasttr) {
	tr->sequence = s++;
	tr++;
    }
    gc->transform.projectionSequence = s;
}

/************************************************************************/

void GLAPI __glim_MatrixMode(GLenum mode)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    switch (mode) {
      case GL_MODELVIEW:
      case GL_PROJECTION:
      case GL_TEXTURE:
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
      case GL_COLOR:
#endif
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    gc->state.transform.matrixMode = mode;
    (*gc->procs.pickTransformProcs)(gc);
}

void GLAPI __glim_LoadIdentity(void)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();
    (*gc->procs.loadIdentity)(gc);
}

void GLAPI __glim_LoadMatrixf(const GLfloat m[16])
{
    __GLmatrix m1;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    m1.matrix[0][0] = m[0];
    m1.matrix[0][1] = m[1];
    m1.matrix[0][2] = m[2];
    m1.matrix[0][3] = m[3];
    m1.matrix[1][0] = m[4];
    m1.matrix[1][1] = m[5];
    m1.matrix[1][2] = m[6];
    m1.matrix[1][3] = m[7];
    m1.matrix[2][0] = m[8];
    m1.matrix[2][1] = m[9];
    m1.matrix[2][2] = m[10];
    m1.matrix[2][3] = m[11];
    m1.matrix[3][0] = m[12];
    m1.matrix[3][1] = m[13];
    m1.matrix[3][2] = m[14];
    m1.matrix[3][3] = m[15];
    m1.matrixType = __GL_MT_GENERAL;
    __glDoLoadMatrix(gc, &m1);
}

void GLAPI __glim_LoadMatrixd(const GLdouble m[16])
{
    __GLmatrix m1;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    m1.matrix[0][0] = m[0];
    m1.matrix[0][1] = m[1];
    m1.matrix[0][2] = m[2];
    m1.matrix[0][3] = m[3];
    m1.matrix[1][0] = m[4];
    m1.matrix[1][1] = m[5];
    m1.matrix[1][2] = m[6];
    m1.matrix[1][3] = m[7];
    m1.matrix[2][0] = m[8];
    m1.matrix[2][1] = m[9];
    m1.matrix[2][2] = m[10];
    m1.matrix[2][3] = m[11];
    m1.matrix[3][0] = m[12];
    m1.matrix[3][1] = m[13];
    m1.matrix[3][2] = m[14];
    m1.matrix[3][3] = m[15];
    m1.matrixType = __GL_MT_GENERAL;
    __glDoLoadMatrix(gc, &m1);
}

void GLAPI __glim_MultMatrixf(const GLfloat m[16])
{
    __GLmatrix m1;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    m1.matrix[0][0] = m[0];
    m1.matrix[0][1] = m[1];
    m1.matrix[0][2] = m[2];
    m1.matrix[0][3] = m[3];
    m1.matrix[1][0] = m[4];
    m1.matrix[1][1] = m[5];
    m1.matrix[1][2] = m[6];
    m1.matrix[1][3] = m[7];
    m1.matrix[2][0] = m[8];
    m1.matrix[2][1] = m[9];
    m1.matrix[2][2] = m[10];
    m1.matrix[2][3] = m[11];
    m1.matrix[3][0] = m[12];
    m1.matrix[3][1] = m[13];
    m1.matrix[3][2] = m[14];
    m1.matrix[3][3] = m[15];
    m1.matrixType = __GL_MT_GENERAL;
    __glDoMultMatrix(gc, &m1, __glMultiplyMatrix);
}

void GLAPI __glim_MultMatrixd(const GLdouble m[16])
{
    __GLmatrix m1;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    m1.matrix[0][0] = m[0];
    m1.matrix[0][1] = m[1];
    m1.matrix[0][2] = m[2];
    m1.matrix[0][3] = m[3];
    m1.matrix[1][0] = m[4];
    m1.matrix[1][1] = m[5];
    m1.matrix[1][2] = m[6];
    m1.matrix[1][3] = m[7];
    m1.matrix[2][0] = m[8];
    m1.matrix[2][1] = m[9];
    m1.matrix[2][2] = m[10];
    m1.matrix[2][3] = m[11];
    m1.matrix[3][0] = m[12];
    m1.matrix[3][1] = m[13];
    m1.matrix[3][2] = m[14];
    m1.matrix[3][3] = m[15];
    m1.matrixType = __GL_MT_GENERAL;
    __glDoMultMatrix(gc, &m1, __glMultiplyMatrix);
}

void GLAPI __glim_Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    __glDoRotate(gc, angle, x, y, z);
}

void GLAPI __glim_Rotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    __glDoRotate(gc, angle, x, y, z);
}

void GLAPI __glim_Scalef(GLfloat x, GLfloat y, GLfloat z)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    __glDoScale(gc, x, y, z);
}

void GLAPI __glim_Scaled(GLdouble x, GLdouble y, GLdouble z)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    __glDoScale(gc, x, y, z);
}

void GLAPI __glim_Translatef(GLfloat x, GLfloat y, GLfloat z)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    __glDoTranslate(gc, x, y, z);
}

void GLAPI __glim_Translated(GLdouble x, GLdouble y, GLdouble z)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    __glDoTranslate(gc, x, y, z);
}

void GLAPI __glim_PushMatrix(void)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();
    (*gc->procs.pushMatrix)(gc);
}

void GLAPI __glim_PopMatrix(void)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();
    (*gc->procs.popMatrix)(gc);
}

void GLAPI __glim_Frustum(GLdouble left, GLdouble right,
			  GLdouble bottom, GLdouble top,
			  GLdouble zNear, GLdouble zFar)
{
    __GLmatrix m;
    __GLfloat deltaX, deltaY, deltaZ;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    deltaX = right - left;
    deltaY = top - bottom;
    deltaZ = zFar - zNear;
    if ((zNear <= __glZero) || (zFar <= __glZero) || (deltaX == __glZero) || 
	    (deltaY == __glZero) || (deltaZ == __glZero)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    (*gc->procs.matrix.makeIdentity)(&m);
    m.matrix[0][0] = zNear * __glTwo / deltaX;
    m.matrix[1][1] = zNear * __glTwo / deltaY;
    m.matrix[2][0] = (right + left) / deltaX;
    m.matrix[2][1] = (top + bottom) / deltaY;
    m.matrix[2][2] = -(zFar + zNear) / deltaZ;
    m.matrix[2][3] = __glMinusOne;
    m.matrix[3][2] = ((__GLfloat) -2.0) * zNear * zFar / deltaZ;
    m.matrix[3][3] = __glZero;
    m.matrixType = __GL_MT_GENERAL;
    __glDoMultMatrix(gc, &m, __glMultiplyMatrix);
}

void GLAPI __glim_Ortho(GLdouble left, GLdouble right, GLdouble bottom, 
			GLdouble top, GLdouble zNear, GLdouble zFar)
{
    __GLmatrix m;
    GLdouble deltax, deltay, deltaz;
    GLdouble zero;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    deltax = right - left;
    deltay = top - bottom;
    deltaz = zFar - zNear;
    if ((deltax == __glZero) || (deltay == __glZero) || (deltaz == __glZero)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    (*gc->procs.matrix.makeIdentity)(&m);
    m.matrix[0][0] = __glTwo / deltax;
    m.matrix[3][0] = -(right + left) / deltax;
    m.matrix[1][1] = __glTwo / deltay;
    m.matrix[3][1] = -(top + bottom) / deltay;
    m.matrix[2][2] = ((__GLfloat) -2.0) / deltaz;
    m.matrix[3][2] = -(zFar + zNear) / deltaz;

    /* 
    ** Screen coordinates matrix?
    */
    zero = 0.0;
    if (left == zero && 
	    bottom == zero && 
	    right == (GLdouble) gc->state.viewport.width &&
	    top == (GLdouble) gc->state.viewport.height &&
	    zNear <= zero && 
	    zFar >= zero) {
	m.matrixType = __GL_MT_IS2DNRSC;
	m.width = gc->state.viewport.width;
	m.height = gc->state.viewport.height;
    } else {
	m.matrixType = __GL_MT_IS2DNR;
    }

    __glDoMultMatrix(gc, &m, __glMultiplyMatrix);
}

void __glUpdateViewport(__GLcontext *gc)
{
    __GLfloat ww, hh;

    /* Compute operational viewport values */
    ww = gc->state.viewport.width * __glHalf;
    hh = gc->state.viewport.height * __glHalf;

    gc->state.viewport.xScale = ww;
#if defined(__GL_ALIGNED_BUFFERS)
    gc->state.viewport.xCenter = gc->state.viewport.x + ww +
	gc->constants.fviewportXAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
    gc->state.viewport.xCenter = gc->state.viewport.x + ww +
	gc->constants.fviewportXAdjust;
#endif /* __GL_ALIGNED_BUFFERS */

    if (gc->constants.yInverted) {
	gc->state.viewport.yScale = -hh;
#if defined(__GL_ALIGNED_BUFFERS)
	gc->state.viewport.yCenter =
	    (gc->constants.height - gc->constants.viewportEpsilon) -
	    (gc->state.viewport.y + hh) +
	    gc->constants.fviewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
	gc->state.viewport.yCenter =
	    (gc->constants.height - gc->constants.viewportEpsilon) -
	    (gc->state.viewport.y + hh) +
	    gc->constants.fviewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
    } else {
	gc->state.viewport.yScale = hh;
#if defined(__GL_ALIGNED_BUFFERS)
	gc->state.viewport.yCenter = gc->state.viewport.y + hh +
	    gc->constants.fviewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
	gc->state.viewport.yCenter = gc->state.viewport.y + hh +
	    gc->constants.fviewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
    }

#if defined(__GL_HALF_PIXEL_OFFSET)
    gc->state.viewport.xCenter += __glHalf;
    gc->state.viewport.yCenter += __glHalf;
#endif /* __GL_HALF_PIXEL_OFFSET */

    __glUpdateViewportTransform(gc);
}

void __glUpdateViewportTransform(__GLcontext *gc)
{
    /* 
    ** Now that the implementation may have found us a new window size,
    ** we compute these offsets...
    */
#if defined(__GL_ALIGNED_BUFFERS)
    gc->transform.minx = gc->state.viewport.x + 
	gc->constants.viewportXAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
    gc->transform.minx = gc->state.viewport.x + gc->constants.viewportXAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
    gc->transform.maxx = gc->transform.minx + gc->state.viewport.width;
    gc->transform.fminx = gc->transform.minx;
    gc->transform.fmaxx = gc->transform.maxx;

#if defined(__GL_ALIGNED_BUFFERS)
    gc->transform.miny = (gc->constants.height - 
			  (gc->state.viewport.y + gc->state.viewport.height)) + 
	gc->constants.viewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
    gc->transform.miny = (gc->constants.height - 
			  (gc->state.viewport.y + gc->state.viewport.height)) + 
	gc->constants.viewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
    gc->transform.maxy = gc->transform.miny + gc->state.viewport.height;
    gc->transform.fminy = gc->transform.miny;
    gc->transform.fmaxy = gc->transform.maxy;

#if defined(__GL_HALF_PIXEL_OFFSET)
    gc->transform.fminx += __glHalf;
    gc->transform.fmaxx += __glHalf;

    gc->transform.fminy += __glHalf;
    gc->transform.fmaxy += __glHalf;
#endif /* __GL_HALF_PIXEL_OFFSET */

    /*
    ** Pickers that notice when the transformation matches the viewport
    ** exactly need to be revalidated.  Ugh.
    */
    (*gc->procs.pickMvpMatrixProcs)(gc, &gc->transform.modelView->mvp);
}

__GLfloat __glReturnWindowX(__GLcontext *gc, __GLfloat x)
{
#if defined(__GL_HALF_PIXEL_OFFSET)
    x -= __glHalf;
#endif /* __GL_HALF_PIXEL_OFFSET */
#if defined(__GL_ALIGNED_BUFFERS)
    return (x - gc->constants.fviewportXAdjustOffset);
#else /* __GL_ALIGNED_BUFFERS */
    return (x - gc->constants.fviewportXAdjust);
#endif /* __GL_ALIGNED_BUFFERS */
}

__GLfloat __glReturnWindowY(__GLcontext *gc, __GLfloat y)
{
#if defined(__GL_HALF_PIXEL_OFFSET)
    y -= __glHalf;
#endif /* __GL_HALF_PIXEL_OFFSET */
#if defined(__GL_ALIGNED_BUFFERS)
    if (gc->constants.yInverted) {
	return ((gc->constants.height - gc->constants.viewportEpsilon) -
		(y - gc->constants.fviewportYAdjustOffset));
    } else {
	return (y - gc->constants.fviewportYAdjustOffset);
    }
#else /* __GL_ALIGNED_BUFFERS */
    if (gc->constants.yInverted) {
	return ((gc->constants.height - gc->constants.viewportEpsilon) -
		(y - gc->constants.fviewportYAdjust));
    } else {
	return (y - gc->constants.fviewportYAdjust);
    }
#endif /* __GL_ALIGNED_BUFFERS */
}

void GLAPI __glim_Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if ((width < 0) || (height < 0)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (width > gc->constants.maxViewportWidth) {
	width = gc->constants.maxViewportWidth;
    }
    if (height > gc->constants.maxViewportHeight) {
	height = gc->constants.maxViewportHeight;
    }

    gc->state.viewport.x = x;
    gc->state.viewport.y = y;
    gc->state.viewport.width = width;
    gc->state.viewport.height = height;

    (*gc->procs.applyViewport)(gc);

    __GL_DELAY_VALIDATE(gc);
}

void GLAPI __glim_DepthRange(GLdouble zNear, GLdouble zFar)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    SetDepthRange(gc, zNear, zFar);
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH | __GL_DIRTY_GENERIC);
}

void GLAPI __glim_Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLuint uwidth, uheight;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if ((width < 0) || (height < 0)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    uwidth = (GLuint) width;
    uheight = (GLuint) height;

    /* return if nothing changed */
    if ((x == gc->state.scissor.scissorX) &&
	(y == gc->state.scissor.scissorY) &&
	(uwidth == gc->state.scissor.scissorWidth) &&
	(uheight == gc->state.scissor.scissorHeight)) {
	return;
    }

    gc->state.scissor.scissorX = x;
    gc->state.scissor.scissorY = y;
    gc->state.scissor.scissorWidth = uwidth;
    gc->state.scissor.scissorHeight = uheight;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_SCISSOR);
    (*gc->procs.computeClipBox)(gc);
    (*gc->procs.applyScissor)(gc);
}

void GLAPI __glim_ClipPlane(GLenum pi, const GLdouble pv[])
{
    __GLfloat p[4];
    __GLtransform *tr;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_STATE();

    if(pi < GL_CLIP_PLANE0) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    pi -= GL_CLIP_PLANE0;
    if (pi >= (GLuint) gc->constants.numberOfClipPlanes) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    p[0] = pv[0];
    p[1] = pv[1];
    p[2] = pv[2];
    p[3] = pv[3];

    /*
    ** Project user clip plane into eye space.
    */
    tr = gc->transform.modelView;
    if (tr->updateInverse) {
	(*gc->procs.computeInverseTranspose)(gc, tr);
    }
    (*tr->inverseTranspose.xf4)(&gc->state.transform.eyeClipPlanes[pi], p,
				&tr->inverseTranspose);

    __GL_DELAY_VALIDATE(gc);
}

/************************************************************************/

void __glPushModelViewMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack;
    GLint num;

    num = gc->constants.maxModelViewStackDepth;
    trp = &gc->transform.modelView;
    stack = gc->transform.modelViewStack;
    tr = *trp;
    if (tr < &stack[num-1]) {
	tr[1] = tr[0];
	*trp = tr + 1;
    } else {
	__glSetError(GL_STACK_OVERFLOW);
    }
}

void __glPopModelViewMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack, *mvtr, *ptr;

    trp = &gc->transform.modelView;
    stack = gc->transform.modelViewStack;
    tr = *trp;
    if (tr > &stack[0]) {
	*trp = tr - 1;

	/*
	** See if sequence number of modelView matrix is the same as the
	** sequence number of the projection matrix.  If not, then
	** recompute the mvp matrix.
	*/
	mvtr = gc->transform.modelView;
	ptr = gc->transform.projection;
	if (mvtr->sequence != ptr->sequence) {
	    mvtr->sequence = ptr->sequence;
	    (*gc->procs.matrix.mult)(&mvtr->mvp, &mvtr->matrix, &ptr->matrix);
	}
	(*gc->procs.pickMvpMatrixProcs)(gc, &mvtr->mvp);
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_MODELVIEW);
    } else {
	__glSetError(GL_STACK_UNDERFLOW);
	return;
    }
}

void __glLoadIdentityModelViewMatrix(__GLcontext *gc)
{
    __GLtransform *mvtr, *ptr;

    mvtr = gc->transform.modelView;
    (*gc->procs.matrix.makeIdentity)(&mvtr->matrix);
    (*gc->procs.matrix.makeIdentity)(&mvtr->inverseTranspose);
    (*gc->procs.pickMatrixProcs)(gc, &mvtr->matrix);
    (*gc->procs.pickInvTransposeProcs)(gc, &mvtr->inverseTranspose);
    mvtr->updateInverse = GL_FALSE;
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_MODELVIEW);

    /* Update mvp matrix */
    ptr = gc->transform.projection;
    mvtr->sequence = ptr->sequence;
    (*gc->procs.matrix.mult)(&mvtr->mvp, &mvtr->matrix, &ptr->matrix);
    (*gc->procs.pickMvpMatrixProcs)(gc, &mvtr->mvp);
}

void __glComputeInverseTranspose(__GLcontext *gc, __GLtransform *tr)
{
    (*gc->procs.matrix.invertTranspose)(&tr->inverseTranspose, &tr->matrix);
    if (gc->state.enables.general & __GL_RESCALE_NORMAL_ENABLE) {
        __GLfloat  factor;
        __GLmatrix *m = &tr->inverseTranspose;

        factor = __GL_SQRTF(m->matrix[0][2]*m->matrix[0][2] +
                            m->matrix[1][2]*m->matrix[1][2] +
                            m->matrix[2][2]*m->matrix[2][2]);

        if (factor == __glZero)
            tr->rescaleFactor = __glOne;
        else
            tr->rescaleFactor = __glOne / factor;
    }
    (*gc->procs.pickInvTransposeProcs)(gc, &tr->inverseTranspose);
    tr->updateInverse = GL_FALSE;
}

void __glValidateCullVertexFromModelView(__GLcontext *gc)
{
    if (gc->state.enables.general & __GL_CULL_VERTEX_ENABLE) {
	/* 
	** If the user has not specified a culling eye position in
	** object space, calculate object space eye direction or location.
	*/
	if (gc->state.transform.objEyeSpecified) {
	    gc->transform.cullEye = gc->state.transform.eyePosObj;
	} else {
	    __GLtransform *tr;

	    tr = gc->transform.modelView;

	    if (tr->updateInverse) {
		(*gc->procs.computeInverseTranspose)(gc, tr);
	    }

	    if (gc->state.transform.eyePos.w != 0.0f) {
		__glXForm4_Pre(&gc->transform.cullEye,
			       &gc->state.transform.eyePos.x,
			       &tr->inverseTranspose);
	    } else {
		__glXForm4_Pre(&gc->transform.cullEye,
			       &gc->state.transform.eyePos.x,
			       &tr->matrix);
		gc->transform.cullEye.w = 0.0f;
	    }
	}
    }
}

/************************************************************************/

void __glPushProjectionMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack;
    GLint num;

    num = gc->constants.maxProjectionStackDepth;
    trp = &gc->transform.projection;
    stack = gc->transform.projectionStack;
    tr = *trp;
    if (tr < &stack[num-1]) {
	tr[1].matrix = tr[0].matrix;
	tr[1].sequence = tr[0].sequence;
	*trp = tr + 1;
    } else {
	__glSetError(GL_STACK_OVERFLOW);
    }
}

void __glPopProjectionMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack, *mvtr, *ptr;

    trp = &gc->transform.projection;
    stack = gc->transform.projectionStack;
    tr = *trp;
    if (tr > &stack[0]) {
	*trp = tr - 1;

	/*
	** See if sequence number of modelView matrix is the same as the
	** sequence number of the projection matrix.  If not, then
	** recompute the mvp matrix.
	*/
	mvtr = gc->transform.modelView;
	ptr = gc->transform.projection;
	if (mvtr->sequence != ptr->sequence) {
	    mvtr->sequence = ptr->sequence;
	    (*gc->procs.matrix.mult)(&mvtr->mvp, &mvtr->matrix, &ptr->matrix);
	}
	(*gc->procs.pickMvpMatrixProcs)(gc, &mvtr->mvp);
    } else {
	__glSetError(GL_STACK_UNDERFLOW);
	return;
    }
}

void __glLoadIdentityProjectionMatrix(__GLcontext *gc)
{
    __GLtransform *mvtr, *ptr;

    ptr = gc->transform.projection;
    (*gc->procs.matrix.makeIdentity)(&ptr->matrix);
    (*gc->procs.pickMatrixProcs)(gc, &ptr->matrix);
    if (++gc->transform.projectionSequence == 0) {
	__glInvalidateSequenceNumbers(gc);
    } else {
	ptr->sequence = gc->transform.projectionSequence;
    }

    /* Update mvp matrix */
    mvtr = gc->transform.modelView;
    mvtr->sequence = ptr->sequence;
    (*gc->procs.matrix.mult)(&mvtr->mvp, &mvtr->matrix, &ptr->matrix);
    (*gc->procs.pickMvpMatrixProcs)(gc, &mvtr->mvp);
}

/************************************************************************/

void __glPushTextureMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack;
    GLint num;

    num = gc->constants.maxTextureStackDepth;
    trp = &gc->transform.texture[gc->state.texture.activeTexture];
    stack = gc->transform.textureStack[gc->state.texture.activeTexture];
    tr = *trp;
    if (tr < &stack[num-1]) {
	tr[1].matrix = tr[0].matrix;
	*trp = tr + 1;
    } else {
	__glSetError(GL_STACK_OVERFLOW);
    }
}

void __glPopTextureMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack;

    trp = &gc->transform.texture[gc->state.texture.activeTexture];
    stack = gc->transform.textureStack[gc->state.texture.activeTexture];
    tr = *trp;
    if (tr > &stack[0]) {
	*trp = tr - 1;
    } else {
	__glSetError(GL_STACK_UNDERFLOW);
	return;
    }
}

void __glLoadIdentityTextureMatrix(__GLcontext *gc)
{
    __GLtransform *tr = gc->transform.texture[gc->state.texture.activeTexture];

    (*gc->procs.matrix.makeIdentity)(&tr->matrix);
    (*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
}

/************************************************************************/

#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
void __glPushColorMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack;
    GLint num;

    num = gc->constants.maxColorStackDepth;
    trp = &gc->transform.color;
    stack = gc->transform.colorStack;
    tr = *trp;
    if (tr < &stack[num-1]) {
	tr[1].matrix = tr[0].matrix;
	*trp = tr + 1;
    } else {
	__glSetError(GL_STACK_OVERFLOW);
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void __glPopColorMatrix(__GLcontext *gc)
{
    __GLtransform **trp, *tr, *stack;

    trp = &gc->transform.color;
    stack = gc->transform.colorStack;
    tr = *trp;
    if (tr > &stack[0]) {
	*trp = tr - 1;
    } else {
	__glSetError(GL_STACK_UNDERFLOW);
	return;
    }
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}

void __glLoadIdentityColorMatrix(__GLcontext *gc)
{
    __GLtransform *tr = gc->transform.color;

    (*gc->procs.matrix.makeIdentity)(&tr->matrix);
    (*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
}
#endif

/************************************************************************/

void __glDoLoadMatrix(__GLcontext *gc, const __GLmatrix *m)
{
    __GLtransform *tr, *otr;

    switch (gc->state.transform.matrixMode) {
      case GL_MODELVIEW:
	tr = gc->transform.modelView;
	(*gc->procs.matrix.copy)(&tr->matrix, m);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	tr->updateInverse = GL_TRUE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_MODELVIEW);

	/* Update mvp matrix */
	otr = gc->transform.projection;
	tr->sequence = otr->sequence;
	(*gc->procs.matrix.mult)(&tr->mvp, &tr->matrix, &otr->matrix);
	(*gc->procs.pickMvpMatrixProcs)(gc, &tr->mvp);
	break;

      case GL_PROJECTION:
	tr = gc->transform.projection;
	(*gc->procs.matrix.copy)(&tr->matrix, m);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	if (++gc->transform.projectionSequence == 0) {
	    __glInvalidateSequenceNumbers(gc);
	} else {
	    tr->sequence = gc->transform.projectionSequence;
	}

	/* Update mvp matrix */
	otr = gc->transform.modelView;
	otr->sequence = tr->sequence;
	(*gc->procs.matrix.mult)(&otr->mvp, &otr->matrix, &tr->matrix);
	(*gc->procs.pickMvpMatrixProcs)(gc, &otr->mvp);
	break;

      case GL_TEXTURE:
	tr = gc->transform.texture[gc->state.texture.activeTexture];
	(*gc->procs.matrix.copy)(&tr->matrix, m);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	break;

      case GL_COLOR:
	tr = gc->transform.color;
	(*gc->procs.matrix.copy)(&tr->matrix, m);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
	break;
    }
}

void __glDoMultMatrix(__GLcontext *gc, void *data, 
		      void (*multiply)(__GLcontext *gc, __GLmatrix *m, 
		      void *data))
{
    __GLtransform *tr, *otr;

    switch (gc->state.transform.matrixMode) {
      case GL_MODELVIEW:
	tr = gc->transform.modelView;
	(*multiply)(gc, &tr->matrix, data);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	tr->updateInverse = GL_TRUE;
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_MODELVIEW);

	/* Update mvp matrix */
	(*multiply)(gc, &tr->mvp, data);
	(*gc->procs.pickMvpMatrixProcs)(gc, &tr->mvp);
	break;

      case GL_PROJECTION:
	tr = gc->transform.projection;
	(*multiply)(gc, &tr->matrix, data);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	if (++gc->transform.projectionSequence == 0) {
	    __glInvalidateSequenceNumbers(gc);
	} else {
	    tr->sequence = gc->transform.projectionSequence;
	}

	/* Update mvp matrix */
	otr = gc->transform.modelView;
	otr->sequence = tr->sequence;
	(*gc->procs.matrix.mult)(&otr->mvp, &otr->matrix, &tr->matrix);
	(*gc->procs.pickMvpMatrixProcs)(gc, &otr->mvp);
	break;

      case GL_TEXTURE:
	tr = gc->transform.texture[gc->state.texture.activeTexture];
	(*multiply)(gc, &tr->matrix, data);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	break;

      case GL_COLOR:
	tr = gc->transform.color;
	(*multiply)(gc, &tr->matrix, data);
	(*gc->procs.pickMatrixProcs)(gc, &tr->matrix);
	__GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_PIXEL);
	break;
    }
}

/************************************************************************/

void __glDoRotate(__GLcontext *gc, __GLfloat angle, __GLfloat ax,
		  __GLfloat ay, __GLfloat az)
{
    __GLmatrix m;
    __GLfloat radians, sine, cosine, ab, bc, ca, t;
    __GLfloat av[4], axis[4];

    av[0] = ax;
    av[1] = ay;
    av[2] = az;
    av[3] = 0;
    (*gc->procs.normalize)(axis, av);

    radians = angle * __glDegreesToRadians;
    sine = __GL_SINF(radians);
    cosine = __GL_COSF(radians);
    ab = axis[0] * axis[1] * (1 - cosine);
    bc = axis[1] * axis[2] * (1 - cosine);
    ca = axis[2] * axis[0] * (1 - cosine);

    (*gc->procs.matrix.makeIdentity)(&m);
    t = axis[0] * axis[0];
    m.matrix[0][0] = t + cosine * (1 - t);
    m.matrix[2][1] = bc - axis[0] * sine;
    m.matrix[1][2] = bc + axis[0] * sine;

    t = axis[1] * axis[1];
    m.matrix[1][1] = t + cosine * (1 - t);
    m.matrix[2][0] = ca + axis[1] * sine;
    m.matrix[0][2] = ca - axis[1] * sine;

    t = axis[2] * axis[2];
    m.matrix[2][2] = t + cosine * (1 - t);
    m.matrix[1][0] = ab - axis[2] * sine;
    m.matrix[0][1] = ab + axis[2] * sine;
    if (ax == __glZero && ay == __glZero) {
	m.matrixType = __GL_MT_IS2D;
    } else {
	m.matrixType = __GL_MT_W0001;
    }
    __glDoMultMatrix(gc, &m, __glMultiplyMatrix);
}

struct __glScaleRec {
    __GLfloat x,y,z;
};

/* ARGSUSED */
void __glScaleMatrix(__GLcontext *gc, __GLmatrix *m, void *data)
{
    struct __glScaleRec *scale;
    __GLfloat x,y,z;
    __GLfloat M0, M1, M2, M3;

    if (m->matrixType > __GL_MT_IS2DNR) {
	m->matrixType = __GL_MT_IS2DNR;
    }
    scale = data;
    x = scale->x;
    y = scale->y;
    z = scale->z;
    
    M0 = x * m->matrix[0][0];
    M1 = x * m->matrix[0][1];
    M2 = x * m->matrix[0][2];
    M3 = x * m->matrix[0][3];
    m->matrix[0][0] = M0;
    m->matrix[0][1] = M1;
    m->matrix[0][2] = M2;
    m->matrix[0][3] = M3;

    M0 = y * m->matrix[1][0];
    M1 = y * m->matrix[1][1];
    M2 = y * m->matrix[1][2];
    M3 = y * m->matrix[1][3];
    m->matrix[1][0] = M0;
    m->matrix[1][1] = M1;
    m->matrix[1][2] = M2;
    m->matrix[1][3] = M3;

    M0 = z * m->matrix[2][0];
    M1 = z * m->matrix[2][1];
    M2 = z * m->matrix[2][2];
    M3 = z * m->matrix[2][3];
    m->matrix[2][0] = M0;
    m->matrix[2][1] = M1;
    m->matrix[2][2] = M2;
    m->matrix[2][3] = M3;
}

void __glDoScale(__GLcontext *gc, __GLfloat x, __GLfloat y, __GLfloat z)
{
    struct __glScaleRec scale;

    scale.x = x;
    scale.y = y;
    scale.z = z;
    __glDoMultMatrix(gc, &scale, __glScaleMatrix);
}

struct __glTranslationRec {
    __GLfloat x,y,z;
};

/*
** Matrix type of m stays the same.
*/
/* ARGSUSED */
void __glTranslateMatrix(__GLcontext *gc, __GLmatrix *m, void *data)
{
    struct __glTranslationRec *trans;
    __GLfloat x,y,z;
    __GLfloat _M30, _M31, _M32, _M33;

    if (m->matrixType > __GL_MT_IS2DNR) {
	m->matrixType = __GL_MT_IS2DNR;
    }
    trans = data;
    x = trans->x;
    y = trans->y;
    z = trans->z;
    _M30 = x * m->matrix[0][0] + y * m->matrix[1][0] + z * m->matrix[2][0] + 
	    m->matrix[3][0];
    _M31 = x * m->matrix[0][1] + y * m->matrix[1][1] + z * m->matrix[2][1] + 
	    m->matrix[3][1];
    _M32 = x * m->matrix[0][2] + y * m->matrix[1][2] + z * m->matrix[2][2] + 
	    m->matrix[3][2];
    _M33 = x * m->matrix[0][3] + y * m->matrix[1][3] + z * m->matrix[2][3] + 
	    m->matrix[3][3];
    m->matrix[3][0] = _M30;
    m->matrix[3][1] = _M31;
    m->matrix[3][2] = _M32;
    m->matrix[3][3] = _M33;
}

void __glDoTranslate(__GLcontext *gc, __GLfloat x, __GLfloat y, __GLfloat z)
{
    struct __glTranslationRec trans;

    trans.x = x;
    trans.y = y;
    trans.z = z;
    __glDoMultMatrix(gc, &trans, __glTranslateMatrix);
}

/************************************************************************/

/*
** Compute the clip box from the scissor (if enabled) and the window
** size.  The resulting clip box is used to clip primitive rasterization
** against.  The "window system" is responsible for doing the fine
** grain clipping (i.e., dealing with overlapping windows, etc.).
*/
void __glComputeClipBox(__GLcontext *gc)
{
    __GLscissor *sp = &gc->state.scissor;
    GLint llx, lly, urx, ury;
    GLint y0, y1;

    if (gc->state.enables.general & __GL_SCISSOR_TEST_ENABLE) {
	llx = sp->scissorX;
	lly = sp->scissorY;
	urx = llx + sp->scissorWidth;
	ury = lly + sp->scissorHeight;

	if ((urx < 0) || (ury < 0) ||
	    (urx <= llx) || (ury <= lly) ||
	    (llx >= (GLint)gc->constants.width) || 
	    (lly >= (GLint)gc->constants.height)) {
	    llx = lly = urx = ury = 0;
	} else {
	    if (llx < 0) llx = 0;
	    if (lly < 0) lly = 0;
	    if (urx > (GLint)gc->constants.width) urx = gc->constants.width;
	    if (ury > (GLint)gc->constants.height) ury = gc->constants.height;
	}
    } else {
	llx = 0;
	lly = 0;
	urx = gc->constants.width;
	ury = gc->constants.height;
    }

    if (gc->constants.yInverted) {
	y0 = gc->constants.height - ury;
	y1 = gc->constants.height - lly;
    } else {
	y0 = lly;
	y1 = ury;
    }

    (*gc->drawablePrivate->setClipRect)(gc->drawablePrivate,
					llx, y0, urx-llx, y1-y0);

#if defined(__GL_ALIGNED_BUFFERS)
    gc->transform.clipX0 = llx + gc->constants.viewportXAdjustOffset;
    gc->transform.clipY0 = y0 + gc->constants.viewportYAdjustOffset;
    gc->transform.clipX1 = urx + gc->constants.viewportXAdjustOffset;
    gc->transform.clipY1 = y1 + gc->constants.viewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
    gc->transform.clipX0 = llx + gc->constants.viewportXAdjust;
    gc->transform.clipY0 = y0 + gc->constants.viewportYAdjust;
    gc->transform.clipX1 = urx + gc->constants.viewportXAdjust;
    gc->transform.clipY1 = y1 + gc->constants.viewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
}

/************************************************************************/

/*
** Note: These xform routines must allow for the case where the result
** vector is equal to the source vector.
*/

/*
** Avoid some transformation computations by knowing that the incoming
** vertex has z=0 and w=1
*/
void __glXForm2(__GLcoord *res, const __GLfloat v[2], const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];

    res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + m->matrix[3][0];
    res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + m->matrix[3][1];
    res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + m->matrix[3][2];
    res->w = x*m->matrix[0][3] + y*m->matrix[1][3] + m->matrix[3][3];
}

void __glXForm3(__GLcoord *res, const __GLfloat v[3], const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];

	res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
	    + m->matrix[3][0];
	res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
	    + m->matrix[3][1];
	res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
	    + m->matrix[3][2];
	res->w = x*m->matrix[0][3] + y*m->matrix[1][3] + z*m->matrix[2][3]
	    + m->matrix[3][3];
}

void __glXForm3_Batch(__GLcoord *res, const __GLfloat v[3], 
		      const __GLmatrix *m, 
		      GLint sstride, GLint dstride, GLint n )
{
    while ( n )
    {
        __GLfloat x = v[0];
        __GLfloat y = v[1];
        __GLfloat z = v[2];
        
        res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
            + m->matrix[3][0];
        res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
            + m->matrix[3][1];
        res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
            + m->matrix[3][2];
        res->w = x*m->matrix[0][3] + y*m->matrix[1][3] + z*m->matrix[2][3]
            + m->matrix[3][3];

        res = ( __GLcoord * ) ( ( ( char * ) res ) + dstride );
        v   = ( const __GLfloat * ) ( ( ( const char * ) v ) + sstride );
        --n;
    }
}

/*
** Full 4x4 transformation.
*/
void __glXForm4(__GLcoord *res, const __GLfloat v[4], const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];
    __GLfloat w = v[3];

    if (w == ((__GLfloat) 1.0)) {
	res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
	    + m->matrix[3][0];
	res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
	    + m->matrix[3][1];
	res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
	    + m->matrix[3][2];
	res->w = x*m->matrix[0][3] + y*m->matrix[1][3] + z*m->matrix[2][3]
	    + m->matrix[3][3];
    } else {
	res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
	    + w*m->matrix[3][0];
	res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
	    + w*m->matrix[3][1];
	res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
	    + w*m->matrix[3][2];
	res->w = x*m->matrix[0][3] + y*m->matrix[1][3] + z*m->matrix[2][3]
	    + w*m->matrix[3][3];
    }
}

void __glXForm4_Batch(__GLcoord *res, const __GLfloat v[4], 
		      const __GLmatrix *m, 
		      GLint sstride, GLint dstride, GLint n )
{
    while ( n )
    {
        __GLfloat x = v[0];
        __GLfloat y = v[1];
        __GLfloat z = v[2];
        __GLfloat w = v[3];
        
        if (w == ((__GLfloat) 1.0)) {
            res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
                + m->matrix[3][0];
            res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
                + m->matrix[3][1];
            res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
                + m->matrix[3][2];
            res->w = x*m->matrix[0][3] + y*m->matrix[1][3] + z*m->matrix[2][3]
                + m->matrix[3][3];
        } else {
            res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
                + w*m->matrix[3][0];
            res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
                + w*m->matrix[3][1];
            res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
                + w*m->matrix[3][2];
            res->w = x*m->matrix[0][3] + y*m->matrix[1][3] + z*m->matrix[2][3]
                + w*m->matrix[3][3];
        }

        v   = ( const __GLfloat * ) ( ( ( const char * ) v ) + sstride );
        res = ( __GLcoord * ) ( ( ( char * ) res ) + dstride );
        --n;
    }
}

/*
** Full 4x4 premultipled transform
*/
void __glXForm4_Pre(__GLcoord *res, const __GLfloat v[4], const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];
    __GLfloat w = v[3];

    res->x = x*m->matrix[0][0] + y*m->matrix[0][1] + z*m->matrix[0][2]
	+ w*m->matrix[0][3];
    res->y = x*m->matrix[1][0] + y*m->matrix[1][1] + z*m->matrix[1][2]
	+ w*m->matrix[1][3];
    res->z = x*m->matrix[2][0] + y*m->matrix[2][1] + z*m->matrix[2][2]
	+ w*m->matrix[2][3];
    res->w = x*m->matrix[3][0] + y*m->matrix[3][1] + z*m->matrix[3][2]
	+ w*m->matrix[3][3];
}

/************************************************************************/

/*
** Avoid some transformation computations by knowing that the incoming
** vertex has z=0 and w=1.  The w column of the matrix is [0 0 0 1].
*/
void __glXForm2_W(__GLcoord *res, const __GLfloat v[2], const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];

    res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + m->matrix[3][0];
    res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + m->matrix[3][1];
    res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + m->matrix[3][2];
    res->w = ((__GLfloat) 1.0);
}

/*
** Avoid some transformation computations by knowing that the incoming
** vertex has w=1.  The w column of the matrix is [0 0 0 1].
*/
void __glXForm3_W(__GLcoord *res, const __GLfloat v[3], const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];

    res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
	+ m->matrix[3][0];
    res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
	+ m->matrix[3][1];
    res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
	+ m->matrix[3][2];
    res->w = ((__GLfloat) 1.0);
}

/*
** Full 4x4 transformation.  The w column of the matrix is [0 0 0 1].
*/
void __glXForm4_W(__GLcoord *res, const __GLfloat v[4], const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];
    __GLfloat w = v[3];
  
    if (w == ((__GLfloat) 1.0)) {
	res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
	    + m->matrix[3][0];
	res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
	    + m->matrix[3][1];
	res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
	    + m->matrix[3][2];
    } else {
	res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + z*m->matrix[2][0]
	    + w*m->matrix[3][0];
	res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + z*m->matrix[2][1]
	    + w*m->matrix[3][1];
	res->z = x*m->matrix[0][2] + y*m->matrix[1][2] + z*m->matrix[2][2]
	    + w*m->matrix[3][2];
    }
    res->w = w;
}

/*
** Avoid some transformation computations by knowing that the incoming
** vertex has z=0 and w=1.
**
** The matrix looks like:
** | . . 0 0 |
** | . . 0 0 |
** | 0 0 . 0 |
** | . . . 1 |
*/
void __glXForm2_2DW(__GLcoord *res, const __GLfloat v[2], 
		    const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];

    res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + m->matrix[3][0];
    res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + m->matrix[3][1];
    res->z = m->matrix[3][2];
    res->w = ((__GLfloat) 1.0);
}

/*
** Avoid some transformation computations by knowing that the incoming
** vertex has w=1.
**
** The matrix looks like:
** | . . 0 0 |
** | . . 0 0 |
** | 0 0 . 0 |
** | . . . 1 |
*/
void __glXForm3_2DW(__GLcoord *res, const __GLfloat v[3], 
		    const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];

    res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + m->matrix[3][0];
    res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + m->matrix[3][1];
    res->z = z*m->matrix[2][2] + m->matrix[3][2];
    res->w = ((__GLfloat) 1.0);
}

/*
** Full 4x4 transformation.
**
** The matrix looks like:
** | . . 0 0 |
** | . . 0 0 |
** | 0 0 . 0 |
** | . . . 1 |
*/
void __glXForm4_2DW(__GLcoord *res, const __GLfloat v[4], 
		    const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];
    __GLfloat w = v[3];

    if (w == ((__GLfloat) 1.0)) {
	res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + m->matrix[3][0];
	res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + m->matrix[3][1];
	res->z = z*m->matrix[2][2] + m->matrix[3][2];
    } else {
	res->x = x*m->matrix[0][0] + y*m->matrix[1][0] + w*m->matrix[3][0];
	res->y = x*m->matrix[0][1] + y*m->matrix[1][1] + w*m->matrix[3][1];
	res->z = z*m->matrix[2][2] + w*m->matrix[3][2];
    }
    res->w = w;
}

/*
** Avoid some transformation computations by knowing that the incoming
** vertex has z=0 and w=1.
**
** The matrix looks like:
** | . 0 0 0 |
** | 0 . 0 0 |
** | 0 0 . 0 |
** | . . . 1 |
*/
void __glXForm2_2DNRW(__GLcoord *res, const __GLfloat v[2], 
		      const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];

    res->x = x*m->matrix[0][0] + m->matrix[3][0];
    res->y = y*m->matrix[1][1] + m->matrix[3][1];
    res->z = m->matrix[3][2];
    res->w = ((__GLfloat) 1.0);
}

/*
** Avoid some transformation computations by knowing that the incoming
** vertex has w=1.
**
** The matrix looks like:
** | . 0 0 0 |
** | 0 . 0 0 |
** | 0 0 . 0 |
** | . . . 1 |
*/
void __glXForm3_2DNRW(__GLcoord *res, const __GLfloat v[3], 
		      const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];

    res->x = x*m->matrix[0][0] + m->matrix[3][0];
    res->y = y*m->matrix[1][1] + m->matrix[3][1];
    res->z = z*m->matrix[2][2] + m->matrix[3][2];
    res->w = ((__GLfloat) 1.0);
}

/*
** Full 4x4 transformation.
**
** The matrix looks like:
** | . 0 0 0 |
** | 0 . 0 0 |
** | 0 0 . 0 |
** | . . . 1 |
*/
void __glXForm4_2DNRW(__GLcoord *res, const __GLfloat v[4], 
		      const __GLmatrix *m)
{
    __GLfloat x = v[0];
    __GLfloat y = v[1];
    __GLfloat z = v[2];
    __GLfloat w = v[3];

    if (w == ((__GLfloat) 1.0)) {
	res->x = x*m->matrix[0][0] + m->matrix[3][0];
	res->y = y*m->matrix[1][1] + m->matrix[3][1];
	res->z = z*m->matrix[2][2] + m->matrix[3][2];
    } else {
	res->x = x*m->matrix[0][0] + w*m->matrix[3][0];
	res->y = y*m->matrix[1][1] + w*m->matrix[3][1];
	res->z = z*m->matrix[2][2] + w*m->matrix[3][2];
    }
    res->w = w;
}

/************************************************************************/

/*
** Recompute the cached 2D matrix from the current mvp matrix and the viewport
** transformation.  This allows us to transform object coordinates directly
** to window coordinates.
*/
static void ReCompute2DMatrix(__GLcontext *gc, __GLmatrix *mvp)
{
    __GLviewport *vp;
    __GLmatrix *m;

    if (mvp->matrixType >= __GL_MT_IS2D) {
	m = &(gc->transform.matrix2D);
	vp = &(gc->state.viewport);
	m->matrix[0][0] = mvp->matrix[0][0] * vp->xScale;
	m->matrix[0][1] = mvp->matrix[0][1] * vp->yScale;
	m->matrix[1][0] = mvp->matrix[1][0] * vp->xScale;
	m->matrix[1][1] = mvp->matrix[1][1] * vp->yScale;
	m->matrix[2][2] = mvp->matrix[2][2];
	m->matrix[3][0] = mvp->matrix[3][0] * vp->xScale + vp->xCenter;
	m->matrix[3][1] = mvp->matrix[3][1] * vp->yScale + vp->yCenter;
	m->matrix[3][2] = mvp->matrix[3][2];
	m->matrix[3][3] = 1.0;
	m->matrixType = mvp->matrixType;
    }
}


/*
** A special picker for the mvp matrix which picks the mvp matrix, then
** calls the vertex picker, because the vertex picker depends upon the mvp 
** matrix.
*/
void __glGenericPickMvpMatrixProcs(__GLcontext *gc, __GLmatrix *m)
{
    __glPickMatrixType(m,
	&gc->transform.modelView->matrix,
	&gc->transform.projection->matrix);
    ReCompute2DMatrix(gc, m);
    (*gc->procs.pickMatrixProcs)(gc, m);
    (*gc->procs.pickVertexProcs)(gc);
}

/* ARGSUSED */
void __glGenericPickMatrixProcs(__GLcontext *gc, __GLmatrix *m)
{
    switch(m->matrixType) {
      case __GL_MT_GENERAL:
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf2 = __glK3DXForm2;
	    m->xf3 = __glK3DXForm3;
	    m->xf4 = __glK3DXForm4;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf2 = __glXForm2;
	    m->xf3 = __glI386XForm3;
	    m->xf4 = __glI386XForm4;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf2 = __glXForm2;
	m->xf3 = __glXForm3;
	m->xf4 = __glXForm4;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
      case __GL_MT_W0001:
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf2 = __glK3DXForm2_W;
	    m->xf3 = __glK3DXForm3_W;
	    m->xf4 = __glK3DXForm4_W;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf2 = __glXForm2_W;
	    m->xf3 = __glI386XForm3_W;
	    m->xf4 = __glI386XForm4_W;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf2 = __glXForm2_W;
	m->xf3 = __glXForm3_W;
	m->xf4 = __glXForm4_W;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
      case __GL_MT_IS2D:
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf2 = __glK3DXForm2_2DW;
	    m->xf3 = __glK3DXForm3_2DW;
	    m->xf4 = __glK3DXForm4_2DW;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf2 = __glXForm2_2DW;
	    m->xf3 = __glXForm3_2DW;
	    m->xf4 = __glXForm4_2DW;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf2 = __glXForm2_2DW;
	m->xf3 = __glXForm3_2DW;
	m->xf4 = __glXForm4_2DW;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
      case __GL_MT_IS2DNR:
      case __GL_MT_IS2DNRSC:
      case __GL_MT_IDENTITY:	/* probably never hit */
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf2 = __glK3DXForm2_2DNRW;
	    m->xf3 = __glK3DXForm3_2DNRW;
	    m->xf4 = __glK3DXForm4_2DNRW;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf2 = __glXForm2_2DNRW;
	    m->xf3 = __glXForm3_2DNRW;
	    m->xf4 = __glXForm4_2DNRW;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf2 = __glXForm2_2DNRW;
	m->xf3 = __glXForm3_2DNRW;
	m->xf4 = __glXForm4_2DNRW;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
    }
}

/* ARGSUSED */
void __glGenericPickInvTransposeProcs(__GLcontext *gc, __GLmatrix *m)
{
    m->xf4 = __glXForm4;

    switch(m->matrixType) {
      case __GL_MT_GENERAL:
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf3 = __glK3DXForm4;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf3 = __glXForm4;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf3 = __glXForm4;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
      case __GL_MT_W0001:
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf3 = __glK3DXForm3_W;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf3 = __glXForm3_W;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf3 = __glXForm3_W;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
      case __GL_MT_IS2D:
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf3 = __glK3DXForm3_2DW;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf3 = __glXForm3_2DW;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf3 = __glXForm3_2DW;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
      case __GL_MT_IS2DNR:
      case __GL_MT_IS2DNRSC:
      case __GL_MT_IDENTITY:	/* probably never hit */
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    m->xf3 = __glK3DXForm3_2DNRW;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    m->xf3 = __glXForm3_2DNRW;
	}
#else /* __GL_USE_ASMCODE && i386 */
	m->xf3 = __glXForm3_2DNRW;
#endif /* __GL_USE_ASMCODE && i386 */
	break;
    }
}
