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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_vertex.c#11 $
*/
#include "context.h"
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

#if !(defined(__GL_USE_ASMCODE) && defined(mips))
/*
** Clip check the clip coordinates against the frustum planes.
*/
GLuint __glClipCheckFrustum(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x, y, z, w, negW, invW;
    GLuint code;
    
    w = vx->clip.w;
    invW = w ? __glOne / w : __glZero;
    x = vx->clip.x;
    y = vx->clip.y;
    z = vx->clip.z;
    negW = -w;
    
    /* Set clip codes */
    code = 0;
    
    /*
    ** NOTE: it is possible for x to be less than negW and greater than w
    ** (if w is negative).  Otherwise there would be "else" clauses here.
    */
    if (x < negW) code |= __GL_CLIP_LEFT;
    if (x > w) code |= __GL_CLIP_RIGHT;
    if (y < negW) code |= __GL_CLIP_BOTTOM;
    if (y > w) code |= __GL_CLIP_TOP;
    if (z < negW) code |= __GL_CLIP_NEAR;
    if (z > w) code |= __GL_CLIP_FAR;
    
    vx->window.w = invW;
    vx->flagBits |= __GL_HAS_CLIP;
    
    /* Compute window coordinates if not clipped */
    if (!code) {
        __GLviewport *vp = &gc->state.viewport;
        __GLfloat wx, wy, wz;
        
        wx = x * vp->xScale * invW + vp->xCenter;
        wy = y * vp->yScale * invW + vp->yCenter;
        wz = z * vp->zScale * invW + vp->zCenter;
        vx->window.x = wx;
        vx->window.y = wy;
        vx->window.z = wz;
    }
    
    return code;
}

/*
** Clip check the clip coordinates against the frustum planes.
*/
GLuint __glClipCheckFrustum2D(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x, y, z, w, negW, invW;
    GLuint code;

    w = vx->clip.w;
    invW = w;		/* W is 1.0 */
    x = vx->clip.x;
    y = vx->clip.y;
    negW = -w;

    /* Set clip codes */
    code = 0;

    /*
    ** NOTE: it is possible for x to be less than negW and greater than w
    ** (if w is negative).  Otherwise there would be "else" clauses here.
    */
    if (x < negW) code |= __GL_CLIP_LEFT;
    if (x > w) code |= __GL_CLIP_RIGHT;
    if (y < negW) code |= __GL_CLIP_BOTTOM;
    if (y > w) code |= __GL_CLIP_TOP;

    vx->window.w = invW;
    vx->flagBits |= __GL_HAS_CLIP;

    if (!code) {
	__GLviewport *vp = &gc->state.viewport;
	__GLfloat wx, wy, wz;

	z = vx->clip.z;
	wx = x * vp->xScale + vp->xCenter;
	wy = y * vp->yScale + vp->yCenter;
	wz = z * vp->zScale + vp->zCenter;
	vx->window.x = wx;
	vx->window.y = wy;
	vx->window.z = wz;
    }

    return code;
}
#endif /* !(__GL_USE_ASMCODE && mips) */

