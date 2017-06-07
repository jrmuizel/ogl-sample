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
** $Header: //depot/main/gfx/lib/opengl/glcore/dl_splc.c#10 $
*/

#ifdef NEW_LISTS
#include "render.h"
#include "global.h"
#include "context.h"
#include "pixel.h"
#include "g_lcomp.h"
#include "g_listop.h"
#include "g_lcfncs.h"
#include "image.h"
#include "dlist.h"
#include "dlistopt.h"
#include "g_imports.h"

#include <string.h> /*XXX imports.h please*/

extern GLvoid *__glSPDlalloc(GLint size, GLint func); /*XXXblythe*/
extern GLvoid __glsplc_InvalidEnum(GLvoid);
extern GLvoid __glsplc_InvalidValue(GLvoid);

extern const GLubyte __GLdlsize_tab[];

#define __glCallListsSize(type)				\
	((type) >= GL_BYTE && (type) <= GL_4_BYTES ?	\
	__GLdlsize_tab[(type)-GL_BYTE] : -1)

/*
** Display list compilation and execution versions of CallList and CallLists
** are maintained here for the sake of sanity.  Note that __glle_CallList
** may not call __glim_CallList or it will break the infinite recursive
** display list prevention code.
*/
void __glsplc_CallList(GLuint list)
{
    struct __gllc_CallList_Rec *data;

    if (list == 0) {
	__glsplc_InvalidEnum();
	return;
    }

    data = __glSPDlalloc(sizeof(struct __gllc_CallList_Rec), __glop_CallList);
    if (data == NULL) return;
    data->list = list;
}

void __glsplc_CallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
    GLuint size;
    GLint arraySize;
    struct __gllc_CallLists_Rec *data;

    if (n < 0) {
	__glsplc_InvalidValue();
	return;
    }
    arraySize = n*__glCallListsSize(type);
    if (arraySize < 0) {
	__glsplc_InvalidEnum();
	return;
    }
    size = (GLuint)sizeof(struct __gllc_CallLists_Rec) + __GL_PAD(arraySize);
    data = __glSPDlalloc(size, __glop_CallLists);
    if (data == NULL) return;
    data->n = n;
    data->type = type;
    __GL_MEMCOPY((GLubyte *)data + sizeof(struct __gllc_CallLists_Rec),
		 lists, arraySize);
}

#define __GL_IMAGE_BITMAP	0
#define __GL_IMAGE_INDICES	1
#define __GL_IMAGE_RGBA		2

void __glsplc_Bitmap(GLsizei width, GLsizei height,
		   GLfloat xorig, GLfloat yorig, 
		   GLfloat xmove, GLfloat ymove, 
		   const GLubyte *oldbits)
{
    __GLbitmap *bitmap;
    GLubyte *newbits;
    GLint imageSize;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__glsplc_InvalidValue();
	return;
    }

    imageSize = height * ((width + 7) >> 3);
    imageSize = __GL_PAD(imageSize);

    bitmap = __glSPDlalloc(imageSize + (GLint)sizeof(__GLbitmap), __glop_Bitmap);
    if (bitmap == NULL) return;

    bitmap->width = width;
    bitmap->height = height;
    bitmap->xorig = xorig;
    bitmap->yorig = yorig;
    bitmap->xmove = xmove;
    bitmap->ymove = ymove;
    bitmap->imageSize = imageSize;

    newbits = (GLubyte *)bitmap + sizeof(__GLbitmap); 
    __glFillImage(gc, width, height, GL_COLOR_INDEX, GL_BITMAP, 
	    oldbits, newbits);
}

void __glsplc_PolygonStipple(const GLubyte *mask)
{
    GLvoid *data;
    __GL_SETUP();
    GLubyte *newbits;

    data = __glSPDlalloc(__glImageSize(32, 32, GL_COLOR_INDEX, GL_BITMAP),
			     __glop_PolygonStipple);
    if (data == NULL) return;

    newbits = (GLubyte *)data;
    __glFillImage(gc, 32, 32, GL_COLOR_INDEX, GL_BITMAP, mask, newbits);
}

struct __gllc_Map1f_Rec {
        GLenum    target;
        __GLfloat u1;
        __GLfloat u2;
        GLint     order;
        /*        points  */
};

