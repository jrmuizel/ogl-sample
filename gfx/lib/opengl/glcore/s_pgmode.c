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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_pgmode.c#4 $
*/
#include "context.h"
#include "global.h"

/*
** Generic triangle handling code.  This code is used when render mode
** is GL_RENDER and the polygon modes are not both fill.
*/
void __glRenderTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			__GLvertex *c)
{
    GLuint needs, modeFlags, faceNeeds;
    GLint ccw, colorFace, face;
    __GLfloat dxAC, dxBC, dyAC, dyBC, area;
    __GLvertex *pv;

    /* Compute signed area of the triangle */
    dxAC = a->window.x - c->window.x;
    dxBC = b->window.x - c->window.x;
    dyAC = a->window.y - c->window.y;
    dyBC = b->window.y - c->window.y;
    area = dxAC * dyBC - dxBC * dyAC;
    ccw = !(*(int *)&area >> 31);

    /* Figure out if face is culled or not */
    face = gc->polygon.face[ccw];
    if (face == gc->polygon.cullFace) {
	/* Culled */
	return;
    }

    /*
    ** Pick face to use for coloring
    */
    modeFlags = gc->polygon.shader.modeFlags;
    if (modeFlags & __GL_SHADE_TWOSIDED) {
	colorFace = face;
	faceNeeds = gc->vertex.faceNeeds[face];
    } else {
	colorFace = __GL_FRONTFACE;
	faceNeeds = gc->vertex.faceNeeds[__GL_FRONTFACE];
    }

    /*
    ** Choose colors for the vertices.
    */
    needs = gc->vertex.needs;
    pv = gc->vertex.provoking;
    if (modeFlags & __GL_SHADE_SMOOTH_LIGHT) {
	/* Smooth shading */
	a->color = &a->colors[colorFace];
	b->color = &b->colors[colorFace];
	c->color = &c->colors[colorFace];
	needs |= faceNeeds;
    } else {
	GLuint pvneeds;

	/*
	** Validate the lighting (and color) information in the provoking
	** vertex only.  Fill routines always use gc->vertex.provoking->color
	** to find the color.
	*/
	pv->color = &pv->colors[colorFace];
	a->color = pv->color;
	b->color = pv->color;
	c->color = pv->color;
	pvneeds = faceNeeds & (__GL_HAS_LIGHTING | 
		__GL_HAS_FRONT_COLOR | __GL_HAS_BACK_COLOR);
	if (~pv->flagBits & pvneeds) DO_VALIDATE(gc, pv, pvneeds);
    }

    /* Validate vertices */
    if (~a->flagBits & needs) DO_VALIDATE(gc, a, needs);
    if (~b->flagBits & needs) DO_VALIDATE(gc, b, needs);
    if (~c->flagBits & needs) DO_VALIDATE(gc, c, needs);

    /* Render triangle using the faces polygon mode */
    switch (gc->polygon.mode[face]) {
      case __GL_POLYGON_MODE_FILL:
	  if (*(int *)&area << 1) {
	      (*gc->procs.fillTriangle)(gc, a, b, c);
	  }
	  break;
      case __GL_POLYGON_MODE_POINT:
	  (*gc->procs.renderTrianglePolygonModePoint)(gc, a, b, c);
	  break;
      case __GL_POLYGON_MODE_LINE:
	  (*gc->procs.renderTrianglePolygonModeLine)(gc, a, b, c);
	  break;
    }

    /* Restore color pointers */
    a->color = &a->colors[__GL_FRONTFACE];
    b->color = &b->colors[__GL_FRONTFACE];
    c->color = &c->colors[__GL_FRONTFACE];
    pv->color = &pv->colors[__GL_FRONTFACE];
}

/*
** the following routines are used when rendering a triangle with a 
** non-fill polygon mode (line or point).  They are called from
** __glRenderTriangle.
**
** There are lock and non-lock versions
*/
void
__glRenderTriangleLineLock(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			   __GLvertex *c)
{
    __GL_LOCK_RENDER_BUFFERS(gc);

    if (a->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderLine)(gc, a, b);
    if (b->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderLine)(gc, b, c);
    if (c->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderLine)(gc, c, a);

    __GL_UNLOCK_RENDER_BUFFERS(gc);
}

void
__glRenderTriangleLine(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
		       __GLvertex *c)
{
    if (a->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderLine)(gc, a, b);
    if (b->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderLine)(gc, b, c);
    if (c->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderLine)(gc, c, a);
}

void
__glRenderTrianglePointLock(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			    __GLvertex *c)
{
    __GL_LOCK_RENDER_BUFFERS(gc);

    if (a->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderPoint)(gc, a);
    if (b->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderPoint)(gc, b);
    if (c->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderPoint)(gc, c);

    __GL_UNLOCK_RENDER_BUFFERS(gc);
}

void
__glRenderTrianglePoint(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
		       __GLvertex *c)
{
    if (a->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderPoint)(gc, a);
    if (b->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderPoint)(gc, b);
    if (c->flagBits & __GL_VERTEX_EDGE_FLAG) (*gc->procs.renderPoint)(gc, c);
}

