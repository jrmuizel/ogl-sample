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
#
# $Date: 2000/04/26 02:10:53 $ $Revision: 1.1 $
# $Header: /home/pub/cvs/projects/ogl-sample/main/doc/registry/specs/wglenum.spec,v 1.1 2000/04/26 02:10:53 ljp Exp $

# This is a mirror of the WGL enumerant registry.
#
# It is an extremely important file. Do not mess with it unless
# you know what you're doing and have permission to do so.
#
# Rules for modification are similar to the rules for the OpenGL
# enumerant registry (gl.spec). Basically, don't modify this
# file unless you're SGI's ARB Representative. In addition, note
# that the true master registry is an Excel spreadsheet maintained
# by Steve Wright, swright@microsoft.com - this is just a mirror
# of that registry in ASCII form for use with generator tools
# and as an easy reference.
#
# Finally, note that this registry does not currently include
# any core WGL enumerants - only enumerants defined by WGL
# extensions.

###############################################################################

# Bitmasks - not consumed by the registry
WGL_ARB_buffer_region enum:
	WGL_FRONT_COLOR_BUFFER_BIT_ARB			= 0x00000001
	WGL_BACK_COLOR_BUFFER_BIT_ARB			= 0x00000002
	WGL_DEPTH_BUFFER_BIT_ARB			= 0x00000004
	WGL_STENCIL_BUFFER_BIT_ARB			= 0x00000008

###############################################################################

# SGI: 0x2000-0x203F

WGL_EXT_pixel_format enum:
	WGL_NUMBER_PIXEL_FORMATS_EXT			= 0x2000
	WGL_DRAW_TO_WINDOW_EXT				= 0x2001
	WGL_DRAW_TO_BITMAP_EXT				= 0x2002
	WGL_ACCELERATION_EXT				= 0x2003
	WGL_NEED_PALETTE_EXT				= 0x2004
	WGL_NEED_SYSTEM_PALETTE_EXT			= 0x2005
	WGL_SWAP_LAYER_BUFFERS_EXT			= 0x2006
	WGL_SWAP_METHOD_EXT				= 0x2007
	WGL_NUMBER_OVERLAYS_EXT				= 0x2008
	WGL_NUMBER_UNDERLAYS_EXT			= 0x2009
	WGL_TRANSPARENT_EXT				= 0x200A
	WGL_TRANSPARENT_VALUE_EXT			= 0x200B
	WGL_SHARE_DEPTH_EXT				= 0x200C
	WGL_SHARE_STENCIL_EXT				= 0x200D
	WGL_SHARE_ACCUM_EXT				= 0x200E
	WGL_SUPPORT_GDI_EXT				= 0x200F
	WGL_SUPPORT_OPENGL_EXT				= 0x2010
	WGL_DOUBLE_BUFFER_EXT				= 0x2011
	WGL_STEREO_EXT					= 0x2012
	WGL_PIXEL_TYPE_EXT				= 0x2013
	WGL_COLOR_BITS_EXT				= 0x2014
	WGL_RED_BITS_EXT				= 0x2015
	WGL_RED_SHIFT_EXT				= 0x2016
	WGL_GREEN_BITS_EXT				= 0x2017
	WGL_GREEN_SHIFT_EXT				= 0x2018
	WGL_BLUE_BITS_EXT				= 0x2019
	WGL_BLUE_SHIFT_EXT				= 0x201A
	WGL_ALPHA_BITS_EXT				= 0x201B
	WGL_ALPHA_SHIFT_EXT				= 0x201C
	WGL_ACCUM_BITS_EXT				= 0x201D
	WGL_ACCUM_RED_BITS_EXT				= 0x201E
	WGL_ACCUM_GREEN_BITS_EXT			= 0x201F
	WGL_ACCUM_BLUE_BITS_EXT				= 0x2020
	WGL_ACCUM_ALPHA_BITS_EXT			= 0x2021
	WGL_DEPTH_BITS_EXT				= 0x2022
	WGL_STENCIL_BITS_EXT				= 0x2023
	WGL_AUX_BUFFERS_EXT				= 0x2024
	WGL_NO_ACCELERATION_EXT				= 0x2025
	WGL_GENERIC_ACCELERATION_EXT			= 0x2026
	WGL_FULL_ACCELERATION_EXT			= 0x2027
	WGL_SWAP_EXCHANGE_EXT				= 0x2028
	WGL_SWAP_COPY_EXT				= 0x2029
	WGL_SWAP_UNDEFINED_EXT				= 0x202A
	WGL_TYPE_RGBA_EXT				= 0x202B
	WGL_TYPE_COLORINDEX_EXT				= 0x202C

