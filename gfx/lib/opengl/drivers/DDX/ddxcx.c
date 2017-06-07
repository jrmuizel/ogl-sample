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
** $Header: //depot/main/gfx/lib/opengl/drivers/DDX/ddxcx.c#8 $
*/

#include <stdio.h>
#include "ddxcx.h"
#include "g_imports.h"
#include "machdep.h"
#include "global.h"
#include "g_imfncs.h"
#include "dlistopt.h"
#include "g_disp.h"
#include "texmgr.h"
#include "ddxci.h"
#include "ddxrgb.h"

#include "glxserver.h"	/* for __GLXdrawablePrivate */

extern GCPtr __glXFBGetGC(__GLdrawableBuffer *buf);


/*****************************************************************************/

Bool __glDdxScreenProbe(int screen)
{
    return 1;
}
/************************************************************************/

static void CheckViewport(__GLcontext *gc)
{
    GLint llx, lly, urx, ury;
    GLint x0, x1, y0, y1;
    GLboolean oldReasonableViewport;

    /*
    ** If this viewport is fully contained in the window, we note this fact,
    ** and this can save us on scissoring tests.
    */
    x0 = gc->transform.clipX0;
    x1 = gc->transform.clipX1;
    y0 = gc->transform.clipY0;
    y1 = gc->transform.clipY1;

#ifdef __GL_ALIGNED_BUFFERS
    llx = gc->state.viewport.x + gc->constants.viewportXAdjustOffset;
    lly = gc->state.viewport.y + gc->constants.viewportYAdjustOffset;
#else
    llx = gc->state.viewport.x + gc->constants.viewportXAdjust;
    lly = gc->state.viewport.y + gc->constants.viewportYAdjust;
#endif
    urx = llx + gc->state.viewport.width;
    ury = lly + gc->state.viewport.height;

    oldReasonableViewport = gc->transform.reasonableViewport;
    if (llx >= x0 && lly >= y0 && urx <= x1 && ury <= y1) {
	gc->transform.reasonableViewport = GL_TRUE;
    } else {
	gc->transform.reasonableViewport = GL_FALSE;
    }
    if (oldReasonableViewport != gc->transform.reasonableViewport) {
	/*
	** Normally resizes occur outside begin/end, but there are some
	** viewport validations that happen inside a begin (those ones
	** are caused by a resize initiated by a drawable lock due to a
	** window resize possibly traceable to another thread).  Handle
	** both cases here (do the appropriate validation).
	*/
	if (__gl_beginMode == __GL_IN_BEGIN) {
	    __GL_VALIDATE(gc);
	} else {
	    __GL_DELAY_VALIDATE(gc);
	}
    }
}

static GLboolean ApplyViewport(__GLcontext *gc)
{
    /* first resize the drawablePrivate buffer */
    (*gc->drawablePrivate->resize)(gc->drawablePrivate);

    if (__glResizeWindow(gc) == GL_TRUE) {
	__glUpdateViewport(gc);
	CheckViewport(gc);
	return GL_TRUE;
    }

    return GL_FALSE;
}

static GLboolean ApplyScissor(__GLcontext *gc)
{
    /* first resize the drawablePrivate buffer */
    (*gc->drawablePrivate->resize)(gc->drawablePrivate);

    if (__glResizeWindow(gc) == GL_TRUE) {
	__glUpdateViewport(gc);
	CheckViewport(gc);
	return GL_TRUE;
    }

    return GL_FALSE;
}

static GLboolean NotifyResize(__GLcontext *gc)
{
    if (__glResizeWindow(gc) == GL_TRUE) {
	__glUpdateViewport(gc);
	CheckViewport(gc);
	return GL_TRUE;
    }

    return GL_FALSE;
}

static void NotifyDestroy(__GLcontext *gc)
{
    /*
    ** Just like a resize, but we resize to zero size.  This way it's
    ** like drawing to a null window.  When the context is not current
    ** anymore, then we'll destroy the drawable
    */
    NotifyResize(gc);
}

/*
** GL_TRUE means we don't have to lock buffers
** GL_FALSE means we have to lock buffers
*/
static GLboolean ObtainSLock(__GLcontext *gc)
{
    return GL_TRUE;
}

/*
** GL_TRUE means we don't have to lock buffers
** GL_FALSE means we have to lock buffers
*/
static GLboolean ReleaseSLock(__GLcontext *gc)
{
    return GL_TRUE;
}

static void LockDevice(__GLcontext *gc)
{
}

