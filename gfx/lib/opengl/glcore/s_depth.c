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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_depth.c#12 $
*/
#include "context.h"
#include "global.h"
#if defined(__GL_USE_ASMCODE) && defined(i386)
#include "i386.h"
#endif

static __GLzValue FetchFastClear(__GLdepthBuffer *fb, GLint x, GLint y)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    GLuint cnt = dbDepth->cnt;
    GLuint zClearInc = dbDepth->zClearInc;
    GLuint zDepthMask = dbDepth->zDepthMask;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);

    if( (fp[0] & ~zDepthMask) != cnt ) {
	/* 
	** big mess..  If the buffer is not from the current frame,
	** then we have to check the z clear value and return that to
	** the user.  Luckily, the clear value is only going to be 0.0 or
	** 1.0, otherwise we woudn't be doing accelerated z-buffering
	*/
	if( zClearInc == 0xff000000 ) {
	    if( gc->state.depth.clear == 1.0 ) {
		if (dbDepth->invertZRange) {
		    return 0x00000000;
		} else {
		    return fb->scale;
		}
	    }
	}
	if( zClearInc == 0x01000000 ) {
	    if( gc->state.depth.clear == 0.0 ) {
		if (dbDepth->invertZRange) {
		    return fb->scale;
		} else {
		    return 0x00000000;
		}
	    }
	}

	/* othwise, return the value there */
    }

    /* ignore frame count, and shift appropriately */
    return ( fp[0] & zDepthMask ) << dbDepth->numFracBits; 
}

static __GLzValue Fetch(__GLdepthBuffer *fb, GLint x, GLint y)
{
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLzValue *fp;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return fp[0];
}

/* ARGSUSED */
static GLboolean StoreNEVER(__GLdepthBuffer *fb,
			    GLint x, GLint y, __GLzValue z)
{
    return GL_FALSE;
}

static GLboolean StoreLESS(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    if ( (GLuint)Z < (GLuint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreLESS_s(__GLdepthBuffer *fb,
			     GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue *), x, y);
    if( (GLint)Z < (GLint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreEQUAL(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    if (Z == fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreLEQUAL(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    if ( (GLuint)Z <= (GLuint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreLEQUAL_s(__GLdepthBuffer *fb,
			       GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue *), x, y);
    if( (GLint)Z <= (GLint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGREATER(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    if ( (GLuint)Z > (GLuint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGREATER_s(__GLdepthBuffer *fb,
				GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue *), x, y);
    if( (GLint)Z > (GLint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreNOTEQUAL(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    if ( Z != fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGEQUAL(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    if ( (GLuint)Z >=  (GLuint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGEQUAL_s(__GLdepthBuffer *fb,
			       GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue *), x, y);
    if( (GLint)Z >= (GLint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreALWAYS(__GLdepthBuffer *fb,
			     GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    fp[0] = (z >> dbDepth->numFracBits) + dbDepth->cnt;
    return GL_TRUE;
}

/************************************************************************/

static GLboolean StoreLESS_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLuint)Z < (GLuint)fp[0] );
}

static GLboolean StoreLESS_W_s(__GLdepthBuffer *fb,
			       GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLint)Z < (GLint)fp[0] );
}

static GLboolean StoreEQUAL_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLuint)Z == (GLuint)fp[0] );
}

static GLboolean StoreLEQUAL_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLuint)Z <= (GLuint)fp[0] );
}

static GLboolean StoreLEQUAL_W_s(__GLdepthBuffer *fb,
				 GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLint)Z <= (GLint)fp[0] );
}

static GLboolean StoreGREATER_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLuint)Z > (GLuint)fp[0] );
}

static GLboolean StoreGREATER_W_s(__GLdepthBuffer *fb,
				  GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLint)Z > (GLint)fp[0] );
}


static GLboolean StoreNOTEQUAL_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( Z != fp[0] );
}

static GLboolean StoreGEQUAL_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLuint)Z >= (GLuint)fp[0] );
}

