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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_cpydsp.c#8 $
*/
/*
 * C version of __glMipsCopyDispatch which is found in mips_copydisp.s
 */

#if !(defined(mips) && defined(__GL_USE_ASMCODE))
#include "types.h"
#include "g_disp.h"
#include "context.h"

#if defined(__GL_WINDOW_SYSTEM_DISPATCH)
/*
** window system function that digests the OpenGL dispatch table for
** use by the window system
*/
extern void __wsUpdateDispatch(__GLcontext *gc,
			       const struct __GLdispatchStateRec *state);
#endif /* __GL_WINDOW_SYSTEM_DISPATCH */

void 
__glCopyDispatch(const __GLdispatchState *src,
		     __GLdispatchState *dst)
{
    if ((dst != src) && src && dst) {
	*dst = *src;
    }
#if defined(__GL_WINDOW_SYSTEM_DISPATCH)
    if (dst == &__gl_dispatch) {
	/* this means copy dispatch into the window system dispatch state */
	__wsUpdateDispatch(__gl_context, src);
    }
#endif /* __GL_WINDOW_SYSTEM_DISPATCH */
}

__GLdispatchState *
__glGetCurrentDispatch(void)
{
    return &__gl_dispatch;
}

#endif /* !(mips && __GL_USE_ASMCODE) */
