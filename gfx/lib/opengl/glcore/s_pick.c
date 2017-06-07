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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_pick.c#29 $
*/
#include "render.h"
#include "context.h"
#include "global.h"
#if defined(__GL_USE_ASMCODE) && defined(mips)
#include "mips.h"
#endif /* __GL_USE_ASMCODE && mips */
#if defined(__GL_FR_RAST)
#include "fr_raster.h"
#endif /* __GL_FR_RAST */
#if defined(__GL_CODEGEN) && defined(i386)
#include "og_geom.h"
#endif /* __GL_CODEGEN && i386 */
#include "texmgr.h"

/************************************************************************/

/* these are depth test routines for C.. */
GLboolean (*__glCDTPixel[32])(__GLzValue, __GLzValue *) = {
    /* unsigned ops, no mask */
    __glDT_NEVER16,
    __glDT_LESS16,
    __glDT_EQUAL16,
    __glDT_LEQUAL16,
    __glDT_GREATER16,
    __glDT_NOTEQUAL16,
    __glDT_GEQUAL16,
    __glDT_ALWAYS16,
    /* unsigned ops, mask */
    __glDT_NEVER16,
    __glDT_LESS16_M,
    __glDT_EQUAL16_M,
    __glDT_LEQUAL16_M,
    __glDT_GREATER16_M,
    __glDT_NOTEQUAL16_M,
    __glDT_GEQUAL16_M,
    __glDT_ALWAYS16_M,
    /* unsigned ops, no mask */
    __glDT_NEVER,
    __glDT_LESS,
    __glDT_EQUAL,
    __glDT_LEQUAL,
    __glDT_GREATER,
    __glDT_NOTEQUAL,
    __glDT_GEQUAL,
    __glDT_ALWAYS,
    /* unsigned ops, mask */
    __glDT_NEVER,
    __glDT_LESS_M,
    __glDT_EQUAL_M,
    __glDT_LEQUAL_M,
    __glDT_GREATER_M,
    __glDT_NOTEQUAL_M,
    __glDT_GEQUAL_M,
    __glDT_ALWAYS_M,
};

#if defined(__GL_USE_ASMCODE) && defined(mips)
/*
** the following are depth testers for assemblerized
** depth test routines.  They have unique calling conventions
** and they should only be used in the routines that they are
** design for.
*/


/* for spans */
void (*__glSDepthTestPixel[32])(void) = {
    /* unsigned ops, no mask */
    __glDTS_NEVER16,
    __glDTS_LESS16,
    __glDTS_EQUAL16,
    __glDTS_LEQUAL16,
    __glDTS_GREATER16,
    __glDTS_NOTEQUAL16,
    __glDTS_GEQUAL16,
    __glDTS_ALWAYS16,
    /* unsigned ops, mask */
    __glDTS_NEVER16,
    __glDTS_LESS16_M,
    __glDTS_EQUAL16_M,
    __glDTS_LEQUAL16_M,
    __glDTS_GREATER16_M,
    __glDTS_NOTEQUAL16_M,
    __glDTS_GEQUAL16_M,
    __glDTS_ALWAYS16_M,
    /* unsigned ops, no mask */
    __glDTS_NEVER,
    __glDTS_LESS,
    __glDTS_EQUAL,
    __glDTS_LEQUAL,
    __glDTS_GREATER,
    __glDTS_NOTEQUAL,
    __glDTS_GEQUAL,
    __glDTS_ALWAYS,
    /* unsigned ops, mask */
    __glDTS_NEVER,
    __glDTS_LESS_M,
    __glDTS_EQUAL_M,
    __glDTS_LEQUAL_M,
    __glDTS_GREATER_M,
    __glDTS_NOTEQUAL_M,
    __glDTS_GEQUAL_M,
    __glDTS_ALWAYS_M,
};

/* for lines */
void (*__glPDepthTestPixel[32])(void) = {
    /* unsigned, no mask */
    __glDTP_NEVER16,
    __glDTP_LESS16,
    __glDTP_EQUAL16,
    __glDTP_LEQUAL16,
    __glDTP_GREATER16,
    __glDTP_NOTEQUAL16,
    __glDTP_GEQUAL16,
    __glDTP_ALWAYS16,
    /* unsigned, mask */
    __glDTP_NEVER16,
    __glDTP_LESS16_M,
    __glDTP_EQUAL16_M,
    __glDTP_LEQUAL16_M,
    __glDTP_GREATER16_M,
    __glDTP_NOTEQUAL16_M,
    __glDTP_GEQUAL16_M,
    __glDTP_ALWAYS16_M,
    /* unsigned, no mask */
    __glDTP_NEVER,
    __glDTP_LESS,
    __glDTP_EQUAL,
    __glDTP_LEQUAL,
    __glDTP_GREATER,
    __glDTP_NOTEQUAL,
    __glDTP_GEQUAL,
    __glDTP_ALWAYS,
    /* unsigned, mask */
    __glDTP_NEVER,
    __glDTP_LESS_M,
    __glDTP_EQUAL_M,
    __glDTP_LEQUAL_M,
    __glDTP_GREATER_M,
    __glDTP_NOTEQUAL_M,
    __glDTP_GEQUAL_M,
    __glDTP_ALWAYS_M,
};
#endif /* __GL_USE_ASMCODE && mips */

void __glGenericPickSpanProcs(__GLcontext *gc)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    __GLcolorBuffer *cfb = gc->drawBuffer;
    __GLspanFunc *sp;
    __GLstippledSpanFunc *ssp;
    int spanCount;
    GLboolean replicateSpan;

    replicateSpan = GL_FALSE;
    sp = gc->procs.span.spanFuncs;
    ssp = gc->procs.span.stippledSpanFuncs;

    /* Load phase one procs */
    if (!gc->transform.reasonableViewport) {
	*sp++ = __glClipSpan;
	*ssp++ = NULL;
    }
    
    if (modeFlags & __GL_SHADE_STIPPLE) {
	*sp++ = __glStippleSpan;
	*ssp++ = __glStippleStippledSpan;
    }

    /* Load phase two procs */

    /* 
    ** If alpha test is on, stencil and depth have to be done after
    ** the alpha test.  If not, doing them before interpolating
    ** and texture mapping, etc. is an optimization.
    */
    if (!(modeFlags & __GL_SHADE_ALPHA_TEST)) {
	if (modeFlags & __GL_SHADE_STENCIL_TEST) {
#if defined(__GL_USE_ASMCODE) && defined(mips)
	    *sp++ = __glStencilTestSpan_asm;
#else /* __GL_USE_ASMCODE && mips */
	    *sp++ = __glStencilTestSpan;
#endif /* __GL_USE_ASMCODE && mips */
	    *ssp++ = __glStencilTestStippledSpan;
	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
#if defined(__GL_USE_ASMCODE) && defined(mips)
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  /* No asm version yet */
		  *sp = __glDepthTestStencilOffsetSpan;
		  *ssp = __glDepthTestStencilStippledOffsetSpan;
		} else {
		  *sp = __glDepthTestStencilSpan_asm;
		  *ssp = __glDepthTestStencilStippledSpan_asm;
		}
#else /* __GL_USE_ASMCODE && mips */
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  *sp = __glDepthTestStencilOffsetSpan;
		  *ssp = __glDepthTestStencilStippledOffsetSpan;
		} else {
		  *sp = __glDepthTestStencilSpan;
		  *ssp = __glDepthTestStencilStippledSpan;
		}
#endif /* __GL_USE_ASMCODE && mips */
	    } else {
		*sp = __glDepthPassSpan;
		*ssp = __glDepthPassStippledSpan;
	    }
	    sp++;
	    ssp++;
	} else {
	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		if (gc->depthBuffer.testFunc == GL_NEVER) {
		    gc->procs.span.processSpan = (__GLspanFunc) __glNop1;
		    return;
		}
#if defined(__GL_USE_ASMCODE) && defined(mips)
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  /* No asm version yet */
		  *sp++ = __glDepthTestOffsetSpan;
		  *ssp++ = __glDepthTestStippledOffsetSpan;
		} else {
		  *sp++ = __glDepthTestSpan_asm;
		  *ssp++ = __glDepthTestStippledSpan_asm;
		}
#else /* __GL_USE_ASMCODE && mips */
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  *sp++ = __glDepthTestOffsetSpan;
		  *ssp++ = __glDepthTestStippledOffsetSpan;
		} else {
		  *sp++ = __glDepthTestSpan;
		  *ssp++ = __glDepthTestStippledSpan;
		}
#endif /* __GL_USE_ASMCODE && mips */
	    }
	}
    }

    /* Load phase three procs */
    if (modeFlags & __GL_SHADE_RGB) {
	if (modeFlags & __GL_SHADE_SMOOTH) {
	    *sp = __glShadeRGBASpan;
	    *ssp = __glShadeRGBASpan;
	} else {
	    *sp = __glFlatRGBASpan;
	    *ssp = __glFlatRGBASpan;
	}
    } else {
	if (modeFlags & __GL_SHADE_SMOOTH) {
	    *sp = __glShadeCISpan;
	    *ssp = __glShadeCISpan;
	} else {
	    *sp = __glFlatCISpan;
	    *ssp = __glFlatCISpan;
	}
    }
    sp++;
    ssp++;
    if (modeFlags & __GL_SHADE_TEXTURE) {
	*sp = __glTextureSpan;
	*ssp = __glTextureStippledSpan;
	sp++;
	ssp++;
    }
    if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
	if (modeFlags & __GL_SHADE_SMOOTH) {
	    *sp = __glShadeSecondaryRGBASpan;
	    *ssp = __glShadeSecondaryRGBASpan;
	} else {
	    *sp = __glFlatSecondaryRGBASpan;
	    *ssp = __glFlatSecondaryRGBASpan;
	}
	sp++;
	ssp++;
    }
    if (modeFlags & __GL_SHADE_SLOW_FOG) {
	if (gc->state.hints.fog == GL_NICEST) {
	    *sp = __glFogSpanSlow;
	    *ssp = __glFogStippledSpanSlow;
	} else {
	    *sp = __glFogSpan;
	    *ssp = __glFogStippledSpan;
	}
	sp++;
	ssp++;
    }

    if (modeFlags & __GL_SHADE_ALPHA_TEST) {
	*sp++ = __glAlphaTestSpan;
	*ssp++ = __glAlphaTestStippledSpan;

	/* If alpha test is on, need to do the depth/stencil tests now. */
	if (modeFlags & __GL_SHADE_STENCIL_TEST) {
#if defined(__GL_USE_ASMCODE) && defined(mips)
	    *sp++ = __glStencilTestSpan_asm;
#else /* __GL_USE_ASMCODE && mips */
	    *sp++ = __glStencilTestSpan;
#endif /* __GL_USE_ASMCODE && mips */
	    *ssp++ = __glStencilTestStippledSpan;
	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
#if defined(__GL_USE_ASMCODE) && defined(mips)
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  /* No asm version yet */
		  *sp = __glDepthTestStencilOffsetSpan;
		  *ssp = __glDepthTestStencilStippledOffsetSpan;
		} else {
		  *sp = __glDepthTestStencilSpan_asm;
		  *ssp = __glDepthTestStencilStippledSpan_asm;
		}
#else /* __GL_USE_ASMCODE && mips */
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  *sp = __glDepthTestStencilOffsetSpan;
		  *ssp = __glDepthTestStencilStippledOffsetSpan;
		} else {
		  *sp = __glDepthTestStencilSpan;
		  *ssp = __glDepthTestStencilStippledSpan;
		}
