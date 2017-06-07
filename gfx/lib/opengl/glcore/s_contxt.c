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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_contxt.c#20 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#include "pixel.h"
#include "image.h"
#include "g_imfncs.h"
#include "g_imports.h"
#if defined(__GL_FR_RAST)
#include "fr_raster.h"
#endif /* __GL_FR_RAST */
#if defined(__GL_USE_ASMCODE) && defined(i386)
#include "i386.h"
#endif /* __GL_USE_ASMCODE && i386) */
#if defined(__GL_CODEGEN) && defined(i386)
#include "ogcache.h"
#endif /* __GL_CODEGEN & i386 */


static GLfloat DefaultAmbient[4] = { 0.2F, 0.2F, 0.2F, 1.0F };
static GLfloat DefaultDiffuse[4] = { 0.8F, 0.8F, 0.8F, 1.0F };
static GLfloat DefaultBlack[4] = { 0.0F, 0.0F, 0.0F, 1.0F };
static GLfloat DefaultWhite[4] = { 1.0F, 1.0F, 1.0F, 1.0F };

/************************************************************************/

/* Dispatch table override control for external agents like libGLS */

static __GLdispatchState* DispatchExec(__GLcontext *gc) {
    return &gc->currentDispatchState;
}

static void BeginDispatchOverride(__GLcontext *gc) 
{
#if !defined(__GL_SUPPORT_EXTERNAL_AGENTS)
    gc->currentDispatchState = __gl_dispatch;
    if (!gc->dlist.currentList) {
        gc->dispatchState = &gc->currentDispatchState;
    }
#endif /* __GL_SUPPORT_EXTERNAL_AGENTS */
}

static void EndDispatchOverride(__GLcontext *gc) 
{
#if !defined(__GL_SUPPORT_EXTERNAL_AGENTS)
    if (!gc->dlist.currentList) {
        gc->dispatchState = &__gl_dispatch;
    }
#endif /* __GL_SUPPORT_EXTERNAL_AGENTS */
}

/************************************************************************/