WGL_ARB_pixel_format enum:
	WGL_NUMBER_PIXEL_FORMATS_ARB			= 0x2000
	WGL_DRAW_TO_WINDOW_ARB				= 0x2001
	WGL_DRAW_TO_BITMAP_ARB				= 0x2002
	WGL_ACCELERATION_ARB				= 0x2003
	WGL_NEED_PALETTE_ARB				= 0x2004
	WGL_NEED_SYSTEM_PALETTE_ARB			= 0x2005
	WGL_SWAP_LAYER_BUFFERS_ARB			= 0x2006
	WGL_SWAP_METHOD_ARB				= 0x2007
	WGL_NUMBER_OVERLAYS_ARB				= 0x2008
	WGL_NUMBER_UNDERLAYS_ARB			= 0x2009
	WGL_TRANSPARENT_ARB				= 0x200A
	WGL_SHARE_DEPTH_ARB				= 0x200C
	WGL_SHARE_STENCIL_ARB				= 0x200D
	WGL_SHARE_ACCUM_ARB				= 0x200E
	WGL_SUPPORT_GDI_ARB				= 0x200F
	WGL_SUPPORT_OPENGL_ARB				= 0x2010
	WGL_DOUBLE_BUFFER_ARB				= 0x2011
	WGL_STEREO_ARB					= 0x2012
	WGL_PIXEL_TYPE_ARB				= 0x2013
	WGL_COLOR_BITS_ARB				= 0x2014
	WGL_RED_BITS_ARB				= 0x2015
	WGL_RED_SHIFT_ARB				= 0x2016
	WGL_GREEN_BITS_ARB				= 0x2017
	WGL_GREEN_SHIFT_ARB				= 0x2018
	WGL_BLUE_BITS_ARB				= 0x2019
	WGL_BLUE_SHIFT_ARB				= 0x201A
	WGL_ALPHA_BITS_ARB				= 0x201B
	WGL_ALPHA_SHIFT_ARB				= 0x201C
	WGL_ACCUM_BITS_ARB				= 0x201D
	WGL_ACCUM_RED_BITS_ARB				= 0x201E
	WGL_ACCUM_GREEN_BITS_ARB			= 0x201F
	WGL_ACCUM_BLUE_BITS_ARB				= 0x2020
	WGL_ACCUM_ALPHA_BITS_ARB			= 0x2021
	WGL_DEPTH_BITS_ARB				= 0x2022
	WGL_STENCIL_BITS_ARB				= 0x2023
	WGL_AUX_BUFFERS_ARB				= 0x2024
	WGL_NO_ACCELERATION_ARB				= 0x2025
	WGL_GENERIC_ACCELERATION_ARB			= 0x2026
	WGL_FULL_ACCELERATION_ARB			= 0x2027
	WGL_SWAP_EXCHANGE_ARB				= 0x2028
	WGL_SWAP_COPY_ARB				= 0x2029
	WGL_SWAP_UNDEFINED_ARB				= 0x202A
	WGL_TYPE_RGBA_ARB				= 0x202B
	WGL_TYPE_COLORINDEX_ARB				= 0x202C
	WGL_DRAW_TO_PBUFFER_ARB				= 0x202D
	WGL_MAX_PBUFFER_PIXELS_ARB			= 0x202E
	WGL_MAX_PBUFFER_WIDTH_ARB			= 0x202F
	WGL_MAX_PBUFFER_HEIGHT_ARB			= 0x2030
	WGL_PBUFFER_LARGEST_ARB				= 0x2033
	WGL_PBUFFER_WIDTH_ARB				= 0x2034
	WGL_PBUFFER_HEIGHT_ARB				= 0x2035
	WGL_TRANSPARENT_RED_VALUE_ARB			= 0x2037
	WGL_TRANSPARENT_GREEN_VALUE_ARB			= 0x2038
	WGL_TRANSPARENT_BLUE_VALUE_ARB			= 0x2039
	WGL_TRANSPARENT_ALPHA_VALUE_ARB			= 0x203A
	WGL_TRANSPARENT_INDEX_VALUE_ARB			= 0x203B

###############################################################################

WGL_EXT_pbuffer enum:
	WGL_DRAW_TO_PBUFFER_EXT				= 0x202D
	WGL_MAX_PBUFFER_PIXELS_EXT			= 0x202E
	WGL_MAX_PBUFFER_WIDTH_EXT			= 0x202F
	WGL_MAX_PBUFFER_HEIGHT_EXT			= 0x2030
	WGL_OPTIMAL_PBUFFER_WIDTH_EXT			= 0x2031
	WGL_OPTIMAL_PBUFFER_HEIGHT_EXT			= 0x2032
	WGL_PBUFFER_LARGEST_EXT				= 0x2033
	WGL_PBUFFER_WIDTH_EXT				= 0x2034
	WGL_PBUFFER_HEIGHT_EXT				= 0x2035