#endif /* __GL_USE_ASMCODE && mips */
	    } else {
		*sp = __glDepthPassSpan;
		*ssp = __glDepthPassStippledSpan;
	    }
	    sp++;
	    ssp++;
	} else {
	    if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		if (gc->depthBuffer.testFunc == GL_NEVER) {
		    gc->procs.span.processSpan = (__GLspanFunc) __glNop1;
		    return;
		}
#if defined(__GL_USE_ASMCODE) && defined(mips)
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  /* No asm version yet */
		  *sp++ = __glDepthTestOffsetSpan;
		  *ssp++ = __glDepthTestStippledOffsetSpan;
		} else {
		  *sp++ = __glDepthTestSpan_asm;
		  *ssp++ = __glDepthTestStippledSpan_asm;
		}
#else /* __GL_USE_ASMCODE && mips */
		if(modeFlags & __GL_SHADE_POLYGON_OFFSET_FILL) {
		  *sp++ = __glDepthTestOffsetSpan;
		  *ssp++ = __glDepthTestStippledOffsetSpan;
		} else {
		  *sp++ = __glDepthTestSpan;
		  *ssp++ = __glDepthTestStippledSpan;
		}
#endif /* __GL_USE_ASMCODE && mips */
	    }
	}
    }

    if (gc->buffers.doubleStore) {
	spanCount = (int)(sp - gc->procs.span.spanFuncs);
	gc->procs.span.n = spanCount;
	replicateSpan = GL_TRUE;
    } 

    /* Load phase four procs */
    if (cfb->needColorFragmentOps) {
	if (modeFlags & (__GL_SHADE_LOGICOP | __GL_SHADE_MASK)) {
	    *sp++ = cfb->fetchSpan;
	    *ssp++ = cfb->fetchStippledSpan;
	} 
	if (modeFlags & __GL_SHADE_BLEND) {
	    GLenum s = gc->state.raster.blendSrc;
	    GLenum d = gc->state.raster.blendDst;

	    if ((~modeFlags & __GL_SHADE_MASK) &&
		(d != GL_ZERO ||
		 s == GL_DST_COLOR || s == GL_ONE_MINUS_DST_COLOR ||
		 s == GL_DST_ALPHA || s == GL_ONE_MINUS_DST_ALPHA ||
		 s == GL_SRC_ALPHA_SATURATE)) {
		*sp++ = cfb->fetchSpan;
		*ssp++ = cfb->fetchStippledSpan;
	    }
	    if (s == GL_SRC_ALPHA) {
		if (d == GL_ONE_MINUS_SRC_ALPHA) {
		    *sp = __glBlendSpan_SA_MSA;
		} else if (d == GL_ONE) {
		    *sp = __glBlendSpan_SA_ONE;
		} else if (d == GL_ZERO) {
		    *sp = __glBlendSpan_SA_ZERO;
		} else {
		    *sp = __glBlendSpan;
		}
	    } else if (s == GL_ONE_MINUS_SRC_ALPHA && d == GL_SRC_ALPHA) {
		*sp = __glBlendSpan_MSA_SA;
	    } else {
		*sp = __glBlendSpan;
	    }
	    sp++;
	    *ssp++ = __glBlendStippledSpan;
	}
	if (modeFlags & __GL_SHADE_DITHER) {
	    if (modeFlags & __GL_SHADE_RGB) {
		*sp = __glDitherRGBASpan;
		*ssp = __glDitherRGBAStippledSpan;
	    } else {
		*sp = __glDitherCISpan;
		*ssp = __glDitherCIStippledSpan;
	    }
	} else {
	    if (modeFlags & __GL_SHADE_RGB) {
		*sp = __glRoundRGBASpan;
		*ssp = __glRoundRGBAStippledSpan;
	    } else {
		*sp = __glRoundCISpan;
		*ssp = __glRoundCIStippledSpan;
	    }
	}
	sp++;
	ssp++;
	if (modeFlags & __GL_SHADE_LOGICOP) {
	    *sp++ = __glLogicOpSpan;
	    *ssp++ = __glLogicOpStippledSpan;
	}
	if (modeFlags & __GL_SHADE_MASK) {
	    if (modeFlags & __GL_SHADE_RGB) {
		*sp = __glMaskRGBASpan;
		*ssp = __glMaskRGBASpan;
	    } else {
		*sp = __glMaskCISpan;
		*ssp = __glMaskCISpan;
	    }
	    sp++;
	    ssp++;
	}
    }

    /* Finally, copy over procs from drawBuffer */
    *sp++ = cfb->storeSpan;
    *ssp++ = cfb->storeStippledSpan;

    spanCount = (int)(sp - gc->procs.span.spanFuncs);
    gc->procs.span.m = spanCount;
    if (replicateSpan) {
	gc->procs.span.processSpan = __glProcessReplicateSpan;
    } else {
	gc->procs.span.processSpan = __glProcessSpan;
	gc->procs.span.n = spanCount;
    }
}

/************************************************************************/

void __glGenericPickPointProcs(__GLcontext *gc)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    /* first initialize the lock versions */
    if (gc->vertex.faceNeeds[__GL_FRONTFACE] == 0) {
	gc->procs.vertexPoints = __glPointFastLock;
    } else {
	gc->procs.vertexPoints = __glPointLock;
    }

    gc->procs.renderTrianglePolygonModePoint = __glRenderTrianglePoint;

    if (gc->renderMode == GL_FEEDBACK) {
	gc->procs.renderPoint = __glFeedbackPoint;
#if defined(__GL_HW_NEEDS_SYNC)
	gc->procs.renderPointTrap = __glRenderPointSWTrap;
	gc->procs.renderPointTrapBackup = gc->procs.renderPoint;
	gc->procs.renderPoint = gc->procs.renderPointTrap;
#endif /* __GL_HW_NEEDS_SYNC */
	return;
    } 
    if (gc->renderMode == GL_SELECT) {
	gc->procs.renderPoint = __glSelectPoint;
#if defined(__GL_HW_NEEDS_SYNC)
	gc->procs.renderPointTrap = __glRenderPointSWTrap;
	gc->procs.renderPointTrapBackup = gc->procs.renderPoint;
	gc->procs.renderPoint = gc->procs.renderPointTrap;
#endif /* __GL_HW_NEEDS_SYNC */
	return;
    } 

    if (gc->state.enables.general & __GL_POINT_SMOOTH_ENABLE) {
	if (gc->modes.colorIndexMode)
	    gc->procs.renderPoint = __glRenderAntiAliasedCIPoint;
	else
	    gc->procs.renderPoint = __glRenderAntiAliasedRGBPoint;
	
    } else {
	if (modeFlags & __GL_SHADE_TEXTURE) {
	    if (modeFlags & __GL_SHADE_SLOW_FOG) 
		gc->procs.renderPoint = __glShadeTextureFogPoint;
	    else
		gc->procs.renderPoint = __glShadeTexturePoint;
	    
	} else {
	    if (modeFlags & __GL_SHADE_SLOW_FOG) 
		gc->procs.renderPoint = __glShadeFogPoint;
	    else
		gc->procs.renderPoint = __glShadePoint;
	}    

	if (gc->state.point.aliasedSize == 1) 
	    gc->procs.renderPoint2 = __glRenderAliasedPoint1;
	else
	    gc->procs.renderPoint2 = __glRenderAliasedPointN;
    }

#if defined(__GL_HW_NEEDS_SYNC)
    gc->procs.renderPointTrap = __glRenderPointSWTrap;
    gc->procs.renderPointTrapBackup = gc->procs.renderPoint;
    gc->procs.renderPoint = gc->procs.renderPointTrap;
#endif /* __GL_HW_NEEDS_SYNC */
}