/*
** Early initialization of context.  Very little is done here, just enough
** to make a context viable.
*/
void __glEarlyInitContext(__GLcontext *gc)
{
    GLint numLights;

    gc->constants.fviewportXAdjust = (__GLfloat) gc->constants.viewportXAdjust;
    gc->constants.fviewportYAdjust = (__GLfloat) gc->constants.viewportYAdjust;
#if defined(__GL_ALIGNED_BUFFERS)
    gc->constants.viewportXAdjustOffset = gc->constants.viewportXAdjust;
    gc->constants.viewportYAdjustOffset = gc->constants.viewportYAdjust;
    gc->constants.fviewportXAdjustOffset = 
	(__GLfloat) gc->constants.viewportXAdjustOffset;
    gc->constants.fviewportYAdjustOffset = 
	(__GLfloat) gc->constants.viewportYAdjustOffset;
#endif /* __GL_ALIGNED_BUFFERS */
    gc->constants.one = (__GLfloat) 1.0;
    gc->constants.half = (__GLfloat) 0.5;

    gc->exports.copyContext = __glCopyContext;
    gc->exports.dispatchExec = DispatchExec;
    gc->exports.beginDispatchOverride = BeginDispatchOverride;
    gc->exports.endDispatchOverride = EndDispatchOverride;
    gc->procs.pickColorMaterialProcs =  (void (*)(__GLcontext *))__glNop1;
    gc->procs.applyColor = (void (*)(__GLcontext *)) __glNop1;

    /* Allocate memory to hold variable sized things */
    numLights = gc->constants.numberOfLights;
    gc->state.light.source = (__GLlightSourceState*)
	(*gc->imports.calloc)(gc, (size_t) numLights,
			      sizeof(__GLlightSourceState));
    gc->light.lutCache = NULL;
    gc->light.source = (__GLlightSourceMachine*)
	(*gc->imports.calloc)(gc, (size_t) numLights,
			      sizeof(__GLlightSourceMachine));
    gc->attributes.stack = (__GLattribute**)
	(*gc->imports.calloc)(gc, (size_t) gc->constants.maxAttribStackDepth,
			      sizeof(__GLattribute*));
    gc->attributes.clientStack = (__GLclientAttribute**)
	(*gc->imports.calloc)(gc,
			      (size_t) gc->constants.maxClientAttribStackDepth,
			      sizeof(__GLclientAttribute*));
    gc->select.stack = (GLuint*)
	(*gc->imports.calloc)(gc, (size_t) gc->constants.maxNameStackDepth,
			      sizeof(GLuint));

#if defined(__GL_ALLOCATE_SCANLINE_ARRAYS)
    /*
    ** Allocate scanline color buffer arrays.  
    */
    gc->polygon.shader.colors = (__GLcolor *)
	(*gc->imports.calloc)(gc, (size_t) __GL_MAX_MAX_VIEWPORT,
			      sizeof(__GLcolor));
    gc->polygon.shader.fbcolors = (__GLcolor *)
	(*gc->imports.calloc)(gc, (size_t) __GL_MAX_MAX_VIEWPORT,
			      sizeof(__GLcolor));
    gc->polygon.shader.stipplePat = (__GLstippleWord *)
	(*gc->imports.calloc)(gc, (size_t) __GL_MAX_STIPPLE_WORDS,
			      sizeof(__GLstippleWord));
#endif /* __GL_ALLOCATE_SCANLINE_ARRAYS */

    __glEarlyInitPixelState(gc);
    __glEarlyInitTextureState(gc);
    __glEarlyInitTransformState(gc);

#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    /*
    ** Allocate aux color buffer records
    ** Note: Does not allocate the actual buffer memory, this is done 
    ** 	elsewhere.
    */
    gc->auxBuffer = (__GLcolorBuffer *)
	(*gc->imports.calloc)(gc, gc->modes.numAuxBuffers,
			      sizeof(__GLcolorBuffer));
#endif

    gc->procs.memory.newArena = __glNewArena;
    gc->procs.memory.deleteArena = __glDeleteArena;
    gc->procs.memory.alloc = __glArenaAlloc;
    gc->procs.memory.freeAll = __glArenaFreeAll;

#if defined(__GL_CODEGEN) && defined(i386)
    gc->constants.textureReadCacheCount = 4;
    gc->constants.textureReadCacheSize = 1024;
    gc->constants.depthTestCacheCount = 4;
    gc->constants.depthTestCacheSize = 256;
    gc->constants.triSetupCacheCount = 40;
    gc->constants.triSetupCacheSize = 4096;
    gc->constants.triSetupDataSize = 2048;
    gc->constants.triRasterCacheCount = 40;
    gc->constants.triRasterCacheSize = (4 * 4096);
    gc->constants.drawVertexesCacheCount = 20;
    gc->constants.drawVertexesCacheSize = 1024;

    __glInitializeCodeCache(gc);

    gc->geomOGdata.last_hit = &gc->geomOGdata.slots[0];
    memset(gc->geomOGdata.slots, 0, sizeof(gc->geomOGdata.slots));

    gc->geomOGdata.slots[0].space =
	(*gc->imports.calloc)(gc, GEOM_OG_NUM_SLOTS, GEOM_OG_BUFFER_SIZE);

    {
	int ii;
	GLubyte *ptr = gc->geomOGdata.slots[0].space;
	for (ii = 1; ii < GEOM_OG_NUM_SLOTS; ii++) {
	    gc->geomOGdata.slots[ii].space = ptr + (ii * GEOM_OG_BUFFER_SIZE);
	}
    }

    gc->ogState.fpconstant_size = 1000;
    gc->ogState.fpconstant_next = 0;
    gc->ogState.fpconstant_pool =
	(*gc->imports.calloc)(gc, gc->ogState.fpconstant_size, sizeof(float));

#if defined(__GL_USE_MMXCODE)
    gc->ogState.mxconstant_size = 1000;
    gc->ogState.mxconstant_next = 0;
    gc->ogState.mxconstant_pool =
	(*gc->imports.calloc)(gc, gc->ogState.mxconstant_size, sizeof(double));
#endif /* __GL_USE_MMXCODE */
#endif /* __GL_CODEGEN && i386 */

    __glInitDlistState(gc);
}

