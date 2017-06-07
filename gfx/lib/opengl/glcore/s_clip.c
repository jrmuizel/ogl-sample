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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_clip.c#4 $
*/
#include "context.h"
#include "global.h"

/*
** Clipping macros.  These are used to reduce the amount of code
** hand written below.
*/
#define __GL_CLIP_POS(d,a,b,t) \
    d->clip.w = t*(a->clip.w - b->clip.w) + b->clip.w; \
    d->window.w = d->clip.w ? 1.0f / d->clip.w : 0.0f; \
    d->clip.x = t*(a->clip.x - b->clip.x) + b->clip.x; \
    d->clip.y = t*(a->clip.y - b->clip.y) + b->clip.y; \
    d->clip.z = t*(a->clip.z - b->clip.z) + b->clip.z; \
    d->flagBits = __GL_CLIP_MASK

/*
** The following is done this way since when we are slow fogging we want to
** clip the eye.z coordinate only, while when we are cheap fogging we want
** to clip the fog value.  This way we avoid doubling the number of clip
** routines.
*/
#define __GL_CLIP_FOG(d,a,b,t)	\
    if (a->flagBits & __GL_HAS_FOG) \
        d->fog = t * (a->fog - b->fog) + b->fog; \
    else \
        d->eye.z = t*(a->eye.z - b->eye.z) + b->eye.z

#define __GL_CLIP_COLOR(d,a,b,t)				      \
    d->colors[__GL_FRONTFACE].r = t*(a->colors[__GL_FRONTFACE].r      \
        - b->colors[__GL_FRONTFACE].r) + b->colors[__GL_FRONTFACE].r; \
    d->colors[__GL_FRONTFACE].g = t*(a->colors[__GL_FRONTFACE].g      \
        - b->colors[__GL_FRONTFACE].g) + b->colors[__GL_FRONTFACE].g; \
    d->colors[__GL_FRONTFACE].b = t*(a->colors[__GL_FRONTFACE].b      \
        - b->colors[__GL_FRONTFACE].b) + b->colors[__GL_FRONTFACE].b; \
    d->colors[__GL_FRONTFACE].a = t*(a->colors[__GL_FRONTFACE].a      \
        - b->colors[__GL_FRONTFACE].a) + b->colors[__GL_FRONTFACE].a

#define __GL_CLIP_INDEX(d,a,b,t)				     \
    d->colors[__GL_FRONTFACE].r = t*(a->colors[__GL_FRONTFACE].r     \
        - b->colors[__GL_FRONTFACE].r) + b->colors[__GL_FRONTFACE].r

#define __GL_CLIP_BACKCOLOR(d,a,b,t)				    \
    d->colors[__GL_BACKFACE].r = t*(a->colors[__GL_BACKFACE].r	    \
        - b->colors[__GL_BACKFACE].r) + b->colors[__GL_BACKFACE].r; \
    d->colors[__GL_BACKFACE].g = t*(a->colors[__GL_BACKFACE].g	    \
        - b->colors[__GL_BACKFACE].g) + b->colors[__GL_BACKFACE].g; \
    d->colors[__GL_BACKFACE].b = t*(a->colors[__GL_BACKFACE].b	    \
        - b->colors[__GL_BACKFACE].b) + b->colors[__GL_BACKFACE].b; \
    d->colors[__GL_BACKFACE].a = t*(a->colors[__GL_BACKFACE].a	    \
        - b->colors[__GL_BACKFACE].a) + b->colors[__GL_BACKFACE].a

#define __GL_CLIP_BACKINDEX(d,a,b,t)				   \
    d->colors[__GL_BACKFACE].r = t*(a->colors[__GL_BACKFACE].r	   \
        - b->colors[__GL_BACKFACE].r) + b->colors[__GL_BACKFACE].r

#define __GL_CLIP_TEXTURE(d,a,b,t) \
    { \
	GLint i; \
	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) { \
	    d->texture[i].x = \
		    t*(a->texture[i].x - b->texture[i].x) + b->texture[i].x; \
	    d->texture[i].y = \
		    t*(a->texture[i].y - b->texture[i].y) + b->texture[i].y; \
	    d->texture[i].z = \
		    t*(a->texture[i].z - b->texture[i].z) + b->texture[i].z; \
	    d->texture[i].w = \
		    t*(a->texture[i].w - b->texture[i].w) + b->texture[i].w; \
	} \
    }

