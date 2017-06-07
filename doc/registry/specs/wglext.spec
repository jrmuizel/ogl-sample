# wglext.spec file
# DON'T REMOVE PREVIOUS LINE!!! libspec depends on it!
#
# License Applicability. Except to the extent portions of this file are
# made subject to an alternative license as permitted in the SGI Free
# Software License B, Version 1.1 (the "License"), the contents of this
# file are subject only to the provisions of the License. You may not use
# this file except in compliance with the License. You may obtain a copy
# of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
# Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
#
# http://oss.sgi.com/projects/FreeB
#
# Note that, as provided in the License, the Software is distributed on an
# "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
# DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
# CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# Original Code. The Original Code is: OpenGL Sample Implementation,
# Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
# Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
# Copyright in any portions created by third parties is as indicated
# elsewhere herein. All Rights Reserved.
#
# Additional Notice Provisions: This software was created using the
# OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
# not been independently verified as being compliant with the OpenGL(R)
# version 1.2.1 Specification.

required-props:
param:		retval retained
category:	wgl ARB_buffer_region ARB_buffer_region ARB_buffer_region ARB_buffer_region ARB_extensions_string ARB_pixel_format ARB_pixel_format ARB_pixel_format ARB_make_current_read ARB_make_current_read ARB_pbuffer ARB_pbuffer ARB_pbuffer ARB_pbuffer ARB_pbuffer EXT_display_color_table EXT_display_color_table EXT_display_color_table EXT_display_color_table EXT_extensions_string EXT_make_current_read EXT_make_current_read EXT_pbuffer EXT_pbuffer EXT_pbuffer EXT_pbuffer EXT_pbuffer EXT_pixel_format EXT_pixel_format EXT_pixel_format EXT_swap_control EXT_swap_control

# required-props in wgl.spec (which is not used for anything):
# dlflags:	  notlistable handcode
# wglflags:	  client-handcode server-handcode non-dispatch

#
# Boilerplate to define types used by some extensions. This is done
#   up front, since it involves some complexities in protecting
#   the declarations whether or not the -protect flag is given to
#   the generator scripts.
#

passthru: #ifndef WGL_ARB_pbuffer
passthru: DECLARE_HANDLE(HPBUFFERARB);
passthru: #endif
passthru: #ifndef WGL_EXT_pbuffer
passthru: DECLARE_HANDLE(HPBUFFEREXT);
passthru: #endif
passthru:

###############################################################################
#
# ARB extension #4
# WGL_ARB_buffer_region commands
#
###############################################################################

CreateBufferRegionARB(hDC, iLayerPlane, uType)
	return		HANDLE
	param		hDC		HDC in value
	param		iLayerPlane	int in value
	param		uType		UINT in value
	category	ARB_buffer_region

DeleteBufferRegionARB(hRegion)
	return		VOID
	param		hRegion		HANDLE in value
	category	ARB_buffer_region

SaveBufferRegionARB(hRegion, x, y, width, height)
	return		BOOL
	param		hRegion		HANDLE in value
	param		x		int in value
	param		y		int in value
	param		width		int in value
	param		height		int in value
	category	ARB_buffer_region

RestoreBufferRegionARB(hRegion, x, y, width, height, xSrc, ySrc)
	return		BOOL
	param		hRegion		HANDLE in value
	param		x		int in value
	param		y		int in value
	param		width		int in value
	param		height		int in value
	param		xSrc		int in value
	param		ySrc		int in value
	category	ARB_buffer_region

###############################################################################
#
# ARB extension #8
# WGL_ARB_extensions_string commands
#
###############################################################################

GetExtensionsStringARB(hdc)
	return		String
	param		hdc		HDC in value
	category	ARB_extensions_string

###############################################################################
#
# ARB extension #9
# WGL_ARB_pixel_format commands
#
###############################################################################

GetPixelFormatAttribivARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues)
	return		BOOL
	param		hdc		HDC in value
	param		iPixelFormat	int in value
	param		iLayerPlane	int in value
	param		nAttributes	UINT in value
	param		piAttributes	int in array [nAttributes]
	param		piValues	int out array [nAttributes]
	category	ARB_pixel_format

GetPixelFormatAttribfvARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues)
	return		BOOL
	param		hdc		HDC in value
	param		iPixelFormat	int in value
	param		iLayerPlane	int in value
	param		nAttributes	UINT in value
	param		piAttributes	int in array [nAttributes]
	param		pfValues	FLOAT out array [nAttributes]
	category	ARB_pixel_format

ChoosePixelFormatARB(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats)
	return		BOOL
	param		hdc		HDC in value
	param		piAttribIList	int in array [COMPSIZE()]
	param		pfAttribFList	FLOAT in array [COMPSIZE()]
	param		nMaxFormats	UINT in value
	param		piFormats	int out array [COMPSIZE(nNumFormats)]
	param		nNumFormats	UINT out reference
	category	ARB_pixel_format

###############################################################################
#
# ARB extension #10
# WGL_ARB_make_current_read commands
#
###############################################################################

MakeContextCurrentARB(hDrawDC, hReadDC, hglrc)
	return		BOOL
	param		hDrawDC		HDC in value
	param		hReadDC		HDC in value
	param		hglrc		HGLRC in value
	category	ARB_make_current_read

GetCurrentReadDCARB()
	return		HDC
	category	ARB_make_current_read

###############################################################################
#
# ARB extension #11
# WGL_ARB_pbuffer commands
#
###############################################################################