void __glGenericPickLineProcs(__GLcontext *gc)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    __GLspanFunc *sp;
    __GLstippledSpanFunc *ssp;
    int spanCount;
    GLboolean wideLine;
    GLboolean replicateLine;
    GLuint aaline;

    /* first initialize the lock versions */
    if (gc->vertex.faceNeeds[__GL_FRONTFACE] == 0) {
	gc->procs.vertexLStrip = __glOtherLStripVertexFastLock;
    } else {
	gc->procs.vertexLStrip = __glOtherLStripVertexLock;
    }
    gc->procs.vertex2ndLines = __gl2ndLinesVertexLock;
    gc->procs.vertex2ndLLoop = __gl2ndLLoopVertexLock;
    gc->procs.endLLoop = __glEndLLoopLock;

    gc->procs.renderTrianglePolygonModeLine = __glRenderTriangleLine;

    if (gc->renderMode == GL_FEEDBACK) {
	gc->procs.renderLine = __glFeedbackLine;
    } else if (gc->renderMode == GL_SELECT) {
	gc->procs.renderLine = __glSelectLine;
    } else {
	replicateLine = wideLine = GL_FALSE;

	aaline = gc->state.enables.general & __GL_LINE_SMOOTH_ENABLE;
	if (aaline) {
	    gc->procs.renderLine = __glRenderAntiAliasLine;
	} else {
	    gc->procs.renderLine = __glRenderAliasLine;
	}

	sp = gc->procs.line.lineFuncs;
	ssp = gc->procs.line.stippledLineFuncs;

	if (!aaline && (modeFlags & __GL_SHADE_LINE_STIPPLE)) {
	    *sp++ = __glStippleLine;
	    *ssp++ = NULL;
	}

	if (!aaline && gc->state.line.aliasedWidth > 1) {
	    wideLine = GL_TRUE;
	}
	spanCount = sp - gc->procs.line.lineFuncs;
	gc->procs.line.n = spanCount;

	*sp++ = __glScissorLine;
	*ssp++ = __glScissorStippledLine;

	if (!aaline) {
	    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
		*sp++ = __glStencilTestLine;
		*ssp++ = __glStencilTestStippledLine;
		if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		    *sp = __glDepthTestStencilLine;
		    *ssp = __glDepthTestStencilStippledLine;
		} else {
		    *sp = __glDepthPassLine;
		    *ssp = __glDepthPassStippledLine;
		}
		sp++;
		ssp++;
	    } else {
		if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		    if (gc->depthBuffer.testFunc == GL_NEVER) {
			/* Unexpected end of line routine picking! */
                        gc->procs.line.processLine = (__GLspanFunc) __glNop1;
                        return;
		    } else {
#if defined(__GL_USE_ASMCODE) && defined(mips)
			*sp++ = __glDepthTestLine_asm;
#else /* __GL_USE_ASMCODE && mips */
			*sp++ = __glDepthTestLine;
#endif /* __GL_USE_ASMCODE && mips */
		    }
		    *ssp++ = __glDepthTestStippledLine;
		}
	    }
	}

	/* Load phase three procs */
	if (modeFlags & __GL_SHADE_RGB) {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		*sp = __glShadeRGBASpan;
		*ssp = __glShadeRGBASpan;
	    } else {
		*sp = __glFlatRGBASpan;
		*ssp = __glFlatRGBASpan;
	    }
	} else {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		*sp = __glShadeCISpan;
		*ssp = __glShadeCISpan;
	    } else {
		*sp = __glFlatCISpan;
		*ssp = __glFlatCISpan;
	    }
	}
	sp++;
	ssp++;
	if (modeFlags & __GL_SHADE_TEXTURE) {
	    *sp++ = __glTextureSpan;
	    *ssp++ = __glTextureStippledSpan;
	}
	if (modeFlags & __GL_SHADE_SECONDARY_COLOR) {
	    if (modeFlags & __GL_SHADE_SMOOTH) {
		*sp = __glShadeSecondaryRGBASpan;
		*ssp = __glShadeSecondaryRGBASpan;
	    } else {
		*sp = __glFlatSecondaryRGBASpan;
		*ssp = __glFlatSecondaryRGBASpan;
	    }
	    sp++;
	    ssp++;
	}
	if (modeFlags & __GL_SHADE_SLOW_FOG) {
	    if (gc->state.hints.fog == GL_NICEST) {
		*sp = __glFogSpanSlow;
		*ssp = __glFogStippledSpanSlow;
	    } else {
		*sp = __glFogSpan;
		*ssp = __glFogStippledSpan;
	    }
	    sp++;
	    ssp++;
	}

	if (aaline) {
	    *sp++ = __glAntiAliasLine;
	    *ssp++ = __glAntiAliasStippledLine;
	}

	if (aaline) {
	    if (modeFlags & __GL_SHADE_STENCIL_TEST) {
		*sp++ = __glStencilTestLine;
		*ssp++ = __glStencilTestStippledLine;
		if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		    *sp = __glDepthTestStencilLine;
		    *ssp = __glDepthTestStencilStippledLine;
		} else {
		    *sp = __glDepthPassLine;
		    *ssp = __glDepthPassStippledLine;
		}
		sp++;
		ssp++;
	    } else {
		if (modeFlags & __GL_SHADE_DEPTH_TEST) {
		    if( gc->depthBuffer.testFunc == GL_NEVER ) {
			/* Unexpected end of line routine picking! */
                	gc->procs.line.processLine = (__GLspanFunc) __glNop1;
                	return;
		    } else {
#if defined(__GL_USE_ASMCODE) && defined(mips)
			*sp++ = __glDepthTestLine_asm;
#else /* __GL_USE_ASMCODE && mips */
			*sp++ = __glDepthTestLine;
#endif /* __GL_USE_ASMCODE && mips */
		    }
		    *ssp++ = __glDepthTestStippledLine;
		}
	    }
	}

	if (modeFlags & __GL_SHADE_ALPHA_TEST) {
	    *sp++ = __glAlphaTestSpan;
	    *ssp++ = __glAlphaTestStippledSpan;
	}

	if (gc->buffers.doubleStore) {
	    replicateLine = GL_TRUE;
	}
	spanCount = sp - gc->procs.line.lineFuncs;
	gc->procs.line.m = spanCount;

	if (0 == gc->drawBuffer->destMask &&
	    0 == (modeFlags & (__GL_SHADE_LOGICOP|__GL_SHADE_BLEND|__GL_SHADE_OWNERSHIP_TEST))) {
	    if (gc->modes.colorIndexMode) {
		if (gc->drawBuffer->buf.drawableBuf->elementSize == 1) {
		    /* 8-bit */
		    if (modeFlags & __GL_SHADE_DITHER) {
			if (modeFlags & (__GL_SHADE_SMOOTH |
					 __GL_SHADE_TEXTURE |
					 __GL_SHADE_SLOW_FOG)) {
			    *sp++ = __glStoreLine_CI_8_Smooth_Dither;
			    *ssp++ = __glStoreStippledLine_CI_8_Smooth_Dither;
			} else {
			    *sp++ = __glStoreLine_CI_8_Flat_Dither;
			    *ssp++ = __glStoreStippledLine_CI_8_Flat_Dither;
			}
		    } else {
			if (modeFlags & (__GL_SHADE_SMOOTH |
					 __GL_SHADE_TEXTURE |
					 __GL_SHADE_SLOW_FOG)) {
			    *sp++ = __glStoreLine_CI_8_Smooth;
			    *ssp++ = __glStoreStippledLine_CI_8_Smooth;
			} else {
			    *sp++ = __glStoreLine_CI_8_Flat;
			    *ssp++ = __glStoreStippledLine_CI_8_Flat;
			}
		    }
		} else {
		    /* Not fast path (yet) */
		    *sp++ = __glStoreLine;
		    *ssp++ = __glStoreStippledLine;
		}
	    } else { /* RGBA */
		if (gc->drawBuffer->buf.drawableBuf->elementSize == 2) {
		    /* 16-bit */
		    if (modeFlags & __GL_SHADE_DITHER) {
			*sp++ = __glStoreLine_RGB_16_Dither;
			*ssp++ = __glStoreStippledLine_RGB_16_Dither;
		    } else {
			if (modeFlags & (__GL_SHADE_SMOOTH |
					 __GL_SHADE_TEXTURE |
					 __GL_SHADE_SLOW_FOG)) {
			    *sp++ = __glStoreLine_RGB_16_Smooth;
			    *ssp++ = __glStoreStippledLine_RGB_16_Smooth;
			} else {
			    *sp++ = __glStoreLine_RGB_16_Flat;
			    *ssp++ = __glStoreStippledLine_RGB_16_Flat;
			}
		    }
		} else {
		    /* Not fast path (yet) */
		    *sp++ = __glStoreLine;
		    *ssp++ = __glStoreStippledLine;
		}
	    }
	} else {
	    *sp++ = __glStoreLine;
	    *ssp++ = __glStoreStippledLine;
	}

	spanCount = sp - gc->procs.line.lineFuncs;
	gc->procs.line.l = spanCount;

	sp = &gc->procs.line.wideLineRep;
	ssp = &gc->procs.line.wideStippledLineRep;
	if (wideLine) {
	    *sp = __glWideLineRep;
	    *ssp = __glWideStippleLineRep;
	    sp = &gc->procs.line.drawLine;
	    ssp = &gc->procs.line.drawStippledLine;
	} 
	if (replicateLine) {
	    *sp = __glDrawBothLine;
	    *ssp = __glDrawBothStippledLine;
	} else {
	    *sp = (__GLspanFunc) __glNop1;
	    *ssp = (__GLstippledSpanFunc) __glNop1;
	    gc->procs.line.m = gc->procs.line.l;
	}
	if (!wideLine) {
	    gc->procs.line.n = gc->procs.line.m;
	}

	if (!wideLine && !replicateLine && spanCount == 3) {
	    gc->procs.line.processLine = __glProcessLine3NW;
	} else {
	    gc->procs.line.processLine = __glProcessLine;
	}
	if ((modeFlags & __GL_SHADE_CHEAP_FOG) &&
		!(modeFlags & __GL_SHADE_SMOOTH_LIGHT)) {
	    gc->procs.renderLine2 = gc->procs.renderLine;
	    gc->procs.renderLine = __glRenderFlatFogLine;
	}
    }

#if defined(__GL_HW_NEEDS_SYNC)
    gc->procs.renderLineTrap = __glRenderLineSWTrap;
    gc->procs.renderLineTrapBackup = gc->procs.renderLine;
    gc->procs.renderLine = gc->procs.renderLineTrap;
#endif /* __GL_HW_NEEDS_SYNC */
}

/*
** Pick the fastest triangle rendering implementation available based on
** the current mode set.  This implementation only has a few triangle
** procs, and falls back on the generic all purpose one when forced to.
*/
void __glGenericPickTriangleProcs(__GLcontext *gc)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    /* first initialize the lock vertex functions */
    gc->procs.vertexEvenTStrip = __glEvenTStripVertexLock;
    gc->procs.vertexOddTStrip = __glOddTStripVertexLock;
    gc->procs.vertexTFan = __glOtherTFanVertexLock;
    gc->procs.vertex3rdTris = __gl3rdTrianglesVertexLock;
    gc->procs.vertex4thQStrip = __gl4thQStripVertexLock;
    gc->procs.vertex4thQuads = __gl4thQuadsVertexLock;
    gc->procs.vertexPoly = __glPolygonVertexLock;
    gc->procs.endPoly = __glEndPolygonLock;

    /*
    ** Setup cullFace so that a single test will do the cull check.
    */
    if (modeFlags & __GL_SHADE_CULL_FACE) {
	switch (gc->state.polygon.cull) {
	  case GL_FRONT:
	    gc->polygon.cullFace = __GL_CULL_FLAG_FRONT;
	    break;
	  case GL_BACK:
	    gc->polygon.cullFace = __GL_CULL_FLAG_BACK;
	    break;
	  case GL_FRONT_AND_BACK:
	    gc->procs.renderTriangle = __glDontRenderTriangle;
	    gc->procs.fillTriangle = 0;		/* Done to find bugs */
#if defined(__GL_HW_NEEDS_SYNC)
	    gc->procs.renderTriangleTrap = __glRenderTriangleSWTrap;
	    gc->procs.renderTriangleTrapBackup = gc->procs.renderTriangle;
	    gc->procs.renderTriangle = gc->procs.renderTriangleTrap;
#endif /* __GL_HW_NEEDS_SYNC */
	    return;
	}
    } else {
	gc->polygon.cullFace = __GL_CULL_FLAG_DONT;
    }

    /* Build lookup table for face direction */
    switch (gc->state.polygon.frontFaceDirection) {
      case GL_CW:
	if (gc->constants.yInverted) {
	    gc->polygon.face[__GL_CW] = __GL_BACKFACE;
	    gc->polygon.face[__GL_CCW] = __GL_FRONTFACE;
	} else {
	    gc->polygon.face[__GL_CW] = __GL_FRONTFACE;
	    gc->polygon.face[__GL_CCW] = __GL_BACKFACE;
	}
	break;
      case GL_CCW:
	if (gc->constants.yInverted) {
	    gc->polygon.face[__GL_CW] = __GL_FRONTFACE;
	    gc->polygon.face[__GL_CCW] = __GL_BACKFACE;
	} else {
	    gc->polygon.face[__GL_CW] = __GL_BACKFACE;
	    gc->polygon.face[__GL_CCW] = __GL_FRONTFACE;
	}
	break;
    }

    /* Make polygon mode indexable and zero based */
    gc->polygon.mode[__GL_FRONTFACE] =
	(GLubyte) (gc->state.polygon.frontMode & 0xf);
    gc->polygon.mode[__GL_BACKFACE] =
	(GLubyte) (gc->state.polygon.backMode & 0xf);
    
    if (gc->renderMode == GL_FEEDBACK) {
	gc->procs.renderTriangle = __glFeedbackTriangle;
	gc->procs.fillTriangle = 0;		/* Done to find bugs */
#if defined(__GL_HW_NEEDS_SYNC)
	gc->procs.renderTriangleTrap = __glRenderTriangleSWTrap;
	gc->procs.renderTriangleTrapBackup = gc->procs.renderTriangle;
	gc->procs.renderTriangle = gc->procs.renderTriangleTrap;
#endif /* __GL_HW_NEEDS_SYNC */
	return;
    }
    if (gc->renderMode == GL_SELECT) {
	gc->procs.renderTriangle = __glSelectTriangle;
	gc->procs.fillTriangle = 0;		/* Done to find bugs */
#if defined(__GL_HW_NEEDS_SYNC)
	gc->procs.renderTriangleTrap = __glRenderTriangleSWTrap;
	gc->procs.renderTriangleTrapBackup = gc->procs.renderTriangle;
	gc->procs.renderTriangle = gc->procs.renderTriangleTrap;
#endif /* __GL_HW_NEEDS_SYNC */
	return;
    }

