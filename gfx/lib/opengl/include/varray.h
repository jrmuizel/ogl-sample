#ifndef __gl_varray_h_
#define __gl_varray_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/varray.h#11 $
*/

/* This defines the stuff in gc->vertArrayState.signature, which is a 
 * bitmask describing current vertex array enables.  ven_get.c has to see
 * these definitions */

/* The signature field is set up the following way:
 * (hi)
 * 4 bits of index type 	0xf0000000
 * 1 bit of edge existence	0x08000000
 * 3 bits of texture size	0x07000000
 * 4 bits of texture type	0x00f00000
 * 4 bits of color size		0x000f0000
 * 4 bits of color type		0x0000f000
 * 4 bits of normal type 	0x00000f00
 * 4 bits of vertex size	0x000000f0
 * 4 bits of vertex type	0x0000000f
 * (lo)
 *
 * Note that edge existence flag is cramped in with texture size (just because
 * we don't have room for everything to have 4 bits)
 * Types are mapped into numbers from by TypeFlag(GLenum)
 */

#define VERTARRAY_V_MASK		0x000000ff
#define VERTARRAY_V_INDEX		32
#define VERTARRAY_VTYPE_SHIFT		0
#define VERTARRAY_VTYPE_MASK		(0xf << VERTARRAY_VTYPE_SHIFT)
#define VERTARRAY_VSIZE_SHIFT		4
#define VERTARRAY_VSIZE_MASK		(0xf << VERTARRAY_VSIZE_SHIFT)

#define VERTARRAY_N_MASK		0x00000f00
#define VERTARRAY_N_INDEX		1
#define VERTARRAY_NTYPE_SHIFT		8
#define VERTARRAY_NTYPE_MASK		(0xf << VERTARRAY_NTYPE_SHIFT)

#define VERTARRAY_C_MASK		0x000ff000
#define VERTARRAY_C_INDEX		2
#define VERTARRAY_CTYPE_SHIFT		12
#define VERTARRAY_CTYPE_MASK		(0xf << VERTARRAY_CTYPE_SHIFT)
#define VERTARRAY_CSIZE_SHIFT		16
#define VERTARRAY_CSIZE_MASK		(0xf << VERTARRAY_CSIZE_SHIFT)

#define VERTARRAY_T_MASK		0x07f00000
#define VERTARRAY_T_INDEX		4
#define VERTARRAY_TTYPE_SHIFT		20
#define VERTARRAY_TTYPE_MASK		(0xf << VERTARRAY_TTYPE_SHIFT)
#define VERTARRAY_TSIZE_SHIFT		24
#define VERTARRAY_TSIZE_MASK		(0x7 << VERTARRAY_TSIZE_SHIFT)

#define VERTARRAY_E_MASK		0x08000000
#define VERTARRAY_E_INDEX		16

#define VERTARRAY_I_MASK		0xf0000000
#define VERTARRAY_I_INDEX		8
#define VERTARRAY_ITYPE_SHIFT		28
#define VERTARRAY_ITYPE_MASK		(0xf << VERTARRAY_ITYPE_SHIFT)

/* type flags for signatures.  we add one here to make sure that 
 * the part of the signature corresponding to something that is enabled
 * (and has been correctly specified) is non-zero.  Kona ucode uses
 * this fact, and also the connection to the GL_<whatever> values.  
 * also, all values must be representable as a 4-bit number. */
#define VERTARRAY_BYTE_FLAG               (GL_BYTE - GL_BYTE + 1)
#define VERTARRAY_UNSIGNED_BYTE_FLAG      (GL_UNSIGNED_BYTE - GL_BYTE + 1)
#define VERTARRAY_SHORT_FLAG              (GL_SHORT - GL_BYTE + 1)
#define VERTARRAY_UNSIGNED_SHORT_FLAG     (GL_UNSIGNED_SHORT - GL_BYTE + 1)
#define VERTARRAY_INT_FLAG                (GL_INT - GL_BYTE + 1)
#define VERTARRAY_UNSIGNED_INT_FLAG       (GL_UNSIGNED_INT - GL_BYTE + 1)
#define VERTARRAY_FLOAT_FLAG              (GL_FLOAT - GL_BYTE + 1)
#define VERTARRAY_DOUBLE_FLAG             (GL_DOUBLE - GL_BYTE + 1)

/* Vertex array control word flags. */
#define VERTARRAY_CW_ALLOW_PRECOMPILE   0x00000001      /* compilable array */
#define VERTARRAY_CW_NEEDS_COMPILE      0x00000002      /* compile needed */
#define VERTARRAY_CW_LOCKED             0x00000004      /* locked currently */
#define VERTARRAY_CW_VX_CULL            0x00000008      /* per vertex cull on */
#define VERTARRAY_CW_VX_CULL_LOCAL      0x00000010      /* local cull */
#define VERTARRAY_CW_SLOWPATH		0x00000020	/* fallback to vcache */
#define VERTARRAY_CW_NO_VERTEX		0x00000040	/* fallback to vcache */

