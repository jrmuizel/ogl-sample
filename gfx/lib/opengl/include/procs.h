#ifndef __gl_procs_h_
#define __gl_procs_h_
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
** $Header: //depot/main/gfx/lib/opengl/include/procs.h#14 $
*/
#include "types.h"
#include "dlist.h"

/*
** These typedefs are used to normalize the calling conventions
** for the span procs.  Some of the arguments are not used by
** many of the span procs, but the arguments are present so that
** the function pointers in the spanProcsRec can be interchanged.
** The idea is to move up in the calling sequence as high as possible
** the final "store" span proc.
**
** The type __GLspanFunc returns GL_TRUE if it stippled the span while
** processing it.  If it also stippled the span all black, it sets
** gc->polygon.shader.done to GL_TRUE.
**
** The type __GLstippledSpanFunc return GL_TRUE if it stippled the span
** to all black, and GL_FALSE otherwise.
*/
typedef GLboolean (*__GLspanFunc)(__GLcontext *gc);
typedef GLboolean (*__GLstippledSpanFunc)(__GLcontext *gc);

#define __GL_MAX_SPAN_FUNCS	15
#define __GL_MAX_LINE_FUNCS	16

typedef struct __GLspanProcsRec {
    /*
    ** First phase of span processing.  Clip the span so that it won't
    ** render outside of the intersection of the window box and the
    ** scissor box.  Then call the stipple proc to replicate the stipple
    ** (and rotate it) sufficient for rendering w pixels.  If there is no
    ** active polygon stipple then the stipple proc is skipped, and the
    ** unstippled form of the next phase of procs is used until stippling
    ** becomes necessary.
    **
    ** Second phase of span processing.  Apply the various test functions 
    ** producing at the end a final stipple for the span.  
    ** Each test procedure outputs a new stipple as
    ** needed, calling the stippled form of the next proc only if the
    ** test failed somewhere in the span.
    **
    ** Next phase of span processing.  This phase is responsible for
    ** generating the final colors to be stored.  The operations are
    ** applied in order as shown below, producing at the end the final
    ** color values.  draw is used to replicate the span so that it
    ** properly renders to the correct number of destination buffers
    ** (e.g., when drawBuffer is FRONT_AND_BACK).
    **
    ** Final phase of span rendering.  Apply blend function, dither
    ** operation, logic-op and writemask before calling the store
    ** proc.  When blending, logic-oping, or writemasking is being done,
    ** the fetch proc will be used to read in the span (from the draw
    ** buffer) before proceeding furthur.
    */

    /*
    ** The 15 layers of the span procs are:
    **
    ** scissor, poly stipple, alpha test, stencil test, depth test,
    ** shading, texturing, fogging, FRONT_AND_BACK drawing, fetching,
    ** blending, dithering, logic op, masking, storing.
    */
    __GLspanFunc spanFuncs[__GL_MAX_SPAN_FUNCS];
    __GLstippledSpanFunc stippledSpanFuncs[__GL_MAX_SPAN_FUNCS];

    /* 
    ** The number of procs stored in the span function arrays.  n is 
    ** the number applied prior to span replication (for drawing to both
    ** FRONT_AND_BACK buffers), and m is the total number applied.
    */
    GLint n, m;

    /*
    ** This is the root span function.  It is called when a span needs 
    ** processing.
    */
    __GLspanFunc processSpan;

#ifdef __GL_USE_MIPSASMCODE
    /*
    ** Assembly routine to depth test a single pixel.  There is no prototype,
    ** since variables are passed in registers.
    */
    void (*depthTestPixel)(void);
#endif

} __GLspanProcs;

