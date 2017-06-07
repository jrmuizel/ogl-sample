#ifndef __gl_fmacros_h__
#define __gl_fmacros_H__

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
** $Header: //depot/main/gfx/lib/opengl/include/fmacros.h#6 $
*/

/*
** Floating format defines
*/

#define FLOAT_MANTISSA_BITS 23
#define DOUBLE_MANTISSA_BITS 20
#define DOUBLE_MANTISSA_OFFSET ((float) (1<<20))

#define FLOAT_MANTISSA_MASK ((1<<(FLOAT_MANTISSA_BITS))-1)
#define GLFLOAT_MANTISSA_MASK ((1<<(FLOAT_MANTISSA_BITS-1))-1)
#define GLFLOAT_QUANTIZE_MASK ((-1)<<(FIXED_FRAC_BITS - COORD_QUANT_BITS))


/*
** Floating point unit macros
*/

/* Floating point rounding mode control */

#define FP_ROUND_NEAREST 0x0000
#define FP_ROUND_DOWN    0x0400
#define FP_ROUND_UP      0x0800
#define FP_ROUND_ZERO    0x0c00

/*
** Generic arithmentic macros
*/

#define Float2Int(_flt_, _int_) \
    (*((float *)(&(_int_)))=(_flt_)+((float)(1<<(FLOAT_MANTISSA_BITS))), \
     (_int_)&=FLOAT_MANTISSA_MASK)

#define SFloat2Int(_flt_, _int_) \
    (*((float *)(&(_int_)))=(_flt_)+((float)(3<<(FLOAT_MANTISSA_BITS-1))), \
     (_int_)=((_int_)&FLOAT_MANTISSA_MASK)-(1<<(FLOAT_MANTISSA_BITS-1)))


/*
** fixed point defines and macros
*/

#define XY_FRAC_BITS		11
#define Z_FRAC_BITS		0
#define COLOR_FRAC_BITS		16
#define STRQ_FRAC_BITS		16

#define FloatBits(a)		    (ftmp = (float) (a), *((int *)&ftmp))
#define FloatBias(bits)		    ((float) (3 << (22-(bits))))
#define BiasedFloatToFixed(a)	    ((FloatBits(a) & 0x7FFFFFL) - 0x400000L)
#define BiasedFloatToInt(a,bits)    TruncFixed(BiasedFloatToFixed(a),bits)

#define DoubleBits(a)		    (dtmp = (double) (a), *((int *)&dtmp))
#define DoubleBias(bits)	    ((double) (3 << 20) * (1UL << (31-(bits))))
#define BiasedDoubleToFixed(a)	    DoubleBits(a)
#define BiasedDoubleToInt(a,bits)   TruncFixed(BiasedDoubleToFixed(a),bits)

#define FloatToFixedI(a,bits)	    ((int) ((a) * (float) (1UL << (bits))))
#define FloatToFixedF(a,bits)	    BiasedFloatToFixed((a)+FloatBias(bits))
#define FloatToFixedD(a,bits)	    BiasedDoubleToFixed((a)+DoubleBias(bits))

#define FloatToInt(a,bits)	    ((int) (a))
#define FloatToFrac(a)		    ((FloatBits((a)+3.0F) << 9) ^ 0x80000000UL)
#define FloatToFixed(a,bits)	    FloatToFixedD(a,bits)
#define TruncFixed(a,bits)	    ((int) (a) >> (bits))

#define CoordToInt(a)		    TruncFixed((a), COLOR_FRAC_BITS)

#define DITHER_BITS		    4
#define DITHER_BITS_LOG2	    2
#define DITHER_MASK		    ((1 << DITHER_BITS_LOG2) - 1)
#define DITHER_SHIFT		    (COLOR_FRAC_BITS-DITHER_BITS)

#define COLOR_ROUND_NO_DITHER	    0.5F
#define COLOR_ROUND_DITHER	    (0.5F / (1 << DITHER_BITS))

#define XY_BIAS			    FloatBias(XY_FRAC_BITS)

#define XY_SUBPIX_FRAC_BITS	    4
#define XY_QUANTIZE_MASK	    (~0 << (XY_FRAC_BITS - XY_SUBPIX_FRAC_BITS))


#endif /* __gl_fmacros_h__ */