#if defined(__GL_FR_RAST)
    if (__glFRPickTriangleProcs(gc)) {
#if defined(__GL_HW_NEEDS_SYNC)
	gc->procs.renderTriangleTrap = __glRenderTriangleSWTrap;
	gc->procs.renderTriangleTrapBackup = gc->procs.renderTriangle;
	gc->procs.renderTriangle = gc->procs.renderTriangleTrap;
#endif /* __GL_HW_NEEDS_SYNC */
	return;
    }
#endif /* __GL_FR_RAST */

    if ((gc->state.polygon.frontMode == gc->state.polygon.backMode) &&
	    (gc->state.polygon.frontMode == GL_FILL)) {
	if (modeFlags & __GL_SHADE_SMOOTH_LIGHT) {
	    gc->procs.renderTriangle = __glRenderSmoothTriangle;
	} else {
	    gc->procs.renderTriangle = __glRenderFlatTriangle;
	}
    } else {
	gc->procs.renderTriangle = __glRenderTriangle;
    }

    if (gc->state.enables.general & __GL_POLYGON_SMOOTH_ENABLE) {
	gc->procs.fillTriangle = __glFillAntiAliasedTriangle;
    } else {
	gc->procs.fillTriangle = __glFillTriangle;
    }
    if ((modeFlags & __GL_SHADE_CHEAP_FOG) &&
	    !(modeFlags & __GL_SHADE_SMOOTH_LIGHT)) {
	gc->procs.fillTriangle2 = gc->procs.fillTriangle;
	gc->procs.fillTriangle = __glFillFlatFogTriangle;
    }

#if defined(__GL_HW_NEEDS_SYNC)
    gc->procs.renderTriangleTrap = __glRenderTriangleSWTrap;
    gc->procs.renderTriangleTrapBackup = gc->procs.renderTriangle;
    gc->procs.renderTriangle = gc->procs.renderTriangleTrap;
#endif /* __GL_HW_NEEDS_SYNC */
}

void __glGenericPickRenderBitmapProcs(__GLcontext *gc)
{
    gc->procs.renderBitmap = __glRenderBitmap;
}

void __glGenericPickClipProcs(__GLcontext *gc)
{
    if (gc->state.light.shadingModel == GL_FLAT) {
	gc->procs.clipLine = __glFastClipFlatLine;
    } else {
	gc->procs.clipLine = __glFastClipSmoothLine;
    }
    gc->procs.clipTriangle = __glClipTriangle;
}

void __glGenericPickCalcTextureProcs(__GLcontext *gc)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLint i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	/* Pick coordinate generation function */
	if ((gc->state.enables.texture[i] & __GL_TEXTURE_GEN_S_ENABLE) &&
	    (gc->state.enables.texture[i] & __GL_TEXTURE_GEN_T_ENABLE) &&
	    !(gc->state.enables.texture[i] & __GL_TEXTURE_GEN_R_ENABLE) &&
	    !(gc->state.enables.texture[i] & __GL_TEXTURE_GEN_Q_ENABLE) &&
	    (gc->state.texture.unit[i].s.mode ==
			gc->state.texture.unit[i].t.mode)) {
	    /* Use a special function when both s&t are enabled and identical */
	    switch (gc->state.texture.unit[i].s.mode) {
	      case GL_EYE_LINEAR:
		gc->procs.calcTextureUnit[i] = __glCalcEyeLinear;
		break;
	      case GL_OBJECT_LINEAR:
		gc->procs.calcTextureUnit[i] = __glCalcObjectLinear;
		break;
	      case GL_SPHERE_MAP:
		gc->procs.calcTextureUnit[i] = __glCalcSphereMap;
		break;
	    }
	} else {
	    if (!(gc->state.enables.texture[i] & __GL_TEXTURE_GEN_S_ENABLE) &&
		!(gc->state.enables.texture[i] & __GL_TEXTURE_GEN_T_ENABLE) &&
		!(gc->state.enables.texture[i] & __GL_TEXTURE_GEN_R_ENABLE) &&
		!(gc->state.enables.texture[i] & __GL_TEXTURE_GEN_Q_ENABLE)) {
		/* Use fast function when both are disabled */
		gc->procs.calcTextureUnit[i] = __glCalcTexture;
	    } else {
		gc->procs.calcTextureUnit[i] = __glCalcMixedTexture;
	    }
	}
	gc->procs.calcRasterTextureUnit[i] = gc->procs.calcTextureUnit[i];

	if ((modeFlags & __GL_SHADE_PROJSCALED_TEXTURE) &&
	    (modeFlags & __GL_SHADE_UVSCALED_TEXTURE)) {
	    gc->procs.calcTexture2Unit[i] = gc->procs.calcTextureUnit[i];
	    if (gc->procs.calcTextureUnit[i] == __glCalcTexture) {
		gc->procs.calcTextureUnit[i] = __glFastCalcTexturePerspUVScale;
	    } else {
		gc->procs.calcTextureUnit[i] = __glCalcTexturePerspUVScale;
	    }
	} else if (modeFlags & __GL_SHADE_PROJSCALED_TEXTURE) {
	    gc->procs.calcTexture2Unit[i] = gc->procs.calcTextureUnit[i];
	    if (gc->procs.calcTextureUnit[i] == __glCalcTexture) {
		gc->procs.calcTextureUnit[i] = __glFastCalcTexturePersp;
	    } else {
		gc->procs.calcTextureUnit[i] = __glCalcTexturePersp;
	    }
	} else if (modeFlags & __GL_SHADE_UVSCALED_TEXTURE) {
	    gc->procs.calcTexture2Unit[i] = gc->procs.calcTextureUnit[i];
	    if (gc->procs.calcTextureUnit[i] == __glCalcTexture) {
		gc->procs.calcTextureUnit[i] = __glFastCalcTextureUVScale;
	    } else {
		gc->procs.calcTextureUnit[i] = __glCalcTextureUVScale;
	    }
	}
    }
    gc->procs.calcTexture = __glCalcMultiTexture;
}

void __glGenericPickTextureEnables(__GLcontext *gc)
{
    GLboolean textureEnabled = GL_FALSE;
    GLint i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	__GLtextureUnitMachine *tu = &gc->texture.unit[i];

	tu->currentTexture = NULL;
	if (gc->state.enables.texture[i] & __GL_TEXTURE_3D_ENABLE) {
	    __GLtexture *tex = __glLookupTexture(gc, i, GL_TEXTURE_3D);

	    if (__glIsTextureConsistent(gc, tex)) {
		gc->texture.unit[i].currentTexture = tex;
		textureEnabled = GL_TRUE;
	    }
	} else
	if (gc->state.enables.texture[i] & __GL_TEXTURE_2D_ENABLE) {
	    __GLtexture *tex = __glLookupTexture(gc, i, GL_TEXTURE_2D);

	    if (__glIsTextureConsistent(gc, tex)) {
		gc->texture.unit[i].currentTexture = tex;
		textureEnabled = GL_TRUE;
	    }
	} else
	if (gc->state.enables.texture[i] & __GL_TEXTURE_1D_ENABLE) {
	    __GLtexture *tex = __glLookupTexture(gc, i, GL_TEXTURE_1D);

	    if (__glIsTextureConsistent(gc, tex)) {
		gc->texture.unit[i].currentTexture = tex;
		textureEnabled = GL_TRUE;
	    }
	}
    }
    gc->texture.textureEnabled = textureEnabled;
}

