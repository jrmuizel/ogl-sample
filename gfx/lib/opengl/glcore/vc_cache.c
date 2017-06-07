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
** $Header: //depot/main/gfx/lib/opengl/glcore/vc_cache.c#14 $
*/

#include <stdlib.h>
#include "context.h"
#include "global.h"
#include "glmath.h"
#include "g_imfncs.h"

#if defined(__GL_CODEGEN) && defined(i386)
#include "og_geom.h"
#endif /* __GL_CODEGEN && i386 */

#pragma optimize("a", on)

#if defined(__GL_ENVVARS)
static GLboolean disable_og_vcache;
#endif
	   
static void (*ProcessPrimitive[])(__GLcontext *gc, GLuint flushed) = {
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
    __glProcessVertexCacheLock,
};

static void (*ProcessPrimitive2[])(__GLcontext *gc, GLuint flushed) = {
    __glProcessCachedPoints,
    __glProcessCachedLines,
    __glProcessCachedLineLoop,
    __glProcessCachedLineStrip,
    __glProcessCachedTriangles,
    __glProcessCachedTriangleStrip,
    __glProcessCachedTriangleFan,
    __glProcessCachedQuads,
    __glProcessCachedQuadStrip,
    __glProcessCachedPolygon,
};

/*
** Initialize the vertex cache and the array for building primitives up. 
*/
void __glInitVertexCacheState(__GLcontext *gc)
{							   
    __GLvcacheMachine *vc = &gc->vertexCache;

    vc->vertexCacheEnabled = GL_TRUE;
#if defined(__GL_ENVVARS)
    {
	char *env = (*gc->imports.getenv)(gc, "__GL_VCACHE");
	if (env)
	    vc->vertexCacheEnabled = (GLboolean) (*gc->imports.atoi)(gc, env);
    }
#if defined(__GL_CODEGEN) && defined(i386)
    {
	char *env = (*gc->imports.getenv)(gc, "__GL_DISABLE_OG_VCACHE");
	if (env)
	    disable_og_vcache = (GLboolean) (*gc->imports.atoi)(gc, env);
    }
#endif /* __GL_CODEGEN && i386 */
#endif /* __GL_ENVVARS */

    /* initialize second stage primitive processing */
    vc->process2[GL_POINTS] = ProcessPrimitive2[GL_POINTS];
    vc->process2[GL_LINES] = ProcessPrimitive2[GL_LINES];
    vc->process2[GL_LINE_LOOP] = ProcessPrimitive2[GL_LINE_LOOP];
    vc->process2[GL_LINE_STRIP] = ProcessPrimitive2[GL_LINE_STRIP];
    vc->process2[GL_TRIANGLES] = ProcessPrimitive2[GL_TRIANGLES];
    vc->process2[GL_TRIANGLE_STRIP] = ProcessPrimitive2[GL_TRIANGLE_STRIP];
    vc->process2[GL_TRIANGLE_FAN] = ProcessPrimitive2[GL_TRIANGLE_FAN];
    vc->process2[GL_QUADS] = ProcessPrimitive2[GL_QUADS];
    vc->process2[GL_QUAD_STRIP] = ProcessPrimitive2[GL_QUAD_STRIP];
    vc->process2[GL_POLYGON] = ProcessPrimitive2[GL_POLYGON];

    /* initialize beginPrim and endPrim */
    if (vc->vertexCacheEnabled) {
	gc->procs.beginPrim[GL_LINE_LOOP] = __glBeginVcache;
	gc->procs.beginPrim[GL_LINE_STRIP] = __glBeginVcache;
	gc->procs.beginPrim[GL_LINES] = __glBeginVcache;
	gc->procs.beginPrim[GL_POINTS] = __glBeginVcache;
	gc->procs.beginPrim[GL_POLYGON] = __glBeginVcache;
	gc->procs.beginPrim[GL_TRIANGLE_STRIP] = __glBeginVcache;
	gc->procs.beginPrim[GL_TRIANGLE_FAN] = __glBeginVcache;
	gc->procs.beginPrim[GL_TRIANGLES] = __glBeginVcache;
	gc->procs.beginPrim[GL_QUAD_STRIP] = __glBeginVcache;
	gc->procs.beginPrim[GL_QUADS] = __glBeginVcache;
	gc->procs.endPrim = __glEndVcache;
    }
}

