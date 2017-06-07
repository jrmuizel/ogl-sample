#ifndef __gl_context_h_
#define __gl_context_h_

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
*/
/*
** Graphics context structures.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/context.h#20 $
*/
#include "types.h"
#include "attrib.h"
#include "feedback.h"
#include "select.h"
#include "buffers.h"
#include "pixel.h"
#include "procs.h"
#include "dlist.h"
#include "xform.h"
#include "render.h"
#include "varray.h"
#include "vcache.h"
#include "ctable.h"
#include "GL/internal/glcore.h"
#include "g_disp.h"
#if (defined(__GL_FR_RAST) || defined(__GL_CODEGEN)) && defined(i386)
#include "og.h"
#include "ogcache.h"
#include "og_geom.h"
#endif /* __GL_CODEGEN && i386 */

/*
** Various constants.  Most of these will never change through the life 
** of the context.
*/
typedef struct __GLcontextConstantsRec {
    __GLfloat half;
    __GLfloat one;
    __GLfloat val255;
    __GLfloat oneOver255;
    __GLfloat val65535;
    __GLfloat oneOver65535;
    /*
    ** Not quite 2^31-1 because of possible floating point errors.  4294965000
    ** is a much safer number to use.
    */
    __GLfloat val4294965000;
    __GLfloat oneOver4294965000;

    /* Stuff for glGetString */
    char *vendor;
    char *renderer;
    char *version;
    char *extensions;

    /* Specific size limits */
    GLint numberOfLights;
    GLint numberOfClipPlanes;
    GLint maxViewportWidth;
    GLint maxViewportHeight;

#ifdef __GL_ALIGNED_BUFFERS
    /*
    ** Viewport offsets used for alignent.
    **
    ** There is hardware with certain limitations of the starting address
    ** of a frame buffer to be aligned to a certain boundary.  This becomes
    ** an issue because usually the front buffer is part of a larger buffer
    ** while the ancillary buffers are not.
    **
    ** The alignment offsets below are re-calculated on a window resize
    ** or move to be offsets from the alignment boundary to the window origin.
    ** They are to be added to the viewport so that rendering occurs at the
    ** correct position (for the front buffer).
    */
    GLint viewportXAlignOffset;
    GLint viewportYAlignOffset;
#endif /* __GL_ALIGNED_BUFFERS */

    /*
    ** Viewport offsets: These numbers are added to the viewport center
    ** values to adjust the computed window coordinates into a
    ** numerically well behaved space (fixed point represented in a
    ** floating point number).
    */
    GLint viewportXAdjust;
    GLint viewportYAdjust;
    __GLfloat fviewportXAdjust;
    __GLfloat fviewportYAdjust;

#ifdef __GL_ALIGNED_BUFFERS
    /*
    ** cached combination of viewport*Align and viewport*Adjust.
    */
    GLint viewportXAdjustOffset;
    GLint viewportYAdjustOffset;
    __GLfloat fviewportXAdjustOffset;
    __GLfloat fviewportYAdjustOffset;
#endif /* __GL_ALIGNED_BUFFERS */
    

    /*
    ** These values are computed from viewportXAdjust when the context
    ** is created.  It is assumed that x and y are forced into the same
    ** fixed point range by viewportXAdjust and viewportYAdjust.  
    **
    ** viewportEpsilon is computed as the smallest possible value that can
    ** be represented in that fixed point space.
    **
    ** viewportAlmostHalf is equal to 0.5 - viewportEpsilon.
    */
    __GLfloat viewportEpsilon;
    __GLfloat viewportAlmostHalf;

    /*
    ** These values are used to bring colors values from 0.0 to 1.0 into
    ** the internal range.
    */
    __GLfloat redScale, blueScale, greenScale, alphaScale;
    __GLfloat oneOverRedScale, oneOverBlueScale,
		oneOverGreenScale, oneOverAlphaScale;
    
    /*
    ** These values take colors from the internal range to the
    ** range of the color buffer.
    */
    __GLfloat redRescale, blueRescale, greenRescale, alphaRescale;
    
    /*
    ** These values are used to bring depth values from 0.0 to 1.0 into
    ** the internal range.
    */
    __GLfloat depthScale, oneOverDepthScale;

    /*
    ** This value take depth values from the internal range to the
    ** range of the depth buffer.
    */
    __GLfloat depthRescale;

    /*
    ** Geometry of the current window.
    */
    GLuint width, height;

    /*
    ** Size of the alpha lookup table for alpha testing, and conversion 
    ** value to convert from scaled alpha to alpha to be used for lookup table.
    */
    GLint alphaTestSize;
    __GLfloat alphaTableConv;

    /*
    ** Random getable constants
    */
    GLint maxTextureSize;
    GLint maxTexture3DSize;
    GLint maxMipMapLevel;
    GLint maxTextureUnits;
    GLint subpixelBits;
    GLint maxListNesting;
    __GLfloat pointSizeMinimum;
    __GLfloat pointSizeMaximum;
    __GLfloat pointSizeGranularity;
    __GLfloat lineWidthMinimum;
    __GLfloat lineWidthMaximum;
    __GLfloat lineWidthGranularity;
    GLint aliasedPointSizeMinimum;
    GLint aliasedPointSizeMaximum;
    GLint aliasedLineWidthMinimum;
    GLint aliasedLineWidthMaximum;
    GLint maxEvalOrder;
    GLint maxPixelMapTable;
    GLint maxAttribStackDepth;
    GLint maxClientAttribStackDepth;
    GLint maxNameStackDepth;

    GLint maxModelViewStackDepth;
    GLint maxProjectionStackDepth;
    GLint maxTextureStackDepth;
    GLint maxColorStackDepth;

    GLint maxConvolution1DWidth;
    GLint maxConvolution2DWidth;
    GLint maxConvolution2DHeight;
    GLint maxSeparable2DWidth;
    GLint maxSeparable2DHeight;

    /*
    ** This is used by the macro __GL_UB_TO_FLOAT which converts
    ** unsigned bytes to floats in the range [0,1].  Once is enough!
    */
    GLfloat uByteToFloat[256];

    /*
    ** On some systems y is inverted.  These two constants help out.
    */
    GLboolean yInverted;
    GLint ySign;

    /*
    ** Bitfield containing processor capabilities.
    */
    GLbitfield cpuCaps;

#if (defined(__GL_FR_RAST) || defined(__GL_CODEGEN)) && defined(i386)
    /*
    ** These control the size of the per-context cache for dynamically
    ** generated triangle rasterization procs.
    */
    GLuint textureReadCacheCount;
    GLuint textureReadCacheSize;
    GLuint depthTestCacheCount;
    GLuint depthTestCacheSize;
    GLuint triSetupCacheCount;
    GLuint triSetupCacheSize;
    GLuint triSetupDataSize;
    GLuint triRasterCacheCount;
    GLuint triRasterCacheSize;
    GLuint drawVertexesCacheCount;
    GLuint drawVertexesCacheSize;
#endif

} __GLcontextConstants;

