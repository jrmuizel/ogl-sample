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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_ptdraw.c#9 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "g_imports.h"

void __glShadeTextureFogPoint(__GLcontext *gc, __GLvertex *vx)
{
    GLint        i;
    GLuint       modeFlags = gc->polygon.shader.modeFlags;
    __GLcolor    color;
    __GLcoord    strq[__GL_NUM_TEXTURE_UNITS];
    __GLfloat    rhow[__GL_NUM_TEXTURE_UNITS];
    __GLfragment frag;
    
    /*
    ** Compute the color
    */
    color.r = vx->color->r;
    color.g = vx->color->g;
    color.b = vx->color->b;
    color.a = vx->color->a;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	strq[i] = vx->texture[i];
	rhow[i] = __glOne;
    }
    (*gc->procs.texture)(gc, &color, strq, rhow);

    if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
	__GLcolor *sc = vx->color + 2;

	color.r += sc->r;
	color.g += sc->g;
	color.b += sc->b;
	color.a += sc->a;
    }

    (*gc->procs.fogColor)(gc, &frag.color[__GL_PRIMARY_COLOR],
			  &color, vx->fog);

    (*gc->procs.renderPoint2)(gc, vx, &frag);
}


void __glShadeTexturePoint(__GLcontext *gc, __GLvertex *vx)
{
    GLint        i;
    GLuint       modeFlags = gc->polygon.shader.modeFlags;
    __GLfragment frag;
    __GLcolor    *fragColor = &frag.color[__GL_PRIMARY_COLOR];
    __GLcoord    strq[__GL_NUM_TEXTURE_UNITS];
    __GLfloat    rhow[__GL_NUM_TEXTURE_UNITS];
    
    /*
    ** Compute the color
    */
    fragColor->r = vx->color->r;
    fragColor->g = vx->color->g;
    fragColor->b = vx->color->b;
    fragColor->a = vx->color->a;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	strq[i] = vx->texture[i];
	rhow[i] = __glOne;
    }
    (*gc->procs.texture)(gc, fragColor, strq, rhow);

    if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
	__GLcolor *sc = vx->color + 2;

	fragColor->r += sc->r;
	fragColor->g += sc->g;
	fragColor->b += sc->b;
	fragColor->a += sc->a;
    }
    
    (*gc->procs.renderPoint2)(gc, vx, &frag);
}


void __glShadeFogPoint(__GLcontext *gc, __GLvertex *vx)
{
    __GLfragment  frag;
    __GLcolor     color;
    GLuint        modeFlags = gc->polygon.shader.modeFlags;

    color.r = vx->color->r;

    if (modeFlags & __GL_SHADE_RGB) {
	color.g = vx->color->g;
	color.b = vx->color->b;
	color.a = vx->color->a;

	if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
	    __GLcolor *sc = vx->color+2;

	    color.r += sc->r;
	    color.g += sc->g;
	    color.b += sc->b;
	    color.a += sc->a;
	}
    }

    (gc->procs.fogColor)(gc, &frag.color[__GL_PRIMARY_COLOR], &color,
			 vx->fog);

    (*gc->procs.renderPoint2)(gc, vx, &frag);
}


void __glShadePoint(__GLcontext *gc, __GLvertex *vx)
{
    GLuint       modeFlags = gc->polygon.shader.modeFlags;
    __GLfragment frag;
    __GLcolor    *color = &frag.color[__GL_PRIMARY_COLOR];
   
    /*
    ** Compute the color
    */
    color->r = vx->color->r;
    if (modeFlags & __GL_SHADE_RGB) {
	color->g = vx->color->g;
	color->b = vx->color->b;
	color->a = vx->color->a;

	if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
	    __GLcolor *sc = vx->color + 2;

	    color->r += sc->r;
	    color->g += sc->g;
	    color->b += sc->b;
	    color->a += sc->a;
	}
    }

    /* Render the single point */
    (*gc->procs.renderPoint2)(gc, vx, &frag);
}


void __glRenderAliasedPoint1(__GLcontext *gc, __GLvertex *vx,
			     __GLfragment *frag)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    __GL_ADJUST_POINT_VERTEX(gc, vx);

    frag->x = (GLint) (vx->window.x - __glHalf);
    frag->y = (GLint) (vx->window.y - __glHalf);
    frag->z = (__GLzValue)vx->window.z;

    /* Render the single point */
    (*gc->procs.store)(gc->drawBuffer, frag);

    __GL_RESTORE_POINT_VERTEX(gc, vx);
}


void __glRenderAliasedPointN(__GLcontext *gc, __GLvertex *vx,
			     __GLfragment *frag)
{
    GLint pointSize, pointSizeHalf, ix, iy, xLeft, xRight, yBottom, yTop;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    void (*store)(__GLcolorBuffer *, const __GLfragment *) = gc->procs.store;

    __GL_ADJUST_POINT_VERTEX(gc, vx);

    if (gc->procs.store == gc->procs.cfbStore) {
	/* We need ownership testing */
	store = __glDoStore;
    }

    /*
    ** Compute the x and y starting coordinates for rendering the square.
    */
    pointSize = gc->state.point.aliasedSize;
    pointSizeHalf = pointSize >> 1;
    if (pointSize & 1) {
	/* odd point size */
	xLeft = (((GLint)(vx->window.x - __glHalf)) - pointSizeHalf);
	yBottom = (((GLint)(vx->window.y - __glHalf)) - pointSizeHalf);
    } else {
	/* even point size */
	xLeft = (((GLint)(vx->window.x)) - pointSizeHalf);
	yBottom = (((GLint)(vx->window.y)) - pointSizeHalf);
    }
    xRight = xLeft + pointSize;
    yTop = yBottom + pointSize;

    /*
    ** Now render the square centered on xCenter,yCenter.
    */
    frag->z = (__GLzValue)vx->window.z;
    for (iy = yBottom; iy < yTop; iy++) {
	for (ix = xLeft; ix < xRight; ix++) {
	    frag->x = ix;
	    frag->y = iy;
	    frag->z = (__GLzValue)vx->window.z;
	    (*store)(gc->drawBuffer, frag);
	}
    }

    __GL_RESTORE_POINT_VERTEX(gc, vx);
}