void __glFreeVertexCacheState(__GLcontext *gc)
{
}

/*
** Process the Vertex Cache. 
*/
void __glProcessVertexCacheLock(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;
    void (*old_xf)(__GLcoord *res, const __GLfloat *v, const __GLmatrix *m);
    GLuint (*old_clipCheck)(__GLcontext *gc, __GLvertex *vx);
    void (*old_validateVertex)(__GLcontext *gc, __GLvertex *v, GLuint needs);
    void (*old_compile)(__GLcontext *, GLint, GLint, GLint);
    void (*old_compileSilhouette)(__GLcontext *, __GLvertex *, GLint);
    __GLvertex *old_varrayPtr;
    GLbitfield old_controlWord;
    GLuint vp_size;

    if (!vc->vertexCount) return;

    /* lock in the beginning, to make sure we get the right data */
    __GL_LOCK_RENDER_BUFFERS(gc);

    old_xf = gc->vertexArray.xf;
    old_clipCheck = gc->vertexArray.clipCheck;
    old_validateVertex = gc->vertexArray.validateVertex;

    if (vc->vertexType >= __GL_VCACHE_TYPE_4D) {
	gc->vertexArray.xf = gc->transform.modelView->mvp.xf4;
	gc->vertexArray.clipCheck = gc->procs.clipCheck4;
	gc->vertexArray.validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_4D_INDEX];
	vp_size = 4;
    } else if (vc->vertexType >= __GL_VCACHE_TYPE_3D) {
	gc->vertexArray.xf = gc->transform.modelView->mvp.xf3;
	gc->vertexArray.clipCheck = gc->procs.clipCheck3;
	gc->vertexArray.validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_3D_INDEX];
	vp_size = 3;
    } else {
	gc->vertexArray.xf = gc->transform.modelView->mvp.xf2;
	gc->vertexArray.clipCheck = gc->procs.clipCheck2;
	gc->vertexArray.validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_2D_INDEX];
	vp_size = 2;
    }

    old_controlWord = gc->vertexArray.controlWord;
    old_compile = gc->vertexArray.compileElements;
    old_compileSilhouette = gc->vertexArray.compileElementsSilhouette;
    old_varrayPtr = gc->vertexArray.varrayPtr;

    /* No precompile or copy when processing the vertex cache */
    gc->vertexArray.controlWord &= ~VERTARRAY_CW_ALLOW_PRECOMPILE;

    gc->vertexArray.compileElements =
	(vc->vertexCacheState & __GL_VCACHE_MATERIAL_VALIDATE) ?
	__glCompileElements_NoCopy :
	vc->compileElements[vp_size-2];

    gc->vertexArray.compileElementsSilhouette = 
	__glCompileElementsSilhouette_NoCopy;
    gc->vertexArray.varrayPtr = gc->vertexCache.vertexCache;

    (*vc->process2[vc->cachedPrimitiveType])(gc, flushed);

    /* restore */
    gc->vertexArray.varrayPtr = old_varrayPtr;
    gc->vertexArray.compileElementsSilhouette = old_compileSilhouette;
    gc->vertexArray.compileElements = old_compile;
    gc->vertexArray.controlWord = old_controlWord;
    gc->vertexArray.validateVertex = old_validateVertex;
    gc->vertexArray.clipCheck = old_clipCheck;
    gc->vertexArray.xf = old_xf;

    __GL_UNLOCK_RENDER_BUFFERS(gc);
}