void __glsplc_Map1f(GLenum target, 
		  GLfloat u1, GLfloat u2,
		  GLint stride, GLint order,
		  const GLfloat *points)
{
    struct __gllc_Map1f_Rec *map1data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();
    
    k=__glEvalComputeK(target);
    if (k < 0) {
	__glsplc_InvalidEnum();
	return;
    }

    if (order > gc->constants.maxEvalOrder || stride < k ||
	    order < 1 || u1 == u2) {
	__glsplc_InvalidValue();
	return;
    }

    cmdsize = (GLint)sizeof(*map1data) + 
	    __glMap1_size(k, order) * (GLint)sizeof(__GLfloat);

    map1data = __glSPDlalloc(cmdsize, __glop_Map1f);
    if (map1data == NULL) return;

    map1data->target = target;
    map1data->u1 = u1;
    map1data->u2 = u2;
    map1data->order = order;
    data = (__GLfloat *) ((GLubyte *)map1data + sizeof(*map1data));
    __glFillMap1f(k, order, stride, points, data);
}

void __glsplc_Map1d(GLenum target, 
		  GLdouble u1, GLdouble u2,
		  GLint stride, GLint order, 
		  const GLdouble *points)
{
    struct __gllc_Map1f_Rec *map1data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();
    
    k=__glEvalComputeK(target);
    if (k < 0) {
	__glsplc_InvalidEnum();
	return;
    }

    if (order > gc->constants.maxEvalOrder || stride < k ||
	    order < 1 || u1 == u2) {
	__glsplc_InvalidValue();
	return;
    }

    cmdsize = (GLint)sizeof(*map1data) + 
	    __glMap1_size(k, order) * (GLint)sizeof(__GLfloat);

    map1data = __glSPDlalloc(cmdsize, __glop_Map1d);
    if (map1data == NULL) return;

    map1data->target = target;
    map1data->u1 = u1;
    map1data->u2 = u2;
    map1data->order = order;
    data = (__GLfloat *) ((GLubyte *)map1data + sizeof(*map1data));
    __glFillMap1d(k, order, stride, points, data);
}

struct __gllc_Map2f_Rec {
        GLenum    target;
        __GLfloat u1;
        __GLfloat u2;
        GLint     uorder;
        __GLfloat v1;
        __GLfloat v2;
        GLint     vorder;
	__4_BYTE_PAD
        /*        points  */
};

void __glsplc_Map2f(GLenum target, 
		  GLfloat u1, GLfloat u2,
		  GLint ustride, GLint uorder, 
		  GLfloat v1, GLfloat v2,
		  GLint vstride, GLint vorder, 
		  const GLfloat *points)
{
    struct __gllc_Map2f_Rec *map2data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();

    k=__glEvalComputeK(target);
    if (k < 0) {
	__glsplc_InvalidEnum();
	return;
    }

    if (vorder > gc->constants.maxEvalOrder || vstride < k ||
	    vorder < 1 || u1 == u2 || ustride < k ||
	    uorder > gc->constants.maxEvalOrder || uorder < 1 ||
	    v1 == v2) {
	__glsplc_InvalidValue();
	return;
    }

    cmdsize = (GLint)sizeof(*map2data) + 
	    __glMap2_size(k, uorder, vorder) * (GLint)sizeof(__GLfloat);

    map2data = __glSPDlalloc(cmdsize, __glop_Map2f);
    if (map2data == NULL) return;

    map2data->target = target;
    map2data->u1 = u1;
    map2data->u2 = u2;
    map2data->uorder = uorder;
    map2data->v1 = v1;
    map2data->v2 = v2;
    map2data->vorder = vorder;

    data = (__GLfloat *) ((GLubyte *)map2data + sizeof(*map2data));
    __glFillMap2f(k, uorder, vorder, ustride, vstride, points, data);
}

void __glsplc_Map2d(GLenum target, 
		  GLdouble u1, GLdouble u2,
                  GLint ustride, GLint uorder, 
		  GLdouble v1, GLdouble v2,
		  GLint vstride, GLint vorder, 
		  const GLdouble *points)
{
    struct __gllc_Map2f_Rec *map2data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();

    k=__glEvalComputeK(target);
    if (k < 0) {
	__glsplc_InvalidEnum();
	return;
    }

    if (vorder > gc->constants.maxEvalOrder || vstride < k ||
	    vorder < 1 || u1 == u2 || ustride < k ||
	    uorder > gc->constants.maxEvalOrder || uorder < 1 ||
	    v1 == v2) {
	__glsplc_InvalidValue();
	return;
    }

    cmdsize = (GLint)sizeof(*map2data) + 
	    __glMap2_size(k, uorder, vorder) * (GLint)sizeof(__GLfloat);

    map2data = __glSPDlalloc(cmdsize, __glop_Map2d);
    if (map2data == NULL) return;

    map2data->target = target;
    map2data->u1 = u1;
    map2data->u2 = u2;
    map2data->uorder = uorder;
    map2data->v1 = v1;
    map2data->v2 = v2;
    map2data->vorder = vorder;

    data = (__GLfloat *) ((GLubyte *)map2data + sizeof(*map2data));
    __glFillMap2d(k, uorder, vorder, ustride, vstride, points, data);
}

