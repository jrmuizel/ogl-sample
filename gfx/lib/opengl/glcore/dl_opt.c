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
** $Header: //depot/main/gfx/lib/opengl/glcore/dl_opt.c#10 $
*/
#include <stdio.h>
#include "global.h"
#include "context.h"
#include "dlistopt.h"
#include "g_imfncs.h"
#include "g_lcfncs.h"
#include "g_disp.h"
#include <GL/gl.h>

/*
** The default display list optimizer.  By default, consecutive material
** calls stored in a display list are optimized.
*/
void __glGenericDlistOptimizer(__GLcontext *gc, __GLcompiledDlist *cdlist)
{
#ifdef __GL_LINT
    gc = gc;
    cdlist = cdlist;
#endif
    __glDlistOptimizeMaterial(gc, cdlist);
}

__GLlistExecFunc *__gl_GenericDlOps[] = {
    __glle_Begin_LineLoop,
    __glle_Begin_LineStrip,
    __glle_Begin_Lines,
    __glle_Begin_Points,
    __glle_Begin_Polygon,
    __glle_Begin_TriangleStrip,
    __glle_Begin_TriangleFan,
    __glle_Begin_Triangles,
    __glle_Begin_QuadStrip,
    __glle_Begin_Quads,
    __glle_InvalidValue,
    __glle_InvalidEnum,
    __glle_InvalidOperation,
    __glle_UnimplementedExtension,
    __glle_FastMaterial,
    __glle_TableTooLarge,
};

/*
** This is the compilation routine for Begin.  It doesn't actually serve
** any terribly important purpose.  It simply stores the type of begin
** in the type of display list entry rather than in the entry itself.
*/
void GLAPI __gllc_Begin(GLenum mode)
{
    __GLdlistOp *dlop;
    __GLlistExecFunc *func;
    GLshort opcode;
    __GL_SETUP();

    dlop = __glDlistAllocOp2(gc, 0);
    if (dlop == NULL) return;

    switch(mode) {
      case GL_LINE_LOOP:
	opcode = __glop_Begin_LineLoop;
	func = __glle_Begin_LineLoop;
	break;
      case GL_LINE_STRIP:
	opcode = __glop_Begin_LineStrip;
	func = __glle_Begin_LineStrip;
	break;
      case GL_LINES:
	opcode = __glop_Begin_Lines;
	func = __glle_Begin_Lines;
	break;
      case GL_POINTS:
	opcode = __glop_Begin_Points;
	func = __glle_Begin_Points;
	break;
      case GL_POLYGON:
	opcode = __glop_Begin_Polygon;
	func = __glle_Begin_Polygon;
	break;
      case GL_TRIANGLE_STRIP:
	opcode = __glop_Begin_TriangleStrip;
	func = __glle_Begin_TriangleStrip;
	break;
      case GL_TRIANGLE_FAN:
	opcode = __glop_Begin_TriangleFan;
	func = __glle_Begin_TriangleFan;
	break;
      case GL_TRIANGLES:
	opcode = __glop_Begin_Triangles;
	func = __glle_Begin_Triangles;
	break;
      case GL_QUAD_STRIP:
	opcode = __glop_Begin_QuadStrip;
	func = __glle_Begin_QuadStrip;
	break;
      case GL_QUADS:
	opcode = __glop_Begin_Quads;
	func = __glle_Begin_Quads;
	break;
      default:
	dlop->opcode = __glop_InvalidEnum;
	__glDlistAppendOp(gc, dlop, __glle_InvalidEnum);
	return;
    }

    gc->dlist.listData.genericFlags |= __GL_DLFLAG_HAS_BEGIN;
    dlop->opcode = opcode;
    __glDlistAppendOp(gc, dlop, func);
}

/************************************************************************/

const GLubyte *__glle_Begin_LineLoop(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_LINE_LOOP);
    return PC;
}

const GLubyte *__glle_Begin_LineStrip(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_LINE_STRIP);
    return PC;
}

const GLubyte *__glle_Begin_Lines(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_LINES);
    return PC;
}

const GLubyte *__glle_Begin_Points(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_POINTS);
    return PC;
}