void __glGenericPickTextureProcs(__GLcontext *gc)
{
    GLuint modeFlags = gc->polygon.shader.modeFlags;
    GLint i;

    for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	__GLtexture *current = gc->texture.unit[i].currentTexture;
	__GLmipMapLevel *lp;

	/* Pick texturing function for the current texture */
	if (current) {
	    __GLtextureParamState *params = &current->state.params;
	    GLenum baseFormat, mode;

	    /*
	    ** Figure out if mipmapping is being used.  If not, then the
	    ** rho computations can be avoided as there is only one texture
	    ** to choose from.
	    */
	    gc->procs.calcLineRho = __glComputeLineRho;
	    gc->procs.calcPolygonRho = __glComputePolygonRho;
	    if ((params->minFilter == GL_LINEAR) ||
		(params->minFilter == GL_NEAREST)) {
		/* No mipmapping needed */

		if (params->minFilter == params->magFilter) {
		    /* No rho needed as min/mag application is identical */
		    current->textureFunc = __glFastTextureFragment;
		    gc->procs.calcLineRho = __glNopLineRho;
		    gc->procs.calcPolygonRho = __glNopPolygonRho;

		} else {
		    current->textureFunc = __glTextureFragment;

		    /*
		    ** Pre-calculate min/mag switchover point.  The rho
		    ** calculation doesn't perform a square root (ever).
		    ** Consequently, these constants are squared.
		    */
		    if ((params->magFilter == GL_LINEAR) &&
			((params->minFilter ==
					GL_NEAREST_MIPMAP_NEAREST) ||
			 (params->minFilter ==
					GL_LINEAR_MIPMAP_NEAREST))) {
			current->c = (__GLfloat)2.0;
		    } else {
			current->c = __glOne;
		    }
		}
	    } else {
		/* mipmapping */
		current->textureFunc = __glMipMapFragment;

		/*
		** Pre-calculate min/mag switchover point.  The rho
		** calculation doesn't perform a square root (ever).
		** Consequently, these constants are squared.
		*/
		if ((params->magFilter == GL_LINEAR) &&
		    ((params->minFilter == GL_NEAREST_MIPMAP_NEAREST) ||
		     (params->minFilter == GL_LINEAR_MIPMAP_NEAREST))) {
		    current->c = (__GLfloat)2.0;
		} else {
		    current->c = __glOne;
		}
	    }

	    lp = current->level[0];
	    current->onebyoneLevel = lp->widthLog2;
	    if (lp->heightLog2 > current->onebyoneLevel) {
		current->onebyoneLevel = lp->heightLog2;
	    }
	    if (lp->depthLog2 > current->onebyoneLevel) {
		current->onebyoneLevel = lp->depthLog2;
	    }

	    current->effectiveMaxLOD = params->maxLOD + params->baseLevel;
	    if (current->effectiveMaxLOD < params->baseLevel) {
		current->effectiveMaxLOD = params->baseLevel;
	    } else if (current->effectiveMaxLOD > params->maxLevel) {
		current->effectiveMaxLOD = params->maxLevel;
	    }
	    if (current->effectiveMaxLOD < 0) {
		current->effectiveMaxLOD = 0;
	    } else if (current->effectiveMaxLOD > current->onebyoneLevel) {
		current->effectiveMaxLOD = current->onebyoneLevel;
	    }

	    /* Pick environment function according to base format and mode */
	    if ((gc->modes.rgbMode == GL_TRUE) &&
		(current->level[params->baseLevel]->baseFormat == GL_COLOR_INDEX)) {
		baseFormat = current->CT.baseFormat;
	    } else {
		baseFormat = current->level[params->baseLevel]->baseFormat;
	    }
	    current->baseFormat = baseFormat;
	    mode = gc->state.texture.unit[i].env[0].mode;
	    switch (mode) {
	      case GL_MODULATE:
		switch (baseFormat) {
		  case GL_LUMINANCE:
		      current->env = __glTextureModulateL;
		    break;
		  case GL_LUMINANCE_ALPHA:
		      current->env = __glTextureModulateLA;
		    break;
		  case GL_RGB:
		      current->env = __glTextureModulateRGB;
		    break;
		  case GL_RGBA:
		      current->env = __glTextureModulateRGBA;
		    break;
		  case GL_ALPHA:
		      current->env = __glTextureModulateA;
		    break;
		  case GL_INTENSITY:
		      current->env = __glTextureModulateI;
		    break;
		case GL_COLOR_INDEX:
		    current->env = __glTextureModulateCI;
		    break;
		}
		break;
	      case GL_DECAL:
		switch (baseFormat) {
		  case GL_LUMINANCE:
		    current->env = (void (*)(__GLcontext *gc, 
			    __GLcolor *frag, __GLtexel *texel)) __glNop3;
		    break;
		  case GL_LUMINANCE_ALPHA:
		    current->env = (void (*)(__GLcontext *gc, 
			    __GLcolor *frag, __GLtexel *texel)) __glNop3;
		    break;
		  case GL_RGB:
		      current->env = __glTextureDecalRGB;
		    break;
		  case GL_RGBA:
		      current->env = __glTextureDecalRGBA;
		    break;
		  case GL_ALPHA:
		    current->env = (void (*)(__GLcontext *gc, 
			    __GLcolor *frag, __GLtexel *texel)) __glNop3;
		    break;
		  case GL_INTENSITY:
		    current->env = (void (*)(__GLcontext *gc, 
			    __GLcolor *frag, __GLtexel *texel)) __glNop3;
		    break;
		}
		break;
	      case GL_BLEND:
		switch (baseFormat) {
		  case GL_LUMINANCE:
		      current->env = __glTextureBlendL;
		    break;
		  case GL_LUMINANCE_ALPHA:
		      current->env = __glTextureBlendLA;
		    break;
		  case GL_RGB:
		      current->env = __glTextureBlendRGB;
		    break;
		  case GL_RGBA:
		      current->env = __glTextureBlendRGBA;
		    break;
		  case GL_ALPHA:
		      current->env = __glTextureBlendA;
		    break;
		  case GL_INTENSITY:
		      current->env = __glTextureBlendI;
		    break;
		}
		break;
	      case GL_REPLACE:
		switch (baseFormat) {
		  case GL_LUMINANCE:
		      current->env = __glTextureReplaceL;
		    break;
		  case GL_LUMINANCE_ALPHA:
		      current->env = __glTextureReplaceLA;
		    break;
		  case GL_RGB:
		      current->env = __glTextureReplaceRGB;
		    break;
		  case GL_RGBA:
		      current->env = __glTextureReplaceRGBA;
		    break;
		  case GL_ALPHA:
		      current->env = __glTextureReplaceA;
		    break;
		  case GL_INTENSITY:
		      current->env = __glTextureReplaceI;
		    break;
		  case GL_COLOR_INDEX:
		      current->env = __glTextureReplaceCI;
		    break;
		}
		break;
	      case GL_ADD:
		switch (baseFormat) {
		  case GL_COLOR_INDEX:
		    current->env = __glTextureAddCI;
		    break;
		}
		break;
	    }

	    /* Pick mag/min functions */
	    switch (current->dim) {
	      case 1:
		current->nearest = __glNearestFilter1;
		current->linear = __glLinearFilter1;
		break;
	      case 2:
		current->nearest = __glNearestFilter2;
		current->linear = __glLinearFilter2;
		break;
	      case 3:
		current->nearest = __glNearestFilter3;
		current->linear = __glLinearFilter3;
		break;
	    }

	    /* set min filter function */
	    switch (params->minFilter) {
	      case GL_LINEAR:
		if (modeFlags & __GL_SHADE_UVSCALED_TEXTURE) {
		    current->minnify = __glLinearFilterUVScaled;
		} else {
		    current->minnify = __glLinearFilter;
		}
		break;
	      case GL_NEAREST:
		if (modeFlags & __GL_SHADE_UVSCALED_TEXTURE) {
		    current->minnify = __glNearestFilterUVScaled;
		} else {
		    current->minnify = __glNearestFilter;
		}
		break;
	      case GL_NEAREST_MIPMAP_NEAREST:
		current->minnify = __glNMNFilter;
		break;
	      case GL_LINEAR_MIPMAP_NEAREST:
		current->minnify = __glLMNFilter;
		break;
	      case GL_NEAREST_MIPMAP_LINEAR:
		current->minnify = __glNMLFilter;
		break;
	      case GL_LINEAR_MIPMAP_LINEAR:
		current->minnify = __glLMLFilter;
		break;
	    }

	    /* set mag filter function */
	    switch (params->magFilter) {
	      case GL_LINEAR:
		if (modeFlags & __GL_SHADE_UVSCALED_TEXTURE) {
		    current->magnify = __glLinearFilterUVScaled;
		} else {
		    current->magnify = __glLinearFilter;
		}
		break;
	      case GL_NEAREST:
		if (modeFlags & __GL_SHADE_UVSCALED_TEXTURE) {
		    current->magnify = __glNearestFilterUVScaled;
		} else {
		    current->magnify = __glNearestFilter;
		}
		break;
	    }

	    gc->procs.textureUnit[i] = current->textureFunc;
	    if (current->residence == __GL_TEXTURE_NOT_RESIDENT) {
		/* texture not loaded.  Load it. */
		(*current->makeResident)(gc, current, params->priority);
	    }
	} else {
	    gc->procs.textureUnit[i] = 0;
	}
    }

    gc->procs.texture = __glMultiTextureFragment;
    if (modeFlags & __GL_SHADE_UVSCALED_TEXTURE) {
	gc->procs.textureRaster = __glMultiTextureFragmentUVScale;
    } else {
	gc->procs.textureRaster = gc->procs.texture;
    }
}

void __glGenericPickFogProcs(__GLcontext *gc)
{
    if (gc->state.enables.general & __GL_FOG_ENABLE) {
	if (gc->state.hints.fog == GL_NICEST) {
	    gc->procs.fogVertex = 0;	/* Better not be called */
	} else {
	    if (gc->state.fog.mode == GL_LINEAR) 
		gc->procs.fogVertex = __glFogVertexLinear;
	    else
		gc->procs.fogVertex = __glFogVertex;
	}
	gc->procs.fogPoint = __glFogFragmentSlow;
	gc->procs.fogColor = __glFogColorSlow;
    } else {
	gc->procs.fogVertex = 0;
	gc->procs.fogPoint = 0;
	gc->procs.fogColor = 0;
    }
}

