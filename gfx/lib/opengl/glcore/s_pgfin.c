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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_pgfin.c#4 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"
#include "pixel.h"

/*
** Material validation routines used to set up matValidate in the methods
** structure.  
*/

/*
** Validate vertexes vbuf[0] through v0 (exclusive of v0)
*/
void __glMatValidateCacheVbuf0N(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    for (v = &gc->vertex.vbuf[0]; v < gc->vertex.v0; v++) {
	if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
    }
}

/*
** Validate vertexes vbuf[0] through v0 (exclusive of v0)
*/
void __glMatValidateVbuf0N(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    for (v = &gc->vertex.vbuf[0]; v < gc->vertex.v0; v++) {
	if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
    }
}

/*
** Validate vbuf[0] and v1.
*/
void __glMatValidateVbuf0V1(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    v = &gc->vertex.vbuf[0];
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);

    v = gc->vertex.v1;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
}

/* 
** Validate v1 only.
*/
void __glMatValidateV1(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    v = gc->vertex.v1;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
}

/* 
** Validate v1 and v2.
*/
void __glMatValidateV1V2(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    v = gc->vertex.v1;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);

    v = gc->vertex.v2;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
}

/* 
** Validate v1, v2 and v3.
*/
void __glMatValidateV1V2V3(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    v = gc->vertex.v1;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);

    v = gc->vertex.v2;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);

    v = gc->vertex.v3;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
}

/* 
** Validate v2 and v3.
*/
void __glMatValidateV2V3(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;

    v = gc->vertex.v2;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);

    v = gc->vertex.v3;
    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
}

#ifdef NOTUSED
/*
** Validate vbuf[0] only.
*/
void __glMatValidateVbuf0(__GLcontext *gc)
{
    __GLvertex *v;
    GLuint needs;

    needs = gc->vertex.materialNeeds;
    v = &gc->vertex.vbuf[0];

    if (~v->flagBits & needs) DO_VALIDATE(gc, v, needs);
}
#endif

/************************************************************************/

/*
** Triangle fan code.  The triangle fan machinery keeps the initial vertex
** in gc->vertex.vbuf[0].  As each vertex comes in, the gc->vertex.v0 and
** gc->vertex.v1 pointers are exchanged so that the next new vertex will
** overwrite the previous previous vertex.  For example, if five verticies
** are input (named A-E) then this is where the various pointers will point
** when the finish proc is called:
**
**	input	v0	v1	new-v0	new-v1	result
**	-----	--	--	------	------	------
**	begin	vbuf[0]	n/a
**	A	A	n/a	vbuf[1]	n/a	none
**	B	B	n/a	vbuf[2]	B	none
**	C	C	B	B	C	draw CAB
**	D	D	C	C	D	draw DAC
**	E	E	D	D	E	draw EAD
*/

void __glOtherTFanVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2;
    GLuint orCodes, andCodes;

    v1 = gc->vertex.v1;
    v2 = &gc->vertex.vbuf[0];

    /* Setup to render this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Setup for next triangle */
    gc->vertex.v1 = v0;
    gc->vertex.v0 = v1;

    /* Clip check */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits) & __GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & v1->flagBits & v2->flagBits & __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle */
	    __GL_LOCK_RENDER_BUFFERS(gc);
	    (*gc->procs.clipTriangle)(gc, v0, v2, v1, orCodes);
	    __GL_UNLOCK_RENDER_BUFFERS(gc);
	}
    } else {
	/* Render the triangle */
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.renderTriangle)(gc, v0, v2, v1);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __glOtherTFanVertex(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2;
    GLuint orCodes, andCodes;

    v1 = gc->vertex.v1;
    v2 = &gc->vertex.vbuf[0];

    /* Setup to render this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Setup for next triangle */
    gc->vertex.v1 = v0;
    gc->vertex.v0 = v1;

    /* Clip check */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits) & __GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & v1->flagBits & v2->flagBits & __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle */
	    (*gc->procs.clipTriangle)(gc, v0, v2, v1, orCodes);
	}
    } else {
	/* Render the triangle */
	(*gc->procs.renderTriangle)(gc, v0, v2, v1);
    }
}

static void SecondTFanVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v1 = v0;
    gc->procs.vertex = gc->procs.vertexTFan;
    gc->procs.matValidate = __glMatValidateVbuf0V1;
}

static void FirstTFanVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = SecondTFanVertex;
}

void __glBeginTFan(__GLcontext *gc)
{
    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = FirstTFanVertex;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}

