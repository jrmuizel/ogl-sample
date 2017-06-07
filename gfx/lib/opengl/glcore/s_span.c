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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_span.c#4 $
*/
#include "context.h"
#include "global.h"

GLboolean __glReadSpan(__GLcolorBuffer *cfb, GLint x, GLint y,
		       __GLcolor *results, GLint w)
{
    __GLcontext *gc = cfb->buf.gc;

    while (--w >= 0) {
	(*cfb->readColor)(cfb, x, y, results);
	x++;
	results++;
    }

    return GL_FALSE;
}

/*
** NOTE: this is a hack.  Late in the game we determined that returning
** a span of data should not also blend.  So this code stacks the old
** blend enable value, disables blending, updates the pick procs, and
** then does the store.  Obviously this is a real slow thing to
** do.
*/
void __glReturnSpan(__GLcolorBuffer *cfb, GLint x, GLint y,
		    const __GLaccumCell *ac, __GLfloat scale, GLint w)
{
    __GLfragment frag;
    GLuint oldEnables;
    __GLcontext *gc = cfb->buf.gc;
    __GLfloat rscale, gscale, bscale, ascale;
    __GLaccumBuffer *afb = &gc->accumBuffer;

    /* Temporarily disable blending if its enabled */
    oldEnables = gc->state.enables.general;
    if (oldEnables & __GL_BLEND_ENABLE) {
	gc->state.enables.general &= ~__GL_BLEND_ENABLE;
	__GL_DELAY_VALIDATE(gc);
	(*gc->procs.validate)(gc);
    }

    rscale = scale * afb->oneOverRedScale;
    gscale = scale * afb->oneOverGreenScale;
    bscale = scale * afb->oneOverBlueScale;
    ascale = scale * afb->oneOverAlphaScale;

    frag.x = x;
    frag.y = y;
    while (--w >= 0) {
	frag.color[__GL_PRIMARY_COLOR].r = ac->r * rscale;
	frag.color[__GL_PRIMARY_COLOR].g = ac->g * gscale;
	frag.color[__GL_PRIMARY_COLOR].b = ac->b * bscale;
	frag.color[__GL_PRIMARY_COLOR].a = ac->a * ascale;
	__glClampRGBColor(cfb->buf.gc, &frag.color[__GL_PRIMARY_COLOR],
			  &frag.color[__GL_PRIMARY_COLOR]);
	(*cfb->store)(cfb, &frag);
	frag.x++;
	ac++;
    }

    /* Restore blending enable */
    if (oldEnables & __GL_BLEND_ENABLE) {
	gc->state.enables.general = oldEnables;
	__GL_DELAY_VALIDATE(gc);
	(*gc->procs.validate)(gc);
    }
}

GLboolean __glFetchSpan(__GLcontext *gc)
{
    __GLcolor *fcp;
    __GLcolorBuffer *cfb;
    GLint x, y;
    GLint w;

    w = gc->polygon.shader.length;

    fcp = gc->polygon.shader.fbcolors;
    cfb = gc->polygon.shader.cfb;
    x = gc->polygon.shader.frag.x;
    y = gc->polygon.shader.frag.y;
    (*cfb->readSpan)(cfb, x, y, fcp, w);

    return GL_FALSE;
}