void __glProcessVertexCache(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;
    void (*old_xf)(__GLcoord *res, const __GLfloat *v, const __GLmatrix *m);
    GLuint (*old_clipCheck)(__GLcontext *gc, __GLvertex *vx);
    void (*old_validateVertex)(__GLcontext *gc, __GLvertex *v, GLuint needs);
    void (*old_compile)(__GLcontext *, GLint, GLint, GLint);
    void (*old_compileSilhouette)(__GLcontext *, __GLvertex *, GLint);
    __GLvertex *old_varrayPtr;
    GLbitfield old_controlWord;
    GLuint vp_size;

    if (!vc->vertexCount) return;

    old_xf = gc->vertexArray.xf;
    old_clipCheck = gc->vertexArray.clipCheck;
    old_validateVertex = gc->vertexArray.validateVertex;

    if (vc->vertexType >= __GL_VCACHE_TYPE_4D) {
	gc->vertexArray.xf = gc->transform.modelView->mvp.xf4;
	gc->vertexArray.clipCheck = gc->procs.clipCheck4;
	gc->vertexArray.validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_4D_INDEX];
	vp_size = 4;
    } else if (vc->vertexType >= __GL_VCACHE_TYPE_3D) {
	gc->vertexArray.xf = gc->transform.modelView->mvp.xf3;
	gc->vertexArray.clipCheck = gc->procs.clipCheck3;
	gc->vertexArray.validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_3D_INDEX];
	vp_size = 3;
    } else {
	gc->vertexArray.xf = gc->transform.modelView->mvp.xf2;
	gc->vertexArray.clipCheck = gc->procs.clipCheck2;
	gc->vertexArray.validateVertex = 
	    gc->procs.validateVertex[__GL_VERTEX_2D_INDEX];
	vp_size = 2;
    }

    old_controlWord = gc->vertexArray.controlWord;
    old_compile = gc->vertexArray.compileElements;
    old_compileSilhouette = gc->vertexArray.compileElementsSilhouette;
    old_varrayPtr = gc->vertexArray.varrayPtr;

    /* No precompile or copy when processing the vertex cache */
    gc->vertexArray.controlWord &= ~VERTARRAY_CW_ALLOW_PRECOMPILE;

    gc->vertexArray.compileElements =
	(vc->vertexCacheState & __GL_VCACHE_MATERIAL_VALIDATE) ?
	__glCompileElements_NoCopy :
	vc->compileElements[vp_size-2];

    gc->vertexArray.compileElementsSilhouette = __glCompileElementsSilhouette_NoCopy;
    gc->vertexArray.varrayPtr = gc->vertexCache.vertexCache;

    (*vc->process2[vc->cachedPrimitiveType])(gc, flushed);

    /* restore */
    gc->vertexArray.varrayPtr = old_varrayPtr;
    gc->vertexArray.compileElementsSilhouette = old_compileSilhouette;
    gc->vertexArray.compileElements = old_compile;
    gc->vertexArray.controlWord = old_controlWord;
    gc->vertexArray.validateVertex = old_validateVertex;
    gc->vertexArray.clipCheck = old_clipCheck;
    gc->vertexArray.xf = old_xf;
}

/*
** Routines for updating the contents of the vertex cache buffers.  Note that
** the Test to see if the buffer needs to be flushed is done at the beginning
** of the routines.  This is important and is done in this order so that if a
** polygon is constructed which has the same number of vertices as the buffer,
** then the cache will be flushed by an __GL_VC_END_VCACHE as opposed to a
** __GL_VC_FLUSH_VCACHE.  This will ensure that the cache handling code can 
** detect the fact that primitive has actually ended. 
*/

#define VX_INIT(dim) { \
    vx->flagBits = gc->state.current.edgeTag | __GL_VERTEX_##dim##D_SIZE; \
}