GLuint __glClipCheckAll2(__GLcontext *gc, __GLvertex *vx)
{
    vx->obj.z = __glZero;
    vx->obj.w = __glOne;
    vx->flagBits = (vx->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
	__GL_VERTEX_2D_SIZE;
    return __glClipCheckAll(gc, vx);
}

GLuint __glClipCheckAll3(__GLcontext *gc, __GLvertex *vx)
{
    vx->obj.w = __glOne;
    vx->flagBits = (vx->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
	__GL_VERTEX_3D_SIZE;
    return __glClipCheckAll(gc, vx);
}

GLuint __glClipCheckAll4(__GLcontext *gc, __GLvertex *vx)
{
    vx->flagBits = (vx->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
	__GL_VERTEX_4D_SIZE;
    return __glClipCheckAll(gc, vx);
}

#if 0
/*
** Clip check if matrix is 2D.  Convert straight to window coords,
** bypassing clip coords and compute clip codes.  Incoming vertex was
** from a glVertex2*() call.
*/
GLuint __glClipCheck2D_2(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x, y, ox, oy;
    GLuint code;
    __GLmatrix *m;

    m = &(gc->transform.matrix2D);

    ox = vx->obj.x;
    oy = vx->obj.y;

    x = ox * m->matrix[0][0] + oy * m->matrix[1][0] + m->matrix[3][0];
    y = ox * m->matrix[0][1] + oy * m->matrix[1][1] + m->matrix[3][1];

    code = 0;
    if (x < gc->transform.fminx) code |= __GL_CLIP_LEFT;
    if (x >= gc->transform.fmaxx) code |= __GL_CLIP_RIGHT;
    if (y < gc->transform.fminy) {
	if (gc->constants.yInverted) {
	    code |= __GL_CLIP_TOP;
	} else {
	    code |= __GL_CLIP_BOTTOM;
	}
    }
    if (y >= gc->transform.fmaxy) {
	if (gc->constants.yInverted) {
	    code |= __GL_CLIP_BOTTOM;
	} else {
	    code |= __GL_CLIP_TOP;
	}
    }

    vx->window.x = x;
    vx->window.y = y;
    vx->flagBits |= __GL_HAS_CLIP;
    /* z is not needed */

    return code;
}


/*
** Clip check if matrix is 2D.  Convert straight to window coords,
** bypassing clip coords and compute clip codes.  Incoming vertex was
** from a glVertex3*() call.
*/
GLuint __glClipCheck2D_3(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat ox, oy, x, y, z;
    GLuint code;
    __GLmatrix *m;

    m = &(gc->transform.matrix2D);

    ox = vx->obj.x;
    oy = vx->obj.y;
    z = vx->obj.z;

    x = ox * m->matrix[0][0] + oy * m->matrix[1][0] + m->matrix[3][0];
    y = ox * m->matrix[0][1] + oy * m->matrix[1][1] + m->matrix[3][1];
    z =  z * m->matrix[2][2] + m->matrix[3][2];

    code = 0;
    if (x < gc->transform.fminx) code |= __GL_CLIP_LEFT;
    if (x >= gc->transform.fmaxx) code |= __GL_CLIP_RIGHT;
    if (y < gc->transform.fminy) {
	if (gc->constants.yInverted) {
	    code |= __GL_CLIP_TOP;
	} else {
	    code |= __GL_CLIP_BOTTOM;
	}
    }
    if (y >= gc->transform.fmaxy) {
	if (gc->constants.yInverted) {
	    code |= __GL_CLIP_BOTTOM;
	} else {
	    code |= __GL_CLIP_TOP;
	}
    }
    if (z > __glOne) code |= __GL_CLIP_FAR;
    if (z < -1.0f) code |= __GL_CLIP_NEAR;
    vx->flagBits |= __GL_HAS_CLIP;

    if (!code) {
	vx->window.x = x;
	vx->window.y = y;
	/* z is not needed */
    }

    return code;
}
#endif /* NOTUSED */

/*
** Clip check against the frustum and user clipping planes.
*/
GLuint __glClipCheckAll(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x, y, z, w, negW, invW, zero, dot;
    GLuint code, bit, clipPlanesMask;
    __GLcoord *plane;

    /* We need eye coordinates to do user clip plane clipping */
    DO_VALIDATE(gc, vx, __GL_HAS_EYE);

    /*
    ** Do frustum checks.
    **
    ** NOTE: it is possible for x to be less than negW and greater than w
    ** (if w is negative).  Otherwise there would be "else" clauses here.
    */
    w = vx->clip.w;
    invW = w ? __glOne / w : __glZero;
    x = vx->clip.x;
    y = vx->clip.y;
    z = vx->clip.z;
    negW = -w;
    code = 0;
    if (x < negW) code |= __GL_CLIP_LEFT;
    if (x > w) code |= __GL_CLIP_RIGHT;
    if (y < negW) code |= __GL_CLIP_BOTTOM;
    if (y > w) code |= __GL_CLIP_TOP;
    if (z < negW) code |= __GL_CLIP_NEAR;
    if (z > w) code |= __GL_CLIP_FAR;
    vx->window.w = invW;

    /*
    ** Now do user clip plane checks
    */
    x = vx->eye.x;
    y = vx->eye.y;
    z = vx->eye.z;
    w = vx->eye.w;
    clipPlanesMask = gc->state.enables.clipPlanes;
    plane = &gc->state.transform.eyeClipPlanes[0];
    bit = __GL_CLIP_USER0;
    zero = __glZero;
    while (clipPlanesMask) {
	if (clipPlanesMask & 1) {
	    /*
	    ** Dot the vertex clip coordinate against the clip plane and see
	    ** if the sign is negative.  If so, then the point is out.
	    */
	    dot = x * plane->x + y * plane->y + z * plane->z + w * plane->w;
	    if (dot < zero) {
		code |= bit;
	    }
	}
	clipPlanesMask >>= 1;
	bit <<= 1;
	plane++;
    }
    vx->flagBits |= __GL_HAS_CLIP;

    /* Compute window coordinates if not clipped */
    if (!code) {
	__GLviewport *vp = &gc->state.viewport;
	__GLfloat wx, wy, wz;

	x = vx->clip.x;
	y = vx->clip.y;
	z = vx->clip.z;
	wx = x * vp->xScale * invW + vp->xCenter;
	wy = y * vp->yScale * invW + vp->yCenter;
	wz = z * vp->zScale * invW + vp->zCenter;
	vx->window.x = wx;
	vx->window.y = wy;
	vx->window.z = wz;
    }

    return vx->flagBits | code;
}

#if !(defined(__GL_USE_ASMCODE) && defined(mips))
/************************************************************************
** NOTE: This code is written like this so the compiler will realize that 
** there is no aliasing going on here, and it will do something reasonable
** (as opposed to the code it usually generates).
*/

void __glSaveN(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z;
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);

    vertex = gc->procs.vertex;
    x = gc->state.current.normal.x;
    y = gc->state.current.normal.y;
    z = gc->state.current.normal.z;
    vx->normal.x = x;
    vx->normal.y = y;
    vx->normal.z = z;
    (*vertex)(gc, vx);
}

void __glSaveCI(__GLcontext *gc, __GLvertex *vx)
{
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);
    __GLfloat index;

    vertex = gc->procs.vertex;
    index = gc->state.current.userColorIndex;
    vx->colors[__GL_FRONTFACE].r = index;
    (*vertex)(gc, vx);
}

void __glSaveC(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat r,g,b,a;
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);

    vertex = gc->procs.vertex;
    r = gc->state.current.color.r;
    g = gc->state.current.color.g;
    b = gc->state.current.color.b;
    a = gc->state.current.color.a;
    vx->colors[__GL_FRONTFACE].r = r;
    vx->colors[__GL_FRONTFACE].g = g;
    vx->colors[__GL_FRONTFACE].b = b;
    vx->colors[__GL_FRONTFACE].a = a;
    (*vertex)(gc, vx);
}

