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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_lcomp.c#21 $
*/

#include "render.h"
#include "global.h"
#include "context.h"
#include "pixel.h"
#include "g_lcomp.h"
#include "g_listop.h"
#include "g_lcfncs.h"
#include "g_imfncs.h"
#include "image.h"
#include "dlist.h"
#include "dlistopt.h"
#include "g_imports.h"
#include "ctable.h"

#define __GL_IMAGE_LE_FUNC(inCmd) __glle_##inCmd

/*
** The code in here makes a lot of assumptions about the size of the 
** various user types (GLfloat, GLint, etcetra).  
*/

void GLAPI __gllc_Bitmap(GLsizei width, GLsizei height,
			 GLfloat xorig, GLfloat yorig, 
			 GLfloat xmove, GLfloat ymove, 
			 const GLubyte *oldbits)
{
    __GLdlistOp *dlop;
    __GLbitmap *bitmap;
    GLubyte *newbits;
    GLint imageSize;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__gllc_InvalidValue(gc);
	return;
    }

    imageSize = height * ((width + 7) >> 3);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(imageSize + sizeof(__GLbitmap)));
    if (dlop == NULL) return;
    dlop->opcode = __glop_Bitmap;

    bitmap = (__GLbitmap *) dlop->data;
    bitmap->width = width;
    bitmap->height = height;
    bitmap->xorig = xorig;
    bitmap->yorig = yorig;
    bitmap->xmove = xmove;
    bitmap->ymove = ymove;
    bitmap->imageSize = imageSize;

    newbits = &dlop->data[0] + sizeof(__GLbitmap); 
    __glFillImage(gc, width, height, GL_COLOR_INDEX, GL_BITMAP, 
	    oldbits, newbits);

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(Bitmap));
}

const GLubyte *__glle_Bitmap(const GLubyte *PC)
{
    const __GLbitmap *bitmap;
    GLuint beginMode;
    __GL_SETUP();
    __GL_API_NOTBE_RENDER();

    bitmap = (const __GLbitmap *) PC;

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return PC + sizeof(__GLbitmap) + bitmap->imageSize;
	}
    }

    (*gc->procs.renderBitmap)(gc, bitmap, (const GLubyte *) (bitmap+1));

    return PC + sizeof(__GLbitmap) + bitmap->imageSize;
}

static void __gllei_PolygonStipple(__GLcontext *gc, const GLubyte *bits)
{
    if (__GL_IN_BEGIN()) {
        __glSetError(GL_INVALID_OPERATION);
        return;
    }

    /* 
    ** Just copy bits into stipple, convertPolygonStipple() will do the rest.
    */
    __GL_MEMCOPY(&gc->state.polygonStipple.stipple[0], bits,
		 sizeof(gc->state.polygonStipple.stipple));
    (*gc->procs.convertPolygonStipple)(gc);
}

void GLAPI __gllc_PolygonStipple(const GLubyte *mask)
{
    __GLdlistOp *dlop;
    __GL_SETUP();
    GLubyte *newbits;

    dlop = __glDlistAllocOp2(gc, 
	    __glImageSize(32, 32, GL_COLOR_INDEX, GL_BITMAP));
    if (dlop == NULL) return;
    dlop->opcode = __glop_PolygonStipple;

    newbits = (GLubyte *) dlop->data;
    __glFillImage(gc, 32, 32, GL_COLOR_INDEX, GL_BITMAP, mask, newbits);

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(PolygonStipple));
}

const GLubyte *__glle_PolygonStipple(const GLubyte *PC)
{
    __GL_SETUP();
    __GL_API_STATE();

    __gllei_PolygonStipple(gc, (const GLubyte *) (PC));
    return PC + __glImageSize(32, 32, GL_COLOR_INDEX, GL_BITMAP);
}

struct __gllc_Map1f_Rec {
        GLenum    target;
        __GLfloat u1;
        __GLfloat u2;
        GLint     order;
        /*        points  */
};

void GLAPI __gllc_Map1f(GLenum target, 
			GLfloat u1, GLfloat u2,
			GLint stride, GLint order,
			const GLfloat *points)
{
    __GLdlistOp *dlop;
    struct __gllc_Map1f_Rec *map1data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();
    
    k=__glEvalComputeK(target);
    if (k < 0) {
	__gllc_InvalidEnum(gc);
	return;
    }

    if (order > gc->constants.maxEvalOrder || stride < k ||
	    order < 1 || u1 == u2) {
	__gllc_InvalidValue(gc);
	return;
    }

    cmdsize = (GLint)sizeof(*map1data) + 
	    __glMap1_size(k, order) * (GLint)sizeof(__GLfloat);

    dlop = __glDlistAllocOp2(gc, cmdsize);
    if (dlop == NULL) return;
    dlop->opcode = __glop_Map1f;

    map1data = (struct __gllc_Map1f_Rec *) dlop->data;
    map1data->target = target;
    map1data->u1 = u1;
    map1data->u2 = u2;
    map1data->order = order;
    data = (__GLfloat *) (&dlop->data[0] + sizeof(*map1data));
    __glFillMap1f(k, order, stride, points, data);

    __glDlistAppendOp(gc, dlop, __glle_Map1);
}