void __glsplc_DrawPixels(GLint width, GLint height, GLenum format, 
		       GLenum type, const GLvoid *pixels)
{
    struct __gllc_DrawPixels_Rec *pixdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__glsplc_InvalidValue();
	return;
    }
    switch (format) {
      case GL_STENCIL_INDEX:
      case GL_COLOR_INDEX:
	index = GL_TRUE;
	break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_RGBA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_DEPTH_COMPONENT:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
	index = GL_FALSE;
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __glsplc_InvalidEnum();
	    return;
	}
	break;
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
	break;
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
	switch (format) {
	  case GL_RGB:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
	switch (format) {
	  case GL_RGBA:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }

    imageSize = __glImageSize(width, height, format, type);
    imageSize = __GL_PAD(imageSize);

    pixdata = __glSPDlalloc((GLint)sizeof(*pixdata) + imageSize, __glop_DrawPixels);
    if (pixdata == NULL) return;

    pixdata->width = width;
    pixdata->height = height;
    pixdata->format = format;
    pixdata->type = type;

    __glFillImage(gc, width, height, format, type, pixels, 
	    (GLubyte *)pixdata + sizeof(*pixdata));
}

void __glsplc_TexImage1D(GLenum target, GLint level, 
		       GLint components,
		       GLint width, GLint border, GLenum format, 
		       GLenum type, const GLvoid *pixels)
{
    struct __gllc_TexImage1D_Rec *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if (target == GL_PROXY_TEXTURE_1D) {
	(*gc->dispatchState->dispatch.TexImage1D)(target, level, components,
				width, border, format, type, pixels);
	return;
    }

    if (border < 0 || border > 1) {
	__glsplc_InvalidValue();
	return;
    }
    if (width < 0) {
	__glsplc_InvalidValue();
	return;
    }
    switch (format) {
      case GL_COLOR_INDEX:
	index = GL_TRUE;
	break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_RGBA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
	index = GL_FALSE;
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __glsplc_InvalidEnum();
	    return;
	}
	break;
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
	break;
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
	switch (format) {
	  case GL_RGB:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
	switch (format) {
	  case GL_RGBA:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }

    imageSize = __glImageSize(width, 1, format, type);
    imageSize = __GL_PAD(imageSize);

    texdata = __glSPDlalloc((GLint)sizeof(*texdata) + imageSize, __glop_TexImage1D);
    if (texdata == NULL) return;

    texdata->target = target;
    texdata->level = level;
    texdata->components = components;
    texdata->width = width;
    texdata->border = border;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0 && pixels != NULL) {
	__glFillImage(gc, width, 1, format, type, pixels, 
		(GLubyte *)texdata + sizeof(*texdata));
    }
}

void __glsplc_TexImage2D(GLenum target, GLint level, 
		       GLint components,
		       GLint width, GLint height, GLint border, 
		       GLenum format, GLenum type, 
		       const GLvoid *pixels)
{
    struct __gllc_TexImage2D_Rec *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if (target == GL_PROXY_TEXTURE_2D) {
	(*gc->dispatchState->dispatch.TexImage2D)(target, level, components,
				width, height, border, format, type, pixels);
	return;
    }

    if (border < 0 || border > 1) {
	__glsplc_InvalidValue();
	return;
    }
    if ((width < 0) || (height < 0)) {
	__glsplc_InvalidValue();
	return;
    }
    switch (format) {
      case GL_COLOR_INDEX:
	index = GL_TRUE;
	break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_RGBA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
	index = GL_FALSE;
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __glsplc_InvalidEnum();
	    return;
	}
	break;
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
	break;
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
	switch (format) {
	  case GL_RGB:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
	switch (format) {
	  case GL_RGBA:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }

    imageSize = __glImageSize(width, height, format, type);
    imageSize = __GL_PAD(imageSize);

    texdata = __glSPDlalloc((GLint)sizeof(*texdata) + imageSize, __glop_TexImage2D);
    if (texdata == NULL) return;

    texdata->target = target;
    texdata->level = level;
    texdata->components = components;
    texdata->width = width;
    texdata->height = height;
    texdata->border = border;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0 && pixels != NULL) {
	__glFillImage(gc, width, height, format, type, pixels, 
		(GLubyte *)texdata + sizeof(*texdata));
    }
}