void __glContextSetColorScales(__GLcontext *gc)
{
    GLint i;
    
    if (gc->modes.rgbMode) {
	__glScaleColorf(gc, 
	        &gc->state.current.rasterPos.colors[__GL_FRONTFACE], 
		&gc->state.current.rasterPos.colors[__GL_FRONTFACE].r);
	__glScaleColorf(gc,
		&gc->state.current.color,
		&gc->state.current.color.r);
    } 

    __glScaleColorf(gc, 
	    &gc->state.light.model.ambient,
	    &gc->state.light.model.ambient.r);
    for (i=0; i<gc->constants.numberOfLights; i++) {
	__glScaleColorf(gc,
		&gc->state.light.source[i].ambient,
		&gc->state.light.source[i].ambient.r);
	__glScaleColorf(gc,
		&gc->state.light.source[i].diffuse,
		&gc->state.light.source[i].diffuse.r);
	__glScaleColorf(gc,
		&gc->state.light.source[i].specular,
		&gc->state.light.source[i].specular.r);
    }
    __glScaleColorf(gc,
   	    &gc->state.light.front.emissive, 
   	    &gc->state.light.front.emissive.r);
    __glScaleColorf(gc,
   	    &gc->state.light.back.emissive, 
   	    &gc->state.light.back.emissive.r);
}

