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
** $Header: //depot/main/gfx/lib/opengl/drivers/DDX/ddxscreens.c#2 $
*/

#include "ddxcx.h"
#include <GL/glxtokens.h>

/*
** All fields are statically initialized here except for the Visual IDs, which
** are set at extension initialization, when the server matches these visual
** configurations with its actual Visuals.
*/

__GLXvisualConfig __glDdxVisualConfigs[] = {
    {
	0,			/* visual ID 		*/
	TrueColor,		/* visual class 	*/
	TRUE,			/* rgba			*/
	3,			/* red size 		*/
	3,			/* green size 		*/
	2,			/* blue size		*/
	0,			/* alpha size		*/
	0,			/* red mask		*/
	0,			/* green mask		*/
	0,			/* blue mask		*/
	0,			/* alpha mask		*/
	16,			/* accum red size	*/
	16,			/* accum green size	*/
	16,			/* accum blue size	*/
	16,			/* accum alpha size	*/
	TRUE,			/* double buffer 	*/
	FALSE,			/* stereo		*/
	8,			/* buffer size 		*/
	32,			/* depth size		*/
	8,			/* stencil size 	*/
	0,			/* aux buffers 		*/
	0,			/* level in plane stacking */
	GLX_NONE_EXT,		/* visual caveats	*/
	GLX_NONE_EXT,		/* transparent pixel	*/
	0,			/* transparent color red */
	0,			/* transparent color green */
	0,			/* transparent color blue */
	0,			/* transparent color alpha */
	0,			/* transparent index 	*/
    },
    {
	0,			/* visual ID 		*/
	TrueColor,		/* visual class 	*/
	TRUE,			/* rgba			*/
	5,			/* red size 		*/
	6,			/* green size 		*/
	5,			/* blue size		*/
	0,			/* alpha size		*/
	0,			/* red mask		*/
	0,			/* green mask		*/
	0,			/* blue mask		*/
	0,			/* alpha mask		*/
	16,			/* accum red size	*/
	16,			/* accum green size	*/
	16,			/* accum blue size	*/
	16,			/* accum alpha size	*/
	TRUE,			/* double buffer 	*/
	FALSE,			/* stereo		*/
	16,			/* buffer size 		*/
	32,			/* depth size		*/
	8,			/* stencil size 	*/
	0,			/* aux buffers 		*/
	0,			/* level in plane stacking */
	GLX_NONE_EXT,		/* visual caveats	*/
	GLX_NONE_EXT,		/* transparent pixel	*/
	0,			/* transparent color red */
	0,			/* transparent color green */
	0,			/* transparent color blue */
	0,			/* transparent color alpha */
	0,			/* transparent index 	*/
    },
    {
	0,			/* visual ID 		*/
	TrueColor,		/* visual class 	*/
	TRUE,			/* rgba			*/
	5,			/* red size 		*/
	5,			/* green size 		*/
	5,			/* blue size		*/
	0,			/* alpha size		*/
	0,			/* red mask		*/
	0,			/* green mask		*/
	0,			/* blue mask		*/
	0,			/* alpha mask		*/
	16,			/* accum red size	*/
	16,			/* accum green size	*/
	16,			/* accum blue size	*/
	16,			/* accum alpha size	*/
	TRUE,			/* double buffer 	*/
	FALSE,			/* stereo		*/
	16,			/* buffer size 		*/
	32,			/* depth size		*/
	8,			/* stencil size 	*/
	0,			/* aux buffers 		*/
	0,			/* level in plane stacking */
	GLX_NONE_EXT,		/* visual caveats	*/
	GLX_NONE_EXT,		/* transparent pixel	*/
	0,			/* transparent color red */
	0,			/* transparent color green */
	0,			/* transparent color blue */
	0,			/* transparent color alpha */
	0,			/* transparent index 	*/
    },
    {
	0,			/* visual ID 		*/
	PseudoColor,		/* visual class 	*/
	FALSE,			/* rgba			*/
	0,			/* red size 		*/
	0,			/* green size 		*/
	0,			/* blue size		*/
	0,			/* alpha size		*/
	0,			/* red mask		*/
	0,			/* green mask		*/
	0,			/* blue mask		*/
	0,			/* alpha mask		*/
	0,			/* accum red size	*/
	0,			/* accum green size	*/
	0,			/* accum blue size	*/
	0,			/* accum alpha size	*/
	FALSE,			/* double buffer 	*/
	FALSE,			/* stereo		*/
	8,			/* buffer size 		*/
	32,			/* depth size		*/
	8,			/* stencil size 	*/
	0,			/* aux buffers 		*/
	0,			/* level in plane stacking */
	GLX_NONE_EXT,		/* visual caveats	*/
	GLX_NONE_EXT,		/* transparent pixel	*/
	0,			/* transparent color red */
	0,			/* transparent color green */
	0,			/* transparent color blue */
	0,			/* transparent color alpha */
	0,			/* transparent index 	*/
    },
    {
	0,				/* visual ID 		*/
	PseudoColor,			/* visual class 	*/
	FALSE,				/* rgba			*/
	0,				/* red size 		*/
	0,				/* green size 		*/
	0,				/* blue size		*/
	0,				/* alpha size		*/
	0,				/* red mask		*/
	0,				/* green mask		*/
	0,				/* blue mask		*/
	0,				/* alpha mask		*/
	0,				/* accum red size	*/
	0,				/* accum green size	*/
	0,				/* accum blue size	*/
	0,				/* accum alpha size	*/
	FALSE,				/* double buffer 	*/
	FALSE,				/* stereo		*/
	2,				/* buffer size 		*/
	32,				/* depth size		*/
	0,				/* stencil size 	*/
	0,				/* aux buffers 		*/
	1,				/* level in plane stacking */
	GLX_NONE_EXT,			/* visual caveats	*/
	GLX_TRANSPARENT_TYPE_EXT,	/* transparent pixel	*/
	0,				/* transparent color red */
	0,				/* transparent color green */
	0,				/* transparent color blue */
	0,				/* transparent color alpha */
	0,				/* transparent index 	*/
    },
};

__GLXscreenInfo __glDDXScreenInfo = {
    __glDdxScreenProbe,
    __glDdxCreateContext,
    &__glDdxVisualConfigs[0],
    sizeof(__glDdxVisualConfigs)/sizeof(__GLXvisualConfig),/* number of visual configs		*/
    0,				/* number of usable visual configs	*/
    NULL,			/* to be filled at extension init 	*/
    NULL,
    NULL,
    NULL,
};

