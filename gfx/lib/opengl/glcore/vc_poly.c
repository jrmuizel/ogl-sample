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
** $Header: //depot/main/gfx/lib/opengl/glcore/vc_poly.c#5 $
*/
  
#include <stdlib.h>
#include "context.h"
#include "global.h"
#include "glmath.h"
#include "g_imfncs.h"

#pragma optimize("a", on)

/*
** Process triangle strips. All of the cached polygon routines use the same
** mechanisms.  The differences between them are how they handle flushing of
** the cached buffer and building the generic triangle list.
*/
void __glProcessCachedTriangleStrip(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount >= 3) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	drawVertices = gc->vertexArray.drawVertices[GL_TRIANGLE_STRIP];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, vc->vertexCount, NULL);

	/*
	** We were flushed, so we have to copy the last two vertices in the 
	** list over and set the start and count values appropriately.
	*/
	if (flushed == __GL_VCACHE_FLUSH) {
	    __GLvertex *src = vc->vertexCache + vc->vertexCount - 2;
	    __GLvertex *dst = vc->vertexCache;
	    dst[0] = src[0]; dst[0].color = &dst[0].colors[__GL_FRONTFACE];
	    dst[1] = src[1]; dst[1].color = &dst[1].colors[__GL_FRONTFACE];
	    vc->vertexStart = vc->vertexCount = 2;
	    gc->vertexArray.continuation = GL_TRUE;
	    return;
	}
    }

    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}


/*
** Process Quad strips, see the triangle mesh code above for more comments.
** The only real main difference between the two is that the triangle list
** has to be built up in a different way.
*/
void __glProcessCachedQuadStrip(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount >= 4) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	drawVertices = gc->vertexArray.drawVertices[GL_QUAD_STRIP];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, vc->vertexCount, NULL);

	/*
	** We were flushed, so we have to copy the last two vertices in the 
	** list over and set the start and count values appropriately.
	*/
	if (flushed == __GL_VCACHE_FLUSH) {
	    __GLvertex *src = vc->vertexCache + vc->vertexCount - 2;
	    __GLvertex *dst = vc->vertexCache;
	    dst[0] = src[0]; dst[0].color = &dst[0].colors[__GL_FRONTFACE];
	    dst[1] = src[1]; dst[1].color = &dst[1].colors[__GL_FRONTFACE];
	    vc->vertexStart = vc->vertexCount = 2;
	    gc->vertexArray.continuation = GL_TRUE;
	    return;
	}
    }

    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}


/*
** Polygons.
*/
void __glProcessCachedPolygon(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;
    GLboolean alreadyInPolygon = vc->vertexCacheState & __GL_VCACHE_IN_POLYGON;

    if (vc->vertexCount >= 3) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	if (flushed == __GL_VCACHE_FLUSH || alreadyInPolygon) {
	    /*
	    ** There is a partial polygon in the cache and this is either
	    ** the first, last, or some middle piece, so we must set edge
	    ** flags appropriately.
	    */
	    if (!alreadyInPolygon) {
		/* first piece of polygon */
		vc->vertexCacheState |= __GL_VCACHE_IN_POLYGON;
		vc->vertexCache[vc->vertexCount - 1].flagBits &= ~__GL_VERTEX_EDGE_FLAG;

	    } else if (flushed == __GL_VCACHE_END) {
		/* last piece of polygon */
		vc->vertexCacheState &= ~__GL_VCACHE_IN_POLYGON;

	    } else {
		/* some middle piece of polygon */
		vc->vertexCache[vc->vertexCount - 1].flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	    }
	}

	drawVertices = gc->vertexArray.drawVertices[GL_POLYGON];
	assert(NULL != drawVertices);

	(*drawVertices)(gc, 0, vc->vertexCount, NULL);

	/*
	** We have been flushed, so copy the last vertex into the second
	** vertex of the buffer. The first vertex remains intact.
	*/
	if (flushed == __GL_VCACHE_FLUSH) {
	    __GLvertex *src = vc->vertexCache + vc->vertexCount - 1;
	    __GLvertex *dst = vc->vertexCache + 1;
	    dst[0] = src[0]; dst[0].color = &dst[0].colors[__GL_FRONTFACE];
	    vc->vertexStart = vc->vertexCount = 2;
	    gc->vertexArray.continuation = GL_TRUE;

	    vc->vertexCache[0].flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	    vc->vertexCache[1].flagBits |= gc->state.current.edgeTag;
	    return;
	}
    }

    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}


/*
** Triangle Fans.
*/
void __glProcessCachedTriangleFan(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount >= 3) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	drawVertices = gc->vertexArray.drawVertices[GL_TRIANGLE_FAN];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, vc->vertexCount, NULL);

	/*
	** We have been flushed, so copy the last vertex into the second
	** vertex of the buffer. The first vertex remains intact.
	*/
	if (flushed == __GL_VCACHE_FLUSH) {
	    __GLvertex *src = vc->vertexCache + vc->vertexCount - 1;
	    __GLvertex *dst = vc->vertexCache + 1;
	    dst[0] = src[0]; dst[0].color = &dst[0].colors[__GL_FRONTFACE];
	    vc->vertexStart = vc->vertexCount = 2;
	    gc->vertexArray.continuation = GL_TRUE;
	    return;
	}
    }

    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}


/*
** Independent Triangles.
*/
void __glProcessCachedTriangles(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount >= 3) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);
	drawVertices = gc->vertexArray.drawVertices[GL_TRIANGLES];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, vc->vertexCount, NULL);
    }

    /*
    ** Whether we have been flushed or have hit an end it makes no difference
    ** because the cache buffer size is divisible by 3.
    */
    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}


/*
** Independent Quads.
*/
void __glProcessCachedQuads(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount >= 4) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);
	drawVertices = gc->vertexArray.drawVertices[GL_QUADS];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, vc->vertexCount, NULL);
    }

    /*
    ** Whether we have been flushed or have hit an end it makes no difference
    ** because the cache buffer size is divisible by 4.
    */
    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}

#pragma optimize("", on)
