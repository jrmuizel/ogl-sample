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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_zfuncs.c#3 $
*/


#ifndef __GL_USE_MIPSASMCODE

#include "render.h"


/*
** this is a series of depth testers written in C
*/

/***********************  non-masked writes ***********************/

/*
** NEVER, no mask
*/
/* ARGSUSED */
GLboolean
__glDT_NEVER( __GLzValue z, __GLzValue *zfb )
{
    return GL_FALSE;
}

/*
** LEQUAL, no mask
*/
GLboolean
__glDT_LEQUAL( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    if( (GLuint)z <= (GLuint)zbv ) {
	zfb[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** LESS, no mask
*/
GLboolean
__glDT_LESS( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    if( (GLuint)z < (GLuint)zbv ) {
	zfb[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** EQUAL, no mask
*/
GLboolean
__glDT_EQUAL( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    if( z == zbv ) {
	zfb[0] = z;	/* why is this there?  Who uses GL_EQUAL anyway? */
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** GREATER, no mask
*/
GLboolean
__glDT_GREATER( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    if( (GLuint)z > (GLuint)zbv ) {
	zfb[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** NOTEQUAL, no mask
*/
GLboolean
__glDT_NOTEQUAL( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    if( z != zbv ) {
	zfb[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** GEQUAL, no mask
*/
GLboolean
__glDT_GEQUAL( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    if( (GLuint)z >= (GLuint)zbv ) {
	zfb[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** ALWAYS, no mask
*/
/* ARGSUSED */
GLboolean
__glDT_ALWAYS( __GLzValue z, __GLzValue *zfb )
{
    zfb[0] = z;
    return GL_TRUE;
}

/***********************  masked writes ***********************/

/*
** LEQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_LEQUAL_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    return ((GLuint)z <= (GLuint)zbv);
}

/*
** LESS, mask
*/
/* ARGSUSED */
GLboolean
__glDT_LESS_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    return ((GLuint)z < (GLuint)zbv);
}

/*
** EQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_EQUAL_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    return (z == zbv);
}

/*
** GREATER, mask
*/
/* ARGSUSED */
GLboolean
__glDT_GREATER_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    return ((GLuint)z > (GLuint)zbv);
}

/*
** NOTEQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_NOTEQUAL_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    return (z != zbv);
}

/*
** GEQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_GEQUAL_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue zbv = *zfb;

    return ((GLuint)z >= (GLuint)zbv);
}

/*
** ALWAYS, mask
*/
/* ARGSUSED */
GLboolean
__glDT_ALWAYS_M( __GLzValue z, __GLzValue *zfb )
{
    return GL_TRUE;
}

/***********************  non-masked writes ***********************/

/*
** NEVER, no mask
*/
/* ARGSUSED */
GLboolean
__glDT_NEVER16( __GLzValue z, __GLzValue *zfb )
{
    return GL_FALSE;
}

/*
** LEQUAL, no mask
*/
GLboolean
__glDT_LEQUAL16( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    if( (GLuint)z <= (GLuint)zbv ) {
	zfb16[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** LESS, no mask
*/
GLboolean
__glDT_LESS16( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    if( (GLuint)z < (GLuint)zbv ) {
	zfb16[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** EQUAL, no mask
*/
GLboolean
__glDT_EQUAL16( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    if( z == zbv ) {
	zfb16[0] = z;	/* why is this there?  Who uses GL_EQUAL anyway? */
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** GREATER, no mask
*/
GLboolean
__glDT_GREATER16( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    if( (GLuint)z > (GLuint)zbv ) {
	zfb16[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** NOTEQUAL, no mask
*/
GLboolean
__glDT_NOTEQUAL16( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    if( z != zbv ) {
	zfb16[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** GEQUAL, no mask
*/
GLboolean
__glDT_GEQUAL16( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    if( (GLuint)z >= (GLuint)zbv ) {
	zfb16[0] = z;
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

/*
** ALWAYS, no mask
*/
/* ARGSUSED */
GLboolean
__glDT_ALWAYS16( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    zfb16[0] = z;
    return GL_TRUE;
}

/***********************  masked writes ***********************/

/*
** LEQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_LEQUAL16_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    return ((GLuint)z <= (GLuint)zbv);
}

/*
** LESS, mask
*/
/* ARGSUSED */
GLboolean
__glDT_LESS16_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    return ((GLuint)z < (GLuint)zbv);
}

/*
** EQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_EQUAL16_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    return (z == zbv);
}

/*
** GREATER, mask
*/
/* ARGSUSED */
GLboolean
__glDT_GREATER16_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    return ((GLuint)z > (GLuint)zbv);
}

/*
** NOTEQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_NOTEQUAL16_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    return (z != zbv);
}

/*
** GEQUAL, mask
*/
/* ARGSUSED */
GLboolean
__glDT_GEQUAL16_M( __GLzValue z, __GLzValue *zfb )
{
    __GLzValue16 *zfb16 = (__GLzValue16 *) zfb;
    __GLzValue16 zbv = *zfb16;

    return ((GLuint)z >= (GLuint)zbv);
}

/*
** ALWAYS, mask
*/
/* ARGSUSED */
GLboolean
__glDT_ALWAYS16_M( __GLzValue z, __GLzValue *zfb )
{
    return GL_TRUE;
}

#endif /* __GL_USE_MIPSASMCODE */