void __glGenericPickBufferProcs(__GLcontext *gc)
{
    __GLbufferMachine *buffers;
    GLuint renderBufferMask = 0;
    GLuint drawBufferMask = 0, readBufferMask = 0;

    buffers = &gc->buffers;
    buffers->doubleStore = GL_FALSE;

    gc->buffers.lock.bufferToLock = NULL;

    /* Set draw buffer pointer */
    switch (gc->state.raster.drawBuffer) {
      case GL_FRONT:
	gc->drawBuffer = gc->front;
	drawBufferMask = __GL_FRONT_BUFFER_MASK;
	if (gc->front->buf.drawableBuf->lock &&
	    gc->front->buf.drawableBuf->unlock) {
	    renderBufferMask = __GL_FRONT_BUFFER_MASK;
	    gc->buffers.lock.bufferToLock = &gc->front->buf;
	}
	break;
      case GL_FRONT_AND_BACK:
	if (gc->modes.doubleBufferMode) {
	    gc->drawBuffer = gc->back;
	    buffers->doubleStore = GL_TRUE;
	    drawBufferMask = __GL_FRONT_BUFFER_MASK | __GL_BACK_BUFFER_MASK;
	    if (gc->front->buf.drawableBuf->lock &&
		gc->front->buf.drawableBuf->unlock) {
		renderBufferMask = __GL_FRONT_BUFFER_MASK;
	    }
	    if (gc->back->buf.drawableBuf->lock &&
		gc->back->buf.drawableBuf->unlock) {
		renderBufferMask |= __GL_BACK_BUFFER_MASK;
	    }
	} else {
	    gc->drawBuffer = gc->front;
	    drawBufferMask = __GL_FRONT_BUFFER_MASK;
	    if (gc->front->buf.drawableBuf->lock &&
		gc->front->buf.drawableBuf->unlock) {
		renderBufferMask = __GL_FRONT_BUFFER_MASK;
		gc->buffers.lock.bufferToLock = &gc->front->buf;
	    }
	}
	break;
      case GL_BACK:
	gc->drawBuffer = gc->back;
	drawBufferMask = __GL_BACK_BUFFER_MASK;
	if (gc->back->buf.drawableBuf->lock &&
	    gc->back->buf.drawableBuf->unlock) {
	    renderBufferMask = __GL_BACK_BUFFER_MASK;
	    gc->buffers.lock.bufferToLock = &gc->back->buf;
	}
	break;
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3: 
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
        {
	    GLint i = gc->state.raster.drawBuffer - GL_AUX0;
	    gc->drawBuffer = &gc->auxBuffer[i];
	    drawBufferMask = __GL_AUX_BUFFER_MASK(i);
	    if (gc->auxBuffer[i].buf.drawableBuf->lock &&
		gc->auxBuffer[i].buf.drawableBuf->unlock) {
		renderBufferMask = __GL_AUX_BUFFER_MASK(i);
		gc->buffers.bufferToLock = &gc->auxBuffer[i];
	    }
        }
#endif
	break;
      default:
	  break;
    }

    /* set read buffer pointer */
    switch (gc->state.pixel.readBuffer) {
      case GL_FRONT:
	  readBufferMask = __GL_FRONT_BUFFER_MASK;
	  break;
      case GL_BACK:
	  readBufferMask = __GL_BACK_BUFFER_MASK;
	  break;
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3:
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
	  {
	      GLint i = gc->state.raster.readBuffer = GL_AUX0;
	      readBufferMask |= __GL_AUX_BUFFER_MASK(i);
	  }
#endif
          break;
      default:
	  break;
    }

    /* figure out lock routines */
    gc->buffers.lock.lockBuffers = __glLockBuffers;
    gc->buffers.lock.unlockBuffers = __glUnlockBuffers;
    gc->buffers.lock.lockRenderBuffers = __glLockRenderBuffers;
    gc->buffers.lock.unlockRenderBuffers = __glUnlockRenderBuffers;

    if (gc->polygon.shader.modeFlags & __GL_SHADE_DEPTH_TEST) {
	renderBufferMask |= __GL_DEPTH_BUFFER_MASK;
    }
    if (gc->polygon.shader.modeFlags & __GL_SHADE_STENCIL_TEST) {
	renderBufferMask |= __GL_STENCIL_BUFFER_MASK;
    }

    /* set fast paths for buffer locking */
    if (((drawBufferMask | __GL_DEPTH_BUFFER_MASK) == renderBufferMask) &&
	(buffers->doubleStore == GL_FALSE)) {
	/* only one color buffer and depth buffer */
	gc->buffers.lock.lockRenderBuffers = __glLockColorBufferDepthBuffer;
	gc->buffers.lock.unlockRenderBuffers = __glUnlockColorBufferDepthBuffer;
    }
    if ((drawBufferMask == renderBufferMask) &&
	(buffers->doubleStore == GL_FALSE)) {
	/* only one color buffer */
	gc->buffers.lock.lockRenderBuffers = __glLockColorBuffer;
	gc->buffers.lock.unlockRenderBuffers = __glUnlockColorBuffer;
    }

    gc->buffers.lock.renderBufferMask = renderBufferMask;
    gc->buffers.lock.drawBufferMask = drawBufferMask;
    gc->buffers.lock.readBufferMask = readBufferMask;
}

void __glGenericPickPixelProcs(__GLcontext *gc)
{
    __GLpixelTransferMode *tm;
    __GLpixelMachine *pm;
    GLboolean mapColor;
    GLfloat red, green, blue, alpha;
    GLint entry;
    GLuint enables = gc->state.enables.general;
    GLuint pixelModeFlags = 0;
    __GLpixelMapHead *pmap;

    /* Set read buffer pointer */
    switch (gc->state.pixel.readBuffer) {
      case GL_FRONT:
	gc->readBuffer = gc->front;
	break;
      case GL_BACK:
	gc->readBuffer = gc->back;
	break;
      case GL_AUX0:
      case GL_AUX1:
      case GL_AUX2:
      case GL_AUX3:
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
        {
	    GLint i = gc->state.pixel.readBuffer - GL_AUX0;
	    gc->readBuffer = &gc->auxBuffer[i];
        }
#endif
	break;
    }

    /*
    ** Rasterization
    */
    if (gc->texture.textureEnabled || (enables & __GL_FOG_ENABLE)) {
	gc->procs.pxStore = __glSlowDrawPixelsStore;
    } else {
	gc->procs.pxStore = gc->procs.store;
    }

    /*
    ** Pixel Transfer Operations
    */
    tm = &gc->state.pixel.transferMode;
    pm = &(gc->pixel);
    mapColor = tm->mapColor;
    if (mapColor || gc->modes.rgbMode || tm->indexShift || tm->indexOffset) {
	pm->iToICurrent = GL_FALSE;
	pm->iToRGBACurrent = GL_FALSE;
	pm->modifyCI = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_INDEX_LOOKUP;
	pixelModeFlags |= __GL_PIXEL_MODE_INDEX_SHIFT_OFFSET;
    } else {
	pm->modifyCI = GL_FALSE;
    }
    if (tm->mapStencil || tm->indexShift || tm->indexOffset) {
	pm->modifyStencil = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_STENCIL_LOOKUP;
	pixelModeFlags |= __GL_PIXEL_MODE_INDEX_SHIFT_OFFSET;
    } else {
	pm->modifyStencil = GL_FALSE;
    }
    if (tm->depthScale != __glOne || tm->depthBias) {
	pm->modifyDepth = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_DEPTH_SCALE_BIAS;
    } else {
	pm->modifyDepth = GL_FALSE;
    }
    if (mapColor || tm->bias.r || tm->bias.g || tm->bias.b || tm->bias.a ||
	tm->scale.r != __glOne || tm->scale.g != __glOne ||
	tm->scale.b != __glOne || tm->scale.a != __glOne) {
	pm->rgbaCurrent = GL_FALSE;
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_RGBA_LOOKUP;
	pixelModeFlags |= __GL_PIXEL_MODE_RGBA_SCALE_BIAS;
    } else {
	pm->modifyRGBA = GL_FALSE;
    }

    if (pm->modifyRGBA) {
	/* Compute default values for red, green, blue, alpha */
	red = gc->state.pixel.transferMode.bias.r;
	green = gc->state.pixel.transferMode.bias.g;
	blue = gc->state.pixel.transferMode.bias.b;
	alpha = gc->state.pixel.transferMode.scale.a +
	    gc->state.pixel.transferMode.bias.a;
	if (mapColor) {
	    pmap = 
		&gc->pixel.pixelMap[__GL_PIXEL_MAP_R_TO_R];
	    entry = (GLint)(red * (pmap->size-1) + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > pmap->size-1) entry = pmap->size-1;
	    red = pmap->base.mapF[entry];

	    pmap = 
		&gc->pixel.pixelMap[__GL_PIXEL_MAP_G_TO_G];
	    entry = (GLint)(green * (pmap->size-1) + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > pmap->size-1) entry = pmap->size-1;
	    green = pmap->base.mapF[entry];

	    pmap = 
		&gc->pixel.pixelMap[__GL_PIXEL_MAP_B_TO_B];
	    entry = (GLint)(blue * (pmap->size-1) + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > pmap->size-1) entry = pmap->size-1;
	    blue = pmap->base.mapF[entry];

	    pmap = 
		&gc->pixel.pixelMap[__GL_PIXEL_MAP_A_TO_A];
	    entry = (GLint)(alpha * (pmap->size-1) + __glHalf);
	    if (entry < 0) entry = 0;
	    else if (entry > pmap->size-1) entry = pmap->size-1;
	    alpha = pmap->base.mapF[entry];
	} else {
	    if (red > __glOne) red = __glOne;
	    else if (red < 0) red = 0;
	    if (green > __glOne) green = __glOne;
	    else if (green < 0) green = 0;
	    if (blue > __glOne) blue = __glOne;
	    else if (blue < 0) blue = 0;
	    if (alpha > __glOne) alpha = __glOne;
	    else if (alpha < 0) alpha = 0;
	}
	pm->red0Mod = red * gc->frontBuffer.redScale;
	pm->green0Mod = green * gc->frontBuffer.greenScale;
	pm->blue0Mod = blue * gc->frontBuffer.blueScale;
	pm->alpha1Mod = alpha * gc->frontBuffer.alphaScale;
    } else {
	pm->red0Mod = __glZero;
	pm->green0Mod = __glZero;
	pm->blue0Mod = __glZero;
	pm->alpha1Mod = gc->frontBuffer.alphaScale;
    }

    /*
    ** Color Tables
    */
    if ((gc->state.enables.pixelPath & __GL_COLOR_TABLE_ENABLE) &&
        (gc->pixel.colorTable[__GL_COLOR_TABLE_INDEX].width > 0)) {
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_COLOR_TABLE;
    }

    if ((gc->state.enables.pixelPath &
    		__GL_POST_CONVOLUTION_COLOR_TABLE_ENABLE) &&
        (gc->pixel.colorTable[__GL_POST_CONVOLUTION_COLOR_TABLE_INDEX].width > 0)) {
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_POST_CONVOLUTION_COLOR_TABLE;
    }

    if ((gc->state.enables.pixelPath &
    		__GL_POST_COLOR_MATRIX_COLOR_TABLE_ENABLE) &&
        (gc->pixel.colorTable[__GL_POST_COLOR_MATRIX_COLOR_TABLE_INDEX].width > 0)) {
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_POST_COLOR_MATRIX_COLOR_TABLE;
    }

    /*
    ** Convolution
    */
    gc->pixel.currentFilter1D = NULL;
    gc->pixel.adjustWidth1D = 0;
    if (gc->state.enables.pixelPath & __GL_CONVOLUTION_1D_ENABLE) {
        __GLconvolutionFilter *cf =
		&gc->pixel.convolutionFilter[__GL_CONVOLUTION_1D_INDEX];
	gc->pixel.currentFilter1D = cf;
	if ((cf->width > 0) && (cf->state.borderMode == GL_REDUCE)) {
	    gc->pixel.adjustWidth1D = -(cf->width-1);
	}
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_CONVOLUTION;
    }

    gc->pixel.currentFilter2D = NULL;
    gc->pixel.adjustWidth2D = 0;
    gc->pixel.adjustHeight2D = 0;
    if (gc->state.enables.pixelPath & __GL_CONVOLUTION_2D_ENABLE) {
        __GLconvolutionFilter *cf =
		&gc->pixel.convolutionFilter[__GL_CONVOLUTION_2D_INDEX];
	gc->pixel.currentFilter2D = cf;
	if ((cf->width > 0) && (cf->state.borderMode == GL_REDUCE)) {
	    gc->pixel.adjustWidth2D = -(cf->width-1);
	}
	if ((cf->height > 0) && (cf->state.borderMode == GL_REDUCE)) {
	    gc->pixel.adjustHeight2D = -(cf->height-1);
	}
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_CONVOLUTION;
    }
    else if (gc->state.enables.pixelPath & __GL_SEPARABLE_2D_ENABLE) {
        __GLconvolutionFilter *cf =
		&gc->pixel.convolutionFilter[__GL_SEPARABLE_2D_INDEX];
        gc->pixel.currentFilter2D = cf;
	if ((cf->width > 0) && (cf->state.borderMode == GL_REDUCE)) {
	    gc->pixel.adjustWidth2D = -(cf->width-1);
	}
	if ((cf->height > 0) && (cf->state.borderMode == GL_REDUCE)) {
	    gc->pixel.adjustHeight2D = -(cf->height-1);
	}
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_CONVOLUTION;
    }

    if (__glNeedScaleBias(gc,
		&gc->state.pixel.transferMode.postConvolutionScale,
		&gc->state.pixel.transferMode.postConvolutionBias)) {
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_POST_CONVOLUTION_SCALE_BIAS;
    }

    /*
    ** Color Matrix
    */
    if (gc->transform.color->matrix.matrixType != __GL_MT_IDENTITY) {
	pm->modifyRGBA = GL_TRUE;
        pixelModeFlags |= __GL_PIXEL_MODE_COLOR_MATRIX;
    }

    if (__glNeedScaleBias(gc,
		&gc->state.pixel.transferMode.postColorMatrixScale,
		&gc->state.pixel.transferMode.postColorMatrixBias)) {
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_MODE_POST_COLOR_MATRIX_SCALE_BIAS;
    }

    /*
    ** Histogram / Minmax
    */
    if (gc->state.enables.pixelPath & __GL_HISTOGRAM_ENABLE &&
    	gc->pixel.histogram.width > 0) {
	pm->modifyRGBA = GL_TRUE;
	pixelModeFlags |= __GL_PIXEL_HISTOGRAM;
    }

    if ((gc->state.enables.pixelPath & __GL_MINMAX_ENABLE)) {
	pm->modifyRGBA = GL_TRUE;
        pixelModeFlags |=  __GL_PIXEL_MINMAX;
    }

    gc->pixel.pixelModeFlags = pixelModeFlags;

    /*
    ** Fragment Operations
    */
    if ((enables & __GL_ALPHA_TEST_ENABLE) || 
	(enables & __GL_STENCIL_TEST_ENABLE) ||
	(enables & __GL_DEPTH_TEST_ENABLE) || 
	gc->state.raster.drawBuffer == GL_NONE ||
	gc->state.raster.drawBuffer == GL_FRONT_AND_BACK ||
	!(enables & __GL_DITHER_ENABLE) ||
	(enables & __GL_BLEND_ENABLE) ||
	gc->texture.textureEnabled ||
	(enables & __GL_FOG_ENABLE)) {
	pm->fastRGBA = GL_FALSE;
    } else {
	pm->fastRGBA = GL_TRUE;
    }

#if 0
    gc->procs.drawPixels = __glOptPickDrawPixels;
#else
    gc->procs.drawPixels = __glSlowPickDrawPixels;
#endif
    gc->procs.readPixels = __glSlowPickReadPixels;
    gc->procs.copyPixels = __glSlowPickCopyPixels;
}