/*
** Initialize all user controllable state, plus any computed state that
** is only set by user commands.  For example, light source position
** is converted immediately into eye coordinates.
**
** Any state that would be initialized to zero is not done here because
** the memory assigned to the context has already been block zeroed.
*/
void __glSoftResetContext(__GLcontext *gc)
{
    __GLlightSourceState *lss;
    __GLlightSourceMachine *lsm;
    __GLvertex *vx;
    GLint i, numLights;
    __GLfloat zero = __glZero;
    __GLfloat one = __glOne;

    /*
    ** Initialize constant values first so that they will
    ** be valid if needed by subsequent initialization steps.
    */

#if defined(__GL_USE_ASMCODE) && defined(i386)
    __glI386GetCPUCaps(gc);
#else /* __GL_USE_ASMCODE && i386 */
    __glGetCPUCaps(gc);
#endif /* __GL_USE_ASMCODE && i386 */

    /* Setup default values for get strings */
    gc->constants.vendor = "Generic";

    if ((gc->constants.cpuCaps & __GL_CPUFLAGS_K3D) != 0) {
	gc->constants.renderer = "Generic/MMX/K3D";
    } else if ((gc->constants.cpuCaps & __GL_CPUFLAGS_MMX) != 0) {
	gc->constants.renderer = "Generic/MMX";
    } else {
	gc->constants.renderer = "Generic";
    }

    gc->constants.version = "1.2.1 Generic";

    /*
    ** Put new extension names in alphabetical order; make sure to include
    ** a space after the name.
    */
    gc->constants.extensions =
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
	"GL_ARB_imaging "
#endif /* __GL_SUPPORT_ARB_IMAGING_SUBSET */
	"GL_ARB_multitexture "
	"GL_EXT_abgr "
	"GL_EXT_bgra "
	"GL_EXT_compiled_vertex_array "
	"GL_EXT_packed_pixels "
	"GL_EXT_paletted_texture "
	"GL_EXT_vertex_array "
	"GL_WIN_swap_hint "
	;

    /*
    ** Not quite 2^31-1 because of possible floating point errors.  4294965000
    ** is a much safer number to use.
    */
    gc->constants.val255 = (__GLfloat) 255.0;
    gc->constants.val65535 = (__GLfloat) 65535.0;
    gc->constants.val4294965000 = (__GLfloat) 4294965000.0;
    gc->constants.oneOver255 = one / gc->constants.val255;
    gc->constants.oneOver65535 = one / gc->constants.val65535;
    gc->constants.oneOver4294965000 = one / gc->constants.val4294965000;

    if (gc->constants.alphaTestSize == 0) {
	gc->constants.alphaTestSize = 256;	/* A default */
    }
    gc->constants.alphaTableConv = (gc->constants.alphaTestSize - 1) / 
	    gc->frontBuffer.alphaScale;

#if !defined(__GL_DEVICE_COLOR_SCALE)
    /*
    ** Use the same scale values as the color buffer
    */
    gc->constants.redScale = gc->frontBuffer.redScale;
    gc->constants.greenScale = gc->frontBuffer.greenScale;
    gc->constants.blueScale = gc->frontBuffer.blueScale;
    gc->constants.alphaScale = gc->frontBuffer.alphaScale;
#endif /* __GL_DEVICE_COLOR_SCALE */

#if !defined(__GL_DEVICE_DEPTH_SCALE)
    /*
    ** Use the same scale values as the depth buffer
    */
    gc->constants.depthScale = gc->depthBuffer.scale;
#endif /* __GL_DEVICE_DEPTH_SCALE */

    gc->frontBuffer.oneOverRedScale = one / gc->frontBuffer.redScale;
    gc->frontBuffer.oneOverGreenScale = one / gc->frontBuffer.greenScale;
    gc->frontBuffer.oneOverBlueScale = one / gc->frontBuffer.blueScale;
    gc->frontBuffer.oneOverAlphaScale = one / gc->frontBuffer.alphaScale;

    gc->constants.oneOverRedScale = one / gc->constants.redScale;
    gc->constants.oneOverGreenScale = one / gc->constants.greenScale;
    gc->constants.oneOverBlueScale = one / gc->constants.blueScale;
    gc->constants.oneOverAlphaScale = one / gc->constants.alphaScale;

    gc->constants.oneOverDepthScale = one / gc->constants.depthScale;

    gc->constants.redRescale =
	gc->frontBuffer.redScale * gc->constants.oneOverRedScale;
    gc->constants.greenRescale =
	gc->frontBuffer.greenScale * gc->constants.oneOverGreenScale;
    gc->constants.blueRescale =
	gc->frontBuffer.blueScale * gc->constants.oneOverBlueScale;
    gc->constants.alphaRescale =
	gc->frontBuffer.alphaScale * gc->constants.oneOverAlphaScale;

    gc->constants.depthRescale =
	gc->depthBuffer.scale * gc->constants.oneOverDepthScale;

    /* Lookup table used in macro __GL_UB_TO_FLOAT */
    for (i = 0; i < 256; i++) {
	gc->constants.uByteToFloat[i] = i * gc->constants.oneOver255;
    }

    /* XXX - Ick! */
    {
	/* Compute some fixed point viewport constants */
	__GLfloat *mem;
	volatile __GLfloat *temp1, *temp2;
	volatile __GLfloat *epsilon, *besteps;

	mem = (__GLfloat *) (*gc->imports.malloc)(gc, sizeof(__GLfloat) * 4);
	temp1 = (volatile __GLfloat *) &(mem[0]);
	temp2 = (volatile __GLfloat *) &(mem[1]);
	epsilon = (volatile __GLfloat *) &(mem[2]);
	besteps = (volatile __GLfloat *) &(mem[3]);

	*temp1 = gc->constants.fviewportXAdjust;
	*epsilon = one;
	*besteps = one;
	for (;;) {
	    *temp2 = *temp1 + *epsilon;
	    if (*temp2 != *temp1) {
		*besteps = *epsilon;
	    } else
		break;
	    *epsilon = *epsilon / 2.0f;
	}

	gc->constants.viewportEpsilon = *besteps;
	gc->constants.viewportAlmostHalf = __glHalf - *besteps;

	(*gc->imports.free)(gc, (void *) mem);
    }

    /* Allocate memory to hold variable sized things */
    numLights = gc->constants.numberOfLights;

    /* Misc machine state */
    __gl_beginMode = __GL_NEED_VALIDATE;
    gc->dirtyMask = __GL_DIRTY_ALL;
    gc->validateMask = ~0;
    gc->attributes.stackPointer = &gc->attributes.stack[0];
    gc->attributes.clientStackPointer = &gc->attributes.clientStack[0];
    gc->vertex.v0 = &gc->vertex.vbuf[0];

    vx = &gc->vertex.vbuf[0];
    for (i = 0; i < __GL_NVBUF; i++, vx++) {
	vx->color = &vx->colors[__GL_FRONTFACE];
    }

    /* GL_LIGHTING_BIT state */
    gc->state.light.model.ambient.r = DefaultAmbient[0];
    gc->state.light.model.ambient.g = DefaultAmbient[1];
    gc->state.light.model.ambient.b = DefaultAmbient[2];
    gc->state.light.model.ambient.a = DefaultAmbient[3];
    gc->state.light.model.twoSided = GL_FALSE;
    gc->state.light.model.localViewer = GL_FALSE;
    gc->state.light.model.colorControl = GL_SINGLE_COLOR;
    gc->state.light.front.ambient.r = DefaultAmbient[0];
    gc->state.light.front.ambient.g = DefaultAmbient[1];
    gc->state.light.front.ambient.b = DefaultAmbient[2];
    gc->state.light.front.ambient.a = DefaultAmbient[3];
    gc->state.light.front.diffuse.r = DefaultDiffuse[0];
    gc->state.light.front.diffuse.g = DefaultDiffuse[1];
    gc->state.light.front.diffuse.b = DefaultDiffuse[2];
    gc->state.light.front.diffuse.a = DefaultDiffuse[3];
    gc->state.light.front.specular.r = DefaultBlack[0];
    gc->state.light.front.specular.g = DefaultBlack[1];
    gc->state.light.front.specular.b = DefaultBlack[2];
    gc->state.light.front.specular.a = DefaultBlack[3];
    gc->state.light.front.emissive.r = DefaultBlack[0];
    gc->state.light.front.emissive.g = DefaultBlack[1];
    gc->state.light.front.emissive.b = DefaultBlack[2];
    gc->state.light.front.emissive.a = DefaultBlack[3];
    gc->state.light.front.cmapa = 0;
    gc->state.light.front.cmaps = 1;
    gc->state.light.front.cmapd = 1;
    gc->state.light.back = gc->state.light.front;

    gc->light.front.specularExponent = -1;
    gc->light.front.specTable = NULL;
    gc->light.front.cache = NULL;
    gc->light.back.specularExponent = -1;
    gc->light.back.specTable = NULL;
    gc->light.back.cache = NULL;

    /* Initialize the individual lights */
    lss = &gc->state.light.source[0];
    lsm = &gc->light.source[0];
    for (i = 0; i < numLights; i++, lss++, lsm++) {
	lss->ambient.r = DefaultBlack[0];
	lss->ambient.g = DefaultBlack[1];
	lss->ambient.b = DefaultBlack[2];
	lss->ambient.a = DefaultBlack[3];
	if (i == 0) {
	    lss->diffuse.r = DefaultWhite[0];
	    lss->diffuse.g = DefaultWhite[1];
	    lss->diffuse.b = DefaultWhite[2];
	    lss->diffuse.a = DefaultWhite[3];
	} else {
	    lss->diffuse.r = DefaultBlack[0];
	    lss->diffuse.g = DefaultBlack[1];
	    lss->diffuse.b = DefaultBlack[2];
	    lss->diffuse.a = DefaultBlack[3];
	}
	lss->specular = lss->diffuse;
	lss->position.z = one;
	lss->positionEye.z = one;
	lsm->position.z = one;
	lss->direction.z = __glMinusOne;
	lsm->direction.z = __glMinusOne;
	lss->spotLightCutOffAngle = 180;
	lss->constantAttenuation = one;
	lsm->spotTable = NULL;
	lsm->spotLightExponent = -1;
	lsm->cache = NULL;
    }
    gc->state.light.colorMaterialFace = GL_FRONT_AND_BACK;
    gc->state.light.colorMaterialParam = GL_AMBIENT_AND_DIFFUSE;
    gc->state.light.shadingModel = GL_SMOOTH;

    /* GL_HINT_BIT state */
    gc->state.hints.perspectiveCorrection = GL_DONT_CARE;
    gc->state.hints.pointSmooth = GL_DONT_CARE;
    gc->state.hints.lineSmooth = GL_DONT_CARE;
    gc->state.hints.polygonSmooth = GL_DONT_CARE;
    gc->state.hints.fog = GL_DONT_CARE;

    /* GL_CURRENT_BIT state */
#if defined(__GL_ALIGNED_BUFFERS)
    gc->state.current.rasterPos.window.x = 
	gc->constants.fviewportXAdjustOffset;
#else /* __GL_ALIGNED_BUFFERS */
    gc->state.current.rasterPos.window.x = gc->constants.fviewportXAdjust;
#endif /* __GL_ALIGNED_BUFFERS */
#if defined(__GL_ALIGNED_BUFFERS)
    if (gc->constants.yInverted) {
	gc->state.current.rasterPos.window.y =
	    (gc->constants.height - gc->constants.viewportEpsilon) +
			gc->constants.fviewportYAdjustOffset;
    } else {
	gc->state.current.rasterPos.window.y = 
	    gc->constants.fviewportYAdjustOffset;
    }
#else /* __GL_ALIGNED_BUFFERS */
    if (gc->constants.yInverted) {
	gc->state.current.rasterPos.window.y =
	    (gc->constants.height - gc->constants.viewportEpsilon) +
			gc->constants.fviewportYAdjust;
    } else {
	gc->state.current.rasterPos.window.y = gc->constants.fviewportYAdjust;
    }
#endif /* __GL_ALIGNED_BUFFERS */
#if defined(__GL_HALF_PIXEL_OFFSET)
    gc->state.current.rasterPos.window.x += __glHalf;
    gc->state.current.rasterPos.window.y += __glHalf;
#endif /* __GL_HALF_PIXEL_OFFSET */

    gc->state.current.rasterPos.clip.w = one;
    gc->state.current.rasterPos.texture[0].w = one;
    gc->state.current.rasterPos.color
	= &gc->state.current.rasterPos.colors[__GL_FRONTFACE];
    if (gc->modes.rgbMode) {
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].r = DefaultWhite[0];
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].g = DefaultWhite[1];
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].b = DefaultWhite[2];
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].a = DefaultWhite[3];
    } else {
	gc->state.current.rasterPos.colors[__GL_FRONTFACE].r = one;
    }
    gc->state.current.validRasterPos = GL_TRUE;
    gc->state.current.edgeTag = __GL_VERTEX_EDGE_FLAG;

    /* GL_FOG_BIT state */
    gc->state.fog.mode = GL_EXP;
    gc->state.fog.density = one;
    gc->state.fog.end = one;

    /* GL_POINT_BIT state */
    gc->state.point.requestedSize = one;
    gc->state.point.smoothSize = one;
    gc->state.point.aliasedSize = 1;

    /* GL_LINE_BIT state */
    gc->state.line.requestedWidth = one;
    gc->state.line.smoothWidth = one;
    gc->state.line.aliasedWidth = 1;
    gc->state.line.stipple = 0xFFFF;
    gc->state.line.stippleRepeat = 1;

    /* GL_POLYGON_BIT state */
    gc->state.polygon.frontMode = GL_FILL;
    gc->state.polygon.backMode = GL_FILL;
    gc->state.polygon.cull = GL_BACK;
    gc->state.polygon.frontFaceDirection = GL_CCW;

    /* GL_POLYGON_STIPPLE_BIT state */
    for (i = 0; i < 4*32; i++) {
	gc->state.polygonStipple.stipple[i] = 0xFF;
    }
    for (i = 0; i < 32; i++) {
	gc->polygon.stipple[i] = 0xFFFFFFFF;
    }

    /* GL_ACCUM_BUFFER_BIT state */

    /* GL_STENCIL_BUFFER_BIT state */
    gc->state.stencil.testFunc = GL_ALWAYS;
    gc->state.stencil.mask = __GL_MAX_STENCIL_VALUE;
    gc->state.stencil.fail = GL_KEEP;
    gc->state.stencil.depthFail = GL_KEEP;
    gc->state.stencil.depthPass = GL_KEEP;
    gc->state.stencil.writeMask = __GL_MAX_STENCIL_VALUE;

    /* GL_DEPTH_BUFFER_BIT state */
    gc->state.depth.writeEnable = GL_TRUE;
    gc->state.depth.testFunc = GL_LESS;
    gc->state.depth.clear = one;

    /* GL_COLOR_BUFFER_BIT state */
    gc->renderMode = GL_RENDER;
    gc->state.raster.alphaFunction = GL_ALWAYS;
    gc->state.raster.blendSrc = GL_ONE;
    gc->state.raster.blendDst = GL_ZERO;
    gc->state.raster.logicOp = GL_COPY;
    gc->state.raster.blendMode = GL_FUNC_ADD;
    gc->state.raster.blendColor.r = zero;
    gc->state.raster.blendColor.g = zero;
    gc->state.raster.blendColor.b = zero;
    gc->state.raster.blendColor.a = zero;
    gc->state.raster.rMask = GL_TRUE;
    gc->state.raster.gMask = GL_TRUE;
    gc->state.raster.bMask = GL_TRUE;
    gc->state.raster.aMask = GL_TRUE;
    if (gc->modes.doubleBufferMode) {
	gc->state.raster.drawBuffer = GL_BACK;
    } else {
	gc->state.raster.drawBuffer = GL_FRONT;
    }
    gc->state.raster.drawBufferReturn = gc->state.raster.drawBuffer;
    gc->state.current.userColor.r = one;
    gc->state.current.userColor.g = one;
    gc->state.current.userColor.b = one;
    gc->state.current.userColor.a = one;
    gc->state.current.userColorIndex = one;
    if (gc->modes.colorIndexMode) {
	gc->state.raster.writeMask = gc->frontBuffer.redMax;
	gc->state.current.color.r = gc->state.current.userColorIndex;
    } else {
	gc->state.current.color = gc->state.current.userColor;
    }
    gc->state.enables.general |= __GL_DITHER_ENABLE;

    gc->select.hit = GL_FALSE;
    gc->select.sp = gc->select.stack;

    gc->feedback.type = GL_2D;

    /*
    ** Initialize larger subsystems by calling their init codes.
    */
    __glInitEvaluatorState(gc);
    __glInitTextureState(gc);
    __glInitTransformState(gc);
    __glInitPixelState(gc);
    __glInitLUTCache(gc);
    __glInitVertexArrayState(gc);
    __glInitVertexCacheState(gc);
}

