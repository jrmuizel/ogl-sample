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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_feed.c#6 $
*/
#include "context.h"
#include "g_imfncs.h"
#include "global.h"

void GLAPI __glim_FeedbackBuffer(GLsizei bufferLength, GLenum type, GLfloat buffer[])
{
    __GL_SETUP_NOT_IN_BEGIN();

    if ((type < GL_2D) || (type > GL_4D_COLOR_TEXTURE)) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }
    if (bufferLength < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (gc->renderMode == GL_FEEDBACK) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    __GL_API_BLAND();

    gc->feedback.resultBase = buffer;
    gc->feedback.result = buffer;
    gc->feedback.resultLength = bufferLength;
    gc->feedback.overFlowed = GL_FALSE;
    gc->feedback.type = type;
}

void GLAPI __glim_PassThrough(GLfloat element)
{
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (gc->renderMode == GL_FEEDBACK) {
	__glFeedbackTag(gc, GL_PASS_THROUGH_TOKEN);
	__glFeedbackTag(gc, element);
    }
}

/************************************************************************/

void __glFeedbackTag(__GLcontext *gc, GLfloat f)
{
    if (!gc->feedback.overFlowed) {
	if (gc->feedback.result >=
		    gc->feedback.resultBase + gc->feedback.resultLength) {
	    gc->feedback.overFlowed = GL_TRUE;
	} else {
	    gc->feedback.result[0] = f;
	    gc->feedback.result = gc->feedback.result + 1;
	}
    }
}

static void Feedback(__GLcontext *gc, __GLvertex *v)
{
    GLenum type = gc->feedback.type;

    switch (type) {
      case GL_2D:
	__glFeedbackTag(gc, __glReturnWindowX(gc, v->window.x));
	__glFeedbackTag(gc, __glReturnWindowY(gc, v->window.y));
	break;
      case GL_3D:
      case GL_3D_COLOR:
      case GL_3D_COLOR_TEXTURE:
	__glFeedbackTag(gc, __glReturnWindowX(gc, v->window.x));
	__glFeedbackTag(gc, __glReturnWindowY(gc, v->window.y));
	__glFeedbackTag(gc, v->window.z * gc->constants.oneOverDepthScale);
	break;
      case GL_4D_COLOR_TEXTURE:
	__glFeedbackTag(gc, __glReturnWindowX(gc, v->window.x));
	__glFeedbackTag(gc, __glReturnWindowY(gc, v->window.y));
	__glFeedbackTag(gc, v->window.z * gc->constants.oneOverDepthScale);
	/*
	** NOTE: return clip.w, as window.w has no spec defined meaning.
	** It is true that this implementation uses window.w, but thats
	** something different.
	*/
	__glFeedbackTag(gc, v->clip.w);
	break;
    }
    switch (type) {
      case GL_3D_COLOR:
      case GL_3D_COLOR_TEXTURE:
      case GL_4D_COLOR_TEXTURE:
	{
	    __GLcolor *c = v->color;
	    if (gc->modes.rgbMode) {
		__glFeedbackTag(gc, c->r * gc->constants.oneOverRedScale);
		__glFeedbackTag(gc, c->g * gc->constants.oneOverGreenScale);
		__glFeedbackTag(gc, c->b * gc->constants.oneOverBlueScale);
		__glFeedbackTag(gc, c->a * gc->constants.oneOverAlphaScale);
	    } else {
		__glFeedbackTag(gc, c->r);
	    }
	}
	break;
      case GL_2D:
      case GL_3D:
        break;
    }
    switch (type) {
      case GL_3D_COLOR_TEXTURE:
      case GL_4D_COLOR_TEXTURE:
	{
	    GLuint modeFlags = gc->polygon.shader.modeFlags;
	    __GLfloat tx = v->texture[0].x;
	    __GLfloat ty = v->texture[0].y;
	    __GLfloat tw = v->texture[0].w;

	    /*
	    ** Make sure texture coordinates are valid too.  They may not
	    ** be in the needs flag if texturing is disabled.  By definition
	    ** feedback will return the texture coordinates whether or
	    ** not texturing is enabled.
	    */
	    if ((v->flagBits & __GL_HAS_TEXTURE) == 0) {
		DO_VALIDATE(gc, v, gc->vertex.needs | __GL_HAS_TEXTURE);
	    }

	    /* Undo projection */
	    if (modeFlags & __GL_SHADE_PROJSCALED_TEXTURE) {
		tx *= v->clip.w;
		ty *= v->clip.w;
		tw *= v->clip.w;
	    }

	    /* Undo scaling */
	    if (modeFlags & __GL_SHADE_UVSCALED_TEXTURE) {
		__GLtexture *tex = gc->texture.active->currentTexture;
		__GLmipMapLevel *lp = tex->level[0];

		tx /= lp->width2f;
		ty /= lp->height2f;
	    }

	    __glFeedbackTag(gc, tx);
	    __glFeedbackTag(gc, ty);
	    __glFeedbackTag(gc, v->texture[0].z);
	    __glFeedbackTag(gc, tw);
	}
	break;
      case GL_2D:
      case GL_3D:
      case GL_3D_COLOR:
	break;
    }
}

