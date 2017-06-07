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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_varray.c#24 $
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "context.h"
#include "global.h"
#include "varray.h"

#include "g_imfncs.h"
#include "g_lcomp.h"

#if defined(__GL_CODEGEN) && defined(i386)
#include "og_geom.h"
#endif /* __GL_CODEGEN && i386 */

#if defined(__GL_ENVVARS)
static GLboolean disable_og_varray;
static GLboolean disable_og_drawvertexes;
#endif /* __GL_ENVVARS */

static GLboolean AllocateVbuf(__GLcontext *gc, int first, int count);

#pragma optimize("aw", on)

static void CopyVertex2s(__GLcontext *gc, const GLshort *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = 0.0F; v->obj.w = 1.0F;
}
static void CopyVertex2i(__GLcontext *gc, const GLint *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = 0.0F; v->obj.w = 1.0F;
}
static void CopyVertex2f(__GLcontext *gc, const GLfloat *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = 0.0F; v->obj.w = 1.0F;
}
static void CopyVertex2d(__GLcontext *gc, const GLdouble *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = 0.0F; v->obj.w = 1.0F;
}
static void CopyVertex3s(__GLcontext *gc, const GLshort *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = 1.0F;
}
static void CopyVertex3i(__GLcontext *gc, const GLint *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = 1.0F;
}
static void CopyVertex3f(__GLcontext *gc, const GLfloat *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = 1.0F;
}
static void CopyVertex3d(__GLcontext *gc, const GLdouble *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = 1.0F;
}
static void CopyVertex4s(__GLcontext *gc, const GLshort *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = vp[3];
}
static void CopyVertex4i(__GLcontext *gc, const GLint *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = vp[3];
}
static void CopyVertex4f(__GLcontext *gc, const GLfloat *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = vp[3];
}
static void CopyVertex4d(__GLcontext *gc, const GLdouble *vp, __GLvertex *v)
{
    v->obj.x = vp[0]; v->obj.y = vp[1]; v->obj.z = vp[2]; v->obj.w = vp[3];
}

static void CopyNormal3b(__GLcontext *gc, const GLbyte *np, __GLvertex *v)
{
    v->normal.x = np[0]; v->normal.y = np[1]; v->normal.z = np[2];
}
static void CopyNormal3s(__GLcontext *gc, const GLshort *np, __GLvertex *v)
{
    v->normal.x = np[0]; v->normal.y = np[1]; v->normal.z = np[2];
}
static void CopyNormal3i(__GLcontext *gc, const GLint *np, __GLvertex *v)
{
    v->normal.x = np[0]; v->normal.y = np[1]; v->normal.z = np[2];
}
static void CopyNormal3f(__GLcontext *gc, const GLfloat *np, __GLvertex *v)
{
    v->normal.x = np[0]; v->normal.y = np[1]; v->normal.z = np[2];
}
static void CopyNormal3d(__GLcontext *gc, const GLdouble *np, __GLvertex *v)
{
    v->normal.x = np[0]; v->normal.y = np[1]; v->normal.z = np[2];
}
static void CopyCurrentNormal(__GLcontext *gc, const GLdouble *np, __GLvertex *v)
{
    v->normal.x = gc->state.current.normal.x;
    v->normal.y = gc->state.current.normal.y;
    v->normal.z = gc->state.current.normal.z;
}

