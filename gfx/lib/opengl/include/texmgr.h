#ifndef __gl_texmgr_h__
#define __gl_texmgr_h__

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
** $Header: //depot/main/gfx/lib/opengl/include/texmgr.h#5 $
*/

#include "texture.h"

/*
 * Texture residence status 
 */

#define __GL_TEXTURE_NOT_RESIDENT           0
#define __GL_TEXTURE_RESIDENCE_LOADED       1
#define __GL_TEXTURE_RESIDENCE_LOWPERF      2
#define __GL_TEXTURE_RESIDENCE_HIPERF       3


/* Texture manager interface */
typedef struct __GLtextureManagerRec __GLtextureManager;

struct __GLtextureManagerRec {

    int refcount;

    /* 
     * Free per-context texture manager state.  If no more contexts
     * reference the texture manager, global state is also released.
     */
    void (*free)(__GLcontext *gc);

    /*
     * Called after glPrioritizeTextures. Frees all of the texture
     * memory so that the texture manager can create a new hierarchy.
     */
    GLboolean (*freeTextures)(__GLcontext *gc, __GLtextureManager *texmgr);
};


/*
 * These functions define the texture manager interface 
 */

/* 
 * The create method is part of the __glDevice and is called during
 * context initialization.
 */
void __glInitTextureManager(__GLcontext *gc);
void __glFreeTextureManager(__GLcontext *gc);
void __glShareTextureManager(__GLcontext *dst, __GLcontext *src);

__GLtexture *__glCreateTexture(__GLcontext *gc, GLuint name,
			       GLuint targetIndex);
void __glFreeTexture(__GLcontext *gc, __GLtexture *tex);
GLenum __glMakeResidentTexture(__GLcontext *gc, __GLtexture *tex,
			       GLclampf priority);
GLboolean __glUnmakeResidentTexture(__GLcontext *gc, __GLtexture *tex);

GLboolean __glInvalidateAllTextures(__GLcontext *gc, 
				    __GLtextureManager *texmgr);


/*
 * These functions define the texture object interface 
 */

void __glFreeTexture(__GLcontext *gc, __GLtexture *tex);

GLenum __glTexObjMakeResident(__GLtexture *tex, GLclampf priority);

GLvoid __glCopyTexImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			__GLtexture *tex, GLint lod);

GLvoid __glGenericReadTexImage(__GLcontext *gc, __GLpixelSpanInfo *spanInfo,
			       __GLtexture *tex, GLint lod);

__GLtextureBuffer *
__glTextureCreateLevel(__GLcontext *gc, __GLtexture *tex, GLint lod,
		       GLint components, GLsizei w, GLsizei h, GLsizei d,
		       GLint border, GLint dim);

__GLtextureBuffer *
__glTextureCreateProxyLevel(__GLcontext *gc, __GLtexture *tex, GLint lod,
			    GLint components, GLsizei w, GLsizei h, GLsizei d,
			    GLint border, GLint dim);

void __glTextureDeleteLevel(__GLcontext *gc, __GLtexture *tex, GLint lod);
void __glTextureDeleteProxyLevel(__GLcontext *gc, __GLtexture *tex, GLint lod);

#endif