typedef struct __GLlineProcsRec {
    /*
    ** The line procs are very similar to the span procs.  The biggest 
    ** difference is that they iterate along a line instead of a span.
    **
    ** The prototypes for the line procs are identical to the prototypes 
    ** to the poly span paths so that some of the leaves can be shared.
    **
    ** The layers of the line procs are as follows:
    **
    ** scissor, line stipple, alpha test, stencil test, depth test, shading,
    ** texturing, fogging, wide line duplication, FRONT_AND_BACK drawing, 
    ** fetching, blending, dithering, logic op, masking, storing.
    */
    __GLspanFunc lineFuncs[__GL_MAX_LINE_FUNCS];
    __GLstippledSpanFunc stippledLineFuncs[__GL_MAX_LINE_FUNCS];

    /* 
    ** The number of procs stored in the line function arrays.  n is 
    ** the number applied prior to wide line replication (for lines of 
    ** width greater than 1), m is the total number applied prior to 
    ** FRONT_AND_BACK line replication, and l is the total number applied
    ** altogether (l > m > n).
    */
    GLint n, m, l;

    /*
    ** This is the root line function.  It is called when a line needs 
    ** processing.
    */
    __GLspanFunc processLine;

    /*
    ** One of these procs is called after the first n procs have been
    ** completed.  This proc is responsible for replicating a wide line
    ** numerous times.
    */
    __GLspanFunc wideLineRep;
    __GLstippledSpanFunc wideStippledLineRep;

    /*
    ** One of these procs is called after the first m procs have been
    ** completed.  This proc is responsible for replication a line to 
    ** be drawn to both the FRONT and BACK buffers.
    */
    __GLspanFunc drawLine;
    __GLstippledSpanFunc drawStippledLine;

#ifdef __GL_USE_MIPSASMCODE
    /*
    ** Assembly routine to depth test a single pixel.  There is no prototype,
    ** since variables are passed in registers.
    */
    void (*depthTestPixel)(void);
#endif

} __GLlineProcs;

typedef struct __GLpixelSpanProcsRec {
    /*
    ** Pixel span reading routines.  For examples of each, see pixel/px_read.c.
    **
    ** These routines read pixel spans for CI, RGBA, Stencil and Depth.  The
    ** base versions perform pixel skipping, and the '2' versions simply read
    ** the span straight.  The RGBA routines should produce outgoing scaled
    ** colors.
    */
    void (*spanReadCI)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
		       GLvoid *span);
    void (*spanReadCI2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			GLvoid *span);
    void (*spanReadRGBA)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			 GLvoid *span);
    void (*spanReadRGBA2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			  GLvoid *span);
    void (*spanReadDepth)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			  GLvoid *span);
    void (*spanReadDepth2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   GLvoid *span);
    void (*spanReadStencil)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    GLvoid *span);
    void (*spanReadStencil2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     GLvoid *span);

    /*
    ** Pixel span rendering routines.  For examples of each, see 
    ** pixel/px_render.c.
    **
    ** These routines render pixel spans for CI, RGBA, Stencil and Depth.  The
    ** base versions perform pixel replication, and the '2' versions simply
    ** render the span straight.  The RGBA routines should take incoming 
    ** scaled colors.
    */
    void (*spanRenderCI)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			 const GLvoid *span);
    void (*spanRenderCI2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			  const GLvoid *span);
    void (*spanRenderRGBA)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			   const GLvoid *span);
    void (*spanRenderRGBA2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *span);
    void (*spanRenderDepth)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			    const GLvoid *span);
    void (*spanRenderDepth2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			     const GLvoid *span);
    void (*spanRenderStencil)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			      const GLvoid *span);
    void (*spanRenderStencil2)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo, 
			       const GLvoid *span);
} __GLpixelSpanProcs;

/************************************************************************/

/*
** Matrix handling function pointers.  Per matrix procs are kept
** elsewhere (see xform.h).  These functions are used to compute a
** matrix, not to use a matrix for computations.
*/
typedef struct __GLmatrixProcsRec {
    void (*copy)(__GLmatrix *d, const __GLmatrix *s);

    void (*invertTranspose)(__GLmatrix *d, const __GLmatrix *s);

    void (*makeIdentity)(__GLmatrix *d);

    void (*mult)(__GLmatrix *d, const __GLmatrix *a, const __GLmatrix *b);
} __GLmatrixProcs;

/************************************************************************/

/*
** A simple memory manager interface.  This manager is used by the 
** display list compiler.  This does mean that it needs to be set once
** and only once at CreateContext time because pixmap rendering will
** want to share the same allocation scheme as the direct renderer.
*/

/*
** This structure remains hidden for modularity's sake.
*/
typedef struct __GLmemoryManagerProcsRec {
    __GLarena *(*newArena)(__GLcontext *gc);
    void (*deleteArena)(__GLarena *);
    void *(*alloc)(__GLarena *, unsigned int size);
    void (*freeAll)(__GLarena *);
} __GLmemoryManagerProcs;

/************************************************************************/

/*
** Display list interface. These functions control the creation, compilation,
** optimization, and execution of display lists. These functions allow an
** implementation to customize the internal format and execution of display
** lists.
*/
typedef struct __GLdlistControlProcsRec {
  __GLdlist *(*compiler)(__GLcontext *gc, __GLcompiledDlist *input);
  void (*optimizer)(__GLcontext *gc, __GLcompiledDlist *list);
  __GLDlistExecFn execute;
} __GLdlistControlProcs;