#if !(defined(__GL_USE_ASMCODE) && defined(mips))

/************************************************************************/

/*
** Generic triangle handling code.  This code is used when render mode
** is GL_RENDER and both polygon modes are FILL and the triangle is
** being flat shaded.
*/
void __glRenderFlatTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			    __GLvertex *c)
{
    GLuint needs, pvneeds, modeFlags, faceNeeds;
    GLint ccw, colorFace, face;
    __GLfloat dxAC, dxBC, dyAC, dyBC, area;
    __GLvertex *pv;

    /* Compute signed area of the triangle */
    dxAC = a->window.x - c->window.x;
    dxBC = b->window.x - c->window.x;
    dyAC = a->window.y - c->window.y;
    dyBC = b->window.y - c->window.y;
    area = dxAC * dyBC - dxBC * dyAC;
    if (!(*(int *)&area << 1)) {
	return;
    }
    ccw = !(*(int *)&area >> 31);

    /* Figure out if face is culled or not */
    face = gc->polygon.face[ccw];
    if (face == gc->polygon.cullFace) {
	/* Culled */
	return;
    }

    /*
    ** Pick face to use for coloring
    */
    modeFlags = gc->polygon.shader.modeFlags;
    if (modeFlags & __GL_SHADE_TWOSIDED) {
	colorFace = face;
	faceNeeds = gc->vertex.faceNeeds[face];
    } else {
	colorFace = __GL_FRONTFACE;
	faceNeeds = gc->vertex.faceNeeds[__GL_FRONTFACE];
    }

    /*
    ** Choose colors for the vertices.
    */
    needs = gc->vertex.needs;
    pv = gc->vertex.provoking;

    /*
    ** Validate the lighting (and color) information in the provoking
    ** vertex only.  Fill routines always use gc->vertex.provoking->color
    ** to find the color.
    */
    pv->color = &pv->colors[colorFace];
    pvneeds = faceNeeds & (__GL_HAS_LIGHTING |
	    __GL_HAS_FRONT_COLOR | __GL_HAS_BACK_COLOR);
    if (~pv->flagBits & pvneeds) DO_VALIDATE(gc, pv, pvneeds);

    /* Validate vertices */
    if (~a->flagBits & needs) DO_VALIDATE(gc, a, needs);
    if (~b->flagBits & needs) DO_VALIDATE(gc, b, needs);
    if (~c->flagBits & needs) DO_VALIDATE(gc, c, needs);

    /* Fill triangle */
    (*gc->procs.fillTriangle)(gc, a, b, c);

    /* Restore color pointers */
    pv->color = &pv->colors[__GL_FRONTFACE];
}

/************************************************************************/

/*
** Generic triangle handling code.  This code is used when render mode
** is GL_RENDER and both polygon modes are FILL and the triangle is
** being smooth shaded.
*/
void __glRenderSmoothTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			      __GLvertex *c)
{
    GLuint needs, modeFlags;
    GLint ccw, colorFace, face;
    __GLfloat dxAC, dxBC, dyAC, dyBC, area;

    /* Compute signed area of the triangle */
    dxAC = a->window.x - c->window.x;
    dxBC = b->window.x - c->window.x;
    dyAC = a->window.y - c->window.y;
    dyBC = b->window.y - c->window.y;
    area = dxAC * dyBC - dxBC * dyAC;
    if (!(*(int *)&area << 1)) {
	return;
    }
    ccw = !(*(int *)&area >> 31);

    /* Figure out if face is culled or not */
    face = gc->polygon.face[ccw];
    if (face == gc->polygon.cullFace) {
	/* Culled */
	return;
    }

    /*
    ** Pick face to use for coloring
    */
    modeFlags = gc->polygon.shader.modeFlags;
    needs = gc->vertex.needs;
    if (modeFlags & __GL_SHADE_TWOSIDED) {
	colorFace = face;
	needs |= gc->vertex.faceNeeds[face];
    } else {
	colorFace = __GL_FRONTFACE;
	needs |= gc->vertex.faceNeeds[__GL_FRONTFACE];
    }

    /*
    ** Choose colors for the vertices.
    */
    a->color = &a->colors[colorFace];
    b->color = &b->colors[colorFace];
    c->color = &c->colors[colorFace];

    /* Validate vertices */
    if (~a->flagBits & needs) DO_VALIDATE(gc, a, needs);
    if (~b->flagBits & needs) DO_VALIDATE(gc, b, needs);
    if (~c->flagBits & needs) DO_VALIDATE(gc, c, needs);

    /* Fill triangle */
    (*gc->procs.fillTriangle)(gc, a, b, c);

    /* Restore color pointers */
    a->color = &a->colors[__GL_FRONTFACE];
    b->color = &b->colors[__GL_FRONTFACE];
    c->color = &c->colors[__GL_FRONTFACE];
}

#endif /* !(__GL_USE_ASMCODE && mips) */

/* ARGSUSED */
void __glDontRenderTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			    __GLvertex *c)
{
}