const GLubyte *__glle_Begin_Polygon(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_POLYGON);
    return PC;
}

const GLubyte *__glle_Begin_TriangleStrip(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_TRIANGLE_STRIP);
    return PC;
}

const GLubyte *__glle_Begin_TriangleFan(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_TRIANGLE_FAN);
    return PC;
}

const GLubyte *__glle_Begin_Triangles(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_TRIANGLES);
    return PC;
}

const GLubyte *__glle_Begin_QuadStrip(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_QUAD_STRIP);
    return PC;
}

const GLubyte *__glle_Begin_Quads(const GLubyte *PC)
{
    (*__gl_dispatch.dispatch.Begin)(GL_QUADS);
    return PC;
}

/************************************************************************/

/*
** Optimized errors.  Strange but true.  These are called to save an error
** in the display list.
*/
void GLAPI __gllc_InvalidValue(__GLcontext *gc)
{
    __GLdlistOp *dlop;

    dlop = __glDlistAllocOp2(gc, 0);
    if (dlop == NULL) return;
    dlop->opcode = __glop_InvalidValue;
    __glDlistAppendOp(gc, dlop, __glle_InvalidValue);
}

void GLAPI __gllc_InvalidEnum(__GLcontext *gc)
{
    __GLdlistOp *dlop;

    dlop = __glDlistAllocOp2(gc, 0);
    if (dlop == NULL) return;
    dlop->opcode = __glop_InvalidEnum;
    __glDlistAppendOp(gc, dlop, __glle_InvalidEnum);
}

void GLAPI __gllc_InvalidOperation(__GLcontext *gc)
{
    __GLdlistOp *dlop;

    dlop = __glDlistAllocOp2(gc, 0);
    if (dlop == NULL) return;
    dlop->opcode = __glop_InvalidOperation;
    __glDlistAppendOp(gc, dlop, __glle_InvalidOperation);
}

void GLAPI __gllc_TableTooLarge(__GLcontext *gc)
{
    __GLdlistOp *dlop;
    dlop = __glDlistAllocOp2(gc, 0);
    if (dlop == NULL) return;
    dlop->opcode = __glop_TableTooLarge;
    __glDlistAppendOp(gc, dlop, __glle_TableTooLarge);
}

/*
** Special case function call. Needs void argument to be plugged into
** dispatch tables.
*/
void GLAPI __gllc_UnimplementedExtension(void)
{
    __GLdlistOp *dlop;
    __GL_SETUP();
    dlop = __glDlistAllocOp2(gc, 0);
    if (dlop == NULL) return;
    dlop->opcode = __glop_UnimplementedExtension;
    __glDlistAppendOp(gc, dlop, __glle_UnimplementedExtension);
}


void GLAPI __gllc_Error(__GLcontext *gc, GLenum error)
{
    switch(error) {
      case GL_INVALID_VALUE:
	__gllc_InvalidValue(gc);
	break;
      case GL_INVALID_ENUM:
	__gllc_InvalidEnum(gc);
	break;
      case GL_INVALID_OPERATION:
	__gllc_InvalidOperation(gc);
	break;
      case GL_TABLE_TOO_LARGE:
	__gllc_TableTooLarge(gc);
	break;
    }
}

/*
** These routines execute an error stored in a display list.
*/
const GLubyte *__glle_InvalidValue(const GLubyte *PC)
{
    __glSetError(GL_INVALID_VALUE);
    return PC;
}

const GLubyte *__glle_InvalidEnum(const GLubyte *PC)
{
    __glSetError(GL_INVALID_ENUM);
    return PC;
}

const GLubyte *__glle_InvalidOperation(const GLubyte *PC)
{
    __glSetError(GL_INVALID_OPERATION);
    return PC;
}

const GLubyte *__glle_TableTooLarge(const GLubyte *PC)
{
    __glSetError(GL_TABLE_TOO_LARGE);
    return PC;
}


/*
** Although it returns the same error condition, I've created a
** new error routine for unimplemented extensions to help with debugging.
*/

const GLubyte *__glle_UnimplementedExtension(const GLubyte *PC)
{
  __glSetError(GL_INVALID_OPERATION);
  return PC;
}