/************************************************************************/

/*
** Bits for gcState
** __GL_INIT_HW is set if the hardware has been initialized.
** __GL_HW_MODE is set if the hardware was mode was used last.
** __GL_PIXMAP_MODE is set if pixmap rendering was done last.
*/
#define __GL_INIT_HW		0x00000001
#define __GL_HW_MODE		0x00000002
#define __GL_PIXMAP_MODE	0x00000004

/*****************************************************************************/

struct __GLcontextRec {

    /*
    ** Os related interfaces; these *must* be the first members of this
    ** structure, because they are exposed to the outside world (i.e. GLX
    ** extension).
    */
    __GLimports imports;
    __GLexports exports;

    /************************************************************************/

    /*
    ** State var indicating what the state of this GC is.
    */
    GLint gcState;

    /************************************************************************/

    /*
    ** Pushattrib, Popattrib Stackable state.  All of the current user 
    ** controllable state is resident here.
    */
    __GLattribute state;

    /************************************************************************/

    /*
    ** Unstackable State
    */

    /*
    ** Current glBegin mode.  Legal values are 0 (not in begin mode), 1
    ** (in beginMode), or 2 (not in begin mode, some validation is
    ** needed).  Because all state changing routines have to fetch this
    ** value, we have overloaded state validation into it.  There is
    ** special code in the __glim_Begin (for software renderers) which
    ** deals with validation.
    */
    __GLbeginMode beginMode;