static void CopyColor3b(__GLcontext *gc, const GLbyte *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_B_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_B_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_B_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor3ub(__GLcontext *gc, const GLubyte *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_UB_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_UB_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_UB_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor3s(__GLcontext *gc, const GLshort *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_S_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_S_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_S_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor3us(__GLcontext *gc, const GLushort *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_US_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_US_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_US_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor3i(__GLcontext *gc, const GLint *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_I_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_I_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_I_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor3ui(__GLcontext *gc, const GLuint *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_UI_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_UI_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_UI_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor3f(__GLcontext *gc, const GLfloat *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = cp[0];
    v->colors[__GL_FRONTFACE].g = cp[1];
    v->colors[__GL_FRONTFACE].b = cp[2];
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor3d(__GLcontext *gc, const GLdouble *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = cp[0];
    v->colors[__GL_FRONTFACE].g = cp[1];
    v->colors[__GL_FRONTFACE].b = cp[2];
    v->colors[__GL_FRONTFACE].a = 1.0F;
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4b(__GLcontext *gc, const GLbyte *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_B_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_B_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_B_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = __GL_B_TO_FLOAT(cp[3]);
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4ub(__GLcontext *gc, const GLubyte *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_UB_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_UB_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_UB_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = __GL_UB_TO_FLOAT(cp[3]);
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4s(__GLcontext *gc, const GLshort *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_S_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_S_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_S_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = __GL_S_TO_FLOAT(cp[3]);
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4us(__GLcontext *gc, const GLushort *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_US_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_US_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_US_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = __GL_US_TO_FLOAT(cp[3]);
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4i(__GLcontext *gc, const GLint *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_I_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_I_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_I_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = __GL_I_TO_FLOAT(cp[3]);
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4ui(__GLcontext *gc, const GLuint *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = __GL_UI_TO_FLOAT(cp[0]);
    v->colors[__GL_FRONTFACE].g = __GL_UI_TO_FLOAT(cp[1]);
    v->colors[__GL_FRONTFACE].b = __GL_UI_TO_FLOAT(cp[2]);
    v->colors[__GL_FRONTFACE].a = __GL_UI_TO_FLOAT(cp[3]);
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4f(__GLcontext *gc, const GLfloat *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = cp[0];
    v->colors[__GL_FRONTFACE].g = cp[1];
    v->colors[__GL_FRONTFACE].b = cp[2];
    v->colors[__GL_FRONTFACE].a = cp[3];
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyColor4d(__GLcontext *gc, const GLdouble *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = cp[0];
    v->colors[__GL_FRONTFACE].g = cp[1];
    v->colors[__GL_FRONTFACE].b = cp[2];
    v->colors[__GL_FRONTFACE].a = cp[3];
    __glClampAndScaleColorf(gc, &v->colors[__GL_FRONTFACE], &v->colors[__GL_FRONTFACE].r);
}
static void CopyCurrentColor(__GLcontext *gc, const GLdouble *cp, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = gc->state.current.color.r;
    v->colors[__GL_FRONTFACE].g = gc->state.current.color.g;
    v->colors[__GL_FRONTFACE].b = gc->state.current.color.b;
    v->colors[__GL_FRONTFACE].a = gc->state.current.color.a;
}

static void CopyIndexub(__GLcontext *gc, const GLubyte *ip, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = ip[0];
}
static void CopyIndexs(__GLcontext *gc, const GLshort *ip, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = ip[0];
}
static void CopyIndexi(__GLcontext *gc, const GLint *ip, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = ip[0];
}
static void CopyIndexf(__GLcontext *gc, const GLfloat *ip, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = ip[0];
}
static void CopyIndexd(__GLcontext *gc, const GLdouble *ip, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = ip[0];
}
static void CopyCurrentIndex(__GLcontext *gc, const GLdouble *ip, __GLvertex *v)
{
    v->colors[__GL_FRONTFACE].r = gc->state.current.userColorIndex;
}

static void CopyTexCoord1s(__GLcontext *gc, const GLshort *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = 0.0F; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord1i(__GLcontext *gc, const GLint *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = 0.0F; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord1f(__GLcontext *gc, const GLfloat *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = 0.0F; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord1d(__GLcontext *gc, const GLdouble *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = 0.0F; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord2s(__GLcontext *gc, const GLshort *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord2i(__GLcontext *gc, const GLint *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord2f(__GLcontext *gc, const GLfloat *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord2d(__GLcontext *gc, const GLdouble *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = 0.0F; v->texture[0].w = 1.0F;
}
static void CopyTexCoord3s(__GLcontext *gc, const GLshort *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = 1.0F;
}
static void CopyTexCoord3i(__GLcontext *gc, const GLint *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = 1.0F;
}
static void CopyTexCoord3f(__GLcontext *gc, const GLfloat *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = 1.0F;
}
static void CopyTexCoord3d(__GLcontext *gc, const GLdouble *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = 1.0F;
}
static void CopyTexCoord4s(__GLcontext *gc, const GLshort *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = tp[3];
}
static void CopyTexCoord4i(__GLcontext *gc, const GLint *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = tp[3];
}
static void CopyTexCoord4f(__GLcontext *gc, const GLfloat *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = tp[3];
}
static void CopyTexCoord4d(__GLcontext *gc, const GLdouble *tp, __GLvertex *v)
{
    v->texture[0].x = tp[0]; v->texture[0].y = tp[1]; v->texture[0].z = tp[2]; v->texture[0].w = tp[3];
}
static void CopyCurrentTexCoord(__GLcontext *gc, const GLdouble *tp, __GLvertex *v)
{
    v->texture[0].x = gc->state.current.texture[0].x;
    v->texture[0].y = gc->state.current.texture[0].y;
    v->texture[0].z = gc->state.current.texture[0].z;
    v->texture[0].w = gc->state.current.texture[0].w;
}

static void CopyEdgeFlag(__GLcontext *gc, const GLboolean *ep, __GLvertex *v)
{
    v->flagBits |= ep[0] ? __GL_VERTEX_EDGE_FLAG : 0;
}
static void CopyCurrentEdgeFlag(__GLcontext *gc, const GLboolean *ep, __GLvertex *v)
{
    v->flagBits |= gc->state.current.edgeTag;
}

static void CompileElements(__GLcontext *gc, GLint offset, 
			    GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *vp;
    int i;

    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	v->flagBits = gc->vertexArray.validateMask;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
    }
}

static void CompileElementsN(__GLcontext *gc, GLint offset, 
			     GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *np, *vp;
    int i;

    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = v->normal.x * (gc->transform.cullEye.x - v->obj.x) +
		      v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
		      v->normal.z * (gc->transform.cullEye.z - v->obj.z);
	    } else {
		dot = v->normal.x * gc->transform.cullEye.x +
		      v->normal.y * gc->transform.cullEye.y +
		      v->normal.z * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElements_N3F_V3F(__GLcontext *gc, GLint offset, 
				    GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    int off = gc->vertexArray.vp.stride * first;
    const GLubyte *np = (const GLubyte *) gc->vertexArray.np.pointer;
    const GLubyte *vp = (const GLubyte *) gc->vertexArray.vp.pointer;
    int i;

    for (i=0, v=vb; i<count; ++i, ++v) {
	const __GLfloat *normalPtr = (const GLfloat *) (np + off);
	const __GLfloat *vertexPtr = (const GLfloat *) (vp + off);

	off += gc->vertexArray.vp.stride;
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = normalPtr[0] * (gc->transform.cullEye.x - vertexPtr[0]) +
		      normalPtr[1] * (gc->transform.cullEye.y - vertexPtr[1]) +
		      normalPtr[2] * (gc->transform.cullEye.z - vertexPtr[2]);
	    } else {
		dot = normalPtr[0] * gc->transform.cullEye.x +
		      normalPtr[1] * gc->transform.cullEye.y +
		      normalPtr[2] * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    v->normal.x = normalPtr[0];
	    v->normal.y = normalPtr[1];
	    v->normal.z = normalPtr[2];
	    v->obj.x = vertexPtr[0];
	    v->obj.y = vertexPtr[1];
	    v->obj.z = vertexPtr[2];
	    v->obj.w = 1.0F;
	}


	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElementsC(__GLcontext *gc, GLint offset, 
			     GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *cp, *vp;
    int i;

    cp = (const GLubyte *) gc->vertexArray.cp.pointer +
				first * gc->vertexArray.cp.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyColor)(gc, cp, v);
	v->flagBits = gc->vertexArray.validateMask;
	cp += gc->vertexArray.cp.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
    }
}

static void CompileElementsI(__GLcontext *gc, GLint offset, 
			     GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *ip, *vp;
    int i;

    ip = (const GLubyte *) gc->vertexArray.ip.pointer +
				first * gc->vertexArray.ip.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyIndex)(gc, ip, v);
	v->flagBits = gc->vertexArray.validateMask;
	ip += gc->vertexArray.ip.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
    }
}

static void CompileElementsT(__GLcontext *gc, GLint offset, 
			     GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *tp, *vp;
    int i;

    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyTexCoord)(gc, tp, v);
	v->flagBits = gc->vertexArray.validateMask;
	tp += gc->vertexArray.tp[0].stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
    }
}

static void CompileElements_T2F_V3F(__GLcontext *gc, GLint offset, 
				    GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    int off = gc->vertexArray.vp.stride * first;
    const GLubyte *tp = (const GLubyte *) gc->vertexArray.tp[0].pointer;
    const GLubyte *vp = (const GLubyte *) gc->vertexArray.vp.pointer;
    int i;

    for (i=0, v=vb; i<count; ++i, ++v) {
	const __GLfloat *texCoordPtr = (const GLfloat *) (tp + off);
	const __GLfloat *vertexPtr = (const GLfloat *) (vp + off);

	off += gc->vertexArray.vp.stride;
	v->flagBits = gc->vertexArray.validateMask;
	v->texture[0].x = texCoordPtr[0];
	v->texture[0].y = texCoordPtr[1];
	v->texture[0].z = 0.0F;
	v->texture[0].w = 1.0F;
	v->obj.x = vertexPtr[0];
	v->obj.y = vertexPtr[1];
	v->obj.z = vertexPtr[2];
	v->obj.w = 1.0F;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
    }
}

static void CompileElementsNC(__GLcontext *gc, GLint offset, 
			      GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *cp, *np, *vp;
    int i;

    cp = (const GLubyte *) gc->vertexArray.cp.pointer +
				first * gc->vertexArray.cp.stride;
    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = v->normal.x * (gc->transform.cullEye.x - v->obj.x) +
		      v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
		      v->normal.z * (gc->transform.cullEye.z - v->obj.z);
	    } else {
		dot = v->normal.x * gc->transform.cullEye.x +
		      v->normal.y * gc->transform.cullEye.y +
		      v->normal.z * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.copyColor)(gc, cp, v);
	}
	cp += gc->vertexArray.cp.stride;
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElementsNI(__GLcontext *gc, GLint offset, 
			      GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *ip, *np, *vp;
    int i;

    ip = (const GLubyte *) gc->vertexArray.ip.pointer +
				first * gc->vertexArray.ip.stride;
    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = v->normal.x * (gc->transform.cullEye.x - v->obj.x) +
		      v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
		      v->normal.z * (gc->transform.cullEye.z - v->obj.z);
	    } else {
		dot = v->normal.x * gc->transform.cullEye.x +
		      v->normal.y * gc->transform.cullEye.y +
		      v->normal.z * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.copyIndex)(gc, ip, v);
	}
	ip += gc->vertexArray.ip.stride;
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElementsNT(__GLcontext *gc, GLint offset, 
			      GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *tp, *np, *vp;
    int i;

    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = v->normal.x * (gc->transform.cullEye.x - v->obj.x) +
		      v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
		      v->normal.z * (gc->transform.cullEye.z - v->obj.z);
	    } else {
		dot = v->normal.x * gc->transform.cullEye.x +
		      v->normal.y * gc->transform.cullEye.y +
		      v->normal.z * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.copyTexCoord)(gc, tp, v);
	}
	tp += gc->vertexArray.tp[0].stride;
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElements_T2F_N3F_V3F(__GLcontext *gc, GLint offset,
					GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    int off = gc->vertexArray.vp.stride * first;
    const GLubyte *tp = (const GLubyte *) gc->vertexArray.tp[0].pointer;
    const GLubyte *np = (const GLubyte *) gc->vertexArray.np.pointer;
    const GLubyte *vp = (const GLubyte *) gc->vertexArray.vp.pointer;
    int i;

    for (i=0, v=vb; i<count; ++i, ++v) {
	const __GLfloat *texCoordPtr = (const GLfloat *) (tp + off);
	const __GLfloat *normalPtr = (const GLfloat *) (np + off);
	const __GLfloat *vertexPtr = (const GLfloat *) (vp + off);

	off += gc->vertexArray.vp.stride;
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = normalPtr[0] * (gc->transform.cullEye.x - vertexPtr[0]) +
		      normalPtr[1] * (gc->transform.cullEye.y - vertexPtr[1]) +
		      normalPtr[2] * (gc->transform.cullEye.z - vertexPtr[2]);
	    } else {
		dot = normalPtr[0] * gc->transform.cullEye.x +
		      normalPtr[1] * gc->transform.cullEye.y +
		      normalPtr[2] * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    v->texture[0].x = texCoordPtr[0];
	    v->texture[0].y = texCoordPtr[1];
	    v->texture[0].z = 0.0F;
	    v->texture[0].w = 1.0F;
	    v->normal.x = normalPtr[0];
	    v->normal.y = normalPtr[1];
	    v->normal.z = normalPtr[2];
	    v->obj.x = vertexPtr[0];
	    v->obj.y = vertexPtr[1];
	    v->obj.z = vertexPtr[2];
	    v->obj.w = 1.0F;
	}
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElementsCT(__GLcontext *gc, GLint offset, 
			      GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *cp, *tp, *vp;
    int i;

    cp = (const GLubyte *) gc->vertexArray.cp.pointer +
				first * gc->vertexArray.cp.stride;
    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyTexCoord)(gc, tp, v);
	(*gc->vertexArray.copyColor)(gc, cp, v);
	v->flagBits = gc->vertexArray.validateMask;
	cp += gc->vertexArray.cp.stride;
	tp += gc->vertexArray.tp[0].stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
    }
}

static void CompileElementsIT(__GLcontext *gc, GLint offset, 
			      GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *ip, *tp, *vp;
    int i;

    ip = (const GLubyte *) gc->vertexArray.ip.pointer +
				first * gc->vertexArray.ip.stride;
    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyTexCoord)(gc, tp, v);
	(*gc->vertexArray.copyIndex)(gc, ip, v);
	v->flagBits = gc->vertexArray.validateMask;
	ip += gc->vertexArray.ip.stride;
	tp += gc->vertexArray.tp[0].stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
    }
}

static void CompileElementsNCT(__GLcontext *gc, GLint offset, 
			       GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *cp, *tp, *np, *vp;
    int i;

    cp = (const GLubyte *) gc->vertexArray.cp.pointer +
				first * gc->vertexArray.cp.stride;
    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = v->normal.x * (gc->transform.cullEye.x - v->obj.x) +
		      v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
		      v->normal.z * (gc->transform.cullEye.z - v->obj.z);
	    } else {
		dot = v->normal.x * gc->transform.cullEye.x +
		      v->normal.y * gc->transform.cullEye.y +
		      v->normal.z * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.copyTexCoord)(gc, tp, v);
	    (*gc->vertexArray.copyColor)(gc, cp, v);
	}
	cp += gc->vertexArray.cp.stride;
	tp += gc->vertexArray.tp[0].stride;
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElementsNIT(__GLcontext *gc, GLint offset, 
			       GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *ip, *tp, *np, *vp;
    int i;

    ip = (const GLubyte *) gc->vertexArray.ip.pointer +
				first * gc->vertexArray.ip.stride;
    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = v->normal.x * (gc->transform.cullEye.x - v->obj.x) +
		      v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
		      v->normal.z * (gc->transform.cullEye.z - v->obj.z);
	    } else {
		dot = v->normal.x * gc->transform.cullEye.x +
		      v->normal.y * gc->transform.cullEye.y +
		      v->normal.z * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.copyTexCoord)(gc, tp, v);
	    (*gc->vertexArray.copyIndex)(gc, ip, v);
	}
	ip += gc->vertexArray.ip.stride;
	tp += gc->vertexArray.tp[0].stride;
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElementsAll(__GLcontext *gc, GLint offset, 
			       GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *cp, *ip, *ep, *tp, *np, *vp;
    int i;

    cp = (const GLubyte *) gc->vertexArray.cp.pointer +
				first * gc->vertexArray.cp.stride;
    ip = (const GLubyte *) gc->vertexArray.ip.pointer +
				first * gc->vertexArray.ip.stride;
    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    ep = (const GLubyte *) gc->vertexArray.ep.pointer +
				first * gc->vertexArray.ep.stride;
    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
		dot = v->normal.x * (gc->transform.cullEye.x - v->obj.x) +
		      v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
		      v->normal.z * (gc->transform.cullEye.z - v->obj.z);
	    } else {
		dot = v->normal.x * gc->transform.cullEye.x +
		      v->normal.y * gc->transform.cullEye.y +
		      v->normal.z * gc->transform.cullEye.z;
	    }
	    v->flagBits = gc->vertexArray.cullCode[dot <= 0.0F] | 
		gc->vertexArray.validateMask;
	} else {
	    v->flagBits = gc->vertexArray.validateMask;
	}
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.copyEdgeFlag)(gc, ep, v);
	    (*gc->vertexArray.copyTexCoord)(gc, tp, v);
	    (*gc->vertexArray.copyIndex)(gc, ip, v);
	    (*gc->vertexArray.copyColor)(gc, cp, v);
	}
	cp += gc->vertexArray.cp.stride;
	ip += gc->vertexArray.ip.stride;
	tp += gc->vertexArray.tp[0].stride;
	ep += gc->vertexArray.ep.stride;
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	if (!(v->flagBits & __GL_CLIP_ALL_MASK)) {
	    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}
    }
}

static void CompileElementsAllNoCull(__GLcontext *gc, GLint offset, 
				     GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    const GLubyte *cp, *ip, *ep, *tp, *np, *vp;
    int i;

    cp = (const GLubyte *) gc->vertexArray.cp.pointer +
				first * gc->vertexArray.cp.stride;
    ip = (const GLubyte *) gc->vertexArray.ip.pointer +
				first * gc->vertexArray.ip.stride;
    tp = (const GLubyte *) gc->vertexArray.tp[0].pointer +
				first * gc->vertexArray.tp[0].stride;
    ep = (const GLubyte *) gc->vertexArray.ep.pointer +
				first * gc->vertexArray.ep.stride;
    np = (const GLubyte *) gc->vertexArray.np.pointer +
				first * gc->vertexArray.np.stride;
    vp = (const GLubyte *) gc->vertexArray.vp.pointer +
				first * gc->vertexArray.vp.stride;

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.copyVertex)(gc, vp, v);
	(*gc->vertexArray.copyNormal)(gc, np, v);
	(*gc->vertexArray.copyEdgeFlag)(gc, ep, v);
	(*gc->vertexArray.copyTexCoord)(gc, tp, v);
	(*gc->vertexArray.copyIndex)(gc, ip, v);
	(*gc->vertexArray.copyColor)(gc, cp, v);
	v->flagBits = gc->vertexArray.validateMask;
	cp += gc->vertexArray.cp.stride;
	ip += gc->vertexArray.ip.stride;
	tp += gc->vertexArray.tp[0].stride;
	ep += gc->vertexArray.ep.stride;
	np += gc->vertexArray.np.stride;
	vp += gc->vertexArray.vp.stride;
    }

    for (i=0, v=vb; i<count; ++i, ++v) {
	(*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v) | __GL_HAS_CLIP;
    }
}

/* wrapper for compileElements when it needs to deal with element indexes */
static void CompileElementsIndexed(__GLcontext *gc, GLint offset, 
				   GLint first, GLsizei count, 
				   GLuint *elements)
{
    if (elements) {
	int i;

	for (i=0; i<count; ++i) {
	    (*gc->vertexArray.compileElements)(gc, offset+i, elements[first+i], 1);
	}
    } else {
	
	(*gc->vertexArray.compileElements)(gc, offset, first, count);
    }
}

/* wrapper for compileElements when it needs to deal with silhouette vertexes */
static void CompileElementsSilhouette(__GLcontext *gc, __GLvertex *v, 
				      GLint index)
{
    int offset = v - gc->vertexArray.varrayPtr;
    GLuint oldFlags = v->flagBits & (__GL_VERTEX_EDGE_FLAG | __GL_VERTEX_CULL_FLAG);

    (*gc->vertexArray.compileElementsNoCull)(gc, offset, index, 1);
    v->flagBits &= ~(__GL_VERTEX_EDGE_FLAG | __GL_VERTEX_CULL_FLAG) | oldFlags;
}

/* This is used only by the vertex cache! */
void __glCompileElements_NoCopy(__GLcontext *gc, GLint offset, 
				GLint first, GLsizei count)
{
    __GLtransform *tr = gc->transform.modelView; 
    __GLvertex *v, *vb = gc->vertexArray.varrayPtr + offset;
    int i;
    GLuint flagBits;

    for (i=0, v=vb; i<count; ++i, ++v) {
	flagBits = v->flagBits;
	if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) {
	    __GLfloat dot;
            if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL_LOCAL) {
                dot =   v->normal.x * (gc->transform.cullEye.x - v->obj.x) + 
			v->normal.y * (gc->transform.cullEye.y - v->obj.y) +
			v->normal.z * (gc->transform.cullEye.z - v->obj.z);
            } else {
                dot =   v->normal.x * gc->transform.cullEye.x + 
			v->normal.y * gc->transform.cullEye.y +
			v->normal.z * gc->transform.cullEye.z;
            }
	    flagBits |= (gc->vertexArray.cullCode[dot <= 0.0F] | 
			 gc->vertexArray.validateMask);
	} else {
	    flagBits |= gc->vertexArray.validateMask;
	}

	if ((flagBits & __GL_CLIP_ALL_MASK) == 0) {
	  (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
	  flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
	}

	v->flagBits = flagBits;
    }
}

/* This is used only by the vertex cache! */
void __glCompileElementsSilhouette_NoCopy(__GLcontext *gc, 
					  __GLvertex *v, GLint index)
{
    __GLtransform *tr = gc->transform.modelView; 

    (*gc->vertexArray.xf)(&v->clip, &v->obj.x, &tr->mvp);
    v->flagBits |= (*gc->vertexArray.clipCheck)(gc, v);
}

#pragma optimize("", on)

#define __NORMAL 1
#define __COLOR 2
#define __TEX 4

static void (*RGBCompileProcs[8])(__GLcontext *, GLint, GLint, GLsizei) = 
{
    CompileElements,	/* none */
    CompileElementsN, 	/* NORMAL */

    CompileElementsC, 	/* COLOR */
    CompileElementsNC, 	/* NORMAL | COLOR */

    CompileElementsT, 	/* TEX */
    CompileElementsNT, 	/* NORMAL | TEX */

    CompileElementsCT, 	/* COLOR | TEX */
    CompileElementsNCT, /* NORMAL | COLOR | TEX */
};

static void (*CICompileProcs[8])(__GLcontext *, GLint, GLint, GLsizei) = 
{
    CompileElements,	/* none */
    CompileElementsN,	/* NORMAL */
    CompileElementsI,	/* COLOR */
    CompileElementsNI, 	/* NORMAL | COLOR */

    CompileElementsT, 	/* TEX */
    CompileElementsNT, 	/* NORMAL | TEX */

    CompileElementsIT, 	/* COLOR | TEX */
    CompileElementsNIT, /* NORMAL | COLOR | TEX */
};

static void (*RGBInterleavedCompileProcs[8])(__GLcontext *, GLint, 
					     GLint, GLsizei) = 
{
    CompileElements,		/* none */
    CompileElements_N3F_V3F,	/* NORMAL */

    CompileElementsC,		/* COLOR */
    CompileElementsNC,		/* NORMAL | COLOR */

    CompileElements_T2F_V3F,	/* TEX */
    CompileElements_T2F_N3F_V3F,/* NORMAL | TEX */

    CompileElementsCT,		/* COLOR | TEX */
    CompileElementsNCT,		/* NORMAL | COLOR | TEX */
};

static void (*CIInterleavedCompileProcs[8])(__GLcontext *, GLint, 
					    GLint, GLsizei) = 
{
    CompileElements,		/* none */
    CompileElements_N3F_V3F,	/* NORMAL */
    CompileElementsI,		/* COLOR */
    CompileElementsNI,		/* NORMAL | COLOR */

    CompileElements_T2F_V3F,	/* TEX */
    CompileElements_T2F_N3F_V3F,/* NORMAL | TEX */

    CompileElementsIT,		/* COLOR | TEX */
    CompileElementsNIT,		/* NORMAL | COLOR | TEX */
};

#if defined(__GL_CODEGEN) && defined(i386)
static void CompileElementsVarrayWrapper(__GLcontext *gc, GLint offset,
					 GLint first, GLsizei count)
{
    __GLvertArrayMachine *va = &gc->vertexArray;    
    void (*proc)(__GLcontext*, GLint, GLint, GLsizei);

    proc = __glGenerateVcacheCompile(gc, va->vp.size, 
				     va->compileIndex & (__GL_GEOM_OG_TEX |
							 __GL_GEOM_OG_COLOR));
    va->compileElements = va->compileElementsArray = proc;
    (*proc)(gc, offset, first, count);
}

static void DrawVerticesTstripWrapper(__GLcontext *gc, GLint first,
				      GLsizei count, GLuint *elements)
{
    void (*proc)(__GLcontext*, GLint, GLsizei, GLuint*);

    proc = __glGenerateVcacheDrawVertices(gc, GL_TRIANGLE_STRIP, 
					  (GLboolean) (elements != 0));
    gc->vertexArray.drawVertices[GL_TRIANGLE_STRIP] = proc;
    (*proc)(gc, first, count, elements);
}
#endif

void __glGenericPickVertexArrayEnables(__GLcontext *gc)
{
    __GLvertArrayMachine *va = &gc->vertexArray;
    GLbitfield interleavedMask = 0;

    /*
    ** Pick transform/clipcheck/validate procs
    */
    switch (va->vp.size) {
    case 2:
	va->xf = gc->transform.modelView->mvp.xf2;
	va->clipCheck = gc->procs.clipCheck2;
	va->validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_2D_INDEX];
	va->validateMask = __GL_VERTEX_2D_SIZE;
	break;
    case 3:
	va->xf = gc->transform.modelView->mvp.xf3;
	va->clipCheck = gc->procs.clipCheck3;
	va->validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_3D_INDEX];
	va->validateMask = __GL_VERTEX_3D_SIZE;
	break;
    case 4:
	va->xf = gc->transform.modelView->mvp.xf4;
	va->clipCheck = gc->procs.clipCheck4;
	va->validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_4D_INDEX];
	va->validateMask = __GL_VERTEX_4D_SIZE;
	break;
    default:
	assert(0);
	break;
    }

    if (va->compileMask & va->mask & VERTARRAY_V_MASK) {
	va->copyVertex = va->vp.copy;
	if (va->copyVertex == 
	    (void (*)(__GLcontext*,const void *,__GLvertex *)) CopyVertex3f) {
	    interleavedMask |= VERTARRAY_V_MASK;
	}
	va->controlWord &= ~VERTARRAY_CW_NO_VERTEX;
    } else {
	va->controlWord |= VERTARRAY_CW_NO_VERTEX;
    }

    if (va->compileMask & va->mask & VERTARRAY_N_MASK) {
	va->copyNormal = va->np.copy;
	if ((va->copyNormal == 
	     (void (*)(__GLcontext*,const void*,__GLvertex*)) CopyNormal3f) && 
	    (va->np.stride == va->vp.stride)) {
	    interleavedMask |= VERTARRAY_N_MASK;
	}
    } else {
	va->copyNormal = (void (*)(__GLcontext*,const void *,__GLvertex *))
	    CopyCurrentNormal;
    }

    if (va->compileMask & va->mask & VERTARRAY_C_MASK) {
	va->copyColor = va->cp.copy;
	if ((va->copyColor == 
	     (void (*)(__GLcontext*,const void*,__GLvertex*)) CopyColor3f) && 
	    (va->cp.stride == va->vp.stride)) {
	    interleavedMask |= (VERTARRAY_C_MASK | VERTARRAY_I_MASK);
	}
    } else {
	va->copyColor = (void (*)(__GLcontext*,const void*,__GLvertex *))
	    CopyCurrentColor;
    }

    if (va->compileMask & va->mask & VERTARRAY_I_MASK) {
	va->copyIndex = va->ip.copy;
	if ((va->copyIndex == 
	     (void (*)(__GLcontext*,const void*,__GLvertex *)) CopyIndexf) && 
	    (va->ip.stride == va->vp.stride)) {
	    interleavedMask |= (VERTARRAY_C_MASK | VERTARRAY_I_MASK);
	}
    } else {
	va->copyIndex = 
	    (void (*)(__GLcontext*,const void*,__GLvertex*)) CopyCurrentIndex;
    }

    if (va->compileMask & va->mask & VERTARRAY_T_MASK) {
	va->copyTexCoord = va->tp[0].copy;
	if ((va->copyTexCoord == 
	     (void (*)(__GLcontext*,const void*,__GLvertex*)) CopyTexCoord2f) 
	    && (va->tp[0].stride == va->vp.stride)) {
	    interleavedMask |= VERTARRAY_T_MASK;
	}
    } else {
	va->copyTexCoord = (void (*)(__GLcontext*,const void*,__GLvertex*)) 
	    CopyCurrentTexCoord;
    }

    if (va->compileMask & va->mask & VERTARRAY_E_MASK) {
	va->copyEdgeFlag = va->ep.copy;
    } else {
	va->copyEdgeFlag = (void (*)(__GLcontext*,const void*,__GLvertex*))
	    CopyCurrentEdgeFlag;
    }

    if (va->compileIndex < 0) {
	va->compileElements = CompileElementsAll;
    } else {
	if (va->compileMask == interleavedMask) {
	    if (gc->modes.rgbMode) {
		va->compileElements =
			RGBInterleavedCompileProcs[va->compileIndex];
	    } else {
		va->compileElements =
			CIInterleavedCompileProcs[va->compileIndex];
	    }
	} else {
	    if (gc->modes.rgbMode) {
		va->compileElements = RGBCompileProcs[va->compileIndex];
	    } else {
		va->compileElements = CICompileProcs[va->compileIndex];
	    }
	}
    }

    if (va->controlWord & VERTARRAY_CW_LOCKED) {
	/* resize vertex buffer to allow entrire array to be precompiled */
	if (AllocateVbuf(gc, va->start, va->count)) {
	    /* vertex buffer resized okay, allow precompile */
	    va->controlWord |= VERTARRAY_CW_ALLOW_PRECOMPILE;
	} else {
	    /* vertex buffer resize failed, don't allow precompile */
	    va->controlWord &= ~VERTARRAY_CW_ALLOW_PRECOMPILE;
	}
    } else {
	/* restore vertex buffer (possibly free up some memory) */
	(void) AllocateVbuf(gc, 0, 0);
	va->controlWord &= ~VERTARRAY_CW_ALLOW_PRECOMPILE;
    }

    /* Vertex state always needs to be recompiled after validation */
    va->controlWord |= VERTARRAY_CW_NEEDS_COMPILE;