void GLAPI __glim_VertexCache2fv(const GLfloat v[2])
{
    __GL_SETUP();
    __GLvertex 	*vx;
    __GLvcacheMachine *vc = &gc->vertexCache;

    /* If the array is full, process the lot */
    if (vc->vertexCount == __GL_VCACHE_MAXSIZE) {
	(*vc->process[vc->cachedPrimitiveType])(gc, __GL_VCACHE_FLUSH);
    }

    /* Store Vertex in Array */	 
    vx = vc->vertexCache + vc->vertexCount;
    assert((vc->vertexIncrement == 0) || (vc->vertexIncrement == 1));
    vc->vertexCount += vc->vertexIncrement;
    vc->vertexType |= __GL_VCACHE_TYPE_2D;
	
    VX_INIT(2);
    vx->obj.x = v[0];
    vx->obj.y = v[1];
    vx->obj.z = __glZero;
    vx->obj.w = __glOne;

    (*vc->save)(gc, vx);
}

void GLAPI __glim_VertexCache3fv(const GLfloat v[3])
{
    __GL_SETUP();
    __GLvertex 	*vx;
    __GLvcacheMachine *vc = &gc->vertexCache;

    /* If the array is full, process the lot */
    if (vc->vertexCount == __GL_VCACHE_MAXSIZE) {
	(*vc->process[vc->cachedPrimitiveType])(gc, __GL_VCACHE_FLUSH);
    }

    /* Store Vertex in Array */	 
    vx = vc->vertexCache + vc->vertexCount;
    assert((vc->vertexIncrement == 0) || (vc->vertexIncrement == 1));
    vc->vertexCount += vc->vertexIncrement;
    vc->vertexType |= __GL_VCACHE_TYPE_3D;

    VX_INIT(3);
    vx->obj.x = v[0];
    vx->obj.y = v[1];
    vx->obj.z = v[2];
    vx->obj.w = __glOne;

    (*vc->save)(gc, vx);
}

void GLAPI __glim_VertexCache4fv(const GLfloat v[4])
{
    __GL_SETUP();
    __GLvertex 	*vx;
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount == __GL_VCACHE_MAXSIZE) {
	(*vc->process[vc->cachedPrimitiveType])(gc, __GL_VCACHE_FLUSH);
    }

    vc->vertexType |= __GL_VCACHE_TYPE_4D;
    vx = vc->vertexCache + vc->vertexCount;
    assert((vc->vertexIncrement == 0) || (vc->vertexIncrement == 1));
    vc->vertexCount += vc->vertexIncrement;

    VX_INIT(4);
    vx->obj.x = v[0];
    vx->obj.y = v[1];
    vx->obj.z = v[2];
    vx->obj.w = v[3];

    (*vc->save)(gc, vx);
}

/*
** Vertex save procs 
*/
static void VCSaveN(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z;

    x = gc->state.current.normal.x;
    y = gc->state.current.normal.y;
    z = gc->state.current.normal.z;
    vx->normal.x = x;
    vx->normal.y = y;
    vx->normal.z = z;
}

static void VCSaveCI(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat index;

    index = gc->state.current.userColorIndex;
    vx->colors[__GL_FRONTFACE].r = index;
}

static void VCSaveNCI(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z;
    __GLfloat index;

    x = gc->state.current.normal.x;
    y = gc->state.current.normal.y;
    z = gc->state.current.normal.z;
    vx->normal.x = x;
    vx->normal.y = y;
    vx->normal.z = z;

    index = gc->state.current.userColorIndex;
    vx->colors[__GL_FRONTFACE].r = index;
}

static void VCSaveC(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat r,g,b,a;

    r = gc->state.current.color.r;
    g = gc->state.current.color.g;
    b = gc->state.current.color.b;
    a = gc->state.current.color.a;
    vx->colors[__GL_FRONTFACE].r = r;
    vx->colors[__GL_FRONTFACE].g = g;
    vx->colors[__GL_FRONTFACE].b = b;
    vx->colors[__GL_FRONTFACE].a = a;
}

