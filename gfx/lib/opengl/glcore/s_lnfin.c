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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_lnfin.c#4 $
*/
#include "context.h"

void __gl1stLinesVertex(__GLcontext *gc, __GLvertex *v0)
{
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = gc->procs.vertex2ndLines;
}

void __gl2ndLinesVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    gc->line.notResetStipple = GL_FALSE;

    gc->vertex.v0 = v0 - 1;
    gc->procs.vertex = __gl1stLinesVertex;

    __GL_LOCK_RENDER_BUFFERS(gc);
    (*gc->procs.clipLine)(gc, v0 - 1, v0);
    __GL_UNLOCK_RENDER_BUFFERS(gc);
}

void __gl2ndLinesVertex(__GLcontext *gc, __GLvertex *v0)
{
    gc->line.notResetStipple = GL_FALSE;

    gc->vertex.v0 = v0 - 1;
    gc->procs.vertex = __gl1stLinesVertex;

    (*gc->procs.clipLine)(gc, v0 - 1, v0);
}

void __glBeginLines(__GLcontext *gc)
{
    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = __gl1stLinesVertex;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}

/************************************************************************/

/*
** input  v0  v1    v0'  v1'   result
** -----  --  --    ---  ---   ------
** begin  --  --    -0   --
** A      A0  --    -1   A0
** B      B1  A0    A0   B1    draw AB
** C      C0  B1    B1   C0    draw BC
*/
void __glOtherLStripVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1 = gc->vertex.v1;

    gc->vertex.v0 = v1;
    gc->vertex.v1 = v0;

    if ((v1->flagBits | v0->flagBits) & __GL_CLIP_ALL_MASK) {
	/*
	** The line must be clipped more carefully.  Cannot trivially
	** accept the lines.
	*/
	if (((v1->flagBits & v0->flagBits) & __GL_CLIP_ALL_MASK) != 0) {
	    /*
	    ** Trivially reject the line.  If anding the codes is non-zero then
	    ** every vertex in the line is outside of the same set of
	    ** clipping planes (at least one).
	    */
	    return;
	}
	__GL_LOCK_RENDER_BUFFERS(gc);
	__glClipLine(gc, v1, v0);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
	return;
    }

    /* Validate provoking vertex color */
    DO_VALIDATE(gc, v0, gc->vertex.faceNeeds[__GL_FRONTFACE]);

    /* Draw the line */
    __GL_LOCK_RENDER_BUFFERS(gc);
    (*gc->procs.renderLine)(gc, v1, v0);
    __GL_UNLOCK_RENDER_BUFFERS(gc);
}

void __glOtherLStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1 = gc->vertex.v1;

    gc->vertex.v0 = v1;
    gc->vertex.v1 = v0;

    if ((v1->flagBits | v0->flagBits) & __GL_CLIP_ALL_MASK) {
	/*
	** The line must be clipped more carefully.  Cannot trivially
	** accept the lines.
	*/
	if (((v1->flagBits & v0->flagBits) & __GL_CLIP_ALL_MASK) != 0) {
	    /*
	    ** Trivially reject the line.  If anding the codes is non-zero then
	    ** every vertex in the line is outside of the same set of
	    ** clipping planes (at least one).
	    */
	    return;
	}
	__glClipLine(gc, v1, v0);
	return;
    }

    /* Validate provoking vertex color */
    DO_VALIDATE(gc, v0, gc->vertex.faceNeeds[__GL_FRONTFACE]);

    /* Draw the line */
    (*gc->procs.renderLine)(gc, v1, v0);
}

void __glOtherLStripVertexFastLock(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1 = gc->vertex.v1;

    gc->vertex.v0 = v1;
    gc->vertex.v1 = v0;

    __GL_LOCK_RENDER_BUFFERS(gc);
    if ((v0->flagBits | v1->flagBits) & __GL_CLIP_ALL_MASK) {
	(*gc->procs.clipLine)(gc, v1, v0);
    } else {
	(*gc->procs.renderLine)(gc, v1, v0);
    }
    __GL_UNLOCK_RENDER_BUFFERS(gc);
}

void __glOtherLStripVertexFast(__GLcontext *gc, __GLvertex *v0)
{
    __GLvertex *v1 = gc->vertex.v1;

    gc->vertex.v0 = v1;
    gc->vertex.v1 = v0;

    if ((v0->flagBits | v1->flagBits) & __GL_CLIP_ALL_MASK) {
	(*gc->procs.clipLine)(gc, v1, v0);
    } else {
	(*gc->procs.renderLine)(gc, v1, v0);
    }
}