const GLubyte *__glle_Map1(const GLubyte *PC)
{
    const struct __gllc_Map1f_Rec *map1data;
    GLint k;
    __GL_SETUP();

    map1data = (const struct __gllc_Map1f_Rec *) PC;
    k = __glEvalComputeK(map1data->target);

    /* Stride of "k" matches internal stride */
#ifdef __GL_DOUBLE
    (*gc->dispatchState->dispatch.Map1d)
#else /* __GL_DOUBLE */
    (*gc->dispatchState->dispatch.Map1f)
#endif /* __GL_DOUBLE */
	    (map1data->target, map1data->u1, map1data->u2,
	    k, map1data->order, (const __GLfloat *)(PC + sizeof(*map1data)));

    return PC + sizeof(*map1data) + 
	    __glMap1_size(k, map1data->order) * sizeof(__GLfloat);
}

void GLAPI __gllc_Map1d(GLenum target, 
			GLdouble u1, GLdouble u2,
			GLint stride, GLint order, 
			const GLdouble *points)
{
    __GLdlistOp *dlop;
    struct __gllc_Map1f_Rec *map1data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();
    
    k=__glEvalComputeK(target);
    if (k < 0) {
	__gllc_InvalidEnum(gc);
	return;
    }

    if (order > gc->constants.maxEvalOrder || stride < k ||
	    order < 1 || u1 == u2) {
	__gllc_InvalidValue(gc);
	return;
    }

    cmdsize = (GLint)sizeof(*map1data) + 
	    __glMap1_size(k, order) * (GLint)sizeof(__GLfloat);

    dlop = __glDlistAllocOp2(gc, cmdsize);
    if (dlop == NULL) return;
    dlop->opcode = __glop_Map1d;

    map1data = (struct __gllc_Map1f_Rec *) dlop->data;
    map1data->target = target;
    map1data->u1 = u1;
    map1data->u2 = u2;
    map1data->order = order;
    data = (__GLfloat *) (&dlop->data[0] + sizeof(*map1data));
    __glFillMap1d(k, order, stride, points, data);

    __glDlistAppendOp(gc, dlop, __glle_Map1);
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

void GLAPI __gllc_Map2f(GLenum target, 
			GLfloat u1, GLfloat u2,
			GLint ustride, GLint uorder, 
			GLfloat v1, GLfloat v2,
			GLint vstride, GLint vorder, 
			const GLfloat *points)
{
    __GLdlistOp *dlop;
    struct __gllc_Map2f_Rec *map2data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();

    k=__glEvalComputeK(target);
    if (k < 0) {
	__gllc_InvalidEnum(gc);
	return;
    }

    if (vorder > gc->constants.maxEvalOrder || vstride < k ||
	    vorder < 1 || u1 == u2 || ustride < k ||
	    uorder > gc->constants.maxEvalOrder || uorder < 1 ||
	    v1 == v2) {
	__gllc_InvalidValue(gc);
	return;
    }

    cmdsize = (GLint)sizeof(*map2data) + 
	    __glMap2_size(k, uorder, vorder) * (GLint)sizeof(__GLfloat);

    dlop = __glDlistAllocOp2(gc, cmdsize);
    if (dlop == NULL) return;
    dlop->opcode = __glop_Map2f;

    map2data = (struct __gllc_Map2f_Rec *) dlop->data;
    map2data->target = target;
    map2data->u1 = u1;
    map2data->u2 = u2;
    map2data->uorder = uorder;
    map2data->v1 = v1;
    map2data->v2 = v2;
    map2data->vorder = vorder;

    data = (__GLfloat *) (&dlop->data[0] + sizeof(*map2data));
    __glFillMap2f(k, uorder, vorder, ustride, vstride, points, data);

    __glDlistAppendOp(gc, dlop, __glle_Map2);
}

const GLubyte *__glle_Map2(const GLubyte *PC)
{
    const struct __gllc_Map2f_Rec *map2data;
    GLint k;
    __GL_SETUP();

    map2data = (const struct __gllc_Map2f_Rec *) PC;
    k = __glEvalComputeK(map2data->target);

    /* Stride of "k" and "k * vorder" matches internal strides */
#ifdef __GL_DOUBLE
    (*gc->dispatchState->dispatch.Map2d)
#else /* __GL_DOUBLE */
    (*gc->dispatchState->dispatch.Map2f)
#endif /* __GL_DOUBLE */
	    (map2data->target, 
	    map2data->u1, map2data->u2, k * map2data->vorder, map2data->uorder,
	    map2data->v1, map2data->v2, k, map2data->vorder,
	    (const __GLfloat *)(PC + sizeof(*map2data)));
    
    return PC + sizeof(*map2data) + 
	    __glMap2_size(k, map2data->uorder, map2data->vorder) * 
	    sizeof(__GLfloat);
}

void GLAPI __gllc_Map2d(GLenum target, 
			GLdouble u1, GLdouble u2,
			GLint ustride, GLint uorder, 
			GLdouble v1, GLdouble v2,
			GLint vstride, GLint vorder, 
			const GLdouble *points)
{
    __GLdlistOp *dlop;
    struct __gllc_Map2f_Rec *map2data;
    GLint k;
    GLint cmdsize;
    __GLfloat *data;
    __GL_SETUP();

    k=__glEvalComputeK(target);
    if (k < 0) {
	__gllc_InvalidEnum(gc);
	return;
    }

    if (vorder > gc->constants.maxEvalOrder || vstride < k ||
	    vorder < 1 || u1 == u2 || ustride < k ||
	    uorder > gc->constants.maxEvalOrder || uorder < 1 ||
	    v1 == v2) {
	__gllc_InvalidValue(gc);
	return;
    }

    cmdsize = (GLint)sizeof(*map2data) + 
	    __glMap2_size(k, uorder, vorder) * (GLint)sizeof(__GLfloat);

    dlop = __glDlistAllocOp2(gc, cmdsize);
    if (dlop == NULL) return;
    dlop->opcode = __glop_Map2d;

    map2data = (struct __gllc_Map2f_Rec *) dlop->data;
    map2data->target = target;
    map2data->u1 = u1;
    map2data->u2 = u2;
    map2data->uorder = uorder;
    map2data->v1 = v1;
    map2data->v2 = v2;
    map2data->vorder = vorder;

    data = (__GLfloat *) (&dlop->data[0] + sizeof(*map2data));
    __glFillMap2d(k, uorder, vorder, ustride, vstride, points, data);

    __glDlistAppendOp(gc, dlop, __glle_Map2);
}

const GLubyte *__glle_DrawPixels(const GLubyte *PC)
{
    const struct __gllc_DrawPixels_Rec *pixdata;
    GLint imageSize;
    GLuint beginMode;
    __GL_SETUP();
    __GL_API_NOTBE_RENDER();

    pixdata = (const struct __gllc_DrawPixels_Rec *) PC;
    imageSize = __glImageSize(pixdata->width, pixdata->height, 
			      pixdata->format, pixdata->type);

    beginMode = __gl_beginMode;
    if (beginMode != __GL_NOT_IN_BEGIN) {
	if (beginMode == __GL_NEED_VALIDATE) {
	    (*gc->procs.validate)(gc);
	    __gl_beginMode = __GL_NOT_IN_BEGIN;
	} else {
	    __glSetError(GL_INVALID_OPERATION);
	    return PC + sizeof(*pixdata) + __GL_PAD(imageSize);
	}
    }

    if (!gc->state.current.validRasterPos) {
	return PC + sizeof(*pixdata) + __GL_PAD(imageSize);
    }

    if (gc->renderMode == GL_FEEDBACK) {
	__glFeedbackDrawPixels(gc, &gc->state.current.rasterPos);
	return PC + sizeof(*pixdata) + __GL_PAD(imageSize);
    }

    if (gc->renderMode == GL_RENDER) {
	(*gc->procs.drawPixels)(gc, pixdata->width, pixdata->height, 
				pixdata->format, pixdata->type, 
				(const GLubyte *)(PC + sizeof(*pixdata)), 
				GL_TRUE);
    }

    return PC + sizeof(*pixdata) + __GL_PAD(imageSize);
}

void GLAPI __gllc_DrawPixels(GLint width, GLint height, GLenum format, 
			     GLenum type, const GLvoid *pixels)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_DrawPixels_Rec *pixdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize(width, height, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*pixdata) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_DrawPixels;

    pixdata = (struct __gllc_DrawPixels_Rec *) dlop->data;
    pixdata->width = width;
    pixdata->height = height;
    pixdata->format = format;
    pixdata->type = type;

    __glFillImage(gc, width, height, adjustFormat, adjustType, pixels, 
	    &dlop->data[0] + sizeof(*pixdata));

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(DrawPixels));
}