/************************************************************************/

/*
** Triangle strip vertex machinery.  The following table shows what
** happens to the gc->vertex.v0, gc->vertex.v1 and gc->vertex.v2 pointers
** as each vertex is received.
**
**	input	v0	v1	v2	new-v0	new-v1	new-v2	result
**	-----	--	--	--	------	------	------	------
**	begin	vbuf[0]	n/a	n/a
**	A	A	n/a	n/a	vbuf[1]	n/a	A	n/a
**	B	B	n/a	A	vbuf[2]	B	A	n/a
**	C	C	B	A	A	C	B	draw CAB
**	D	D	C	B	B	D	C	draw DCB
**	E	E	D	C	C	E	D	draw ECD
*/

void __glEvenTStripVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2;
    GLuint orCodes, andCodes;

    v1 = gc->vertex.v1;
    v2 = gc->vertex.v2;

    /* setup for rendering this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Clip check & setup for next triangle */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits) & __GL_CLIP_ALL_MASK;
    gc->vertex.v0 = v2;
    gc->vertex.v1 = v0;
    gc->vertex.v2 = v1;
    gc->procs.vertex = gc->procs.vertexOddTStrip;

    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & v1->flagBits & v2->flagBits & __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle (NOTE: v0, v1, v2) */
	    __GL_LOCK_RENDER_BUFFERS(gc);
	    (*gc->procs.clipTriangle)(gc, v0, v1, v2, orCodes);
	    __GL_UNLOCK_RENDER_BUFFERS(gc);
	}
    } else {
	/* Render the triangle (NOTE: v0, v1, v2) */
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.renderTriangle)(gc, v0, v1, v2);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __glEvenTStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2;
    GLuint orCodes, andCodes;

    v1 = gc->vertex.v1;
    v2 = gc->vertex.v2;

    /* setup for rendering this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Clip check & setup for next triangle */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits) & __GL_CLIP_ALL_MASK;
    gc->vertex.v0 = v2;
    gc->vertex.v1 = v0;
    gc->vertex.v2 = v1;
    gc->procs.vertex = gc->procs.vertexOddTStrip;

    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & v1->flagBits & v2->flagBits & __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle (NOTE: v0, v1, v2) */
	    (*gc->procs.clipTriangle)(gc, v0, v1, v2, orCodes);
	}
    } else {
	/* Render the triangle (NOTE: v0, v1, v2) */
	(*gc->procs.renderTriangle)(gc, v0, v1, v2);
    }
}

void __glOddTStripVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2;
    GLuint orCodes, andCodes;

    v1 = gc->vertex.v1;
    v2 = gc->vertex.v2;

    /* setup for rendering this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Clip check & setup for next triangle */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits) & __GL_CLIP_ALL_MASK;
    gc->vertex.v0 = v2;
    gc->vertex.v1 = v0;
    gc->vertex.v2 = v1;
    gc->procs.vertex = gc->procs.vertexEvenTStrip;

    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & v1->flagBits & v2->flagBits & __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle (NOTE: v0, v2, v1) */
	    __GL_LOCK_RENDER_BUFFERS(gc);
	    (*gc->procs.clipTriangle)(gc, v0, v2, v1, orCodes);
	    __GL_UNLOCK_RENDER_BUFFERS(gc);
	}
    } else {
	/* Render the triangle (NOTE: v0, v2, v1) */
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.renderTriangle)(gc, v0, v2, v1);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __glOddTStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2;
    GLuint orCodes, andCodes;

    v1 = gc->vertex.v1;
    v2 = gc->vertex.v2;

    /* setup for rendering this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Clip check & setup for next triangle */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits) & __GL_CLIP_ALL_MASK;
    gc->vertex.v0 = v2;
    gc->vertex.v1 = v0;
    gc->vertex.v2 = v1;
    gc->procs.vertex = gc->procs.vertexEvenTStrip;

    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & v1->flagBits & v2->flagBits & __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle (NOTE: v0, v2, v1) */
	    (*gc->procs.clipTriangle)(gc, v0, v2, v1, orCodes);
	}
    } else {
	/* Render the triangle (NOTE: v0, v2, v1) */
	(*gc->procs.renderTriangle)(gc, v0, v2, v1);
    }
}

static void SecondTStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v1 = v0;
    gc->procs.vertex = gc->procs.vertexOddTStrip;
    gc->procs.matValidate = __glMatValidateV1V2;
}

