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
** $Header: //depot/main/gfx/lib/opengl/glcore/vc_line.c#5 $
*/

#include <stdlib.h>
#include "context.h"
#include "global.h"
#include "glmath.h"
#include "g_imfncs.h"
#include "render.h"

#pragma optimize("a", on)

/*
** Process line strips. 
*/
void __glProcessCachedLineStrip(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount >= 2) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	drawVertices = gc->vertexArray.drawVertices[GL_LINE_STRIP];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, gc->vertexCache.vertexCount, NULL);

	/*
	** We were flushed, so we have to copy the last  vertex in the 
	** list over and set the start and count values appropriately.
	*/
	if (flushed == __GL_VCACHE_FLUSH) {
	    __GLvertex *src = vc->vertexCache + vc->vertexCount - 1;
	    __GLvertex *dst = vc->vertexCache;
	    dst[0] = src[0]; dst[0].color = &dst[0].colors[__GL_FRONTFACE];
	    vc->vertexStart = vc->vertexCount = 1;
	    return;
	}
    }

    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}

/*
** Process lines. 
*/
void __glProcessCachedLines(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;

    if (vc->vertexCount >= 2) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);
	drawVertices = gc->vertexArray.drawVertices[GL_LINES];
	assert(NULL != drawVertices);
	(*drawVertices)(gc, 0, gc->vertexCache.vertexCount, NULL);
    }
						
    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}												 
/*
** Process line loops. 
*/
void __glProcessCachedLineLoop(__GLcontext *gc, GLuint flushed)
{
    __GLvcacheMachine *vc = &gc->vertexCache;
    GLsizei count, start;
    GLboolean alreadyInLoop = vc->vertexCacheState & __GL_VCACHE_IN_LINE_LOOP;

    if (vc->vertexCount >= 2) {
	void (*drawVertices)(__GLcontext *gc, GLint first, GLsizei count, GLuint *elements);

	if (flushed == __GL_VCACHE_FLUSH || alreadyInLoop) {
	    /*
	    ** There is a partial loop in the cache and this is either
	    ** the first, last, or some middle piece, so we must render
	    ** it as a line strip.
	    */
	    drawVertices = gc->vertexArray.drawVertices[GL_LINE_STRIP];
	    if (!alreadyInLoop) {
		/* first piece of line loop */
		start = 0;
		count = vc->vertexCount;
		vc->vertexCacheState |= __GL_VCACHE_IN_LINE_LOOP;

	    } else if (flushed == __GL_VCACHE_END) {
		/* last piece of line loop */
		__GLvertex *src = vc->vertexCache + 0;
		__GLvertex *dst = vc->vertexCache + vc->vertexCount;
		dst[0] = src[0]; dst[0].color = &dst[0].colors[__GL_FRONTFACE];
		start = 1;
		count = vc->vertexCount;
		vc->vertexCacheState &= ~__GL_VCACHE_IN_LINE_LOOP;

	    } else {
		/* some middle piece of line loop */
		start = 1;
		count = vc->vertexCount - 1;
	    }
	} else {
	    /*
	    ** There is one complete loop in the cache, so we can
	    ** process it as a line loop.
	    */
	    drawVertices = gc->vertexArray.drawVertices[GL_LINE_LOOP];
	    start = 0;
	    count = vc->vertexCount;
	}

	assert(NULL != drawVertices);
	(*drawVertices)(gc, start, count, NULL);

	if (flushed == __GL_VCACHE_FLUSH) {
	    __GLvertex *src = vc->vertexCache + vc->vertexCount - 1;
	    __GLvertex *dst = vc->vertexCache + 1;
	    dst[0] = src[0]; dst[0].color = &dst[0].colors[__GL_FRONTFACE];
	    vc->vertexStart = vc->vertexCount = 2;
	    return;
	}
    }

    vc->vertexStart = vc->vertexCount = 0;
    vc->vertexCacheState &= ~__GL_VCACHE_MATERIAL_VALIDATE;
}

#pragma optimize("", on)