const GLubyte *__glle_TexImage1D(const GLubyte *PC)
{
    const struct __gllc_TexImage1D_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_TexImage1D_Rec *) PC;
    __gllei_TexImage1D(gc, data->target, data->level, data->internalformat,
		       data->width, data->border, data->format, data->type, 
		       (const GLubyte *)(PC + sizeof(*data)));

    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

const GLubyte *__glle_TexImage2D(const GLubyte *PC)
{
    const struct __gllc_TexImage2D_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_TexImage2D_Rec *) PC;
    __gllei_TexImage2D(gc, data->target, data->level, data->internalformat,
		       data->width, data->height, data->border,
		       data->format, data->type,
		       (const GLubyte *)(PC + sizeof(*data)));

    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

const GLubyte *__glle_TexImage3D(const GLubyte *PC)
{
    __GL_SETUP();
    const struct __gllc_TexImage3D_Rec *data;

    data = (const struct __gllc_TexImage3D_Rec *)PC;
    __gllei_TexImage3D(gc, data->target, data->level, data->internalformat,
                       data->width, data->height, data->depth, data->border,
                       data->format, data->type,
                       (const GLubyte *)(PC + sizeof(*data)));

    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

void GLAPI __gllc_TexImage1D(GLenum target, GLint level, 
			     GLint internalformat,
			     GLint width, GLint border, GLenum format, 
			     GLenum type, const GLvoid *pixels)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_TexImage1D_Rec *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if (target == GL_PROXY_TEXTURE_1D) {
	(*gc->dispatchState->dispatch.TexImage1D)(target, level, 
						  internalformat, width, 
						  border, format, type, 
						  pixels);
	return;
    }

    if (border < 0 || border > 1) {
	__gllc_InvalidValue(gc);
	return;
    }
    if (width < 0) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize(width, 1, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*texdata) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_TexImage1D;

    texdata = (struct __gllc_TexImage1D_Rec *) dlop->data;
    texdata->target = target;
    texdata->level = level;
    texdata->internalformat = internalformat;
    texdata->width = width;
    texdata->border = border;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0 && pixels != NULL) {
	__glFillImage(gc, width, 1, adjustFormat, adjustType, pixels, 
		&dlop->data[0] + sizeof(*texdata));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(TexImage1D));
}