static void FirstTStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v2 = v0;
    gc->procs.vertex = SecondTStripVertex;
}

void __glEndTStrip(__GLcontext *gc)
{
    /* __GLvertex *v0 = gc->vertex.v0; */
    /* __GLvertex *vFirst = &gc->vertex.vbuf[0]; */

    gc->procs.endPrim = __glEndPrim;
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
}

void __glBeginTStrip(__GLcontext *gc)
{
    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = FirstTStripVertex;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}

/************************************************************************/

/*
** Separate triangle vertex machinery.  This is the simplest machine in
** that all that needs doing is to advance the finish proc each time
** until the a third vertex is received.  After the third vertex is
** received, a triangle is emitted to the clipper and the finish proc is
** reset to the beginning.
*/

/* forward declaration */
static void FirstTrianglesVertex(__GLcontext*, __GLvertex*);

void __gl3rdTrianglesVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *vbuf0 = &gc->vertex.vbuf[0];
    __GLvertex *vbuf1 = &gc->vertex.vbuf[1];
    GLuint orCodes, andCodes;

    /* Setup for this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= gc->state.current.edgeTag;
    gc->vertex.provoking = v0;

    /* Setup for the next triangle */
    gc->vertex.v0 = vbuf0;
    gc->procs.vertex = FirstTrianglesVertex;

    /* Clip check */
    orCodes = (v0->flagBits | vbuf0->flagBits | vbuf1->flagBits) & __GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = (v0->flagBits & vbuf0->flagBits & vbuf1->flagBits) & 
	    __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle */
	    __GL_LOCK_RENDER_BUFFERS(gc);
	    (*gc->procs.clipTriangle)(gc, v0, vbuf0, vbuf1, orCodes);
	    __GL_UNLOCK_RENDER_BUFFERS(gc);
	}
    } else {
	/* Render the triangle */
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.renderTriangle)(gc, v0, vbuf0, vbuf1);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __gl3rdTrianglesVertex(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *vbuf0 = &gc->vertex.vbuf[0];
    __GLvertex *vbuf1 = &gc->vertex.vbuf[1];
    GLuint orCodes, andCodes;

    /* Setup for this triangle */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= gc->state.current.edgeTag;
    gc->vertex.provoking = v0;

    /* Setup for the next triangle */
    gc->vertex.v0 = vbuf0;
    gc->procs.vertex = FirstTrianglesVertex;

    /* Clip check */
    orCodes = (v0->flagBits | vbuf0->flagBits | vbuf1->flagBits) & __GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = (v0->flagBits & vbuf0->flagBits & vbuf1->flagBits) & 
	    __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the triangle */
	} else {
	    /* Clip the triangle */
	    (*gc->procs.clipTriangle)(gc, v0, vbuf0, vbuf1, orCodes);
	}
    } else {
	/* Render the triangle */
	(*gc->procs.renderTriangle)(gc, v0, vbuf0, vbuf1);
    }
}

static void SecondTrianglesVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= gc->state.current.edgeTag;
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = gc->procs.vertex3rdTris;
}

static void FirstTrianglesVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= gc->state.current.edgeTag;
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = SecondTrianglesVertex;
}

void __glBeginTriangles(__GLcontext *gc)
{
    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = FirstTrianglesVertex;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}

/************************************************************************/

/*
** Quad strip vertex machinery.
**
** In the table below, headings v0 through v3 define what the pointers
** v0-v3 point to before the finish proc is executed (and after the
** vertex has been set by the users glVertex call).  v0' through v3'
** define what the pointers point to after the finish proc has executed.
** Result indicates what the consequences of the finish proc were.  The
** symbols used for a vertex column indicate which of the users verticies
** (A through H) are pointed to by the vertex pointer, and which buffer
** cell is being used.  For example, A0 means that buffer cell zero holds
** the A vertex.  The symbol "-2" means that the vertex points to buffer
** cell two, but that it holds no meaningful user vertex.
**
** The table shows the pointer transitions for 10 verticies which emit
** four quads (or eight triangles).
**
** input v0  v1  v2  v3    v0'  v1'  v2'  v3'   result
** ----- --  --  --  --    ---  ---  ---  ---   ------
** begin --  --  --  --    -0   --   --   --
** A     A0  --  --  --    -1   --   --   A0
** B     B1  --  --  A0    -2   --   B1   A0
** C     C2  --  B1  A0    -3   C2   B1   A0
** D     D3  C2  B1  A0    -0   C2   D3   C2    draw ABC, CBD
** E     E0  C2  D3  C2    -1   E0   D3   C2
** F     F1  E0  D3  C2    -2   E0   F1   E0    draw CDE, EDF
** G     G2  E0  F1  E0    -3   G2   F1   E0
** H     H3  G2  F1  E0    -0   G2   H3   G2    draw EFG, GFH
** I     I0  G2  H3  G2    -1   I0   H3   G2
** J     J1  I0  H3  G2    -2   I0   J1   I0    draw GHI, IHJ
*/

