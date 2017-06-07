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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_clear.c#6 $
*/
#include "context.h"
#include "global.h"
#include "g_imfncs.h"

void GLAPI __glim_Clear(GLbitfield mask)
{
    __GL_SETUP();
    GLuint beginMode;
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    GLint i;
#endif

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	    glClear(mask);
	    return;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
    }

    __GL_API_NOTBE_RENDER();

    if (mask & ~(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT
		 | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (gc->renderMode == GL_RENDER) {
	if (mask & GL_COLOR_BUFFER_BIT) {
	    switch (gc->state.raster.drawBuffer) {
	      case GL_NONE:
		break;
	      case GL_FRONT:
		(*gc->front->clear)(gc->front);
		break;
	      case GL_BACK:
		if (gc->modes.doubleBufferMode) {
		    (*gc->back->clear)(gc->back);
		}
		break;
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
	      case GL_AUX0:
	      case GL_AUX1:
	      case GL_AUX2:
	      case GL_AUX3:
		i = gc->state.raster.drawBuffer - GL_AUX0;
		if (i < gc->modes.numAuxBuffers) {
		    (*gc->auxBuffer[i].clear)(&gc->auxBuffer[i]);
		}
		break;
#endif
	      case GL_FRONT_AND_BACK:
		(*gc->front->clear)(gc->front);
		if (gc->modes.doubleBufferMode) {
		    (*gc->back->clear)(gc->back);
		}
		break;
	    }
	}
	if ((mask & GL_ACCUM_BUFFER_BIT) && gc->modes.haveAccumBuffer) {
	    (*gc->accumBuffer.clear)(&gc->accumBuffer);
	}
	if ((mask & GL_STENCIL_BUFFER_BIT) && gc->modes.haveStencilBuffer) {
	    (*gc->stencilBuffer.clear)(&gc->stencilBuffer);
	}
	if ((mask & GL_DEPTH_BUFFER_BIT) && gc->modes.haveDepthBuffer) {
	    (*gc->depthBuffer.clear)(&gc->depthBuffer);
	}
    }
}