void __glSaveT(__GLcontext *gc, __GLvertex *vx)
{
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);
    __GLfloat x,y,z,w;
    GLint i;

    vertex = gc->procs.vertex;
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	x = gc->state.current.texture[i].x;
	y = gc->state.current.texture[i].y;
	z = gc->state.current.texture[i].z;
	w = gc->state.current.texture[i].w;
	vx->texture[i].x = x;
	vx->texture[i].y = y;
	vx->texture[i].z = z;
	vx->texture[i].w = w;
    }
    (*vertex)(gc, vx);
}

void __glSaveCT(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat r,g,b,a;
    __GLfloat x,y,z,w;
    GLint i;
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);

    vertex = gc->procs.vertex;
    r = gc->state.current.color.r;
    g = gc->state.current.color.g;
    b = gc->state.current.color.b;
    a = gc->state.current.color.a;
    vx->colors[__GL_FRONTFACE].r = r;
    vx->colors[__GL_FRONTFACE].g = g;
    vx->colors[__GL_FRONTFACE].b = b;
    vx->colors[__GL_FRONTFACE].a = a;
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	x = gc->state.current.texture[i].x;
	y = gc->state.current.texture[i].y;
	z = gc->state.current.texture[i].z;
	w = gc->state.current.texture[i].w;
	vx->texture[i].x = x;
	vx->texture[i].y = y;
	vx->texture[i].z = z;
	vx->texture[i].w = w;
    }
    (*vertex)(gc, vx);
}

