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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_btdraw.c#6 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "image.h"

#define __GL_BITS_PER_UINT32 (sizeof(GLuint) * __GL_BITS_PER_BYTE)

void __glDrawBitmap(__GLcontext *gc, GLsizei width, GLsizei height,
		    GLfloat xOrig, GLfloat yOrig, 
		    GLfloat xMove, GLfloat yMove, 
		    const GLubyte oldbits[])
{
    __GLbitmap bitmap;

    bitmap.width = width;
    bitmap.height = height;
    bitmap.xorig = xOrig;
    bitmap.yorig = yOrig;
    bitmap.xmove = xMove;
    bitmap.ymove = yMove;

    /* 
    ** Could check the pixel transfer modes and see if we can maybe just 
    ** render oldbits directly rather than converting it first.
    */
    if (width > 0 && height > 0) {
	GLubyte *newbits;

        newbits = (GLubyte *) (*gc->imports.malloc)(gc, (size_t)
				__glImageSize(width, height, GL_COLOR_INDEX, GL_BITMAP));
        
        __glFillImage(gc, width, height, GL_COLOR_INDEX, GL_BITMAP,
                      oldbits, newbits);

        (*gc->procs.renderBitmap)(gc, &bitmap, newbits);

        (*gc->imports.free)(gc, newbits);
    } else {
	/*
	** Nothing to draw, but we still need to update the current
	** raster position, and we might be in selection or feedback mode.
	*/
        (*gc->procs.renderBitmap)(gc, &bitmap, NULL);
    }
}

void __glRenderBitmap(__GLcontext *gc, const __GLbitmap *bitmap,
		      const GLubyte *data)
{
    __GLfragment frag;
    __GLvertex *rp;
    __GLfloat fx;
    GLint x, y, bit;
    GLint ySign;
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLint x0 = gc->transform.clipX0;
    GLint x1 = gc->transform.clipX1;
    GLint y0 = gc->transform.clipY0;
    GLint y1 = gc->transform.clipY1;

    ySign = gc->constants.ySign;

    /*
    ** Check if current raster position is valid.  Do not render if invalid.
    ** Also, if selection is in progress skip the rendering of the
    ** bitmap.  Bitmaps are invisible to selection and do not generate
    ** selection hits.
    */
    rp = &gc->state.current.rasterPos;
    if (!gc->state.current.validRasterPos) {
	return;
    }

    if (gc->renderMode == GL_SELECT) {
	rp->window.x += bitmap->xmove;
	rp->window.y += ySign * bitmap->ymove;
	return;
    }

    if (gc->renderMode == GL_FEEDBACK) {
	__glFeedbackBitmap(gc, rp);
	/*
	** Advance the raster position as if the bitmap had been rendered.
	*/
	rp->window.x += bitmap->xmove;
	rp->window.y += ySign * bitmap->ymove;
	return;
    }

    frag.color[__GL_PRIMARY_COLOR].r = rp->color->r * gc->constants.redRescale;
    frag.color[__GL_PRIMARY_COLOR].g = rp->color->g * gc->constants.greenRescale;
    frag.color[__GL_PRIMARY_COLOR].b = rp->color->b * gc->constants.blueRescale;
    frag.color[__GL_PRIMARY_COLOR].a = rp->color->a * gc->constants.alphaRescale;
    if (modeFlags & __GL_SHADE_TEXTURE) {
	__GLcoord strq[__GL_NUM_TEXTURE_UNITS];
	__GLfloat rhow[__GL_NUM_TEXTURE_UNITS];
	GLint i;

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    strq[i] = rp->texture[i];
	    rhow[i] = __glOne;
	}
	(*gc->procs.textureRaster)(gc,
			&frag.color[__GL_PRIMARY_COLOR], strq, rhow);
    }
    /* XXX - is this the correct test */
    if (gc->state.enables.general & __GL_FOG_ENABLE) {
	(*gc->procs.fogPoint)(gc, &frag, rp->eye.z);
    }

    __GL_LOCK_RENDER_BUFFERS(gc);

#if defined(__GL_HALF_PIXEL_OFFSET)
    fx = (GLint) ((rp->window.x - __glHalf) - bitmap->xorig);
    frag.y = (GLint) ((rp->window.y - __glHalf) - ySign * bitmap->yorig);
#else
    fx = (GLint) (rp->window.x - bitmap->xorig);
    frag.y = (GLint) (rp->window.y - ySign * bitmap->yorig);
#endif
    frag.z = rp->window.z * gc->constants.depthRescale;

    bit = 7;
    for (y = 0; y < bitmap->height; y++) {
	frag.x = fx;
	for (x = 0; x < bitmap->width; x++) {
	    if (y0 <= frag.y && frag.y < y1 && x0 <= frag.x && frag.x < x1) {
		if (*data & (1<<bit)) {
		    (*gc->procs.store)(gc->drawBuffer, &frag);
		}
	    }
	    frag.x++;
	    bit--;
	    if (bit < 0) {
		bit = 7;
		data++;
	    }
	}
	frag.y += ySign;
	if (bit != 7) {
	    bit = 7;
	    data++;
	}
    }

    __GL_UNLOCK_RENDER_BUFFERS(gc);

    /*
    ** Advance current raster position.
    */
    rp->window.x += bitmap->xmove;
    rp->window.y += ySign * bitmap->ymove;
}