void GLAPI __gllc_TexImage2D(GLenum target, GLint level, 
			     GLint internalformat,
			     GLint width, GLint height, GLint border, 
			     GLenum format, GLenum type, 
			     const GLvoid *pixels)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_TexImage2D_Rec  *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if (target == GL_PROXY_TEXTURE_2D) {
	(*gc->dispatchState->dispatch.TexImage2D)(target, level, 
						  internalformat, 
						  width, height, border, 
						  format, type, pixels);
						  
	return;
    }

    if (border < 0 || border > 1) {
	__gllc_InvalidValue(gc);
	return;
    }
    if ((width < 0) || (height < 0)) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize(width, height, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*texdata) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_TexImage2D;

    texdata = (struct __gllc_TexImage2D_Rec *) dlop->data;
    texdata->target = target;
    texdata->level = level;
    texdata->internalformat = internalformat;
    texdata->width = width;
    texdata->height = height;
    texdata->border = border;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0 && pixels != NULL) {
	__glFillImage(gc, width, height, adjustFormat, adjustType, pixels, 
		(GLubyte *) &dlop->data[0] + (int)sizeof(*texdata));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(TexImage2D));
}

extern void GLAPI __gllc_TexImage3D(GLenum target, GLint level,
                                    GLenum internalformat, GLsizei width,
                                    GLsizei height, GLsizei depth,
                                    GLint border, GLenum format,
                                    GLenum type, const GLvoid *pixels)
{
    __GLdlistOp *dlop;
    struct __gllc_TexImage3D_Rec *texdata;
    GLint imageSize;
    __GL_SETUP();

    if (target == GL_PROXY_TEXTURE_3D) {
	(*gc->dispatchState->dispatch.TexImage3D)(
				target, level, internalformat,
				width, height, depth, border,
				format, type, pixels);
	return;
    }

    if (border < 0 || border > 1) {
	__gllc_InvalidValue(gc);
	return;
    }
    if ((width < 0) || (height < 0) || (depth < 0)) {
	__gllc_InvalidValue(gc);
	return;
    }
    switch (format) {
      case GL_COLOR_INDEX:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_BGR:
      case GL_RGBA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_BGRA:
      case GL_ABGR_EXT:
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
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
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_BGRA:
	  case GL_ABGR_EXT:
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize3D(width, height, depth, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, sizeof(*texdata) + imageSize);
    if (dlop == NULL) return;
    dlop->opcode = __glop_TexImage3D;

    texdata = (struct __gllc_TexImage3D_Rec *)dlop->data;
    texdata->target = target;
    texdata->level = level;
    texdata->internalformat = internalformat;
    texdata->width = width;
    texdata->height = height;
    texdata->depth = depth;
    texdata->border = border;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0 && pixels != NULL) {
	__glFillImage3D(gc, width, height, depth, format, type, pixels, 
		        (GLubyte *)&dlop->data[0] + (int)sizeof(*texdata));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(TexImage3D));
}

const GLubyte *__glle_TexSubImage1D(const GLubyte *PC)
{
    const struct __gllc_TexSubImage1D_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_TexSubImage1D_Rec *) PC;
    __gllei_TexSubImage1D(gc, data->target, data->level, 
			     data->xoffset, data->width,
			     data->format, data->type, 
			  (const GLubyte *)(PC + sizeof(*data)));
    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