#if defined(__GL_CODEGEN) && defined(i386)
    if ((gc->renderMode == GL_RENDER)
#if defined(__GL_USE_ASMCODE) && defined(__GL_USE_K3DCODE)
	&& ((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) == 0)
#endif /* __GL_USE_ASMCODE && __GL_USE_K3DCODE */
	) {
#if defined(__GL_ENVVARS)
	if (!disable_og_varray) {
	    va->compileElements = CompileElementsVarrayWrapper;
	}
	if (!disable_og_drawvertexes) {
	    va->drawVertices[GL_TRIANGLE_STRIP] = DrawVerticesTstripWrapper;
	}
#else /* __GL_ENVVARS */
	va->compileElements = CompileElementsVarrayWrapper;
	va->drawVertices[GL_TRIANGLE_STRIP] = DrawVerticesTstripWrapper;
#endif
    }
#endif
}

void __glGenericPickVertexArrayProcs(__GLcontext *gc)
{
    __GLvertArrayMachine *va = &gc->vertexArray;
    GLuint enables = gc->state.enables.general;

    /*
    ** Check for unsupported modes and that the fastpath isn't disabled.
    ** TBD-- fix colorMaterial for the fast paths.
    */
    if (((enables & __GL_LIGHTING_ENABLE) &&
	 (enables & __GL_COLOR_MATERIAL_ENABLE)))
    {
	va->controlWord |= VERTARRAY_CW_SLOWPATH;
    } else {
	va->controlWord &= ~VERTARRAY_CW_SLOWPATH;
    }

    /*
    ** Set up for vertex culling.
    */
    if (enables & __GL_CULL_VERTEX_ENABLE) {
	if (gc->state.transform.objEyeSpecified) {
	    if (gc->state.transform.eyePosObj.w != 0.0F) {
		va->controlWord |= VERTARRAY_CW_VX_CULL_LOCAL;
	    } else {
		va->controlWord &= ~VERTARRAY_CW_VX_CULL_LOCAL;
	    }
	} else {
	    if (gc->state.transform.eyePos.w != 0.0F) {
		va->controlWord |= VERTARRAY_CW_VX_CULL_LOCAL;
	    } else {
		va->controlWord &= ~VERTARRAY_CW_VX_CULL_LOCAL;
	    }
	}

	switch (gc->state.polygon.cull) {
	case GL_FRONT:
	    va->cullCode[__GL_FRONTFACE] = __GL_VERTEX_CULL_FLAG;
	    va->cullCode[__GL_BACKFACE] = 0;
	    break;
	case GL_BACK:
	    va->cullCode[__GL_FRONTFACE] = 0;
	    va->cullCode[__GL_BACKFACE] = __GL_VERTEX_CULL_FLAG;
	    break;
	case GL_FRONT_AND_BACK:
	    va->cullCode[__GL_FRONTFACE] = __GL_VERTEX_CULL_FLAG;
	    va->cullCode[__GL_BACKFACE] = __GL_VERTEX_CULL_FLAG;
	    break;
	default:
	    assert(0);
	    break;
	}

	va->controlWord |= VERTARRAY_CW_VX_CULL;
    } else {
	va->controlWord &= ~(VERTARRAY_CW_VX_CULL | VERTARRAY_CW_VX_CULL_LOCAL);
    }

    /*
    ** Pick compile procs
    */
    if ((gc->renderMode == GL_FEEDBACK) ||
	(gc->state.polygon.frontMode != GL_FILL) ||
	(gc->state.polygon.backMode != GL_FILL))
    {
	/* Need everything */
	va->compileMask =
	    VERTARRAY_V_MASK | VERTARRAY_N_MASK |
	    VERTARRAY_C_MASK | VERTARRAY_I_MASK |
	    VERTARRAY_T_MASK | VERTARRAY_E_MASK;
	va->compileIndex = -1;
    } else {
	va->compileMask = VERTARRAY_V_MASK;
	va->compileIndex = 0;
	if (enables & __GL_LIGHTING_ENABLE) {
	    va->compileMask |= VERTARRAY_N_MASK;
	    va->compileIndex |= __NORMAL;
	} else {
	    va->compileMask |= (VERTARRAY_C_MASK | VERTARRAY_I_MASK);
	    va->compileIndex |= __COLOR;
	}
	if (enables & __GL_CULL_VERTEX_ENABLE) {
	    va->compileMask |= VERTARRAY_N_MASK;
	    va->compileIndex |= __NORMAL;
	}
	if (gc->texture.textureEnabled) {
	    va->compileMask |= VERTARRAY_T_MASK;
	    va->compileIndex |= __TEX;
	    if (gc->state.enables.texture[0] & __GL_TEXTURE_GEN_S_ENABLE) {
		if (gc->state.texture.active->s.mode == GL_SPHERE_MAP) {
		    va->compileMask |= VERTARRAY_N_MASK;
		    va->compileIndex |= __NORMAL;
		}
	    }
	    if (gc->state.enables.texture[0] & __GL_TEXTURE_GEN_T_ENABLE) {
		if (gc->state.texture.active->t.mode == GL_SPHERE_MAP) {
		    va->compileMask |= VERTARRAY_N_MASK;
		    va->compileIndex |= __NORMAL;
		}
	    }
	}
    }

    va->compileElementsIndexed = CompileElementsIndexed;
    va->compileElementsNoCull = CompileElementsAllNoCull;
    va->compileElementsSilhouette = CompileElementsSilhouette;

    __glGenericPickVertexArrayEnables(gc);

    /* let's assume that everything uses locking */
    va->drawVerticesLockMask = ~0;
}

/************************************************************************/

static GLboolean AllocateVbuf(__GLcontext *gc, int first, int count)
{
    int bufSize, currentBufSize;

    bufSize = count * sizeof(__GLvertex);
    currentBufSize = gc->vertexArray.varrayBufSize;

    if (bufSize > currentBufSize) {
	int blockSize = gc->vertexArray.blockSize;
	__GLvertex *vBuf = gc->vertexArray.varrayBuf;

	/* smallest # of blocks that will contain <count> vertexes */
	bufSize = ((bufSize + (blockSize-1)) / blockSize) * blockSize;

	/* Align to 32byte cache line */
	bufSize += 32;

	/* allocate buffer */
	if (currentBufSize && vBuf) {
	    vBuf = (__GLvertex *)
		(*gc->imports.realloc)(gc, vBuf, (size_t)bufSize);
	} else {
	    vBuf = (__GLvertex *)
		(*gc->imports.malloc)(gc, (size_t)bufSize);
	}

	/* This breaks 'realloc' and 'free' later */
#if 0
	/* Align to 32byte cache line */
	vBuf = (__GLvertex *)(((unsigned int)vBuf & 0xFFFFFFE0UL) + 32);
#endif

	gc->vertexArray.varrayBuf = vBuf;
	gc->vertexArray.varrayBufSize = bufSize;

	/* Initialize color pointers */
	{
	    __GLvertex *vx;
	    int i, n = bufSize / sizeof(__GLvertex);

	    for (i=0, vx = vBuf; i<n; i++, vx++) {
		vx->color = &vx->colors[__GL_FRONTFACE];
	    }
	}

	/* Update vcache pointer */
	gc->vertexCache.vertexCache = gc->vertexArray.varrayBuf;
    }

    /*
    ** If <first> is not zero, then the vertex array data is locked
    ** starting at a non-zero offset.  We could allocate <first>+<count>
    ** elements, but that seems wasteful.  Instead we offset the
    ** array pointer.
    */
    if (first) {
	gc->vertexArray.varrayPtr = gc->vertexArray.varrayBuf - first;
    } else {
	gc->vertexArray.varrayPtr = gc->vertexArray.varrayBuf;
    }

    return GL_TRUE;
}

