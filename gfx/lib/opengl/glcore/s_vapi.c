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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_vapi.c#6 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"

void GLAPI __glim_EdgeFlag(GLboolean tag)
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.edgeTag = tag ? __GL_VERTEX_EDGE_FLAG : 0x0;
}

void GLAPI __glim_EdgeFlagv(const GLboolean tag[1])
{
    __GL_SETUP();
    __GL_API_VAPI();
    gc->state.current.edgeTag = tag[0] ? __GL_VERTEX_EDGE_FLAG : 0x0;
}

/************************************************************************
** NOTE: This code is written like this so the compiler will realize that 
** there is no aliasing going on here, and it will do something reasonable
** (as opposed to the code it usually generates).
*/

/*
** The fast path Vertex[234]fv() routines:
*/
void GLAPI __glim_Vertex2fv(const GLfloat v[2])
{
    __GL_SETUP();
    __GLvertex *vx;
    __GLtransform *tr;
    __GLfloat x,y;
    __GL_API_VAPI();

    vx = gc->vertex.v0;
    x = v[0];
    y = v[1];
    tr = gc->transform.modelView;
    vx->obj.x = x;
    vx->obj.y = y;
    vx->obj.z = __glZero;
    vx->obj.w = __glOne;
    (*tr->mvp.xf2)(&vx->clip, &vx->obj.x, &tr->mvp);
    vx->flagBits = (*gc->procs.clipCheck2)(gc, vx) | __GL_VERTEX_2D_SIZE;
    (*gc->procs.v)(gc, vx);
}

void GLAPI __glim_Vertex3fv(const GLfloat v[3])
{
    __GL_SETUP();
    __GLvertex *vx;
    __GLfloat x,y,z;
    __GLtransform *tr;
    __GL_API_VAPI();

    vx = gc->vertex.v0;
    tr = gc->transform.modelView;
    x = v[0];
    y = v[1];
    z = v[2];
    vx->obj.x = x;
    vx->obj.y = y;
    vx->obj.z = z;
    vx->obj.w = __glOne;
    (*tr->mvp.xf3)(&vx->clip, &vx->obj.x, &tr->mvp);
    vx->flagBits = (*gc->procs.clipCheck3)(gc, vx) | __GL_VERTEX_3D_SIZE;
    (*gc->procs.v)(gc, vx);
}

void GLAPI __glim_Vertex4fv(const GLfloat v[4])
{
    __GL_SETUP();
    __GLvertex *vx;
    __GLtransform *tr;
    __GLfloat x,y,z,w;
    __GL_API_VAPI();

    vx = gc->vertex.v0;
    tr = gc->transform.modelView;
    x = v[0];
    y = v[1];
    z = v[2];
    w = v[3];
    vx->obj.x = x;
    vx->obj.y = y;
    vx->obj.z = z;
    vx->obj.w = w;
    (*tr->mvp.xf4)(&vx->clip, &vx->obj.x, &tr->mvp);
    vx->flagBits = (*gc->procs.clipCheck4)(gc, vx) | __GL_VERTEX_4D_SIZE;
    (*gc->procs.v)(gc, vx);
}

/************************************************************************
** Slow path vertex routines.  They go through the dispatch table to
** call the primary fast path routines.
**
** Note: Our compilers really need a clue about tail recursion.  They don't
** deal with these functions as optimally as they could.
*/