void __glsplc_TexSubImage1D(GLenum target, GLint level,
			     GLsizei xoffset, GLsizei width,
			     GLenum format, GLenum type, const GLvoid *pixels)
{
    struct __gllc_TexSubImage1D_Rec *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if (width < 0) {
	__glsplc_InvalidValue();
	return;
    }
    switch (format) {
      case GL_COLOR_INDEX:
	index = GL_TRUE;
	break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_RGBA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
	index = GL_FALSE;
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __glsplc_InvalidEnum();
	    return;
	}
	break;
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
	break;
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
	switch (format) {
	  case GL_RGB:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
	switch (format) {
	  case GL_RGBA:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }

    imageSize = __glImageSize(width, 1, format, type);
    imageSize = __GL_PAD(imageSize);

    texdata = __glSPDlalloc((GLint)sizeof(*texdata) + imageSize,
			 __glop_TexSubImage1D);
    if (texdata == NULL) return;

    texdata->target = target;
    texdata->level = level;
    texdata->xoffset = xoffset;
    texdata->width = width;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0 && pixels != NULL) {
	__glFillImage(gc, width, 1, format, type, pixels, 
		(GLubyte *)texdata + sizeof(*texdata));
    }
}

void __glsplc_TexSubImage2D(GLenum target, GLint level,
			     GLsizei xoffset, GLsizei yoffset,
			     GLsizei width, GLsizei height,
			     GLenum format, GLenum type, const GLvoid *pixels)
{

    struct __gllc_TexSubImage2D_Rec *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__glsplc_InvalidValue();
	return;
    }
    switch (format) {
      case GL_COLOR_INDEX:
	index = GL_TRUE;
	break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_RGBA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_ABGR_EXT:
      case GL_BGR:
      case GL_BGRA:
	index = GL_FALSE;
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __glsplc_InvalidEnum();
	    return;
	}
	break;
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
	break;
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
	switch (format) {
	  case GL_RGB:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
	switch (format) {
	  case GL_RGBA:
	    break;
	  default:
	    __glsplc_InvalidOperation();
	    return;
	}
	break;
      default:
	__glsplc_InvalidEnum();
	return;
    }

    imageSize = __glImageSize(width, height, format, type);
    imageSize = __GL_PAD(imageSize);

    texdata = __glSPDlalloc((GLint)sizeof(*texdata) + imageSize,
			 __glop_TexSubImage2D);
    if (texdata == NULL) return;

    texdata->target = target;
    texdata->level = level;
    texdata->xoffset = xoffset;
    texdata->yoffset = yoffset;
    texdata->width = width;
    texdata->height = height;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0 && pixels != NULL) {
	__glFillImage(gc, width, height, format, type, pixels, 
		(GLubyte *) texdata + sizeof(*texdata));
    }
}

void __glsplc_Disable(GLenum cap)
{
    struct __gllc_Disable_Rec *data;

    if (( cap >= GL_VERTEX_ARRAY_EXT ) && ( cap <= GL_EDGE_FLAG_ARRAY_EXT)) {
        __GLcontext *pGC = (__GLcontext *)__gl_context;
	
	switch (cap) {
#if 0
	  case GL_VERTEX_ARRAY_EXT:
	    pGC->vertexArray.mask &= ~VERTARRAY_V_MASK;
	    pGC->vertexArray.index &= ~VERTARRAY_V_INDEX;
	    return;
	  case GL_NORMAL_ARRAY_EXT:
	    pGC->vertexArray.mask &= ~VERTARRAY_N_MASK;
	    pGC->vertexArray.index &= ~VERTARRAY_N_INDEX;
	    return;
	  case GL_COLOR_ARRAY_EXT:
	    pGC->vertexArray.mask &= ~VERTARRAY_C_MASK;
	    pGC->vertexArray.index &= ~VERTARRAY_C_INDEX;
	    return;
	  case GL_INDEX_ARRAY_EXT:
	    pGC->vertexArray.mask &= ~VERTARRAY_I_MASK;
	    pGC->vertexArray.index &= ~VERTARRAY_I_INDEX;
	    return;
	  case GL_TEXTURE_COORD_ARRAY_EXT:
	    pGC->vertexArray.textureEnables &=
	    				~(1<<pGC->clientTexture.activeTexture);
	    if (pGC->vertexArray.textureEnables == 0) {
		pGC->vertexArray.mask &= ~VERTARRAY_T_MASK;
		pGC->vertexArray.index &= ~VERTARRAY_T_INDEX;
	    }
	    return;
	  case GL_EDGE_FLAG_ARRAY_EXT:
	    pGC->vertexArray.mask &= ~VERTARRAY_E_MASK;
	    pGC->vertexArray.index &= ~VERTARRAY_E_INDEX;
	    return;
#endif
	}
    }

    data = (struct __gllc_Disable_Rec *) __glSPDlalloc(sizeof(struct __gllc_Disable_Rec), __glop_Disable);
    if (data == NULL) return;
    data->cap = cap;
}