static GLboolean StoreGEQUAL_W_s(__GLdepthBuffer *fb,
				 GLint x, GLint y, __GLzValue z)
{
    __GLzValue *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue*), x, y);
    return ( (GLint)Z >= (GLint)fp[0] );
}

/* ARGSUSED */
static GLboolean StoreALWAYS_W(__GLdepthBuffer *fb,
			     GLint x, GLint y, __GLzValue z)
{
    return GL_TRUE;
}

/************************************************************************/

static __GLzValue Fetch16(__GLdepthBuffer *fb, GLint x, GLint y)
{
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLzValue16 *fp;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return fp[0];
}

/* ARGSUSED */
static GLboolean StoreNEVER16(__GLdepthBuffer *fb,
			    GLint x, GLint y, __GLzValue z)
{
    return GL_FALSE;
}

static GLboolean StoreLESS16(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if ( (GLuint)Z < (GLuint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreLESS16_s(__GLdepthBuffer *fb,
			     GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if( (GLint)Z < (GLint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreEQUAL16(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if (Z == fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreLEQUAL16(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if ( (GLuint)Z <= (GLuint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreLEQUAL16_s(__GLdepthBuffer *fb,
			       GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if( (GLint)Z <= (GLint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGREATER16(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if ( (GLuint)Z > (GLuint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGREATER16_s(__GLdepthBuffer *fb,
				GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if( (GLint)Z > (GLint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreNOTEQUAL16(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if ( Z != fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGEQUAL16(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if ( (GLuint)Z >=  (GLuint)fp[0]) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreGEQUAL16_s(__GLdepthBuffer *fb,
			       GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    if( (GLint)Z >= (GLint)fp[0] ) {
	fp[0] = Z;
	return GL_TRUE;
    }
    return GL_FALSE;
}

static GLboolean StoreALWAYS16(__GLdepthBuffer *fb,
			     GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    fp[0] = (z >> dbDepth->numFracBits) + dbDepth->cnt;
    return GL_TRUE;
}

/************************************************************************/

static GLboolean StoreLESS16_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLuint)Z < (GLuint)fp[0] );
}

static GLboolean StoreLESS16_W_s(__GLdepthBuffer *fb,
			       GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLint)Z < (GLint)fp[0] );
}

static GLboolean StoreEQUAL16_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLuint)Z == (GLuint)fp[0] );
}

static GLboolean StoreLEQUAL16_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLuint)Z <= (GLuint)fp[0] );
}

static GLboolean StoreLEQUAL16_W_s(__GLdepthBuffer *fb,
				 GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLint)Z <= (GLint)fp[0] );
}

static GLboolean StoreGREATER16_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLuint)Z > (GLuint)fp[0] );
}

static GLboolean StoreGREATER16_W_s(__GLdepthBuffer *fb,
				  GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLint)Z > (GLint)fp[0] );
}


static GLboolean StoreNOTEQUAL16_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( Z != fp[0] );
}

static GLboolean StoreGEQUAL16_W(__GLdepthBuffer *fb,
			   GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLuint)Z >= (GLuint)fp[0] );
}

static GLboolean StoreGEQUAL16_W_s(__GLdepthBuffer *fb,
				 GLint x, GLint y, __GLzValue z)
{
    __GLzValue16 *fp;
    __GLcontext *gc = fb->buf.gc;
    __GLdrawableBuffer *drawableBuf = fb->buf.drawableBuf;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(fb);
    __GLzValue16 Z = (z >> dbDepth->numFracBits) + dbDepth->cnt;

    fp = __GL_DEPTH_ADDR(gc, fb, drawableBuf, (__GLzValue16*), x, y);
    return ( (GLint)Z >= (GLint)fp[0] );
}

/* ARGSUSED */
static GLboolean StoreALWAYS16_W(__GLdepthBuffer *fb,
			     GLint x, GLint y, __GLzValue z)
{
    return GL_TRUE;
}

/************************************************************************/

/*
** Operations which affect the fast depth clear state machine:
**   depth test enable
**   depth function
**   depth range
**   depth write mask
**   depth clear value
**   scissor enable changed
**   scissor box changed
**   make current
**   lose current
**   read depth buffer
**   draw depth buffer
**   clear depth buffer
*/

/*
**  count   val     inc     func    lt      gt      range
**  -------------------------------------------------------
**  127     1.0     -1      <       -       invert  254-127
**  0       1.0     -1      <       -       invert  127-0
**  128     0.0     +1      >       invert  -       128-1
**  255     0.0     +1      >       invert  -       255-128
*/

void __glValidateZCount(__GLdepthBuffer *dfb)
{
    __GLcontext *gc = dfb->buf.gc;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(dfb);

    if (dbDepth->zClearInc == 0xff000000) {
	if (dbDepth->cnt == 0xff000000) {
	    dbDepth->cnt = 0x80000000;
	    dbDepth->zClearInc = 0x01000000;
	    switch (gc->state.depth.testFunc) {
	    case GL_LESS:
		dfb->testFunc = GL_GREATER;
		dbDepth->invertZRange = GL_TRUE;
		break;
	    case GL_LEQUAL:
		dfb->testFunc = GL_GEQUAL;
		dbDepth->invertZRange = GL_TRUE;
		break;
	    case GL_GREATER:
		dfb->testFunc = GL_GREATER;
		dbDepth->invertZRange = GL_FALSE;
		break;
	    case GL_GEQUAL:
		dfb->testFunc = GL_GEQUAL;
		dbDepth->invertZRange = GL_FALSE;
		break;
	    default:
		dfb->testFunc = gc->state.depth.testFunc;
		dbDepth->invertZRange = GL_FALSE;
		break;
	    }
	    __glUpdateDepthRange(gc);
	    /* changed testFunc, have to re-pick rendering procs */
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);
	}
    } else if (dbDepth->zClearInc == 0x01000000) {
	if (dbDepth->cnt == 0x00000000) {
	    dbDepth->cnt = 0x7f000000;
	    dbDepth->zClearInc = 0xff000000;
	    switch (gc->state.depth.testFunc) {
	    case GL_LESS:
		dfb->testFunc = GL_LESS;
		dbDepth->invertZRange = GL_FALSE;
		break;
	    case GL_LEQUAL:
		dfb->testFunc = GL_LEQUAL;
		dbDepth->invertZRange = GL_FALSE;
		break;
	    case GL_GREATER:
		dfb->testFunc = GL_LESS;
		dbDepth->invertZRange = GL_TRUE;
		break;
	    case GL_GEQUAL:
		dfb->testFunc = GL_LEQUAL;
		dbDepth->invertZRange = GL_TRUE;
		break;
	    default:
		dfb->testFunc = gc->state.depth.testFunc;
		dbDepth->invertZRange = GL_FALSE;
		break;
	    }
	    __glUpdateDepthRange(gc);
	    /* changed testFunc, have to re-pick rendering procs */
	    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);
	}
    } else {
	dfb->testFunc = gc->state.depth.testFunc;
	dbDepth->invertZRange = GL_FALSE;
    }

}