/* forward declaration */
static void ThirdQStripVertex(__GLcontext*, __GLvertex*);

void __gl4thQStripVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2, *v3;
    GLuint orCodes, andCodes;
    __GLvertex *iv[4];

    v1 = gc->vertex.v1;
    v2 = gc->vertex.v2;
    v3 = gc->vertex.v3;

    /* Setup for this quad */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Setup for next quad */
    gc->vertex.v0 = v3;
    gc->vertex.v2 = v0;
    gc->vertex.v3 = v1;
    gc->procs.vertex = ThirdQStripVertex;
    gc->procs.matValidate = __glMatValidateV2V3;

    /* Clip Check */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits | v3->flagBits) &
	__GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & 
	    v1->flagBits & 
	    v2->flagBits & 
	    v3->flagBits & 
	    __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the quad */
	} else {
	    /* Clip the quad as a polygon */
	    iv[0] = v3;
	    iv[1] = v2;
	    iv[2] = v0;
	    iv[3] = v1;
	    __GL_LOCK_RENDER_BUFFERS(gc);
	    __glDoPolygonClip(gc, &iv[0], 4, orCodes);
	    __GL_UNLOCK_RENDER_BUFFERS(gc);
	}
    } else {
	/* Render the quad as two triangles */
	__GL_LOCK_RENDER_BUFFERS(gc);
	v2->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v1, v3, v2);
	v1->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	v2->flagBits |= __GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v1, v2, v0);
	v1->flagBits |= __GL_VERTEX_EDGE_FLAG;
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __gl4thQStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1, *v2, *v3;
    GLuint orCodes, andCodes;
    __GLvertex *iv[4];

    v1 = gc->vertex.v1;
    v2 = gc->vertex.v2;
    v3 = gc->vertex.v3;

    /* Setup for this quad */
    gc->line.notResetStipple = GL_FALSE;
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.provoking = v0;

    /* Setup for next quad */
    gc->vertex.v0 = v3;
    gc->vertex.v2 = v0;
    gc->vertex.v3 = v1;
    gc->procs.vertex = ThirdQStripVertex;
    gc->procs.matValidate = __glMatValidateV2V3;

    /* Clip Check */
    orCodes = (v0->flagBits | v1->flagBits | v2->flagBits | v3->flagBits) &
	__GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & 
	    v1->flagBits & 
	    v2->flagBits & 
	    v3->flagBits & 
	    __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the quad */
	} else {
	    /* Clip the quad as a polygon */
	    iv[0] = v3;
	    iv[1] = v2;
	    iv[2] = v0;
	    iv[3] = v1;
	    __glDoPolygonClip(gc, &iv[0], 4, orCodes);
	}
    } else {
	/* Render the quad as two triangles */
	v2->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v1, v3, v2);
	v1->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	v2->flagBits |= __GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v1, v2, v0);
	v1->flagBits |= __GL_VERTEX_EDGE_FLAG;
    }
}

static void ThirdQStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v1 = v0;
    gc->procs.vertex = gc->procs.vertex4thQStrip;
    gc->procs.matValidate = __glMatValidateV1V2V3;
}

static void SecondQStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v2 = v0;
    gc->procs.vertex = ThirdQStripVertex;
}

static void FirstQStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= __GL_VERTEX_EDGE_FLAG;
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v3 = v0;
    gc->procs.vertex = SecondQStripVertex;
}

void __glEndQStrip(__GLcontext *gc)
{
    /* __GLvertex *v0 = gc->vertex.v0; */
    /* __GLvertex *vFirst = &gc->vertex.vbuf[0]; */

    gc->procs.endPrim = __glEndPrim;
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
}

void __glBeginQStrip(__GLcontext *gc)
{
    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = FirstQStripVertex;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}

/************************************************************************/

/*
** Separate quad vertex machinery.  This is the simplest machine in that
** all that needs doing is to advance the finish proc each time until the
** a fourth vertex is received.  After the fourth vertex is received, two
** triangles are emitted to the clipper and the finish proc is reset to
** the beginning.
*/