/************************************************************************/

typedef struct __GLprocsRec __GLprocs;
struct __GLprocsRec {

    /* Flush and finish procs */
    void (*flush)(__GLcontext *gc);
    void (*finish)(__GLcontext *gc);
    void (*error)(__GLcontext *gc, GLenum code);

    /**************************************************************/

    /*
    ** Hardware/Software Coordination. In a mixed-rendering system, 
    ** software code must wait for the drawing engine to finish before 
    ** beginning software rendering
    */
    void (*waitIdle)(__GLcontext *gc);

    /**************************************************************/

    /*
    ** Context function pointer management procs.
    */

    /* Validate the context derived state */
    void (*validate)(__GLcontext *gc);

    /*
    ** Pick procs to choose the other procs in this structure.  These
    ** default to procedures in pick.c (and elsewhere) but can be
    ** overriden by the machine dependent context code.
    */
    void (*pickBlendProcs)(__GLcontext *gc);
    void (*pickColorMaterialProcs)(__GLcontext *gc);
    void (*pickTextureProcs)(__GLcontext *gc);
    void (*pickCalcTextureProcs)(__GLcontext *gc);
    void (*pickFogProcs)(__GLcontext *gc);
    void (*pickTransformProcs)(__GLcontext *gc);

    void (*pickPointProcs)(__GLcontext *gc);
    void (*pickLineProcs)(__GLcontext *gc);
    void (*pickTriangleProcs)(__GLcontext *gc);
    void (*pickRenderBitmapProcs)(__GLcontext *gc);
    void (*pickPixelProcs)(__GLcontext *gc);

    void (*pickClipProcs)(__GLcontext *gc);
    void (*pickParameterClipProcs)(__GLcontext *gc);

    void (*pickBufferProcs)(__GLcontext *gc);
    void (*pickStoreProcs)(__GLcontext *gc);
    void (*pickSpanProcs)(__GLcontext *gc);
    void (*pickVertexProcs)(__GLcontext *gc);
    void (*pickVertexArrayProcs)(__GLcontext *gc);
    void (*pickVertexCacheProcs)(__GLcontext *gc);

    void (*pickMatrixProcs)(__GLcontext *gc, __GLmatrix *m);
    void (*pickInvTransposeProcs)(__GLcontext *gc, __GLmatrix *m);
    void (*pickMvpMatrixProcs)(__GLcontext *gc, __GLmatrix *m);

    GLint (*pickDepthProcs)(__GLcontext *gc);

    GLboolean (*pickAllProcsPhase1)(__GLcontext *gc);
    void (*pickAllProcsPhase2)(__GLcontext *gc);

    /**************************************************************/

    /*
    ** Function pointers used for transformation, viewport and normal
    ** processing.
    */

    /* Proces used to operate on individual matricies */
    __GLmatrixProcs matrix;

    /* Matrix stack operations */
    void (*pushMatrix)(__GLcontext *gc);
    void (*popMatrix)(__GLcontext *gc);
    void (*loadIdentity)(__GLcontext *gc);

    /* Recompute the inverse transpose of a given model-view matrix */
    void (*computeInverseTranspose)(__GLcontext *gc, __GLtransform *tr);

    /* Normalize an incoming normal coordinate */
    void (*normalize)(__GLfloat dst[3], const __GLfloat src[3]);

    /* Scale a vector by a scalar constant.  This is presently used for
    **  the GL_RESCALE_NORMAL functionality */
    void (*scaleVector)(__GLfloat dst[3], const __GLfloat c,
                        const __GLfloat src[3]);

    /* Called when the scissor changes, or is enabled/disabled */
    GLboolean (*applyScissor)(__GLcontext *gc);

    /* Called when the viewport changes */
    GLboolean (*applyViewport)(__GLcontext *gc);

    /* Called to compute clipX0, clipX1, clipY0, clipY1 */
    void (*computeClipBox)(__GLcontext *gc);

    /**************************************************************/

    /*
    ** Function pointers used for vertex processing
    */

    /*
    ** Vertex handlers.
    */
    void (*v)(__GLcontext *gc, __GLvertex *vx);

    /* Clip checking */
    /* For Vertex2*() commands */
    GLuint (*clipCheck2)(__GLcontext *gc, __GLvertex *vx);

    /* For Vertex3*() commands */
    GLuint (*clipCheck3)(__GLcontext *gc, __GLvertex *vx);