static __GLzValue DepthClearVal(__GLdepthBuffer *dfb)
{
    __GLcontext *gc = dfb->buf.gc;
    __GLdrawableDepthBuffer *dbDepth = __GL_DEPTH_DP(dfb);
    __GLzValue z;

    __glValidateZCount(dfb);

    /* create the z value to be put in the frame buffer */
    if (dbDepth->invertZRange) {
	z = (__GLzValue)((1.0 - gc->state.depth.clear) * dfb->scale);
    } else {
	z = (__GLzValue)(gc->state.depth.clear * dfb->scale);
    }
    z = (z >> dbDepth->numFracBits) + dbDepth->cnt;
    return z;
}

static void Clear(__GLdepthBuffer *dfb)
{
    __GLcontext *gc = dfb->buf.gc;
    __GLdrawableBuffer *drawableBuf;
    __GLdrawableDepthBuffer *dbDepth;
    __GLzValue *fb;
    __GLzValue z;
    GLint x, y, x1, y1, w, h, skip;

    if (!gc->state.depth.writeEnable) {
	return;
    }

    __GL_LOCK_DP(gc->drawablePrivate, gc);

    x = gc->transform.clipX0;
    y = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;
    if (((w = x1 - x) == 0) || ((h = y1 - y) == 0)) {
	__GL_UNLOCK_DP(gc->drawablePrivate);
	return;
    }

    drawableBuf = dfb->buf.drawableBuf;
    dbDepth = __GL_DEPTH_DP(dfb);

    if(dbDepth->fullZClear || !dbDepth->allowPartialZClear ) {
	/* reset the count to a first frame */
	if (dbDepth->zClearInc != 0x00000000) {
	    dbDepth->cnt = ~(dbDepth->zDepthMask);
	}

	z = DepthClearVal(dfb);

	/* now do the rest */

#if !defined(__GL_CORE_CLEARS_BUFFERS)
        if (dfb->buf.drawableBuf->fill) {
	    __GL_UNLOCK_DP(gc->drawablePrivate);
	     x -= gc->constants.viewportXAdjust;
	     y -= gc->constants.viewportYAdjust;
             (*drawableBuf->fill)(drawableBuf, gc->drawablePrivate, 
				  z, x, y, w, h);
        } else
#endif /* __GL_CORE_CLEARS_BUFFERS */
	{
	    /* cancel the full clear for next frame */
	    dbDepth->fullZClear = GL_FALSE;

	    __GL_LOCK_BUFFER_MASK(gc, __GL_DEPTH_BUFFER_MASK);

	    fb = __GL_DEPTH_ADDR(gc, dfb, drawableBuf, (__GLzValue*), x, y);
	    skip = drawableBuf->outerWidth - w;

#if defined(__GL_USE_ASMCODE) && defined(i386)
	    if(drawableBuf->elementSize == 2) {
	        __glI386ClearMemAlign2(gc,
			  (unsigned short*)fb, w, h, 
			  skip*sizeof(unsigned short), z);
       	    } else {
	        __glI386ClearMemAlign4(gc,
			  (unsigned long *)fb, w, h, 
			  skip*sizeof(unsigned long), z);
            }
#else /* __GL_USE_ASMCODE && i386 */
	    if (drawableBuf->elementSize == 2) {
		__GLzValue16 *fb16 = (__GLzValue16 *) fb;
		GLint w32, w4, w1;

		w32 = w >> 5;
		w4 = (w & 31) >> 2;
		w1 = w & 3;

		for (; y < y1; y++) {
		    w = w32;
		    while (--w >= 0) {
			fb16[0] = z; fb16[1] = z; fb16[2] = z; fb16[3] = z;
			fb16[4] = z; fb16[5] = z; fb16[6] = z; fb16[7] = z;
			fb16[8] = z; fb16[9] = z; fb16[10] = z; fb16[11] = z;
			fb16[12] = z; fb16[13] = z; fb16[14] = z; fb16[15] = z;
			fb16[16] = z; fb16[17] = z; fb16[18] = z; fb16[19] = z;
			fb16[20] = z; fb16[21] = z; fb16[22] = z; fb16[23] = z;
			fb16[24] = z; fb16[25] = z; fb16[26] = z; fb16[27] = z;
			fb16[28] = z; fb16[29] = z; fb16[30] = z; fb16[31] = z;
			fb16 += 32;
		    }
		    w = w4;
		    while (--w >= 0) {
			fb16[0] = z; fb16[1] = z; fb16[2] = z; fb16[3] = z;
			fb16 += 4;
		    }
		    w = w1;
		    while (--w >= 0) {
			*fb16++ = z;
		    }
		    fb16 += skip;
		}
	    } else {
		GLint w32, w4, w1;

		w32 = w >> 5;
		w4 = (w & 31) >> 2;
		w1 = w & 3;

		for (; y < y1; y++) {
		    w = w32;
		    while (--w >= 0) {
			fb[0] = z; fb[1] = z; fb[2] = z; fb[3] = z;
			fb[4] = z; fb[5] = z; fb[6] = z; fb[7] = z;
			fb[8] = z; fb[9] = z; fb[10] = z; fb[11] = z;
			fb[12] = z; fb[13] = z; fb[14] = z; fb[15] = z;
			fb[16] = z; fb[17] = z; fb[18] = z; fb[19] = z;
			fb[20] = z; fb[21] = z; fb[22] = z; fb[23] = z;
			fb[24] = z; fb[25] = z; fb[26] = z; fb[27] = z;
			fb[28] = z; fb[29] = z; fb[30] = z; fb[31] = z;
			fb += 32;
		    }
		    w = w4;
		    while (--w >= 0) {
			fb[0] = z; fb[1] = z; fb[2] = z; fb[3] = z;
			fb += 4;
		    }
		    w = w1;
		    while (--w >= 0) {
			*fb++ = z;
		    }
		    fb += skip;
		}
	    }
#endif /* __GL_USE_ASMCODE && i386 */
	    __GL_UNLOCK_BUFFER_MASK(gc, __GL_DEPTH_BUFFER_MASK);
	    __GL_UNLOCK_DP(gc->drawablePrivate);
	}
    } else {
	/* partial buffer clear */
	GLint dx, dy;
	GLint np, ndp;
	GLint indx;
	GLint start_pix, end_pix;
	GLint start_line, end_line;

	/* increment the counter */
	dbDepth->cnt += dbDepth->zClearInc;

	z = DepthClearVal(dfb);

	/* now do the rest */

	/* figure out scanline for starting point */
	dx = x1 - x;
	dy = y1 - y;
	np = dx * dy;	/* number of pixels in window */
	ndp = np >> 7;	/* number of drawable pixels in wind in this itertn */
	indx = (dbDepth->cnt >> 24) & 0x7f;/* which iteration is this */

	start_pix = ndp * indx;		/* starting pixel */
	end_pix = start_pix + ndp;	/* ending pixel */

	start_line = y + start_pix / dx; /* truncate to form starting line */
	end_line = y + end_pix / dx;	/* truncate to form ending line */

	/* make sure lines don't access beyond the end of the buffer */
	if( start_line >= y1 ) start_line = y1-1;
	if( end_line >= y1 ) end_line = y1-1;

	/* if we should, clear a couple of scanlines */
	if( start_line != end_line ) {
	    __GL_LOCK_BUFFER_MASK(gc, __GL_DEPTH_BUFFER_MASK);

	    fb = __GL_DEPTH_ADDR(gc, dfb, drawableBuf, (__GLzValue *), 
				 x, start_line );

	    skip = drawableBuf->outerWidth - w;
#if defined(__GL_USE_ASMCODE) && defined(i386)
	    /* If we decide to support 16 bit incremental clears,
	       generalize to call __glI386MemClear2(). */
	    h = (end_line - start_line) + 1;
	    __glI386ClearMemAlign4(gc,
			  (unsigned long *) fb, w, h, 
			  skip*sizeof(unsigned long), z);
#else /* __GL_USE_ASMCODE && i386 */
	    {
		GLint w32, w4, w1;

		w32 = w >> 5;
		w4 = (w & 31) >> 2;
		w1 = w & 3;

		for( ; start_line < end_line; start_line++ ) {
		    w = w32;
		    while (--w >= 0) {
			fb[0] = z; fb[1] = z; fb[2] = z; fb[3] = z;
			fb[4] = z; fb[5] = z; fb[6] = z; fb[7] = z;
			fb[8] = z; fb[9] = z; fb[10] = z; fb[11] = z;
			fb[12] = z; fb[13] = z; fb[14] = z; fb[15] = z;
			fb[16] = z; fb[17] = z; fb[18] = z; fb[19] = z;
			fb[20] = z; fb[21] = z; fb[22] = z; fb[23] = z;
			fb[24] = z; fb[25] = z; fb[26] = z; fb[27] = z;
			fb[28] = z; fb[29] = z; fb[30] = z; fb[31] = z;
			fb += 32;
		    }
		    w = w4;
		    while (--w >= 0) {
			fb[0] = z; fb[1] = z; fb[2] = z; fb[3] = z;
			fb += 4;
		    }
		    w = w1;
		    while (--w >= 0) {
			*fb++ = z;
		    }

		    fb += skip;
		}
	    }
#endif /* __GL_USE_ASMCODE && i386 */
	    __GL_UNLOCK_BUFFER_MASK(gc, __GL_DEPTH_BUFFER_MASK);
	    __GL_UNLOCK_DP(gc->drawablePrivate);
	}

    }
}