CreatePbufferARB(hDC, iPixelFormat, iWidth, iHeight, piAttribList)
	return		HPBUFFERARB
	param		hDC		HDC in value
	param		iPixelFormat	int in value
	param		iWidth		int in value
	param		iHeight		int in value
	param		piAttribList	int in array [COMPSIZE()]
	category	ARB_pbuffer

GetPbufferDCARB(hPbuffer)
	return		HDC
	param		hPbuffer	HPBUFFERARB in value
	category	ARB_pbuffer

ReleasePbufferDCARB(hPbuffer, hDC)
	return		int
	param		hPbuffer	HPBUFFERARB in value
	param		hDC		HDC in value
	category	ARB_pbuffer

DestroyPbufferARB(hPbuffer)
	return		BOOL
	param		hPbuffer	HPBUFFERARB in value
	category	ARB_pbuffer

QueryPbufferARB(hPbuffer, iAttribute, piValue)
	return		BOOL
	param		hPbuffer	HPBUFFERARB in value
	param		iAttribute	int in value
	param		piValue		int out reference
	category	ARB_pbuffer

###############################################################################
#
# Extension #167
# WGL_EXT_display_color_table commands
#
###############################################################################

CreateDisplayColorTableEXT(id)
	return		GLboolean
	param		id		GLushort in value
	category	EXT_display_color_table

LoadDisplayColorTableEXT(table, length)
	return		GLboolean
	param		table		GLushort in array [length]
	param		length		GLuint in value
	category	EXT_display_color_table

BindDisplayColorTableEXT(id)
	return		GLboolean
	param		id		GLushort in value
	category	EXT_display_color_table

DestroyDisplayColorTableEXT(id)
	return		VOID
	param		id		GLushort in value
	category	EXT_display_color_table

###############################################################################
#
# Extension #168
# WGL_EXT_extensions_string commands
#
###############################################################################

GetExtensionsStringEXT()
	return		String
	category	EXT_extensions_string

###############################################################################
#
# Extension #169
# WGL_EXT_make_current_read commands
#
###############################################################################

MakeContextCurrentEXT(hDrawDC, hReadDC, hglrc)
	return		BOOL
	param		hDrawDC		HDC in value
	param		hReadDC		HDC in value
	param		hglrc		HGLRC in value
	category	EXT_make_current_read

GetCurrentReadDCEXT()
	return		HDC
	category	EXT_make_current_read

###############################################################################
#
# Extension #171
# WGL_EXT_pbuffer commands
#
###############################################################################

CreatePbufferEXT(hDC, iPixelFormat, iWidth, iHeight, piAttribList)
	return		HPBUFFEREXT
	param		hDC		HDC in value
	param		iPixelFormat	int in value
	param		iWidth		int in value
	param		iHeight		int in value
	param		piAttribList	int in array [COMPSIZE()]
	category	EXT_pbuffer

GetPbufferDCEXT(hPbuffer)
	return		HDC
	param		hPbuffer	HPBUFFEREXT in value
	category	EXT_pbuffer

ReleasePbufferDCEXT(hPbuffer, hDC)
	return		int
	param		hPbuffer	HPBUFFEREXT in value
	param		hDC		HDC in value
	category	EXT_pbuffer

DestroyPbufferEXT(hPbuffer)
	return		BOOL
	param		hPbuffer	HPBUFFEREXT in value
	category	EXT_pbuffer

QueryPbufferEXT(hPbuffer, iAttribute, piValue)
	return		BOOL
	param		hPbuffer	HPBUFFEREXT in value
	param		iAttribute	int in value
	param		piValue		int out reference
	category	EXT_pbuffer

###############################################################################
#
# Extension #170
# WGL_EXT_pixel_format commands
#
###############################################################################

GetPixelFormatAttribivEXT(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues)
	return		BOOL
	param		hdc		HDC in value
	param		iPixelFormat	int in value
	param		iLayerPlane	int in value
	param		nAttributes	UINT in value
	param		piAttributes	int out array [nAttributes]
	param		piValues	int out array [nAttributes]
	category	EXT_pixel_format

GetPixelFormatAttribfvEXT(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues)
	return		BOOL
	param		hdc		HDC in value
	param		iPixelFormat	int in value
	param		iLayerPlane	int in value
	param		nAttributes	UINT in value
	param		piAttributes	int out array [nAttributes]
	param		pfValues	FLOAT out array [nAttributes]
	category	EXT_pixel_format

ChoosePixelFormatEXT(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats)
	return		BOOL
	param		hdc		HDC in value
	param		piAttribIList	int in array [COMPSIZE()]
	param		pfAttribFList	FLOAT in array [COMPSIZE()]
	param		nMaxFormats	UINT in value
	param		piFormats	int out array [COMPSIZE(nNumFormats)]
	param		nNumFormats	UINT out reference
	category	EXT_pixel_format

###############################################################################
#
# Extension #172
# WGL_EXT_swap_control commands
#
###############################################################################

SwapIntervalEXT(interval)
	return		BOOL
	param		interval	int in value
	category	EXT_swap_control

GetSwapIntervalEXT()
	return		int
	category	EXT_swap_control

###############################################################################
#
# Extension #177
# WGL_EXT_depth_float commands
#
###############################################################################

# (none)
newcategory: WGL_EXT_depth_float

###############################################################################
#
# Extension #207
# WGL_3DFX_multisample commands
#
###############################################################################

# (none)
newcategory: WGL_3DFX_multisample

###############################################################################
#
# Extension #209
# WGL_EXT_multisample commands
#
###############################################################################

# (none)
newcategory: WGL_EXT_multisample