    /* For Vertex4*() commands */
    GLuint (*clipCheck4)(__GLcontext *gc, __GLvertex *vx);

    /*
    ** Vertex finish procedure.  When all other related pre-computation
    ** for the vertex has finished, this procedure is invoked to
    ** handle the vertex.  The procedure this points to is a function
    ** of the currently active rendering primitive.
    **
    ** vertexPoints is used for GL_POINTS
    ** vertexLStrip is used for GL_LINE_LOOP and GL_LINE_STRIP
    ** vertex2ndLines is used for even vertices of GL_LINES
    ** vertex2ndLLoop is used for even vertices of GL_LINE_LOOP
    ** endLLoop is used at the end of a GL_LINE_LOOP
    ** vertexTFan is used for GL_TRIANGLE_FAN
    ** vertexEvenTStrip is used for even vertices of GL_TRIANGLE_STRIP
    ** vertexOddTStrip is used for odd vertices of GL_TRIANGLE_STRIP
    ** vertex3rdTris is used for the 3rd vertex of GL_TRIANGES
    ** vertex4thQStrip is used for the 4th vertex of GL_QUAD_STRIP
    ** vertex4thQuads is used for the 4th vertex of GL_QUADS
    ** vertexPoly is used for GL_POLYGON
    ** endPoly is used at the end of a GL_POLYGON
    */
    void (*vertex)(__GLcontext *gc, __GLvertex *vx);
    void (*vertexPoints)(__GLcontext *gc, __GLvertex *vx);
    void (*vertexLStrip)(__GLcontext *gc, __GLvertex *vx);
    void (*vertex2ndLines)(__GLcontext *gc, __GLvertex *vx);
    void (*vertex2ndLLoop)(__GLcontext *gc, __GLvertex *vx);
    void (*endLLoop)(__GLcontext *gc);
    void (*vertexTFan)(__GLcontext *gc, __GLvertex *vx);
    void (*vertexEvenTStrip)(__GLcontext *gc, __GLvertex *vx);
    void (*vertexOddTStrip)(__GLcontext *gc, __GLvertex *vx);
    void (*vertex3rdTris)(__GLcontext *gc, __GLvertex *vx);
    void (*vertex4thQStrip)(__GLcontext *gc, __GLvertex *vx);
    void (*vertex4thQuads)(__GLcontext *gc, __GLvertex *vx);
    void (*vertexPoly)(__GLcontext *gc, __GLvertex *vx);
    void (*endPoly)(__GLcontext *gc);

    /*
    ** Vertex validate procedures.  These are dimensionality and mode
    ** sensitive (potentially) procedures that are responsible for
    ** validating the vertex computed values before rendering or
    ** clipping.
    */
    void (*validateVertex[__GL_MAX_VERTEX_SIZE])(__GLcontext *gc, 
						 __GLvertex *v, GLuint needs);

    /*
    ** Process an evaluator coord from the user.
    */
    void (*ec1)(__GLcontext *gc, __GLfloat u);
    void (*ec2)(__GLcontext *gc, __GLfloat u, __GLfloat v);

    /*
    ** Calculate the texture coordinate(s) for a vertex.  calcTexture2
    ** is used when texture coordinate generation is enabled.
    ** calcRasterTexture is used for raster positions (it never scales
    ** and projects texture coordinates).
    */
    void (*calcTexture)(__GLcontext *gc, __GLvertex *v);
    void (*calcTextureUnit[__GL_NUM_TEXTURE_UNITS])(__GLcontext *gc, GLint unit, __GLvertex *v);
    void (*calcTexture2Unit[__GL_NUM_TEXTURE_UNITS])(__GLcontext *gc, GLint unit, __GLvertex *v);
    void (*calcRasterTextureUnit[__GL_NUM_TEXTURE_UNITS])(__GLcontext *gc, GLint unit, __GLvertex *v);

    /**************************************************************/

    /*
    ** Function pointers used for coloring
    */

    /*
    ** applyColor processes a color from the user that has been loaded
    ** into the gc->state.current.color.  If color material is enabled
    ** then the color will be applied the appropriate material(s).
    ** Otherwise, the color will be either scaled or scaled and clamped.
    */
    void (*applyColor)(__GLcontext *gc);

    /*
    ** Calculate the color for a vertex.  calcColor2 is used when
    ** SHADE_CHEAP_FOG is true.  calcRasterColor is used for coloring
    ** raster positions (it never cheap fogs).
    */
    void (*calcColor)(__GLcontext *gc, GLint face, __GLvertex *vx);
    void (*calcColor2)(__GLcontext *gc, GLint face, __GLvertex *vx);
    void (*calcRasterColor)(__GLcontext *gc, GLint face, __GLvertex *vx);