static void UnlockDevice(__GLcontext *gc)
{
}

/************************************************************************/

static void Finish(__GLcontext *gc)
{
    __GL_API_FLUSH();
}

static void Flush(__GLcontext *gc)
{
    __GL_API_FLUSH();
}

static void WaitIdle(__GLcontext *gc)

{
    /* This is a noop in a software implementation */
}

/************************************************************************/

/*
** initialize context pointers
*/
static void InitFnPtrs(__GLcontext *gc)
{
    gc->procs.ec1 = __glDoEvalCoord1;
    gc->procs.ec2 = __glDoEvalCoord2;
    gc->procs.bitmap = __glDrawBitmap;
    gc->procs.rect = __glRect;
    gc->procs.clipPolygon = __glClipPolygon;
    gc->procs.validate = __glGenericValidate;
    gc->procs.convertPolygonStipple = __glConvertStipple;

    gc->procs.pushMatrix = __glPushModelViewMatrix;
    gc->procs.popMatrix = __glPopModelViewMatrix;
    gc->procs.loadIdentity = __glLoadIdentityModelViewMatrix;

    gc->procs.matrix.copy = __glCopyMatrix;
    gc->procs.matrix.invertTranspose = __glInvertTransposeMatrix;
    gc->procs.matrix.makeIdentity = __glMakeIdentity;
#if defined(__GL_USE_ASMCODE) && defined(mips)
    gc->procs.matrix.mult = __glMipsMultMatrix;
#else
    gc->procs.matrix.mult = __glMultMatrix;
#endif
    gc->procs.computeInverseTranspose = __glComputeInverseTranspose;
#if defined(__GL_USE_ASMCODE) && defined(mips)
    gc->procs.normalize = __glMipsNormalize;
#else
    gc->procs.normalize = __glNormalize;
#endif
    gc->procs.scaleVector = __glScaleVector;
    
    gc->procs.beginPrim[GL_LINE_LOOP] = __glBeginLLoop;
    gc->procs.beginPrim[GL_LINE_STRIP] = __glBeginLStrip;
    gc->procs.beginPrim[GL_LINES] = __glBeginLines;
    gc->procs.beginPrim[GL_POINTS] = __glBeginPoints;
    gc->procs.beginPrim[GL_POLYGON] = __glBeginPolygon;
    gc->procs.beginPrim[GL_TRIANGLE_STRIP] = __glBeginTStrip;
    gc->procs.beginPrim[GL_TRIANGLE_FAN] = __glBeginTFan;
    gc->procs.beginPrim[GL_TRIANGLES] = __glBeginTriangles;
    gc->procs.beginPrim[GL_QUAD_STRIP] = __glBeginQStrip;
    gc->procs.beginPrim[GL_QUADS] = __glBeginQuads;
    gc->procs.endPrim = __glEndPrim;

    gc->procs.vertex = (void (*)(__GLcontext*, __GLvertex*)) __glNop2;
    gc->procs.rasterPos2 = __glRasterPos2;
    gc->procs.rasterPos3 = __glRasterPos3;
    gc->procs.rasterPos4 = __glRasterPos4;

    /*
    ** Load in the device specific pick procs.  Do this before
    ** resetting the default state as that may need pick procs.
    */
    gc->procs.pickAllProcsPhase1 = __glGenericPickAllProcsPhase1;
    gc->procs.pickAllProcsPhase2 = __glGenericPickAllProcsPhase2;
    gc->procs.pickBlendProcs = __glGenericPickBlendProcs;
    gc->procs.pickBufferProcs = __glGenericPickBufferProcs;
    gc->procs.pickClipProcs = __glGenericPickClipProcs;
    gc->procs.pickColorMaterialProcs = __glGenericPickColorMaterialProcs;
    gc->procs.pickFogProcs = __glGenericPickFogProcs;
    gc->procs.pickTransformProcs = __glGenericPickTransformProcs;
    gc->procs.pickLineProcs = __glGenericPickLineProcs;
    gc->procs.pickMatrixProcs = __glGenericPickMatrixProcs;
    gc->procs.pickInvTransposeProcs = __glGenericPickInvTransposeProcs;
    gc->procs.pickMvpMatrixProcs = __glGenericPickMvpMatrixProcs;
    gc->procs.pickParameterClipProcs = __glGenericPickParameterClipProcs;
    gc->procs.pickPixelProcs = __glGenericPickPixelProcs;
    gc->procs.pickPointProcs = __glGenericPickPointProcs;
    gc->procs.pickRenderBitmapProcs = __glGenericPickRenderBitmapProcs;

    gc->procs.pickSpanProcs = __glGenericPickSpanProcs;
    gc->procs.pickStoreProcs = __glGenericPickStoreProcs;
    gc->procs.pickTextureProcs = __glGenericPickTextureProcs;
    gc->procs.pickCalcTextureProcs = __glGenericPickCalcTextureProcs;
    gc->procs.pickTriangleProcs = __glGenericPickTriangleProcs;
    gc->procs.pickVertexProcs = __glGenericPickVertexProcs;
    gc->procs.pickVertexArrayProcs = __glGenericPickVertexArrayProcs;
    gc->procs.pickVertexCacheProcs = __glGenericPickVertexCacheProcs;
    gc->procs.pickDepthProcs = __glGenericPickDepthProcs;

    gc->procs.copyImage = __glGenericPickCopyImage;
    gc->procs.readImage = __glGenericPickReadImage;
    gc->procs.fetchTex     = __glGenericPickGetTexture;
    gc->procs.storeTex     = __glGenericPickStoreTexture;

    gc->procs.pixel.spanReadCI = __glSpanReadCI;
    gc->procs.pixel.spanReadCI2 = __glSpanReadCI2;
    gc->procs.pixel.spanReadRGBA = __glSpanReadRGBA;
    gc->procs.pixel.spanReadRGBA2 = __glSpanReadRGBA2;
    gc->procs.pixel.spanReadDepth = __glSpanReadDepth;
    gc->procs.pixel.spanReadDepth2 = __glSpanReadDepth2;
    gc->procs.pixel.spanReadStencil = __glSpanReadStencil;
    gc->procs.pixel.spanReadStencil2 = __glSpanReadStencil2;
    gc->procs.pixel.spanRenderCI = __glSpanRenderCI;
    gc->procs.pixel.spanRenderCI2 = __glSpanRenderCI2;
    gc->procs.pixel.spanRenderRGBA = __glSpanRenderRGBA;
    gc->procs.pixel.spanRenderRGBA2 = __glSpanRenderRGBA2;
    gc->procs.pixel.spanRenderDepth = __glSpanRenderDepth;
    gc->procs.pixel.spanRenderDepth2 = __glSpanRenderDepth2;
    gc->procs.pixel.spanRenderStencil = __glSpanRenderStencil;
    gc->procs.pixel.spanRenderStencil2 = __glSpanRenderStencil2;

    gc->procs.applyScissor = ApplyScissor;
    gc->procs.applyViewport = ApplyViewport;
    gc->procs.computeClipBox = __glComputeClipBox;
    gc->procs.finish = Finish;
    gc->procs.flush = Flush;

    gc->procs.waitIdle = WaitIdle;
}