void __glGenericPickTransformProcs(__GLcontext *gc)
{
    switch (gc->state.transform.matrixMode) {
      case GL_MODELVIEW:
	gc->procs.pushMatrix = __glPushModelViewMatrix;
	gc->procs.popMatrix = __glPopModelViewMatrix;
	gc->procs.loadIdentity = __glLoadIdentityModelViewMatrix;
	break;
      case GL_PROJECTION:
	gc->procs.pushMatrix = __glPushProjectionMatrix;
	gc->procs.popMatrix = __glPopProjectionMatrix;
	gc->procs.loadIdentity = __glLoadIdentityProjectionMatrix;
	break;
      case GL_TEXTURE:
	gc->procs.pushMatrix = __glPushTextureMatrix;
	gc->procs.popMatrix = __glPopTextureMatrix;
	gc->procs.loadIdentity = __glLoadIdentityTextureMatrix;
	break;
#if defined(__GL_SUPPORT_ARB_IMAGING_SUBSET)
      case GL_COLOR:
	gc->procs.pushMatrix = __glPushColorMatrix;
	gc->procs.popMatrix = __glPopColorMatrix;
	gc->procs.loadIdentity = __glLoadIdentityColorMatrix;
	break;
#endif
    }
}

/*
** pick the depth function pointers
*/
GLint __glGenericPickDepthProcs(__GLcontext *gc)
{
    extern void __glValidateZCount(__GLdepthBuffer *);
    GLint depthIndex;

    if (!(gc->polygon.shader.modeFlags & __GL_SHADE_DEPTH_TEST)) {
	return 0;
    }

    __glValidateZCount(&gc->depthBuffer);

    depthIndex = gc->depthBuffer.testFunc;

    depthIndex -= GL_NEVER;

    if ( gc->state.depth.writeEnable == GL_FALSE ) {
	depthIndex += 8;
    }

    if ( gc->depthBuffer.buf.drawableBuf->depth > 16 ) {
	depthIndex += 16;
    }

    if (gc->modes.haveDepthBuffer) {
	(*gc->depthBuffer.pick)(gc, &gc->depthBuffer, depthIndex);
    }

    gc->procs.DTPixel = __glCDTPixel[depthIndex];
#if defined(__GL_USE_ASMCODE) && defined(mips)
    gc->procs.span.depthTestPixel = __glSDepthTestPixel[depthIndex];
    gc->procs.line.depthTestPixel = __glPDepthTestPixel[depthIndex];
#endif /* __GL_USE_ASMCODE && mips */
    return depthIndex;
}


void __glGenericValidate(__GLcontext *gc)
{
    if ((*gc->procs.pickAllProcsPhase1)(gc)) {
	(*gc->procs.pickAllProcsPhase2)(gc);
    }

    gc->validateMask = 0;
    gc->dirtyMask = 0;

    /* copy the dispatch table, if needed */
    __glCopyDispatch(&__gl_dispatch, &__gl_dispatch);
}

GLboolean __glGenericPickAllProcsPhase1(__GLcontext *gc)
{
    GLuint enables = gc->state.enables.general;
    GLuint modeFlags = 0;

    /*
    ** Some dirty bits are limited in scope, so we get handle them as
    ** soon as possible
    */
    if (gc->dirtyMask & __GL_DIRTY_MODELVIEW) {
	__glValidateModelViewForLighting(gc);
	__glValidateCullVertexFromModelView(gc);
	__glValidateTextureModelView(gc);
	gc->dirtyMask &= ~__GL_DIRTY_MODELVIEW;
    }

    if (gc->dirtyMask & __GL_DIRTY_VERTARRAY) {
	__glGenericPickVertexArrayEnables(gc);
	gc->dirtyMask &= ~__GL_DIRTY_VERTARRAY;
    }

    /*
    ** If there is no work to be done, we are lucky!
    */
    if ((gc->dirtyMask == 0) && (gc->validateMask == 0)) {
	return GL_FALSE;	 /* FALSE: no further validation */
    }

    if (gc->dirtyMask & (__GL_DIRTY_GENERIC | __GL_DIRTY_LIGHTING)) {
	/* 
	** Set textureEnabled flag early on, so we can set modeFlags
	** based upon it.
	*/
	__glGenericPickTextureEnables(gc);
    }

    /* Compute shading mode flags before triangle, span, and line picker */
    if (gc->modes.rgbMode) {
	modeFlags |= __GL_SHADE_RGB;
	if (enables & __GL_COLOR_LOGIC_OP_ENABLE) {
	    modeFlags |= __GL_SHADE_LOGICOP;
	} else if (enables & __GL_BLEND_ENABLE) {
	    modeFlags |= __GL_SHADE_SRC_ALPHA | __GL_SHADE_BLEND;
	}
	if (enables & __GL_LIGHTING_ENABLE &&
	    gc->state.light.model.colorControl == GL_SEPARATE_SPECULAR_COLOR) {
	    modeFlags |= __GL_SHADE_SECONDARY_COLOR;
	}
	if (enables & __GL_ALPHA_TEST_ENABLE) {
	    modeFlags |= __GL_SHADE_SRC_ALPHA | __GL_SHADE_ALPHA_TEST;
	}
	if (gc->modes.alphaBits > 0) {
	    modeFlags |= __GL_SHADE_SRC_ALPHA;
	}
	if (!gc->state.raster.rMask ||
	    !gc->state.raster.gMask ||
	    !gc->state.raster.bMask ||
	    (!gc->state.raster.aMask && gc->modes.alphaBits)) {
	    modeFlags |= __GL_SHADE_MASK;
	}
    } else {
	if (enables & __GL_INDEX_LOGIC_OP_ENABLE) {
	    modeFlags |= __GL_SHADE_LOGICOP;
	}
	if (gc->state.raster.writeMask != __GL_MASK_INDEXI(gc, ~0)) {
	    modeFlags |= __GL_SHADE_MASK;
	}
    }
    if (gc->texture.textureEnabled) {
        GLint i;

	modeFlags |= __GL_SHADE_TEXTURE;

	for (i=0; i<__GL_NUM_TEXTURE_UNITS; ++i) {
	    __GLtexture *tex = gc->texture.unit[i].currentTexture;

	    /* XXX could track this modeFlag separately for each texture unit */
	    if (tex &&
	        tex->state.params.minFilter != tex->state.params.magFilter) {
		modeFlags |= __GL_SHADE_MIPMAP_TEXTURE;
	    }
	}

#if defined(__GL_UVSCALED_TEXCOORDS)
	if (!(modeFlags & __GL_SHADE_MIPMAP_TEXTURE)) {
	    modeFlags |= __GL_SHADE_UVSCALED_TEXTURE;
	}
#endif
	/*
	** XXX the triangle rasterization code assumes perspective
	** correction if rho interpolation is required.
	*/
	if ((gc->state.hints.perspectiveCorrection != GL_FASTEST) ||
	    (modeFlags & __GL_SHADE_MIPMAP_TEXTURE)) {
	    modeFlags |= __GL_SHADE_PERSP_TEXTURE;
#if defined(__GL_PROJSCALED_TEXCOORDS)
	    modeFlags |= __GL_SHADE_PROJSCALED_TEXTURE;
#endif
	}
    }
    if (gc->state.light.shadingModel == GL_SMOOTH) {
	modeFlags |= __GL_SHADE_SMOOTH | __GL_SHADE_SMOOTH_LIGHT;
    }
    if ((enables & __GL_DEPTH_TEST_ENABLE) && 
	gc->modes.haveDepthBuffer) {
            modeFlags |= ( __GL_SHADE_DEPTH_TEST |  __GL_SHADE_DEPTH_ITER );
	    if((enables & __GL_POLYGON_OFFSET_FILL_ENABLE) &&
	       (gc->state.polygon.factor != 0.0 ||
		gc->state.polygon.units != 0.0)) {
	           modeFlags |= __GL_SHADE_POLYGON_OFFSET_FILL;
	    }
    }
    if (enables & __GL_CULL_FACE_ENABLE) {
	modeFlags |= __GL_SHADE_CULL_FACE;
    }
    /* Only turn on dither for 8- and 16-bit framebuffers */
    if (enables & __GL_DITHER_ENABLE && gc->front->buf.drawableBuf->elementSize < 3) {
	modeFlags |= __GL_SHADE_DITHER;
    }
    if (enables & __GL_POLYGON_STIPPLE_ENABLE) {
	modeFlags |= __GL_SHADE_STIPPLE;
    }
    if (enables & __GL_LINE_STIPPLE_ENABLE) {
	modeFlags |= __GL_SHADE_LINE_STIPPLE;
    }
    if ((enables & __GL_STENCIL_TEST_ENABLE) && 
	    gc->modes.haveStencilBuffer) {
	modeFlags |= __GL_SHADE_STENCIL_TEST;
    }
    if ((enables & __GL_LIGHTING_ENABLE) && 
	    gc->state.light.model.twoSided) {
	modeFlags |= __GL_SHADE_TWOSIDED;
    }

    if (enables & __GL_FOG_ENABLE) {
	/* Figure out type of fogging to do.  Try to do cheap fog */
	if (!(modeFlags & __GL_SHADE_TEXTURE) &&
		(gc->state.hints.fog != GL_NICEST)) {
	    /*
	    ** Cheap fog can be done.  Now figure out which kind we
	    ** will do.  If smooth shading, its easy - just change
	    ** the calcColor proc (let the color proc picker do it).
	    ** Otherwise, set has flag later on to use smooth shading
	    ** to do flat shaded fogging.
	    */
	    modeFlags |= __GL_SHADE_CHEAP_FOG | __GL_SHADE_SMOOTH;
	} else {
	    /* Use slowest fog mode */
	    modeFlags |= __GL_SHADE_SLOW_FOG;
	}
    }

    if ((gc->state.raster.drawBuffer == GL_FRONT ||
	 gc->state.raster.drawBuffer == GL_FRONT_AND_BACK)) {
	modeFlags |= __GL_SHADE_OWNERSHIP_TEST;
    }

    gc->polygon.shader.modeFlags = modeFlags;

    /*
    ** Initialize buffer procs.  Needed to be done before phase 2.
    */
    (*gc->procs.pickBufferProcs)(gc);

    return GL_TRUE;
}

