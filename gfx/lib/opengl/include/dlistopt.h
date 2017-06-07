#ifndef __gl_dlistopt_h_
#define __gl_dlistopt_h_

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
*/
/*
** Display list state descriptions.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/dlistopt.h#8 $
*/

/*
** Generic optimizer.  This optimizer simply uses all of the generic
** optimizations.
*/
void __glDlistOptimizer(__GLcontext *gc, __GLcompiledDlist *cdlist);

/*
** Optimizer for multiple consecutive material changes.  This routine 
** combines a bunch of material changes into one quick material change.
*/
void __glDlistOptimizeMaterial(__GLcontext *gc, __GLcompiledDlist *cdlist);

/*
** Generic flags used for optimization (during gllc routines).
*/
#define __GL_DLFLAG_HAS_VERTEX		0x00000001
#define __GL_DLFLAG_HAS_NORMAL		0x00000002
#define __GL_DLFLAG_HAS_COLOR		0x00000004
#define __GL_DLFLAG_HAS_TEXCOORDS	0x00000008
#define __GL_DLFLAG_HAS_INDEX		0x00000010
#define __GL_DLFLAG_HAS_RASTERPOS	0x00000020
#define __GL_DLFLAG_HAS_RECT		0x00000040
#define __GL_DLFLAG_HAS_BEGIN		0x00000080
#define __GL_DLFLAG_HAS_MATERIAL	0x00000100

/*
** Generic opcodes created during generic dlist optimizations.
*/
#define __GL_GENERIC_DLIST_OPCODE	1000
#define __glop_Begin_LineLoop		1000
#define __glop_Begin_LineStrip		1001
#define __glop_Begin_Lines		1002
#define __glop_Begin_Points		1003
#define __glop_Begin_Polygon		1004
#define __glop_Begin_TriangleStrip	1005
#define __glop_Begin_TriangleFan	1006
#define __glop_Begin_Triangles		1007
#define __glop_Begin_QuadStrip		1008
#define __glop_Begin_Quads		1009
#define __glop_InvalidValue		1010
#define __glop_InvalidEnum		1011
#define __glop_InvalidOperation		1012
#define __glop_UnimplementedExtension   1013
#define __glop_FastMaterial		1014
#define __glop_TableTooLarge            1015

/*
** List execution functions.
*/
extern __GLlistExecFunc *__gl_GenericDlOps[];
extern const GLubyte *__glle_Begin_LineLoop(const GLubyte *);
extern const GLubyte *__glle_Begin_LineStrip(const GLubyte *);
extern const GLubyte *__glle_Begin_Lines(const GLubyte *);
extern const GLubyte *__glle_Begin_Points(const GLubyte *);
extern const GLubyte *__glle_Begin_Polygon(const GLubyte *);
extern const GLubyte *__glle_Begin_TriangleStrip(const GLubyte *);
extern const GLubyte *__glle_Begin_TriangleFan(const GLubyte *);
extern const GLubyte *__glle_Begin_Triangles(const GLubyte *);
extern const GLubyte *__glle_Begin_QuadStrip(const GLubyte *);
extern const GLubyte *__glle_Begin_Quads(const GLubyte *);
extern const GLubyte *__glle_InvalidValue(const GLubyte *);
extern const GLubyte *__glle_InvalidEnum(const GLubyte *);
extern const GLubyte *__glle_InvalidOperation(const GLubyte *);
extern const GLubyte *__glle_UnimplementedExtension(const GLubyte *);
extern const GLubyte *__glle_TableTooLarge(const GLubyte *);
extern const GLubyte *__glle_FastMaterial(const GLubyte *);
extern void GLAPI __gllc_InvalidValue(__GLcontext *gc);
extern void GLAPI __gllc_InvalidEnum(__GLcontext *gc);
extern void GLAPI __gllc_InvalidOperation(__GLcontext *gc);
extern void GLAPI __gllc_TableTooLarge(__GLcontext *gc);
extern void GLAPI __gllc_UnimplementedExtension(void);
extern void GLAPI __gllc_Error(__GLcontext *gc, GLenum error);

#endif /* __gl_dlistopt_h_ */
