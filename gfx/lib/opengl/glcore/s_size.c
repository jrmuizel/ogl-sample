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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_size.c#7 $
*/
#include "types.h"

#include "size.h"

GLint __glFogiv_size(GLenum pname)
{
    switch (pname) {
      case GL_FOG_COLOR:	return 4;
      case GL_FOG_DENSITY:	return 1;
      case GL_FOG_END:		return 1;
      case GL_FOG_MODE:		return 1;
      case GL_FOG_INDEX:	return 1;
      case GL_FOG_START:	return 1;
      default:
	return -1;
    }
}

GLint __glFogfv_size(GLenum pname)
{
    return __glFogiv_size(pname);
}

GLint __glLightfv_size(GLenum pname)
{
    switch (pname) {
      case GL_SPOT_EXPONENT:		return 1;
      case GL_SPOT_CUTOFF:		return 1;
      case GL_AMBIENT:			return 4;
      case GL_DIFFUSE:			return 4;
      case GL_SPECULAR:			return 4;
      case GL_POSITION:			return 4;
      case GL_SPOT_DIRECTION:		return 3;
      case GL_CONSTANT_ATTENUATION:	return 1;
      case GL_LINEAR_ATTENUATION:	return 1;
      case GL_QUADRATIC_ATTENUATION:	return 1;
      default:
	return -1;
    }
}

GLint __glLightiv_size(GLenum pname)
{
    return __glLightfv_size(pname);
}


GLint __glLightModelfv_size(GLenum pname)
{
    switch (pname) {
      case GL_LIGHT_MODEL_AMBIENT:		return 4;
      case GL_LIGHT_MODEL_LOCAL_VIEWER:		return 1;
      case GL_LIGHT_MODEL_TWO_SIDE:		return 1;
      case GL_LIGHT_MODEL_COLOR_CONTROL:	return 1;
      default:
	return -1;
    }
}

GLint __glLightModeliv_size(GLenum pname)
{
    return __glLightModelfv_size(pname);
}

GLint __glMaterialfv_size(GLenum pname)
{
    switch (pname) {
      case GL_SHININESS:		return 1;
      case GL_EMISSION:			return 4;
      case GL_AMBIENT:			return 4;
      case GL_DIFFUSE:			return 4;
      case GL_SPECULAR:			return 4;
      case GL_AMBIENT_AND_DIFFUSE:	return 4;
      case GL_COLOR_INDEXES:		return 3;
      default:
	return -1;
    }
}

GLint __glMaterialiv_size(GLenum pname)
{
    return __glMaterialfv_size(pname);
}

GLint __glColorTableParameterfv_size(GLenum pname)
{
    switch (pname) {
      case GL_COLOR_TABLE_SCALE:
      case GL_COLOR_TABLE_BIAS:     return 4;
      default:
	return -1;
    }
}

GLint __glColorTableParameteriv_size(GLenum pname)
{
    return __glColorTableParameterfv_size(pname);
}
