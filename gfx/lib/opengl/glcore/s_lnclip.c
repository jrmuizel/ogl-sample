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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_lnclip.c#8 $
*/
#include "math.h"
#include "context.h"
#include "global.h"

/*
** The following is a discussion of the math used to do edge clipping against
** a clipping plane.
** 
**     P1 is an end point of the edge
**     P2 is the other end point of the edge
** 
**     Q = t*P1 + (1 - t)*P2
**     That is, Q lies somewhere on the line formed by P1 and P2.
** 
**     0 <= t <= 1
**     This constrains Q to lie between P1 and P2.
** 
**     C is the plane equation for the clipping plane
** 
**     D1 = P1 dot C
**     D1 is the distance between P1 and C.  If P1 lies on the plane
**     then D1 will be zero.  The sign of D1 will determine which side
**     of the plane that P1 is on, with negative being outside.
** 
**     D2 = P2 dot C
**     D2 is the distance between P2 and C.  If P2 lies on the plane
**     then D2 will be zero.  The sign of D2 will determine which side
**     of the plane that P2 is on, with negative being outside.
** 
** Because we are trying to find the intersection of the P1 P2 line
** segment with the clipping plane we require that:
** 
**     Q dot C = 0
** 
** Therefore
** 
**     (t*P1 + (1 - t)*P2) dot C = 0
** 
**     (t*P1 + P2 - t*P2) dot C = 0
** 
**     t*P1 dot C + P2 dot C - t*P2 dot C = 0
** 
** Substituting D1 and D2 in
** 
**     t*D1 + D2 - t*D2 = 0
** 
** Solving for t
** 
**     t = -D2 / (D1 - D2)
** 
**     t = D2 / (D2 - D1)
*/