void __glSaveNT(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z,w;
    GLint i;
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);

    vertex = gc->procs.vertex;
    x = gc->state.current.normal.x;
    y = gc->state.current.normal.y;
    z = gc->state.current.normal.z;
    vx->normal.x = x;
    vx->normal.y = y;
    vx->normal.z = z;
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	x = gc->state.current.texture[i].x;
	y = gc->state.current.texture[i].y;
	z = gc->state.current.texture[i].z;
	w = gc->state.current.texture[i].w;
	vx->texture[i].x = x;
	vx->texture[i].y = y;
	vx->texture[i].z = z;
	vx->texture[i].w = w;
    }
    (*vertex)(gc, vx);
}

void __glSaveCIAll(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z,w;
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);
    __GLfloat index;
    GLint i;

    index = gc->state.current.userColorIndex;
    vertex = gc->procs.vertex;
    x = gc->state.current.normal.x;
    y = gc->state.current.normal.y;
    z = gc->state.current.normal.z;
    vx->colors[__GL_FRONTFACE].r = index;
    vx->normal.x = x;
    vx->normal.y = y;
    vx->normal.z = z;
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	x = gc->state.current.texture[i].x;
	y = gc->state.current.texture[i].y;
	z = gc->state.current.texture[i].z;
	w = gc->state.current.texture[i].w;
	vx->texture[i].x = x;
	vx->texture[i].y = y;
	vx->texture[i].z = z;
	vx->texture[i].w = w;
    }
    (*vertex)(gc, vx);
}

void __glSaveCAll(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat r,g,b,a;
    __GLfloat x,y,z,w;
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);
    GLint i;

    vertex = gc->procs.vertex;
    x = gc->state.current.normal.x;
    y = gc->state.current.normal.y;
    z = gc->state.current.normal.z;
    r = gc->state.current.color.r;
    g = gc->state.current.color.g;
    b = gc->state.current.color.b;
    a = gc->state.current.color.a;
    vx->colors[__GL_FRONTFACE].r = r;
    vx->colors[__GL_FRONTFACE].g = g;
    vx->colors[__GL_FRONTFACE].b = b;
    vx->colors[__GL_FRONTFACE].a = a;
    vx->normal.x = x;
    vx->normal.y = y;
    vx->normal.z = z;
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	x = gc->state.current.texture[i].x;
	y = gc->state.current.texture[i].y;
	z = gc->state.current.texture[i].z;
	w = gc->state.current.texture[i].w;
	vx->texture[i].x = x;
	vx->texture[i].y = y;
	vx->texture[i].z = z;
	vx->texture[i].w = w;
    }
    (*vertex)(gc, vx);
}
#endif  /* !(__GL_USE_ASMCODE && mips) */

void __glSaveCIT(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat index;
    __GLfloat x,y,z,w;
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);
    GLint i;

    vertex = gc->procs.vertex;
    index = gc->state.current.userColorIndex;
    vx->colors[__GL_FRONTFACE].r = index;
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	x = gc->state.current.texture[i].x;
	y = gc->state.current.texture[i].y;
	z = gc->state.current.texture[i].z;
	w = gc->state.current.texture[i].w;
	vx->texture[i].x = x;
	vx->texture[i].y = y;
	vx->texture[i].z = z;
	vx->texture[i].w = w;
    }
    (*vertex)(gc, vx);
}

/************************************************************************/


#define __NORMAL 1
#define __COLOR 2
#define __TEX 4

static void (*CISaveProcs[8])(__GLcontext*, __GLvertex*) = {
    0, 			/* none */
    __glSaveN,		/* __NORMAL */
    __glSaveCI,		/* __COLOR */
    0, 			/* __NORMAL | __COLOR */

    __glSaveT, 		/* __TEX */
    __glSaveNT, 	/* __NORMAL | __TEX */

    __glSaveCIT, 	/* __COLOR | __TEX */
    __glSaveCIAll, 	/* __NORMAL | __COLOR | __TEX */
};