    /* Current rendering mode */
    GLenum renderMode;

    /*
    ** Most recent error code, or GL_NO_ERROR if no error has occurred
    ** since the last glGetError.
    */
    GLint error;

    /*
    ** Mode information that describes the kind of buffers and rendering
    ** modes that this context manages.
    */
    __GLcontextModes modes;

    /* Implementation dependent constants */
    __GLcontextConstants constants;

    /* Feedback and select state */
    __GLfeedbackMachine feedback;
    __GLselectMachine select;

    /* Display list state */
    __GLdlistMachine dlist;

    /* Vertex Array state */
    __GLvertArrayMachine vertexArray;

    /* Client pixel state */
    __GLclientPixelState clientPixel;

    /* Client texture state */
    __GLclientTextureState clientTexture;

    /*
    ** Current dispatch tables state. currentDispatchState is used to
    ** save/restore the global dispatch pointers state by glXMakeCurrent.
    ** When making a new context current, first the old context's global
    ** pointers state will be saved in its currentDispatchState and then
    ** the new context's dispatch table pointers state will be copied
    ** from its currentDispatchState into the global pointers state.
    **
    ** When entering list-compilation mode (NewList) the value of the
    ** global state will be copied into savedDispatchState.  Then the
    ** global state will be loaded with the values in listCompState.  For
    ** compile and execute to work, the compilation routines will invoke
    ** procedures as follows:
    **
    ** 		(*gc->savedDispatchState.vertex->vertex3fv)(v).
    **
    ** Any "pick" procs that might be updating the dispatch tables on the
    ** fly as state changes occur need to know which set of pointers
    ** to update.  In this case the "pick" procs use the dispatchState
    ** pointer which will point to either the global pointers or to
    ** savedDispatchState.
    **
    ** Finally, executing a CallList or CallLists while in display list
    ** compile and execute mode requires special treatment.  In this case
    ** the CallList/CallLists call will be recorded in the display list
    ** being constructed.  Next, the code will temporarily switch the
    ** global pointers to contain the contents of savedDispatchState.
    ** After calling the immediate mode CallList/CallLists code (using
    ** gc->savedDispatchState to get its address), the global pointers
    ** will be reset to point back to the list compilation routines.
    **
    ** An external agent (e.g. the GLF metafile library) may override
    ** the per-thread dispatch table __gl_dispatch.  When the per-thread
    ** variable __gl_dispatchOverride is NULL, everything works as above.
    ** When __gl_dispatchOverride is non-NULL, it points to an override
    ** dispatch table, and __gl_dispatch == *__gl_dispatchOverride.
    **
    ** If __gl_dispatchOverride is non-NULL for some thread, and if a GL
    ** context is current to that thread, then the dispatch pointers
    ** that would have been maintained in __gl_dispatch (if
    ** __gl_dispatchOverride was NULL) are instead maintained in
    ** __gl_context->currentDispatchState.  In other words,
    ** a context's currentDispatchState is used to store the
    ** context's global dispatch pointer state both when the context is not
    ** current to any thread \and/ when the context is current to a thread
    ** that has a non-NULL __gl_dispatchOverride.
    ** 
    ** When __gl_dispatchOverride is NULL, __gl_context->dispatchState always
    ** points to either __gl_context->savedDispatchState (in list-compilation
    ** mode) or __gl_dispatch (in immediate mode).  When __gl_dispatchOverride
    ** is non-NULL, __gl_context->dispatchState always points to either
    ** __gl_context->savedDispatchState (in list-compilation mode) or
    ** __gl_context->currentDispatchState (in immediate mode).
    **    
    ** Since the "pick" procs update dispatch tables through the pointer
    ** __gl_context->dispatchState, a context's global dispatch state will
    ** always be updated properly, independent of whether it is stored in
    ** __gl_dispatch (the non-override case) or
    ** __gl_context->currentDispatchState (the override case).
    ** 
    */
    __GLdispatchState *dispatchState;
    __GLdispatchState listCompState;
    __GLdispatchState *listCompExecState;
    __GLdispatchState currentDispatchState;
    __GLdispatchState savedDispatchState;

