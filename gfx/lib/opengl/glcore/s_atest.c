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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_atest.c#4 $
*/
#include "context.h"
#include "render.h"

/*
** Initialize a lookup table that is indexed by the iterated alpha value.
** The table indicates whether the alpha test passed or failed, based on
** the current alpha function and the alpha reference value.
**
** NOTE:  The alpha span routines will never be called if the alpha test
** is GL_ALWAYS (its useless) or if the alpha test is GL_NEVER.  This
** is accomplished in the __glGenericPickSpanProcs procedure.
*/
void __glValidateAlphaTest(__GLcontext *gc)
{
    GLubyte *atft;
    GLint i, limit;
    GLint ref;
    GLenum alphaTestFunc = gc->state.raster.alphaFunction;

    limit = gc->constants.alphaTestSize;
    ref = (GLint)
	((gc->state.raster.alphaReference * gc->frontBuffer.alphaScale) *
	gc->constants.alphaTableConv);

    /*
    ** Allocate alpha test function table the first time.  It needs
    ** to have at most one entry for each possible alpha value.
    */
    atft = gc->frontBuffer.alphaTestFuncTable;
    if (!atft) {
	atft = (GLubyte*)
	    (*gc->imports.malloc)(gc, (size_t) ((limit) * sizeof(GLubyte)));
	gc->frontBuffer.alphaTestFuncTable = atft;
    }

    /*
    ** Build up alpha test lookup table.  The computed alpha value is
    ** used as an index into this table to determine if the alpha
    ** test passed or failed.
    */
    for (i = 0; i < limit; i++) {
	switch (alphaTestFunc) {
	  case GL_NEVER:	*atft++ = GL_FALSE; break;
	  case GL_LESS:		*atft++ = (GLubyte) (i <  ref); break;
	  case GL_EQUAL:	*atft++ = (GLubyte) (i == ref); break;
	  case GL_LEQUAL:	*atft++ = (GLubyte) (i <= ref); break;
	  case GL_GREATER:	*atft++ = (GLubyte) (i >  ref); break;
	  case GL_NOTEQUAL:	*atft++ = (GLubyte) (i != ref); break;
	  case GL_GEQUAL:	*atft++ = (GLubyte) (i >= ref); break;
	  case GL_ALWAYS:	*atft++ = GL_TRUE; break;
	}
    }
}