void __glFeedbackCopyPixels(__GLcontext *gc, __GLvertex *vx)
{
    __glFeedbackTag(gc, GL_COPY_PIXEL_TOKEN);
    Feedback(gc, vx);
}

void __glFeedbackDrawPixels(__GLcontext *gc, __GLvertex *vx)
{
    __glFeedbackTag(gc, GL_DRAW_PIXEL_TOKEN);
    Feedback(gc, vx);
}

void __glFeedbackBitmap(__GLcontext *gc, __GLvertex *vx)
{
    __glFeedbackTag(gc, GL_BITMAP_TOKEN);
    Feedback(gc, vx);
}

/* feedback version of renderPoint proc */
void __glFeedbackPoint(__GLcontext *gc, __GLvertex *vx)
{
    __glFeedbackTag(gc, GL_POINT_TOKEN);
    Feedback(gc, vx);
}

/* feedback version of renderLine proc */
void __glFeedbackLine(__GLcontext *gc, __GLvertex *a, __GLvertex *b)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    __GLcolor *oacp;

    oacp = a->color;
    if (!(modeFlags & __GL_SHADE_SMOOTH_LIGHT)) {
	a->color = b->color;
    }

    if (gc->line.notResetStipple) {
	__glFeedbackTag(gc, GL_LINE_TOKEN);
    } else {
	gc->line.notResetStipple = GL_TRUE;
	__glFeedbackTag(gc, GL_LINE_RESET_TOKEN);
    }
    Feedback(gc, a);
    Feedback(gc, b);

    a->color = oacp;
}

