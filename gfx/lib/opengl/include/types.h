#ifndef __gl_types_h_
#define __gl_types_h_

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
** Low level data types.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/types.h#14 $
*/

#include <limits.h>
#include <float.h>

#include "g_os.h"

/*
** Typedefs.  These are forward declarations to internal data structures.
** This eases the burden of dealing with circular references among
** the header files.
*/
typedef GLshort __GLaccumCellElement;
typedef GLubyte __GLstencilCell;
typedef struct __GLaccumBufferRec __GLaccumBuffer;
typedef struct __GLattributeRec __GLattribute;
typedef struct __GLclientAttributeRec __GLclientAttribute;
typedef struct __GLbitmapRec __GLbitmap;
typedef struct __GLbufferRec __GLbuffer;
typedef struct __GLownershipBufferRec __GLownershipBuffer;
typedef struct __GLcolorBufferRec __GLcolorBuffer;
typedef struct __GLdepthBufferRec __GLdepthBuffer;
typedef struct __GLfogMachineRec __GLfogMachine;
typedef struct __GLfragmentRec __GLfragment;
typedef struct __GLlightModelStateRec __GLlightModelState;
typedef struct __GLlightSourceMachineRec __GLlightSourceMachine;
typedef struct __GLlineOptionsRec __GLlineOptions;
typedef struct __GLmaterialMachineRec __GLmaterialMachine;
typedef struct __GLmaterialStateRec __GLmaterialState;
typedef struct __GLmipMapLevelRec __GLmipMapLevel;
typedef struct __GLmatrixRec __GLmatrix;
typedef struct __GLpixelSpanInfoRec __GLpixelSpanInfo;
typedef struct __GLprocTableRec __GLprocTable;
typedef struct __GLscreenRec __GLscreen;
typedef struct __GLshadeRec __GLshade;
typedef struct __GLstencilBufferRec __GLstencilBuffer;
typedef struct __GLstippleRec __GLstipple;
typedef struct __GLtexelRec __GLtexel;
typedef struct __GLtextureRec __GLtexture;
typedef struct __GLtransformRec __GLtransform;
typedef struct __GLvertexRec __GLvertex;
typedef struct __GLdispatchStateRec __GLdispatchState;
typedef struct __GLarenaRec __GLarena;
typedef struct __GLnamesArrayRec __GLnamesArray;
typedef struct __GLcolorIteratorRec __GLcolorIterator;
typedef struct __GLtextureIteratorRec __GLtextureIterator;

/*
** Type of z value used by the software z buffering code.
** NOTE: must be unsigned
*/
typedef unsigned int __GLzValue;
typedef unsigned short __GLzValue16;

typedef int __GLfixedCoord;/*XXX*/

/************************************************************************/

/*
** Implementation data types.  The implementation is designed to run in
** single precision or double precision mode, all of which is controlled
** by an ifdef and the following typedef's.
**
** Use __GL_MAX_FLOAT to get the largest representable GL_FLOAT
*/
#ifdef	__GL_DOUBLE
typedef double __GLfloat;
#define __GL_MAX_FLOAT DBL_MAX
#else
typedef float __GLfloat;
#define __GL_MAX_FLOAT FLT_MAX
#endif

/************************************************************************/

/*
** Coordinate structure.  Coordinates contain x, y, z and w.
*/
typedef struct __GLcoordRec {
    __GLfloat x, y, z, w;
} __GLcoord;

/*
** Color structure.  Colors are composed of red, green, blue and alpha.
*/
typedef struct __GLcolorRec {
    __GLfloat r, g, b, a;
} __GLcolor;

/*
** Generic no-op procedures.  Used when function pointers need to
** be stubbed out when an operation is disabled.
*/
extern void __glNop0(void);
extern void __glNop1(GLint);
extern void __glNop2(GLint,GLint);
extern void __glNop3(GLint,GLint,GLint);
extern void __glNop4(GLint,GLint,GLint,GLint);
extern void __glNop5(GLint,GLint,GLint,GLint,GLint);
extern void __glNop6(GLint,GLint,GLint,GLint,GLint,GLint);

extern void GLAPI __glGLAPINop1(GLint);

/*
** Use when an extension isn't implemented, but has an entry in the
** dispatch table because some platform is using the extension.
** Returns an INVALID_OPERATION error.
*/
extern void GLAPI __glim_UnimplementedExtension(void);

/* The display list version of above */

extern void GLAPI __gllc_UnimplementedExtension(void);

#endif /* __gl_types_h_ */