/*
** Destroy a context.  If it's the current context then the
** current context is set to GL_NULL.
*/
GLboolean __glDestroyContext(__GLcontext *gc)
{
    __GLlightSourceMachine *lsm;
    GLint i;

#if defined(__GL_CODEGEN) && defined(i386)
#if defined(__GL_USE_MMXCODE)
    if (gc->ogState.mxconstant_pool) {
	(*gc->imports.free)(gc, gc->ogState.mxconstant_pool);
    }
#endif /* __GL_USE_MMXCODE */
    if (gc->ogState.fpconstant_pool) {
	(*gc->imports.free)(gc, gc->ogState.fpconstant_pool);
    }
    if (gc->geomOGdata.slots[0].space) {
	(*gc->imports.free)(gc, gc->geomOGdata.slots[0].space);
    }
    __glReleaseCodeCache(gc);
#endif /* __GL_CODEGEN && i386 */

    if (gc->attributes.stack) {
	__glFreeAttributeState(gc);
	(*gc->imports.free)(gc, gc->attributes.stack);
    }

    for (i=0; i < gc->constants.maxClientAttribStackDepth; i++) {
	if (gc->attributes.clientStack[i]) {
	    (*gc->imports.free)(gc, gc->attributes.clientStack[i]);
	}
    }
    if (gc->attributes.clientStack) {
	(*gc->imports.free)(gc, gc->attributes.clientStack);
    }

    lsm = gc->light.source;
    for (i = 0; i < gc->constants.numberOfLights; i++, lsm++) {
	if (lsm->cache) {
	    __glFreeSpecLUT( gc, lsm->cache );
	}
    }
    if(gc->state.light.source) (*gc->imports.free)(gc, gc->state.light.source);
    if(gc->light.source) (*gc->imports.free)(gc, gc->light.source);
    if(gc->select.stack) (*gc->imports.free)(gc, gc->select.stack);

#if defined(__GL_ALLOCATE_SCANLINE_ARRAYS)
    if (gc->polygon.shader.colors)
	(*gc->imports.free)(gc, gc->polygon.shader.colors);
    if (gc->polygon.shader.fbcolors)
	(*gc->imports.free)(gc, gc->polygon.shader.fbcolors);
    if (gc->polygon.shader.stipplePat)
	(*gc->imports.free)(gc, gc->polygon.shader.stipplePat);
#endif /* __GL_ALLOCATE_SCANLINE_ARRAYS */

    /* XXX: The following should be freed in *_rgb.c or *_ci.c.  */
    /* FYI: It is allocated lazylily in so_alphatst.c */
    if( gc->frontBuffer.alphaTestFuncTable ) {
	(*gc->imports.free)(gc, gc->frontBuffer.alphaTestFuncTable);
    }

    /*
    ** Free other malloc'd data associated with the context
    */
    __glFreeEvaluatorState(gc);
    __glFreePixelState(gc);
    __glFreeVertexArrayState(gc);
    __glFreeTransformState(gc);
    __glFreeTextureState(gc);
    if (gc->dlist.dlistArray) __glFreeDlistState(gc);
    /*
    ** The material state may have references to the LUT cache, so
    ** free the material state entries to decrement the refcounts
    ** and allow the whole LUT cache to be freed.
    */

    if (gc->light.front.cache) __glFreeSpecLUT(gc, gc->light.front.cache);
    if (gc->light.back.cache) __glFreeSpecLUT(gc, gc->light.back.cache);
    if (gc->light.lutCache) __glFreeLUTCache(gc);
    __glFreeVertexCacheState(gc);

#if __GL_NUMBER_OF_AUX_BUFFERS > 0
    /*
    ** Free any aux color buffer records
    ** Note: Does not free the actual buffer memory, this is done elsewhere.
    */
    if (gc->auxBuffer) (*gc->imports.free)(gc, gc->auxBuffer);
#endif

    /*
    ** Note: We do not free the software buffers here.  They are attached
    ** to the drawable, and is the glx extension's responsibility to free
    ** them when the drawable is destroyed.
    */

    return GL_TRUE;
}