/************************************************************************/

/*
** adjust the range of the z-buffer so that things work whenever
** we switch z-buffer clearing direction or scissor rect.
*/
static void Fix_zbuffer(__GLcontext *gc, __GLdepthBuffer *dfb, GLuint destRange)
{
    __GLdrawableBuffer *drawableBuf;
    __GLdrawableDepthBuffer *dbDepth;
    GLuint cnt;
    GLuint zDepthMask;
    GLuint zClearInc;
    GLuint clearVal;
    __GLzValue *p;
    GLint x, y, x1, y1;
    GLint w, skip, i;

    x = gc->transform.clipX0;
    y = gc->transform.clipY0;
    x1 = gc->transform.clipX1;
    y1 = gc->transform.clipY1;
    if (((w = x1-x) == 0) || (y1-y == 0)) {
	return;
    }

#if defined(__GL_DEBUG_DEPTH_CLEAR)
    if ((*gc->imports.getenv)(gc, "DEBUG_ZBUFFER")) {
	char str[200];
	(*gc->imports.sprintf)(gc, str, 
			       "fixing from cnt: 0x%x to cnt: 0x%x\n", 
			       cnt, destRange);
	(*gc->imports.warning)(gc, str);
    }
#endif /* __GL_DEBUG_DEPTH_CLEAR */

    /*
    ** there are several cases:
    ** 1. If we use accelerated z-buffering, then the depth clear value
    **    is at the end of the range (0.0 or 1.0), depending on the value
    **    of glDepthFunc.  In that case, use that value as a "background"
    **    value, if the current value of the buffer is outside the range
    **    for the current frame.
    ** 2. If we use accelerated z-buffering, and we change the scissor
    **    rectangle, then, in order to simplify things, we move the whole
    **    z-buffer in the 0xffxxxxxx block, and we start from there.  Then,
    **    the next time we do another scissor, we don't have to deal with
    **    identifying in which frame each pixel belongs, and if it is really
    **    above or below us..  We know that the scirror occured at the
    **    0xff block, so the next scirror will move the old scissor rectangle
    **    to the 0xff block and start from there again..  
    ** 3. We don't use accelerated z-buffering.  We have nothing to worry
    **    about.  This function does nothing.
    */

    __GL_LOCK_DP(gc->drawablePrivate, gc);
    drawableBuf = dfb->buf.drawableBuf;
    dbDepth = __GL_DEPTH_DP(dfb);
    cnt = dbDepth->cnt;
    zDepthMask = dbDepth->zDepthMask;
    zClearInc = dbDepth->zClearInc;

    /* decide what the clear value is */
    if ((zClearInc == 0xff000000) && (gc->state.depth.clear == 1.0)) {
	clearVal = destRange | zDepthMask;
    } else if ((zClearInc == 0x01000000) && (gc->state.depth.clear == 0.0)) {
	clearVal = destRange;
    } else {
	__GL_UNLOCK_DP(gc->drawablePrivate);
	return;
    }

    __GL_LOCK_BUFFER_MASK(gc, __GL_DEPTH_BUFFER_MASK);

    /* figure out the beginning of the buffer */
    p = __GL_DEPTH_ADDR(gc, dfb, drawableBuf, (__GLzValue *), x, y);
    /* how much to skip from scanline to scanline */
    skip = drawableBuf->outerWidth - w;

    for (; y < y1; y++) {
	for (i=w; i; i--, p++) {
	    if ((p[0] & ~zDepthMask) == cnt) {
		p[0] = (p[0] & zDepthMask) | destRange;
	    } else {
		p[0] = clearVal;
	    }
	}
	p += skip;
    }

    __GL_UNLOCK_BUFFER_MASK(gc, __GL_DEPTH_BUFFER_MASK);
    __GL_UNLOCK_DP(gc->drawablePrivate);
}