void __glsplc_Enable(GLenum cap)
{
    struct __gllc_Enable_Rec *data;

    if (( cap >= GL_VERTEX_ARRAY_EXT ) && ( cap <= GL_EDGE_FLAG_ARRAY_EXT)) {
        __GLcontext *pGC = (__GLcontext *)__gl_context;
	
	switch (cap) {
#if 0
	case GL_VERTEX_ARRAY_EXT:
	    pGC->vertexArray.mask |= VERTARRAY_V_MASK;
	    pGC->vertexArray.index |= VERTARRAY_V_INDEX;
	    return;
	case GL_NORMAL_ARRAY_EXT:
	    pGC->vertexArray.mask |= VERTARRAY_N_MASK;
	    pGC->vertexArray.index |= VERTARRAY_N_INDEX;
	    return;
	case GL_COLOR_ARRAY_EXT:
	    pGC->vertexArray.mask |= VERTARRAY_C_MASK;
	    pGC->vertexArray.index |= VERTARRAY_C_INDEX;
	    return;
	case GL_INDEX_ARRAY_EXT:
	    pGC->vertexArray.mask |= VERTARRAY_I_MASK;
	    pGC->vertexArray.index |= VERTARRAY_I_INDEX;
	    return;
	case GL_TEXTURE_COORD_ARRAY_EXT:
	    pGC->vertexArray.textureEnables |=
	    				(1<<pGC->clientTexture.activeTexture);
	    pGC->vertexArray.mask |= VERTARRAY_T_MASK;
	    pGC->vertexArray.index |= VERTARRAY_T_INDEX;
	    return;
	case GL_EDGE_FLAG_ARRAY_EXT:
	    pGC->vertexArray.mask |= VERTARRAY_E_MASK;
	    pGC->vertexArray.index |= VERTARRAY_E_INDEX;
	    return;
#endif
	}
    }
    data = (struct __gllc_Enable_Rec *) __glSPDlalloc(sizeof(struct __gllc_Enable_Rec), __glop_Enable);
    if (data == NULL) return;
    data->cap = cap;
}

void 
__glsplc_ColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid* table)
{
    GLuint size;
    GLint tableSize;
    struct __gllc_ColorTable_Rec *data;

    switch (target) {
      case GL_PROXY_TEXTURE_1D:
      case GL_PROXY_TEXTURE_2D:
      case GL_PROXY_TEXTURE_3D:
      case GL_PROXY_COLOR_TABLE:
      case GL_PROXY_POST_CONVOLUTION_COLOR_TABLE:
      case GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE:
	(*gc->dispatchState->dispatch.ColorTable)(target, internalformat, 
						  width, format, type, table);
						  
	return;
    }
    
    if((rvalue = __glCheckColorTableArgs(gc, target, internalformat, width,
					 format, type))) {
	switch(rvalue) {
	case GL_INVALID_ENUM:
	    __glsplc_InvalidEnum(gc);
	    return;
	case GL_INVALID_VALUE:
	    __glsplc_InvalidEnum(gc);
	    return;
	}
    }

    tableSize = __glImageSize(width, 1, format, type);
    tableSize = __GL_PAD(tableSize);
    size = (int)sizeof(*data) + tableSize;
    data = (struct __gllc_ColorTable_Rec *) __glSPDlalloc(size, __glop_ColorTable);
    if (data == NULL) return;
    data->target = target;
    data->internalformat = internalformat;
    data->width = width;
    data->format = format;
    data->type = type;
    data->imageSize = tableSize;
    __GL_MEMCOPY((GLubyte *)data + sizeof(*data), table, tableSize);
}