static void (*RGBSaveProcs[8])(__GLcontext*, __GLvertex*) = {
    0, 			/* none */
    __glSaveN, 		/* __NORMAL */

    __glSaveC, 		/* __COLOR */
    0, 			/* __NORMAL | __COLOR */

    __glSaveT, 		/* __TEX */
    __glSaveNT, 	/* __NORMAL | __TEX */

    __glSaveCT, 	/* __COLOR | __TEX */
    __glSaveCAll, 	/* __NORMAL | __COLOR | __TEX */
};

void __glGenericPickVertexProcs(__GLcontext *gc)
{
    GLuint enables = gc->state.enables.general;
    GLint ix;
    GLenum mvpMatrixType;
    __GLmatrix *m;

    m = &(gc->transform.modelView->mvp);
    mvpMatrixType = m->matrixType;

    /* Pick clipCheck proc */
#if defined(__GL_USE_ASMCODE) && defined(mips)
    if (gc->state.enables.clipPlanes) {
	/*
	** Transform and clip routines that are called from the assemblerized
	** vertex handlers are supposed to preserve t0, t1, t2 and t3.
	** Since we don't have assembly code versions for ClipCheckAll
	** routines we call assembly-code wrappers that save those registers
	** and then call the equivalent c code routine.
	*/
	gc->procs.clipCheck2 = __glMipsClipCheckAll2;
	gc->procs.clipCheck3 = __glMipsClipCheckAll3;
	gc->procs.clipCheck4 = __glMipsClipCheckAll4;
    } else {
	/*
	** Pick faster clip checkers if we know that incoming W is 1 and 
	** mvp matrix doesn't change it.
	*/
	if (mvpMatrixType >= __GL_MT_W0001) {
	    if (mvpMatrixType >= __GL_MT_IS2D &&
		    m->matrix[3][2] >= -1.0f && m->matrix[3][2] <= 1.0f) {
		gc->procs.clipCheck2 = __glMipsClipCheckFrustum2D;
	    } else {
		gc->procs.clipCheck2 = __glMipsClipCheckFrustum_W;
	    }
	    gc->procs.clipCheck3 = __glMipsClipCheckFrustum_W;
	} else {
	    gc->procs.clipCheck2 = __glMipsClipCheckFrustum;
	    gc->procs.clipCheck3 = __glMipsClipCheckFrustum;
	}

	gc->procs.clipCheck4 = __glMipsClipCheckFrustum;
    }
#else /* __GL_USE_ASMCODE && mips */
    if (gc->state.enables.clipPlanes) {
	gc->procs.clipCheck2 = __glClipCheckAll2;
	gc->procs.clipCheck3 = __glClipCheckAll3;
	gc->procs.clipCheck4 = __glClipCheckAll4;
    } else {
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    gc->procs.clipCheck3 = __glK3DClipCheckFrustum;
	    gc->procs.clipCheck4 = __glK3DClipCheckFrustum;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    gc->procs.clipCheck3 = __glI386ClipCheckFrustum;
	    gc->procs.clipCheck4 = __glI386ClipCheckFrustum;
	}
#else /* __GL_USE_ASMCODE && i386 */
	gc->procs.clipCheck3 = __glClipCheckFrustum;
	gc->procs.clipCheck4 = __glClipCheckFrustum;
#endif /* __GL_USE_ASMCODE && i386 */
	if (mvpMatrixType >= __GL_MT_IS2D &&
		m->matrix[3][2] >= -1.0f && m->matrix[3][2] <= 1.0f) {
	    gc->procs.clipCheck2 = __glClipCheckFrustum2D;
	} else {
#if defined(__GL_USE_ASMCODE) && defined(i386)
#if defined(__GL_USE_K3DCODE)
	if((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	    gc->procs.clipCheck2 = __glK3DClipCheckFrustum;
	} else
#endif /* __GL_USE_K3DCODE */
	{
	    gc->procs.clipCheck2 = __glI386ClipCheckFrustum;
	}
#else /* __GL_USE_ASMCODE && i386 */
	    gc->procs.clipCheck2 = __glClipCheckFrustum;
#endif /* __GL_USE_ASMCODE && i386 */
	}
    }
#endif /* __GL_USE_ASMCODE && mips */

    /* Pick vertex validation routines */
    if (gc->vertex.faceNeeds[__GL_FRONTFACE] == 0) {
	gc->procs.validateVertex[__GL_VERTEX_0D_INDEX] = 
		(void (*)(__GLcontext *, __GLvertex *, GLuint)) __glNop3;
	gc->procs.validateVertex[__GL_VERTEX_2D_INDEX] = 
		(void (*)(__GLcontext *, __GLvertex *, GLuint)) __glNop3;
	gc->procs.validateVertex[__GL_VERTEX_3D_INDEX] =
		(void (*)(__GLcontext *, __GLvertex *, GLuint)) __glNop3;
	gc->procs.validateVertex[__GL_VERTEX_4D_INDEX] = 
		(void (*)(__GLcontext *, __GLvertex *, GLuint)) __glNop3;
    } else {
	gc->procs.validateVertex[__GL_VERTEX_0D_INDEX] = 
		(void (*)(__GLcontext *, __GLvertex *, GLuint)) __glNop3;
	gc->procs.validateVertex[__GL_VERTEX_2D_INDEX] = __glValidateVertex2;
	gc->procs.validateVertex[__GL_VERTEX_3D_INDEX] = __glValidateVertex3;
	gc->procs.validateVertex[__GL_VERTEX_4D_INDEX] = __glValidateVertex4;
    }

    if (gc->vertexCache.vertexCacheEnabled) {
	(*gc->procs.pickVertexCacheProcs)(gc);
	return;
    } else {
	gc->dispatchState->dispatch.Begin = __glim_Begin;
	gc->dispatchState->dispatch.End = __glim_End;
	gc->dispatchState->dispatch.Vertex2fv = __glim_Vertex2fv;
	gc->dispatchState->dispatch.Vertex3fv = __glim_Vertex3fv;
	gc->dispatchState->dispatch.Vertex4fv = __glim_Vertex4fv;
    }

    if (gc->renderMode == GL_FEEDBACK) {
	if (gc->modes.colorIndexMode) {
	    gc->procs.v = __glSaveCIAll;
	} else {
	    gc->procs.v = __glSaveCAll;
	}
    } else {
	ix = 0;
	if (enables & __GL_LIGHTING_ENABLE) {
	    ix |= __NORMAL;
	} else {
	    ix |= __COLOR;
	}
	if (gc->texture.textureEnabled) {
	    ix |= __TEX;
	    if (gc->state.enables.texture[0] & __GL_TEXTURE_GEN_S_ENABLE) {
		if (gc->state.texture.active->s.mode == GL_SPHERE_MAP) {
		    ix |= __NORMAL;
		}
	    }
	    if (gc->state.enables.texture[0] & __GL_TEXTURE_GEN_T_ENABLE) {
		if (gc->state.texture.active->t.mode == GL_SPHERE_MAP) {
		    ix |= __NORMAL;
		}
	    }
	}
	if (gc->modes.rgbMode) {
	    gc->procs.v = RGBSaveProcs[ix];
	} else {
	    gc->procs.v = CISaveProcs[ix];
	}
	assert(gc->procs.v != 0);
    }
}