/* forward declaration */
static void FirstQuadsVertex(__GLcontext*, __GLvertex*);

void __gl4thQuadsVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *va, *vb, *vc;
    GLuint orCodes, andCodes;
    GLuint saveFlag;
    __GLvertex *iv[4];

    va = &gc->vertex.vbuf[0];
    vb = &gc->vertex.vbuf[1];
    vc = &gc->vertex.vbuf[2];

    /* Setup for this triangle */
    v0->flagBits |= gc->state.current.edgeTag;
    gc->line.notResetStipple = GL_FALSE;
    gc->vertex.provoking = v0;

    /* Setup for next quad */
    gc->vertex.v0 = va;
    gc->procs.vertex = FirstQuadsVertex;

    /* Clip Check */
    orCodes = (v0->flagBits | va->flagBits | vb->flagBits | vc->flagBits) & 
	__GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & va->flagBits & vb->flagBits & vc->flagBits & 
	    __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the quad */
	} else {
	    /* Clip the quad as a polygon */
	    iv[0] = va;
	    iv[1] = vb;
	    iv[2] = vc;
	    iv[3] = v0;
	    __GL_LOCK_RENDER_BUFFERS(gc);
	    __glDoPolygonClip(gc, &iv[0], 4, orCodes);
	    __GL_UNLOCK_RENDER_BUFFERS(gc);
	}
    } else {
	/* Render the quad as two triangles */
	__GL_LOCK_RENDER_BUFFERS(gc);
	saveFlag = vb->flagBits & __GL_VERTEX_EDGE_FLAG;
	vb->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v0, va, vb);
	vb->flagBits |= saveFlag;
	v0->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v0, vb, vc);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
}

void __gl4thQuadsVertex(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *va, *vb, *vc;
    GLuint orCodes, andCodes;
    GLuint saveFlag;
    __GLvertex *iv[4];

    va = &gc->vertex.vbuf[0];
    vb = &gc->vertex.vbuf[1];
    vc = &gc->vertex.vbuf[2];

    /* Setup for this triangle */
    v0->flagBits |= gc->state.current.edgeTag;
    gc->line.notResetStipple = GL_FALSE;
    gc->vertex.provoking = v0;

    /* Setup for next quad */
    gc->vertex.v0 = va;
    gc->procs.vertex = FirstQuadsVertex;

    /* Clip Check */
    orCodes = (v0->flagBits | va->flagBits | vb->flagBits | vc->flagBits) & 
	__GL_CLIP_ALL_MASK;
    if (orCodes) {
	/* Some kind of clipping is needed */
	andCodes = v0->flagBits & va->flagBits & vb->flagBits & vc->flagBits & 
	    __GL_CLIP_ALL_MASK;
	if (andCodes) {
	    /* Trivially reject the quad */
	} else {
	    /* Clip the quad as a polygon */
	    iv[0] = va;
	    iv[1] = vb;
	    iv[2] = vc;
	    iv[3] = v0;
	    __glDoPolygonClip(gc, &iv[0], 4, orCodes);
	}
    } else {
	/* Render the quad as two triangles */
	saveFlag = vb->flagBits & __GL_VERTEX_EDGE_FLAG;
	vb->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v0, va, vb);
	vb->flagBits |= saveFlag;
	v0->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	(*gc->procs.renderTriangle)(gc, v0, vb, vc);
    }
}

static void ThirdQuadsVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= gc->state.current.edgeTag;
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = gc->procs.vertex4thQuads;
}

static void SecondQuadsVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= gc->state.current.edgeTag;
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = ThirdQuadsVertex;
}

static void FirstQuadsVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= gc->state.current.edgeTag;
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = SecondQuadsVertex;
}

void __glBeginQuads(__GLcontext *gc)
{
    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = FirstQuadsVertex;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}

/************************************************************************/