void __glGenericPickAllProcsPhase2(__GLcontext *gc)
{
    GLuint enables = gc->state.enables.general;
    GLuint modeFlags = gc->polygon.shader.modeFlags;

    if (gc->dirtyMask & (__GL_DIRTY_GENERIC | __GL_DIRTY_LIGHTING)) {
	GLuint needs;
	GLuint faceNeeds;

	/* Compute needs mask */
	faceNeeds = needs = 0;
	if (gc->texture.textureEnabled) {
	    needs |= __GL_HAS_TEXTURE;
	    if ((gc->state.enables.texture[0] & __GL_TEXTURE_GEN_S_ENABLE)) {
		switch (gc->state.texture.unit[0].s.mode) {
		  case GL_EYE_LINEAR:
		    needs |= __GL_HAS_EYE;
		    break;
		  case GL_SPHERE_MAP:
		    needs |= __GL_HAS_EYE | __GL_HAS_NORMAL;
		    break;
		}
	    }
	    if ((gc->state.enables.texture[0] & __GL_TEXTURE_GEN_T_ENABLE)) {
		switch (gc->state.texture.unit[0].t.mode) {
		  case GL_EYE_LINEAR:
		    needs |= __GL_HAS_EYE;
		    break;
		  case GL_SPHERE_MAP:
		    needs |= __GL_HAS_EYE | __GL_HAS_NORMAL;
		    break;
		}
	    }
	}
	if (enables & __GL_LIGHTING_ENABLE) {
	    faceNeeds |= __GL_HAS_NORMAL;
	    if (gc->state.light.model.localViewer) {
		faceNeeds |= __GL_HAS_EYE;
	    } else {
		GLint i;
		__GLlightSourceState *lss = &gc->state.light.source[0];

		for (i = 0; i < gc->constants.numberOfLights; i++, lss++)
		    if ((gc->state.enables.lights & (1<<i)) && 
			(lss->positionEye.w != __glZero))
		    {
			/* local light source enabled */
			faceNeeds |= __GL_HAS_EYE;
			break;
		    }
	    }
	}
	if (enables & __GL_FOG_ENABLE) {
	    /* Need z in eye coordinates for fog */
	    needs |= __GL_HAS_EYE;

	    /* Need fog value at vertex if not nicest */
	    if (gc->state.hints.fog != GL_NICEST)
		needs |= __GL_HAS_FOG;
	}
	if (gc->state.enables.clipPlanes) {
	    /* Clip with user planes in eye space! */
	    needs |= __GL_HAS_EYE;
	}
	gc->vertex.needs = needs;
	gc->vertex.faceNeeds[__GL_FRONTFACE] = faceNeeds | needs;
	gc->vertex.faceNeeds[__GL_BACKFACE] = faceNeeds | needs;
	if ((enables & __GL_LIGHTING_ENABLE) || 
		(modeFlags & 
		(__GL_SHADE_CHEAP_FOG | __GL_SHADE_SMOOTH_LIGHT))) {
	    gc->vertex.faceNeeds[__GL_FRONTFACE] |= __GL_HAS_FRONT_COLOR;
	    if (gc->state.light.model.twoSided) {
		gc->vertex.faceNeeds[__GL_BACKFACE] |= __GL_HAS_BACK_COLOR;
	    } else {
		gc->vertex.faceNeeds[__GL_BACKFACE] = 
			gc->vertex.faceNeeds[__GL_FRONTFACE];
	    }
	} 
	if ((gc->state.light.shadingModel == GL_SMOOTH) ||
	    gc->vertexCache.vertexCacheEnabled) {
	    gc->vertex.materialNeeds = 
		    (gc->vertex.faceNeeds[__GL_FRONTFACE] | 
		     gc->vertex.faceNeeds[__GL_BACKFACE]) & ~__GL_HAS_TEXTURE;
	} else {
	    /* Need nothing if only the provoking vertex needs to be lit! */
	    gc->vertex.materialNeeds = 0;
	}

#if defined(__GL_CODEGEN) && defined(i386)
	__glGeneratePickVertexShape(gc);
#endif /* __GL_CODEGEN && i386 */

	(*gc->front->pick)(gc, gc->front);
	if (gc->modes.doubleBufferMode) {
	    (*gc->back->pick)(gc, gc->back);
	}
#if __GL_NUMBER_OF_AUX_BUFFERS > 0
	{
	    GLint i;
	    for (i = 0; i < gc->modes.numAuxBuffers; i++) {
		(*gc->auxBuffer[i].pick)(gc, &gc->auxBuffer[i]);
	    }
	}
#endif /* __GL_NUMBER_OF_AUX_BUFFERS > 0 */
	if (gc->modes.haveStencilBuffer) {
	    (*gc->stencilBuffer.pick)(gc, &gc->stencilBuffer);
	}

	/* 
	** Note: Must call gc->front->pick and gc->back->pick before calling
	** pickStoreProcs.  This also must be called prior to line, point, 
	** polygon, clipping, or bitmap pickers.  The LIGHT implementation
	** depends upon it.
	*/
	(*gc->procs.pickStoreProcs)(gc);

	(*gc->procs.pickTransformProcs)(gc);

	__glValidateLighting(gc);

	__glValidateCullVertexFromModelView(gc);

	/*
	** Note: pickColorMaterialProcs is called frequently outside of this
	** generic picking routine.
	*/
	(*gc->procs.pickColorMaterialProcs)(gc);

	(*gc->procs.pickTextureProcs)(gc);
	(*gc->procs.pickCalcTextureProcs)(gc);

	(*gc->procs.pickBlendProcs)(gc);
	(*gc->procs.pickFogProcs)(gc);

	(*gc->procs.pickParameterClipProcs)(gc);
	(*gc->procs.pickClipProcs)(gc);

	/*
	** Needs to be done after pickStoreProcs.
	*/
	(*gc->procs.pickRenderBitmapProcs)(gc);

	if (gc->validateMask & __GL_VALIDATE_ALPHA_FUNC) {
	    __glValidateAlphaTest(gc);
	}

	(*gc->procs.computeClipBox)(gc);

    }

    if (gc->dirtyMask & __GL_DIRTY_POLYGON_STIPPLE) {
	/*
	** Usually, the polygon stipple is converted immediately after
	** it is changed.  However, if the polygon stipple was changed
	** when this context was the destination of a CopyContext, then
	** the polygon stipple will be converted here.
	*/
	(*gc->procs.convertPolygonStipple)(gc);
    }

    if (gc->dirtyMask & __GL_DIRTY_DEPTH) {
	(*gc->procs.pickDepthProcs)(gc);
    }

    if (gc->dirtyMask & (__GL_DIRTY_GENERIC | __GL_DIRTY_POLYGON | 
	    __GL_DIRTY_LIGHTING | __GL_DIRTY_DEPTH)) {
	/* 
	** May be used for picking Rect() procs, need to check polygon 
	** bit.  Must also be called after gc->vertex.needs is set.
	** Needs to be called prior to point, line, and triangle pickers.
	** Also needs to be called after the store procs picker is called.
	*/
	(*gc->procs.pickVertexProcs)(gc);

	(*gc->procs.pickSpanProcs)(gc);
	(*gc->procs.pickTriangleProcs)(gc);
    }

    if (gc->dirtyMask & (__GL_DIRTY_GENERIC | __GL_DIRTY_POINT |
	    __GL_DIRTY_LIGHTING)) {
	(*gc->procs.pickPointProcs)(gc);
    }

    if (gc->dirtyMask & (__GL_DIRTY_GENERIC | __GL_DIRTY_LINE |
	    __GL_DIRTY_LIGHTING)) {
	(*gc->procs.pickLineProcs)(gc);
    }

    if (gc->dirtyMask & (__GL_DIRTY_GENERIC | __GL_DIRTY_PIXEL | 
	    __GL_DIRTY_LIGHTING)) {
	(*gc->procs.pickPixelProcs)(gc);
    }

    if (gc->dirtyMask & (__GL_DIRTY_GENERIC | __GL_DIRTY_LIGHTING)) {
	(*gc->procs.pickVertexArrayProcs)(gc);
    }
}
