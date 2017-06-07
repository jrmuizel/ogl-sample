#ifndef __glddxcx_h_
#define __glddxcx_h_

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
** $Header: //depot/main/gfx/lib/opengl/drivers/DDX/ddxcx.h#2 $
*/
#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/Xmd.h>
#include <misc.h>
#include <dixstruct.h>
#include <pixmapstr.h>
#include <windowstr.h>
#include <gcstruct.h>
#include <extnsionst.h>
#include <resource.h>
#include <scrnintstr.h>

/*
** The X header misc.h define these math functions.
*/
#undef abs
#undef fabs

#include <GL/gl.h>
#include "GL/internal/glxint.h"
#include <glxerror.h>
#include "global.h"
#include "context.h"
#include <glxscreens.h>


/************************************************************************/

struct __GLDdxContextRec {
    /*
    ** The GL context must be the first member of this structure.
    */
    __GLcontext gc;

    /*
    ** the X GC used for rendering
    */
    GCPtr pGC;

    /*
    ** Shifts needed to construct a pixel for X.
    */
    GLint redShifts, greenShifts, blueShifts;
};
typedef struct __GLDdxContextRec __GLDdxContext;

/************************************************************************/

extern Bool __glDdxScreenProbe(int screen);
extern __GLinterface *__glDdxCreateContext(__GLimports *imports,
					   __GLcontextModes *modes);
extern Bool __glDdxMakeCurrent(__GLcontext *gc);
extern void __glDdxDestroyContext(__GLcontext *gc);

#endif /* __glddxcx_h_ */