void GLAPI __glim_LockArraysEXT(GLint start, GLint count)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (start < 0 || count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (gc->vertexArray.controlWord & VERTARRAY_CW_LOCKED) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    __GL_API_BLAND();

    gc->vertexArray.controlWord |= VERTARRAY_CW_LOCKED;
    gc->vertexArray.start = start;
    gc->vertexArray.count = count;

    /*
    ** Compilation is delayed until validation so it catches all of the 
    ** current state at glBegin.
    */
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

void GLAPI __glim_UnlockArraysEXT(void)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (!(gc->vertexArray.controlWord & VERTARRAY_CW_LOCKED)) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    __GL_API_BLAND();

    gc->vertexArray.controlWord &= ~VERTARRAY_CW_LOCKED;
    gc->vertexArray.start = gc->vertexArray.count = 0;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

typedef struct __GLinterleavedFormatRec {
    GLboolean enableTexCoord;
    GLboolean enableColor;
    GLboolean enableIndex;
    GLboolean enableNormal;
    GLint sizeTexCoord;
    GLint sizeColor;
    GLint sizeVertex;
    GLenum typeColor;
    GLint offsetColor;
    GLint offsetIndex;
    GLint offsetNormal;
    GLint offsetVertex;
    GLint stride;
} __GLinterleavedFormat;

#define __GL_SIZE_F \
	sizeof(GLfloat)

#define __GL_SIZE_C \
	(((4*sizeof(GLubyte) + (__GL_SIZE_F-1)) / __GL_SIZE_F) * __GL_SIZE_F)

#define __GL_SIZE_I \
	(((sizeof(GLuint) + (__GL_SIZE_F-1)) / __GL_SIZE_F) * __GL_SIZE_F)

static const __GLinterleavedFormat interleavedFormats[] = {
    {	/* V2F */
	GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, 0, 0, 2, 0,
	0, 0, 0, 0,
	2*__GL_SIZE_F,
    },
    {	/* V3F */
	GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, 0, 0, 3, 0,
	0, 0, 0, 0,
	3*__GL_SIZE_F,
    },
    {	/* C4UB_V2F */
	GL_FALSE,  GL_TRUE, GL_FALSE, GL_FALSE, 0, 4, 2, GL_UNSIGNED_BYTE,
	0, 0, 0, __GL_SIZE_C,
	__GL_SIZE_C+2*__GL_SIZE_F,
    },
    {	/* C4UB_V3F */
	GL_FALSE,  GL_TRUE, GL_FALSE, GL_FALSE, 0, 4, 3, GL_UNSIGNED_BYTE,
	0, 0, 0, __GL_SIZE_C,
	__GL_SIZE_C+3*__GL_SIZE_F,
    },
    {	/* C3F_V3F */
	GL_FALSE,  GL_TRUE, GL_FALSE, GL_FALSE, 0, 3, 3, GL_FLOAT,
	0, 0, 0, 3*__GL_SIZE_F,
	6*__GL_SIZE_F,
    },
    {	/* N3F_V3F */
	GL_FALSE, GL_FALSE, GL_FALSE,  GL_TRUE, 0, 0, 3, 0,
	0, 0, 0, 3*__GL_SIZE_F,
	6*__GL_SIZE_F,
    },
    {	/* C4F_N3F_V3F */
	GL_FALSE,  GL_TRUE, GL_FALSE,  GL_TRUE, 0, 4, 3, GL_FLOAT,
	0, 0, 4*__GL_SIZE_F, 7*__GL_SIZE_F,
	10*__GL_SIZE_F,
    },
    {	/* T2F_V3F */
	 GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE, 2, 0, 3, 0,
	0, 0, 0, 2*__GL_SIZE_F,
	5*__GL_SIZE_F,
    },
    {	/* T4F_V4F */
	 GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE, 4, 0, 4, 0,
	0, 0, 0, 4*__GL_SIZE_F,
	8*__GL_SIZE_F,
    },
    {	/* T2F_C4UB_V3F */
	 GL_TRUE,  GL_TRUE, GL_FALSE, GL_FALSE, 2, 4, 3, GL_UNSIGNED_BYTE,
	2*__GL_SIZE_F, 0, 0, __GL_SIZE_C+2*__GL_SIZE_F,
	__GL_SIZE_C+5*__GL_SIZE_F,
    },
    {	/* T2F_C3F_V3F */
	 GL_TRUE,  GL_TRUE, GL_FALSE, GL_FALSE, 2, 3, 3, GL_FLOAT,
	2*__GL_SIZE_F, 0, 0, 5*__GL_SIZE_F, 8*__GL_SIZE_F,
    },
    {	/* T2F_N3F_V3F */
	 GL_TRUE, GL_FALSE, GL_FALSE,  GL_TRUE, 2, 0, 3, 0,
	0, 0, 2*__GL_SIZE_F, 5*__GL_SIZE_F,
	8*__GL_SIZE_F,
    },
    {	/* T2F_C4F_N3F_V3F */
	 GL_TRUE,  GL_TRUE, GL_FALSE,  GL_TRUE, 2, 4, 3, GL_FLOAT,
	2*__GL_SIZE_F, 0, 6*__GL_SIZE_F, 9*__GL_SIZE_F,
	12*__GL_SIZE_F,
    },
    {	/* T4F_C4F_N3F_V4F */
	 GL_TRUE,  GL_TRUE, GL_FALSE,  GL_TRUE, 4, 4, 4, GL_FLOAT,
	4*__GL_SIZE_F, 0, 8*__GL_SIZE_F, 11*__GL_SIZE_F,
	15*__GL_SIZE_F,
    },
};

void GLAPI __glim_InterleavedArrays(GLenum format, GLsizei stride, 
				    const GLvoid *pointer)
{
    const GLubyte *p = (const GLubyte *) pointer;
    const __GLinterleavedFormat *f;

    __GL_SETUP_NOT_IN_BEGIN();

    if (stride < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if ((int) format >= GL_V2F &&
	(int) format <= GL_T4F_C4F_N3F_V4F) {
	f = &interleavedFormats[(int) format - (int) GL_V2F];

    } else {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_BLAND();

    if (stride == 0) stride = f->stride;

    glDisableClientState(GL_EDGE_FLAG_ARRAY);

    if (f->enableTexCoord) {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(f->sizeTexCoord, GL_FLOAT, stride, p);
    } else {
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (f->enableColor) {
	glColorPointer(f->sizeColor, f->typeColor, stride, p+f->offsetColor);
	glEnableClientState(GL_COLOR_ARRAY);
    } else {
	glDisableClientState(GL_COLOR_ARRAY);
    }
    if (f->enableIndex) {
	glIndexPointer(GL_INT, stride, p+f->offsetIndex);
	glEnableClientState(GL_INDEX_ARRAY);
    } else {
	glDisableClientState(GL_INDEX_ARRAY);
    }
    if (f->enableNormal) {
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, p+f->offsetNormal);
    } else {
	glDisableClientState(GL_NORMAL_ARRAY);
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(f->sizeVertex, GL_FLOAT, stride, p+f->offsetVertex);

    gc->vertexArray.interleavedPointer = pointer;
    gc->vertexArray.interleavedFormat = format;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

static struct {
    GLint staticCount;
    GLint linkCount;
} PrimModes[] = {
    { 0, 0 },	/* GL_POINTS */
    { 0, 0 },	/* GL_LINES */
    { 1, 2 },	/* GL_LINE_LOOP */
    { 0, 1 },	/* GL_LINE_STRIP */
    { 0, 0 },	/* GL_TRIANGLES */
    { 0, 2 },	/* GL_TRIANGL_STRIP */
    { 1, 2 },	/* GL_TRIANGLE_FAN */
    { 0, 0 },	/* GL_QUADS */
    { 0, 2 },	/* GL_QUAD_STRIP */
    { 1, 2 },	/* GL_POLYGON */
};

static GLuint
indexToBatchIndex(__GLcontext *gc, GLint index)
{
    __GLvertArrayMachine *va = &gc->vertexArray;

    if ((index >= PrimModes[va->batchMode].staticCount) &&
					((va->batchIndex-va->batchFirst) > 0)) {
	index += va->batchIndex - PrimModes[va->batchMode].linkCount;
    } else {
	index += va->batchFirst;
    }

    if (va->batchElements) {
	return va->batchElements[index];
    } else {
	return index;
    }
}

static void
ClipLine(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1)
{
    {
	if ((v0->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v0 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v0, index);
	}
	if ((v1->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v1 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v1, index);
	}
    }

    if ((v0->flagBits & v1->flagBits & __GL_CLIP_ALL_MASK) == 0) {
	__glClipLine(gc, v1, v0);
    }
}

void 
__glVarrayClipTriangle(__GLcontext *gc, __GLvertex *v0, 
		   __GLvertex *v1, __GLvertex *v2)
{

    /* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
    {
	if ((v0->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v0 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v0, index);
	}
	if ((v1->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v1 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v1, index);
	}
	if ((v2->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v2 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v2, index);
	}
    }

    if ((v0->flagBits & v1->flagBits & v2->flagBits & __GL_CLIP_ALL_MASK)==0) {
	GLuint orCodes = (v0->flagBits | v1->flagBits | v2->flagBits) & 
	    __GL_CLIP_ALL_MASK;
	(*gc->procs.clipTriangle)(gc, v0, v1, v2, orCodes);
    }
}
static void
ClipQuad(__GLcontext *gc,
	 __GLvertex *v0, __GLvertex *v1, __GLvertex *v2, __GLvertex *v3)
{
     {
	if ((v0->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v0 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v0, index);
	}
	if ((v1->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v1 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v1, index);
	}
	if ((v2->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v2 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v2, index);
	}
	if ((v3->flagBits & __GL_HAS_CLIP) == 0) {
	    GLint index = v3 - gc->vertexArray.varrayPtr;

	    if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
		index = indexToBatchIndex(gc, index);
	    }
	    (*gc->vertexArray.compileElementsSilhouette)(gc, v3, index);
	}
    }

    if ((v0->flagBits & v1->flagBits & v2->flagBits & v3->flagBits & 
	 __GL_CLIP_ALL_MASK) == 0) {
	GLuint orCodes = (v0->flagBits | v1->flagBits | 
			  v2->flagBits | v3->flagBits) & 
	    __GL_CLIP_ALL_MASK;
	__GLvertex *iv[4];
	iv[0] = v0; iv[1] = v1; iv[2] = v2; iv[3] = v3;
	__glDoPolygonClip(gc, &iv[0], 4, orCodes);
    }
}

static void DrawVerticesTStrip2(__GLcontext *gc, GLint first, 
				GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    __GLvertex *v0 = (__GLvertex *) NULL, 
               *v1 = (__GLvertex *) NULL, 
               *v2 = (__GLvertex *) NULL;
    int i;

    if (count < 3) return;
    assert(count >= 3);
    assert((first == 0) || (elements == 0));

    v0 = vBuf + elements[0];
    v2 = vBuf + elements[1];

    for (i=2; i<count;) {
	if (i & 1) {
	    v0 = v2;
	} else {
	    v1 = v2;
	}
	v2 = vBuf + elements[i++];

	if (((v0->flagBits | v1->flagBits | v2->flagBits) & 
	     __GL_CLIP_ALL_MASK) == 0) {
	    (*gc->procs.renderTriangle)(gc, v0, v1, v2);
	}
    }
}

static void DrawVerticesTStrip(__GLcontext *gc, GLint first, 
			       GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    __GLvertex *v0 = (__GLvertex *) NULL, 
               *v1 = (__GLvertex *) NULL, 
               *v2 = (__GLvertex *) NULL;
    int batchFirst, batchIndex, batchCount, batchOffset;
    GLuint *batchElements;
    int i;

    if (count < 3) return;
    assert(count >= 3);
    assert((first == 0) || (elements == 0));


    batchFirst = batchIndex = first;
    batchCount = count;
    batchOffset = 0;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_TRIANGLE_STRIP;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
		vBuf[0] = vBuf[batchOffset+(batchCount-2)];
		vBuf[0].color = &vBuf[0].colors[__GL_FRONTFACE];
		vBuf[1] = vBuf[batchOffset+(batchCount-1)];
		vBuf[1].color = &vBuf[1].colors[__GL_FRONTFACE];
		batchOffset = 2;
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    if (gc->vertexArray.continuation)
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset + 2,
							batchIndex,
							batchCount - 2,
							elements);
	    else
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset,
							batchIndex,
							batchCount,
							elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}


	if (batchElements) {
	    v0 = vBuf + batchElements[0];
	    v2 = vBuf + batchElements[1];
	} else {
	    v0 = vBuf + batchFirst;
	    v2 = vBuf + batchFirst + 1;
	}
	v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
	v2->flagBits |= __GL_VERTEX_EDGE_FLAG;

	for (i=batchFirst+2; i<batchCount+batchOffset;) {
	    if (i & 1) {
		v0 = v2;
	    } else {
		v1 = v2;
	    }
	    if (batchElements) {
		v2 = vBuf + batchElements[i++];
	    } else {
		v2 = vBuf + i++;
	    }
	    v2->flagBits |= __GL_VERTEX_EDGE_FLAG;

	    gc->line.notResetStipple = GL_FALSE;
	    gc->vertex.provoking = v2;

	    if (((v0->flagBits | v1->flagBits | v2->flagBits) & 
		 __GL_CLIP_ALL_MASK) == 0) {
		(*gc->procs.renderTriangle)(gc, v0, v1, v2);
	    } else if (v0->flagBits & v1->flagBits & v2->flagBits & 
		       __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		__glVarrayClipTriangle(gc, v0, v1, v2);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

static void DrawVerticesTFan(__GLcontext *gc, GLint first, 
			     GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    __GLvertex *v0, *v1, *v2;
    int batchFirst, batchIndex, batchCount, batchOffset;
    GLuint *batchElements;
    int i;

    if (count < 3) return;
    assert(count >= 3);
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchOffset = 0;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_TRIANGLE_FAN;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
		vBuf[1] = vBuf[batchOffset+(batchCount-1)];
		vBuf[1].color = &vBuf[1].colors[__GL_FRONTFACE];
		batchOffset = 2;
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    if (gc->vertexArray.continuation)
		(*gc->vertexArray.compileElementsIndexed)(gc,
							  batchOffset + 2,
							  batchIndex,
							  batchCount - 2,
							  elements);
	    else
		(*gc->vertexArray.compileElementsIndexed)(gc,
							  batchOffset,
							  batchIndex,
							  batchCount,
							  elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	if (batchElements) {
	    v0 = vBuf + batchElements[0];
	    v2 = vBuf + batchElements[1];
	} else {
	    v0 = vBuf + batchFirst;
	    v2 = vBuf + batchFirst + 1;
	}
	v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
	v2->flagBits |= __GL_VERTEX_EDGE_FLAG;

	for (i=batchFirst+2; i<batchCount+batchOffset;) {
	    v1 = v2;
	    if (batchElements) {
		v2 = vBuf + batchElements[i++];
	    } else {
		v2 = vBuf + i++;
	    }
	    v2->flagBits |= __GL_VERTEX_EDGE_FLAG;

	    gc->line.notResetStipple = GL_FALSE;
	    gc->vertex.provoking = v2;

	    if (((v0->flagBits | v1->flagBits | v2->flagBits) & 
		 __GL_CLIP_ALL_MASK) == 0) {
		(*gc->procs.renderTriangle)(gc, v0, v1, v2);
	    } else if (v0->flagBits & v1->flagBits & v2->flagBits & 
		       __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		__glVarrayClipTriangle(gc, v0, v1, v2);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

static void DrawVerticesQStrip(__GLcontext *gc, GLint first, 
			       GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    __GLvertex *v0, *v1, *v2, *v3;
    int batchFirst, batchIndex, batchCount, batchOffset;
    GLuint *batchElements;
    int i;

    if (count < 4) return;
    count = count & 0xFFFFFFFE;
    assert((count >= 4) && (count % 2 == 0));
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchOffset = 0;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_QUAD_STRIP;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
		vBuf[0] = vBuf[batchOffset+(batchCount-2)];
		vBuf[0].color = &vBuf[0].colors[__GL_FRONTFACE];
		vBuf[1] = vBuf[batchOffset+(batchCount-1)];
		vBuf[1].color = &vBuf[1].colors[__GL_FRONTFACE];
		batchOffset = 2;
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    if (gc->vertexArray.continuation)
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset + 2,
							batchIndex,
							batchCount - 2,
							elements);
	    else
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset,
							batchIndex,
							batchCount,
							elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	if (batchElements) {
	    v2 = vBuf + batchElements[0];
	    v3 = vBuf + batchElements[1];
	} else {
	    v2 = vBuf + batchFirst;
	    v3 = vBuf + batchFirst + 1;
	}
	v2->flagBits |= __GL_VERTEX_EDGE_FLAG;
	v3->flagBits |= __GL_VERTEX_EDGE_FLAG;

	for (i=batchFirst+2; i<batchCount+batchOffset;) {
	    v0 = v2;
	    v1 = v3;
	    if (batchElements) {
		v2 = vBuf + batchElements[i++];
		v3 = vBuf + batchElements[i++];
	    } else {
		v2 = vBuf + i++;
		v3 = vBuf + i++;
	    }
	    v2->flagBits |= __GL_VERTEX_EDGE_FLAG;
	    v3->flagBits |= __GL_VERTEX_EDGE_FLAG;

	    gc->line.notResetStipple = GL_FALSE;
	    gc->vertex.provoking = v3;

	    if (((v0->flagBits | v1->flagBits | v2->flagBits | v3->flagBits) &
		 __GL_CLIP_ALL_MASK) == 0) {
		v1->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
		(*gc->procs.renderTriangle)(gc, v0, v1, v2);
		v1->flagBits |= __GL_VERTEX_EDGE_FLAG;
		v2->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
		(*gc->procs.renderTriangle)(gc, v2, v1, v3);
		v2->flagBits |= __GL_VERTEX_EDGE_FLAG;
	    } else if (v0->flagBits & v1->flagBits & 
		       v2->flagBits & v3->flagBits & __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		ClipQuad(gc, v0, v1, v3, v2);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

static void DrawVerticesLStrip(__GLcontext *gc, GLint first, 
			       GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    __GLvertex *v0, *v1;
    int batchFirst, batchIndex, batchCount, batchOffset;
    GLuint *batchElements;
    int i;

    if (count < 2) return;
    assert(count >= 2);
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchOffset = 0;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_LINE_STRIP;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
		vBuf[0] = vBuf[batchOffset+(batchCount-1)];
		vBuf[0].color = &vBuf[0].colors[__GL_FRONTFACE];
		batchOffset = 1;
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    if (gc->vertexArray.continuation)
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset + 1,
							batchIndex,
							batchCount - 1,
							elements);
	    else
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset,
							batchIndex,
							batchCount,
							elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	if (batchElements) {
	    v1 = vBuf + batchElements[0];
	} else {
	    v1 = vBuf + batchFirst;
	}
	DO_VALIDATE(gc, v1, gc->vertex.faceNeeds[__GL_FRONTFACE]);

	gc->line.notResetStipple = GL_FALSE;

	for (i=batchFirst+1; i<batchCount+batchOffset;) {
	    v0 = v1;
	    if (batchElements) {
		v1 = vBuf + batchElements[i++];
	    } else {
		v1 = vBuf + i++;
	    }

	    gc->vertex.provoking = v1;

	    if (((v0->flagBits | v1->flagBits) & __GL_CLIP_ALL_MASK) == 0) {
		DO_VALIDATE(gc, v1, gc->vertex.faceNeeds[__GL_FRONTFACE]);
		(*gc->procs.renderLine)(gc, v0, v1);
	    } else if (v0->flagBits & v1->flagBits & __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		ClipLine(gc, v0, v1);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

static void DrawVerticesLLoop(__GLcontext *gc, GLint first, 
			      GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    __GLvertex *v0, *v1, *vFirst;
    int batchFirst, batchIndex, batchCount, batchOffset;
    GLuint *batchElements;
    int i;

    if (count < 2) return;
    assert(count >= 2);
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchOffset = 0;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_LINE_LOOP;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
		vBuf[1] = vBuf[batchOffset+(batchCount-1)];
		vBuf[1].color = &vBuf[1].colors[__GL_FRONTFACE];
		batchFirst = 1;
		batchOffset = 1;
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    if (gc->vertexArray.continuation)
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset + 1,
							batchIndex,
							batchCount - 1,
							elements);
	    else
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset,
							batchIndex,
							batchCount,
							elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	if (batchElements) {
	    v1 = vBuf + batchElements[0];
	} else {
	    v1 = vBuf + batchFirst;
	}
	DO_VALIDATE(gc, v1, gc->vertex.faceNeeds[__GL_FRONTFACE]);

	gc->line.notResetStipple = GL_FALSE;

	vFirst = v1;

	for (i=batchFirst+1; i<batchCount+batchOffset;) {
	    v0 = v1;
	    if (batchElements) {
		v1 = vBuf + batchElements[i++];
	    } else {
		v1 = vBuf + i++;
	    }

	    gc->vertex.provoking = v1;

	    if (((v0->flagBits | v1->flagBits) & __GL_CLIP_ALL_MASK) == 0) {
		DO_VALIDATE(gc, v1, gc->vertex.faceNeeds[__GL_FRONTFACE]);
		(*gc->procs.renderLine)(gc, v0, v1);
	    } else if (v0->flagBits & v1->flagBits & __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		ClipLine(gc, v0, v1);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);

    /* Draw line from last to first. */
    if (((v1->flagBits | vFirst->flagBits) & __GL_CLIP_ALL_MASK) == 0) {
	(*gc->procs.renderLine)(gc, v1, vFirst);
    } else if ((v1->flagBits & vFirst->flagBits & __GL_CLIP_ALL_MASK) == 0) {
	ClipLine(gc, v1, vFirst);
    }
}

static void DrawVerticesTriangles(__GLcontext *gc, GLint first, 
				  GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    int batchFirst, batchIndex, batchCount;
    GLuint *batchElements;
    int i;

    if (count < 3) return;
    assert(count >= 3);
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_TRIANGLES;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    (*gc->vertexArray.compileElementsIndexed)(gc,
			    0, batchIndex, batchCount, elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	for (i=batchFirst; i+2<batchCount;) {
	    __GLvertex *v0, *v1, *v2;

	    if (batchElements) {
		v0 = vBuf + batchElements[i++];
		v1 = vBuf + batchElements[i++];
		v2 = vBuf + batchElements[i++];
	    } else {
		v0 = vBuf + i++;
		v1 = vBuf + i++;
		v2 = vBuf + i++;
	    }

	    gc->line.notResetStipple = GL_FALSE;
	    gc->vertex.provoking = v2;

	    if (((v0->flagBits | v1->flagBits | v2->flagBits) & 
		 __GL_CLIP_ALL_MASK) == 0) {
		(*gc->procs.renderTriangle)(gc, v0, v1, v2);
	    } else if (v0->flagBits & v1->flagBits & v2->flagBits & 
		       __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		__glVarrayClipTriangle(gc, v0, v1, v2);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

static void DrawVerticesQuads(__GLcontext *gc, GLint first, 
			      GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    int batchFirst, batchIndex, batchCount;
    GLuint *batchElements;
    int i;

    if (count < 4) return;
    count = count & 0xFFFFFFFC;
    assert((count >= 4) && (count % 4 == 0));
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_QUADS;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    (*gc->vertexArray.compileElementsIndexed)(gc,
			    0, batchIndex, batchCount, elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	for (i=batchFirst; i<batchCount;) {
	    __GLvertex *v0, *v1, *v2, *v3;

	    if (batchElements) {
		v0 = vBuf + batchElements[i++];
		v1 = vBuf + batchElements[i++];
		v2 = vBuf + batchElements[i++];
		v3 = vBuf + batchElements[i++];
	    } else {
		v0 = vBuf + i++;
		v1 = vBuf + i++;
		v2 = vBuf + i++;
		v3 = vBuf + i++;
	    }

	    gc->line.notResetStipple = GL_FALSE;
	    gc->vertex.provoking = v3;

	    if (((v0->flagBits | v1->flagBits | v2->flagBits | v3->flagBits) &
		 __GL_CLIP_ALL_MASK) == 0) {
		GLuint flagBits = v1->flagBits & __GL_VERTEX_EDGE_FLAG;
		v1->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
		(*gc->procs.renderTriangle)(gc, v0, v1, v3);
		v1->flagBits |= flagBits;
		flagBits = v3->flagBits & __GL_VERTEX_EDGE_FLAG;
		v3->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
		(*gc->procs.renderTriangle)(gc, v1, v2, v3);
		v3->flagBits |= flagBits;
	    } else if (v0->flagBits & v1->flagBits & 
		       v2->flagBits & v3->flagBits & __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		ClipQuad(gc, v0, v1, v2, v3);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

static void DrawVerticesPolygon(__GLcontext *gc, GLint first, 
				GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    __GLvertex *v0, *v1, *v2;
    int batchFirst, batchIndex, batchCount, batchOffset;
    GLuint *batchElements;
    int i, last = first+count;

    if (count < 3) return;
    assert(count >= 3);
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchOffset = 0;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_POLYGON;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
		vBuf[1] = vBuf[batchOffset+(batchCount-1)];
		vBuf[1].color = &vBuf[1].colors[__GL_FRONTFACE];
		batchOffset = 2;
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    if (gc->vertexArray.continuation)
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset + 2,
							batchIndex,
							batchCount - 2,
							elements);
	    else
	      (*gc->vertexArray.compileElementsIndexed)(gc,
							batchOffset,
							batchIndex,
							batchCount,
							elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	if (batchElements) {
	    v0 = vBuf + batchElements[0];
	    v2 = vBuf + batchElements[1];
	} else {
	    v0 = vBuf + batchFirst;
	    v2 = vBuf + batchFirst + 1;
	}

	gc->line.notResetStipple = GL_FALSE;
	gc->vertex.provoking = v0;

	for (i=batchFirst+2; i<batchCount+batchOffset; ++i) {
	    GLuint saveEdgeFlags0, saveEdgeFlags2;

	    v1 = v2;
	    if (batchElements) {
		v2 = vBuf + batchElements[i];
	    } else {
		v2 = vBuf + i;
	    }

	    saveEdgeFlags0 = v0->flagBits & __GL_VERTEX_EDGE_FLAG;
	    if (i+batchIndex != first+2) {
		v0->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	    }
	    saveEdgeFlags2 = v2->flagBits & __GL_VERTEX_EDGE_FLAG;
	    if (i+batchIndex != last-1) {
		v2->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	    }

	    if (((v0->flagBits | v1->flagBits | v2->flagBits) & 
		 __GL_CLIP_ALL_MASK) == 0) {
		(*gc->procs.renderTriangle)(gc, v0, v1, v2);
	    } else if (v0->flagBits & v1->flagBits & v2->flagBits & 
		       __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		__glVarrayClipTriangle(gc, v0, v1, v2);
	    }

	    v0->flagBits |= saveEdgeFlags0;
	    v2->flagBits |= saveEdgeFlags2;
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);

}

static void DrawVerticesLines(__GLcontext *gc, GLint first, 
			      GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    int batchFirst, batchIndex, batchCount;
    GLuint *batchElements;
    int i;

    if (count < 2) return;
    count = count & 0xFFFFFFFE;
    assert((count >= 2) && (count % 2 == 0));
    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
		/* if (gc->vertexArray.controlWord & VERTARRAY_CW_VX_CULL) */
		{
		    gc->vertexArray.batchMode = GL_LINES;
		    gc->vertexArray.batchFirst = first;
		    gc->vertexArray.batchElements = elements;
		}
	    } else {
		/* subsequent batch */
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    (*gc->vertexArray.compileElementsIndexed)(gc,
			    0, batchIndex, batchCount, elements);
	    gc->vertexArray.batchIndex = batchIndex;
	}

	for (i=batchFirst; i<batchCount;) {
	    __GLvertex *v0, *v1;

	    if (batchElements) {
		v0 = vBuf + batchElements[i++];
		v1 = vBuf + batchElements[i++];
	    } else {
		v0 = vBuf + i++;
		v1 = vBuf + i++;
	    }

	    gc->line.notResetStipple = GL_FALSE;
	    gc->vertex.provoking = v1;

	    if (((v0->flagBits | v1->flagBits) & __GL_CLIP_ALL_MASK) == 0) {
		DO_VALIDATE(gc, v0, gc->vertex.faceNeeds[__GL_FRONTFACE]);
		DO_VALIDATE(gc, v1, gc->vertex.faceNeeds[__GL_FRONTFACE]);
		(*gc->procs.renderLine)(gc, v0, v1);
	    } else if (v0->flagBits & v1->flagBits & __GL_CLIP_ALL_MASK) {
		continue; /* cull -or- trivial reject */
	    } else {
		ClipLine(gc, v0, v1);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

static void DrawVerticesPoints(__GLcontext *gc, GLint first, 
			       GLsizei count, GLuint *elements)
{
    __GLvertex *vBuf = (__GLvertex *) gc->vertexArray.varrayPtr;
    int batchFirst, batchIndex, batchCount;
    GLuint *batchElements;
    int i;

    assert((first == 0) || (elements == 0));

    batchFirst = batchIndex = first;
    batchCount = count;
    batchElements = elements;

    do {
	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (batchFirst == batchIndex) {
		/* first batch */
		batchFirst = 0;
		batchCount = gc->vertexArray.batchSize;
		batchElements = NULL;
	    } else {
		/* subsequent batch */
	    }
	    if (count < batchCount) {
		batchCount = count;
	    }
	    (*gc->vertexArray.compileElementsIndexed)(gc,
			    0, batchIndex, batchCount, elements);
	}

	for (i=batchFirst; i<batchCount;) {
	    __GLvertex *v0;

	    if (batchElements) {
		v0 = vBuf + batchElements[i++];
	    } else {
		v0 = vBuf + i++;
	    }

	    if ((v0->flagBits & __GL_CLIP_ALL_MASK) == 0) {
		DO_VALIDATE(gc, v0, 
		  gc->vertex.faceNeeds[__GL_FRONTFACE] | __GL_HAS_FRONT_COLOR);
		(*gc->procs.renderPoint)(gc, v0);
	    }
	}
	batchIndex += batchCount;
	count -= batchCount;
    } while (count > 0);
}

void __glInitVertexArrayState(__GLcontext *gc)
{
    GLint i;
#if defined(__GL_CODEGEN) && defined(i386)
    {
	char *env = (*gc->imports.getenv)(gc, "__GL_DISABLE_OG_VARRAY");
	if (env)
	    disable_og_varray = (GLboolean) (*gc->imports.atoi)(gc, env);

	env = (*gc->imports.getenv)(gc, "__GL_DISABLE_OG_DRAWVERTEXES");
	if (env)
	    disable_og_drawvertexes = (GLboolean) (*gc->imports.atoi)(gc, env);
    }
#endif /* __GL_CODEGEN && i386 */

    /*
     * Set up vertex array default values
     */
    gc->vertexArray.vp.size = 4;
    gc->vertexArray.vp.type = GL_FLOAT;
    gc->vertexArray.vp.proc = (void (GLAPI *)(const char *))__glGLAPINop1;
    gc->vertexArray.np.size = 3;
    gc->vertexArray.np.type = GL_FLOAT;
    gc->vertexArray.np.proc = (void (GLAPI *)(const char *))__glGLAPINop1;
    gc->vertexArray.cp.size = 4;
    gc->vertexArray.cp.type = GL_FLOAT;
    gc->vertexArray.cp.proc = (void (GLAPI *)(const char *))__glGLAPINop1;
    gc->vertexArray.ip.size = 1;
    gc->vertexArray.ip.type = GL_FLOAT;
    gc->vertexArray.ip.proc = (void (GLAPI *)(const char *))__glGLAPINop1;
    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	gc->vertexArray.tp[i].size = 4;
	gc->vertexArray.tp[i].type = GL_FLOAT;
	gc->vertexArray.tp[i].proc = (void (GLAPI *)(const char *))__glGLAPINop1;
    } 
    gc->vertexArray.ep.size = 1;
    gc->vertexArray.ep.type = GL_UNSIGNED_BYTE;
    gc->vertexArray.ep.proc = (void (GLAPI *)(const char *))__glGLAPINop1;
    gc->vertexArray.maxElementsVertices = INT_MAX;
    gc->vertexArray.maxElementsIndices = INT_MAX;

    gc->vertexArray.interleavedFormat = GL_NONE;

    gc->vertexArray.drawVertices[GL_POINTS] = DrawVerticesPoints;
    gc->vertexArray.drawVertices[GL_LINES] = DrawVerticesLines;
    gc->vertexArray.drawVertices[GL_LINE_LOOP] = DrawVerticesLLoop;
    gc->vertexArray.drawVertices[GL_LINE_STRIP] = DrawVerticesLStrip;
    gc->vertexArray.drawVertices[GL_TRIANGLES] = DrawVerticesTriangles;
    gc->vertexArray.drawVertices[GL_TRIANGLE_STRIP] = DrawVerticesTStrip;
    gc->vertexArray.drawVertices[GL_TRIANGLE_FAN] = DrawVerticesTFan;
    gc->vertexArray.drawVertices[GL_QUADS] = DrawVerticesQuads;
    gc->vertexArray.drawVertices[GL_QUAD_STRIP] = DrawVerticesQStrip;
    gc->vertexArray.drawVertices[GL_POLYGON] = DrawVerticesPolygon;

#if defined(__GL_CODEGEN) && defined(i386)
    if (
#if defined(__GL_ENVVARS)
	(!disable_og_drawvertexes) &&
#endif /* __GL_ENVVARS */
#if defined(__GL_USE_ASMCODE) && defined(__GL_USE_K3DCODE)
	((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) == 0) &&
#endif /* __GL_USE_ASMCODE && __GL_USE_K3DCODE */
	1) {
	gc->vertexArray.drawVertices[GL_TRIANGLE_STRIP] = 
	    DrawVerticesTstripWrapper;
    }
#endif

    /*
    ** The batch size must be a multiple of 2, 3 and 4 to simplify
    ** processing arrays of independent lines, tris, and quads.
    ** It should also be at least as large as the vertex cache, so
    ** that vertexes in the vcache are only batched once.
    */
    gc->vertexArray.batchSize = __GL_VCACHE_MAXSIZE;
    assert(gc->vertexArray.batchSize >= __GL_VCACHE_MAXSIZE);
    assert((gc->vertexArray.batchSize % 3) == 0);
    assert((gc->vertexArray.batchSize % 4) == 0);

    /*
    ** Allocate a block of space for the internal vertex array.
    ** TBD-- this probably needs to be page aligned and sized
    ** appropriately.  For now it is a constant.
    */
    gc->vertexArray.blockSize = 4096;
    (void) AllocateVbuf(gc, 0, gc->vertexArray.batchSize+2);
}

void __glFreeVertexArrayState(__GLcontext *gc)
{
    if (NULL != gc->vertexArray.varrayBuf) {
	(*gc->imports.free)(gc, gc->vertexArray.varrayBuf);
	gc->vertexArray.varrayBuf = NULL;
	gc->vertexArray.varrayPtr = NULL;
	gc->vertexArray.varrayBufSize = 0;
    }
}

void GLAPI __glim_ArrayElement(GLint i) 
{
    __GL_SETUP();
    GLint index = gc->vertexArray.index;

    if (index & VERTARRAY_N_INDEX) 
	(*gc->vertexArray.np.proc)
	  (gc->vertexArray.np.pointer + i*gc->vertexArray.np.stride);
    if (index & VERTARRAY_C_INDEX) 
	(*gc->vertexArray.cp.proc)
	  (gc->vertexArray.cp.pointer + i*gc->vertexArray.cp.stride);
    if (index & VERTARRAY_T_INDEX) {
        GLint i;

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    if (gc->vertexArray.textureEnables & (1<<i)) {
		(*gc->vertexArray.tp[i].proc)
		  (gc->vertexArray.tp[i].pointer +
		  	i*gc->vertexArray.tp[i].stride);
	    }
	}
    }
    if (index & VERTARRAY_I_INDEX) 
	(*gc->vertexArray.ip.proc)
	  (gc->vertexArray.ip.pointer + i*gc->vertexArray.ip.stride);
    if (index & VERTARRAY_E_INDEX)
	(*gc->vertexArray.ep.proc)
	  (gc->vertexArray.ep.pointer + i*gc->vertexArray.ep.stride);
    if (index & VERTARRAY_V_INDEX) 
	(*gc->vertexArray.vp.proc)
	  (gc->vertexArray.vp.pointer + i*gc->vertexArray.vp.stride);
}

void GLAPI __glim_ArrayElementEXT(GLint i)
{
    __glim_ArrayElement(i);
}

void GLAPI __glim_DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    GLuint lockBuffers;
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

    /* check that first and count are positive */
    if (first < 0 || count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (mode > GL_POLYGON) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if (count == 0) return;

    lockBuffers = gc->vertexArray.drawVerticesLockMask & (1<<mode);

    if (lockBuffers) {
	__GL_LOCK_RENDER_BUFFERS(gc);
    }

    if (gc->vertexArray.controlWord & 
		(VERTARRAY_CW_SLOWPATH | VERTARRAY_CW_NO_VERTEX)) {
	int i, last = first+count;

	glBegin(mode);
	for (i=first; i<last; i++) {
	    glArrayElement(i);
	}
	glEnd();

    } else { 
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_ALLOW_PRECOMPILE) {
		(*gc->vertexArray.compileElements)(gc, gc->vertexArray.start,
				gc->vertexArray.start, gc->vertexArray.count);
		gc->vertexArray.controlWord &= ~VERTARRAY_CW_NEEDS_COMPILE;
	    }
	}

	drawVertices = gc->vertexArray.drawVertices[mode];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, first, count, NULL);
    }

    if (lockBuffers) {
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void GLAPI __glim_DrawArraysEXT(GLenum mode,
				GLint first,
				GLsizei count)
{
    __glim_DrawArrays(mode, first, count);
}

void GLAPI __glim_DrawElements(GLenum mode, GLsizei count, GLenum type, 
			       const GLvoid *indices)
{
    GLuint *elements;
    GLuint lockBuffers;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

    /* check that count is positive */
    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (mode > GL_POLYGON) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    /*
    ** The optimized path is for unsigned int indices, but we
    ** can make the other types go through that path by converting
    ** the indices to unsigned int.  Later we will want to find
    ** a way to propagate the optimized path to all types.
    */
    switch(type) {
    case GL_UNSIGNED_BYTE:
	{
	    const GLubyte *byteElements = (const GLubyte *)indices;
	    int i;

	    elements = (GLuint *) gc->imports.malloc(gc, count*sizeof(GLuint));
	    for (i=0; i<count; i++) {
		elements[i] = byteElements[i];
	    }
	}
	break;
    case GL_UNSIGNED_SHORT:
	{
	    const GLushort *shortElements = (const GLushort *)indices;
	    int i;

	    elements = (GLuint *) gc->imports.malloc(gc, count*sizeof(GLuint));
	    for (i=0; i<count; i++) {
		elements[i] = shortElements[i];
	    }
	}
	break;
    case GL_UNSIGNED_INT:
	elements = (GLuint *)indices;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if (count == 0) goto done;

    lockBuffers = gc->vertexArray.drawVerticesLockMask & (1<<mode);

    if (lockBuffers) {
	__GL_LOCK_RENDER_BUFFERS(gc);
    }

    if (gc->vertexArray.controlWord &
		(VERTARRAY_CW_SLOWPATH | VERTARRAY_CW_NO_VERTEX)) {
	int i;

	glBegin(mode);
	for (i=0; i<count; i++) {
	    glArrayElement(elements[i]);
	}
	glEnd();

    } else {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	if (gc->vertexArray.controlWord & VERTARRAY_CW_NEEDS_COMPILE) {
	    if (gc->vertexArray.controlWord & VERTARRAY_CW_ALLOW_PRECOMPILE) {
		(*gc->vertexArray.compileElements)(gc, gc->vertexArray.start,
				gc->vertexArray.start, gc->vertexArray.count);
		gc->vertexArray.controlWord &= ~VERTARRAY_CW_NEEDS_COMPILE;
	    }
	}

	drawVertices = gc->vertexArray.drawVertices[mode];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, count, elements);
    }

    if (lockBuffers) {
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }

done:
    if ((GLvoid *) elements != indices) {
	gc->imports.free(gc, elements);
    }
}

void GLAPI __glim_DrawRangeElements(GLenum mode, GLuint start, GLuint end,
				    GLsizei count, GLenum type, 
				    const GLvoid *indices)
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

    if (end < start) {
        __glSetError(GL_INVALID_VALUE);
        return;
    }

    __glim_DrawElements(mode,count,type,indices);
}

/*
 * HOW THE POINTER SETUP CALLS WORK
 * The pointer setup calls are all table-driven whereever possible.  This 
 * is what happens:
 * 	stride is checked for <= 0
 *	count is checked for <= 0
 *	size is range-checked (for example, as in [1..4] for tex coords)
 *	type is range-checked for >= GL_BYTE and <= GL_DOUBLE.  This 
 *		assures we will not core dump when we do table lookups 
 *		a few lines hence
 *	The function pointer is looked up in the appropriate table (normal, 
 *		vertex, color, index, or tex coord)
 *	If the function pointer is null, type must not be valid (since we 
 *		already checked for all other errors which could cause this), 
 *		so set an error
 *	Update the pointer, size, type, stride, userStride, and count in 
 *		the context (the userStride differs from the stride in that 
 * 		if the usr passed in a 0 (for tightly packed arrays), the 
 * 		userStride will still be 0 but the stride will be the 
 *		appropriate value to use in the drawing loops.  This is
 *		needed so that the get commands will return the right thing.
 *	Update the signature.  This is a bitflag used in machine-dependent 
 *		implementations to find special cases.  It encodes the sizes 
 *		and types.
 */

/*
 * Stride table -- indexed by type and size
 */
static int stride_array[GL_DOUBLE - GL_BYTE + 1][5] = {
    0, sizeof(GLbyte),		2*sizeof(GLbyte),	3*sizeof(GLbyte),
    4*sizeof(GLbyte),
    0, sizeof(GLubyte), 	2*sizeof(GLubyte),      3*sizeof(GLubyte),
    4*sizeof(GLubyte),
    0, sizeof(GLshort), 	2*sizeof(GLshort),      3*sizeof(GLshort), 
    4*sizeof(GLshort),
    0, sizeof(GLushort), 	2*sizeof(GLushort),     3*sizeof(GLushort),
    4*sizeof(GLushort),
    0, sizeof(GLint), 		2*sizeof(GLint),        3*sizeof(GLint), 
    4*sizeof(GLint),
    0, sizeof(GLuint), 		2*sizeof(GLuint),       3*sizeof(GLuint),
    4*sizeof(GLuint),
    0, sizeof(GLfloat), 	2*sizeof(GLfloat),  	3*sizeof(GLfloat),
    4*sizeof(GLfloat),
    0, 0, 0,                    0,                   	0,
    0, 0, 0,                    0,                      0,
    0, 0, 0,                    0,                      0,
    0, sizeof(GLdouble),	2*sizeof(GLdouble),	3*sizeof(GLdouble),
    4*sizeof(GLdouble),
};

/*
 * Pointer table prototype.  Setting the table up as a structure and
 * then casting it to an array will help force people to enter the right 
 * function in the right spot since it will generate compiler warnings 
 * otherwise.  The final table will be indexed by type and size.
 */
typedef struct {
    /* functions used to unpack vertex arrays on the slow path */
    void (GLAPI *byte_funcs[5])(const GLbyte *);
    void (GLAPI *ubyte_funcs[5])(const GLubyte *);
    void (GLAPI *short_funcs[5])(const GLshort *);
    void (GLAPI *ushort_funcs[5])(const GLushort *);
    void (GLAPI *int_funcs[5])(const GLint *);
    void (GLAPI *uint_funcs[5])(const GLuint *);
    void (GLAPI *float_funcs[5])(const GLfloat *);
    /* skip over GL_2_BYTES...GL_4_BYTES */
    void (GLAPI *filler[3*5])(void);
    void (GLAPI *double_funcs[5])(const GLdouble *);

    /* functions used to unpack vertex arrays on the fast path */
    void (*byte_copy_funcs[5])(__GLcontext *,
			       const GLbyte *, __GLvertex *);
    void (*ubyte_copy_funcs[5])(__GLcontext *,
				const GLubyte *, __GLvertex *);
    void (*short_copy_funcs[5])(__GLcontext *,
				const GLshort *, __GLvertex *);
    void (*ushort_copy_funcs[5])(__GLcontext *,
				 const GLushort *, __GLvertex *);
    void (*int_copy_funcs[5])(__GLcontext *,
			      const GLint *, __GLvertex *);
    void (*uint_copy_funcs[5])(__GLcontext *,
			       const GLuint *, __GLvertex *);
    void (*float_copy_funcs[5])(__GLcontext *,
				const GLfloat *, __GLvertex *);
    /* skip over GL_2_BYTES...GL_4_BYTES */
    void (*copy_filler[3*5])(void);
    void (*double_copy_funcs[5])(__GLcontext *,
				 const GLdouble *, __GLvertex *);
} func_array;

static func_array vertex_funcs = {
    0, 0, 0,       	0,        	0,
    0, 0, 0,        	0,              0,
    0, 0, glVertex2sv,  glVertex3sv,	glVertex4sv,
    0, 0, 0,         	0,              0,
    0, 0, glVertex2iv,	glVertex3iv,    glVertex4iv,
    0, 0, 0,          	0,              0,
    0, 0, glVertex2fv, 	glVertex3fv,    glVertex4fv,
    0, 0, 0,         	0,              0,
    0, 0, 0,          	0,              0,
    0, 0, 0,          	0,              0,
    0, 0, glVertex2dv,	glVertex3dv,    glVertex4dv,

    0, 0, 0,       	0,        	0,
    0, 0, 0,        	0,              0,
    0, 0, CopyVertex2s, CopyVertex3s,	CopyVertex4s,
    0, 0, 0,         	0,              0,
    0, 0, CopyVertex2i,	CopyVertex3i,   CopyVertex4i,
    0, 0, 0,          	0,              0,
    0, 0, CopyVertex2f,	CopyVertex3f,   CopyVertex4f,
    0, 0, 0,         	0,              0,
    0, 0, 0,          	0,              0,
    0, 0, 0,          	0,              0,
    0, 0, CopyVertex2d,	CopyVertex3d,    CopyVertex4d,
};
static void (GLAPI *(*vertex_func_table)[2][11][5])(const char *) = 
     (void (GLAPI *(*)[2][11][5])(const char *))(&vertex_funcs);

static func_array normal_funcs = {
    0, 0, 0,       	glNormal3bv,    0,
    0, 0, 0,        	0,              0,
    0, 0, 0,        	glNormal3sv,    0,
    0, 0, 0,         	0,              0,
    0, 0, 0,		glNormal3iv,    0,
    0, 0, 0,         	0,              0,
    0, 0, 0,  		glNormal3fv,    0,
    0, 0, 0,            0,              0,
    0, 0, 0,            0,              0,
    0, 0, 0,            0,              0,
    0, 0, 0,  		glNormal3dv,    0,

    0, 0, 0,       	CopyNormal3b,   0,
    0, 0, 0,        	0,              0,
    0, 0, 0,        	CopyNormal3s,   0,
    0, 0, 0,         	0,              0,
    0, 0, 0,		CopyNormal3i,   0,
    0, 0, 0,         	0,              0,
    0, 0, 0,  		CopyNormal3f,   0,
    0, 0, 0,            0,              0,
    0, 0, 0,            0,              0,
    0, 0, 0,            0,              0,
    0, 0, 0,  		CopyNormal3d,   0,
};
static void (GLAPI *(*normal_func_table)[2][11][5])(const char *) = 
     (void (GLAPI *(*)[2][11][5])(const char *))(&normal_funcs);

static func_array color_funcs = {
    0, 0, 0,		glColor3bv,	glColor4bv,
    0, 0, 0,		glColor3ubv,	glColor4ubv,
    0, 0, 0,		glColor3sv,	glColor4sv,
    0, 0, 0,		glColor3usv,	glColor4usv,
    0, 0, 0,		glColor3iv,	glColor4iv,
    0, 0, 0,		glColor3uiv,	glColor4uiv,
    0, 0, 0,		glColor3fv,	glColor4fv,
    0, 0, 0,		0,		0,
    0, 0, 0,		0,		0,
    0, 0, 0,		0,		0,
    0, 0, 0,		glColor3dv,	glColor4dv,

    0, 0, 0,		CopyColor3b,	CopyColor4b,
    0, 0, 0,		CopyColor3ub,	CopyColor4ub,
    0, 0, 0,		CopyColor3s,	CopyColor4s,
    0, 0, 0,		CopyColor3us,	CopyColor4us,
    0, 0, 0,		CopyColor3i,	CopyColor4i,
    0, 0, 0,		CopyColor3ui,	CopyColor4ui,
    0, 0, 0,		CopyColor3f,	CopyColor4f,
    0, 0, 0,		0,		0,
    0, 0, 0,		0,		0,
    0, 0, 0,		0,		0,
    0, 0, 0,		CopyColor3d,	CopyColor4d,
};
static void (GLAPI *(*color_func_table)[2][11][5])(const char *) = 
     (void (GLAPI *(*)[2][11][5])(const char *))(&color_funcs);     

/* eventually, this should not be a full-blown func_array
 * since only one column is used... */
static func_array index_funcs = {
    0, 0,		0,		0,		0,
    0, glIndexubv,	0,		0,		0,
    0, glIndexsv,	0,		0,		0,
    0, 0,		0,		0,		0,
    0, glIndexiv,	0,		0,		0,
    0, 0,		0,		0,		0,
    0, glIndexfv,	0,		0,		0,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, glIndexdv,	0,		0,		0,

    0, 0,		0,		0,		0,
    0, CopyIndexub,	0,		0,		0,
    0, CopyIndexs,	0,		0,		0,
    0, 0,		0,		0,		0,
    0, CopyIndexi,	0,		0,		0,
    0, 0,		0,		0,		0,
    0, CopyIndexf,	0,		0,		0,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, CopyIndexd,	0,		0,		0,
};
static void (GLAPI *(*index_func_table)[2][11][5])(const char *) = 
     (void (GLAPI *(*)[2][11][5])(const char *))(&index_funcs);     

static func_array tex_coord_funcs = {
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, glTexCoord1sv,	glTexCoord2sv,	glTexCoord3sv,	glTexCoord4sv,
    0, 0,		0,		0,		0,
    0, glTexCoord1iv,	glTexCoord2iv,	glTexCoord3iv,	glTexCoord4iv,
    0, 0,		0,		0,		0,
    0, glTexCoord1fv,	glTexCoord2fv,	glTexCoord3fv,	glTexCoord4fv,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, glTexCoord1dv,	glTexCoord2dv,	glTexCoord3dv,	glTexCoord4dv,

    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, CopyTexCoord1s,	CopyTexCoord2s,	CopyTexCoord3s,	CopyTexCoord4s,
    0, 0,		0,		0,		0,
    0, CopyTexCoord1i,	CopyTexCoord2i,	CopyTexCoord3i,	CopyTexCoord4i,
    0, 0,		0,		0,		0,
    0, CopyTexCoord1f,	CopyTexCoord2f,	CopyTexCoord3f,	CopyTexCoord4f,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, 0,		0,		0,		0,
    0, CopyTexCoord1d,	CopyTexCoord2d,	CopyTexCoord3d,	CopyTexCoord4d,
};
static void (GLAPI *(*tex_coord_func_table)[2][11][5])(const char *) = 
     (void (GLAPI *(*)[2][11][5])(const char *))(&tex_coord_funcs);     

/* type flag table -- indexed by type */
static int type_flag_table[] = {
    VERTARRAY_BYTE_FLAG,
    VERTARRAY_UNSIGNED_BYTE_FLAG,
    VERTARRAY_SHORT_FLAG,
    VERTARRAY_UNSIGNED_SHORT_FLAG,
    VERTARRAY_INT_FLAG,
    VERTARRAY_UNSIGNED_INT_FLAG,
    VERTARRAY_FLOAT_FLAG,
    0, 
    0,
    0,
    VERTARRAY_DOUBLE_FLAG,
};

void GLAPI __glim_VertexPointer(GLint size, GLenum type,
				GLsizei stride, const GLvoid *pointer)
{
    void (GLAPI *proc)(const char *);
    __GL_SETUP_NOT_IN_BEGIN();

    if (stride < 0 || size < 2 || size > 4) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (type < GL_BYTE || type > GL_DOUBLE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    proc = (*vertex_func_table)[0][type - GL_BYTE][size];
    if (proc == (void (GLAPI *)(const char *))0x0) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_BLAND();

    /* update the gc */
    gc->vertexArray.vp.proc = proc;
    gc->vertexArray.vp.copy =
	(void (*)(__GLcontext *, const void *, __GLvertex *))
	(*vertex_func_table)[1][type - GL_BYTE][size];
    gc->vertexArray.vp.pointer = pointer;
    gc->vertexArray.vp.size = size;
    gc->vertexArray.vp.type = type;
    gc->vertexArray.vp.stride = 
	stride ? stride : stride_array[type - GL_BYTE][size];
    gc->vertexArray.vp.userStride = stride;
    gc->vertexArray.signature = 
	(gc->vertexArray.signature & ~VERTARRAY_V_MASK) | 
	    (type_flag_table[type - GL_BYTE] << VERTARRAY_VTYPE_SHIFT) | 
		(size << VERTARRAY_VSIZE_SHIFT);
    gc->vertexArray.vp.count = 0;
    gc->vertexArray.interleavedFormat = GL_NONE;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

void GLAPI __glim_NormalPointer(GLenum type, GLsizei stride,
				const GLvoid *pointer)
{
    void (GLAPI *proc)(const char *);
    __GL_SETUP_NOT_IN_BEGIN();

    if (stride < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (type < GL_BYTE || type > GL_DOUBLE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    proc = (*normal_func_table)[0][type - GL_BYTE][3];
    if (proc == (void (GLAPI *)(const char *))0x0) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_BLAND();

    /* update the gc */
    gc->vertexArray.np.proc = proc;
    gc->vertexArray.np.copy =
	(void (*)(__GLcontext *, const void *, __GLvertex *))
	(*normal_func_table)[1][type - GL_BYTE][3];
    gc->vertexArray.np.pointer = pointer;
    gc->vertexArray.np.type = type;
    gc->vertexArray.np.stride = 
	stride ? stride : stride_array[type - GL_BYTE][3];
    gc->vertexArray.np.userStride = stride;
    gc->vertexArray.signature =
	(gc->vertexArray.signature & ~VERTARRAY_N_MASK) | 
	    (type_flag_table[type - GL_BYTE] << VERTARRAY_NTYPE_SHIFT);
    gc->vertexArray.np.count = 0;
    gc->vertexArray.interleavedFormat = GL_NONE;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

void GLAPI __glim_ColorPointer(GLint size, GLenum type,
			       GLsizei stride, const GLvoid *pointer)
{
    void (GLAPI *proc)(const char *);
    __GL_SETUP_NOT_IN_BEGIN();

    if (stride < 0 || size < 3 || size > 4) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (type < GL_BYTE || type > GL_DOUBLE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    proc = (*color_func_table)[0][type - GL_BYTE][size];
    if (proc == (void (GLAPI *)(const char *))0x0) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_BLAND();

    /* update the gc */
    gc->vertexArray.cp.proc = proc;
    gc->vertexArray.cp.copy =
	(void (*)(__GLcontext *, const void *, __GLvertex *))
	(*color_func_table)[1][type - GL_BYTE][size];
    gc->vertexArray.cp.pointer = pointer;
    gc->vertexArray.cp.size = size;
    gc->vertexArray.cp.type = type;
    gc->vertexArray.cp.stride = 
	stride ? stride : stride_array[type - GL_BYTE][size];
    gc->vertexArray.cp.userStride = stride;
    gc->vertexArray.signature = 
	(gc->vertexArray.signature & ~VERTARRAY_C_MASK) |
	    (type_flag_table[type - GL_BYTE] << VERTARRAY_CTYPE_SHIFT) |
		(size << VERTARRAY_CSIZE_SHIFT);
    gc->vertexArray.cp.count = 0;
    gc->vertexArray.interleavedFormat = GL_NONE;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

void GLAPI __glim_IndexPointer(GLenum type, GLsizei stride,
			       const GLvoid *pointer)
{
    void (GLAPI *proc)(const char *);
    __GL_SETUP_NOT_IN_BEGIN();

    if (stride < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (type < GL_BYTE || type > GL_DOUBLE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    proc = (*index_func_table)[0][type - GL_BYTE][1];
    if (proc == (void (GLAPI *)(const char *))0x0) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_BLAND();

    /* update the rest of the gc */
    gc->vertexArray.ip.proc = proc;
    gc->vertexArray.ip.copy =
	(void (*)(__GLcontext *, const void *, __GLvertex *))
	(*index_func_table)[1][type - GL_BYTE][1];
    gc->vertexArray.ip.pointer = pointer;
    gc->vertexArray.ip.type = type;
    gc->vertexArray.ip.stride = 
	stride ? stride : stride_array[type - GL_BYTE][1];
    gc->vertexArray.ip.userStride = stride;
    gc->vertexArray.signature = 
	(gc->vertexArray.signature & ~VERTARRAY_I_MASK) | 
	    (type_flag_table[type - GL_BYTE] << VERTARRAY_ITYPE_SHIFT);
    gc->vertexArray.ip.count = 0;
    gc->vertexArray.interleavedFormat = GL_NONE;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

void GLAPI __glim_TexCoordPointer(GLint size, GLenum type,
				  GLsizei stride, const GLvoid *pointer)
{
    void (GLAPI *proc)(const char *);
    __GL_SETUP_NOT_IN_BEGIN();

    if (stride < 0 || size < 1 || size > 4) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (type < GL_BYTE || type > GL_DOUBLE) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    proc = (*tex_coord_func_table)[0][type - GL_BYTE][size];
    if (proc == (void (GLAPI *)(const char *))0x0) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __GL_API_BLAND();

    /* update the rest of the gc */
    gc->vertexArray.tp[gc->clientTexture.activeTexture].proc = proc;
    gc->vertexArray.tp[gc->clientTexture.activeTexture].copy =
	(void (*)(__GLcontext *, const void *, __GLvertex *))
	(*tex_coord_func_table)[1][type - GL_BYTE][size];
    gc->vertexArray.tp[gc->clientTexture.activeTexture].pointer = pointer;
    gc->vertexArray.tp[gc->clientTexture.activeTexture].size = size;
    gc->vertexArray.tp[gc->clientTexture.activeTexture].type = type;
    gc->vertexArray.tp[gc->clientTexture.activeTexture].stride = 
	stride ? stride : stride_array[type - GL_BYTE][size];
    gc->vertexArray.tp[gc->clientTexture.activeTexture].userStride = stride;
    gc->vertexArray.signature = 
	(gc->vertexArray.signature & ~VERTARRAY_T_MASK) | 
	    (type_flag_table[type - GL_BYTE] << VERTARRAY_TTYPE_SHIFT) |
		(size << VERTARRAY_TSIZE_SHIFT);
    gc->vertexArray.tp[gc->clientTexture.activeTexture].count = 0;
    gc->vertexArray.interleavedFormat = GL_NONE;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

void GLAPI __glim_EdgeFlagPointer(GLint stride, const GLboolean *pointer)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (stride < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    __GL_API_BLAND();

    gc->vertexArray.ep.proc = (void (GLAPI *)(const char *))glEdgeFlagv;
    gc->vertexArray.ep.copy = 
	(void (*)(__GLcontext *, const void *, __GLvertex *)) CopyEdgeFlag;
    gc->vertexArray.ep.pointer = pointer;
    gc->vertexArray.ep.stride = stride ? stride : sizeof(GLboolean);
    gc->vertexArray.ep.userStride = stride;
    gc->vertexArray.signature = 
    gc->vertexArray.signature | VERTARRAY_E_MASK;
    gc->vertexArray.ep.count = 0;
    gc->vertexArray.interleavedFormat = GL_NONE;

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_VERTARRAY);
}

/* list comp */

void GLAPI __gllc_ArrayElement(GLint i) 
{
    __glim_ArrayElement(i);
}

void GLAPI __gllc_ArrayElementEXT(GLint i)
{
    __glim_ArrayElement(i);
}

void GLAPI __gllc_DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    GLint index;
    int i;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

    index = gc->vertexArray.index;

    /* check that first and count are positive */
    if (first < 0 || count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (mode > GL_POLYGON) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    /* could just call ArrayElement here, but let's try to not do
     * anything that ridiculously slow.  can't share with the immediate
     * mode routine since the special cases may dodge the dispatch table,
     * which would mess things up. */

    glBegin(mode);
    count += first;
    for (i = first; i < count; i++) {
	if (index & VERTARRAY_N_INDEX) 
	    (*gc->vertexArray.np.proc)
		(gc->vertexArray.np.pointer + i*gc->vertexArray.np.stride);
	if (index & VERTARRAY_C_INDEX) 
	    (*gc->vertexArray.cp.proc)
		(gc->vertexArray.cp.pointer + i*gc->vertexArray.cp.stride);
	if (index & VERTARRAY_T_INDEX) {
	    GLint j;

	    for (j=0; i<__GL_NUM_TEXTURE_UNITS; ++j) {
	        if (gc->vertexArray.textureEnables & (1<<j)) {
		    (*gc->vertexArray.tp[j].proc)
			(gc->vertexArray.tp[j].pointer + 
			 i*gc->vertexArray.tp[j].stride);
		}
	    }
	}
	if (index & VERTARRAY_I_INDEX) 
	    (*gc->vertexArray.ip.proc)
		(gc->vertexArray.ip.pointer + i*gc->vertexArray.ip.stride);
	if (index & VERTARRAY_E_INDEX)
	    (*gc->vertexArray.ep.proc)
		(gc->vertexArray.ep.pointer + i*gc->vertexArray.ep.stride);
	if (index & VERTARRAY_V_INDEX) 
	    (*gc->vertexArray.vp.proc)
		(gc->vertexArray.vp.pointer + i*gc->vertexArray.vp.stride);
    }
    glEnd();
}

void GLAPI __gllc_DrawArraysEXT(GLenum mode, GLint first, GLsizei count)
{
    __gllc_DrawArrays(mode, first, count);
}

void GLAPI __gllc_DrawElements(GLenum mode, GLsizei count, GLenum type, 
			       const GLvoid *indices)
{
    GLint index;
    GLuint *elements;
    int i;

    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

    index = gc->vertexArray.index;

    /* check that count is positive */
    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (mode > GL_POLYGON) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    /*
    ** The optimized path is for unsigned int indices, but we
    ** can make the other types go through that path by converting
    ** the indices to unsigned int.  Later we will want to find
    ** a way to propagate the optimized path to all types.
    */
    switch(type) {
    case GL_UNSIGNED_BYTE:
	{
	    const GLubyte *byteElements = (const GLubyte *)indices;
	    elements = (GLuint *)
		    gc->imports.malloc(gc, count*sizeof(GLuint));
	    for (i=0; i<count; i++) {
		elements[i] = byteElements[i];
	    }
	}
	break;
    case GL_UNSIGNED_SHORT:
	{
	    const GLushort *shortElements = (const GLushort *)indices;
	    elements = (GLuint *)
		    gc->imports.malloc(gc, count*sizeof(GLuint));
	    for (i=0; i<count; i++) {
		elements[i] = shortElements[i];
	    }
	}
	break;
    case GL_UNSIGNED_INT:
	elements = (GLuint *)indices;
	break;
    default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    /* could just call ArrayElement here, but let's try to not do
     * anything that ridiculously slow.  can't share with the immediate
     * mode routine since the special cases may dodge the dispatch table,
     * which would mess things up. */

    glBegin(mode);
    for (i = 0; i < count; i++) {
	GLint element = elements[i];

	if (index & VERTARRAY_N_INDEX) 
	    (*gc->vertexArray.np.proc)
		(gc->vertexArray.np.pointer + 
		 element*gc->vertexArray.np.stride);
	if (index & VERTARRAY_C_INDEX) 
	    (*gc->vertexArray.cp.proc)
		(gc->vertexArray.cp.pointer + 
		 element*gc->vertexArray.cp.stride);
	if (index & VERTARRAY_T_INDEX) {
	    GLint j;

	    for (j=0; i<__GL_NUM_TEXTURE_UNITS; ++j) {
	        if (gc->vertexArray.textureEnables & (1<<j)) {
		    (*gc->vertexArray.tp[j].proc)
			(gc->vertexArray.tp[j].pointer + 
			 element*gc->vertexArray.tp[j].stride);
		}
	    }
	}
	if (index & VERTARRAY_I_INDEX) 
	    (*gc->vertexArray.ip.proc)
		(gc->vertexArray.ip.pointer + 
		 element*gc->vertexArray.ip.stride);
	if (index & VERTARRAY_E_INDEX)
	    (*gc->vertexArray.ep.proc)
		(gc->vertexArray.ep.pointer + 
		 element*gc->vertexArray.ep.stride);
	if (index & VERTARRAY_V_INDEX) 
	    (*gc->vertexArray.vp.proc)
		(gc->vertexArray.vp.pointer + 
		 element*gc->vertexArray.vp.stride);
    }
    glEnd();

    if ((GLvoid *)elements != indices) {
	gc->imports.free(gc, elements);
    }
}

void GLAPI __gllc_DrawRangeElements(GLenum mode, GLuint start, GLuint end,
				    GLsizei count, GLenum type, 
				    const GLvoid *indices)
{
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

    if (end < start) {
	__glSetError(GL_INVALID_VALUE);
        return;
    }

    __gllc_DrawElements(mode, count, type, indices);
}

/* EXT_vertex_array */

void GLAPI __glim_VertexPointerEXT(GLint size, GLenum type, GLsizei stride,
				   GLsizei count, const GLvoid *pointer)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    __glim_VertexPointer(size, type, stride, pointer);
    gc->vertexArray.vp.count = count;	/* XXX only if no errors occured */
}

void GLAPI __glim_NormalPointerEXT(GLenum type, GLsizei stride,
				   GLsizei count, const GLvoid *pointer)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    __glim_NormalPointer(type, stride, pointer);
    gc->vertexArray.np.count = count;	/* XXX only if no errors occured */
}

void GLAPI __glim_ColorPointerEXT(GLint size, GLenum type, GLsizei stride,
				  GLsizei count, const GLvoid *pointer)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    __glim_ColorPointer(size, type, stride, pointer);
    gc->vertexArray.cp.count = count;	/* XXX only if no errors occured */
}

void GLAPI __glim_IndexPointerEXT(GLenum type, GLsizei stride,
				  GLsizei count, const GLvoid *pointer)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    __glim_IndexPointer(type, stride, pointer);
    gc->vertexArray.ip.count = count;	/* XXX only if no errors occured */
}

void GLAPI __glim_TexCoordPointerEXT(GLint size, GLenum type, 
				     GLsizei stride, GLsizei count, 
				     const GLvoid *pointer)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    __glim_TexCoordPointer(size, type, stride, pointer);
    gc->vertexArray.tp[gc->clientTexture.activeTexture].count = count;	/* XXX only if no errors occured */
}

void GLAPI __glim_EdgeFlagPointerEXT(GLsizei stride, GLsizei count, 
				     const GLboolean *pointer)
{
    __GL_SETUP_NOT_IN_BEGIN();

    if (count < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    __glim_EdgeFlagPointer(stride, pointer);
    gc->vertexArray.ep.count = count;	/* XXX only if no errors occured */
}