    /*
    ** Perform the color material operation.
    */
    void (*colorMaterial)(__GLcontext *gc, __GLcolor *col);

    /*
    ** Apply a color change to a material.
    */
    void (*changeMaterial)(__GLcontext *gc, __GLmaterialState *ms,
			   __GLmaterialMachine *msm);

    /*
    ** This procedure is called when the material is about to change.
    ** It validates any queued vertices that depend upon the current
    ** material.
    */
    void (*matValidate)(__GLcontext *gc);

    /**************************************************************/

    /*
    ** Z buffer test procs
    */

    /*
    ** assembly routines to depth test a single pixel.  These are 
    ** highly optimized C-callable routines...
    */
    GLboolean (*DTPixel)( __GLzValue z, __GLzValue *zfb );

    /**************************************************************/

    /*
    ** Primitive handling procs.
    */
    void (*beginPrim[10])(__GLcontext *gc);
    void (*endPrim)(__GLcontext *gc);

    /**************************************************************/

    /*
    ** Function pointers used for polygon, triangle, quad and rect
    ** rendering.
    */

    /* Rendering */
    void (*renderTriangle)(__GLcontext *gc, __GLvertex *v0,
			   __GLvertex *v1, __GLvertex *v2);
    void (*renderTriangle2)(__GLcontext *gc, __GLvertex *v0,
			    __GLvertex *v1, __GLvertex *v2);
    void (*fillTriangle)(__GLcontext *gc, __GLvertex *v0,
			 __GLvertex *v1, __GLvertex *v2);
    void (*fillTriangle2)(__GLcontext *gc, __GLvertex *v0,
			  __GLvertex *v1, __GLvertex *v2);

#if defined(__GL_HW_NEEDS_SYNC)
    void (*renderTriangleTrap)(__GLcontext *gc, __GLvertex *v0,
			       __GLvertex *v1, __GLvertex *v2);
    void (*renderTriangleTrapBackup)(__GLcontext *gc, __GLvertex *v0,
				     __GLvertex *v1, __GLvertex *v2);
#endif /* __GL_HW_NEEDS_SYNC */

#ifdef __GL_FR_RAST
    /* Rasterization fastpath procs */
#if defined(__GL_CODEGEN) && defined(i386)
    /* Assembly function for renderTriangle generated by OG code */
    void *renderTriangleCode;

    void (*fillSpans)(void *tr);
#endif
    void (*fillSpanlet)(GLbitfield mask, void *tr);
    void (*extractTexels)(GLbitfield mask, void *tr);
    void (*afterGenMask)(GLbitfield mask, void *tr);
    void (*afterStipple)(GLbitfield mask, void *tr);
    void (*afterDepthTest)(GLbitfield mask, void *tr);
    void (*afterGenColors)(GLbitfield mask, void *tr);
    void (*afterFog)(GLbitfield mask, void *tr);
    void (*afterAlphaTest)(GLbitfield mask, void *tr);
    void (*afterStencilTest)(GLbitfield mask, void *tr);
    void (*stencilOpSFail)(GLbitfield mask, void *tr);
    void (*stencilOpZFail)(GLbitfield mask, void *tr);
    void (*stencilOpZPass)(GLbitfield mask, void *tr);
    void (*afterStencilDepthTest)(GLbitfield mask, void *tr);
    void (*doubleStoreProc)(GLbitfield mask, void *tr);

    /* Texture fallback routines */
    void (*slowMinFilter)(__GLtexture *tex, GLuint lod,
			  GLint fs, GLint ft, __GLtexel *result);
    void (*slowMagFilter)(__GLtexture *tex, GLuint lod,
			  GLint fs, GLint ft, __GLtexel *result);
#endif
    /* Clipping */
    void (*clipTriangle)(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1,
			 __GLvertex *v2, GLuint orCodes);
    void (*clipPolygon)(__GLcontext *gc, __GLvertex *v0, GLint nv);
    void (*polyClipParam)(__GLvertex *result, const __GLvertex *a,
			  const __GLvertex *b, __GLfloat t);

    /* Rect */
    void (*rect)(__GLcontext *gc, __GLfloat x0, __GLfloat y0, __GLfloat x1, 
	    __GLfloat y1);

    /* Function pointers specific to span level rendering */
    __GLspanProcs span;