/*
** Clip a line against the frustum clip planes and any user clipping planes.
** If an edge remains after clipping then compute the window coordinates
** and invoke the renderer.
**
** Notice:  This algorithim is an example of an implementation that is
** different than what the spec says.  This is equivalent in functionality
** and meets the spec, but doesn't clip in eye space.  This clipper clips
** in NTVP (clip) space.
**
** Trivial accept/reject has already been dealt with.
*/
void __glClipLine(__GLcontext *gc, __GLvertex *a, __GLvertex *b)
{
    __GLvertex *provoking = b;
    __GLvertex np1, np2;
    __GLcoord *plane;
    GLuint needs, allClipCodes, clipCodes;
    void (*clip)(__GLvertex*, const __GLvertex*, const __GLvertex*, __GLfloat);
    __GLfloat zero;
    __GLfloat winx, winy;
    __GLfloat vpXCenter, vpYCenter, vpZCenter;
    __GLfloat vpXScale, vpYScale, vpZScale;
    __GLviewport *vp;
    __GLfloat x, y, z, wInv;

    /* Check for trivial pass of the line */
    needs = gc->vertex.faceNeeds[__GL_FRONTFACE] | __GL_HAS_CLIP;
    allClipCodes = (a->flagBits | b->flagBits) & __GL_CLIP_MASK;
    allClipCodes >>= __GL_CLIP_SHIFT;

    /*
    ** Have to clip.  Validate a&b before starting.  This might be
    ** wasted, but only in "rare?" cases.
    */
    if (~(a->flagBits) & needs) {
	DO_VALIDATE(gc, a, needs);
    }
    if (~(b->flagBits) & needs) {
	DO_VALIDATE(gc, b, needs);
    }

    /*
    ** For each clippling plane that something is out on, clip
    ** check the verticies.  Note that no bits will be set in
    ** allClipCodes for clip planes that are not enabled.
    */
    zero = __glZero;
    clip = gc->procs.lineClipParam;

    /* 
    ** Do user clip planes first, because we will maintain eye coordinates
    ** only while doing user clip planes.  They are ignored for the
    ** frustum clipping planes.
    */
    clipCodes = allClipCodes >> 6;
    if (clipCodes) {
	plane = &gc->state.transform.eyeClipPlanes[0];
	do {
	    /*
	    ** See if this clip plane has anything out of it.  If not,
	    ** press onward to check the next plane.  Note that we
	    ** shift this mask to the right at the bottom of the loop.
	    */
	    if (clipCodes & (__GL_CLIP_LEFT >> __GL_CLIP_SHIFT)) {
		__GLfloat t, d1, d2;

		d1 = (plane->x * a->eye.x) + (plane->y * a->eye.y) +
		     (plane->z * a->eye.z) + (plane->w * a->eye.w);
		d2 = (plane->x * b->eye.x) + (plane->y * b->eye.y) +
		     (plane->z * b->eye.z) + (plane->w * b->eye.w);
		if (d1 < zero) {
		    /* a is out */
		    if (d2 < zero) {
			/* a & b are out */
			return;
		    }

		    /*
		    ** A is out and B is in.  Compute new A coordinate
		    ** clipped to the plane.
		    */
		    t = d2 / (d2 - d1);
		    (*clip)(&np1, a, b, t);
		    np1.eye.x = t*(a->eye.x - b->eye.x) + b->eye.x;
		    np1.eye.y = t*(a->eye.y - b->eye.y) + b->eye.y;
		    np1.eye.z = t*(a->eye.z - b->eye.z) + b->eye.z;
		    np1.eye.w = t*(a->eye.w - b->eye.w) + b->eye.w;
		    a = &np1;
		    a->flagBits = 
		      (b->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
			__GL_VERTEX_0D_SIZE;
		    a->color = &a->colors[__GL_FRONTFACE];
		} else {
		    /* a is in */
		    if (d2 < zero) {
			/*
			** A is in and B is out.  Compute new B
			** coordinate clipped to the plane.
			**
			** NOTE: To avoid cracking in polygons with
			** shared clipped edges we always compute "t"
			** from the out vertex to the in vertex.  The
			** above clipping code gets this for free (b is
			** in and a is out).  In this code b is out and a
			** is in, so we reverse the t computation and the
			** argument order to (*clip).
			*/
			t = d1 / (d1 - d2);
			(*clip)(&np2, b, a, t);
			np2.eye.x = t*(b->eye.x - a->eye.x) + a->eye.x;
			np2.eye.y = t*(b->eye.y - a->eye.y) + a->eye.y;
			np2.eye.z = t*(b->eye.z - a->eye.z) + a->eye.z;
			np2.eye.w = t*(b->eye.w - a->eye.w) + a->eye.w;
			b = &np2;
			b->flagBits = 
			  (a->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
			    __GL_VERTEX_0D_SIZE;
			b->color = &b->colors[__GL_FRONTFACE];
		    } else {
			/* A and B are in */
		    }
		}
	    }
	    plane++;
	    clipCodes >>= 1;
	} while (clipCodes);
    }

    allClipCodes &= (__GL_CLIP_FRUSTUM_MASK >> __GL_CLIP_SHIFT);
    if (allClipCodes) {
	plane = &__gl_frustumClipPlanes[0];
	do {
	    /*
	    ** See if this clip plane has anything out of it.  If not,
	    ** press onward to check the next plane.  Note that we
	    ** shift this mask to the right at the bottom of the loop.
	    */
	    if (allClipCodes & (__GL_CLIP_LEFT >> __GL_CLIP_SHIFT)) {
		__GLfloat t, d1, d2;

		d1 = (plane->x * a->clip.x) + (plane->y * a->clip.y) +
		     (plane->z * a->clip.z) + (plane->w * a->clip.w);
		d2 = (plane->x * b->clip.x) + (plane->y * b->clip.y) +
		     (plane->z * b->clip.z) + (plane->w * b->clip.w);
		if (d1 < zero) {
		    /* a is out */
		    if (d2 < zero) {
			/* a & b are out */
			return;
		    }

		    /*
		    ** A is out and B is in.  Compute new A coordinate
		    ** clipped to the plane.
		    */
		    t = d2 / (d2 - d1);
		    (*clip)(&np1, a, b, t);
		    a = &np1;
		    a->flagBits = 
		      (b->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
			__GL_VERTEX_0D_SIZE;
		    a->color = &a->colors[__GL_FRONTFACE];
		} else {
		    /* a is in */
		    if (d2 < zero) {
			/*
			** A is in and B is out.  Compute new B
			** coordinate clipped to the plane.
			**
			** NOTE: To avoid cracking in polygons with
			** shared clipped edges we always compute "t"
			** from the out vertex to the in vertex.  The
			** above clipping code gets this for free (b is
			** in and a is out).  In this code b is out and a
			** is in, so we reverse the t computation and the
			** argument order to (*clip).
			*/
			t = d1 / (d1 - d2);
			(*clip)(&np2, b, a, t);
			b = &np2;
			b->flagBits =
			 (a->flagBits & ~__GL_VERTEX_SIZE_MASK) | 
			  __GL_VERTEX_0D_SIZE;
			b->color = &b->colors[__GL_FRONTFACE];
		    } else {
			/* A and B are in */
		    }
		}
	    }
	    plane++;
	    allClipCodes >>= 1;
	} while (allClipCodes);
    }

    vp = &gc->state.viewport;
    vpXCenter = vp->xCenter;
    vpYCenter = vp->yCenter;
    vpZCenter = vp->zCenter;
    vpXScale = vp->xScale;
    vpYScale = vp->yScale;
    vpZScale = vp->zScale;

    /* Compute window coordinates for both vertices. */
    wInv = a->clip.w ? __glOne / a->clip.w : __glZero;
    x = a->clip.x; 
    y = a->clip.y; 
    z = a->clip.z;
    winx = x * vpXScale * wInv + vpXCenter;
    winy = y * vpYScale * wInv + vpYCenter;
    a->window.z = z * vpZScale * wInv + vpZCenter;
    a->window.w = wInv;
    a->window.x = winx;
    a->window.y = winy;
    if ((a->flagBits & __GL_CLIP_MASK) &&
	gc->polygon.shader.modeFlags & __GL_SHADE_PROJSCALED_TEXTURE) {
	a->texture[0].x *= a->window.w;
	a->texture[0].y *= a->window.w;
	a->texture[0].w *= a->window.w;
    }

    wInv = b->clip.w ? __glOne / b->clip.w : __glZero;
    x = b->clip.x; 
    y = b->clip.y; 
    z = b->clip.z;
    winx = x * vpXScale * wInv + vpXCenter;
    winy = y * vpYScale * wInv + vpYCenter;
    b->window.z = z * vpZScale * wInv + vpZCenter;
    b->window.w = wInv;
    b->window.x = winx;
    b->window.y = winy;
    if ((b->flagBits & __GL_CLIP_MASK) &&
	gc->polygon.shader.modeFlags & __GL_SHADE_PROJSCALED_TEXTURE) {
	b->texture[0].x *= b->window.w;
	b->texture[0].y *= b->window.w;
	b->texture[0].w *= b->window.w;
    }

    /* Validate line state */
    if (gc->state.light.shadingModel == GL_FLAT) {
	GLuint pn = needs & (__GL_HAS_LIGHTING | __GL_HAS_FRONT_COLOR);

	/* Validate provoking vertex color */
	if (~provoking->flagBits & pn) {
	    DO_VALIDATE(gc, provoking, pn);
	}
	b->color = &provoking->colors[__GL_FRONTFACE];

	/* Validate a&b verticies.  Don't need color so strip it out */
	needs &= ~(__GL_HAS_LIGHTING | __GL_HAS_FRONT_COLOR);
	if (~a->flagBits & needs) DO_VALIDATE(gc, a, needs);
	if (~b->flagBits & needs) DO_VALIDATE(gc, b, needs);

	/* Draw the line then restore the b color pointer */
	(*gc->procs.renderLine)(gc, a, b);
	b->color = &b->colors[__GL_FRONTFACE];
    } else {
	if (~a->flagBits & needs) DO_VALIDATE(gc, a, needs);
	if (~b->flagBits & needs) DO_VALIDATE(gc, b, needs);

	(*gc->procs.renderLine)(gc, a, b);
    }
}

void __glFastClipSmoothLine(__GLcontext *gc, __GLvertex *a, __GLvertex *b)
{
    GLuint needs;

    if ((a->flagBits | b->flagBits) & __GL_CLIP_MASK) {
	/*
	** The line must be clipped more carefully.  Cannot trivially
	** accept the lines.
	*/
	if ((a->flagBits & b->flagBits & __GL_CLIP_MASK) != 0) {
	    /*
	    ** Trivially reject the line.  If anding the codes is non-zero then
	    ** every vertex in the line is outside of the same set of
	    ** clipping planes (at least one).
	    */
	    return;
	}
	__glClipLine(gc, a, b);
	return;
    }
    needs = gc->vertex.needs | __GL_HAS_FRONT_COLOR;
    DO_VALIDATE(gc, a, needs);
    DO_VALIDATE(gc, b, needs);
    (*gc->procs.renderLine)(gc, a, b);
}

void __glFastClipFlatLine(__GLcontext *gc, __GLvertex *a, __GLvertex *b)
{
    GLuint needs;

    if ((a->flagBits | b->flagBits) & __GL_CLIP_MASK) {
	/*
	** The line must be clipped more carefully.  Cannot trivially
	** accept the lines.
	*/
	if ((a->flagBits & b->flagBits & __GL_CLIP_MASK) != 0) {
	    /*
	    ** Trivially reject the line.  If anding the codes is non-zero then
	    ** every vertex in the line is outside of the same set of
	    ** clipping planes (at least one).
	    */
	    return;
	}
	__glClipLine(gc, a, b);
	return;
    }
    needs = gc->vertex.needs;

    /* 
    ** Validate a vertex.  Don't need color so strip it out.
    ** Note can't strip out __GL_HAS_LIGHTING because it will strip out
    ** __GL_HAS_EYE and eye.z is not exclusively used for lighting.
    */
    DO_VALIDATE(gc, a, needs & ~__GL_HAS_FRONT_COLOR);

    /* Validate provoking vertex color */
    DO_VALIDATE(gc, b, needs | __GL_HAS_FRONT_COLOR);

    /* Draw the line */
    (*gc->procs.renderLine)(gc, a, b);
}

void __glClipAndRenderLine(__GLcontext *gc, __GLvertex *a, __GLvertex *b)
{
    __GLdrawablePrivate *dp = gc->drawablePrivate;
    __GLdrawableRegion *glRegion;
    __GLregionRect *rect;

    /* 
    ** We need to lock before we get the cliprect, since it may change
    ** while rendering.  This is bad, since the actual triangle routine
    ** may be using the hw to render, but we can argue that the hardware
    ** is broken for not being able to handle this.
    */
    __GL_LOCK_DP(dp, gc);

    glRegion = &dp->ownershipRegion;
    rect = &glRegion->rects[0];

    /* in the common case, no clipping is necessery */
    if (1 == glRegion->numRects &&
	gc->transform.reasonableViewport &&
#if defined(__GL_ALIGNED_BUFFERS)
	rect->x0 == gc->transform.clipX0 - gc->constants.viewportXAdjustOffset &&
	rect->x1 == gc->transform.clipX1 - gc->constants.viewportXAdjustOffset &&
	rect->y0 == gc->transform.clipY0 - gc->constants.viewportYAdjustOffset &&
	rect->y1 == gc->transform.clipY1 - gc->constants.viewportYAdjustOffset
#else /* __GL_ALIGNED_BUFFERS */
	rect->x0 == gc->transform.clipX0 - gc->constants.viewportXAdjust &&
	rect->x1 == gc->transform.clipX1 - gc->constants.viewportXAdjust &&
	rect->y0 == gc->transform.clipY0 - gc->constants.viewportYAdjust &&
	rect->y1 == gc->transform.clipY1 - gc->constants.viewportYAdjust
#endif /* __GL_ALIGNED_BUFFERS */
	) {
	(*gc->procs.renderLine2)(gc, a, b);
	__GL_UNLOCK_DP(dp);
	return;
    }

    /* 
    ** We must have validated vertices to work with, so we have to do
    ** the same work as clip line.
    ** Note that we take a pessimistic view of what we'll need.  We could
    ** probably do better in the double sided case here...
    */
    {
	GLuint needs = gc->vertex.faceNeeds[__GL_FRONTFACE] |
	    gc->vertex.faceNeeds[__GL_BACKFACE];

	if (~a->flagBits & needs) DO_VALIDATE(gc, a, needs);
	if (~b->flagBits & needs) DO_VALIDATE(gc, b, needs);
    }

    /* Now Clip to each region in the list in turn */
    {
	__GLclipVertex cv[2];
	GLint i;

	for (i=0; i < glRegion->numRects; i++) {
	    rect = &glRegion->rects[i];

	    cv[0].v = *a;
	    cv[1].v = *b;

	    /* generate outcode for vertices */
	    __glCalc2DOutcode(gc, &cv[0], rect);
	    __glCalc2DOutcode(gc, &cv[1], rect);

	    if ((cv[0].outcode | cv[1].outcode) == 0) {
		/* Fully visible. Just draw it. */
		(*gc->procs.renderLine2)(gc, a, b);
	    } else if ((cv[0].outcode & cv[1].outcode) == 0) {
		/* Partially visible.  Have to do a 2D clip. */

		static const GLuint edge_flags[] = {
		    __GL_CLIP_LEFT, 
		    __GL_CLIP_RIGHT, 
		    __GL_CLIP_TOP, 
		    __GL_CLIP_BOTTOM
		};
		int tour;
		__GLclipVertex *a;
		__GLclipVertex *b;

		for (tour = 0; tour < 4; tour++) {
		    __GLclipVertex *newVertex;

		    /* trivial ordering of vertices */
		    a = &cv[0];
		    b = &cv[1];

		    if ((a->outcode ^ b->outcode) & edge_flags[tour]) {
			__GLfloat edge, t;

			if (tour < 2) {
			    /* swap vertices if necessary */
			    if ((a->v.window.x < b->v.window.x) ||
				((a->v.window.x == b->v.window.x) &&
				 (a->v.window.y < b->v.window.y))) {
				newVertex = a; a = b; b = newVertex;
			    }
			    if (tour == 0) {
				/* tour == 0 */
				edge = (__GLfloat)rect->x0;
#if defined(__GL_ALIGNED_BUFFERS)
				edge += gc->constants.fviewportXAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
				edge += gc->constants.fviewportXAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
				newVertex = b;
			    } else {
				/* tour == 1 */
				edge = (__GLfloat)rect->x1;
#if defined(__GL_ALIGNED_BUFFERS)
				edge += gc->constants.fviewportXAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
				edge += gc->constants.fviewportXAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
				newVertex = a;
			    }
			} else {
			    /* tour >= 2 */

			    /* swap vertices if necessary */
			    if ((a->v.window.y < b->v.window.y) ||
				((a->v.window.y == b->v.window.y) &&
				 (a->v.window.x < b->v.window.x))) {
				newVertex = a; a = b; b = newVertex;
			    }
			    if (tour == 2) {
				/* tour == 2 */
				edge = (__GLfloat)rect->y0;
#if defined(__GL_ALIGNED_BUFFERS)
				edge += gc->constants.fviewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
				edge += gc->constants.fviewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
				newVertex = b;
			    } else {
				/* tour == 3 */
				edge = (__GLfloat)rect->y1;
#if defined(__GL_ALIGNED_BUFFERS)
				edge += gc->constants.fviewportYAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
				edge += gc->constants.fviewportYAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
				newVertex = a;
			    }
			}

#if defined(__GL_HALF_PIXEL_OFFSET)
			edge += __glHalf;
#endif /* __GL_HALF_PIXEL_OFFSET */
			if (tour < 2) {
			    t = (b->v.window.x - edge) /
				(b->v.window.x - a->v.window.x);
			    if ((t < __glZero) || (t > __glOne)) continue;
			    newVertex->v.window.x = edge;
			    newVertex->v.window.y = b->v.window.y +
				t * (a->v.window.y - b->v.window.y);
			} else {
			    t = (b->v.window.y - edge) /
				(b->v.window.y - a->v.window.y);
			    if ((t < __glZero) || (t > __glOne)) continue;
			    newVertex->v.window.x = b->v.window.x +
				t * (a->v.window.x - b->v.window.x);
			    newVertex->v.window.y = edge;
			}
			newVertex->v.window.z = b->v.window.z +
			    t * (a->v.window.z - b->v.window.z);
			newVertex->outcode &= ~edge_flags[tour];
		    }
		}

		(*gc->procs.renderLine2)(gc, &a->v, &b->v);
	    }
	}
    }

    __GL_UNLOCK_DP(dp);
}