void __gl1stLStripVertex(__GLcontext *gc, __GLvertex *v0)
{
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v1 = v0;
    gc->procs.vertex = gc->procs.vertexLStrip;
    gc->procs.matValidate = __glMatValidateV1;

    /* Validate first vertex, if needed */
    if (gc->vertex.faceNeeds[__GL_FRONTFACE])
	DO_VALIDATE(gc, v0, gc->vertex.faceNeeds[__GL_FRONTFACE]);
}

void __glBeginLStrip(__GLcontext *gc)
{
    gc->line.notResetStipple = GL_FALSE;

    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = __gl1stLStripVertex;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}

/************************************************************************/

/*
** Here is a three vertex example of a line loop:
**     input   v0  v1    v0'  v1'    result
**     -----   --  --    ---  ---    ------
**     begin   --  --    -0   --
**     A       A0  --    -1   --
**     B       B1  --    -2   B1     draw AB
**     C       C2  B1    B1   C2     draw BC
**     end     B1  C2    --   --     draw CA
**
** Here is a two vertex example of a line loop:
**     input   v0  v1    v0'  v1'    result
**     -----   --  --    ---  ---    ------
**     begin   --  --    -0   --
**     A       A0  --    -1   --
**     B       B1  --    -2   B1     draw AB
**     end     -2  B1    --   --     draw BA
**
** Here is a one vertex example of a line loop:
**     input   v0  v1    v0'  v1'    result
**     -----   --  --    ---  ---    ------
**     begin   --  --    -0   --
**     A       A0  --    -1   --
**     end     -1  --    --   --     nothing drawn
*/
void __gl2ndLLoopVertexLock(__GLcontext *gc, __GLvertex *v0)
{
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v1 = v0;
    gc->procs.vertex = gc->procs.vertexLStrip;
    gc->procs.matValidate = __glMatValidateVbuf0V1;

    __GL_LOCK_RENDER_BUFFERS(gc);
    (*gc->procs.clipLine)(gc, v0 - 1, v0);
    __GL_UNLOCK_RENDER_BUFFERS(gc);
}

void __gl2ndLLoopVertex(__GLcontext *gc, __GLvertex *v0)
{
    gc->vertex.v0 = v0 + 1;
    gc->vertex.v1 = v0;
    gc->procs.vertex = gc->procs.vertexLStrip;
    gc->procs.matValidate = __glMatValidateVbuf0V1;

    (*gc->procs.clipLine)(gc, v0 - 1, v0);
}

static void FirstLLoopVertex(__GLcontext *gc, __GLvertex *v0)
{
    gc->vertex.v0 = v0 + 1;
    gc->procs.vertex = gc->procs.vertex2ndLLoop;
}

void __glEndLLoopLock(__GLcontext *gc)
{
    /*
    ** This isn't a terribly kosher way of checking if we have gotten 
    ** two vertices already, but it is the best I can think of.
    */
    if (gc->procs.vertex != FirstLLoopVertex &&
	gc->procs.vertex != gc->procs.vertex2ndLLoop) {
	/*
	** Close off the loop by drawing a final line segment back to the
	** first vertex.  The first vertex was saved in vbuf[0].
	*/
	__GL_LOCK_RENDER_BUFFERS(gc);
	(*gc->procs.clipLine)(gc, gc->vertex.v1, &gc->vertex.vbuf[0]);
	__GL_UNLOCK_RENDER_BUFFERS(gc);
    }
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
    gc->procs.endPrim = __glEndPrim;
}

void __glEndLLoop(__GLcontext *gc)
{
    /*
    ** This isn't a terribly kosher way of checking if we have gotten 
    ** two vertices already, but it is the best I can think of.
    */
    if (gc->procs.vertex != FirstLLoopVertex &&
	gc->procs.vertex != gc->procs.vertex2ndLLoop) {
	/*
	** Close off the loop by drawing a final line segment back to the
	** first vertex.  The first vertex was saved in vbuf[0].
	*/
	(*gc->procs.clipLine)(gc, gc->vertex.v1, &gc->vertex.vbuf[0]);
    }
    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
    gc->procs.endPrim = __glEndPrim;
}

void __glBeginLLoop(__GLcontext *gc)
{
    gc->line.notResetStipple = GL_FALSE;

    gc->vertex.v0 = &gc->vertex.vbuf[0];
    gc->procs.vertex = FirstLLoopVertex;
    gc->procs.endPrim = gc->procs.endLLoop;
    gc->procs.matValidate = __glMatValidateVbuf0N;
}