    /* Function pointers specific to line level rendering */
    __GLlineProcs line;

    /* Function pointers specific to pixel routines (DrawPixels, CopyPixels,
    ** ReadPixels).
    */
    __GLpixelSpanProcs pixel;

    /**************************************************************/

    /*
    ** Function pointers used for lines.
    */
    void (*renderLine)(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);
    void (*renderLine2)(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);

#if defined(__GL_HW_NEEDS_SYNC)
    void (*renderLineTrap)(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);
    void (*renderLineTrapBackup)(__GLcontext *gc, __GLvertex *v0, 
				 __GLvertex *v1);
#endif /* __GL_HW_NEEDS_SYNC */

    /* Perform line clipping */
    void (*clipLine)(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1);

    /* Line specific parameter clip proc */
    void (*lineClipParam)(__GLvertex *result, const __GLvertex *a,
			  const __GLvertex *b, __GLfloat t);

    /* line routine called when GL_POLYGON_MODE is GL_LINE */
    void (*renderTrianglePolygonModeLine)(__GLcontext *gc, __GLvertex *a, 
					  __GLvertex *b, __GLvertex *c);

    /*
    ** The default slow path renderLine proc simply initializes some line
    ** data, and then calls this proc.
    */
    void (*rasterizeLine)(__GLcontext *gc);

    /**************************************************************/

    /*
    ** Point procs.
    */
    void (*renderPoint)(__GLcontext *gc, __GLvertex *v);
    void (*renderPoint2)(__GLcontext *gc, __GLvertex *v, __GLfragment *f);

#if defined(__GL_HW_NEEDS_SYNC)
    void (*renderPointTrap)(__GLcontext *gc, __GLvertex *v);
    void (*renderPointTrapBackup)(__GLcontext *gc, __GLvertex *v);
#endif /* __GL_HW_NEEDS_SYNC */

    /* point routine called when GL_POLYGON_MODE is GL_POINT */
    void (*renderTrianglePolygonModePoint)(__GLcontext *gc, __GLvertex *a,
					   __GLvertex *b, __GLvertex *c);

    /**************************************************************/

    /*
    ** Bitmap procs.
    */
    void (*bitmap)(__GLcontext *gc, GLint width, GLint height,
		   GLfloat xOrig, GLfloat yOrig,
		   GLfloat xMove, GLfloat yMove, const GLubyte bits[]);
    void (*renderBitmap)(__GLcontext *gc, const __GLbitmap *bitmap,
			 const GLubyte *bits);

    /**************************************************************/

    /*
    ** Texturing procs.  The rho procs compute the next rho value
    ** for mipmap selection.  They might be simple procedures if
    ** mipmapping is not being done.
    ** textureRaster is used for Bitmap, DrawPixels and CopyPixels
    ** which use the current raster texture coords.
    */
    __GLfloat (*calcLineRho)(__GLcontext *gc, GLint unit, __GLfloat s,
			     __GLfloat t, __GLfloat r, __GLfloat winv);
    __GLfloat (*calcPolygonRho)(__GLcontext *gc, GLint unit,
    				const __GLshade *sh,
				__GLfloat s, __GLfloat t, __GLfloat r,
				__GLfloat winv);
    void (*texture)(__GLcontext *gc, __GLcolor *frag,
			__GLcoord strq[__GL_NUM_TEXTURE_UNITS],
			__GLfloat rhow[__GL_NUM_TEXTURE_UNITS]);
    void (*textureRaster)(__GLcontext *gc, __GLcolor *frag,
			__GLcoord strq[__GL_NUM_TEXTURE_UNITS],
			__GLfloat rhow[__GL_NUM_TEXTURE_UNITS]);
    void (*textureUnit[__GL_NUM_TEXTURE_UNITS])(__GLcontext *gc, __GLtexture *tex, __GLcolor *frag, __GLfloat s, __GLfloat t, __GLfloat r, __GLfloat rho);

    /**************************************************************/

    /*
    ** Fogging procs.  Vertex fogging computes the fog factor at the
    ** vertex and then interpolates that.  High quality fogging
    ** (GL_FOG_HINT set to GL_NICEST) interpolates the eyeZ at then
    ** evaluates the fog function for each fragment.
    */
    void (*fogPoint)(__GLcontext *gc, __GLfragment *frag, __GLfloat eyeZ);
    void (*fogColor)(__GLcontext *gc, __GLcolor *out, __GLcolor *in, 
	    	     __GLfloat eyeZ);
    __GLfloat (*fogVertex)(__GLcontext *gc, __GLvertex *vx);