    /************************************************************************/

    /*
    ** The remaining state is used primarily by the software renderer.
    */

    /*
    ** Mask word for validation state to help guide the gc validation
    ** code.  Only operations which are largely expensive are broken
    ** out here.  See the #define's below for the values being used.
    */
    GLuint validateMask;

    /*
    ** Mask word of dirty bits.  Most routines just set the GENERIC bit to 
    ** dirty, others may set more specific bits.  The list of bits is 
    ** listed below.
    */
    GLuint dirtyMask;

    /* Current draw buffer, set by glDrawBuffer */
    __GLcolorBuffer *drawBuffer;

    /* Current read buffer, set by glReadBuffer */
    __GLcolorBuffer *readBuffer;

    /* Function pointers that are mode dependent */
    __GLprocs procs;

    /* Attribute stack state */
    __GLattributeMachine attributes;

    /* Machine structures defining software rendering "machine" state */
    __GLvertexMachine vertex;
    __GLlightMachine light;
    __GLtextureMachine texture;
    __GLevaluatorMachine eval;
    __GLtransformMachine transform;
    __GLlineMachine line;
    __GLpolygonMachine polygon;
    __GLpixelMachine pixel;
    __GLbufferMachine buffers;

    /* Buffers */
    __GLcolorBuffer *front;
    __GLcolorBuffer *back;
    __GLcolorBuffer frontBuffer;
    __GLcolorBuffer backBuffer;
    __GLcolorBuffer *auxBuffer;
    __GLstencilBuffer stencilBuffer;
    __GLdepthBuffer depthBuffer;
    __GLaccumBuffer accumBuffer;

    /* Pointer to drawable private data */
    __GLdrawablePrivate *drawablePrivate;

    /* Vertex cache machinery */
    __GLvcacheMachine vertexCache;

#if (defined(__GL_FR_RAST) || defined(__GL_CODEGEN)) && defined(i386)
    __GLOGState ogState;
    __GLcodeCache textureReadCache;
    __GLcodeCache depthTestCache;
    __GLcodeCache triSetupCache;
    __GLcodeCache triRasterCache;
    __GLcodeCache drawVertexesCache;
    GLvoid *triSetupData;
    __GLnameSpace *tr;
    __GLnameSpace vx_ns;
    __GLgeomOGData geomOGdata;
#endif /* (__GL_FR_RAST || __GL_CODEGEN) && i386 */

    /* Performance histogram used to count clock cycles */
    void *perf_histogram;

};

/*
** Bit values for the validateMask word
*/
#define __GL_VALIDATE_ALPHA_FUNC	0x00000001
#define __GL_VALIDATE_STENCIL_FUNC	0x00000002
#define __GL_VALIDATE_STENCIL_OP	0x00000004
#define __GL_VALIDATE_INDEX_FUNC	0x00000008

/*
** Bit values for dirtyMask word.
**
** These are all for delayed validation.  There are a few things that do 
** not trigger delayed validation.  They are:
**
** Matrix operations -- matrices (but not inverse) are validated immediately.
** Material changes -- they also validate immediately.
** Color Material change -- validated immediately.
** Color Material enable -- validated immediately.
** Pixel Map changes -- no validation.
*/

/* 
** All things not listed elsewhere. 
*/
#define __GL_DIRTY_GENERIC		0x00000001

/* 
** Line stipple, line stipple enable, line width, line smooth enable, 
** line smooth hint. 
*/
#define __GL_DIRTY_LINE			0x00000002

/* 
** Polygon stipple, polygon stipple enable, polygon smooth enable, face 
** culling, front face orientation, polygon mode, point smooth hint.
** EXT Extension: polygon offset
*/
#define __GL_DIRTY_POLYGON		0x00000004

/* 
** Point smooth, point smooth hint, point width.
*/
#define __GL_DIRTY_POINT		0x00000008

/*
** Pixel store, pixel zoom, pixel transfer, (pixel maps don't cause 
** validation), read buffer.
*/
#define __GL_DIRTY_PIXEL		0x00000010

