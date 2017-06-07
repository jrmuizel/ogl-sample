#ifndef __glapimacro
#define __glapimacro

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
** $Header: //depot/main/gfx/lib/opengl/include/apimacro.h#5 $
*/

/*
** This file contains macros that appear at the start of every entry
** point in OpenGL. Depending upon the nature of the api call the macro
** is different - some need to cause buffer flushing and others don't
** for example.
*/

/*
** A conCAT primitive is one that is continued across begin/end pairs.
** This is done because of successive calls to glBegin with the same
** primitive type.  This macro flushes outstanding CAT primitives
*/
#if defined(__GL_VCACHE_ALLOW_CAT_PRIMITIVE)
#define __GL_API_FLUSH_CAT_PRIMITIVE()				\
    if (gc->vertexCache.vertexCacheState & VC_CAT_PRIMITIVE) {	\
	__glProcessVertexCache(gc, GL_END_VCACHE);		\
	gc->vertexCache.vertexCacheState &= ~VC_CAT_PRIMITIVE;	\
    }
#else
#define __GL_API_FLUSH_CAT_PRIMITIVE()
#endif

/*
** Flush outstanding API calls, so that we can execute non begin/end
** types of primitives.  Currently, we only need to flush the CAT
** primitives.  Nothing else can be outstanding.
*/
#define	__GL_API_FLUSH() \
	__GL_API_FLUSH_CAT_PRIMITIVE()

/*
** Make sure that nothing is outstanding after this call.  This flushes
** all outstanding API calls, and makes sure graphics is idle.
*/
#define __GL_API_IDLE() \
	__GL_API_FLUSH(); \
	(*gc->procs.waitIdle)(gc);

/*
** Note that entry points are labelled with either glim__ or gllc__.  In
** the case of the gllc__ entry points the checks are not needed and if 
** they render at the same time as building up the list, they call the 
** glim__ routines anyway.
*/

/* called by glBegin */
#define	__GL_API_BGN_RENDER()

/* called by glEnd */
#define	__GL_API_END_RENDER()

/*
** called by any rendering call other than a bgn..end type. If we get
** here, we need to flush any primitives that are currently outstanding.
** This only is a problem when we use concatenated primitives
*/
#define	__GL_API_NOTBE_RENDER() \
	__GL_API_FLUSH_CAT_PRIMITIVE()

/*
** eval api calls - do nothing at the moment as they call into the 
** __glim_ functions, which will perform any testing etc.
*/
#define	__GL_API_EVAL()

/*
** called by any API that will change state.  If this happens, then
** any outstanding cached rendering needs to be flushed out. It is
** assumed that if we are here, we are not in a bgn..end, so the flush
** has to happen as though it's an glEnd() type flush.
*/
#define	__GL_API_STATE() \
	__GL_API_FLUSH_CAT_PRIMITIVE()

/* called by any api call that will get information, no flush is needed. */
#define	__GL_API_GET()

/*
** called by any pixel operation..bitmap..readpixels..accum type things.
** there is a need to flush since the operations depend upon the 
** buffer being up-to-date.
*/
#define	__GL_API_PIXEL_OP() \
	__GL_API_IDLE();

/*
** called by vapi type calls - vertex, color... This is here for
** completeness - these calls do not need to check for any buffer
** flushing.
*/
#define	__GL_API_VAPI()

/* This is a placeholder as well that doesn't need action */
#define	__GL_API_BLAND()

#endif /* __glapimacro */
