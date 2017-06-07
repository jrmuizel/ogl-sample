#ifndef __gl_texfmt_h_
#define __gl_texfmt_h_

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
** $Header: //depot/main/gfx/lib/opengl/include/texfmt.h#6 $
*/

#include "types.h"


/* 
 * 2 mag methods and 6 min methods
 */
#define __GL_NUM_FILTER_METHODS	  8
/* Texture Format table data structures, for drivers.
 *
 * A table of these is pointed at by the context.  
 */

struct __GLtextureFormatRec {
    GLint internalFormat;
    GLint indexSize;
    GLint redSize;
    GLint greenSize;
    GLint blueSize;
    GLint alphaSize;
    GLint luminanceSize;
    GLint intensitySize;

    GLint bitsPerTexel;

    /* pixel path storage information */
    GLenum pxFormat;
    GLenum pxType;
    GLenum pxAlignment;

    /* fetch a single texel (slow path used for software rendering) */

    void (*extractTexel)(__GLmipMapLevel *level, 
			 __GLtexture *tex, GLint, GLint,
			 GLint, __GLtexel *res);

    void (*extractTexelBorder)(__GLmipMapLevel *level, 
			       __GLtexture *tex, GLint,
			       GLint, GLint, __GLtexel *res);

    void (*extractTexel3D)(__GLmipMapLevel *level, 
			   __GLtexture *tex, GLint, GLint,
			   GLint, __GLtexel *res);

    void (*extractTexelBorder3D)(__GLmipMapLevel *level, 
			         __GLtexture *tex, GLint,
			         GLint, GLint, __GLtexel *res);

    /* get and set rows of texels (used to download and read back textures) */

#if 0
    void (*store)(__GLcontext *gc,
                  __GLmipMapLevel *level, __GLtexture *tex,
                  GLint x, GLint y, GLint w, __GLtexel *src);

    void (*fetch)(__GLcontext *gc, __GLmipMapLevel *level,
		  __GLtexture *tex,
                  GLint  x, GLint y, GLint w, __GLtexel *dest);
#else
    void (*store)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		  const GLvoid *inspan, GLvoid *outspan);

    void (*fetch)(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
		  const GLvoid *inspan, GLvoid *outspan);
#endif

    /* Get and set rects of texels (used to download and read back textures) */

    void (*storeRect)(__GLcontext *gc, struct __GLmipMapLevelRec *level,
		      struct __GLtextureRec *tex, GLint x, GLint y, GLint w,
		      GLint h, GLint srcStride, __GLtexel *src);

    void (*fetchRect)(__GLcontext *gc, struct __GLmipMapLevelRec *level,
		      struct __GLtextureRec *tex, GLint x, GLint y, GLint w,
		      GLint h, GLint dstStride, __GLtexel *dest);

};

typedef struct __GLtextureFormatRec  __GLtextureFormat;


/* The built-in texture internal formats
 * custom formats may be added as offsets from __GL_FORMAT_VENDOR_SPECIFIC
 */
enum {
    __GL_FORMAT_LUMINANCE8,
    __GL_FORMAT_LUMINANCE_ALPHA8,
    __GL_FORMAT_RGB8,
    __GL_FORMAT_RGB332,
    __GL_FORMAT_XRGB1555,
    __GL_FORMAT_RGB565,
    __GL_FORMAT_RGBA8,
    __GL_FORMAT_RGBA4,
    __GL_FORMAT_ARGB4,
    __GL_FORMAT_RGBA5551,
    __GL_FORMAT_ARGB1555,
    __GL_FORMAT_ALPHA8,
    __GL_FORMAT_INTENSITY8,
    __GL_FORMAT_COLOR_INDEX8,
    __GL_FORMAT_COLOR_INDEX16,
    __GL_FORMAT_VENDOR_SPECIFIC
};

extern const __GLtextureFormat *
__glLookupTextureFormat(const GLenum components, GLenum *baseFormat);

extern const __GLtextureFormat __glTexFormatLuminance8;
extern const __GLtextureFormat __glTexFormatLuminanceAlpha8;
extern const __GLtextureFormat __glTexFormatRGB8;
extern const __GLtextureFormat __glTexFormatRGB332;
extern const __GLtextureFormat __glTexFormatXRGB1555;
extern const __GLtextureFormat __glTexFormatRGB565;
extern const __GLtextureFormat __glTexFormatRGBA8;
extern const __GLtextureFormat __glTexFormatRGBA4;
extern const __GLtextureFormat __glTexFormatARGB4;
extern const __GLtextureFormat __glTexFormatRGBA5551;
extern const __GLtextureFormat __glTexFormatARGB1555;
extern const __GLtextureFormat __glTexFormatAlpha8;
extern const __GLtextureFormat __glTexFormatIntensity8;
extern const __GLtextureFormat __glTexFormatColorIndex8;
extern const __GLtextureFormat __glTexFormatColorIndex16;

#endif