/*
** Light, Light Model, lighting enable, lightx enable, (color material 
** validates immediately), (NOT shade model -- it is generic), (color material
** enable validates immediately)
*/
#define __GL_DIRTY_LIGHTING		0x00000020

/* 
** Polygon stipple
*/
#define __GL_DIRTY_POLYGON_STIPPLE	0x00000040

/*
** The depth mode has changed.
*/
#define __GL_DIRTY_DEPTH		0x00000080

/*
** The scissor mode has changed.
*/
#define __GL_DIRTY_SCISSOR		0x00000100

/*
** The vertex array enables and/or pointers have changed.
*/
#define __GL_DIRTY_VERTARRAY		0x00000200

/*
** The model-view matrix has changed.
*/
#define __GL_DIRTY_MODELVIEW		0x00000400

#define __GL_DIRTY_ALL			0x000007ff

/*
** Bit values for changes to material colors
*/
#define __GL_MATERIAL_AMBIENT		0x00000001
#define __GL_MATERIAL_DIFFUSE		0x00000002
#define __GL_MATERIAL_SPECULAR		0x00000004
#define __GL_MATERIAL_EMISSIVE		0x00000008
#define __GL_MATERIAL_SHININESS		0x00000010
#define __GL_MATERIAL_COLORINDEXES	0x00000020
#define __GL_MATERIAL_ALL		0x0000003f

#define __GL_DELAY_VALIDATE(gc)		      \
    assert((gc)->beginMode != __GL_IN_BEGIN); \
    (gc)->beginMode = __GL_NEED_VALIDATE;     \
    (gc)->dirtyMask |= __GL_DIRTY_GENERIC

#define __GL_DELAY_VALIDATE_MASK(gc, mask)	\
    assert((gc)->beginMode != __GL_IN_BEGIN); 	\
    (gc)->beginMode = __GL_NEED_VALIDATE;     	\
    (gc)->dirtyMask |= (mask)

#define __GL_VALIDATE(gc)			\
    assert((gc)->beginMode == __GL_IN_BEGIN);	\
    (gc)->beginMode = __GL_NEED_VALIDATE;	\
    (gc)->dirtyMask |= __GL_DIRTY_GENERIC;	\
    (*(gc)->procs.validate)(gc);		\
    (gc)->beginMode = __GL_IN_BEGIN

	

/************************************************************************/

/* Applies to current context */
extern void __glSetError(GLenum code);

extern void __glCopyDispatch(const __GLdispatchState *src,
			     __GLdispatchState *dst);
extern __GLdispatchState *__glGetCurrentDispatch(void);

extern void __glFreeEvaluatorState(__GLcontext *gc);
extern void __glFreeDlistState(__GLcontext *gc);
extern void __glFreeMachineState(__GLcontext *gc);
extern void __glFreePixelState(__GLcontext *gc);
extern void __glFreeTextureState(__GLcontext *gc);
extern void __glFreeTransformState(__GLcontext *gc);
extern void __glFreeVertexArrayState(__GLcontext *gc);

extern void __glInitDlistState(__GLcontext *gc);
extern void __glInitEvaluatorState(__GLcontext *gc);
extern void __glEarlyInitPixelState(__GLcontext *gc);
extern void __glInitPixelState(__GLcontext *gc);
extern void __glInitTextureState(__GLcontext *gc);
extern void __glEarlyInitTransformState(__GLcontext *gc);
extern void __glInitTransformState(__GLcontext *gc);

void __glEarlyInitContext(__GLcontext *gc);
void __glContextSetColorScales(__GLcontext *gc);
void __glSoftResetContext(__GLcontext *gc);
GLboolean __glDestroyContext(__GLcontext *gc);

void __glFreePrivate(__GLdrawablePrivate *dp);

GLboolean __glResizeWindow(__GLcontext *gc);
void __glMakeCurrentBuffers(__GLcontext *gc);
void __glLoseCurrentBuffers(__GLcontext *gc);
void __glFreeBufData(__GLcontext *gc);

extern void __glCalc2DOutcode(__GLcontext *gc, __GLclipVertex *cv,
			      __GLregionRect *rect);

#endif /* __gl_context_h_ */