void 
__glsplc_ColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid* table)
{
    GLuint size;
    GLint tableSize;
    struct __gllc_ColorSubTable_Rec *data;

    if((rvalue = __glCheckColorSubTableArgs(gc, target, start, count,
					 format, type))) {
	switch(rvalue) {
	case GL_INVALID_ENUM:
	    __glsplc_InvalidEnum(gc);
	    return;
	case GL_INVALID_VALUE:
	    __glsplc_InvalidEnum(gc);
	    return;
	}
    }

    tableSize = __glImageSize(width, 1, format, type);
    tableSize = __GL_PAD(tableSize);
    size = (int)sizeof(*data) + tableSize;
    data = (struct __gllc_ColorSubTable_Rec *) __glSPDlalloc(size, __glop_ColorSubTable);
    if (data == NULL) return;
    data->target = target;
    data->start = start;
    data->count = count;
    data->format = format;
    data->type = type;
    data->imageSize = tableSize;
    __GL_MEMCOPY((GLubyte *)data + sizeof(*data), table, tableSize);
}

void __glsplc_DrawArrays(GLenum mode, GLint first, GLsizei count)
{
     __GL_SETUP();
    GLint index = gc->vertexArray.index;
    int i;

    if (mode > GL_POLYGON) {
        __glsplc_InvalidEnum();
        return;
    }
    if (count < 0) {
        __glsplc_InvalidValue();
        return;
    }
    glBegin(mode);
    count += first;
    for (i = first; i < count; i++) {
	if (index & VERTARRAY_N_INDEX) 
	    (*gc->vertexArray.np.proc)
		(gc->vertexArray.np.pointer + i*gc->vertexArray.np.stride);
	if (index & VERTARRAY_C_INDEX) 
	    (*gc->vertexArray.cp.proc)
		(gc->vertexArray.cp.pointer + i*gc->vertexArray.cp.stride);
	if (index & VERTARRAY_T_INDEX) {
	    GLint j;

	    for (j=0; j<__GL_NUM_TEXTURE_UNITS; ++j) {
	        if (gc->vertexArray.textureEnables & (1<<j)) {
		    (*gc->vertexArray.tp[j].proc)
			(gc->vertexArray.tp[j].pointer +
				i*gc->vertexArray.tp[j].stride);
		}
	    }
	}
	if (index & VERTARRAY_I_INDEX) 
	    (*gc->vertexArray.ip.proc)
		(gc->vertexArray.ip.pointer + i*gc->vertexArray.ip.stride);
	if (index & VERTARRAY_E_INDEX)
	    (*gc->vertexArray.ep.proc)
		(gc->vertexArray.ep.pointer + i*gc->vertexArray.ep.stride);
	if (index & VERTARRAY_V_INDEX) 
	    (*gc->vertexArray.vp.proc)
		(gc->vertexArray.vp.pointer + i*gc->vertexArray.vp.stride);
    }
    glEnd();
}

void __glsplc_DrawElements(GLenum mode, GLsizei count, GLenum type, 
			   const GLvoid *indices)
{
    __GL_SETUP();

    if (count < 0) {
        __gllc_InvalidValue(gc);
        return;
    }
    if (mode > GL_POLYGON || (type != GL_UNSIGNED_BYTE &&
			      type != GL_UNSIGNED_SHORT &&
			      type != GL_UNSIGNED_INT)) {
        __gllc_InvalidEnum(gc);
	return;
    }
    glBegin(mode);
    switch (type) {
    int i;
    case GL_UNSIGNED_BYTE:
        for (i = 0; i < count; i++) {
	    glArrayElement( ((GLubyte *)indices)[i]);
	} 
	break;
    case GL_UNSIGNED_SHORT:
        for (i = 0; i < count; i++) {
	    glArrayElement( ((GLushort *)indices)[i]);
	} 
	break;
    case GL_UNSIGNED_INT:
        for (i = 0; i < count; i++) {
	    glArrayElement( ((GLuint *)indices)[i]);
	} 
	break;
    }
    glEnd();
}

void __glsplc_DrawRangeElements(GLenum mode, GLuint start, GLuint end,
				GLsizei count, GLenum type, 
				const GLvoid *indices)
{
    __GL_SETUP();

    if (end < start) {
        __gllc_InvalidValue(gc);
        return;
    }

    __glsplc_DrawElements(mode, count, type, indices);
}

#endif /* NEW_LISTS */
