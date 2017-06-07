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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_fog.c#7 $
*/
#include "context.h"
#include "global.h"
#include "g_imports.h"

GLboolean __glFogSpan(__GLcontext *gc)
{
    __GLcolor *cp, *fogColor;
    __GLfloat f, oneMinusFog, fog;
    GLint w;

    w = gc->polygon.shader.length;

    f = gc->polygon.shader.frag.f;
    cp = gc->polygon.shader.colors;
    fogColor = &gc->state.fog.bufColor;
    while (--w >= 0) {
	/* clamp fog value */
	fog = f;
	if (fog < __glZero) fog = __glZero;
	else if (fog > __glOne) fog = __glOne;
	oneMinusFog = __glOne - fog;

	/* Blend incoming color against the fog color */
	if (gc->modes.colorIndexMode) {
	    cp->r = cp->r + oneMinusFog * gc->state.fog.index;
	} else {
	    cp->r = fog * cp->r + oneMinusFog * fogColor->r;
	    cp->g = fog * cp->g + oneMinusFog * fogColor->g;
	    cp->b = fog * cp->b + oneMinusFog * fogColor->b;
	}

	f += gc->polygon.shader.dfdx;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glFogStippledSpan(__GLcontext *gc)
{
    __GLstippleWord bit, inMask, *sp;
    __GLcolor *cp, *fogColor;
    __GLfloat f, oneMinusFog, fog;
    GLint count;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    f = gc->polygon.shader.frag.f;
    cp = gc->polygon.shader.colors;
    fogColor = &gc->state.fog.bufColor;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		/* clamp fog value */
		fog = f;
		if (fog < __glZero) fog = __glZero;
		else if (fog > __glOne) fog = __glOne;
		oneMinusFog = __glOne - fog;

		/* Blend incoming color against the fog color */
		if (gc->modes.colorIndexMode) {
		    cp->r = cp->r + oneMinusFog * gc->state.fog.index;
		} else {
		    cp->r = fog * cp->r + oneMinusFog * fogColor->r;
		    cp->g = fog * cp->g + oneMinusFog * fogColor->g;
		    cp->b = fog * cp->b + oneMinusFog * fogColor->b;
		}
	    }
	    f += gc->polygon.shader.dfdx;
	    cp++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/************************************************************************/

GLboolean __glFogSpanSlow(__GLcontext *gc)
{
    __GLcolor *cp, *fogColor;
    __GLfloat f, oneMinusFog, fog = 0.0f, eyeZ;
    __GLfloat density, density2, end;
    GLint w;

    w = gc->polygon.shader.length;

    f = gc->polygon.shader.frag.f;
    cp = gc->polygon.shader.colors;
    fogColor = &gc->state.fog.bufColor;
    density = gc->state.fog.density;
    density2 = density * density;
    end = gc->state.fog.end;
    while (--w >= 0) {
	/* Compute fog value */
	eyeZ = f;
	if (eyeZ < __glZero) eyeZ = -eyeZ;
	switch (gc->state.fog.mode) {
	  case GL_EXP:
	    fog = __GL_POWF(__glE, -density * eyeZ);
	    break;
	  case GL_EXP2:
	    fog = __GL_POWF(__glE, -(density2 * eyeZ * eyeZ));
	    break;
	  case GL_LINEAR:
	    fog = (end - eyeZ) * gc->state.fog.oneOverEMinusS;
	    break;
	}

	/* clamp fog value */
	if (fog < __glZero) fog = __glZero;
	else if (fog > __glOne) fog = __glOne;
	oneMinusFog = __glOne - fog;

	/* Blend incoming color against the fog color */
	if (gc->modes.colorIndexMode) {
	    cp->r = cp->r + oneMinusFog * gc->state.fog.index;
	} else {
	    cp->r = fog * cp->r + oneMinusFog * fogColor->r;
	    cp->g = fog * cp->g + oneMinusFog * fogColor->g;
	    cp->b = fog * cp->b + oneMinusFog * fogColor->b;
	}

	f += gc->polygon.shader.dfdx;
	cp++;
    }

    return GL_FALSE;
}

GLboolean __glFogStippledSpanSlow(__GLcontext *gc)
{
    __GLstippleWord bit, inMask, *sp;
    __GLcolor *cp, *fogColor;
    __GLfloat f, oneMinusFog, fog = 0.0f, eyeZ;
    __GLfloat density, density2, end;
    GLint count;
    GLint w;

    w = gc->polygon.shader.length;
    sp = gc->polygon.shader.stipplePat;

    f = gc->polygon.shader.frag.f;
    cp = gc->polygon.shader.colors;
    fogColor = &gc->state.fog.bufColor;
    density = gc->state.fog.density;
    density2 = density * density;
    end = gc->state.fog.end;
    while (w) {
	count = w;
	if (count > __GL_STIPPLE_BITS) {
	    count = __GL_STIPPLE_BITS;
	}
	w -= count;

	inMask = *sp++;
	bit = __GL_STIPPLE_SHIFT(0);
	while (--count >= 0) {
	    if (inMask & bit) {
		/* Compute fog value */
		eyeZ = f;
		if (eyeZ < __glZero) eyeZ = -eyeZ;
		switch (gc->state.fog.mode) {
		  case GL_EXP:
		    fog = __GL_POWF(__glE, -density * eyeZ);
		    break;
		  case GL_EXP2:
		    fog = __GL_POWF(__glE, -(density2 * eyeZ * eyeZ));
		    break;
		  case GL_LINEAR:
		    fog = (end - eyeZ) * gc->state.fog.oneOverEMinusS;
		    break;
		}

		/* Clamp resulting fog value */
		if (fog < __glZero) fog = __glZero;
		else if (fog > __glOne) fog = __glOne;
		oneMinusFog = __glOne - fog;

		/* Blend incoming color against the fog color */
		if (gc->modes.colorIndexMode) {
		    cp->r = cp->r + oneMinusFog * gc->state.fog.index;
		} else {
		    cp->r = fog * cp->r + oneMinusFog * fogColor->r;
		    cp->g = fog * cp->g + oneMinusFog * fogColor->g;
		    cp->b = fog * cp->b + oneMinusFog * fogColor->b;
		}
	    }
	    f += gc->polygon.shader.dfdx;
	    cp++;
#ifdef __GL_STIPPLE_MSB
	    bit >>= 1;
#else
	    bit <<= 1;
#endif
	}
    }

    return GL_FALSE;
}

/************************************************************************/

/*
** Compute the fog value given an eyeZ.  Then blend into fragment.
** This is used when fragment fogging is done (GL_FOG_HINT == GL_NICEST)
** or by the point rendering routines.
** NOTE: the code below has the -eyeZ factored out.
*/
void __glFogFragmentSlow(__GLcontext *gc, __GLfragment *frag, __GLfloat eyeZ)
{
    __GLfloat fog = 0.0f, oneMinusFog, density, end;
    __GLcolor *fogColor;

    if (eyeZ < __glZero) eyeZ = -eyeZ;

    switch (gc->state.fog.mode) {
      case GL_EXP:
	density = gc->state.fog.density;
	fog = __GL_POWF(__glE, -density * eyeZ);
	break;
      case GL_EXP2:
	density = gc->state.fog.density;
	fog = __GL_POWF(__glE, -(density * eyeZ * density * eyeZ));
	break;
      case GL_LINEAR:
	end = gc->state.fog.end;
	fog = (end - eyeZ) * gc->state.fog.oneOverEMinusS;
	break;
    }

    /*
    ** clamp fog value
    */
    if (fog < __glZero)
	fog = __glZero;
    else
    if (fog > __glOne)
	fog = __glOne;
    oneMinusFog = __glOne - fog;

    /*
    ** Blend incoming color against the fog color
    */
    fogColor = &gc->state.fog.bufColor;
    if (gc->modes.colorIndexMode) {
	frag->color[__GL_PRIMARY_COLOR].r += oneMinusFog * gc->state.fog.index;
    } else {
	frag->color[__GL_PRIMARY_COLOR].r = fog *
	    frag->color[__GL_PRIMARY_COLOR].r + oneMinusFog * fogColor->r;
	frag->color[__GL_PRIMARY_COLOR].g = fog *
	    frag->color[__GL_PRIMARY_COLOR].g + oneMinusFog * fogColor->g;
	frag->color[__GL_PRIMARY_COLOR].b = fog *
	    frag->color[__GL_PRIMARY_COLOR].b + oneMinusFog * fogColor->b;
    }
}


/*
** Compute generic fog value for vertex.
*/
__GLfloat __glFogVertex(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat eyeZ, fog = 0.0f, density, end;

    eyeZ = vx->eye.z;
    if (eyeZ < __glZero) eyeZ = -eyeZ;

    switch (gc->state.fog.mode) {
      case GL_EXP:
	density = gc->state.fog.density;
	fog = __GL_POWF(__glE, -density * eyeZ);
	break;
      case GL_EXP2:
	density = gc->state.fog.density;
	fog = __GL_POWF(__glE, -(density * eyeZ * density * eyeZ));
	break;
      case GL_LINEAR:
	end = gc->state.fog.end;
	fog = (end - eyeZ) * gc->state.fog.oneOverEMinusS;
	break;
    }

    /*
    ** Since this routine is called when we are doing slow fog, we can
    ** safely clamp the fog value here. 
    */
    if (fog < __glZero)
	fog = __glZero;
    else if (fog > __glOne)
	fog = __glOne;
    
    return fog;
}

/*
** Compute linear fog value for vertex
*/
__GLfloat __glFogVertexLinear(__GLcontext *gc, __GLvertex *vx)
{
    __GLfloat eyeZ, fog, end;

    eyeZ = vx->eye.z;
    if (eyeZ < __glZero) eyeZ = -eyeZ;

    end = gc->state.fog.end;
    fog = (end - eyeZ) * gc->state.fog.oneOverEMinusS;

    if (fog < __glZero)
	fog = __glZero;
    else if (fog > __glOne)
	fog = __glOne;
    
    return fog;
}


/*
** Compute the fogged color given an incoming color and a fog value.
*/
void __glFogColorSlow(__GLcontext *gc, __GLcolor *out, __GLcolor *in, 
		      __GLfloat fog)
{
    __GLcolor *fogColor;
    __GLfloat oneMinusFog;
    __GLfloat r, g, b;

    oneMinusFog = __glOne - fog;

    /*
    ** Blend incoming color against the fog color
    */
    fogColor = &gc->state.fog.color;
    if (gc->modes.colorIndexMode) {
	out->r = in->r + oneMinusFog * gc->state.fog.index;
    } else {
	/*
	** The following is coded like this to give the instruction scheduler
	** a hand.
	*/
	r = fog * in->r;
	g = fog * in->g;
	b = fog * in->b;
	r += oneMinusFog * fogColor->r;
	g += oneMinusFog * fogColor->g;
	b += oneMinusFog * fogColor->b;
	out->r = r;
	out->g = g;
	out->b = b;
	out->a = in->a;
    }
}
