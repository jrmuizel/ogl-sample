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
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/s_rpos.c#5 $
*/
#include "context.h"
#include "global.h"
#include "g_imports.h"

void __glRasterPos2(__GLcontext *gc, const __GLfloat v[2])
{
    __GLvertex *rp = &gc->state.current.rasterPos;
    __GLtransform *tr;
    GLuint needs;
    void (*cc)(__GLcontext*, GLint, __GLvertex*);
    void (*ct[__GL_NUM_TEXTURE_UNITS])(__GLcontext*, GLint, __GLvertex*);

    rp->obj.x = v[0];
    rp->obj.y = v[1];
    rp->obj.z = __glZero;
    rp->obj.w = __glOne;

    tr = gc->transform.modelView;
    (*tr->mvp.xf2)(&rp->clip, &rp->obj.x, &tr->mvp);

    /* clip check the raster pos */
    rp->flagBits &= __GL_CLIP_ALL_MASK;
    rp->flagBits |= __GL_VERTEX_2D_SIZE;
    if ((*gc->procs.clipCheck2)(gc, rp) & __GL_CLIP_ALL_MASK) {
	gc->state.current.validRasterPos = GL_FALSE;
	return;
    }
    gc->state.current.validRasterPos = GL_TRUE;

    /* Copy info into vertex structure */
    rp->normal = gc->state.current.normal;
    if (gc->modes.rgbMode) {
	rp->colors[__GL_FRONTFACE] = gc->state.current.color;
    } else {
	rp->colors[__GL_FRONTFACE].r = gc->state.current.userColorIndex;
    }
    rp->texture[0] = gc->state.current.texture[0];

    /* This may be generous, but it will work */
    needs = gc->vertex.needs | __GL_NEEDS_RASTERPOS;

    /*
    ** Save away calcColor and calcTexture procs, then validate vertex.
    */
    cc = gc->procs.calcColor;
    gc->procs.calcColor = gc->procs.calcRasterColor;
    __GL_MEMCOPY(ct, gc->procs.calcTextureUnit,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));
    __GL_MEMCOPY(gc->procs.calcTextureUnit, gc->procs.calcRasterTextureUnit,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));
    DO_VALIDATE(gc, rp, needs);
    gc->procs.calcColor = cc;
    __GL_MEMCOPY(gc->procs.calcTextureUnit, ct,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));

    if (gc->renderMode == GL_SELECT) {
	__glSelectPoint(gc, rp);
    }
}

void __glRasterPos3(__GLcontext *gc, const __GLfloat v[3])
{
    __GLvertex *rp = &gc->state.current.rasterPos;
    __GLtransform *tr;
    GLuint needs;
    void (*cc)(__GLcontext*, GLint, __GLvertex*);
    void (*ct[__GL_NUM_TEXTURE_UNITS])(__GLcontext*, GLint, __GLvertex*);

    rp->obj.x = v[0];
    rp->obj.y = v[1];
    rp->obj.z = v[2];
    rp->obj.w = __glOne;

    tr = gc->transform.modelView;
    (*tr->mvp.xf3)(&rp->clip, &rp->obj.x, &tr->mvp);

    /* clip check the raster pos */
    rp->flagBits &= __GL_CLIP_ALL_MASK;
    rp->flagBits |= __GL_VERTEX_3D_SIZE;
    if ((*gc->procs.clipCheck3)(gc, rp) & __GL_CLIP_ALL_MASK) {
	gc->state.current.validRasterPos = GL_FALSE;
	return;
    }
    gc->state.current.validRasterPos = GL_TRUE;

    /* Copy info into vertex structure */
    rp->normal = gc->state.current.normal;
    if (gc->modes.rgbMode) {
	rp->colors[__GL_FRONTFACE] = gc->state.current.color;
    } else {
	rp->colors[__GL_FRONTFACE].r = gc->state.current.userColorIndex;
    }
    rp->texture[0] = gc->state.current.texture[0];

    /* This may be generous, but it will work */
    needs = gc->vertex.needs | __GL_NEEDS_RASTERPOS;

    /*
    ** Save away calcColor and calcTexture procs, then validate vertex.
    */
    cc = gc->procs.calcColor;
    gc->procs.calcColor = gc->procs.calcRasterColor;
    __GL_MEMCOPY(ct, gc->procs.calcTextureUnit,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));
    __GL_MEMCOPY(gc->procs.calcTextureUnit, gc->procs.calcRasterTextureUnit,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));
    DO_VALIDATE(gc, rp, needs);
    gc->procs.calcColor = cc;
    __GL_MEMCOPY(gc->procs.calcTextureUnit, ct,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));

    if (gc->renderMode == GL_SELECT) {
	__glSelectPoint(gc, rp);
    }
}

void __glRasterPos4(__GLcontext *gc, const __GLfloat v[4])
{
    __GLvertex *rp = &gc->state.current.rasterPos;
    __GLtransform *tr;
    GLuint needs;
    void (*cc)(__GLcontext*, GLint, __GLvertex*);
    void (*ct[__GL_NUM_TEXTURE_UNITS])(__GLcontext*, GLint, __GLvertex*);

    rp->obj.x = v[0];
    rp->obj.y = v[1];
    rp->obj.z = v[2];
    rp->obj.w = v[3];

    tr = gc->transform.modelView;
    (*tr->mvp.xf4)(&rp->clip, &rp->obj.x, &tr->mvp);

    /* clip check the raster pos */
    rp->flagBits &= __GL_CLIP_ALL_MASK;
    rp->flagBits |= __GL_VERTEX_4D_SIZE;
    if ((*gc->procs.clipCheck4)(gc, rp) & __GL_CLIP_ALL_MASK) {
	gc->state.current.validRasterPos = GL_FALSE;
	return;
    }
    gc->state.current.validRasterPos = GL_TRUE;

    /* Copy info into vertex structure */
    rp->normal = gc->state.current.normal;
    if (gc->modes.rgbMode) {
	rp->colors[__GL_FRONTFACE] = gc->state.current.color;
    } else {
	rp->colors[__GL_FRONTFACE].r = gc->state.current.userColorIndex;
    }
    rp->texture[0] = gc->state.current.texture[0];

    /* This may be generous, but it will work */
    needs = gc->vertex.needs | __GL_NEEDS_RASTERPOS;

    /*
    ** Save away calcColor and calcTexture procs, then validate vertex.
    */
    cc = gc->procs.calcColor;
    gc->procs.calcColor = gc->procs.calcRasterColor;
    __GL_MEMCOPY(ct, gc->procs.calcTextureUnit,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));
    __GL_MEMCOPY(gc->procs.calcTextureUnit, gc->procs.calcRasterTextureUnit,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));
    DO_VALIDATE(gc, rp, needs);
    gc->procs.calcColor = cc;
    __GL_MEMCOPY(gc->procs.calcTextureUnit, ct,
    		__GL_NUM_TEXTURE_UNITS*sizeof(ct[0]));

    if (gc->renderMode == GL_SELECT) {
	__glSelectPoint(gc, rp);
    }
}
