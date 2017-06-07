#ifndef __gl_vertex_h_
#define __gl_vertex_h_
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
** $Header: //depot/main/gfx/lib/opengl/include/vertex.h#14 $
*/
#include "types.h"

/*
** Vertex structure.  Each vertex contains enough state to properly
** render the active primitive.
*/
struct __GLvertexRec {
    /*
    ** Keep this data structure aligned: have all vectors start on 
    ** 4-word boundary, and sizeof this struct should be kept at 
    ** a multiple of 4 words. Also helps to bunch together most
    ** frequently used items, helps cache.
    */

    /*
    ** Coordinates straight from client. These fields may not be
    ** set depending on the active modes.  The normal and texture
    ** coordinate are used by lighting and texturing.  These cells
    ** may be overwritten by the eyeNormal and the generated texture
    ** coordinate, depending on the active modes.
    */
    __GLcoord obj;
    __GLcoord normal;

    /*
    ** Projected clip coordinate.  This field is filled in when the users
    ** object coordinate has been multiplied by the combined modelview
    ** and projection matrix.
    */
    __GLcoord clip;

    /*
    ** This contains lots of different kind of flags:
    **   has: Which members of the vertex are valid
    **   general: General flags for the vertex (like edge flags, etc.)
    **   vertexsize: 2/3/4D vertex
    **   clipcodes: for frustum and user clipplane clipping.
    */
    GLuint flagBits;

    /*
    ** Pointer to current face color in use. Additionally, when we
    ** need to access the secondary face color, dereference the address
    ** (color + 2), which will point to the respective secondary face
    ** color.  Currently, the secondary color is only accessed for use
    ** with separate specular color.
    */
    __GLcolor *color;

    /*
    ** For value for this vertex.  Used only for cheap fogging.
    */
    __GLfloat fog;

    GLuint pad0;

    /*
    ** Window and eye coordinate. This field is filled in when the window 
    ** clip coordinate is converted to a drawing surface relative "window" 
    ** coordinate.
    ** NOTE: the window.w coordinate contains 1/clip.w.
    */
    __GLcoord window;
    __GLcoord eye;

    /*
    ** Coordinates straight from client. These fields may not be
    ** set depending on the active modes.  The normal and texture
    ** coordinate are used by lighting and texturing.  These cells
    ** may be overwritten by the eyeNormal and the generated texture
    ** coordinate, depending on the active modes.
    */
    __GLcoord texture[__GL_NUM_TEXTURE_UNITS];

    /*
    ** Colors.  colors[0] is the "front" color, colors[1] is the "back" color.
    ** The color pointer points to which color is current for this
    ** vertex.  Verticies can have more than one color when two sided
    ** lighting is enabled. (note color pointer moved up top).
    */
    __GLcolor colors[4];
};

/*
** used by __glCalc2DOutcode for window rect clipping
*/
typedef struct __GLclipVertexRec {
    __GLvertex v;
    GLuint outcode;
} __GLclipVertex;

/* Indicies for colors[] array in vertex */
#define __GL_FRONTFACE		0
#define __GL_BACKFACE		1
#define __GL_FRONTFACE_SPECULAR	2
#define __GL_BACKFACE_SPECULAR	3

/*
** Definitions of bits for flags
*/

/* Bits for has */
#define __GL_HAS_SHIFT		0
#define	__GL_HAS_FRONT_COLOR	(0x0001 << __GL_HAS_SHIFT)
#define __GL_HAS_BACK_COLOR	(0x0002 << __GL_HAS_SHIFT)
#define	__GL_HAS_TEXTURE	(0x0004 << __GL_HAS_SHIFT)
#define __GL_HAS_NORMAL		(0x0008 << __GL_HAS_SHIFT)
#define __GL_HAS_EYE		(0x0010 << __GL_HAS_SHIFT)
#define __GL_HAS_CLIP		(0x0020 << __GL_HAS_SHIFT)
#define __GL_HAS_FOG		(0x0040 << __GL_HAS_SHIFT)
#define __GL_HAS_WINDOW		(0x0080 << __GL_HAS_SHIFT)
#define __GL_HAS_MASK		(0x00ff << __GL_HAS_SHIFT)

