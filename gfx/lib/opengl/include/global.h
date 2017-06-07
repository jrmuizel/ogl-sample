#ifndef __gl_global_h_
#define __gl_global_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/global.h#6 $
*/
#include "context.h"

/*
** Some misc constants
*/
#define __glZero		((__GLfloat) 0.0)
#define __glOne			(gc->constants.one)
#define __glMinusOne		((__GLfloat) -1.0)
#define __glHalf		(gc->constants.half)
#define __glTwo			((__GLfloat) 2.0)
#define __glDegreesToRadians	(__glPi / (__GLfloat) 180.0)
#define __glPi			((__GLfloat) 3.14159265358979323846)
#define __glSqrt2		((__GLfloat) 1.41421356237309504880)
#define __glE			((__GLfloat) 2.7182818284590452354)

/************************************************************************/

#define __GL_SETUP() \
    __GLcontext *gc = __gl_context

#define __GL_IN_BEGIN() \
    (__gl_beginMode == __GL_IN_BEGIN)

#define __GL_SETUP_NOT_IN_BEGIN()	    \
    __GL_SETUP();			    \
    if (__GL_IN_BEGIN()) {		    \
	__glSetError(GL_INVALID_OPERATION); \
	return;				    \
    }

#define __GL_SETUP_NOT_IN_BEGIN_NOGLSETUP()   \
    if (__GL_IN_BEGIN()) {		    \
	__glSetError(GL_INVALID_OPERATION); \
	return;				    \
    }


#define __GL_SETUP_NOT_IN_BEGIN_VALIDATE()	\
    __GL_SETUP();				\
    __GLbeginMode beginMode = __gl_beginMode;	\
    if (beginMode != __GL_NOT_IN_BEGIN) {	\
	if (beginMode == __GL_NEED_VALIDATE) {	\
	    (*gc->procs.validate)(gc);		\
	    __gl_beginMode = __GL_NOT_IN_BEGIN;	\
	} else {				\
	    __glSetError(GL_INVALID_OPERATION);	\
	    return;				\
	}					\
    }

#define __GL_SETUP_NOT_IN_BEGIN2()	    \
    __GL_SETUP();			    \
    if (__GL_IN_BEGIN()) {		    \
	__glSetError(GL_INVALID_OPERATION); \
	return 0;			    \
    }

#endif /* __gl_global_h_ */