static void VCSaveNC(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z;
    __GLfloat r,g,b,a;

    x = gc->state.current.normal.x;
    y = gc->state.current.normal.y;
    z = gc->state.current.normal.z;
    vx->normal.x = x;
    vx->normal.y = y;
    vx->normal.z = z;

    r = gc->state.current.color.r;
    g = gc->state.current.color.g;
    b = gc->state.current.color.b;
    a = gc->state.current.color.a;
    vx->colors[__GL_FRONTFACE].r = r;
    vx->colors[__GL_FRONTFACE].g = g;
    vx->colors[__GL_FRONTFACE].b = b;
    vx->colors[__GL_FRONTFACE].a = a;
}

static void VCSaveT(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z,w;
    GLint i;

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
}

static void VCSaveCT(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat r,g,b,a;
    __GLfloat x,y,z,w;
    GLint i;

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
}

static void VCSaveNT(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z,w;
    GLint i;

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
}

static void VCSaveCIAll(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat x,y,z,w;
    __GLfloat index;
    GLint i;

    index = gc->state.current.userColorIndex;
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
}

static void VCSaveCAll(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat r,g,b,a;
    __GLfloat x,y,z,w;
    GLint i;

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
}

static void VCSaveCIT(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat index;
    __GLfloat x,y,z,w;
    GLint i;

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
}

/************************************************************************/


#define __NORMAL 1
#define __COLOR 2
#define __TEX 4

static void (*CISaveProcs[8])(__GLcontext*, __GLvertex*) = {
    0, 			/* none */
    VCSaveN,		/* __NORMAL */
    VCSaveCI,		/* __COLOR */
    VCSaveNCI, 		/* __NORMAL | __COLOR */

    VCSaveT, 		/* __TEX */
    VCSaveNT, 		/* __NORMAL | __TEX */

    VCSaveCIT, 		/* __COLOR | __TEX */
    VCSaveCIAll, 	/* __NORMAL | __COLOR | __TEX */
};

static void (*RGBSaveProcs[8])(__GLcontext*, __GLvertex*) = {
    0, 			/* none */
    VCSaveN, 		/* __NORMAL */

    VCSaveC, 		/* __COLOR */
    VCSaveNC, 		/* __NORMAL | __COLOR */

    VCSaveT, 		/* __TEX */
    VCSaveNT, 		/* __NORMAL | __TEX */

    VCSaveCT, 		/* __COLOR | __TEX */
    VCSaveCAll, 	/* __NORMAL | __COLOR | __TEX */
};

/******************************************************************************
**
** VertexCache Begin / End procs
**
******************************************************************************/

void GLAPI __glim_VertexCacheBegin(GLenum mode)
{
    __GL_SETUP();
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	    glBegin(mode);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    if ((GLuint)mode > GL_POLYGON) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    __gl_beginMode = __GL_IN_BEGIN;
    __GL_API_BGN_RENDER();

#if defined(__GL_VCACHE_ALLOW_CAT_PRIMITIVE)
    if (gc->vertexCache.vertexCacheState & VC_CAT_PRIMITIVE) {
	if (gc->vertexCache.cachedPrimitiveType == mode) {
	    return;
	} else {
	    /*
	    ** Change in primitive type...flush the current
	    ** buffer out and then continue afresh.
	    */
	    (*vc->processCacheProcs[vc->cachedPrimitiveType])
		(gc, __GL_VC_END_VCACHE);
	    gc->vertexCache.vertexCacheState &= 
		~(__GL_VC_CAT_PRIMITIVE | __GL_VC_MATERIAL_VALIDATE);
	}
    }
#else
    assert(0 == gc->vertexCache.vertexCount);
#endif

    gc->vertexCache.cachedPrimitiveType = mode;
    gc->vertexCache.vertexType = 0;
    gc->vertexCache.vertexIncrement = 1;
    gc->vertexCache.colorChanged = GL_FALSE;
    /* force vertex array recompile after vertex cache */
    gc->vertexArray.controlWord |= VERTARRAY_CW_NEEDS_COMPILE;
    gc->vertexArray.continuation = GL_FALSE;

    (*gc->procs.beginPrim[mode])(gc);
}