/*
** initialize context's side buffers
*/
static void InitBuffers(__GLcontext *gc)
{
    /*
    ** Dirty all bits so that all state will be recomputed when the next
    ** primitive is attempted. (Much of the state depends upon the
    ** color buffer scale factors, and may need to be recomputed when
    ** going from pixmap to hardware).
    */
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_ALL);

    /* Initialize front/back color buffer(s) */
    gc->front = &gc->frontBuffer;
    if (gc->modes.doubleBufferMode) {
	gc->back = &gc->backBuffer;
	if (gc->modes.colorIndexMode) {
	    __glDdxInitCI(gc->front, gc);
	    __glInitCI(gc->back, gc);
	} else {
	    __glDdxInitRGB(gc->front, gc);
	    __glInitRGB(gc->back, gc);
	}
    } else {
	if (gc->modes.colorIndexMode) {
	    __glDdxInitCI(gc->front, gc);
	} else {
	    __glDdxInitRGB(gc->front, gc);
	}
    }

#if __GL_MAX_AUXBUFFERS > 0
    /* Initialize any aux color buffers */
    if (gc->modes.maxAuxBuffers > 0) {
	GLint i;

	for (i = 0; i < gc->modes.maxAuxBuffers; ++i) {
	    if (gc->modes.colorIndexMode) {
		__glInitCI(&gc->auxBuffer[i], gc);
	    } else {
		__glInitRGB(&gc->auxBuffer[i], gc);
	    }
	}
    }