    /**************************************************************/

    /*
    ** Blend an incoming fragment according to the current blending
    ** mode and return a pointer to the new fragment which contains
    ** the updated colors.
    */
    void (*blend)(__GLcontext *gc, __GLcolorBuffer *cfb,
		  const __GLfragment *frag, __GLcolor *result);
    void (*blendColor)(__GLcontext *gc, const __GLcolor *source,
		       const __GLcolor *dest, __GLcolor *result);
    void (*blendSrc)(__GLcontext *gc, const __GLcolor *source,
		     const __GLcolor *dest, __GLcolor *result);
    void (*blendDst)(__GLcontext *gc, const __GLcolor *frag,
		     const __GLcolor *dest, __GLcolor *result);
    void (*blendEquation)( __GLcontext *gc, const __GLcolor *frag,
	   	     const __GLcolor *dest, __GLcolor *result);

    /**************************************************************/

    /* Pixel proc pointers */
    void (*drawPixels)(__GLcontext *gc, GLint width, GLint height,
		       GLenum format, GLenum type, const GLvoid *pixels,
		       GLboolean packed);
    void (*copyPixels)(__GLcontext *gc, GLint x, GLint y,
		       GLsizei width, GLsizei height, GLenum type);
    void (*readPixels)(__GLcontext *gc, GLint x, GLint y,
		       GLsizei width, GLsizei height,
		       GLenum format, GLenum type, GLvoid *pixels);
    void (*copyImage)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo);
    void (*storeTex) (__GLcontext *gc, __GLmipMapLevel *lp,
                      __GLpixelSpanInfo *spanInfo);
    void (*fetchTex) (__GLcontext *gc, __GLmipMapLevel *lp,
                      __GLpixelSpanInfo *spanInfo);
    void (*readImage)(__GLcontext *gc, __GLmipMapLevel *lp,
                      __GLpixelSpanInfo *spanInfo);
    void (*pxStore)(__GLcolorBuffer *cfb, const __GLfragment *frag);

    /**************************************************************/

    /*
    ** Store a fragment into the given frame buffer, applying any
    ** currently active rasterization modes properly.
    */
    void (*store)(__GLcolorBuffer *cfb, const __GLfragment *frag);

    /*
    ** Store a fragment into the given frame buffer.
    */
    void (*cfbStore)(__GLcolorBuffer *cfb, const __GLfragment *frag);

    /**************************************************************/

    /*
    ** The RasterPos API points.
    */
    void (*rasterPos2)(__GLcontext *gc, const __GLfloat v[2]);
    void (*rasterPos3)(__GLcontext *gc, const __GLfloat v[3]);
    void (*rasterPos4)(__GLcontext *gc, const __GLfloat v[4]);

    /**************************************************************/

    /*
    ** Memory manager to make display list compilation fast.
    */
    __GLmemoryManagerProcs memory;

    /**************************************************************/

    /*
    ** Display list customizing routines
    */
    
    __GLdlistControlProcs dlist;


    /**************************************************************/



    /*
    ** Function pointers used for attribute processing.
    */

    /* called when the polygon stipple changes */
    void (*convertPolygonStipple)(__GLcontext *gc);
};

extern void __glGenericValidate(__GLcontext *gc);


/* Optimized implementations of pick procs */
extern void __glOptPickRenderBitmapProcs(__GLcontext *gc);


/* Generic (portable) implementations of the pick procs */
extern void __glGenericPickBlendProcs(__GLcontext *gc);
extern void __glGenericPickColorMaterialProcs(__GLcontext *gc);
extern void __glGenericPickTextureProcs(__GLcontext *gc);
extern void __glGenericPickCalcTextureProcs(__GLcontext *gc);
extern void __glGenericPickFogProcs(__GLcontext *gc);
extern void __glGenericPickTransformProcs(__GLcontext *gc);
extern void __glGenericPickParameterClipProcs(__GLcontext *gc);
extern void __glGenericPickPointProcs(__GLcontext *gc);
extern void __glGenericPickTriangleProcs(__GLcontext *gc);
extern void __glGenericPickLineProcs(__GLcontext *gc);
extern void __glGenericPickRenderBitmapProcs(__GLcontext *gc);
extern void __glGenericPickClipProcs(__GLcontext *gc);
extern void __glGenericPickBufferProcs(__GLcontext *gc);
extern void __glGenericPickStoreProcs(__GLcontext *gc);
extern void __glGenericPickSpanProcs(__GLcontext *gc);
extern void __glGenericPickVertexProcs(__GLcontext *gc);
extern void __glGenericPickVertexArrayEnables(__GLcontext *gc);
extern void __glGenericPickVertexArrayProcs(__GLcontext *gc);
extern void __glGenericPickPixelProcs(__GLcontext *gc);
extern void __glGenericPickTextureEnables(__GLcontext *gc);
extern GLint __glGenericPickDepthProcs(__GLcontext *gc);
extern GLboolean __glGenericPickAllProcsPhase1(__GLcontext *gc);
extern void __glGenericPickAllProcsPhase2(__GLcontext *gc);