/************************************************************************/

static GLboolean (*__glStoreProcs[64])(__GLdepthBuffer*, GLint, GLint, __GLzValue)
 = {
     /* unsigned */
     StoreNEVER16,
     StoreLESS16,
     StoreEQUAL16,
     StoreLEQUAL16,
     StoreGREATER16,
     StoreNOTEQUAL16,
     StoreGEQUAL16,
     StoreALWAYS16,
     /* unsigned, masked */
     StoreNEVER16,
     StoreLESS16_W,
     StoreEQUAL16_W,
     StoreLEQUAL16_W,
     StoreGREATER16_W,
     StoreNOTEQUAL16_W,
     StoreGEQUAL16_W,
     StoreALWAYS16_W,
     /* unsigned */
     StoreNEVER,
     StoreLESS,
     StoreEQUAL,
     StoreLEQUAL,
     StoreGREATER,
     StoreNOTEQUAL,
     StoreGEQUAL,
     StoreALWAYS,
     /* unsigned, masked */
     StoreNEVER,
     StoreLESS_W,
     StoreEQUAL_W,
     StoreLEQUAL_W,
     StoreGREATER_W,
     StoreNOTEQUAL_W,
     StoreGEQUAL_W,
     StoreALWAYS_W,
};

/* ARGSUSED */
static void Pick(__GLcontext *gc, __GLdepthBuffer *fb, GLint depthIndex)
{
    GLenum testFunc = gc->state.depth.testFunc;
    __GLdrawableDepthBuffer *dbDepth;

    __GL_LOCK_DP(gc->drawablePrivate, gc);
    dbDepth = __GL_DEPTH_DP(fb);

    /* check to see if we can use partial clear */
    dbDepth->allowPartialZClear = GL_FALSE;
    if (dbDepth->zClearInc != 0x00000000) {
	if( testFunc == GL_LESS || testFunc == GL_LEQUAL ) {
	    /* if we don't clear to the furthest point of the z-buffer */
	    if( gc->state.depth.clear == 1.0 ) {
		/* we can't accelerate z buffering */
		dbDepth->allowPartialZClear = GL_TRUE;
	    }
	} else if( testFunc == GL_GREATER || testFunc == GL_GEQUAL ) {
	    /* if we don't clear to the closest point of the z-buffer */
	    if( gc->state.depth.clear == 0.0 ) {
		/* we can't accelate z buffering */
		dbDepth->allowPartialZClear = GL_TRUE;
	    }
	}
    }

    __GL_UNLOCK_DP(gc->drawablePrivate);

    fb->store = __glStoreProcs[depthIndex];
}

