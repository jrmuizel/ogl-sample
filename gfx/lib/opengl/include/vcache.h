#ifndef __glvertexcache
#define __glvertexcache

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
** $Header: //depot/main/gfx/lib/opengl/include/vcache.h#5 $
*/

/* Size of the vertex cache. */
/* The value of the __GL_VCACHE_MAXSIZE should be a multiple of both
** 3 and 4 - divisible by 12.  This is so that for independent
** quads and triangles, a whole number of them will fit into the 
** buffer thereby making the flushing operation much easier.
*/
#define	__GL_VCACHE_MAXSIZE		36

typedef struct __GLvcacheRec {                   
    __GLvertex 	*vertexCache;		/* Vertex Cache buffer */
    int		vertexCount;		/* Count of the number of vertices */
    int		vertexIncrement;	/* increment count (0 or 1) */
    GLbitfield	vertexCacheState;	/* state of vertex cache */
    GLboolean	vertexCacheEnabled;	/* Global vertex caching enable flag */
    GLboolean	colorChanged;		/* detect color changes */
    GLenum	cachedPrimitiveType; 	/* Type of primitive in the cache */
    int		vertexStart;		/* MatValidate starts here */
    int		vertexType;		/* Types of vertexes in the cache */
    void	(*save)(__GLcontext*, __GLvertex*);
    void	(*compileElements[3])(__GLcontext *gc, GLint offset,
				      GLint first, GLsizei count);

    /* procs to process the cached primitives */
    void	(*process[10])(__GLcontext *gc, GLuint flushed);
    void	(*process2[10])(__GLcontext *gc, GLuint flushed);
} __GLvcacheMachine;

/* Types of vertices in a polygon, these are bit-significant. */
#define	__GL_VCACHE_TYPE_2D		0x1
#define	__GL_VCACHE_TYPE_3D		0x2
#define	__GL_VCACHE_TYPE_4D		0x4

/* Flush types */
#define	__GL_VCACHE_END			0x1
#define	__GL_VCACHE_FLUSH		0x2

/* VertexCacheState flags */
#define __GL_VCACHE_IN_LINE_LOOP	0x1  /* Continuing a line loop */
#define __GL_VCACHE_IN_POLYGON		0x2  /* Continuing a polygon */
#define __GL_VCACHE_CAT_PRIMITIVE	0x4  /* Primitives are being concatenated */
#define __GL_VCACHE_MATERIAL_VALIDATE	0x8  /* Some vertices have been validated */

/* vc_cache */
extern void GLAPI __glim_VertexCacheBegin(GLenum mode);
extern void GLAPI __glim_VertexCacheEnd(void);
extern void GLAPI __glim_VertexCache2fv(const GLfloat *);
extern void GLAPI __glim_VertexCache3fv(const GLfloat *);
extern void GLAPI __glim_VertexCache4fv(const GLfloat *);

extern void __glBeginVcache(__GLcontext *gc);
extern void __glEndVcache(__GLcontext *gc);

extern void __glInitVertexCacheState(__GLcontext *gc);
extern void __glFreeVertexCacheState(__GLcontext *gc);
extern void __glProcessVertexCache(__GLcontext *gc, GLuint flushed);
extern void __glProcessVertexCacheLock(__GLcontext *gc, GLuint flushed);
extern void __glGenericPickVertexCacheProcs(__GLcontext *gc);
extern void __glMatValidateVcache(__GLcontext *gc);

/* vc_line */
extern void __glProcessCachedLineStrip(__GLcontext *gc, GLuint flushed);
extern void __glProcessCachedLines(__GLcontext *gc, GLuint flushed);
extern void __glProcessCachedLineLoop(__GLcontext *gc, GLuint flushed);

/* vc_point */
extern void __glProcessCachedPoints(__GLcontext *gc, GLuint flushed);

/* vc_poly */
extern void __glProcessCachedTriangleStrip(__GLcontext *gc, GLuint flushed);
extern void __glProcessCachedQuadStrip(__GLcontext *gc, GLuint flushed);
extern void __glProcessCachedPolygon(__GLcontext *gc, GLuint flushed);
extern void __glProcessCachedTriangleFan(__GLcontext *gc, GLuint flushed);
extern void __glProcessCachedTriangles(__GLcontext *gc, GLuint flushed);
extern void __glProcessCachedQuads(__GLcontext *gc, GLuint flushed);

#endif /* __glvertexcache */