#define __GL_HAS_BOTH		(__GL_HAS_FRONT_COLOR | __GL_HAS_BACK_COLOR)
#define __GL_HAS_LIGHTING	(__GL_HAS_EYE | __GL_HAS_NORMAL)

#define __GL_NEEDS_RASTERPOS	(__GL_HAS_FRONT_COLOR | \
				 __GL_HAS_TEXTURE | \
				 __GL_HAS_NORMAL | \
				 __GL_HAS_EYE)

/* Bits for general */
#define __GL_VERTEX_GENERAL_SHIFT	12
#define __GL_VERTEX_EDGE_FLAG		(0x1 << __GL_VERTEX_GENERAL_SHIFT)
#define __GL_VERTEX_CULL_FLAG		(0x2 << __GL_VERTEX_GENERAL_SHIFT)
#define __GL_VERTEX_GENERAL_MASK	(0x3 << __GL_VERTEX_GENERAL_SHIFT)

/* Bits for vertex sizes (2,3,4 D) */
#define __GL_VERTEX_SIZE_SHIFT	14
#define __GL_VERTEX_0D_INDEX	0x0
#define __GL_VERTEX_0D_SIZE	(__GL_VERTEX_0D_INDEX << __GL_VERTEX_SIZE_SHIFT)
#define __GL_VERTEX_2D_INDEX	0x1
#define __GL_VERTEX_2D_SIZE	(__GL_VERTEX_2D_INDEX << __GL_VERTEX_SIZE_SHIFT)
#define __GL_VERTEX_3D_INDEX	0x2
#define __GL_VERTEX_3D_SIZE	(__GL_VERTEX_3D_INDEX << __GL_VERTEX_SIZE_SHIFT)
#define __GL_VERTEX_4D_INDEX	0x3
#define __GL_VERTEX_4D_SIZE	(__GL_VERTEX_4D_INDEX << __GL_VERTEX_SIZE_SHIFT)
#define __GL_VERTEX_SIZE_MASK	(0x3 << __GL_VERTEX_SIZE_SHIFT)

#define __GL_MAX_VERTEX_SIZE	4

#define DO_VALIDATE(g,v,n) \
     (*gc->procs.validateVertex[(v->flagBits & __GL_VERTEX_SIZE_MASK) >> __GL_VERTEX_SIZE_SHIFT])(g,v,n)

/* Bits for clip codes (view frustum and user defined) */
#define __GL_CLIP_SHIFT		16
#define __GL_CLIP_LEFT		(0x0001 << __GL_CLIP_SHIFT)
#define __GL_CLIP_RIGHT		(0x0002 << __GL_CLIP_SHIFT)
#define __GL_CLIP_BOTTOM	(0x0004 << __GL_CLIP_SHIFT)
#define __GL_CLIP_TOP		(0x0008 << __GL_CLIP_SHIFT)
#define __GL_CLIP_NEAR		(0x0010 << __GL_CLIP_SHIFT)
#define __GL_CLIP_FAR		(0x0020 << __GL_CLIP_SHIFT)
#define __GL_CLIP_FRUSTUM_MASK	(0x003f << __GL_CLIP_SHIFT)
#define __GL_CLIP_USER0		(0x0040 << __GL_CLIP_SHIFT)
#define __GL_CLIP_USER_MASK	(0xffc0 << __GL_CLIP_SHIFT)
#define __GL_CLIP_MASK		(__GL_CLIP_USER_MASK | __GL_CLIP_FRUSTUM_MASK)

#define __GL_MAX_USER_CLIP_PLANES 10

#define __GL_CLIP_ALL_MASK	(__GL_CLIP_MASK | __GL_VERTEX_CULL_FLAG)

