#ifndef _gl_ctable_h_
#define _gl_ctable_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/ctable.h#7 $
*/
#include "types.h"

/*
** Number of color table targets
*/
#define __GL_NUM_COLOR_TABLE_TARGETS			3
#define __GL_COLOR_TABLE_INDEX				0
#define __GL_POST_CONVOLUTION_COLOR_TABLE_INDEX		1
#define __GL_POST_COLOR_MATRIX_COLOR_TABLE_INDEX	2

/*
** Stackable per color table state
*/
typedef struct __GLcolorTableStateRec {
    __GLcolor scale;
    __GLcolor bias;
} __GLcolorTableState;

/*
** Color table state
*/
typedef struct __GLcolorTableRec {
    GLenum target;

    GLvoid *table;

    GLsizei width;

    GLenum format;			/* actual internal format */
    GLenum formatReturn;		/* requested internal format */
    GLenum baseFormat;			/* internal format w/o size info */

    GLenum type;

    GLsizei redSize;
    GLsizei greenSize;
    GLsizei blueSize;
    GLsizei alphaSize;
    GLsizei luminanceSize;
    GLsizei intensitySize;

    __GLcolorTableState state;		/* stackable state */
} __GLcolorTable;

/*
** Function prototypes
*/
GLenum __glCheckColorTableArgs(__GLcontext *gc, GLenum target,
			       GLenum internalformat, GLsizei width,
			       GLenum format, GLenum type);

void __gllei_ColorTable(__GLcontext *gc, GLenum target, GLenum internalformat,
		    GLsizei width, GLenum format, GLenum type,
		    const void *table);

GLenum __glCheckColorSubTableArgs(__GLcontext *gc, GLenum target,
			          GLsizei start, GLsizei count,
			          GLenum format, GLenum type);

void __gllei_ColorSubTable(__GLcontext *gc, GLenum target, GLsizei start,
		       GLsizei count, GLenum format, GLenum type,
		       const void *table);

void __glEarlyInitColorTableState(__GLcontext *gc);
void __glInitColorTableState(__GLcontext *gc);
void __glFreeColorTableState(__GLcontext *gc);

#endif /* _gl_ctable_h_ */
