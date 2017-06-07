#ifndef __gl_cpu_h_
#define __gl_cpu_h_

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
*/
/*
** CPU dependent constants.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/cpu.h#7 $
*/

/*
** cpu flags showing which CPU features/capabilities are present
*/
#define __GL_CPUFLAGS_MMX	(1<<0)	/* cpu supports MMX instructions */
#define __GL_CPUFLAGS_K3D	(1<<1)	/* cpu is an AMD K3D */

/*
** function prototypes
*/
void __glGetCPUCaps(__GLcontext *gc);
#if defined(__GL_USE_ASMCODE) && defined(i386)
void __glI386GetCPUCaps(__GLcontext *gc);
#endif /* __GL_USE_ASMCODE && i386 */

/*
** Representation of FP numbers
*/

#if defined(_MIPSEB)
/* Big Endian */

#define __GL_BITS_PER_BYTE	8
#define __GL_STIPPLE_MSB	1

/* IEEE Single Precision Floating Point */
typedef union __GLcpuFloatRec {
    __GLfloat f;
    unsigned int ul;
    struct {
	unsigned int sign : 1;
	unsigned int exponent : 8;
	unsigned int mantissa : 23;
    } field;
} __GLcpuFloat;

#define __GL_FLOAT_MANTISSA_BITS	23
#define __GL_FLOAT_MANTISSA_SHIFT	0
#define __GL_FLOAT_EXPONENT_BIAS	127
#define __GL_FLOAT_EXPONENT_BITS	8
#define __GL_FLOAT_EXPONENT_SHIFT	23
#define __GL_FLOAT_SIGN_SHIFT		31

#else
/* Little Endian */

#define __GL_BITS_PER_BYTE	8
#define __GL_STIPPLE_MSB	0

/* IEEE Single Precision Floating Point */
typedef union __GLcpuFloatRec {
    __GLfloat f;
    unsigned int ul;
    struct {
	unsigned int sign : 1;
	unsigned int exponent : 8;
	unsigned int mantissa : 23;
    } field;
} __GLcpuFloat;

#define __GL_FLOAT_MANTISSA_BITS	23
#define __GL_FLOAT_MANTISSA_SHIFT	0
#define __GL_FLOAT_EXPONENT_BIAS	127
#define __GL_FLOAT_EXPONENT_BITS	8
#define __GL_FLOAT_EXPONENT_SHIFT	23
#define __GL_FLOAT_SIGN_SHIFT		31

#endif

#endif /* __gl_cpu_h_ */