/************************************************************************/

static GLboolean Resize(__GLbuffer *fb, 
			GLint x, GLint y, GLint w, GLint h, GLuint bufferMask)
{
    __GLdepthBuffer *dfb = (__GLdepthBuffer *)fb;

    __GL_LOCK_DP(fb->gc->drawablePrivate, fb->gc);
    __GL_DEPTH_DP(dfb)->fullZClear = GL_TRUE;
    __GL_UNLOCK_DP(fb->gc->drawablePrivate);

    return GL_TRUE;
}

/************************************************************************/

static void FreePrivate(__GLdrawableBuffer *drawableBuf, __GLdrawablePrivate *dp)
{
    __GLdrawableDepthBuffer *dbDepth = 
	(__GLdrawableDepthBuffer *) drawableBuf->private;

    /* no need to lock the drawableBuf.  We are called from wgl safely */

    if (dbDepth) {
	(*dp->free)(dbDepth);
    }

    drawableBuf->private = NULL;
}

/************************************************************************/

static void MakeCurrent(__GLbuffer *buf, __GLdrawableBuffer *drawableBuf,
			__GLcontext *gc)
{
    __GLdrawableDepthBuffer *dbDepth = 
	(__GLdrawableDepthBuffer *) drawableBuf->private;
    GLint zbits = drawableBuf->depth;

    __glMakeCurrentBuffer(buf, drawableBuf, gc);

    /* late allocation of drawablebuffer section */
    if (dbDepth == NULL) {
	dbDepth = (__GLdrawableDepthBuffer *)
	    (*gc->drawablePrivate->malloc)(sizeof(__GLdrawableDepthBuffer));
	assert(dbDepth != NULL);

	drawableBuf->private = (void *) dbDepth;
	drawableBuf->freePrivate = FreePrivate;

	if (zbits <= 16) {
	    dbDepth->cnt = 0x00000000;
	    dbDepth->numFracBits = 16;
	    dbDepth->zDepthMask = 0x0000ffff;
	    dbDepth->zClearInc = 0x00000000;
	    dbDepth->fullZClear = GL_TRUE;
	    dbDepth->invertZRange = GL_FALSE;
	} else if (zbits <= 24) {
	    dbDepth->cnt = 0xff000000;
	    dbDepth->numFracBits = 7;
	    dbDepth->zDepthMask = 0x00ffffff;
	    dbDepth->zClearInc = 0xff000000; /* -1 */
	    dbDepth->fullZClear = GL_FALSE;
	    dbDepth->invertZRange = GL_FALSE;
	} else {
	    dbDepth->cnt = 0x00000000;
	    dbDepth->numFracBits = 0;
	    dbDepth->zDepthMask = 0xffffffff;
	    dbDepth->zClearInc = 0x00000000;
	    dbDepth->fullZClear = GL_TRUE;
	    dbDepth->invertZRange = GL_FALSE;
	}
    }

    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH | __GL_DIRTY_SCISSOR);
}