/************************************************************************/

#if !(defined(__GL_USE_ASMCODE) && defined(mips))
void __glValidateVertex2(__GLcontext *gc, __GLvertex *vx, GLuint needs)
{
    GLuint has = vx->flagBits;
    GLuint wants = needs & ~has;

    if (wants & __GL_HAS_EYE) {
	__GLtransform *tr = gc->transform.modelView;
	(*tr->matrix.xf2)(&vx->eye, &vx->obj.x, &tr->matrix);
    }
    if (wants & __GL_HAS_NORMAL) {
	__GLcoord ne;
	__GLtransform *tr = gc->transform.modelView;

	if (tr->inverseTranspose.matrixType == __GL_MT_GENERAL) {
	    /* this is needed only if there is projection info */
	    vx->normal.w =
		-(vx->normal.x * vx->obj.x + vx->normal.y * vx->obj.y);
	}
	if (gc->state.enables.general & __GL_NORMALIZE_ENABLE) {
	    (*tr->inverseTranspose.xf3)(&ne, &vx->normal.x, 
					&tr->inverseTranspose);
	    (*gc->procs.normalize)(&vx->normal.x, &ne.x);
	} else if (gc->state.enables.general & __GL_RESCALE_NORMAL_ENABLE) {
	  (*tr->inverseTranspose.xf3)(&ne, &vx->normal.x, 
				      &tr->inverseTranspose);
#if defined(__GL_RESCALE_NORMAL_NORMALIZES)
	  (*gc->procs.normalize)(&vx->normal.x, &ne.x);
#else /* __GL_RESCALE_NORMAL_NORMALIZES */
	  (*gc->procs.scaleVector)(&vx->normal.x, tr->rescaleFactor, &ne.x);
#endif /* __GL_RESCALE_NORMAL_NORMALIZES */
	} else {
	    (*tr->inverseTranspose.xf3)(&vx->normal, &vx->normal.x,
					&tr->inverseTranspose);
	}
    }
    if (wants & __GL_HAS_FOG) {
	vx->fog = (*gc->procs.fogVertex)(gc, vx);
    }
    if (wants & __GL_HAS_TEXTURE) {
	vx->obj.z = __glZero;
	vx->obj.w = __glOne;
	(*gc->procs.calcTexture)(gc, vx);
    }
    if (wants & __GL_HAS_FRONT_COLOR) {
	(*gc->procs.calcColor)(gc, __GL_FRONTFACE, vx);
    } 
    if (wants & __GL_HAS_BACK_COLOR) {
	(*gc->procs.calcColor)(gc, __GL_BACKFACE, vx);
    }

    vx->flagBits = has | wants;
}