void __glSetError(GLenum code)
{
    __GL_SETUP();

    if ((*gc->imports.getenv)(gc, "GLERRORABORT")) {
	(*gc->imports.fatal)(gc, "__glSetError(): GLERRORABORT set, aborting");
    }

    if (!gc->error) {
	gc->error = code;
    }
    if (gc->procs.error) (*gc->procs.error)(gc, code);
}

GLint GLAPI __glim_RenderMode(GLenum mode)
{
    GLint rv = 0;
    __GL_SETUP_NOT_IN_BEGIN2();

    switch (mode) {
      case GL_RENDER:
      case GL_FEEDBACK:
      case GL_SELECT:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return 0;
    }

    __GL_API_STATE();

    /* Switch out of old render mode.  Get return value. */
    switch (gc->renderMode) {
      case GL_RENDER:
	rv = 0;
	break;
      case GL_FEEDBACK:
	rv = gc->feedback.overFlowed ? -1 :
	    (GLint)(gc->feedback.result - gc->feedback.resultBase);
	break;
      case GL_SELECT:
	rv = gc->select.overFlowed ? -1 : gc->select.hits;
	break;
    }

    /* Switch to new render mode */
    gc->renderMode = mode;
    __GL_DELAY_VALIDATE(gc);
    switch (mode) {
      case GL_FEEDBACK:
	if (!gc->feedback.resultBase) {
	    __glSetError(GL_INVALID_OPERATION);
	    return rv;
	}
	gc->feedback.result = gc->feedback.resultBase;
	gc->feedback.overFlowed = GL_FALSE;
	break;
      case GL_SELECT:
	if (!gc->select.resultBase) {
	    __glSetError(GL_INVALID_OPERATION);
	    return rv;
	}
	gc->select.result = gc->select.resultBase;
	gc->select.overFlowed = GL_FALSE;
	gc->select.sp = gc->select.stack;
	gc->select.hit = GL_FALSE;
	gc->select.hits = 0;
	gc->select.z = 0;
	break;
    }
    return rv;
}

/*
** CPU capabilities
*/
void __glGetCPUCaps(__GLcontext *gc)
{
    /* no processor-independent processor capabilities */
    gc->constants.cpuCaps = 0x0;
}