extern void __glGenericDlistOptimizer(__GLcontext *gc, __GLcompiledDlist *);
extern __GLdlist *__glGenericDlistCompiler(__GLcontext *gc,__GLcompiledDlist *);

/* some useful store procs */
extern void __glDoStore_ASD(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoStore_AS(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoStore_AD(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoStore_SD(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoStore_A(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoStore_S(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoStore_D(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoStore(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoNullStore(__GLcolorBuffer *, const __GLfragment *);
extern void __glDoDoubleStore(__GLcolorBuffer *, const __GLfragment *);

/* Save routines */
extern void __glSaveN(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveC(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveCI(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveT(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveCT(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveNT(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveCIT(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveCAll(__GLcontext *gc, __GLvertex *vx);
extern void __glSaveCIAll(__GLcontext *gc, __GLvertex *vx);


/* Primitive assemblers */
extern void __gl1stLinesVertex(__GLcontext *gc, __GLvertex *vx);
extern void __gl2ndLinesVertex(__GLcontext *gc, __GLvertex *vx);
extern void __gl2ndLinesVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __gl1stLStripVertex(__GLcontext *gc, __GLvertex *vx);
extern void __glOtherLStripVertex(__GLcontext *gc, __GLvertex *vx);
extern void __glOtherLStripVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __glOtherLStripVertexFast(__GLcontext *gc, __GLvertex *vx);
extern void __glOtherLStripVertexFastLock(__GLcontext *gc, __GLvertex *vx);
extern void __gl2ndLLoopVertex(__GLcontext *gc, __GLvertex *vx);
extern void __gl2ndLLoopVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __glEndLLoop(__GLcontext *gc);
extern void __glEndLLoopLock(__GLcontext *gc);
extern void __glOtherTFanVertex(__GLcontext *gc, __GLvertex *vx);
extern void __glOtherTFanVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __glEvenTStripVertex(__GLcontext *gc, __GLvertex *vx);
extern void __glEvenTStripVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __glOddTStripVertex(__GLcontext *gc, __GLvertex *vx);
extern void __glOddTStripVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __glEndTStrip(__GLcontext *gc);
extern void __gl3rdTrianglesVertex(__GLcontext *gc, __GLvertex *vx);
extern void __gl3rdTrianglesVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __gl4thQStripVertex(__GLcontext *gc, __GLvertex *vx);
extern void __gl4thQStripVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __gl4thQuadsVertex(__GLcontext *gc, __GLvertex *vx);
extern void __gl4thQuadsVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __glEndQStrip(__GLcontext *gc);
extern void __glPolygonVertex(__GLcontext *gc, __GLvertex *vx);
extern void __glPolygonVertexLock(__GLcontext *gc, __GLvertex *vx);
extern void __glEndPolygon(__GLcontext *gc);
extern void __glEndPolygonLock(__GLcontext *gc);

/* Memory manager */
extern __GLarena *__glNewArena(__GLcontext *gc);
extern void __glDeleteArena(__GLarena *arena);
extern void *__glArenaAlloc(__GLarena *arena, unsigned int size);
extern void __glArenaFreeAll(__GLarena *arena);

/*
** Material validation routines used during primitive assembly if the
** user changes the current material.
*/
extern void __glMatValidateCacheVbuf0N(__GLcontext *gc);
extern void __glMatValidateVbuf0N(__GLcontext *gc);
extern void __glMatValidateVbuf0V1(__GLcontext *gc);
extern void __glMatValidateV1(__GLcontext *gc);
extern void __glMatValidateV1V2(__GLcontext *gc);
extern void __glMatValidateV1V2V3(__GLcontext *gc);
extern void __glMatValidateV2V3(__GLcontext *gc);
extern void __glMatValidateVbuf0(__GLcontext *gc);

#endif /* __gl_procs_h_ */