void __glValidateVertex3(__GLcontext *gc, __GLvertex *vx, GLuint needs)
{
    GLuint has = vx->flagBits;
    GLuint wants = needs & ~has;

    if (wants & __GL_HAS_EYE) {
	__GLtransform *tr = gc->transform.modelView;
	(*tr->matrix.xf3)(&vx->eye, &vx->obj.x, &tr->matrix);
    }

    if (wants & __GL_HAS_NORMAL) {
	__GLcoord ne;
	__GLtransform *tr = gc->transform.modelView;

	if (tr->inverseTranspose.matrixType == __GL_MT_GENERAL) {
	    /* this is needed only if there is projection info */
	    vx->normal.w =
		-(vx->normal.x * vx->obj.x + vx->normal.y * vx->obj.y
			 + vx->normal.z * vx->obj.z);
	}
	if (gc->state.enables.general & __GL_NORMALIZE_ENABLE) {
	    (*tr->inverseTranspose.xf3)(&ne, &vx->normal.x, 
		    &tr->inverseTranspose);
	    (*gc->procs.normalize)(&vx->normal.x, &ne.x);
	} else if (gc->state.enables.general & __GL_RESCALE_NORMAL_ENABLE) {
	  (*tr->inverseTranspose.xf3)(&ne, &vx->normal.x, 
		    &tr->inverseTranspose);
#if defined(__GL_RESCALE_NORMAL_NORMALIZES)
	  (*gc->procs.normalize)(&vx->normal.x, &ne.x);
#else /* __GL_RESCALE_NORMAL_NORMALIZES */
	  (*gc->procs.scaleVector)(&vx->normal.x, tr->rescaleFactor, &ne.x);
#endif /* __GL_RESCALE_NORMAL_NORMALIZES */
	} else {
	    (*tr->inverseTranspose.xf3)(&vx->normal,
		    &vx->normal.x, &tr->inverseTranspose);
	}
    }
    if (wants & __GL_HAS_FOG) {
	vx->fog = (*gc->procs.fogVertex)(gc, vx);
    }
    if (wants & __GL_HAS_TEXTURE) {
	vx->obj.w = __glOne;
	(*gc->procs.calcTexture)(gc, vx);
    }
    if (wants & __GL_HAS_FRONT_COLOR) {
	(*gc->procs.calcColor)(gc, __GL_FRONTFACE, vx);
    } 
    if (wants & __GL_HAS_BACK_COLOR) {
	(*gc->procs.calcColor)(gc, __GL_BACKFACE, vx);
    }

    vx->flagBits = has | wants;
}