void GLAPI __gllc_TexSubImage1D(GLenum target, GLint level,
				GLint xoffset, GLsizei width,
				GLenum format, GLenum type, const GLvoid *pixels)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_TexSubImage1D_Rec *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if (width < 0) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize(width, 1, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*texdata) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_TexSubImage1D;

    texdata = (struct __gllc_TexSubImage1D_Rec *) dlop->data;
    texdata->target = target;
    texdata->level = level;
    texdata->xoffset = xoffset;
    texdata->width = width;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0) {
	__glFillImage(gc, width, 1, adjustFormat, adjustType, pixels, 
		(GLubyte *) &dlop->data[0] + sizeof(*texdata));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(TexSubImage1D));
}

const GLubyte *__glle_TexSubImage2D(const GLubyte *PC)
{
    const struct __gllc_TexSubImage2D_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_TexSubImage2D_Rec *) PC;
    __gllei_TexSubImage2D(gc, data->target, data->level,
			     data->xoffset, data->yoffset,
			     data->width, data->height,
			     data->format, data->type,
		       (const GLubyte *)(PC + sizeof(*data)));
    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

void GLAPI __gllc_TexSubImage2D(GLenum target, GLint level,
				GLint xoffset, GLint yoffset,
				GLsizei width, GLsizei height,
				GLenum format, GLenum type, const GLvoid *pixels)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_TexSubImage2D_Rec *texdata;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize(width, height, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*texdata) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_TexSubImage2D;

    texdata = (struct __gllc_TexSubImage2D_Rec *) dlop->data;
    texdata->target = target;
    texdata->level = level;
    texdata->xoffset = xoffset;
    texdata->yoffset = yoffset;
    texdata->width = width;
    texdata->height = height;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0) {
	__glFillImage(gc, width, height, adjustFormat, adjustType, pixels, 
		(GLubyte *) &dlop->data[0] + sizeof(*texdata));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(TexSubImage2D));
}