#define __GL_CLIP_PROJSCALED_TEXTURE(d,a,b,t) \
    { \
	__GLfloat ax, ay, az, aw; \
	__GLfloat bx, by, bz, bw; \
	GLint i; \
	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) { \
	    if (!(a->flagBits & __GL_CLIP_MASK)) { \
		ax = a->texture[i].x * a->clip.w; \
		ay = a->texture[i].y * a->clip.w; \
		az = a->texture[i].z * a->clip.w; \
		aw = a->texture[i].w * a->clip.w; \
	    } else { \
		ax = a->texture[i].x; \
		ay = a->texture[i].y; \
		az = a->texture[i].z; \
		aw = a->texture[i].w; \
	    } \
	    if (!(b->flagBits & __GL_CLIP_MASK)) { \
		bx = b->texture[i].x * b->clip.w; \
		by = b->texture[i].y * b->clip.w; \
		bz = b->texture[i].z * b->clip.w; \
		bw = b->texture[i].w * b->clip.w; \
	    } else { \
		bx = b->texture[i].x; \
		by = b->texture[i].y; \
		bz = b->texture[i].z; \
		bw = b->texture[i].w; \
	    } \
	    d->texture[i].x = t*(ax - bx) + bx; \
	    d->texture[i].y = t*(ay - by) + by; \
	    d->texture[i].z = t*(az - bz) + bz; \
	    d->texture[i].w = t*(aw - bw) + bw; \
	} \
    }

/************************************************************************/

static void Clip(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
}

static void ClipC(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		  __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
}

static void ClipI(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		  __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
}

static void ClipBC(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		   __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_BACKCOLOR(dst,a,b,t);
}

static void ClipBI(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		   __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_BACKINDEX(dst,a,b,t);
}

static void ClipT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipIT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipBIT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_BACKINDEX(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipCT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipBCT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_BACKCOLOR(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipIPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipBIPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_BACKINDEX(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipCPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipBCPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_BACKCOLOR(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipF(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
}

static void ClipIF(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
}

static void ClipBIF(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_BACKINDEX(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
}

static void ClipCF(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
}

static void ClipBCF(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		 __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_BACKCOLOR(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
}

static void ClipFT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		   __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipIFT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		    __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipBIFT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		     __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_BACKINDEX(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipCFT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		    __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipBCFT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		     __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_BACKCOLOR(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_TEXTURE(dst,a,b,t);
}

static void ClipFPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		   __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipIFPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		    __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipBIFPT(__GLvertex *dst, const __GLvertex *a, 
		      const __GLvertex *b, __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_INDEX(dst,a,b,t);
    __GL_CLIP_BACKINDEX(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipCFPT(__GLvertex *dst, const __GLvertex *a, const __GLvertex *b,
		    __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void ClipBCFPT(__GLvertex *dst, const __GLvertex *a, 
		      const __GLvertex *b, __GLfloat t)
{
    __GL_CLIP_POS(dst,a,b,t);
    __GL_CLIP_COLOR(dst,a,b,t);
    __GL_CLIP_BACKCOLOR(dst,a,b,t);
    __GL_CLIP_FOG(dst,a,b,t);
    __GL_CLIP_PROJSCALED_TEXTURE(dst,a,b,t);
}

static void (*clipProcs[30])(__GLvertex*, const __GLvertex*, const __GLvertex*,
			     __GLfloat) =
{
    Clip, ClipI, ClipC, ClipBI, ClipBC,
    ClipF, ClipIF, ClipCF, ClipBIF, ClipBCF,
    ClipT, ClipIT, ClipCT, ClipBIT, ClipBCT,
    ClipFT, ClipIFT, ClipCFT, ClipBIFT, ClipBCFT,
    ClipPT, ClipIPT, ClipCPT, ClipBIPT, ClipBCPT,
    ClipFPT, ClipIFPT, ClipCFPT, ClipBIFPT, ClipBCFPT,
};

void __glGenericPickParameterClipProcs(__GLcontext *gc)
{
    GLint line = 0, poly = 0;
    GLuint enables = gc->state.enables.general;
    GLboolean twoSided = (enables & __GL_LIGHTING_ENABLE)
	&& gc->state.light.model.twoSided;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    if (gc->modes.rgbMode) {
	if (gc->state.light.shadingModel != GL_FLAT) {
	    line = 2;
	    if (twoSided) {
		poly = 4;
	    } else {
		poly = 2;
	    }
	}
    } else {
	if (gc->state.light.shadingModel != GL_FLAT) {
	    line = 1;
	    if (twoSided) {
		poly = 3;
	    } else {
		poly = 1;
	    }
	}
    }
    if ((modeFlags & __GL_SHADE_SLOW_FOG) ||
	    ((modeFlags & __GL_SHADE_CHEAP_FOG) && 
	    !(modeFlags & __GL_SHADE_SMOOTH_LIGHT))) {
	line += 5;
	poly += 5;
    }
    if (gc->texture.textureEnabled) { /*XXX - don't change this (see Derrick)*/
	if (modeFlags & __GL_SHADE_PROJSCALED_TEXTURE) {
	    line += 20;
	    poly += 20;
	} else {
	    line += 10;
	    poly += 10;
	}
    }
    gc->procs.lineClipParam = clipProcs[line];
    gc->procs.polyClipParam = clipProcs[poly];
}