WGL_ARB_pbuffer enum:
	WGL_DRAW_TO_PBUFFER_ARB				= 0x202D
	WGL_MAX_PBUFFER_PIXELS_ARB			= 0x202E
	WGL_MAX_PBUFFER_WIDTH_ARB			= 0x202F
	WGL_MAX_PBUFFER_HEIGHT_ARB			= 0x2030
	WGL_PBUFFER_LARGEST_ARB				= 0x2033
	WGL_PBUFFER_WIDTH_ARB				= 0x2034
	WGL_PBUFFER_HEIGHT_ARB				= 0x2035
	WGL_PBUFFER_LOST_ARB				= 0x2036
	use WGL_ARB_pixel_format WGL_TRANSPARENT_RED_VALUE_ARB
	use WGL_ARB_pixel_format WGL_TRANSPARENT_GREEN_VALUE_ARB
	use WGL_ARB_pixel_format WGL_TRANSPARENT_BLUE_VALUE_ARB
	use WGL_ARB_pixel_format WGL_TRANSPARENT_ALPHA_VALUE_ARB
	use WGL_ARB_pixel_format WGL_TRANSPARENT_INDEX_VALUE_ARB

###############################################################################

# SGI_future_use: 0x203C-0x203F

###############################################################################

# Intense3D: 0x2040-0x205F

WGL_EXT_depth_float enum:
	WGL_DEPTH_FLOAT_EXT				= 0x2040

###############################################################################

WGL_EXT_multisample enum:
	WGL_SAMPLE_BUFFERS_EXT				= 0x2041
	WGL_SAMPLES_EXT					= 0x2042

###############################################################################

WGL_EXT_make_current_read enum:
	ERROR_INVALID_PIXEL_TYPE_EXT			= 0x2043

WGL_ARB_make_current_read enum:
	ERROR_INVALID_PIXEL_TYPE_ARB			= 0x2043
	ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB		= 0x2054

###############################################################################

# Intense3D hasn't spec'ed this extension, but it's in the enum registry
WGL_I3D_unknown_genlock_extension_name enum:
	WGL_GENLOCK_SOURCE_MULTIVIEW_I3D		= 0x2044
	WGL_GENLOCK_SOURCE_EXTENAL_SYNC_I3D		= 0x2045
	WGL_GENLOCK_SOURCE_EXTENAL_FIELD_I3D		= 0x2046
	WGL_GENLOCK_SOURCE_EXTENAL_TTL_I3D		= 0x2047
	WGL_GENLOCK_SOURCE_DIGITAL_SYNC_I3D		= 0x2048
	WGL_GENLOCK_SOURCE_DIGITAL_FIELD_I3D		= 0x2049
	WGL_GENLOCK_SOURCE_EDGE_FALLING_I3D		= 0x204A
	WGL_GENLOCK_SOURCE_EDGE_RISING_I3D		= 0x204B
	WGL_GENLOCK_SOURCE_EDGE_BOTH_I3D		= 0x204C

###############################################################################

# Intense3D_future_use: 0x204D

###############################################################################

# Intense3D hasn't spec'ed this extension, but it's in the enum registry
WGL_I3D_unknown_gamma_extension_name enum:
	WGL_GAMMA_TABLE_SIZE_I3D			= 0x204E
	WGL_GAMMA_EXCLUDE_DESKTOP_I3D			= 0x204F

###############################################################################

# Intense3D hasn't spec'ed this extension, but it's in the enum registry
WGL_I3D_unknown_digital_video_cursor_extension_name enum:
	WGL_DIGITAL_VIDEO_CURSOR_ALPHA_FRAMEBUFFER_I3D	= 0x2050
	WGL_DIGITAL_VIDEO_CURSOR_ALPHA_VALUE_I3D	= 0x2051
	WGL_DIGITAL_VIDEO_CURSOR_INCLUDED_I3D		= 0x2052
	WGL_DIGITAL_VIDEO_GAMMA_CORRECTED_I3D		= 0x2053

###############################################################################

# WGL_ARB_make_current_read (additional; see above): 0x2054

###############################################################################

# Intense3D_future_use: 0x2055-0x205F

###############################################################################

# 3DFX: 0x2060-0x206F

WGL_3DFX_multisample enum:
	WGL_SAMPLE_BUFFERS_3DFX				= 0x2060
	WGL_SAMPLES_3DFX				= 0x2061

###############################################################################

# Enum blocks should be allocated by Steve Wright (swright@microsoft.com)
# Any_vendor_future_use: 0x2070-0xFFFF
