#ifndef __gl_imports_h_
#define __gl_imports_h_

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
*/
/*
** Imports from outside libraries.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/importsx.h#4 $
*/
#include <memory.h>
#include <string.h>
#include <math.h>

#define __GL_MEMCOPY(to,from,count)	memcpy(to,from,(size_t)(count))
#define __GL_MEMZERO(to,count)		memset(to,0,(size_t)(count))

/* Use this for overlapping copies!! Often slower than memcpy */
#define __GL_MEMMOVE(to,from,count)	memmove(to,from,(size_t)(count))

#ifdef __sgi
#define __GL_CEILF(f)			ceilf(f)
#define __GL_SQRTF(f)			sqrtf(f)
#define __GL_POWF(a,b)			powf(a,b)
#define __GL_ABSF(f)			fabsf(f)
#define __GL_FLOORF(f)			floorf(f)
#define __GL_FLOORD(f)			floor(f)
#define __GL_SINF(f)			sinf(f)
#define __GL_COSF(f)			cosf(f)
#define __GL_ATANF(f)			atanf(f)
#define __GL_LOGF(f)			logf(f)
#else
#include "GL/glx_ansic.h"

#undef __GL_MEMCOPY
#define __GL_MEMCOPY(to,from,count)	__glXMemcpy(to,from,(size_t)(count))
#undef __GL_MEMZERO
#define __GL_MEMZERO(to,count)		__glXMemset(to,0,(size_t)(count))
#undef __GL_MEMMOVE
#define __GL_MEMMOVE(to,from,count)	__glXMemmove(to,from,(size_t)(count))

#define __GL_CEILF(f)			((GLfloat)__glXCeil((double) (f)))
#define __GL_SQRTF(f)			((GLfloat)__glXSqrt((double) (f)))
#define __GL_POWF(a,b)			((GLfloat)__glXPow((double) (a),(double) (b)))
#define __GL_ABSF(f)			((GLfloat)__glXAbs((double) (f)))
#define __GL_FLOORF(f)			((GLfloat)__glXFloor((double) (f)))
#define __GL_FLOORD(f)			floor(f)
#define __GL_SINF(f)			((GLfloat)__glXSin((double) (f)))
#define __GL_COSF(f)			((GLfloat)__glXCos((double) (f)))
#define __GL_ATANF(f)			((GLfloat)__glXAtan((double) (f)))
#define __GL_LOGF(f)			((GLfloat)__glXLog((double) (f)))
#endif

#endif /* __gl_imports_h_ */