#endif

    /* Initialize any other ancillary buffers */
    if (gc->modes.haveAccumBuffer) {
	__glInitAccum64(&gc->accumBuffer, gc);
    }
    if (gc->modes.haveDepthBuffer) {
	__glInitDepth(&gc->depthBuffer, gc);
    } else {
	/*
	** Set the scale factor to allow window z values to be computed.
	** Set it not to use the high bit (to avoid floating point
	** exceptions) or low bits (to match floating point precision).
	*/
	gc->depthBuffer.scale = 0x7fffff80;
    }
    if (gc->modes.haveStencilBuffer) {
	__glInitStencil8(&gc->stencilBuffer, gc);
    }

    /* initialize buffer lock pointers, etc. */
    gc->buffers.lock.obtainSLock = ObtainSLock;
    gc->buffers.lock.releaseSLock = ReleaseSLock;
    gc->buffers.lock.lockDevice = LockDevice;
    gc->buffers.lock.unlockDevice = UnlockDevice;
}

/************************************************************************/

/*
** NotifySwapBuffers is used so that the context knows which buffer is
** front and which is back.  Actual buffer swapping is performed elsewhere.
*/
static void NotifySwapBuffers(__GLcontext *gc)
{
}

/************************************************************************/

static GLboolean DestroyContext(__GLcontext *gc)
{
    __GLDdxContext *hwcx = (__GLDdxContext *)gc;

    /* 
    ** Free ancillary buffer related data.  Note that these calls do
    ** *not* free software ancillary buffers, just any related data
    ** stored in them.
    */
    __glFreeBufData(gc);

    /* Destroy rest of software context */
    __glDestroyContext(gc);

    /* Free memory for the host cpu hw context */
    (*gc->imports.free)(gc, gc);

    return GL_TRUE;
}

static GLboolean LoseCurrent(__GLcontext *gc)
{
    __GLDdxContext *hwcx = (__GLDdxContext *)gc;

    /* 
    ** Illegal to makeCurrent when the current context is inside begin/end
    */
    if (__gl_beginMode == __GL_IN_BEGIN) {
	return GL_FALSE;
    }

    /* unset the gc */
    hwcx->pGC = NULL;

    __glLoseCurrentBuffers(gc);

    gc->beginMode = __gl_beginMode;
    __gl_context = NULL;

    return GL_TRUE;
}

/*
** Make this context the current context for this process.
*/
static GLboolean MakeCurrent(__GLcontext *gc)
{
    __GLDdxContext *hwcx = (__GLDdxContext *)gc;

    /* Load up global variables for the new context */
    __gl_context = gc;
    __gl_beginMode = gc->beginMode;

    /* Initialize dispatch tables */
    gc->dispatchState = &gc->currentDispatchState;
    gc->currentDispatchState = __glImmedState;
    gc->listCompState = __glListCompState;

    /*
    ** Dirty the depth mask, since we may need to update depth pointers.
    */
    __GL_DELAY_VALIDATE_MASK(gc, __GL_DIRTY_DEPTH);

    /* Entering HW mode rendering */

    /*
    ** This initializes context's side buffer structures and various
    ** other pointers.
    */
    if( (gc->gcState & __GL_HW_MODE) == 0 ) {
	InitBuffers(gc);
    }

    /*
    ** page in buffer info
    */
    __glMakeCurrentBuffers(gc);

    /* init pointers only when entering hw mode */
    if ((gc->gcState & __GL_HW_MODE) == 0 ) {
	InitFnPtrs(gc);
    }

    __glUpdateDepthRange(gc);

    /* Reset the context if this is the first MakeCurrent */
    if ((gc->gcState & (__GL_HW_MODE | __GL_PIXMAP_MODE)) == 0) {
	GLint x, y;
	GLuint width, height;

	/* Now reset the context to its default state */
	__glSoftResetContext(gc);

	/*
	** The first time a context is made current the spec requires that
	** the viewport and scissor be initialized.
	*/
	(*gc->drawablePrivate->getDrawableSize)(gc->drawablePrivate,
						&x, &y, &width, &height);
	(*gc->currentDispatchState.dispatch.Viewport)(0, 0, width, height);
	(*gc->currentDispatchState.dispatch.Scissor)(0, 0, width, height);
    } else {
	/*
	** We may have never bound to this window before, find out what
	** its size is.
	*/
	(*gc->procs.applyViewport)(gc);
	(*gc->procs.applyScissor)(gc);
    }

    /*
    ** grab the GC pointer from the drawable
    */
    hwcx->pGC = __glXFBGetGC(gc->frontBuffer.buf.drawableBuf);

    /* 
    ** Reset the HW context if this is the first device dependent 
    ** MakeCurrent 
    */
    if ((gc->gcState & __GL_INIT_HW) == 0) {
	__GLcontextModes *modes = gc->drawablePrivate->modes;
	/*
	** Scale all state that depends upon the color scales.
	*/
	__glContextSetColorScales(gc);

	/*
	** figure out rgb bit shifts
	*/
	if (modes->rgbMode) {
	    unsigned long rMask, gMask, bMask;
	    GLint i;

	    hwcx->redShifts = hwcx->greenShifts = hwcx->blueShifts = 0;
	    rMask = modes->redMask;
	    if (rMask) {
		for (i=0; !(rMask&1); i++) rMask >>= 1;
		hwcx->redShifts = i;
	    }
	    gMask = modes->greenMask;
	    if (gMask) {
		for (i=0; !(gMask&1); i++) gMask >>= 1;
		hwcx->greenShifts = i;
	    }
	    bMask = modes->blueMask;
	    if (bMask) {
		for (i=0; !(bMask&1); i++) bMask >>= 1;
		hwcx->blueShifts = i;
	    }
	}

	(*gc->procs.validate)(gc);
    }

    /*
    ** NOTE: now that context is initialized reset to use the global
    ** table
    */
    if (gc->dlist.currentList) {
	gc->dispatchState = &gc->savedDispatchState;
	__glCopyDispatch(&gc->listCompState, &__gl_dispatch);
    } else {
	gc->dispatchState = &__gl_dispatch;
    }

    gc->gcState |= __GL_INIT_HW | __GL_HW_MODE;
    gc->gcState &= ~__GL_PIXMAP_MODE;

    return GL_TRUE;
}