void __glPolygonVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= gc->state.current.edgeTag;
    if (v0 == &gc->vertex.vbuf[__GL_NVBUF - 1]) {
	__GLvertex *vFirst = &gc->vertex.vbuf[0];
	__GLvertex *vPrev = v0 - 1;
	GLuint vPrevFlag = vPrev->flagBits & __GL_VERTEX_EDGE_FLAG;

	/*
	** This vertex (v0) just filled up the last cell in the vertex
	** buffer.  Flush out the current polygon state (without this new
	** vertex) into the clipper.  Mark the closing edge of this
	** decomposed polygon as non-boundary because we are
	** synthetically generating it.
	*/
	vPrev->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.clipPolygon)(gc, vFirst, __GL_NVBUF - 1);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
	vPrev->flagBits = vPrevFlag;

	/*
	** Now reset the vertex buffer to contain three verticies:
	** vFirst, vPrev, and the new vertex in v0.  This is done with
	** copies because the decomposer expects the polygon verticies to
	** be in sequential memory order.  Since this is supposedly
	** a very rare event, the copies are probably reasonable.
	*/
	gc->vertex.vbuf[1] = *vPrev;
	gc->vertex.vbuf[2] = *v0;
	gc->vertex.v0 = &gc->vertex.vbuf[3];

	/*
	** Mark the first vertex's edge tag as non-boundary because when
	** it gets rendered again it will no longer be a boundary edge.
	*/
	vFirst->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
    } else {
	gc->vertex.v0 = v0 + 1;
    }
}

void __glPolygonVertex(__GLcontext *gc, __GLvertex *v0)
{
    v0->flagBits |= gc->state.current.edgeTag;
    if (v0 == &gc->vertex.vbuf[__GL_NVBUF - 1]) {
	__GLvertex *vFirst = &gc->vertex.vbuf[0];
	__GLvertex *vPrev = v0 - 1;
	GLuint vPrevFlag = vPrev->flagBits & __GL_VERTEX_EDGE_FLAG;

	/*
	** This vertex (v0) just filled up the last cell in the vertex
	** buffer.  Flush out the current polygon state (without this new
	** vertex) into the clipper.  Mark the closing edge of this
	** decomposed polygon as non-boundary because we are
	** synthetically generating it.
	*/
	vPrev->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
	(*gc->procs.clipPolygon)(gc, vFirst, __GL_NVBUF - 1);
	vPrev->flagBits = vPrevFlag;

	/*
	** Now reset the vertex buffer to contain three verticies:
	** vFirst, vPrev, and the new vertex in v0.  This is done with
	** copies because the decomposer expects the polygon verticies to
	** be in sequential memory order.  Since this is supposedly
	** a very rare event, the copies are probably reasonable.
	*/
	gc->vertex.vbuf[1] = *vPrev;
	gc->vertex.vbuf[2] = *v0;
	gc->vertex.v0 = &gc->vertex.vbuf[3];

	/*
	** Mark the first vertex's edge tag as non-boundary because when
	** it gets rendered again it will no longer be a boundary edge.
	*/
	vFirst->flagBits &= ~__GL_VERTEX_EDGE_FLAG;
    } else {
	gc->vertex.v0 = v0 + 1;
    }
}

void __glEndPolygonLock(__GLcontext *gc)
{
    __GLvertex *v0 = gc->vertex.v0;
    __GLvertex *vFirst = &gc->vertex.vbuf[0];
    GLint nv = (GLint)(v0 - vFirst);

    if (nv >= 3) {
	/*
	** Decompose polygon remaining in the buffer.  The first vertex's
	** edge tag will have been set properly if this is the tail part
	** of a large polygon that overflowed the internal vertex buffer.
	*/
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.clipPolygon)(gc, vFirst, nv);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
    gc->procs.endPrim = __glEndPrim;
}

void __glEndPolygon(__GLcontext *gc)
{
    __GLvertex *v0 = gc->vertex.v0;
    __GLvertex *vFirst = &gc->vertex.vbuf[0];
    GLint nv = (GLint)(v0 - vFirst);

    if (nv >= 3) {
	/*
	** Decompose polygon remaining in the buffer.  The first vertex's
	** edge tag will have been set properly if this is the tail part
	** of a large polygon that overflowed the internal vertex buffer.
	*/
	(*gc->procs.clipPolygon)(gc, vFirst, nv);
    }
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
    gc->procs.endPrim = __glEndPrim;
}

void __glBeginPolygon(__GLcontext *gc)
{
    gc->line.notResetStipple = GL_FALSE;

    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = gc->procs.vertexPoly;
    gc->procs.endPrim = gc->procs.endPoly;

    /*
    ** XXX
    ** This is pretty stupid.  We should pay more attention with polygons
    ** as to which vertexes have been validated.  We don't want to revalidate
    ** them all of the time if the material keeps changing (there could 
    ** be alot!).  However, this should work, so it will do for now.
    */
    gc->procs.matValidate = __glMatValidateVbuf0N;
}