void GLAPI __glim_Vertex2f(GLfloat x, GLfloat y)
{
    GLfloat fv[2];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex2fv;
    fv[0] = x;
    fv[1] = y;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex2d(GLdouble x, GLdouble y)
{
    GLfloat fv[2];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex2fv;
    fv[0] = x;
    fv[1] = y;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex2dv(const GLdouble v[2])
{
    GLfloat x,y,fv[2];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    vertexfv = gc->dispatchState->dispatch.Vertex2fv;
    fv[0] = x;
    fv[1] = y;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex2i(GLint x, GLint y)
{
    GLfloat fv[2];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex2fv;
    fv[0] = x;
    fv[1] = y;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex2iv(const GLint v[2])
{
    GLfloat x,y,fv[2];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    vertexfv = gc->dispatchState->dispatch.Vertex2fv;
    fv[0] = x;
    fv[1] = y;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex2s(GLshort x, GLshort y)
{
    GLfloat fv[2];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex2fv;
    fv[0] = x;
    fv[1] = y;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex2sv(const GLshort v[2])
{
    GLfloat x,y,fv[2];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    vertexfv = gc->dispatchState->dispatch.Vertex2fv;
    fv[0] = x;
    fv[1] = y;
    (*vertexfv)(fv);
}

/************************************************************************/

void GLAPI __glim_Vertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat fv[3];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex3fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex3d(GLdouble x, GLdouble y, GLdouble z)
{
    GLfloat fv[3];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex3fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex3dv(const GLdouble v[3])
{
    GLfloat x,y,z,fv[3];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    z = v[2];
    vertexfv = gc->dispatchState->dispatch.Vertex3fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex3i(GLint x, GLint y, GLint z)
{
    GLfloat fv[3];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex3fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex3iv(const GLint v[3])
{
    GLfloat x,y,z,fv[3];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    z = v[2];
    vertexfv = gc->dispatchState->dispatch.Vertex3fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex3s(GLshort x, GLshort y, GLshort z)
{
    GLfloat fv[3];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex3fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex3sv(const GLshort v[3])
{
    GLfloat x,y,z,fv[3];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    z = v[2];
    vertexfv = gc->dispatchState->dispatch.Vertex3fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    (*vertexfv)(fv);
}

/************************************************************************/

void GLAPI __glim_Vertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLfloat fv[4];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex4fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    fv[3] = w;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLfloat fv[4];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex4fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    fv[3] = w;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex4dv(const GLdouble v[4])
{
    GLfloat x,y,z,w,fv[4];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    z = v[2];
    w = v[3];
    vertexfv = gc->dispatchState->dispatch.Vertex4fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    fv[3] = w;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex4i(GLint x, GLint y, GLint z, GLint w)
{
    GLfloat fv[4];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex4fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    fv[3] = w;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex4iv(const GLint v[4])
{
    GLfloat x,y,z,w,fv[4];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    z = v[2];
    w = v[3];
    vertexfv = gc->dispatchState->dispatch.Vertex4fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    fv[3] = w;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLfloat fv[4];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    vertexfv = gc->dispatchState->dispatch.Vertex4fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    fv[3] = w;
    (*vertexfv)(fv);
}

void GLAPI __glim_Vertex4sv(const GLshort v[4])
{
    GLfloat x,y,z,w,fv[4];
    void (GLAPI *vertexfv)(const GLfloat *);
    __GL_SETUP();

    x = v[0];
    y = v[1];
    z = v[2];
    w = v[3];
    vertexfv = gc->dispatchState->dispatch.Vertex4fv;
    fv[0] = x;
    fv[1] = y;
    fv[2] = z;
    fv[3] = w;
    (*vertexfv)(fv);
}

#if 0

/*
** These are all written in assembly, so these routines are commented out.
** They are still included in the file, however, because c is better for
** documentation than assembly.
*/

/*****************************************************************************/
/*
** Vertex routines which do not transform the object coordinates into 
** clip space.  Instead, they just call the clip checker, and pass the 
** vertex on.  These routines are presumably for the case where window
** coordinates can easily be computed straight from the object coordinates,
** and we won't generate clip coordinates unless we really need to clip
** the primitive.
*/

void GLAPI __glim_NoXFVertex2fv(const GLfloat v[2])
{
    __GLvertex *vx;
    __GLfloat x,y;
    void (*validate)(__GLcontext *gc, __GLvertex *v, GLuint needs);
    __GL_SETUP();

    vx = gc->vertex.v0;
    x = v[0];
    y = v[1];
    vx->obj.x = x;
    vx->obj.y = y;
    vx->obj.z = __glZero;
    vx->obj.w = __glOne;
    vx->flagBits = (*gc->procs.clipCheck2)(gc, vx) | __GL_VERTEX_2D_SIZE;
    (*gc->procs.v)(gc, vx);
}

void GLAPI __glim_NoXFVertex3fv(const GLfloat v[3])
{
    __GLvertex *vx;
    __GLfloat x,y,z;
    void (*validate)(__GLcontext *gc, __GLvertex *v, GLuint needs);
    __GL_SETUP();

    vx = gc->vertex.v0;
    x = v[0];
    y = v[1];
    z = v[2];
    vx->obj.x = x;
    vx->obj.y = y;
    vx->obj.z = z;
    vx->obj.w = __glOne;
    vx->flagBits = (*gc->procs.clipCheck3)(gc, vx) | __GL_VERTEX_3D_SIZE;
    (*gc->procs.v)(gc, vx);
}

void GLAPI __glim_NoXFVertex4fv(const GLfloat v[4])
{
    __GLvertex *vx;
    __GLfloat x,y,z,w;
    void (*validate)(__GLcontext *gc, __GLvertex *v, GLuint needs);
    __GL_SETUP();

    vx = gc->vertex.v0;
    x = v[0];
    y = v[1];
    z = v[2];
    w = v[3];
    vx->obj.x = x;
    vx->obj.y = y;
    vx->obj.z = z;
    vx->obj.w = w;
    vx->flagBits = (*gc->procs.clipCheck4)(gc, vx) | __GL_VERTEX_4D_SIZE;
    (*gc->procs.v)(gc, vx);
}

#endif /* 0 */