/************************************************************************/

/*
** Total number of clipping planes supported by this gl.  This includes
** the frustum's six clipping planes.
*/
#define	__GL_MAX_CLIP_PLANES	(6 + __GL_MAX_USER_CLIP_PLANES)

/*
** Number of static verticies in the context.  Polygon's larger than
** this number will be decomposed.
*/
#define __GL_NVBUF 16

/*
** State for managing the vertex machinery.
*/
typedef struct __GLvertexMachineRec {
    /*
    ** Vertex pointers. v0 always points to the next slot in vbuf to
    ** be filled in when a new vertex arrives.  v1, v2 and v3 are
    ** used by the per-primitive vertex handlers.
    */
    __GLvertex *v0;
    __GLvertex *v1;
    __GLvertex *v2;
    __GLvertex *v3;
    __GLvertex vbuf[__GL_NVBUF];

    /*
    ** Provoking vertex.  For flat shaded primitives the triangle
    ** renderer needs to know which vertex provoked the primitive to
    ** properly assign the color during scan conversion.  This is kept
    ** around as its a big pain to remember which vertex was provoking
    ** during clipping (and to keep its parameters right).
    */
    __GLvertex *provoking;

    /*
    ** needs is a bit field that keeps track of what kind of information
    ** is needed in the verticies.  See the vertex->has bits define for
    ** the definition of the bits used here.
    **
    ** frontNeeds is what the front faces need, and backNeeds is what
    ** the back faces need.
    */
    GLuint needs;

    /*
    ** frontNeeds and backNeeds are the needs
    */
    GLuint faceNeeds[2];

    /*
    ** materialNeeds is a bit field indicating what kind of information is
    ** needed in the vertices if the material is going to change.
    */
    GLuint materialNeeds;
} __GLvertexMachine;

/************************************************************************/

extern GLuint __glComputeUserClipCodes(__GLcontext *gc, __GLvertex *vx,
					GLuint clipPlanesMask);

GLuint __glClipCheckFrustum(__GLcontext *gc, __GLvertex *vx);
GLuint __glClipCheckFrustum2D(__GLcontext *gc, __GLvertex *vx);
GLuint __glClipCheckAll(__GLcontext *gc, __GLvertex *vx);
GLuint __glClipCheckAll2(__GLcontext *gc, __GLvertex *vx);
GLuint __glClipCheckAll3(__GLcontext *gc, __GLvertex *vx);
GLuint __glClipCheckAll4(__GLcontext *gc, __GLvertex *vx);

GLuint __glClipCheck2D_2(__GLcontext *gc, __GLvertex *vx);
GLuint __glClipCheck2D_3(__GLcontext *gc, __GLvertex *vx);

void __glRasterPos2(__GLcontext *gc, const __GLfloat v[2]);
void __glRasterPos3(__GLcontext *gc, const __GLfloat v[3]);
void __glRasterPos4(__GLcontext *gc, const __GLfloat v[4]);

void __glEndPrim(__GLcontext *gc);

void __glValidateVertex2(__GLcontext *gc, __GLvertex *vx, GLuint needs);
void __glValidateVertex3(__GLcontext *gc, __GLvertex *vx, GLuint needs);
void __glValidateVertex4(__GLcontext *gc, __GLvertex *vx, GLuint needs);
void __glValidateVertex2Clip(__GLcontext *gc, __GLvertex *vx, GLuint needs);
void __glValidateVertex3Clip(__GLcontext *gc, __GLvertex *vx, GLuint needs);
void __glValidateVertex4Clip(__GLcontext *gc, __GLvertex *vx, GLuint needs);

void __glim_NoXFVertex2fv(const GLfloat v[2]);
void __glim_NoXFVertex3fv(const GLfloat v[3]);
void __glim_NoXFVertex4fv(const GLfloat v[4]);

#endif /* __gl_vertex_h_ */