void __glValidateVertex4(__GLcontext *gc, __GLvertex *vx, GLuint needs)
{
    GLuint has = vx->flagBits;
    GLuint wants = needs & ~has;

    if (wants & __GL_HAS_EYE) {
	__GLtransform *tr = gc->transform.modelView;
	(*tr->matrix.xf4)(&vx->eye, &vx->obj.x, &tr->matrix);
    }
    if (wants & __GL_HAS_NORMAL) {
	__GLcoord ne;
	__GLtransform *tr = gc->transform.modelView;

	if (tr->inverseTranspose.matrixType == __GL_MT_GENERAL && vx->obj.w) {
	    /* this is needed only if there is projection info */
	    vx->normal.w = -(vx->normal.x * vx->obj.x + 
			     vx->normal.y * vx->obj.y + 
			     vx->normal.z * vx->obj.z) / vx->obj.w;
	} else {
	    vx->normal.w = __glZero;
	}
	if (gc->state.enables.general & __GL_NORMALIZE_ENABLE) {
	    (*tr->inverseTranspose.xf3)(&ne, &vx->normal.x, 
		    &tr->inverseTranspose);
	    (*gc->procs.normalize)(&vx->normal.x, &ne.x);
	} else if (gc->state.enables.general & __GL_RESCALE_NORMAL_ENABLE) {
	  (*tr->inverseTranspose.xf3)(&ne, &vx->normal.x, 
				      &tr->inverseTranspose);
#if defined(__GL_RESCALE_NORMAL_NORMALIZES)
	    (*gc->procs.normalize)(&vx->normal.x, &ne.x);
#else /* __GL_RESCALE_NORMAL_NORMALIZES */
	  (*gc->procs.scaleVector)(&vx->normal.x, tr->rescaleFactor, &ne.x);
#endif /* __GL_RESCALE_NORMAL_NORMALIZES */
	} else {
	    (*tr->inverseTranspose.xf3)(&vx->normal,
		    &vx->normal.x, &tr->inverseTranspose);
	}
    }
    if (wants & __GL_HAS_FOG) {
	vx->fog = (*gc->procs.fogVertex)(gc, vx);
    }
    if (wants & __GL_HAS_TEXTURE) {
	(*gc->procs.calcTexture)(gc, vx);
    }
    if (wants & __GL_HAS_FRONT_COLOR) {
	(*gc->procs.calcColor)(gc, __GL_FRONTFACE, vx);
    } 
    if (wants & __GL_HAS_BACK_COLOR) {
	(*gc->procs.calcColor)(gc, __GL_BACKFACE, vx);
    }

    vx->flagBits = has | wants;
}
#endif /* !(__GL_USE_ASMCODE && mips) */

void __glValidateVertex2Clip(__GLcontext *gc, __GLvertex *vx, GLuint needs)
{
    __GLtransform *tr;

    tr = gc->transform.modelView;
    if (needs & __GL_HAS_CLIP) {
	(*tr->mvp.xf2)(&vx->clip, &vx->obj.x, &tr->mvp);
	vx->flagBits |= __GL_HAS_CLIP;
    }
}

void __glValidateVertex3Clip(__GLcontext *gc, __GLvertex *vx, GLuint needs)
{
    __GLtransform *tr;

    tr = gc->transform.modelView;
    if (needs & __GL_HAS_CLIP) {
	(*tr->mvp.xf3)(&vx->clip, &vx->obj.x, &tr->mvp);
	vx->flagBits |= __GL_HAS_CLIP;
    }
}

void __glValidateVertex4Clip(__GLcontext *gc, __GLvertex *vx, GLuint needs)
{
    __GLtransform *tr;

    tr = gc->transform.modelView;
    if (needs & __GL_HAS_CLIP) {
	(*tr->mvp.xf4)(&vx->clip, &vx->obj.x, &tr->mvp);
	vx->flagBits |= __GL_HAS_CLIP;
    }
}