#define VERTARRAY_CW_DISABLE_FAST       0x00000080      /* disable fast paths */
#define VERTARRAY_CW_DISABLE_RASTER     0x00000100      /* for timing */
#define VERTARRAY_CW_SAME_BUF           0x00002000	/* SameBuf enabled */
#define VERTARRAY_CW_SMALL_GLVERTEX     0x00004000	/* small vx struct */
#define VERTARRAY_CW_SKIP_VXCULL     	0x00008000	/* save branches */
#define VERTARRAY_CW_COD                0x00010000      /* compile on demand */

/****************************************************************************/

typedef struct {
    const char *pointer;
    GLsizei stride;
    void (GLAPI *proc)(const char *);
    void (*copy)(__GLcontext *gc, const void *vp, __GLvertex *v);
    GLint size;
    GLenum type;
    GLsizei userStride;
    GLsizei count;
} __GLvertArrayPointerState;

/*
 * Vertex array state.
 */
typedef struct {
    __GLvertArrayPointerState vp;
    __GLvertArrayPointerState np;
    __GLvertArrayPointerState cp;
    __GLvertArrayPointerState ip;
    __GLvertArrayPointerState tp[__GL_NUM_TEXTURE_UNITS];
    __GLvertArrayPointerState ep;

    GLbitfield textureEnables;
    GLbitfield signature;
    GLbitfield mask;
    GLint index;

    const char *interleavedPointer;	/* pointer to interleaved array */
    GLenum 	interleavedFormat;	/* set by InterleavedArrays */
    GLuint 	controlWord;

    GLuint 	validateMask;		/* Bits for validate */

    GLint	start;			/* set by LockArrays */
    GLint	count;			/* set by LockArrays */

    GLint 	blockSize;		/* size of incremental buf allocs */
    GLint	numVertices;		/* number of vxs currently supported */

    GLint varrayBufSize;		/* size in bytes of varrayBuf */
    __GLvertex  *varrayBuf;		/* internal vertex array */
    __GLvertex  *varrayPtr;		/* used when processing varrayBuf */

    GLint 	batchSize;		/* number of vertices in batch */

    GLenum batchMode;
    GLint batchFirst;
    GLuint *batchElements;
    GLint batchIndex;

    GLuint cullCode[2];

    void (*drawVertices[10])(__GLcontext *gc, GLint first, 
				     GLsizei count, GLuint *elements);

    /* bitflag specifying which primitive modes we are locking */
    GLuint drawVerticesLockMask;

    void (*copyVertex)(__GLcontext *gc, const void *vp, __GLvertex *v);
    void (*copyNormal)(__GLcontext *gc, const void *vp, __GLvertex *v);
    void (*copyColor)(__GLcontext *gc, const void *vp, __GLvertex *v);
    void (*copyIndex)(__GLcontext *gc, const void *vp, __GLvertex *v);
    void (*copyTexCoord)(__GLcontext *gc, const void *vp, __GLvertex *v);
    void (*copyEdgeFlag)(__GLcontext *gc, const void *vp, __GLvertex *v);

    GLint compileIndex;
    GLbitfield compileMask;

    void (*compileElementsArray)(__GLcontext *gc, GLint offset,
				 GLint first, GLsizei count);
    void (*compileElements)(__GLcontext *gc, GLint offset,
			    GLint first, GLsizei count);
    void (*compileElementsIndexed)(__GLcontext *gc, GLint offset, GLint first,
				   GLsizei count, GLuint *elements);
    void (*compileElementsNoCull)(__GLcontext *gc, GLint offset,
				  GLint first, GLsizei count);
    void (*compileElementsSilhouette)(__GLcontext *gc, __GLvertex *v,
				      GLint index);

    void (*xf)(__GLcoord *res, const __GLfloat *v, const __GLmatrix *m);
    GLuint (*clipCheck)(__GLcontext *gc, __GLvertex *vx);
    void (*validateVertex)(__GLcontext *gc, __GLvertex *v, GLuint needs);
    GLboolean continuation;		/* Are we in the rest of strip or fan? */

    GLint maxElementsVertices;
    GLint maxElementsIndices;
} __GLvertArrayMachine;

typedef __GLvertArrayMachine __GLclientVertArrayState;

extern void __glInitVertexArrayState(__GLcontext *gc);

extern void __glCompileElements_NoCopy(__GLcontext *gc, GLint offset,
				       GLint first, GLsizei count);
extern void __glCompileElementsSilhouette_NoCopy(__GLcontext *gc, 
						 __GLvertex *v, GLint index);
extern void __glVarrayClipTriangle(__GLcontext *gc, __GLvertex *v0,
			       __GLvertex *v1, __GLvertex *v2);

#endif /* __gl_varray_h_ */