static GLboolean ShareContext(__GLcontext *dst, __GLcontext *src)
{
    /*
    ** Set up the sharable state: currently display lists and texture objects.
    */
    __glShareDlist(dst, src);
    __glShareTextureObjects(dst, src);
    __glShareTextureManager(dst, src);

    return GL_TRUE;
}

static GLboolean ForceCurrent(__GLcontext *gc)
{
    /*
    ** Set the current context for the DDX layer
    */
    __gl_context = gc;

    /*
    ** This implementation has no erorrs to report here.  However, there
    ** might be implementions that do.
    */
    return GL_TRUE;
}

/*
** Create a new context.  The new context is not made current.
** This can fail, but the caller is responsible for dealing with it.
*/
__GLinterface *__glDdxCreateContext(__GLimports *imports, 
				    __GLcontextModes *modes)
{
    __GLDdxContext *hwcx;
    __GLcontext *gc;

    /* Allocate memory for host cpu hardware context */
    hwcx = (__GLDdxContext *) (*imports->calloc)(0, 1, sizeof(__GLDdxContext));
    if (!hwcx) {
	return 0;
    }
    gc = &hwcx->gc;
    gc->imports = *imports;
    gc->modes = *modes;

    /*
    ** Load some device specific constants into the context
    */
    gc->constants.maxViewportWidth = __GL_DEFAULT_MAX_WINDOW_WIDTH;
    gc->constants.maxViewportHeight = __GL_DEFAULT_MAX_WINDOW_HEIGHT;
    gc->constants.viewportXAdjust =
	__GL_DEFAULT_VERTEX_X_BIAS + __GL_DEFAULT_VERTEX_X_FIX;
    gc->constants.viewportYAdjust =
	__GL_DEFAULT_VERTEX_Y_BIAS + __GL_DEFAULT_VERTEX_Y_FIX;
    gc->constants.subpixelBits = __GL_DEFAULT_COORD_SUBPIXEL_BITS;

#if defined(__GL_DEVICE_COLOR_SCALE)
    gc->constants.redScale = 255.0F;
    gc->constants.greenScale = 255.0F;
    gc->constants.blueScale = 255.0F;
    gc->constants.alphaScale = 255.0F;
#endif

#if defined(__GL_DEVICE_DEPTH_SCALE)
    gc->constants.depthScale = 1.0F;
#endif

    gc->constants.numberOfLights = __GL_DEFAULT_NUMBER_OF_LIGHTS;
    gc->constants.numberOfClipPlanes = __GL_DEFAULT_NUMBER_OF_CLIP_PLANES;
    gc->constants.maxTextureUnits = __GL_DEFAULT_MAX_TEXTURE_UNITS;
    gc->constants.maxTextureSize = __GL_DEFAULT_MAX_MIPMAP_LEVEL;/*XXX*/
    gc->constants.maxMipMapLevel = __GL_DEFAULT_MAX_MIPMAP_LEVEL;
    gc->constants.maxListNesting = __GL_DEFAULT_MAX_LIST_NESTING;
    gc->constants.maxEvalOrder = __GL_DEFAULT_MAX_EVAL_ORDER;
    gc->constants.maxPixelMapTable = __GL_DEFAULT_MAX_PIXEL_MAP_TABLE;
    gc->constants.maxAttribStackDepth = __GL_DEFAULT_MAX_ATTRIB_STACK_DEPTH;
    gc->constants.maxClientAttribStackDepth =
	__GL_DEFAULT_MAX_CLIENT_ATTRIB_STACK_DEPTH;
    gc->constants.maxNameStackDepth = __GL_DEFAULT_MAX_NAME_STACK_DEPTH;
    gc->constants.maxModelViewStackDepth =
	__GL_DEFAULT_MAX_MODELVIEW_STACK_DEPTH;
    gc->constants.maxProjectionStackDepth =
	__GL_DEFAULT_MAX_PROJECTION_STACK_DEPTH;
    gc->constants.maxTextureStackDepth = __GL_DEFAULT_MAX_TEXTURE_STACK_DEPTH;
    gc->constants.maxColorStackDepth = __GL_DEFAULT_MAX_COLOR_STACK_DEPTH;
    gc->constants.maxConvolution1DWidth = __GL_DEFAULT_MAX_CONV_1D_WIDTH;
    gc->constants.maxConvolution2DWidth = __GL_DEFAULT_MAX_CONV_2D_WIDTH;
    gc->constants.maxConvolution2DHeight = __GL_DEFAULT_MAX_CONV_2D_HEIGHT;
    gc->constants.maxSeparable2DWidth = __GL_DEFAULT_MAX_SEP_2D_WIDTH;
    gc->constants.maxSeparable2DHeight = __GL_DEFAULT_MAX_SEP_2D_HEIGHT;

    gc->constants.pointSizeMinimum = __GL_DEFAULT_POINT_SIZE_MINIMUM;
    gc->constants.pointSizeMaximum = __GL_DEFAULT_POINT_SIZE_MAXIMUM;
    gc->constants.pointSizeGranularity = __GL_DEFAULT_POINT_SIZE_GRANULARITY;
    gc->constants.aliasedPointSizeMinimum =
	__GL_DEFAULT_ALIASED_POINT_SIZE_MINIMUM;
    gc->constants.aliasedPointSizeMaximum =
	__GL_DEFAULT_ALIASED_POINT_SIZE_MAXIMUM;
    gc->constants.lineWidthMinimum = __GL_DEFAULT_LINE_WIDTH_MINIMUM;
    gc->constants.lineWidthMaximum = __GL_DEFAULT_LINE_WIDTH_MAXIMUM;
    gc->constants.lineWidthGranularity = __GL_DEFAULT_LINE_WIDTH_GRANULARITY;
    gc->constants.aliasedLineWidthMinimum =
	__GL_DEFAULT_ALIASED_LINE_WIDTH_MINIMUM;
    gc->constants.aliasedLineWidthMaximum =
	__GL_DEFAULT_ALIASED_LINE_WIDTH_MAXIMUM;

    gc->dlist.optimizer = __glGenericDlistOptimizer;
    gc->dlist.compiler = __glGenericDlistCompiler;
    gc->dlist.listExec = __gl_GenericDlOps;
    gc->dlist.baseListExec = __glListExecTable;
    gc->dlist.machineListExec = NULL;
    gc->dlist.checkOp = (void (*)(__GLcontext *gc, __GLdlistOp *)) __glNop2;
    gc->dlist.initState = (void (*)(__GLcontext *gc)) __glNop1;

    gc->texture.initTextureManager = __glInitTextureManager;
    gc->texture.lookupTextureFormat = __glLookupTextureFormat;

    gc->exports.destroyContext = DestroyContext;
    gc->exports.loseCurrent = LoseCurrent;
    gc->exports.makeCurrent = MakeCurrent;
    gc->exports.shareContext = ShareContext;
    gc->exports.forceCurrent = ForceCurrent;
    gc->exports.notifyResize = NotifyResize;
    gc->exports.notifyDestroy = NotifyDestroy;
    gc->exports.notifySwapBuffers = NotifySwapBuffers;

    __glEarlyInitContext(gc);

    return (__GLinterface *)gc;
}