void __glBeginVcache(__GLcontext *gc)
{
#if defined(__GL_VCACHE_ALLOW_CAT_PRIMITIVE)
    /*
    ** See if the primitive being rendered is one suitable 
    ** for primitive concatenation.
    */
    GLenum mode = gc->vertexCache.cachedPrimitiveType;

    if (mode == GL_POINTS    ||
	mode == GL_LINES     ||
	mode == GL_TRIANGLES ||
	mode == GL_QUADS)
    {
	gc->vertexCache.vertexCacheState |= VC_CAT_PRIMITIVE;
    }
#endif
}

void GLAPI __glim_VertexCacheEnd(void)
{
    __GL_SETUP();
    GLuint beginMode;

    beginMode = __gl_beginMode;
    if (beginMode == __GL_NOT_IN_BEGIN || beginMode == __GL_NEED_VALIDATE) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }
    assert(beginMode == __GL_IN_BEGIN);
    __GL_API_END_RENDER();

    (*gc->procs.endPrim)(gc);

    gc->vertexCache.vertexIncrement = 0;

    gc->vertexArray.continuation = GL_FALSE;
    __gl_beginMode = __GL_NOT_IN_BEGIN;
}

void __glEndVcache(__GLcontext *gc)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

#if defined(__GL_VCACHE_ALLOW_CAT_PRIMITIVE)
    /*
    ** If we are rendering any of the independent primitives
    ** then don't flush...thereby concatenating successive 
    ** primitives.
    */
    if (gc->vertexCache.vertexCacheState & VC_CAT_PRIMITIVE) {
	/* Fix up the vertex count according to the primitive type */
	switch (gc->vertexCache.cachedPrimitiveType) {
	case GL_LINES:
	    gc->vertexCache.vertexCount -= gc->vertexCache.vertexCount % 2;
	    break;
	case GL_TRIANGLES:
	    gc->vertexCache.vertexCount -= gc->vertexCache.vertexCount % 3;
	    break;
	case GL_QUADS:
	    gc->vertexCache.vertexCount -= gc->vertexCache.vertexCount % 4;
	    break;
	}
    } else {
	(*vc->process[vc->cachedPrimitiveType])(gc, __GL_VCACHE_END);
    }
#else
    (*vc->process[vc->cachedPrimitiveType])(gc, __GL_VCACHE_END);
#endif
}


/*
** Validate Cache Vertices. 
*/
#if defined(__GL_CODEGEN) && defined(i386)
static void CompileElementsVcache2Wrapper(__GLcontext *gc, GLint offset,
					  GLint first, GLsizei count)
{
    __GLvertArrayMachine *va = &gc->vertexArray;
    __GLvcacheMachine *vc = &gc->vertexCache;
    vc->compileElements[0] = 
	__glGenerateVcacheCompile(gc, 2, __GL_GEOM_OG_NOCOPY);
    va->compileElements = vc->compileElements[0];
    (*va->compileElements)(gc, offset, first, count);
}

static void CompileElementsVcache3Wrapper(__GLcontext *gc, GLint offset,
					  GLint first, GLsizei count)
{
    __GLvertArrayMachine *va = &gc->vertexArray;
    __GLvcacheMachine *vc = &gc->vertexCache;
    vc->compileElements[1] = 
	__glGenerateVcacheCompile(gc, 3, __GL_GEOM_OG_NOCOPY);
    va->compileElements = vc->compileElements[1];
    (*va->compileElements)(gc, offset, first, count);
}

static void CompileElementsVcache4Wrapper(__GLcontext *gc, GLint offset,
					  GLint first, GLsizei count)
{
    __GLvertArrayMachine *va = &gc->vertexArray;
    __GLvcacheMachine *vc = &gc->vertexCache;
    vc->compileElements[2] = 
	__glGenerateVcacheCompile(gc, 4, __GL_GEOM_OG_NOCOPY);
    va->compileElements = vc->compileElements[2];
    (*va->compileElements)(gc, offset, first, count);
}
#endif