/************************************************************************/


void __glInitDepth16(__GLdepthBuffer *fb, __GLcontext *gc)
{
    __glInitBuffer( &fb->buf, gc );

    fb->buf.resize = Resize;
    fb->buf.makeCurrent = MakeCurrent;

    fb->pick = Pick;
    fb->clear = Clear;
    fb->store2 = StoreALWAYS16;
    fb->fetch = Fetch16;

    fb->scale = (__GLzValue) 0xffff0000;
    fb->testFunc = GL_LESS;
    fb->fixZBuffer = ( void (*)(__GLcontext *, __GLdepthBuffer *, GLuint) ) __glNop3;
    fb->writeMask = (__GLzValue) ~0;
    fb->minResolution = 0x1000;
}


void __glInitDepth24(__GLdepthBuffer *fb, __GLcontext *gc )
{
    __glInitBuffer( &fb->buf, gc );

    fb->buf.resize = Resize;
    fb->buf.makeCurrent = MakeCurrent;

    fb->pick = Pick;
    fb->clear = Clear;
    fb->store2 = StoreALWAYS;
    fb->fetch = FetchFastClear;

    fb->scale = (__GLzValue) 0x7fffff80;
    fb->testFunc = GL_LESS;
    fb->fixZBuffer = Fix_zbuffer;
    fb->writeMask = (__GLzValue) ~0;
    fb->minResolution = 0x1000;
}


void __glInitDepth32(__GLdepthBuffer *fb, __GLcontext *gc )
{
    __glInitBuffer( &fb->buf, gc );

    fb->buf.resize = Resize;
    fb->buf.makeCurrent = MakeCurrent;

    fb->fixZBuffer = ( void (*)(__GLcontext *, __GLdepthBuffer *, GLuint) ) __glNop3;
    fb->pick = Pick;
    fb->clear = Clear;
    fb->store2 = StoreALWAYS;
    fb->fetch = Fetch;

    fb->scale = (__GLzValue) 0x7fffff80;
    fb->testFunc = GL_LESS;
    fb->fixZBuffer = ( void (*)(__GLcontext *, __GLdepthBuffer *, GLuint) ) __glNop3;
    fb->writeMask = (__GLzValue) ~0;
    fb->minResolution = 0x1000;
}

void __glInitDepth(__GLdepthBuffer *fb, __GLcontext *gc )
{
    int	zbits;

    zbits = gc->modes.depthBits;

    if( zbits <= 16 ) {
	__glInitDepth16( fb, gc );
    } else if( zbits <= 24 ) {
	__glInitDepth24( fb, gc );
    } else {
	__glInitDepth32( fb, gc );
    }
}
