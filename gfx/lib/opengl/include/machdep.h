#ifndef __gl_machdep_h_
#define __gl_machdep_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/machdep.h#7 $
*/
#include "context.h"

/*
** Machine dependent implementation limits
*/
#define	__GL_DEFAULT_NUMBER_OF_CLIP_PLANES	6
#define	__GL_DEFAULT_NUMBER_OF_LIGHTS		8

#define	__GL_DEFAULT_MAX_MODELVIEW_STACK_DEPTH	32
#define	__GL_DEFAULT_MAX_PROJECTION_STACK_DEPTH	2
#define	__GL_DEFAULT_MAX_TEXTURE_STACK_DEPTH	10
#define	__GL_DEFAULT_MAX_COLOR_STACK_DEPTH	10
#define	__GL_DEFAULT_MAX_ATTRIB_STACK_DEPTH	16
#define	__GL_DEFAULT_MAX_CLIENT_ATTRIB_STACK_DEPTH	16
#define	__GL_DEFAULT_MAX_TEXTURE_UNITS		4
#define	__GL_DEFAULT_MAX_NAME_STACK_DEPTH	128
#define __GL_DEFAULT_MAX_EVAL_ORDER		30
#define __GL_DEFAULT_MAX_MIPMAP_LEVEL		11
#define __GL_DEFAULT_MAX_PIXEL_MAP_TABLE	65536
#define __GL_DEFAULT_MAX_LIST_NESTING		64
#define __GL_DEFAULT_MAX_CONV_1D_WIDTH		11
#define __GL_DEFAULT_MAX_CONV_2D_WIDTH		11
#define __GL_DEFAULT_MAX_CONV_2D_HEIGHT		11
#define __GL_DEFAULT_MAX_SEP_2D_WIDTH		11
#define __GL_DEFAULT_MAX_SEP_2D_HEIGHT		11

#define __GL_DEFAULT_POINT_SIZE_MINIMUM		0.5
#define __GL_DEFAULT_POINT_SIZE_MAXIMUM		10.0
#define __GL_DEFAULT_POINT_SIZE_GRANULARITY	0.125
#define __GL_DEFAULT_ALIASED_POINT_SIZE_MINIMUM	1
#define __GL_DEFAULT_ALIASED_POINT_SIZE_MAXIMUM 64

#define __GL_DEFAULT_LINE_WIDTH_MINIMUM		0.5 
#define __GL_DEFAULT_LINE_WIDTH_MAXIMUM		10.0
#define __GL_DEFAULT_LINE_WIDTH_GRANULARITY	0.125
#define __GL_DEFAULT_ALIASED_LINE_WIDTH_MINIMUM	1
#define __GL_DEFAULT_ALIASED_LINE_WIDTH_MAXIMUM	64

/************************************************************************/

#define	__GL_DEFAULT_MAX_WINDOW_HEIGHT		2048
#define	__GL_DEFAULT_MAX_WINDOW_WIDTH		2048

/*
** Bias numbers for moving window coordinates into a positive space.
** These values are used during viewport computations.
*/
#define __GL_DEFAULT_VERTEX_X_BIAS		2048
#define __GL_DEFAULT_VERTEX_Y_BIAS		2048

/* 
** Fixing numbers.  These are used to move the biased window coordinates
** into a range where the number of fraction bits are constant from the
** minimal value in the range to the largest value in the range.
*/
#define __GL_DEFAULT_VERTEX_X_FIX		4096
#define __GL_DEFAULT_VERTEX_Y_FIX		4096

#define __GL_DEFAULT_COORD_SUBPIXEL_BITS	3

/************************************************************************/

/*
** Primary dispatch tables
*/
extern __GLdispatchState __glImmedState;
extern __GLdispatchState __glListCompState;
extern __GLdispatchState __glNopDispatchState;

/************************************************************************/

#endif /* __gl_machdep_h_ */
