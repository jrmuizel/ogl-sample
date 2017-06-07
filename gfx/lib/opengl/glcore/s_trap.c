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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_trap.c#4 $
*/

#ifdef __GL_HW_NEEDS_SYNC

#include "context.h"

/*
** Functions that deal with the switching between software and hardware
** rendering.  Flushing may be needed in those cases.
*/

/*
** Here's what happens:
**
** When in hw mode, commands are conceivable queued up, and executed sometime
** in the future (for example in the case of DMA).  When switching from hw
** to sw mode, there is a need to flush all commands, so that we can access
** the buffers via software.  To avoid an extra 'if' statement, we use "trap"
** functions that are being set up during pick time.
**
** When we pick sw, a "SW" trap is being set in the place of the rendering
** routine.  This trap routine makes sure that no hw commands are pending,
** then gets out of the way by re-initializing the rendering routine
** appropriately.
**
** When we pick hw, a "HW" trap is being set.  This trap gets out of the
** way without waiting for commands to be flushed.
*/


/*
** first the SW routines
*/
void __glRenderPointSWTrap(__GLcontext *gc, __GLvertex *v)
{
    (*gc->procs.waitIdle)(gc);

    gc->procs.renderPoint = gc->procs.renderPointTrapBackup;
    gc->procs.renderLine = gc->procs.renderLineTrap;
    gc->procs.renderTriangle = gc->procs.renderTriangleTrap;

    (*gc->procs.renderPoint)(gc, v);
}

void __glRenderLineSWTrap(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1)
{
    (*gc->procs.waitIdle)(gc);

    gc->procs.renderPoint = gc->procs.renderPointTrap;
    gc->procs.renderLine = gc->procs.renderLineTrapBackup;
    gc->procs.renderTriangle = gc->procs.renderTriangleTrap;

    (*gc->procs.renderLine)(gc, v0, v1);
}

void __glRenderTriangleSWTrap(__GLcontext *gc, __GLvertex *v0,
				__GLvertex *v1, __GLvertex *v2)
{
    (*gc->procs.waitIdle)(gc);

    gc->procs.renderPoint = gc->procs.renderPointTrap;
    gc->procs.renderLine = gc->procs.renderLineTrap;
    gc->procs.renderTriangle = gc->procs.renderTriangleTrapBackup;

    (*gc->procs.renderTriangle)(gc, v0, v1, v2);
}

/*
** now the HW routines
*/
void __glRenderPointHWTrap(__GLcontext *gc, __GLvertex *v)
{
    gc->procs.renderPoint = gc->procs.renderPointTrapBackup;
    gc->procs.renderLine = gc->procs.renderLineTrap;
    gc->procs.renderTriangle = gc->procs.renderTriangleTrap;

    (*gc->procs.renderPoint)(gc, v);
}

void __glRenderLineHWTrap(__GLcontext *gc, __GLvertex *v0, __GLvertex *v1)
{
    gc->procs.renderPoint = gc->procs.renderPointTrap;
    gc->procs.renderLine = gc->procs.renderLineTrapBackup;
    gc->procs.renderTriangle = gc->procs.renderTriangleTrap;

    (*gc->procs.renderLine)(gc, v0, v1);
}

void __glRenderTriangleHWTrap(__GLcontext *gc, __GLvertex *v0,
				 __GLvertex *v1, __GLvertex *v2)
{
    gc->procs.renderPoint = gc->procs.renderPointTrap;
    gc->procs.renderLine = gc->procs.renderLineTrap;
    gc->procs.renderTriangle = gc->procs.renderTriangleTrapBackup;

    (*gc->procs.renderTriangle)(gc, v0, v1, v2);
}


#endif /* __GL_HW_NEEDS_SYNC */