const GLubyte *__glle_TexSubImage3D(const GLubyte *PC)
{
    __GL_SETUP();
    const struct __gllc_TexSubImage3D_Rec *data;

    data = (const struct __gllc_TexSubImage3D_Rec *) PC;
    __gllei_TexSubImage3D(gc, data->target, data->level,
                          data->xoffset, data->yoffset, data->zoffset,
                          data->width, data->height, data->depth,
                          data->format, data->type,
                          (const GLubyte *)(PC + sizeof(*data)));
    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

void GLAPI __gllc_TexSubImage3D(GLenum target, GLint level,
                                GLint xoffset, GLint yoffset, GLint zoffset,
                                GLsizei width, GLsizei height, GLsizei depth,
                                GLenum format, GLenum type, const GLvoid *pixels)
{
    __GLdlistOp *dlop;
    struct __gllc_TexSubImage3D_Rec *texdata;
    GLint imageSize;
    __GL_SETUP();

    if ((width < 0) || (height < 0) || (depth < 0)) {
	__gllc_InvalidValue(gc);
	return;
    }
    switch (format) {
      case GL_COLOR_INDEX:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_BGR:
      case GL_RGBA:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_BGRA:
      case GL_ABGR_EXT:
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
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
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_BGRA:
	  case GL_ABGR_EXT:
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize3D(width, height, depth, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, sizeof(*texdata) + imageSize);
    if (dlop == NULL) return;
    dlop->opcode = __glop_TexSubImage3D;

    texdata = (struct __gllc_TexSubImage3D_Rec *)dlop->data;
    texdata->target = target;
    texdata->level = level;
    texdata->xoffset = xoffset;
    texdata->yoffset = yoffset;
    texdata->zoffset = zoffset;
    texdata->width = width;
    texdata->height = height;
    texdata->depth = depth;
    texdata->format = format;
    texdata->type = type;
    texdata->imageSize = imageSize;

    if (imageSize > 0) {
	__glFillImage3D(gc, width, height, depth, format, type, pixels, 
		        (GLubyte *)&dlop->data[0] + sizeof(*texdata));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(TexSubImage3D));
}

const GLubyte *__glle_Disable(const GLubyte *PC)
{
    const struct __gllc_Disable_Rec *data;

    data = (const struct __gllc_Disable_Rec *) PC;
    (*__gl_dispatch.dispatch.Disable)(data->cap);
    return PC + sizeof(struct __gllc_Disable_Rec);
}

void GLAPI __gllc_Disable(GLenum cap)
{
    __GLdlistOp *dlop;
    struct __gllc_Disable_Rec *data;
    __GL_SETUP();

    switch (cap) {
#if 0
      case GL_VERTEX_ARRAY_EXT:
	gc->vertexArray.mask &= ~VERTARRAY_V_MASK;
	gc->vertexArray.index &= ~VERTARRAY_V_INDEX;
	return;
      case GL_NORMAL_ARRAY_EXT:
	gc->vertexArray.mask &= ~VERTARRAY_N_MASK;
	gc->vertexArray.index &= ~VERTARRAY_N_INDEX;
	return;
      case GL_COLOR_ARRAY_EXT:
	gc->vertexArray.mask &= ~VERTARRAY_C_MASK;
	gc->vertexArray.index &= ~VERTARRAY_C_INDEX;
	return;
      case GL_INDEX_ARRAY_EXT:
	gc->vertexArray.mask &= ~VERTARRAY_I_MASK;
	gc->vertexArray.index &= ~VERTARRAY_I_INDEX;
	return;
      case GL_TEXTURE_COORD_ARRAY_EXT:
	gc->vertexArray.mask &= ~VERTARRAY_T_MASK;
	gc->vertexArray.index &= ~VERTARRAY_T_INDEX;
	return;
      case GL_EDGE_FLAG_ARRAY_EXT:
	gc->vertexArray.mask &= ~VERTARRAY_E_MASK;
	gc->vertexArray.index &= ~VERTARRAY_E_INDEX;
	return;
#endif
    }

    dlop = __glDlistAllocOp2(gc, sizeof(struct __gllc_Disable_Rec));
    if (dlop == NULL) return;
    dlop->opcode = __glop_Disable;
    data = (struct __gllc_Disable_Rec *) dlop->data;
    data->cap = cap;
    __glDlistAppendOp(gc, dlop, __glle_Disable);
}

const GLubyte *__glle_Enable(const GLubyte *PC)
{
    const struct __gllc_Enable_Rec *data;

    data = (const struct __gllc_Enable_Rec *) PC;
    (*__gl_dispatch.dispatch.Enable)(data->cap);
    return PC + sizeof(struct __gllc_Enable_Rec);
}

void GLAPI __gllc_Enable(GLenum cap)
{
    __GLdlistOp *dlop;
    struct __gllc_Enable_Rec *data;
    __GL_SETUP();

    switch (cap) {
#if 0
      case GL_VERTEX_ARRAY_EXT:
	gc->vertexArray.mask |= VERTARRAY_V_MASK;
	gc->vertexArray.index |= VERTARRAY_V_INDEX;
	return;
      case GL_NORMAL_ARRAY_EXT:
	gc->vertexArray.mask |= VERTARRAY_N_MASK;
	gc->vertexArray.index |= VERTARRAY_N_INDEX;
	return;
      case GL_COLOR_ARRAY_EXT:
	gc->vertexArray.mask |= VERTARRAY_C_MASK;
	gc->vertexArray.index |= VERTARRAY_C_INDEX;
	return;
      case GL_INDEX_ARRAY_EXT:
	gc->vertexArray.mask |= VERTARRAY_I_MASK;
	gc->vertexArray.index |= VERTARRAY_I_INDEX;
	return;
      case GL_TEXTURE_COORD_ARRAY_EXT:
	gc->vertexArray.mask |= VERTARRAY_T_MASK;
	gc->vertexArray.index |= VERTARRAY_T_INDEX;
	return;
      case GL_EDGE_FLAG_ARRAY_EXT:
	gc->vertexArray.mask |= VERTARRAY_E_MASK;
	gc->vertexArray.index |= VERTARRAY_E_INDEX;
	return;
#endif
    }

    dlop = __glDlistAllocOp2(gc, sizeof(struct __gllc_Enable_Rec));
    if (dlop == NULL) return;
    dlop->opcode = __glop_Enable;
    data = (struct __gllc_Enable_Rec *) dlop->data;
    data->cap = cap;
    __glDlistAppendOp(gc, dlop, __glle_Enable);
}

const GLubyte *__glle_DrawArraysEXT(const GLubyte *PC)
{
    return PC;
}

const GLubyte *__glle_DrawArrays(const GLubyte *PC)
{
    return PC;
}
const GLubyte *__glle_DrawElements(const GLubyte *PC)
{
    return PC;
}

const GLubyte *__glle_DrawRangeElements(const GLubyte *PC)
{
    return PC;
}

const GLubyte *__glle_ArrayElement(const GLubyte *PC)
{
    return PC;
}
const GLubyte *__glle_ArrayElementEXT(const GLubyte *PC)
{
    return PC;
}

const GLubyte *__glle_ColorTable(const GLubyte *PC)
{
    const struct __gllc_ColorTable_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_ColorTable_Rec *) PC;
    __gllei_ColorTable(gc, data->target, data->internalformat,
			    data->width, data->format, data->type,
			    (const GLvoid *) (PC + sizeof(*data)));
    return PC + (GLuint)sizeof(*data) + __GL_PAD(data->imageSize);
}


void GLAPI __gllc_ColorTable(GLenum target, GLenum internalformat,
				GLsizei width, GLenum format, GLenum type,
				const void *table)
{
    __GLdlistOp *dlop;
    struct __gllc_ColorTable_Rec *data;
    GLint imageSize;
    GLenum rvalue;
    GLubyte *tableContents;
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

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
	    __gllc_InvalidEnum(gc);
	    return;
	case GL_INVALID_VALUE:
	    __gllc_InvalidEnum(gc);
	    return;
	}
    }
    imageSize = __glImageSize(width, 1, format, type);
    imageSize = __GL_PAD(imageSize);
    
    dlop = __glDlistAllocOp2(gc, (GLuint)(imageSize + sizeof(*data)));
    if(dlop == NULL) return;
    dlop->opcode = __glop_ColorTable;
    data = (struct __gllc_ColorTable_Rec *)dlop->data;

    data->target = target;
    data->internalformat = internalformat;
    data->width = width;
    data->format = format;
    data->type = type;
    data->imageSize = imageSize;

    tableContents = &dlop->data[0] + (int)sizeof(*data);
    __glFillImage(gc, width, 1, format, type, table, tableContents);
    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(ColorTable));
}