/* feedback version of renderTriangle proc */
void __glFeedbackTriangle(__GLcontext *gc, __GLvertex *a, __GLvertex *b,
			  __GLvertex *c)
{
    __GLfloat dxAC, dxBC, dyAC, dyBC, area;
    GLboolean ccw;
    GLint face;
    GLuint needs, modeFlags;

    /* Compute signed area of the triangle */
    dxAC = a->window.x - c->window.x;
    dxBC = b->window.x - c->window.x;
    dyAC = a->window.y - c->window.y;
    dyBC = b->window.y - c->window.y;
    area = dxAC * dyBC - dxBC * dyAC;
    ccw = area >= __glZero;

    /*
    ** Figure out if face is culled or not.  The face check needs to be
    ** based on the vertex winding before sorting.  This code uses the
    ** reversed flag to invert the sense of ccw - an xor accomplishes
    ** this conversion without an if test.
    **
    ** 		ccw	reversed		xor
    ** 		---	--------		---
    ** 		0	0			0 (remain !ccw)
    ** 		1	0			1 (remain ccw)
    ** 		0	1			1 (become ccw)
    ** 		1	1			0 (become cw)
    */
    face = gc->polygon.face[ccw];
    if (face == gc->polygon.cullFace) {
	/* Culled */
	return;
    }

    /*
    ** Pick face to use for coloring
    */
    modeFlags = gc->polygon.shader.modeFlags;
    needs = gc->vertex.needs;
    if (gc->state.light.shadingModel == GL_FLAT) {
	__GLvertex *pv = gc->vertex.provoking;
	GLuint pvneeds;
	GLuint faceNeeds;
	GLint colorFace;

	if (modeFlags & __GL_SHADE_TWOSIDED) {
	    colorFace = face;
	    faceNeeds = gc->vertex.faceNeeds[face];
	} else {
	    colorFace = __GL_FRONTFACE;
	    faceNeeds = gc->vertex.faceNeeds[__GL_FRONTFACE];
	}

	pv->color = &pv->colors[colorFace];
	a->color = pv->color;
	b->color = pv->color;
	c->color = pv->color;
	pvneeds = faceNeeds & (__GL_HAS_LIGHTING |
		__GL_HAS_FRONT_COLOR | __GL_HAS_BACK_COLOR);
	if (~pv->flagBits & pvneeds) {
	    DO_VALIDATE(gc, pv, pvneeds);
	}
    } else {
	GLint colorFace;

	if (modeFlags & __GL_SHADE_TWOSIDED) {
	    colorFace = face;
	    needs |= gc->vertex.faceNeeds[face];
	} else {
	    colorFace = __GL_FRONTFACE;
	    needs |= gc->vertex.faceNeeds[__GL_FRONTFACE];
	}

	a->color = &a->colors[colorFace];
	b->color = &b->colors[colorFace];
	c->color = &c->colors[colorFace];
    }
    if (~a->flagBits & needs) DO_VALIDATE(gc, a, needs);
    if (~b->flagBits & needs) DO_VALIDATE(gc, b, needs);
    if (~c->flagBits & needs) DO_VALIDATE(gc, c, needs);

    /* Deal with polygon face modes */
    switch (gc->polygon.mode[face]) {
      case __GL_POLYGON_MODE_POINT:
	if (a->flagBits & __GL_VERTEX_EDGE_FLAG) {
	    __glFeedbackTag(gc, GL_POINT_TOKEN);
	    Feedback(gc, a);
	}
	if (b->flagBits & __GL_VERTEX_EDGE_FLAG) {
	    __glFeedbackTag(gc, GL_POINT_TOKEN);
	    Feedback(gc, b);
	}
	if (c->flagBits & __GL_VERTEX_EDGE_FLAG) {
	    __glFeedbackTag(gc, GL_POINT_TOKEN);
	    Feedback(gc, c);
	}
	break;
      case __GL_POLYGON_MODE_LINE:
	if (a->flagBits & __GL_VERTEX_EDGE_FLAG) {
	    if (!gc->line.notResetStipple) {
		gc->line.notResetStipple = GL_TRUE;
		__glFeedbackTag(gc, GL_LINE_RESET_TOKEN);
	    } else {
		__glFeedbackTag(gc, GL_LINE_TOKEN);
	    }
	    Feedback(gc, a);
	    Feedback(gc, b);
	}
	if (b->flagBits & __GL_VERTEX_EDGE_FLAG) {
	    if (!gc->line.notResetStipple) {
		gc->line.notResetStipple = GL_TRUE;
		__glFeedbackTag(gc, GL_LINE_RESET_TOKEN);
	    } else {
		__glFeedbackTag(gc, GL_LINE_TOKEN);
	    }
	    Feedback(gc, b);
	    Feedback(gc, c);
	}
	if (c->flagBits & __GL_VERTEX_EDGE_FLAG) {
	    if (!gc->line.notResetStipple) {
		gc->line.notResetStipple = GL_TRUE;
		__glFeedbackTag(gc, GL_LINE_RESET_TOKEN);
	    } else {
		__glFeedbackTag(gc, GL_LINE_TOKEN);
	    }
	    Feedback(gc, c);
	    Feedback(gc, a);
	}
	break;
      case __GL_POLYGON_MODE_FILL:
	__glFeedbackTag(gc, GL_POLYGON_TOKEN);
	__glFeedbackTag(gc, 3);
	Feedback(gc, a);
	Feedback(gc, b);
	Feedback(gc, c);
	break;
    }

    /* Restore color pointers */
    a->color = &a->colors[__GL_FRONTFACE];
    b->color = &b->colors[__GL_FRONTFACE];
    c->color = &c->colors[__GL_FRONTFACE];
    if (gc->state.light.shadingModel == GL_FLAT) {
	__GLvertex *pv = gc->vertex.provoking;

	pv->color = &pv->colors[__GL_FRONTFACE];
    }
}
