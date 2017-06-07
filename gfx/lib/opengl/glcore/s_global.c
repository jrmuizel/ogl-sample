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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_global.c#6 $
*/
#include "global.h"
#include "render.h"

#if defined(unix)

/* 
** global thead-specific gc pointer.  Since the sample server is 
** single threaded, we can just put that into global memory
*/
__GLcontext *__gc = NULL;

#endif /* unix */

__GLcoord __gl_frustumClipPlanes[6] = {
    {  1.0,  0.0,  0.0,  1.0 },		/* left */
    { -1.0,  0.0,  0.0,  1.0 },		/* right */
    {  0.0,  1.0,  0.0,  1.0 },		/* bottom */
    {  0.0, -1.0,  0.0,  1.0 },		/* top */
    {  0.0,  0.0,  1.0,  1.0 },		/* near */
    {  0.0,  0.0, -1.0,  1.0 },		/* far */
};

GLbyte __glDitherTable[16] = {
     0,  8,  2, 10,
    12,  4, 14,  6,
     3, 11,  1,  9,
    15,  7, 13,  5,
};

__GLfloat __glDitherTableFloat[16] = {
    -0.468750,  0.031250, -0.343750,  0.156250,
     0.281250, -0.218750,  0.406250, -0.093750,
    -0.281250,  0.218750, -0.406250,  0.093750,
     0.468750, -0.031250,  0.343750, -0.156250,
};