const GLubyte *__glle_ColorSubTable(const GLubyte *PC)
{
    const struct __gllc_ColorSubTable_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_ColorSubTable_Rec *) PC;
    __gllei_ColorSubTable(gc, data->target, data->start,
			       data->count, data->format, data->type,
			       (const GLvoid *) (PC + sizeof(*data)));
    return PC + (GLuint)sizeof(*data) + __GL_PAD(data->imageSize);
}


void GLAPI __gllc_ColorSubTable(GLenum target, GLsizei start,
				   GLsizei count, GLenum format, GLenum type,
				   const void *table)
{
    __GLdlistOp *dlop;
    struct __gllc_ColorSubTable_Rec *data;
    GLint imageSize;
    GLubyte *tableContents;
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();
    
    if (__glCheckColorSubTableArgs(gc, target, start, count, format, type)) {
	__gllc_InvalidEnum(gc);
	return;
    }
    imageSize = __glImageSize(count, 1, format, type);
    imageSize = __GL_PAD(imageSize);
    
    dlop = __glDlistAllocOp2(gc, (GLuint)(imageSize + sizeof(*data)));
    if(dlop == NULL) return;
    dlop->opcode = __glop_ColorSubTable;
    data = (struct __gllc_ColorSubTable_Rec *)dlop->data;

    data->target = target;
    data->start = start;
    data->count = count;
    data->format = format;
    data->type = type;
    data->imageSize = imageSize;

    tableContents = &dlop->data[0] + (int)sizeof(*data);
    __glFillImage(gc, count, 1, format, type, table, tableContents);
    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(ColorSubTable));
}

const GLubyte *__glle_ConvolutionFilter1D(const GLubyte *PC)
{
    const struct __gllc_ConvolutionFilter1D_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_ConvolutionFilter1D_Rec *) PC;
    __gllei_ConvolutionFilter1D(gc, data->target, data->internalformat,
		       data->width, data->format, data->type, 
		       (const GLubyte *)(PC + sizeof(*data)));

    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

void GLAPI __gllc_ConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_ConvolutionFilter1D_Rec *data;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if (width < 0) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize(width, 1, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*data) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_TexImage1D;

    data = (struct __gllc_ConvolutionFilter1D_Rec *) dlop->data;
    data->target = target;
    data->internalformat = internalformat;
    data->width = width;
    data->format = format;
    data->type = type;
    data->imageSize = imageSize;

    if (imageSize > 0 && image != NULL) {
	__glFillImage(gc, width, 1, adjustFormat, adjustType, image, 
		&dlop->data[0] + sizeof(*data));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(ConvolutionFilter1D));
}

const GLubyte *__glle_ConvolutionFilter2D(const GLubyte *PC)
{
    const struct __gllc_ConvolutionFilter2D_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_ConvolutionFilter2D_Rec *) PC;
    __gllei_ConvolutionFilter2D(gc, data->target, data->internalformat,
		       data->width, data->height,
		       data->format, data->type,
		       (const GLubyte *)(PC + sizeof(*data)));

    return PC + sizeof(*data) + __GL_PAD(data->imageSize);
}

void GLAPI __gllc_ConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_ConvolutionFilter2D_Rec  *data;
    GLint imageSize;
    GLboolean index;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    imageSize = __glImageSize(width, height, format, type);
    imageSize = __GL_PAD(imageSize);

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*data) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_ConvolutionFilter2D;

    data = (struct __gllc_ConvolutionFilter2D_Rec *) dlop->data;
    data->target = target;
    data->internalformat = internalformat;
    data->width = width;
    data->height = height;
    data->format = format;
    data->type = type;
    data->imageSize = imageSize;

    if (imageSize > 0 && image != NULL) {
	__glFillImage(gc, width, height, adjustFormat, adjustType, image, 
		(GLubyte *) &dlop->data[0] + (int)sizeof(*data));
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(ConvolutionFilter2D));
}