void __glGenericPickVertexCacheProcs (__GLcontext *gc)
{
    __GLvcacheMachine *vc = &gc->vertexCache;
    GLuint enables = gc->state.enables.general;
    GLint ix;

    /* initialize first stage primitive processing */
    vc->process[GL_POINTS] = ProcessPrimitive[GL_POINTS];
    vc->process[GL_LINES] = ProcessPrimitive[GL_LINES];
    vc->process[GL_LINE_LOOP] = ProcessPrimitive[GL_LINE_LOOP];
    vc->process[GL_LINE_STRIP] = ProcessPrimitive[GL_LINE_STRIP];
    vc->process[GL_TRIANGLES] = ProcessPrimitive[GL_TRIANGLES];
    vc->process[GL_TRIANGLE_STRIP] = ProcessPrimitive[GL_TRIANGLE_STRIP];
    vc->process[GL_TRIANGLE_FAN] = ProcessPrimitive[GL_TRIANGLE_FAN];
    vc->process[GL_QUADS] = ProcessPrimitive[GL_QUADS];
    vc->process[GL_QUAD_STRIP] = ProcessPrimitive[GL_QUAD_STRIP];
    vc->process[GL_POLYGON] = ProcessPrimitive[GL_POLYGON];

    gc->dispatchState->dispatch.Begin = __glim_VertexCacheBegin;
    gc->dispatchState->dispatch.End = __glim_VertexCacheEnd;
    gc->dispatchState->dispatch.Vertex2fv = __glim_VertexCache2fv;
    gc->dispatchState->dispatch.Vertex3fv = __glim_VertexCache3fv;
    gc->dispatchState->dispatch.Vertex4fv = __glim_VertexCache4fv;
    gc->procs.matValidate = __glMatValidateVcache;

#if defined(__GL_CODEGEN) && defined(i386)
    if (
#if defined(__GL_USE_ASMCODE) && defined(__GL_USE_K3DCODE)
	((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) == 0) &&
#endif /* __GL_USE_ASMCODE && __GL_USE_K3DCODE */
#if defined(__GL_ENVVARS)
	!disable_og_vcache &&
#endif /* __GL_ENVVARS */
	(gc->renderMode == GL_RENDER)) {
	vc->compileElements[0] = CompileElementsVcache2Wrapper;
	vc->compileElements[1] = CompileElementsVcache3Wrapper;
	vc->compileElements[2] = CompileElementsVcache4Wrapper;
    } else
#endif /* __GL_CODEGEN && i386 */
    {
	vc->compileElements[0] = __glCompileElements_NoCopy;
	vc->compileElements[1] = __glCompileElements_NoCopy;
	vc->compileElements[2] = __glCompileElements_NoCopy;
    }

    if (gc->renderMode == GL_FEEDBACK) {
	ix = __NORMAL | __COLOR | __TEX;
    } else {
	ix = 0;
	if (enables & __GL_LIGHTING_ENABLE) {
	    ix |= __NORMAL;
	} else {
	    ix |= __COLOR;
	}
	if (enables & __GL_CULL_VERTEX_ENABLE) {
	    ix |= __NORMAL;
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
    }

    if (gc->modes.rgbMode) {
	vc->save = RGBSaveProcs[ix];
    } else {
	vc->save = CISaveProcs[ix];
    }
    assert(vc->save != 0);
}


/*
** Validate vertexes in the vcache
*/
void __glMatValidateVcache(__GLcontext *gc)
{
    __GLvcacheMachine *vc = &gc->vertexCache;
    __GLvertex *v, *last;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    v = vc->vertexCache + vc->vertexStart;
    last = vc->vertexCache + vc->vertexCount;
    for (; v < last; v++) {
	if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
    }

    vc->vertexStart = vc->vertexCount;
    vc->vertexCacheState |= __GL_VCACHE_MATERIAL_VALIDATE;
}

#pragma optimize("", on)