const GLubyte *__glle_SeparableFilter2D(const GLubyte *PC)
{
    const struct __gllc_SeparableFilter2D_Rec *data;
    GLint rowSize, colSize;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_SeparableFilter2D_Rec *) PC;
    rowSize = __glImageSize(data->width, 1, data->format, data->type);
    rowSize = __GL_PAD(rowSize);
    colSize = __glImageSize(data->height, 1, data->format, data->type);
    colSize = __GL_PAD(colSize);
    __gllei_SeparableFilter2D(gc, data->target, data->internalformat,
		       data->width, data->height,
		       data->format, data->type,
		       (const GLubyte *)(PC + sizeof(*data)),
		       (const GLubyte *)(PC + sizeof(*data)) + rowSize);

    return PC + sizeof(*data) + rowSize + colSize;
}

void GLAPI __gllc_SeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *col)
{
    GLenum adjustFormat= format;
    GLenum adjustType= type;
    __GLdlistOp *dlop;
    struct __gllc_SeparableFilter2D_Rec  *data;
    GLint rowSize, colSize, imageSize;
    GLboolean index;
    __GL_SETUP();

    if ((width < 0) || (height < 0)) {
	__gllc_InvalidValue(gc);
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
	__gllc_InvalidEnum(gc);
	return;
    }
    switch (type) {
      case GL_BITMAP:
	if (!index) {
	    __gllc_InvalidEnum(gc);
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
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_BYTE;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
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
	  case GL_ABGR_EXT:
          case GL_BGRA:
	    adjustFormat= GL_LUMINANCE;	/* or anything that's 1 component */
	    adjustType= GL_UNSIGNED_SHORT;
            if (type == GL_UNSIGNED_INT_8_8_8_8 ||
		type == GL_UNSIGNED_INT_8_8_8_8_REV || 
                type == GL_UNSIGNED_INT_10_10_10_2 ||
                type == GL_UNSIGNED_INT_2_10_10_10_REV)
	       adjustType= GL_UNSIGNED_INT;
	    break;
	  default:
	    __glSetError(GL_INVALID_OPERATION);
	    return;
	}
	break;
      default:
	__gllc_InvalidEnum(gc);
	return;
    }

    rowSize = __glImageSize(width, 1, format, type);
    rowSize = __GL_PAD(rowSize);

    colSize = __glImageSize(1, height, format, type);
    colSize = __GL_PAD(colSize);

    imageSize = rowSize+colSize;

    dlop = __glDlistAllocOp2(gc, (GLuint)(sizeof(*data) + imageSize));
    if (dlop == NULL) return;
    dlop->opcode = __glop_SeparableFilter2D;

    data = (struct __gllc_SeparableFilter2D_Rec *) dlop->data;
    data->target = target;
    data->internalformat = internalformat;
    data->width = width;
    data->height = height;
    data->format = format;
    data->type = type;

    if (rowSize > 0 && row != NULL) {
	__glFillImage(gc, width, 1, adjustFormat, adjustType, row, 
		(GLubyte *) &dlop->data[0] + (int)sizeof(*data));
    }

    if (colSize > 0 && col != NULL) {
	__glFillImage(gc, 1, height, adjustFormat, adjustType, col, 
		(GLubyte *) &dlop->data[0] + (int)sizeof(*data) + rowSize);
    }

    __glDlistAppendOp(gc, dlop, __GL_IMAGE_LE_FUNC(SeparableFilter2D));
}

const GLubyte *__glle_Histogram(const GLubyte *PC)
{
    const struct __gllc_Histogram_Rec *data;
    __GL_SETUP();
    __GL_API_STATE();

    data = (const struct __gllc_Histogram_Rec *) PC;
    (*__gl_dispatch.dispatch.Histogram)(data->target, data->width, 
                            data->internalformat, data->sink);
    return PC + (GLuint)sizeof(*data);
}


void GLAPI __gllc_Histogram(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    __GLdlistOp *dlop;
    struct __gllc_Histogram_Rec *data;
    __GL_SETUP_NOT_IN_BEGIN_VALIDATE();

    switch (target) {
      case GL_PROXY_HISTOGRAM:
	(*gc->dispatchState->dispatch.Histogram)(target, width,
							internalformat, sink);
						  
	return;
    }
    
    dlop = __glDlistAllocOp2(gc, (GLuint)sizeof(*data));
    if(dlop == NULL) return;
    dlop->opcode = __glop_Histogram;
    data = (struct __gllc_Histogram_Rec *)dlop->data;

    data->target = target;
    data->width = width;
    data->internalformat = internalformat;
    data->sink = sink;

    __glDlistAppendOp(gc, dlop, __glle_Histogram);
}

