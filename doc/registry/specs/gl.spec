# gl.spec file
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

# @@ NOTE - need to distinguish extensions via some (new?) flag for glext.pl
# @@ NOTE - 'alias' commands are not yet used in SI generator scripts, but should be
# @@ NOTE - SI should support GLX protocol for at least these extensions:
#    AreTexturesResidentEXT BindTextureEXT DeleteTexturesEXT GenTexturesEXT IsTextureEXT

#
# $Date: 2000/05/30 07:11:57 $ $Revision: 1.5 $
# $Header: /home/pub/cvs/projects/ogl-sample/main/doc/registry/specs/gl.spec,v 1.5 2000/05/30 07:11:57 ljp Exp $
#

required-props:
param:		retval retained
dlflags:	notlistable handcode
glxflags:	client-intercept client-handcode server-handcode EXT SGI ignore ARB
vectorequiv:	*
category:	display-list drawing drawing-control feedback framebuf misc modeling pixel-op pixel-rw state-req xform 1_1 VERSION_1_2 EXT_blend_color EXT_blend_minmax EXT_convolution EXT_copy_texture EXT_histogram EXT_polygon_offset EXT_subtexture EXT_texture3D EXT_texture_object EXT_vertex_array SGIS_detail_texture SGIS_multisample SGIS_pixel_texture EXT_point_parameters SGIS_point_parameters SGIS_sharpen_texture SGIS_texture4D SGIS_texture_filter4 SGIX_flush_raster SGIX_fragment_lighting SGIX_framezoom SGIX_instruments SGIX_list_priority SGIX_pixel_texture SGIX_reference_plane SGIX_sprite SGIX_tag_sample_buffer SGI_color_table ARB_multitexture ARB_multisample ARB_texture_compression ARB_texture_cube_map ARB_transpose_matrix EXT_compiled_vertex_array EXT_cull_vertex EXT_index_func EXT_index_material EXT_draw_range_elements EXT_vertex_weighting NV_register_combiners NV_vertex_array_range MESA_resize_buffers MESA_window_pos PGI_misc_hints EXT_fog_coord EXT_blend_func_separate EXT_color_subtable EXT_coordinate_frame EXT_light_texture EXT_multi_draw_arrays EXT_paletted_texture EXT_pixel_transform EXT_secondary_color EXT_texture_perturb_normal HP_image_transform IBM_multimode_draw_arrays IBM_vertex_array_lists INTEL_parallel_arrays SUNX_constant_data SUN_global_alpha SUN_triangle_list SUN_vertex 3DFX_tbuffer EXT_multisample SGIS_fog_function SGIS_texture_color_mask
# categories for extensions with no functions - need not be included now
#   ARB_texture_env_add ARB_texture_cube_map EXT_abgr EXT_texture EXT_color_matrix SGI_texture_color_table EXT_cmyka EXT_packed_pixels SGIS_texture_lod EXT_rescale_normal EXT_misc_attribute SGIS_generate_mipmap SGIX_clipmap SGIX_shadow SGIS_texture_edge_clamp SGIS_texture_border_clamp EXT_blend_subtract EXT_blend_logic_op SGIX_interlace SGIX_pixel_tiles SGIX_texture_select SGIX_texture_multi_buffer SGIX_texture_scale_bias SGIX_depth_texture SGIX_fog_offset HP_convolution_border_modes SGIX_texture_add_env PGI_vertex_hints EXT_clip_volume_hint SGIX_ir_instrument1 SGIX_calligraphic_fragment SGIX_texture_lod_bias SGIX_shadow_ambient EXT_index_texture EXT_index_array_formats SGIX_ycrcb IBM_rasterpos_clip HP_texture_lighting WIN_phong_shading WIN_specular_fog SGIX_blend_alpha_minmax EXT_bgra HP_occlusion_test EXT_pixel_transform_color_table EXT_shared_texture_palette EXT_separate_specular_color EXT_texture_env REND_screen_coordinates EXT_texture_env_combine APPLE_specular_vector APPLE_transform_hint SGIX_fog_scale INGR_color_clamp INGR_interlace_read EXT_stencil_wrap EXT_422_pixels NV_texgen_reflection SUN_convolution_border_modes EXT_texture_env_add EXT_texture_lod_bias EXT_texture_filter_anisotropic NV_light_max_exponent NV_fog_distance NV_texgen_emboss NV_blend_square NV_texture_env_combine4 IBM_cull_vertex SGIX_subsample SGIX_ycrcba SGIX_ycrcb_subsample SGIX_depth_pass_instrument 3DFX_texture_compression_FXT1 3DFX_multisample SGI_vertex_preclip SGIX_convolution_accuracy SGIX_resample
version:	1.0 1.1 1.2
glxsingle:	*
glxropcode:	*
glxvendorpriv:	*
glsflags:	capture-handcode client get gl-enum ignore matrix pixel-null pixel-pack pixel-unpack client-state
glsopcode:	*
wglflags:	client-handcode server-handcode small-data batchable
extension:	future not_implemented soft WINSOFT NV10
alias:		*
aliasprivate:	*
offset:		*

###############################################################################
#
# GLX opcodes
#	glxsingle:		101-159     (1.0-1.2 core)
#				160	    (ARB_texture_compression)
#	glxropcode:		1-196	    (1.2 core; ropcode 140 unused?!)
#				197-213     (ARB_multitexture)
#				214-219     (ARB_texture_compression)
#				2048-2082   (SGI extensions)
#				4096-4123   (1.2 core and multivendor EXT)
#				4124-4142   (EXT extensions)
# XFree86 dispatch offsets:	0-560
#
# New opcodes and offsets must be allocated by SGI in the master registry file;
#    a copy of this is in doc/registry/extensions/extensions.reserved, but only
#    the copy maintained by SGI is the official and current version.
#
###############################################################################

###############################################################################
#
# things to remember when adding an extension command
#
# - add new commands at end of file
# - use tabs, not spaces
# - set glsflags to "ignore" until GLS is updated to support the new command
# - set glxflags to "ignore" until GLX is updated to support the new command
# - add new data types to typemaps/spec2wire.map
# - add extension name in alphabetical order to category list
# - add a batch of extensions in alphabetical order by extension name
# - add commands within an extension in spec order
# - use existing command entries as a model (where possible)
# - when reserving new glxropcodes, update
#	gfx/lib/opengl/doc/glspec/extensions.reserved to indicate this
#
###############################################################################

###############################################################################
#
# display-list commands
#
###############################################################################

NewList(list, mode)
	return		void
	param		list		List in value
	param		mode		ListMode in value
	dlflags		notlistable
	category	display-list
	version		1.0
	glxsingle	101
	glsopcode	0x0030
	wglflags	batchable
	offset		0

EndList()
	return		void
	dlflags		notlistable
	category	display-list
	version		1.0
	glxsingle	102
	glsopcode	0x0031
	wglflags	batchable
	offset		1

CallList(list)
	return		void
	param		list		List in value
	category	display-list
	version		1.0
	glxropcode	1
	glsopcode	0x0032
	offset		2

CallLists(n, type, lists)
	return		void
	param		n		SizeI in value
	param		type		ListNameType in value
	param		lists		Void in array [COMPSIZE(n/type)]
	category	display-list
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	2
	glsopcode	0x0033
	offset		3

DeleteLists(list, range)
	return		void
	param		list		List in value
	param		range		SizeI in value
	dlflags		notlistable
	category	display-list
	version		1.0
	glxsingle	103
	glsopcode	0x0034
	wglflags	batchable
	offset		4

GenLists(range)
	return		List
	param		range		SizeI in value
	dlflags		notlistable
	category	display-list
	version		1.0
	glxsingle	104
	glsopcode	0x0035
	offset		5

ListBase(base)
	return		void
	param		base		List in value
	category	display-list
	version		1.0
	glxropcode	3
	glsopcode	0x0036
	offset		6

###############################################################################
#
# drawing commands
#
###############################################################################

Begin(mode)
	return		void
	param		mode		BeginMode in value
	category	drawing
	version		1.0
	glxropcode	4
	glsopcode	0x0037
	offset		7

Bitmap(width, height, xorig, yorig, xmove, ymove, bitmap)
	return		void
	param		width		SizeI in value
	param		height		SizeI in value
	param		xorig		CoordF in value
	param		yorig		CoordF in value
	param		xmove		CoordF in value
	param		ymove		CoordF in value
	param		bitmap		UInt8 in array [COMPSIZE(width/height)]
	category	drawing
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	5
	glsflags	pixel-unpack
	glsopcode	0x0038
	wglflags	client-handcode server-handcode
	offset		8

Color3b(red, green, blue)
	return		void
	param		red		ColorB in value
	param		green		ColorB in value
	param		blue		ColorB in value
	category	drawing
	vectorequiv	Color3bv
	version		1.0
	offset		9

Color3bv(v)
	return		void
	param		v		ColorB in array [3]
	category	drawing
	version		1.0
	glxropcode	6
	glsopcode	0x0039
	offset		10

Color3d(red, green, blue)
	return		void
	param		red		ColorD in value
	param		green		ColorD in value
	param		blue		ColorD in value
	category	drawing
	vectorequiv	Color3dv
	version		1.0
	offset		11

Color3dv(v)
	return		void
	param		v		ColorD in array [3]
	category	drawing
	version		1.0
	glxropcode	7
	glsopcode	0x003A
	offset		12

Color3f(red, green, blue)
	return		void
	param		red		ColorF in value
	param		green		ColorF in value
	param		blue		ColorF in value
	category	drawing
	vectorequiv	Color3fv
	version		1.0
	offset		13

Color3fv(v)
	return		void
	param		v		ColorF in array [3]
	category	drawing
	version		1.0
	glxropcode	8
	glsopcode	0x003B
	offset		14

Color3i(red, green, blue)
	return		void
	param		red		ColorI in value
	param		green		ColorI in value
	param		blue		ColorI in value
	category	drawing
	vectorequiv	Color3iv
	version		1.0
	offset		15

Color3iv(v)
	return		void
	param		v		ColorI in array [3]
	category	drawing
	version		1.0
	glxropcode	9
	glsopcode	0x003C
	offset		16

Color3s(red, green, blue)
	return		void
	param		red		ColorS in value
	param		green		ColorS in value
	param		blue		ColorS in value
	category	drawing
	vectorequiv	Color3sv
	version		1.0
	offset		17

Color3sv(v)
	return		void
	param		v		ColorS in array [3]
	category	drawing
	version		1.0
	glxropcode	10
	glsopcode	0x003D
	offset		18

Color3ub(red, green, blue)
	return		void
	param		red		ColorUB in value
	param		green		ColorUB in value
	param		blue		ColorUB in value
	category	drawing
	vectorequiv	Color3ubv
	version		1.0
	offset		19

Color3ubv(v)
	return		void
	param		v		ColorUB in array [3]
	category	drawing
	version		1.0
	glxropcode	11
	glsopcode	0x003E
	offset		20

Color3ui(red, green, blue)
	return		void
	param		red		ColorUI in value
	param		green		ColorUI in value
	param		blue		ColorUI in value
	category	drawing
	vectorequiv	Color3uiv
	version		1.0
	offset		21

Color3uiv(v)
	return		void
	param		v		ColorUI in array [3]
	category	drawing
	version		1.0
	glxropcode	12
	glsopcode	0x003F
	offset		22

Color3us(red, green, blue)
	return		void
	param		red		ColorUS in value
	param		green		ColorUS in value
	param		blue		ColorUS in value
	category	drawing
	vectorequiv	Color3usv
	version		1.0
	offset		23

Color3usv(v)
	return		void
	param		v		ColorUS in array [3]
	category	drawing
	version		1.0
	glxropcode	13
	glsopcode	0x0040
	offset		24

Color4b(red, green, blue, alpha)
	return		void
	param		red		ColorB in value
	param		green		ColorB in value
	param		blue		ColorB in value
	param		alpha		ColorB in value
	category	drawing
	vectorequiv	Color4bv
	version		1.0
	offset		25

Color4bv(v)
	return		void
	param		v		ColorB in array [4]
	category	drawing
	version		1.0
	glxropcode	14
	glsopcode	0x0041
	offset		26

Color4d(red, green, blue, alpha)
	return		void
	param		red		ColorD in value
	param		green		ColorD in value
	param		blue		ColorD in value
	param		alpha		ColorD in value
	category	drawing
	vectorequiv	Color4dv
	version		1.0
	offset		27

Color4dv(v)
	return		void
	param		v		ColorD in array [4]
	category	drawing
	version		1.0
	glxropcode	15
	glsopcode	0x0042
	offset		28

Color4f(red, green, blue, alpha)
	return		void
	param		red		ColorF in value
	param		green		ColorF in value
	param		blue		ColorF in value
	param		alpha		ColorF in value
	category	drawing
	vectorequiv	Color4fv
	version		1.0
	offset		29

Color4fv(v)
	return		void
	param		v		ColorF in array [4]
	category	drawing
	version		1.0
	glxropcode	16
	glsopcode	0x0043
	offset		30

Color4i(red, green, blue, alpha)
	return		void
	param		red		ColorI in value
	param		green		ColorI in value
	param		blue		ColorI in value
	param		alpha		ColorI in value
	category	drawing
	vectorequiv	Color4iv
	version		1.0
	offset		31

Color4iv(v)
	return		void
	param		v		ColorI in array [4]
	category	drawing
	version		1.0
	glxropcode	17
	glsopcode	0x0044
	offset		32

Color4s(red, green, blue, alpha)
	return		void
	param		red		ColorS in value
	param		green		ColorS in value
	param		blue		ColorS in value
	param		alpha		ColorS in value
	category	drawing
	vectorequiv	Color4sv
	version		1.0
	offset		33

Color4sv(v)
	return		void
	param		v		ColorS in array [4]
	category	drawing
	version		1.0
	glxropcode	18
	glsopcode	0x0045
	offset		34

Color4ub(red, green, blue, alpha)
	return		void
	param		red		ColorUB in value
	param		green		ColorUB in value
	param		blue		ColorUB in value
	param		alpha		ColorUB in value
	category	drawing
	vectorequiv	Color4ubv
	version		1.0
	offset		35

Color4ubv(v)
	return		void
	param		v		ColorUB in array [4]
	category	drawing
	version		1.0
	glxropcode	19
	glsopcode	0x0046
	offset		36

Color4ui(red, green, blue, alpha)
	return		void
	param		red		ColorUI in value
	param		green		ColorUI in value
	param		blue		ColorUI in value
	param		alpha		ColorUI in value
	category	drawing
	vectorequiv	Color4uiv
	version		1.0
	offset		37

Color4uiv(v)
	return		void
	param		v		ColorUI in array [4]
	category	drawing
	version		1.0
	glxropcode	20
	glsopcode	0x0047
	offset		38

Color4us(red, green, blue, alpha)
	return		void
	param		red		ColorUS in value
	param		green		ColorUS in value
	param		blue		ColorUS in value
	param		alpha		ColorUS in value
	category	drawing
	vectorequiv	Color4usv
	version		1.0
	offset		39

Color4usv(v)
	return		void
	param		v		ColorUS in array [4]
	category	drawing
	version		1.0
	glxropcode	21
	glsopcode	0x0048
	offset		40

EdgeFlag(flag)
	return		void
	param		flag		Boolean in value
	category	drawing
	vectorequiv	EdgeFlagv
	version		1.0
	offset		41

EdgeFlagv(flag)
	return		void
	param		flag		Boolean in array [1]
	category	drawing
	version		1.0
	glxropcode	22
	glsopcode	0x0049
	offset		42

End()
	return		void
	category	drawing
	version		1.0
	glxropcode	23
	glsopcode	0x004A
	offset		43

Indexd(c)
	return		void
	param		c		ColorIndexValueD in value
	category	drawing
	vectorequiv	Indexdv
	version		1.0
	offset		44

Indexdv(c)
	return		void
	param		c		ColorIndexValueD in array [1]
	category	drawing
	version		1.0
	glxropcode	24
	glsopcode	0x004B
	offset		45

Indexf(c)
	return		void
	param		c		ColorIndexValueF in value
	category	drawing
	vectorequiv	Indexfv
	version		1.0
	offset		46

Indexfv(c)
	return		void
	param		c		ColorIndexValueF in array [1]
	category	drawing
	version		1.0
	glxropcode	25
	glsopcode	0x004C
	offset		47

Indexi(c)
	return		void
	param		c		ColorIndexValueI in value
	category	drawing
	vectorequiv	Indexiv
	version		1.0
	offset		48

Indexiv(c)
	return		void
	param		c		ColorIndexValueI in array [1]
	category	drawing
	version		1.0
	glxropcode	26
	glsopcode	0x004D
	offset		49

Indexs(c)
	return		void
	param		c		ColorIndexValueS in value
	category	drawing
	vectorequiv	Indexsv
	version		1.0
	offset		50

Indexsv(c)
	return		void
	param		c		ColorIndexValueS in array [1]
	category	drawing
	version		1.0
	glxropcode	27
	glsopcode	0x004E
	offset		51

Normal3b(nx, ny, nz)
	return		void
	param		nx		Int8 in value
	param		ny		Int8 in value
	param		nz		Int8 in value
	category	drawing
	vectorequiv	Normal3bv
	version		1.0
	offset		52

Normal3bv(v)
	return		void
	param		v		Int8 in array [3]
	category	drawing
	version		1.0
	glxropcode	28
	glsopcode	0x004F
	offset		53

Normal3d(nx, ny, nz)
	return		void
	param		nx		CoordD in value
	param		ny		CoordD in value
	param		nz		CoordD in value
	category	drawing
	vectorequiv	Normal3dv
	version		1.0
	offset		54

Normal3dv(v)
	return		void
	param		v		CoordD in array [3]
	category	drawing
	version		1.0
	glxropcode	29
	glsopcode	0x0050
	offset		55

Normal3f(nx, ny, nz)
	return		void
	param		nx		CoordF in value
	param		ny		CoordF in value
	param		nz		CoordF in value
	category	drawing
	vectorequiv	Normal3fv
	version		1.0
	offset		56

Normal3fv(v)
	return		void
	param		v		CoordF in array [3]
	category	drawing
	version		1.0
	glxropcode	30
	glsopcode	0x0051
	offset		57

Normal3i(nx, ny, nz)
	return		void
	param		nx		Int32 in value
	param		ny		Int32 in value
	param		nz		Int32 in value
	category	drawing
	vectorequiv	Normal3iv
	version		1.0
	offset		58

Normal3iv(v)
	return		void
	param		v		Int32 in array [3]
	category	drawing
	version		1.0
	glxropcode	31
	glsopcode	0x0052
	offset		59

Normal3s(nx, ny, nz)
	return		void
	param		nx		Int16 in value
	param		ny		Int16 in value
	param		nz		Int16 in value
	category	drawing
	vectorequiv	Normal3sv
	version		1.0
	offset		60

Normal3sv(v)
	return		void
	param		v		Int16 in array [3]
	category	drawing
	version		1.0
	glxropcode	32
	glsopcode	0x0053
	offset		61

RasterPos2d(x, y)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	category	drawing
	vectorequiv	RasterPos2dv
	version		1.0
	offset		62

RasterPos2dv(v)
	return		void
	param		v		CoordD in array [2]
	category	drawing
	version		1.0
	glxropcode	33
	glsopcode	0x0054
	offset		63

RasterPos2f(x, y)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	category	drawing
	vectorequiv	RasterPos2fv
	version		1.0
	offset		64

RasterPos2fv(v)
	return		void
	param		v		CoordF in array [2]
	category	drawing
	version		1.0
	glxropcode	34
	glsopcode	0x0055
	offset		65

RasterPos2i(x, y)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	category	drawing
	vectorequiv	RasterPos2iv
	version		1.0
	offset		66

RasterPos2iv(v)
	return		void
	param		v		CoordI in array [2]
	category	drawing
	version		1.0
	glxropcode	35
	glsopcode	0x0056
	offset		67

RasterPos2s(x, y)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	category	drawing
	vectorequiv	RasterPos2sv
	version		1.0
	offset		68

RasterPos2sv(v)
	return		void
	param		v		CoordS in array [2]
	category	drawing
	version		1.0
	glxropcode	36
	glsopcode	0x0057
	offset		69

RasterPos3d(x, y, z)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	param		z		CoordD in value
	vectorequiv	RasterPos3dv
	category	drawing
	version		1.0
	offset		70

RasterPos3dv(v)
	return		void
	param		v		CoordD in array [3]
	category	drawing
	version		1.0
	glxropcode	37
	glsopcode	0x0058
	offset		71

RasterPos3f(x, y, z)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	param		z		CoordF in value
	category	drawing
	vectorequiv	RasterPos3fv
	version		1.0
	offset		72

RasterPos3fv(v)
	return		void
	param		v		CoordF in array [3]
	category	drawing
	version		1.0
	glxropcode	38
	glsopcode	0x0059
	offset		73

RasterPos3i(x, y, z)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	param		z		CoordI in value
	category	drawing
	vectorequiv	RasterPos3iv
	version		1.0
	offset		74

RasterPos3iv(v)
	return		void
	param		v		CoordI in array [3]
	category	drawing
	version		1.0
	glxropcode	39
	glsopcode	0x005A
	offset		75

RasterPos3s(x, y, z)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	param		z		CoordS in value
	category	drawing
	vectorequiv	RasterPos3sv
	version		1.0
	offset		76

RasterPos3sv(v)
	return		void
	param		v		CoordS in array [3]
	category	drawing
	version		1.0
	glxropcode	40
	glsopcode	0x005B
	offset		77

RasterPos4d(x, y, z, w)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	param		z		CoordD in value
	param		w		CoordD in value
	vectorequiv	RasterPos4dv
	category	drawing
	version		1.0
	offset		78

RasterPos4dv(v)
	return		void
	param		v		CoordD in array [4]
	category	drawing
	version		1.0
	glxropcode	41
	glsopcode	0x005C
	offset		79

RasterPos4f(x, y, z, w)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	param		z		CoordF in value
	param		w		CoordF in value
	category	drawing
	vectorequiv	RasterPos4fv
	version		1.0
	offset		80

RasterPos4fv(v)
	return		void
	param		v		CoordF in array [4]
	category	drawing
	version		1.0
	glxropcode	42
	glsopcode	0x005D
	offset		81

RasterPos4i(x, y, z, w)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	param		z		CoordI in value
	param		w		CoordI in value
	category	drawing
	vectorequiv	RasterPos4iv
	version		1.0
	offset		82

RasterPos4iv(v)
	return		void
	param		v		CoordI in array [4]
	category	drawing
	version		1.0
	glxropcode	43
	glsopcode	0x005E
	offset		83

RasterPos4s(x, y, z, w)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	param		z		CoordS in value
	param		w		CoordS in value
	category	drawing
	vectorequiv	RasterPos4sv
	version		1.0
	offset		84

RasterPos4sv(v)
	return		void
	param		v		CoordS in array [4]
	category	drawing
	version		1.0
	glxropcode	44
	glsopcode	0x005F
	offset		85

Rectd(x1, y1, x2, y2)
	return		void
	param		x1		CoordD in value
	param		y1		CoordD in value
	param		x2		CoordD in value
	param		y2		CoordD in value
	category	drawing
	vectorequiv	Rectdv
	version		1.0
	offset		86

Rectdv(v1, v2)
	return		void
	param		v1		CoordD in array [2]
	param		v2		CoordD in array [2]
	category	drawing
	version		1.0
	glxropcode	45
	glsopcode	0x0060
	offset		87

Rectf(x1, y1, x2, y2)
	return		void
	param		x1		CoordF in value
	param		y1		CoordF in value
	param		x2		CoordF in value
	param		y2		CoordF in value
	category	drawing
	vectorequiv	Rectfv
	version		1.0
	offset		88

Rectfv(v1, v2)
	return		void
	param		v1		CoordF in array [2]
	param		v2		CoordF in array [2]
	category	drawing
	version		1.0
	glxropcode	46
	glsopcode	0x0061
	offset		89

Recti(x1, y1, x2, y2)
	return		void
	param		x1		CoordI in value
	param		y1		CoordI in value
	param		x2		CoordI in value
	param		y2		CoordI in value
	category	drawing
	vectorequiv	Rectiv
	version		1.0
	offset		90

Rectiv(v1, v2)
	return		void
	param		v1		CoordI in array [2]
	param		v2		CoordI in array [2]
	category	drawing
	version		1.0
	glxropcode	47
	glsopcode	0x0062
	offset		91

Rects(x1, y1, x2, y2)
	return		void
	param		x1		CoordS in value
	param		y1		CoordS in value
	param		x2		CoordS in value
	param		y2		CoordS in value
	category	drawing
	vectorequiv	Rectsv
	version		1.0
	offset		92

Rectsv(v1, v2)
	return		void
	param		v1		CoordS in array [2]
	param		v2		CoordS in array [2]
	category	drawing
	version		1.0
	glxropcode	48
	glsopcode	0x0063
	offset		93

TexCoord1d(s)
	return		void
	param		s		CoordD in value
	category	drawing
	vectorequiv	TexCoord1dv
	version		1.0
	offset		94

TexCoord1dv(v)
	return		void
	param		v		CoordD in array [1]
	category	drawing
	version		1.0
	glxropcode	49
	glsopcode	0x0064
	offset		95

TexCoord1f(s)
	return		void
	param		s		CoordF in value
	category	drawing
	vectorequiv	TexCoord1fv
	version		1.0
	offset		96

TexCoord1fv(v)
	return		void
	param		v		CoordF in array [1]
	category	drawing
	version		1.0
	glxropcode	50
	glsopcode	0x0065
	offset		97

TexCoord1i(s)
	return		void
	param		s		CoordI in value
	category	drawing
	vectorequiv	TexCoord1iv
	version		1.0
	offset		98

TexCoord1iv(v)
	return		void
	param		v		CoordI in array [1]
	category	drawing
	version		1.0
	glxropcode	51
	glsopcode	0x0066
	offset		99

TexCoord1s(s)
	return		void
	param		s		CoordS in value
	category	drawing
	vectorequiv	TexCoord1sv
	version		1.0
	offset		100

TexCoord1sv(v)
	return		void
	param		v		CoordS in array [1]
	category	drawing
	version		1.0
	glxropcode	52
	glsopcode	0x0067
	offset		101

TexCoord2d(s, t)
	return		void
	param		s		CoordD in value
	param		t		CoordD in value
	category	drawing
	vectorequiv	TexCoord2dv
	version		1.0
	offset		102

TexCoord2dv(v)
	return		void
	param		v		CoordD in array [2]
	category	drawing
	version		1.0
	glxropcode	53
	glsopcode	0x0068
	offset		103

TexCoord2f(s, t)
	return		void
	param		s		CoordF in value
	param		t		CoordF in value
	category	drawing
	vectorequiv	TexCoord2fv
	version		1.0
	offset		104

TexCoord2fv(v)
	return		void
	param		v		CoordF in array [2]
	category	drawing
	version		1.0
	glxropcode	54
	glsopcode	0x0069
	offset		105

TexCoord2i(s, t)
	return		void
	param		s		CoordI in value
	param		t		CoordI in value
	category	drawing
	vectorequiv	TexCoord2iv
	version		1.0
	offset		106

TexCoord2iv(v)
	return		void
	param		v		CoordI in array [2]
	category	drawing
	version		1.0
	glxropcode	55
	glsopcode	0x006A
	offset		107

TexCoord2s(s, t)
	return		void
	param		s		CoordS in value
	param		t		CoordS in value
	category	drawing
	vectorequiv	TexCoord2sv
	version		1.0
	offset		108

TexCoord2sv(v)
	return		void
	param		v		CoordS in array [2]
	category	drawing
	version		1.0
	glxropcode	56
	glsopcode	0x006B
	offset		109

TexCoord3d(s, t, r)
	return		void
	param		s		CoordD in value
	param		t		CoordD in value
	param		r		CoordD in value
	category	drawing
	vectorequiv	TexCoord3dv
	version		1.0
	offset		110

TexCoord3dv(v)
	return		void
	param		v		CoordD in array [3]
	category	drawing
	version		1.0
	glxropcode	57
	glsopcode	0x006C
	offset		111

TexCoord3f(s, t, r)
	return		void
	param		s		CoordF in value
	param		t		CoordF in value
	param		r		CoordF in value
	category	drawing
	vectorequiv	TexCoord3fv
	version		1.0
	offset		112

TexCoord3fv(v)
	return		void
	param		v		CoordF in array [3]
	category	drawing
	version		1.0
	glxropcode	58
	glsopcode	0x006D
	offset		113

TexCoord3i(s, t, r)
	return		void
	param		s		CoordI in value
	param		t		CoordI in value
	param		r		CoordI in value
	category	drawing
	vectorequiv	TexCoord3iv
	version		1.0
	offset		114

TexCoord3iv(v)
	return		void
	param		v		CoordI in array [3]
	category	drawing
	version		1.0
	glxropcode	59
	glsopcode	0x006E
	offset		115

TexCoord3s(s, t, r)
	return		void
	param		s		CoordS in value
	param		t		CoordS in value
	param		r		CoordS in value
	category	drawing
	vectorequiv	TexCoord3sv
	version		1.0
	offset		116

TexCoord3sv(v)
	return		void
	param		v		CoordS in array [3]
	category	drawing
	version		1.0
	glxropcode	60
	glsopcode	0x006F
	offset		117

TexCoord4d(s, t, r, q)
	return		void
	param		s		CoordD in value
	param		t		CoordD in value
	param		r		CoordD in value
	param		q		CoordD in value
	category	drawing
	vectorequiv	TexCoord4dv
	version		1.0
	offset		118

TexCoord4dv(v)
	return		void
	param		v		CoordD in array [4]
	category	drawing
	version		1.0
	glxropcode	61
	glsopcode	0x0070
	offset		119

TexCoord4f(s, t, r, q)
	return		void
	param		s		CoordF in value
	param		t		CoordF in value
	param		r		CoordF in value
	param		q		CoordF in value
	category	drawing
	vectorequiv	TexCoord4fv
	version		1.0
	offset		120

TexCoord4fv(v)
	return		void
	param		v		CoordF in array [4]
	category	drawing
	version		1.0
	glxropcode	62
	glsopcode	0x0071
	offset		121

TexCoord4i(s, t, r, q)
	return		void
	param		s		CoordI in value
	param		t		CoordI in value
	param		r		CoordI in value
	param		q		CoordI in value
	category	drawing
	vectorequiv	TexCoord4iv
	version		1.0
	offset		122

TexCoord4iv(v)
	return		void
	param		v		CoordI in array [4]
	category	drawing
	version		1.0
	glxropcode	63
	glsopcode	0x0072
	offset		123

TexCoord4s(s, t, r, q)
	return		void
	param		s		CoordS in value
	param		t		CoordS in value
	param		r		CoordS in value
	param		q		CoordS in value
	category	drawing
	vectorequiv	TexCoord4sv
	version		1.0
	offset		124

TexCoord4sv(v)
	return		void
	param		v		CoordS in array [4]
	category	drawing
	version		1.0
	glxropcode	64
	glsopcode	0x0073
	offset		125

Vertex2d(x, y)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	category	drawing
	vectorequiv	Vertex2dv
	version		1.0
	offset		126

Vertex2dv(v)
	return		void
	param		v		CoordD in array [2]
	category	drawing
	version		1.0
	glxropcode	65
	glsopcode	0x0074
	offset		127

Vertex2f(x, y)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	category	drawing
	vectorequiv	Vertex2fv
	version		1.0
	offset		128

Vertex2fv(v)
	return		void
	param		v		CoordF in array [2]
	category	drawing
	version		1.0
	glxropcode	66
	glsopcode	0x0075
	offset		129

Vertex2i(x, y)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	category	drawing
	vectorequiv	Vertex2iv
	version		1.0
	offset		130

Vertex2iv(v)
	return		void
	param		v		CoordI in array [2]
	category	drawing
	version		1.0
	glxropcode	67
	glsopcode	0x0076
	offset		131

Vertex2s(x, y)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	category	drawing
	vectorequiv	Vertex2sv
	version		1.0
	offset		132

Vertex2sv(v)
	return		void
	param		v		CoordS in array [2]
	category	drawing
	version		1.0
	glxropcode	68
	glsopcode	0x0077
	offset		133

Vertex3d(x, y, z)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	param		z		CoordD in value
	category	drawing
	vectorequiv	Vertex3dv
	version		1.0
	offset		134

Vertex3dv(v)
	return		void
	param		v		CoordD in array [3]
	category	drawing
	version		1.0
	glxropcode	69
	glsopcode	0x0078
	offset		135

Vertex3f(x, y, z)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	param		z		CoordF in value
	category	drawing
	vectorequiv	Vertex3fv
	version		1.0
	offset		136

Vertex3fv(v)
	return		void
	param		v		CoordF in array [3]
	category	drawing
	version		1.0
	glxropcode	70
	glsopcode	0x0079
	offset		137

Vertex3i(x, y, z)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	param		z		CoordI in value
	category	drawing
	vectorequiv	Vertex3iv
	version		1.0
	offset		138

Vertex3iv(v)
	return		void
	param		v		CoordI in array [3]
	category	drawing
	version		1.0
	glxropcode	71
	glsopcode	0x007A
	offset		139

Vertex3s(x, y, z)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	param		z		CoordS in value
	category	drawing
	vectorequiv	Vertex3sv
	version		1.0
	offset		140

Vertex3sv(v)
	return		void
	param		v		CoordS in array [3]
	category	drawing
	version		1.0
	glxropcode	72
	glsopcode	0x007B
	offset		141

Vertex4d(x, y, z, w)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	param		z		CoordD in value
	param		w		CoordD in value
	category	drawing
	vectorequiv	Vertex4dv
	version		1.0
	offset		142

Vertex4dv(v)
	return		void
	param		v		CoordD in array [4]
	category	drawing
	version		1.0
	glxropcode	73
	glsopcode	0x007C
	offset		143

Vertex4f(x, y, z, w)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	param		z		CoordF in value
	param		w		CoordF in value
	category	drawing
	vectorequiv	Vertex4fv
	version		1.0
	offset		144

Vertex4fv(v)
	return		void
	param		v		CoordF in array [4]
	category	drawing
	version		1.0
	glxropcode	74
	glsopcode	0x007D
	offset		145

Vertex4i(x, y, z, w)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	param		z		CoordI in value
	param		w		CoordI in value
	category	drawing
	vectorequiv	Vertex4iv
	version		1.0
	offset		146

Vertex4iv(v)
	return		void
	param		v		CoordI in array [4]
	category	drawing
	version		1.0
	glxropcode	75
	glsopcode	0x007E
	offset		147

Vertex4s(x, y, z, w)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	param		z		CoordS in value
	param		w		CoordS in value
	category	drawing
	vectorequiv	Vertex4sv
	version		1.0
	offset		148

Vertex4sv(v)
	return		void
	param		v		CoordS in array [4]
	category	drawing
	version		1.0
	glxropcode	76
	glsopcode	0x007F
	offset		149

###############################################################################
#
# drawing-control commands
#
###############################################################################

ClipPlane(plane, equation)
	return		void
	param		plane		ClipPlaneName in value
	param		equation	Float64 in array [4]
	category	drawing-control
	version		1.0
	glxropcode	77
	glsopcode	0x0080
	offset		150

ColorMaterial(face, mode)
	return		void
	param		face		MaterialFace in value
	param		mode		ColorMaterialParameter in value
	category	drawing-control
	version		1.0
	glxropcode	78
	glsopcode	0x0081
	offset		151

CullFace(mode)
	return		void
	param		mode		CullFaceMode in value
	category	drawing-control
	version		1.0
	glxropcode	79
	glsopcode	0x0082
	offset		152

Fogf(pname, param)
	return		void
	param		pname		FogParameter in value
	param		param		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	80
	glsflags	gl-enum
	glsopcode	0x0083
	wglflags	small-data
	offset		153

Fogfv(pname, params)
	return		void
	param		pname		FogParameter in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	81
	glsflags	gl-enum
	glsopcode	0x0084
	wglflags	small-data
	offset		154

Fogi(pname, param)
	return		void
	param		pname		FogParameter in value
	param		param		CheckedInt32 in value
	category	drawing-control
	version		1.0
	glxropcode	82
	glsflags	gl-enum
	glsopcode	0x0085
	wglflags	small-data
	offset		155

Fogiv(pname, params)
	return		void
	param		pname		FogParameter in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	83
	glsflags	gl-enum
	glsopcode	0x0086
	wglflags	small-data
	offset		156

FrontFace(mode)
	return		void
	param		mode		FrontFaceDirection in value
	category	drawing-control
	version		1.0
	glxropcode	84
	glsopcode	0x0087
	offset		157

Hint(target, mode)
	return		void
	param		target		HintTarget in value
	param		mode		HintMode in value
	category	drawing-control
	version		1.0
	glxropcode	85
	glsopcode	0x0088
	offset		158

Lightf(light, pname, param)
	return		void
	param		light		LightName in value
	param		pname		LightParameter in value
	param		param		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	86
	glsopcode	0x0089
	wglflags	small-data
	offset		159

Lightfv(light, pname, params)
	return		void
	param		light		LightName in value
	param		pname		LightParameter in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	87
	glsopcode	0x008A
	wglflags	small-data
	offset		160

Lighti(light, pname, param)
	return		void
	param		light		LightName in value
	param		pname		LightParameter in value
	param		param		CheckedInt32 in value
	category	drawing-control
	version		1.0
	glxropcode	88
	glsopcode	0x008B
	wglflags	small-data
	offset		161

Lightiv(light, pname, params)
	return		void
	param		light		LightName in value
	param		pname		LightParameter in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	89
	glsopcode	0x008C
	wglflags	small-data
	offset		162

LightModelf(pname, param)
	return		void
	param		pname		LightModelParameter in value
	param		param		Float32 in value
	category	drawing-control
	version		1.0
	glxropcode	90
	glsflags	gl-enum
	glsopcode	0x008D
	wglflags	small-data
	offset		163

LightModelfv(pname, params)
	return		void
	param		pname		LightModelParameter in value
	param		params		Float32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	91
	glsflags	gl-enum
	glsopcode	0x008E
	wglflags	small-data
	offset		164

LightModeli(pname, param)
	return		void
	param		pname		LightModelParameter in value
	param		param		Int32 in value
	category	drawing-control
	version		1.0
	glxropcode	92
	glsflags	gl-enum
	glsopcode	0x008F
	wglflags	small-data
	offset		165

LightModeliv(pname, params)
	return		void
	param		pname		LightModelParameter in value
	param		params		Int32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	93
	glsflags	gl-enum
	glsopcode	0x0090
	wglflags	small-data
	offset		166

LineStipple(factor, pattern)
	return		void
	param		factor		CheckedInt32 in value
	param		pattern		LineStipple in value
	category	drawing-control
	version		1.0
	glxropcode	94
	glsopcode	0x0091
	offset		167

LineWidth(width)
	return		void
	param		width		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	95
	glsopcode	0x0092
	offset		168

Materialf(face, pname, param)
	return		void
	param		face		MaterialFace in value
	param		pname		MaterialParameter in value
	param		param		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	96
	glsopcode	0x0093
	wglflags	small-data
	offset		169

Materialfv(face, pname, params)
	return		void
	param		face		MaterialFace in value
	param		pname		MaterialParameter in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	97
	glsopcode	0x0094
	wglflags	small-data
	offset		170

Materiali(face, pname, param)
	return		void
	param		face		MaterialFace in value
	param		pname		MaterialParameter in value
	param		param		CheckedInt32 in value
	category	drawing-control
	version		1.0
	glxropcode	98
	glsopcode	0x0095
	wglflags	small-data
	offset		171

Materialiv(face, pname, params)
	return		void
	param		face		MaterialFace in value
	param		pname		MaterialParameter in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	99
	glsopcode	0x0096
	wglflags	small-data
	offset		172

PointSize(size)
	return		void
	param		size		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	100
	glsopcode	0x0097
	offset		173

PolygonMode(face, mode)
	return		void
	param		face		MaterialFace in value
	param		mode		PolygonMode in value
	category	drawing-control
	version		1.0
	glxropcode	101
	glsopcode	0x0098
	offset		174

PolygonStipple(mask)
	return		void
	param		mask		UInt8 in array [COMPSIZE()]
	category	drawing-control
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	102
	glsflags	pixel-unpack
	glsopcode	0x0099
	wglflags	client-handcode server-handcode
	offset		175

Scissor(x, y, width, height)
	return		void
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	category	drawing-control
	version		1.0
	glxropcode	103
	glsopcode	0x009A
	offset		176

ShadeModel(mode)
	return		void
	param		mode		ShadingModel in value
	category	drawing-control
	version		1.0
	glxropcode	104
	glsopcode	0x009B
	offset		177

TexParameterf(target, pname, param)
	return		void
	param		target		TextureTarget in value
	param		pname		TextureParameterName in value
	param		param		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	105
	glsflags	gl-enum
	glsopcode	0x009C
	wglflags	small-data
	offset		178

TexParameterfv(target, pname, params)
	return		void
	param		target		TextureTarget in value
	param		pname		TextureParameterName in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	106
	glsflags	gl-enum
	glsopcode	0x009D
	wglflags	small-data
	offset		179

TexParameteri(target, pname, param)
	return		void
	param		target		TextureTarget in value
	param		pname		TextureParameterName in value
	param		param		CheckedInt32 in value
	category	drawing-control
	version		1.0
	glxropcode	107
	glsflags	gl-enum
	glsopcode	0x009E
	wglflags	small-data
	offset		180

TexParameteriv(target, pname, params)
	return		void
	param		target		TextureTarget in value
	param		pname		TextureParameterName in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	108
	glsflags	gl-enum
	glsopcode	0x009F
	wglflags	small-data
	offset		181

TexImage1D(target, level, internalformat, width, border, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	TextureComponentCount in value
	param		width		SizeI in value
	param		border		CheckedInt32 in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width)]
	category	drawing-control
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	109
	glsflags	pixel-null pixel-unpack
	glsopcode	0x00A0
	wglflags	client-handcode server-handcode
	offset		182

TexImage2D(target, level, internalformat, width, height, border, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	TextureComponentCount in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		border		CheckedInt32 in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width/height)]
	category	drawing-control
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	110
	glsflags	pixel-null pixel-unpack
	glsopcode	0x00A1
	wglflags	client-handcode server-handcode
	offset		183

TexEnvf(target, pname, param)
	return		void
	param		target		TextureEnvTarget in value
	param		pname		TextureEnvParameter in value
	param		param		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	111
	glsflags	gl-enum
	glsopcode	0x00A2
	wglflags	small-data
	offset		184

TexEnvfv(target, pname, params)
	return		void
	param		target		TextureEnvTarget in value
	param		pname		TextureEnvParameter in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	112
	glsflags	gl-enum
	glsopcode	0x00A3
	wglflags	small-data
	offset		185

TexEnvi(target, pname, param)
	return		void
	param		target		TextureEnvTarget in value
	param		pname		TextureEnvParameter in value
	param		param		CheckedInt32 in value
	category	drawing-control
	version		1.0
	glxropcode	113
	glsflags	gl-enum
	glsopcode	0x00A4
	wglflags	small-data
	offset		186

TexEnviv(target, pname, params)
	return		void
	param		target		TextureEnvTarget in value
	param		pname		TextureEnvParameter in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	114
	glsflags	gl-enum
	glsopcode	0x00A5
	wglflags	small-data
	offset		187

TexGend(coord, pname, param)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		param		Float64 in value
	category	drawing-control
	version		1.0
	glxropcode	115
	glsflags	gl-enum
	glsopcode	0x00A6
	wglflags	small-data
	offset		188

TexGendv(coord, pname, params)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		params		Float64 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	116
	glsflags	gl-enum
	glsopcode	0x00A7
	wglflags	small-data
	offset		189

TexGenf(coord, pname, param)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		param		CheckedFloat32 in value
	category	drawing-control
	version		1.0
	glxropcode	117
	glsflags	gl-enum
	glsopcode	0x00A8
	wglflags	small-data
	offset		190

TexGenfv(coord, pname, params)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	118
	glsflags	gl-enum
	glsopcode	0x00A9
	wglflags	small-data
	offset		191

TexGeni(coord, pname, param)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		param		CheckedInt32 in value
	category	drawing-control
	version		1.0
	glxropcode	119
	glsflags	gl-enum
	glsopcode	0x00AA
	wglflags	small-data
	offset		192

TexGeniv(coord, pname, params)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	drawing-control
	version		1.0
	glxropcode	120
	glsflags	gl-enum
	glsopcode	0x00AB
	wglflags	small-data
	offset		193

###############################################################################
#
# feedback commands
#
###############################################################################

FeedbackBuffer(size, type, buffer)
	return		void
	param		size		SizeI in value
	param		type		FeedbackType in value
	param		buffer		FeedbackElement out array [size] retained
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	category	feedback
	version		1.0
	glxsingle	105
	glsflags	client
	glsopcode	0x00AC
	wglflags	client-handcode server-handcode batchable
	offset		194

SelectBuffer(size, buffer)
	return		void
	param		size		SizeI in value
	param		buffer		SelectName out array [size] retained
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	category	feedback
	version		1.0
	glxsingle	106
	glsflags	client
	glsopcode	0x00AD
	wglflags	client-handcode server-handcode batchable
	offset		195

RenderMode(mode)
	return		Int32
	param		mode		RenderingMode in value
	category	feedback
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.0
	glxsingle	107
	glsopcode	0x00AE
	wglflags	client-handcode server-handcode
	offset		196

InitNames()
	return		void
	category	feedback
	version		1.0
	glxropcode	121
	glsopcode	0x00AF
	offset		197

LoadName(name)
	return		void
	param		name		SelectName in value
	category	feedback
	version		1.0
	glxropcode	122
	glsopcode	0x00B0
	offset		198

PassThrough(token)
	return		void
	param		token		FeedbackElement in value
	category	feedback
	version		1.0
	glxropcode	123
	glsopcode	0x00B1
	offset		199

PopName()
	return		void
	category	feedback
	version		1.0
	glxropcode	124
	glsopcode	0x00B2
	offset		200

PushName(name)
	return		void
	param		name		SelectName in value
	category	feedback
	version		1.0
	glxropcode	125
	glsopcode	0x00B3
	offset		201

###############################################################################
#
# framebuf commands
#
###############################################################################

DrawBuffer(mode)
	return		void
	param		mode		DrawBufferMode in value
	category	framebuf
	version		1.0
	glxropcode	126
	glsopcode	0x00B4
	offset		202

Clear(mask)
	return		void
	param		mask		ClearBufferMask in value
	category	framebuf
	version		1.0
	glxropcode	127
	glsopcode	0x00B5
	offset		203

ClearAccum(red, green, blue, alpha)
	return		void
	param		red		Float32 in value
	param		green		Float32 in value
	param		blue		Float32 in value
	param		alpha		Float32 in value
	category	framebuf
	version		1.0
	glxropcode	128
	glsopcode	0x00B6
	offset		204

ClearIndex(c)
	return		void
	param		c		MaskedColorIndexValueF in value
	category	framebuf
	version		1.0
	glxropcode	129
	glsopcode	0x00B7
	offset		205

ClearColor(red, green, blue, alpha)
	return		void
	param		red		ClampedColorF in value
	param		green		ClampedColorF in value
	param		blue		ClampedColorF in value
	param		alpha		ClampedColorF in value
	category	framebuf
	version		1.0
	glxropcode	130
	glsopcode	0x00B8
	offset		206

ClearStencil(s)
	return		void
	param		s		StencilValue in value
	category	framebuf
	version		1.0
	glxropcode	131
	glsopcode	0x00B9
	offset		207

ClearDepth(depth)
	return		void
	param		depth		ClampedFloat64 in value
	category	framebuf
	version		1.0
	glxropcode	132
	glsopcode	0x00BA
	offset		208

StencilMask(mask)
	return		void
	param		mask		MaskedStencilValue in value
	category	framebuf
	version		1.0
	glxropcode	133
	glsopcode	0x00BB
	offset		209

ColorMask(red, green, blue, alpha)
	return		void
	param		red		Boolean in value
	param		green		Boolean in value
	param		blue		Boolean in value
	param		alpha		Boolean in value
	category	framebuf
	version		1.0
	glxropcode	134
	glsopcode	0x00BC
	offset		210

DepthMask(flag)
	return		void
	param		flag		Boolean in value
	category	framebuf
	version		1.0
	glxropcode	135
	glsopcode	0x00BD
	offset		211

IndexMask(mask)
	return		void
	param		mask		MaskedColorIndexValueI in value
	category	framebuf
	version		1.0
	glxropcode	136
	glsopcode	0x00BE
	offset		212

###############################################################################
#
# misc commands
#
###############################################################################

Accum(op, value)
	return		void
	param		op		AccumOp in value
	param		value		CoordF in value
	category	misc
	version		1.0
	glxropcode	137
	glsopcode	0x00BF
	offset		213

Disable(cap)
	return		void
	param		cap		EnableCap in value
	category	misc
	version		1.0
	dlflags		handcode
	glxflags	client-handcode client-intercept
	glxropcode	138
	glsflags	client
	glsopcode	0x00C0
	offset		214

Enable(cap)
	return		void
	param		cap		EnableCap in value
	category	misc
	version		1.0
	dlflags		handcode
	glxflags	client-handcode client-intercept
	glxropcode	139
	glsflags	client
	glsopcode	0x00C1
	offset		215

Finish()
	return		void
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	category	misc
	version		1.0
	glxsingle	108
	glsopcode	0x00C2
	offset		216

Flush()
	return		void
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	category	misc
	version		1.0
	glxsingle	142
	glsopcode	0x00C3
	offset		217

PopAttrib()
	return		void
	category	misc
	version		1.0
	glxropcode	141
	glsopcode	0x00C4
	offset		218

PushAttrib(mask)
	return		void
	param		mask		AttribMask in value
	category	misc
	version		1.0
	glxropcode	142
	glsopcode	0x00C5
	offset		219

###############################################################################
#
# modeling commands
#
###############################################################################

Map1d(target, u1, u2, stride, order, points)
	return		void
	param		target		MapTarget in value
	param		u1		CoordD in value
	param		u2		CoordD in value
	param		stride		Int32 in value
	param		order		CheckedInt32 in value
	param		points		CoordD in array [COMPSIZE(target/stride/order)]
	category	modeling
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	143
	glsflags	capture-handcode
	glsopcode	0x00C6
	wglflags	client-handcode server-handcode
	offset		220

Map1f(target, u1, u2, stride, order, points)
	return		void
	param		target		MapTarget in value
	param		u1		CoordF in value
	param		u2		CoordF in value
	param		stride		Int32 in value
	param		order		CheckedInt32 in value
	param		points		CoordF in array [COMPSIZE(target/stride/order)]
	category	modeling
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	144
	glsflags	capture-handcode
	glsopcode	0x00C7
	wglflags	client-handcode server-handcode
	offset		221

Map2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points)
	return		void
	param		target		MapTarget in value
	param		u1		CoordD in value
	param		u2		CoordD in value
	param		ustride		Int32 in value
	param		uorder		CheckedInt32 in value
	param		v1		CoordD in value
	param		v2		CoordD in value
	param		vstride		Int32 in value
	param		vorder		CheckedInt32 in value
	param		points		CoordD in array [COMPSIZE(target/ustride/uorder/vstride/vorder)]
	category	modeling
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	145
	glsflags	capture-handcode
	glsopcode	0x00C8
	wglflags	client-handcode server-handcode
	offset		222

Map2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points)
	return		void
	param		target		MapTarget in value
	param		u1		CoordF in value
	param		u2		CoordF in value
	param		ustride		Int32 in value
	param		uorder		CheckedInt32 in value
	param		v1		CoordF in value
	param		v2		CoordF in value
	param		vstride		Int32 in value
	param		vorder		CheckedInt32 in value
	param		points		CoordF in array [COMPSIZE(target/ustride/uorder/vstride/vorder)]
	category	modeling
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	146
	glsflags	capture-handcode
	glsopcode	0x00C9
	wglflags	client-handcode server-handcode
	offset		223

MapGrid1d(un, u1, u2)
	return		void
	param		un		Int32 in value
	param		u1		CoordD in value
	param		u2		CoordD in value
	category	modeling
	version		1.0
	glxropcode	147
	glsopcode	0x00CA
	offset		224

MapGrid1f(un, u1, u2)
	return		void
	param		un		Int32 in value
	param		u1		CoordF in value
	param		u2		CoordF in value
	category	modeling
	version		1.0
	glxropcode	148
	glsopcode	0x00CB
	offset		225

MapGrid2d(un, u1, u2, vn, v1, v2)
	return		void
	param		un		Int32 in value
	param		u1		CoordD in value
	param		u2		CoordD in value
	param		vn		Int32 in value
	param		v1		CoordD in value
	param		v2		CoordD in value
	category	modeling
	version		1.0
	glxropcode	149
	glsopcode	0x00CC
	offset		226

MapGrid2f(un, u1, u2, vn, v1, v2)
	return		void
	param		un		Int32 in value
	param		u1		CoordF in value
	param		u2		CoordF in value
	param		vn		Int32 in value
	param		v1		CoordF in value
	param		v2		CoordF in value
	category	modeling
	version		1.0
	glxropcode	150
	glsopcode	0x00CD
	offset		227

EvalCoord1d(u)
	return		void
	param		u		CoordD in value
	category	modeling
	vectorequiv	EvalCoord1dv
	version		1.0
	offset		228

EvalCoord1dv(u)
	return		void
	param		u		CoordD in array [1]
	category	modeling
	version		1.0
	glxropcode	151
	glsopcode	0x00CE
	offset		229

EvalCoord1f(u)
	return		void
	param		u		CoordF in value
	category	modeling
	vectorequiv	EvalCoord1fv
	version		1.0
	offset		230

EvalCoord1fv(u)
	return		void
	param		u		CoordF in array [1]
	category	modeling
	version		1.0
	glxropcode	152
	glsopcode	0x00CF
	offset		231

EvalCoord2d(u, v)
	return		void
	param		u		CoordD in value
	param		v		CoordD in value
	category	modeling
	vectorequiv	EvalCoord2dv
	version		1.0
	offset		232

EvalCoord2dv(u)
	return		void
	param		u		CoordD in array [2]
	category	modeling
	version		1.0
	glxropcode	153
	glsopcode	0x00D0
	offset		233

EvalCoord2f(u, v)
	return		void
	param		u		CoordF in value
	param		v		CoordF in value
	category	modeling
	vectorequiv	EvalCoord2fv
	version		1.0
	offset		234

EvalCoord2fv(u)
	return		void
	param		u		CoordF in array [2]
	category	modeling
	version		1.0
	glxropcode	154
	glsopcode	0x00D1
	offset		235

EvalMesh1(mode, i1, i2)
	return		void
	param		mode		MeshMode1 in value
	param		i1		CheckedInt32 in value
	param		i2		CheckedInt32 in value
	category	modeling
	version		1.0
	glxropcode	155
	glsopcode	0x00D2
	offset		236

EvalPoint1(i)
	return		void
	param		i		Int32 in value
	category	modeling
	version		1.0
	glxropcode	156
	glsopcode	0x00D3
	offset		237

EvalMesh2(mode, i1, i2, j1, j2)
	return		void
	param		mode		MeshMode2 in value
	param		i1		CheckedInt32 in value
	param		i2		CheckedInt32 in value
	param		j1		CheckedInt32 in value
	param		j2		CheckedInt32 in value
	category	modeling
	version		1.0
	glxropcode	157
	glsopcode	0x00D4
	offset		238

EvalPoint2(i, j)
	return		void
	param		i		CheckedInt32 in value
	param		j		CheckedInt32 in value
	category	modeling
	version		1.0
	glxropcode	158
	glsopcode	0x00D5
	offset		239

###############################################################################
#
# pixel-op commands
#
###############################################################################

AlphaFunc(func, ref)
	return		void
	param		func		AlphaFunction in value
	param		ref		ClampedFloat32 in value
	category	pixel-op
	version		1.0
	glxropcode	159
	glsopcode	0x00D6
	offset		240

BlendFunc(sfactor, dfactor)
	return		void
	param		sfactor		BlendingFactorSrc in value
	param		dfactor		BlendingFactorDest in value
	category	pixel-op
	version		1.0
	glxropcode	160
	glsopcode	0x00D7
	offset		241

LogicOp(opcode)
	return		void
	param		opcode		LogicOp in value
	category	pixel-op
	version		1.0
	glxropcode	161
	glsopcode	0x00D8
	offset		242

StencilFunc(func, ref, mask)
	return		void
	param		func		StencilFunction in value
	param		ref		ClampedStencilValue in value
	param		mask		MaskedStencilValue in value
	category	pixel-op
	version		1.0
	glxropcode	162
	glsopcode	0x00D9
	offset		243

StencilOp(fail, zfail, zpass)
	return		void
	param		fail		StencilOp in value
	param		zfail		StencilOp in value
	param		zpass		StencilOp in value
	category	pixel-op
	version		1.0
	glxropcode	163
	glsopcode	0x00DA
	offset		244

DepthFunc(func)
	return		void
	param		func		DepthFunction in value
	category	pixel-op
	version		1.0
	glxropcode	164
	glsopcode	0x00DB
	offset		245

###############################################################################
#
# pixel-rw commands
#
###############################################################################

PixelZoom(xfactor, yfactor)
	return		void
	param		xfactor		Float32 in value
	param		yfactor		Float32 in value
	category	pixel-rw
	version		1.0
	glxropcode	165
	glsopcode	0x00DC
	offset		246

PixelTransferf(pname, param)
	return		void
	param		pname		PixelTransferParameter in value
	param		param		CheckedFloat32 in value
	category	pixel-rw
	version		1.0
	glxropcode	166
	glsflags	gl-enum
	glsopcode	0x00DD
	offset		247

PixelTransferi(pname, param)
	return		void
	param		pname		PixelTransferParameter in value
	param		param		CheckedInt32 in value
	category	pixel-rw
	version		1.0
	glxropcode	167
	glsflags	gl-enum
	glsopcode	0x00DE
	offset		248

PixelStoref(pname, param)
	return		void
	param		pname		PixelStoreParameter in value
	param		param		CheckedFloat32 in value
	dlflags		notlistable
	glxflags	client-handcode
	category	pixel-rw
	version		1.0
	glxsingle	109
	glsflags	client gl-enum
	glsopcode	0x00DF
	wglflags	batchable
	offset		249

PixelStorei(pname, param)
	return		void
	param		pname		PixelStoreParameter in value
	param		param		CheckedInt32 in value
	dlflags		notlistable
	glxflags	client-handcode
	category	pixel-rw
	version		1.0
	glxsingle	110
	glsflags	client gl-enum
	glsopcode	0x00E0
	wglflags	batchable
	offset		250

PixelMapfv(map, mapsize, values)
	return		void
	param		map		PixelMap in value
	param		mapsize		CheckedInt32 in value
	param		values		Float32 in array [mapsize]
	category	pixel-rw
	glxflags	client-handcode
	version		1.0
	glxropcode	168
	glsopcode	0x00E1
	offset		251

PixelMapuiv(map, mapsize, values)
	return		void
	param		map		PixelMap in value
	param		mapsize		CheckedInt32 in value
	param		values		UInt32 in array [mapsize]
	category	pixel-rw
	glxflags	client-handcode
	version		1.0
	glxropcode	169
	glsopcode	0x00E2
	offset		252

PixelMapusv(map, mapsize, values)
	return		void
	param		map		PixelMap in value
	param		mapsize		CheckedInt32 in value
	param		values		UInt16 in array [mapsize]
	category	pixel-rw
	glxflags	client-handcode
	version		1.0
	glxropcode	170
	glsopcode	0x00E3
	offset		253

ReadBuffer(mode)
	return		void
	param		mode		ReadBufferMode in value
	category	pixel-rw
	version		1.0
	glxropcode	171
	glsopcode	0x00E4
	offset		254

CopyPixels(x, y, width, height, type)
	return		void
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		type		PixelCopyType in value
	category	pixel-rw
	version		1.0
	glxropcode	172
	glsopcode	0x00E5
	offset		255

ReadPixels(x, y, width, height, format, type, pixels)
	return		void
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void out array [COMPSIZE(format/type/width/height)]
	category	pixel-rw
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.0
	glxsingle	111
	glsflags	get pixel-pack
	glsopcode	0x00E6
	wglflags	client-handcode server-handcode
	offset		256

DrawPixels(width, height, format, type, pixels)
	return		void
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width/height)]
	category	pixel-rw
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.0
	glxropcode	173
	glsflags	pixel-unpack
	glsopcode	0x00E7
	wglflags	client-handcode server-handcode
	offset		257

###############################################################################
#
# state-req commands
#
###############################################################################

GetBooleanv(pname, params)
	return		void
	param		pname		GetPName in value
	param		params		Boolean out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode
	version		1.0
	glxsingle	112
	glsflags	client get
	glsopcode	0x00E8
	wglflags	small-data
	offset		258

GetClipPlane(plane, equation)
	return		void
	param		plane		ClipPlaneName in value
	param		equation	Float64 out array [4]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	113
	glxflags	client-handcode server-handcode
	glsflags	get
	glsopcode	0x00E9
	offset		259

GetDoublev(pname, params)
	return		void
	param		pname		GetPName in value
	param		params		Float64 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode
	version		1.0
	glxsingle	114
	glsflags	client get
	glsopcode	0x00EA
	wglflags	small-data
	offset		260

GetError()
	return		ErrorCode
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode
	version		1.0
	glxsingle	115
	glsflags	get
	glsopcode	0x00EB
	offset		261

GetFloatv(pname, params)
	return		void
	param		pname		GetPName in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode
	version		1.0
	glxsingle	116
	glsflags	client get
	glsopcode	0x00EC
	wglflags	small-data
	offset		262

GetIntegerv(pname, params)
	return		void
	param		pname		GetPName in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode
	version		1.0
	glxsingle	117
	glsflags	client get
	glsopcode	0x00ED
	wglflags	small-data
	offset		263

GetLightfv(light, pname, params)
	return		void
	param		light		LightName in value
	param		pname		LightParameter in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	118
	glsflags	get
	glsopcode	0x00EE
	wglflags	small-data
	offset		264

GetLightiv(light, pname, params)
	return		void
	param		light		LightName in value
	param		pname		LightParameter in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	119
	glsflags	get
	glsopcode	0x00EF
	wglflags	small-data
	offset		265

GetMapdv(target, query, v)
	return		void
	param		target		MapTarget in value
	param		query		GetMapQuery in value
	param		v		Float64 out array [COMPSIZE(target/query)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	120
	glsflags	get
	glsopcode	0x00F0
	offset		266

GetMapfv(target, query, v)
	return		void
	param		target		MapTarget in value
	param		query		GetMapQuery in value
	param		v		Float32 out array [COMPSIZE(target/query)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	121
	glsflags	get
	glsopcode	0x00F1
	offset		267

GetMapiv(target, query, v)
	return		void
	param		target		MapTarget in value
	param		query		GetMapQuery in value
	param		v		Int32 out array [COMPSIZE(target/query)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	122
	glsflags	get
	glsopcode	0x00F2
	offset		268

GetMaterialfv(face, pname, params)
	return		void
	param		face		MaterialFace in value
	param		pname		MaterialParameter in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	123
	glsflags	get
	glsopcode	0x00F3
	wglflags	small-data
	offset		269

GetMaterialiv(face, pname, params)
	return		void
	param		face		MaterialFace in value
	param		pname		MaterialParameter in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	124
	glsflags	get
	glsopcode	0x00F4
	wglflags	small-data
	offset		270

GetPixelMapfv(map, values)
	return		void
	param		map		PixelMap in value
	param		values		Float32 out array [COMPSIZE(map)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	125
	glsflags	get
	glsopcode	0x00F5
	offset		271

GetPixelMapuiv(map, values)
	return		void
	param		map		PixelMap in value
	param		values		UInt32 out array [COMPSIZE(map)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	126
	glsflags	get
	glsopcode	0x00F6
	offset		272

GetPixelMapusv(map, values)
	return		void
	param		map		PixelMap in value
	param		values		UInt16 out array [COMPSIZE(map)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	127
	glsflags	get
	glsopcode	0x00F7
	offset		273

GetPolygonStipple(mask)
	return		void
	param		mask		UInt8 out array [COMPSIZE()]
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.0
	glxsingle	128
	glsflags	get pixel-pack
	glsopcode	0x00F8
	wglflags	client-handcode server-handcode
	offset		274

GetString(name)
	return		String
	param		name		StringName in value
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.0
	glxsingle	129
	glsflags	get
	glsopcode	0x00F9
	wglflags	client-handcode server-handcode
	offset		275

GetTexEnvfv(target, pname, params)
	return		void
	param		target		TextureEnvTarget in value
	param		pname		TextureEnvParameter in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	130
	glsflags	get
	glsopcode	0x00FA
	wglflags	small-data
	offset		276

GetTexEnviv(target, pname, params)
	return		void
	param		target		TextureEnvTarget in value
	param		pname		TextureEnvParameter in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	131
	glsflags	get
	glsopcode	0x00FB
	wglflags	small-data
	offset		277

GetTexGendv(coord, pname, params)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		params		Float64 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	132
	glsflags	get
	glsopcode	0x00FC
	wglflags	small-data
	offset		278

GetTexGenfv(coord, pname, params)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	133
	glsflags	get
	glsopcode	0x00FD
	wglflags	small-data
	offset		279

GetTexGeniv(coord, pname, params)
	return		void
	param		coord		TextureCoordName in value
	param		pname		TextureGenParameter in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	134
	glsflags	get
	glsopcode	0x00FE
	wglflags	small-data
	offset		280

GetTexImage(target, level, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void out array [COMPSIZE(target/level/format/type)]
	category	state-req
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.0
	glxsingle	135
	glsflags	get pixel-pack
	glsopcode	0x00FF
	wglflags	client-handcode server-handcode
	offset		281

GetTexParameterfv(target, pname, params)
	return		void
	param		target		TextureTarget in value
	param		pname		GetTextureParameter in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	136
	glsflags	get
	glsopcode	0x0100
	wglflags	small-data
	offset		282

GetTexParameteriv(target, pname, params)
	return		void
	param		target		TextureTarget in value
	param		pname		GetTextureParameter in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	137
	glsflags	get
	glsopcode	0x0101
	wglflags	small-data
	offset		283

GetTexLevelParameterfv(target, level, pname, params)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		pname		GetTextureParameter in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	138
	glsflags	get
	glsopcode	0x0102
	wglflags	small-data
	offset		284

GetTexLevelParameteriv(target, level, pname, params)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		pname		GetTextureParameter in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	139
	glsflags	get
	glsopcode	0x0103
	wglflags	small-data
	offset		285

IsEnabled(cap)
	return		Boolean
	param		cap		EnableCap in value
	category	state-req
	dlflags		notlistable
	version		1.0
	glxflags	client-handcode client-intercept
	glxsingle	140
	glsflags	client get
	glsopcode	0x0104
	offset		286

IsList(list)
	return		Boolean
	param		list		List in value
	category	state-req
	dlflags		notlistable
	version		1.0
	glxsingle	141
	glsflags	get
	glsopcode	0x0105
	offset		287

###############################################################################
#
# xform commands
#
###############################################################################

DepthRange(near, far)
	return		void
	param		near		ClampedFloat64 in value
	param		far		ClampedFloat64 in value
	category	xform
	version		1.0
	glxropcode	174
	glsopcode	0x0106
	offset		288

Frustum(left, right, bottom, top, zNear, zFar)
	return		void
	param		left		Float64 in value
	param		right		Float64 in value
	param		bottom		Float64 in value
	param		top		Float64 in value
	param		zNear		Float64 in value
	param		zFar		Float64 in value
	category	xform
	version		1.0
	glxropcode	175
	glsopcode	0x0107
	offset		289

LoadIdentity()
	return		void
	category	xform
	version		1.0
	glxropcode	176
	glsopcode	0x0108
	offset		290

LoadMatrixf(m)
	return		void
	param		m		Float32 in array [16]
	category	xform
	version		1.0
	glxropcode	177
	glsflags	matrix
	glsopcode	0x0109
	offset		291

LoadMatrixd(m)
	return		void
	param		m		Float64 in array [16]
	category	xform
	version		1.0
	glxropcode	178
	glsflags	matrix
	glsopcode	0x010A
	offset		292

MatrixMode(mode)
	return		void
	param		mode		MatrixMode in value
	category	xform
	version		1.0
	glxropcode	179
	glsopcode	0x010B
	offset		293

MultMatrixf(m)
	return		void
	param		m		Float32 in array [16]
	category	xform
	version		1.0
	glxropcode	180
	glsflags	matrix
	glsopcode	0x010C
	offset		294

MultMatrixd(m)
	return		void
	param		m		Float64 in array [16]
	category	xform
	version		1.0
	glxropcode	181
	glsflags	matrix
	glsopcode	0x010D
	offset		295

Ortho(left, right, bottom, top, zNear, zFar)
	return		void
	param		left		Float64 in value
	param		right		Float64 in value
	param		bottom		Float64 in value
	param		top		Float64 in value
	param		zNear		Float64 in value
	param		zFar		Float64 in value
	category	xform
	version		1.0
	glxropcode	182
	glsopcode	0x010E
	offset		296

PopMatrix()
	return		void
	category	xform
	version		1.0
	glxropcode	183
	glsopcode	0x010F
	offset		297

PushMatrix()
	return		void
	category	xform
	version		1.0
	glxropcode	184
	glsopcode	0x0110
	offset		298

Rotated(angle, x, y, z)
	return		void
	param		angle		Float64 in value
	param		x		Float64 in value
	param		y		Float64 in value
	param		z		Float64 in value
	category	xform
	version		1.0
	glxropcode	185
	glsopcode	0x0111
	offset		299

Rotatef(angle, x, y, z)
	return		void
	param		angle		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	xform
	version		1.0
	glxropcode	186
	glsopcode	0x0112
	offset		300

Scaled(x, y, z)
	return		void
	param		x		Float64 in value
	param		y		Float64 in value
	param		z		Float64 in value
	category	xform
	version		1.0
	glxropcode	187
	glsopcode	0x0113
	offset		301

Scalef(x, y, z)
	return		void
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	xform
	version		1.0
	glxropcode	188
	glsopcode	0x0114
	offset		302

Translated(x, y, z)
	return		void
	param		x		Float64 in value
	param		y		Float64 in value
	param		z		Float64 in value
	category	xform
	version		1.0
	glxropcode	189
	glsopcode	0x0115
	offset		303

Translatef(x, y, z)
	return		void
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	xform
	version		1.0
	glxropcode	190
	glsopcode	0x0116
	offset		304

Viewport(x, y, width, height)
	return		void
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	category	xform
	version		1.0
	glxropcode	191
	glsopcode	0x0117
	offset		305

###############################################################################
#
# OpenGL 1.1 commands
#
###############################################################################

ArrayElement(i)
	return		void
	param		i		Int32 in value
	category	1_1
	dlflags		handcode
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsopcode	?
	offset		306

ColorPointer(size, type, stride, pointer)
	return		void
	param		size		Int32 in value
	param		type		ColorPointerType in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(size/type/stride)] retained
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x0152
	offset		308

DisableClientState(array)
	return		void
	param		array		EnableCap in value
	category	1_1
	version		1.1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	glsflags	client
	glsopcode	0x0153
	offset		309

DrawArrays(mode, first, count)
	return		void
	param		mode		BeginMode in value
	param		first		Int32 in value
	param		count		SizeI in value
	category	1_1
	dlflags		handcode
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glxropcode	193
	glsopcode	?
	offset		310

DrawElements(mode, count, type, indices)
	return		void
	param		mode		BeginMode in value
	param		count		SizeI in value
	param		type		DrawElementsType in value
	param		indices		Void in array [COMPSIZE(count/type)]
	category	1_1
	dlflags		handcode
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsopcode	0x0154
	offset		311

EdgeFlagPointer(stride, pointer)
	return		void
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(stride)] retained
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x0155
	offset		312

EnableClientState(array)
	return		void
	param		array		EnableCap in value
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x0156
	offset		313

GetPointerv(pname, params)
	return		void
	param		pname		GetPointervPName in value
	param		params		VoidPointer out array [1]
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsopcode	?
	offset		329

IndexPointer(type, stride, pointer)
	return		void
	param		type		IndexPointerType in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(type/stride)] retained
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x0157
	offset		314

InterleavedArrays(format, stride, pointer)
	return		void
	param		format		InterleavedArrayFormat in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(format/stride)] retained
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x0158
	offset		317

NormalPointer(type, stride, pointer)
	return		void
	param		type		NormalPointerType in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(type/stride)] retained
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x0159
	offset		318

TexCoordPointer(size, type, stride, pointer)
	return		void
	param		size		Int32 in value
	param		type		TexCoordPointerType in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(size/type/stride)] retained
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x015A
	offset		320

VertexPointer(size, type, stride, pointer)
	return		void
	param		size		Int32 in value
	param		type		VertexPointerType in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(size/type/stride)] retained
	category	1_1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsflags	client
	glsopcode	0x015B
	offset		321

PolygonOffset(factor, units)
	return		void
	param		factor		Float32 in value
	param		units		Float32 in value
	category	1_1
	version		1.1
	glxropcode	192
	glsopcode	0x015C
	offset		319

# Arguably TexelInternalFormat, not PixelInternalFormat
CopyTexImage1D(target, level, internalformat, x, y, width, border)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		border		CheckedInt32 in value
	category	1_1
	version		1.1
	glxropcode	4119
	glxflags	EXT
	glsopcode	?
	offset		323

# Arguably TexelInternalFormat, not PixelInternalFormat
CopyTexImage2D(target, level, internalformat, x, y, width, height, border)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		border		CheckedInt32 in value
	category	1_1
	version		1.1
	glxropcode	4120
	glxflags	EXT
	glsopcode	?
	offset		324

CopyTexSubImage1D(target, level, xoffset, x, y, width)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	category	1_1
	version		1.1
	glxropcode	4121
	glxflags	EXT
	glsopcode	?
	offset		325

CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		yoffset		CheckedInt32 in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	category	1_1
	version		1.1
	glxropcode	4122
	glxflags	EXT
	glsopcode	?
	offset		326

TexSubImage1D(target, level, xoffset, width, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		width		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width)]
	category	1_1
	dlflags		handcode
	glxflags	EXT client-handcode server-handcode
	version		1.1
	glxropcode	4099
	glsopcode	?
	offset		332

TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		yoffset		CheckedInt32 in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width/height)]
	category	1_1
	dlflags		handcode
	glxflags	EXT client-handcode server-handcode
	version		1.1
	glxropcode	4100
	glsopcode	?
	offset		333

AreTexturesResident(n, textures, residences)
	return		Boolean
	param		n		SizeI in value
	param		textures	Texture in array [n]
	param		residences	Boolean out array [n]
	category	1_1
	glxsingle	143
	dlflags		notlistable
	version		1.1
	glsopcode	?
	offset		322

BindTexture(target, texture)
	return		void
	param		target		TextureTarget in value
	param		texture		Texture in value
	category	1_1
	version		1.1
	glxropcode	4117
	glxflags	EXT
	glsopcode	?
	offset		307

DeleteTextures(n, textures)
	return		void
	param		n		SizeI in value
	param		textures	Texture in array [n]
	category	1_1
	dlflags		notlistable
	version		1.1
	glxsingle	144
	glsopcode	?
	offset		327

GenTextures(n, textures)
	return		void
	param		n		SizeI in value
	param		textures	Texture out array [n]
	category	1_1
	dlflags		notlistable
	version		1.1
	glxsingle	145
	glsopcode	?
	offset		328

IsTexture(texture)
	return		Boolean
	param		texture		Texture in value
	category	1_1
	dlflags		notlistable
	version		1.1
	glxsingle	146
	glsopcode	?
	offset		330

PrioritizeTextures(n, textures, priorities)
	return		void
	param		n		SizeI in value
	param		textures	Texture in array [n]
	param		priorities	ClampedFloat32 in array [n]
	category	1_1
	version		1.1
	glxropcode	4118
	glxflags	EXT
	glsopcode	?
	offset		331

Indexub(c)
	return		void
	param		c		ColorIndexValueUB in value
	category	1_1
	vectorequiv	Indexubv
	version		1.1
	offset		315

Indexubv(c)
	return		void
	param		c		ColorIndexValueUB in array [1]
	category	1_1
	version		1.1
	glxropcode	194
	glsopcode	0x015D
	offset		316

PopClientAttrib()
	return		void
	category	1_1
	version		1.1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	glsflags	client
	glsopcode	0x015E
	offset		334

PushClientAttrib(mask)
	return		void
	param		mask		ClientAttribMask in value
	category	1_1
	version		1.1
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	glsflags	client
	glsopcode	0x015F
	offset		335

###############################################################################
#
# OpenGL 1.2 commands
#
###############################################################################

BlendColor(red, green, blue, alpha)
	return		void
	param		red		ClampedColorF in value
	param		green		ClampedColorF in value
	param		blue		ClampedColorF in value
	param		alpha		ClampedColorF in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4096
	glsopcode	?
	offset		336

BlendEquation(mode)
	return		void
	param		mode		BlendEquationMode in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4097
	glsopcode	?
	offset		337

DrawRangeElements(mode, start, end, count, type, indices)
	return		void
	param		mode		BeginMode in value
	param		start		UInt32 in value
	param		end		UInt32 in value
	param		count		SizeI in value
	param		type		DrawElementsType in value
	param		indices		Void in array [COMPSIZE(count/type)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode client-intercept server-handcode
	version		1.2
	glsopcode	0x0190
	offset		338

################################################################################
#
# color_table extension commands
#
###############################################################################

ColorTable(target, internalformat, width, format, type, table)
	return		void
	param		target		ColorTableTarget in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		table		Void in array [COMPSIZE(format/type/width)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.2
	glxropcode	2053
	glsopcode	?
	offset		339

ColorTableParameterfv(target, pname, params)
	return		void
	param		target		ColorTableTarget in value
	param		pname		ColorTableParameterPName in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	2054
	glsopcode	?
	offset		340

ColorTableParameteriv(target, pname, params)
	return		void
	param		target		ColorTableTarget in value
	param		pname		ColorTableParameterPName in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	2055
	glsopcode	?
	offset		341

CopyColorTable(target, internalformat, x, y, width)
	return		void
	param		target		ColorTableTarget in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	2056
	glsopcode	?
	offset		342

GetColorTable(target, format, type, table)
	return		void
	param		target		ColorTableTarget in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		table		Void out array [COMPSIZE(target/format/type)]
	category	VERSION_1_2
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.2
	glxsingle	147
	glsopcode	?
	offset		343

GetColorTableParameterfv(target, pname, params)
	return		void
	param		target		ColorTableTarget in value
	param		pname		GetColorTableParameterPName in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	148
	glsopcode	?
	offset		344

GetColorTableParameteriv(target, pname, params)
	return		void
	param		target		ColorTableTarget in value
	param		pname		GetColorTableParameterPName in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	149
	glsopcode	?
	offset		345

################################################################################
#
#  color_subtable extension commands
#
###############################################################################

ColorSubTable(target, start, count, format, type, data)
	return		void
	param		target		ColorTableTarget in value
	param		start		SizeI in value
	param		count		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		data		Void in array [COMPSIZE(format/type/count)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode server-handcode
	version		1.2
	glxropcode	195
	glsflags	pixel-unpack
	glsopcode	0x018E
	offset		346

CopyColorSubTable(target, start, x, y, width)
	return		void
	param		target		ColorTableTarget in value
	param		start		SizeI in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	category	VERSION_1_2
	version		1.2
	glxropcode	196
	glsopcode	0x018F
	offset		347

################################################################################
#
#  convolution extension commands
#
###############################################################################

ConvolutionFilter1D(target, internalformat, width, format, type, image)
	return		void
	param		target		ConvolutionTarget in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		image		Void in array [COMPSIZE(format/type/width)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.2
	glxropcode	4101
	glsopcode	?
	offset		348

ConvolutionFilter2D(target, internalformat, width, height, format, type, image)
	return		void
	param		target		ConvolutionTarget in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		image		Void in array [COMPSIZE(format/type/width/height)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.2
	glxropcode	4102
	glsopcode	?
	offset		349

ConvolutionParameterf(target, pname, params)
	return		void
	param		target		ConvolutionTarget in value
	param		pname		ConvolutionParameter in value
	param		params		CheckedFloat32 in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4103
	glsopcode	?
	offset		350

ConvolutionParameterfv(target, pname, params)
	return		void
	param		target		ConvolutionTarget in value
	param		pname		ConvolutionParameter in value
	param		params		CheckedFloat32 in array [COMPSIZE(pname)]
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4104
	glsopcode	?
	offset		351

ConvolutionParameteri(target, pname, params)
	return		void
	param		target		ConvolutionTarget in value
	param		pname		ConvolutionParameter in value
	param		params		CheckedInt32 in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4105
	glsopcode	?
	offset		352

ConvolutionParameteriv(target, pname, params)
	return		void
	param		target		ConvolutionTarget in value
	param		pname		ConvolutionParameter in value
	param		params		CheckedInt32 in array [COMPSIZE(pname)]
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4106
	glsopcode	?
	offset		353

CopyConvolutionFilter1D(target, internalformat, x, y, width)
	return		void
	param		target		ConvolutionTarget in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4107
	glsopcode	?
	offset		354

CopyConvolutionFilter2D(target, internalformat, x, y, width, height)
	return		void
	param		target		ConvolutionTarget in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4108
	glsopcode	?
	offset		355

GetConvolutionFilter(target, format, type, image)
	return		void
	param		target		ConvolutionTarget in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		image		Void out array [COMPSIZE(target/format/type)]
	category	VERSION_1_2
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.2
	glxsingle	150
	glsopcode	?
	offset		356

GetConvolutionParameterfv(target, pname, params)
	return		void
	param		target		ConvolutionTarget in value
	param		pname		GetConvolutionParameterPName in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	151
	glsopcode	?
	offset		357

GetConvolutionParameteriv(target, pname, params)
	return		void
	param		target		ConvolutionTarget in value
	param		pname		GetConvolutionParameterPName in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	152
	glsopcode	?
	offset		358

GetSeparableFilter(target, format, type, row, column, span)
	return		void
	param		target		SeparableTarget in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		row		Void out array [COMPSIZE(target/format/type)]
	param		column		Void out array [COMPSIZE(target/format/type)]
	param		span		Void out array [COMPSIZE(target/format/type)]
	category	VERSION_1_2
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.2
	glxsingle	153
	glsopcode	?
	offset		359

SeparableFilter2D(target, internalformat, width, height, format, type, row, column)
	return		void
	param		target		SeparableTarget in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		row		Void in array [COMPSIZE(target/format/type/width)]
	param		column		Void in array [COMPSIZE(target/format/type/height)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.2
	glxropcode	4109
	glsopcode	?
	offset		360

################################################################################
#
#  histogram extension commands
#
###############################################################################

GetHistogram(target, reset, format, type, values)
	return		void
	param		target		HistogramTarget in value
	param		reset		Boolean in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		values		Void out array [COMPSIZE(target/format/type)]
	category	VERSION_1_2
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.2
	glxsingle	154
	glsopcode	?
	offset		361

GetHistogramParameterfv(target, pname, params)
	return		void
	param		target		HistogramTarget in value
	param		pname		GetHistogramParameterPName in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	155
	glsopcode	?
	offset		362

GetHistogramParameteriv(target, pname, params)
	return		void
	param		target		HistogramTarget in value
	param		pname		GetHistogramParameterPName in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	156
	glsopcode	?
	offset		363

GetMinmax(target, reset, format, type, values)
	return		void
	param		target		MinmaxTarget in value
	param		reset		Boolean in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		values		Void out array [COMPSIZE(target/format/type)]
	category	VERSION_1_2
	dlflags		notlistable
	glxflags	client-handcode server-handcode
	version		1.2
	glxsingle	157
	glsopcode	?
	offset		364

GetMinmaxParameterfv(target, pname, params)
	return		void
	param		target		MinmaxTarget in value
	param		pname		GetMinmaxParameterPName in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	158
	glsopcode	?
	offset		365

GetMinmaxParameteriv(target, pname, params)
	return		void
	param		target		MinmaxTarget in value
	param		pname		GetMinmaxParameterPName in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	VERSION_1_2
	dlflags		notlistable
	version		1.2
	glxsingle	159
	glsopcode	?
	offset		366

Histogram(target, width, internalformat, sink)
	return		void
	param		target		HistogramTarget in value
	param		width		SizeI in value
	param		internalformat	PixelInternalFormat in value
	param		sink		Boolean in value
	category	VERSION_1_2
	dlflags		handcode
	glxflags	EXT
	version		1.2
	glxropcode	4110
	glsopcode	?
	offset		367

Minmax(target, internalformat, sink)
	return		void
	param		target		MinmaxTarget in value
	param		internalformat	PixelInternalFormat in value
	param		sink		Boolean in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4111
	glsopcode	?
	offset		368

ResetHistogram(target)
	return		void
	param		target		HistogramTarget in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4112
	glsopcode	?
	offset		369

ResetMinmax(target)
	return		void
	param		target		MinmaxTarget in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4113
	glsopcode	?
	offset		370

################################################################################
#
#  texture3D extension commands
#
###############################################################################

# Arguably TexelInternalFormat, not PixelInternalFormat
TexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	TextureComponentCount in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		depth		SizeI in value
	param		border		CheckedInt32 in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width/height/depth)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.2
	glxropcode	4114
	glsopcode	?
	offset		371

TexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		yoffset		CheckedInt32 in value
	param		zoffset		CheckedInt32 in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		depth		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width/height/depth)]
	category	VERSION_1_2
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.2
	glxropcode	4115
	glsopcode	?
	offset		372

################################################################################
#
#   copy_texture extension commands (specific to texture3D)
#
###############################################################################

CopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		yoffset		CheckedInt32 in value
	param		zoffset		CheckedInt32 in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	category	VERSION_1_2
	glxflags	EXT
	version		1.2
	glxropcode	4123
	glsopcode	?
	offset		373

###############################################################################
###############################################################################
#
# ARB extensions, in order by ARB extension number
#
###############################################################################
###############################################################################

################################################################################
#
# ARB Extension #1
# ARB_multitexture commands
#
###############################################################################

ActiveTextureARB(texture)
	return		void
	param		texture		TextureUnit in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	197
	glsopcode	?
	offset		374

ClientActiveTextureARB(texture)
	return		void
	param		texture		TextureUnit in value
	category	ARB_multitexture
	dlflags		notlistable
	glxflags	ARB client-handcode client-intercept server-handcode
	version		1.2
	glsopcode	?
	offset		375

MultiTexCoord1dARB(target, s)
	return		void
	param		target		TextureUnit in value
	param		s		CoordD in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord1dvARB
	glsopcode	?
	offset		376

MultiTexCoord1dvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordD in array [1]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	198
	glsopcode	?
	offset		377

MultiTexCoord1fARB(target, s)
	return		void
	param		target		TextureUnit in value
	param		s		CoordF in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord1fvARB
	glsopcode	?
	offset		378

MultiTexCoord1fvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordF in array [1]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	199
	glsopcode	?
	offset		379

MultiTexCoord1iARB(target, s)
	return		void
	param		target		TextureUnit in value
	param		s		CoordI in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord1ivARB
	glsopcode	?
	offset		380

MultiTexCoord1ivARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordI in array [1]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	200
	glsopcode	?
	offset		381

MultiTexCoord1sARB(target, s)
	return		void
	param		target		TextureUnit in value
	param		s		CoordS in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord1svARB
	glsopcode	?
	offset		382

MultiTexCoord1svARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordS in array [1]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	201
	glsopcode	?
	offset		383

MultiTexCoord2dARB(target, s, t)
	return		void
	param		target		TextureUnit in value
	param		s		CoordD in value
	param		t		CoordD in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord2dvARB
	glsopcode	?
	offset		384

MultiTexCoord2dvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordD in array [2]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	202
	glsopcode	?
	offset		385

MultiTexCoord2fARB(target, s, t)
	return		void
	param		target		TextureUnit in value
	param		s		CoordF in value
	param		t		CoordF in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord2fvARB
	glsopcode	?
	offset		386

MultiTexCoord2fvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordF in array [2]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	203
	glsopcode	?
	offset		387

MultiTexCoord2iARB(target, s, t)
	return		void
	param		target		TextureUnit in value
	param		s		CoordI in value
	param		t		CoordI in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord2ivARB
	glsopcode	?
	offset		388

MultiTexCoord2ivARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordI in array [2]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	204
	glsopcode	?
	offset		389

MultiTexCoord2sARB(target, s, t)
	return		void
	param		target		TextureUnit in value
	param		s		CoordS in value
	param		t		CoordS in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord2svARB
	glsopcode	?
	offset		390

MultiTexCoord2svARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordS in array [2]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	205
	glsopcode	?
	offset		391

MultiTexCoord3dARB(target, s, t, r)
	return		void
	param		target		TextureUnit in value
	param		s		CoordD in value
	param		t		CoordD in value
	param		r		CoordD in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord3dvARB
	glsopcode	?
	offset		392

MultiTexCoord3dvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordD in array [3]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	206
	glsopcode	?
	offset		393

MultiTexCoord3fARB(target, s, t, r)
	return		void
	param		target		TextureUnit in value
	param		s		CoordF in value
	param		t		CoordF in value
	param		r		CoordF in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord3fvARB
	glsopcode	?
	offset		394

MultiTexCoord3fvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordF in array [3]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	207
	glsopcode	?
	offset		395

MultiTexCoord3iARB(target, s, t, r)
	return		void
	param		target		TextureUnit in value
	param		s		CoordI in value
	param		t		CoordI in value
	param		r		CoordI in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord3ivARB
	glsopcode	?
	offset		396

MultiTexCoord3ivARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordI in array [3]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	208
	glsopcode	?
	offset		397

MultiTexCoord3sARB(target, s, t, r)
	return		void
	param		target		TextureUnit in value
	param		s		CoordS in value
	param		t		CoordS in value
	param		r		CoordS in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord3svARB
	glsopcode	?
	offset		398

MultiTexCoord3svARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordS in array [3]
	category	ARB_multitexture
	version		1.2
	glxflags	ARB
	glxropcode	209
	glsopcode	?
	offset		399

MultiTexCoord4dARB(target, s, t, r, q)
	return		void
	param		target		TextureUnit in value
	param		s		CoordD in value
	param		t		CoordD in value
	param		r		CoordD in value
	param		q		CoordD in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord4dvARB
	glsopcode	?
	offset		400

MultiTexCoord4dvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordD in array [4]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	210
	glsopcode	?
	offset		401

MultiTexCoord4fARB(target, s, t, r, q)
	return		void
	param		target		TextureUnit in value
	param		s		CoordF in value
	param		t		CoordF in value
	param		r		CoordF in value
	param		q		CoordF in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord4fvARB
	glsopcode	?
	offset		402

MultiTexCoord4fvARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordF in array [4]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	211
	glsopcode	?
	offset		403

MultiTexCoord4iARB(target, s, t, r, q)
	return		void
	param		target		TextureUnit in value
	param		s		CoordI in value
	param		t		CoordI in value
	param		r		CoordI in value
	param		q		CoordI in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord4ivARB
	glsopcode	?
	offset		404

MultiTexCoord4ivARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordI in array [4]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	212
	glsopcode	?
	offset		405

MultiTexCoord4sARB(target, s, t, r, q)
	return		void
	param		target		TextureUnit in value
	param		s		CoordS in value
	param		t		CoordS in value
	param		r		CoordS in value
	param		q		CoordS in value
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	vectorequiv	MultiTexCoord4svARB
	glsopcode	?
	offset		406

MultiTexCoord4svARB(target, v)
	return		void
	param		target		TextureUnit in value
	param		v		CoordS in array [4]
	category	ARB_multitexture
	glxflags	ARB
	version		1.2
	glxropcode	213
	glsopcode	?
	offset		407

################################################################################
#
# ARB Extension #2 - GLX_ARB_get_proc_address
#
###############################################################################

################################################################################
#
# ARB extension #3
# ARB_transpose_matrix commands
#
###############################################################################

LoadTransposeMatrixfARB(m)
	return		void
	param		m		Float32 in array [16]
	category	ARB_transpose_matrix
	glxflags	ARB client-handcode client-intercept server-handcode
	version		1.2
	glsflags	matrix
	glsopcode	?
	offset		408

LoadTransposeMatrixdARB(m)
	return		void
	param		m		Float64 in array [16]
	category	ARB_transpose_matrix
	glxflags	ARB client-handcode client-intercept server-handcode
	version		1.2
	glsflags	matrix
	glsopcode	?
	offset		409

MultTransposeMatrixfARB(m)
	return		void
	param		m		Float32 in array [16]
	category	ARB_transpose_matrix
	glxflags	ARB client-handcode client-intercept server-handcode
	version		1.2
	glsflags	matrix
	glsopcode	?
	offset		410

MultTransposeMatrixdARB(m)
	return		void
	param		m		Float64 in array [16]
	category	ARB_transpose_matrix
	glxflags	ARB client-handcode client-intercept server-handcode
	version		1.2
	glsflags	matrix
	glsopcode	?
	offset		411

################################################################################
#
# ARB Extension #4 - WGL_ARB_buffer_region
#
###############################################################################

################################################################################
#
# ARB extension #5
# ARB_multisample commands
#
###############################################################################

SampleCoverageARB(value, invert)
	return		void
	param		value		ClampedFloat32 in value
	param		invert		Boolean in value
	category	ARB_multisample
	glxflags	ARB
	version		1.2
	glxropcode	?
	glsopcode	?
	offset		412

SamplePassARB(pass)
	return		void
	param		pass		SamplePassARB in value
	category	ARB_multisample
	glxflags	ARB
	version		1.2
	glxropcode	?
	glsopcode	?
	offset		413

################################################################################
#
# ARB extension #6
# ARB_texture_env_add commands
#
###############################################################################

# (none)
newcategory: ARB_texture_env_add

################################################################################
#
# ARB extension #7
# ARB_texture_cube_map commands
#
###############################################################################

# (none)
newcategory: ARB_texture_cube_map

################################################################################
#
# ARB extension #8 - WGL_ARB_extensions_string
# ARB extension #9 - WGL_ARB_pixel_format commands
# ARB extension #10 - WGL_ARB_make_current_read commands
# ARB extension #11 - WGL_ARB_pbuffer
#
###############################################################################

################################################################################
#
# ARB extension #12
# ARB_texture_compression commands
#
###############################################################################

# Arguably TexelInternalFormat, not PixelInternalFormat
CompressedTexImage3DARB(target, level, internalformat, width, height, depth, border, imageSize, data)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		depth		SizeI in value
	param		border		CheckedInt32 in value
	param		imageSize	SizeI in value
	param		data		Void in array [COMPSIZE()]
	category	ARB_texture_compression
	dlflags		handcode
	glxflags	ARB client-handcode server-handcode
	version		1.2
	glxropcode	216
	glsflags	pixel-null pixel-unpack
	glsopcode	?
	wglflags	client-handcode server-handcode
	offset		554

# Arguably TexelInternalFormat, not PixelInternalFormat
CompressedTexImage2DARB(target, level, internalformat, width, height, border, imageSize, data)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		border		CheckedInt32 in value
	param		imageSize	SizeI in value
	param		data		Void in array [COMPSIZE()]
	category	ARB_texture_compression
	dlflags		handcode
	glxflags	ARB client-handcode server-handcode
	version		1.2
	glxropcode	215
	glsflags	pixel-null pixel-unpack
	glsopcode	?
	wglflags	client-handcode server-handcode
	offset		555

# Arguably TexelInternalFormat, not PixelInternalFormat
CompressedTexImage1DARB(target, level, internalformat, width, border, imageSize, data)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		border		CheckedInt32 in value
	param		imageSize	SizeI in value
	param		data		Void in array [COMPSIZE()]
	category	ARB_texture_compression
	dlflags		handcode
	glxflags	ARB client-handcode server-handcode
	version		1.2
	glxropcode	214
	glsflags	pixel-null pixel-unpack
	glsopcode	?
	wglflags	client-handcode server-handcode
	offset		556

CompressedTexSubImage3DARB(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		yoffset		CheckedInt32 in value
	param		zoffset		CheckedInt32 in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		depth		SizeI in value
	param		format		PixelFormat in value
	param		imageSize	SizeI in value
	param		data		Void in array [COMPSIZE()]
	category	ARB_texture_compression
	dlflags		handcode
	glxflags	ARB client-handcode server-handcode
	version		1.2
	glxropcode	219
	glsflags	pixel-null pixel-unpack
	glsopcode	?
	wglflags	client-handcode server-handcode
	offset		557

CompressedTexSubImage2DARB(target, level, xoffset, yoffset, width, height, format, imageSize, data)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		yoffset		CheckedInt32 in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		imageSize	SizeI in value
	param		data		Void in array [COMPSIZE()]
	category	ARB_texture_compression
	dlflags		handcode
	glxflags	ARB client-handcode server-handcode
	version		1.2
	glxropcode	218
	glsflags	pixel-null pixel-unpack
	glsopcode	?
	wglflags	client-handcode server-handcode
	offset		558

CompressedTexSubImage1DARB(target, level, xoffset, width, format, imageSize, data)
	return		void
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		xoffset		CheckedInt32 in value
	param		width		SizeI in value
	param		format		PixelFormat in value
	param		imageSize	SizeI in value
	param		data		Void in array [COMPSIZE()]
	category	ARB_texture_compression
	dlflags		handcode
	glxflags	ARB client-handcode server-handcode
	version		1.2
	glxropcode	217
	glsflags	pixel-null pixel-unpack
	glsopcode	?
	wglflags	client-handcode server-handcode
	offset		559

GetCompressedTexImageARB(target, level, img)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		img		void out array [COMPSIZE(target/level)]
	category	ARB_texture_compression
	dlflags		notlistable
	glxflags	ARB client-handcode server-handcode
	version		1.2
	glxsingle	160
	glsflags	get pixel-pack
	glsopcode	?
	wglflags	client-handcode server-handcode
	offset		560

###############################################################################
###############################################################################
#
# Non-ARB extensions, in order by registry extension number
#
###############################################################################
###############################################################################

###############################################################################
#
# Extension #1
# EXT_abgr commands
#
###############################################################################

# (none)
newcategory: EXT_abgr

###############################################################################
#
# Extension #2
# EXT_blend_color commands
#
###############################################################################

BlendColorEXT(red, green, blue, alpha)
	return		void
	param		red	ClampedColorF in value
	param		green	ClampedColorF in value
	param		blue	ClampedColorF in value
	param		alpha	ClampedColorF in value
	category	EXT_blend_color
	version		1.0
	glxropcode	4096
	glxflags	EXT
	extension	soft
	glsopcode	0x0120
	alias		BlendColor

###############################################################################
#
# Extension #3
# EXT_polygon_offset commands
#
###############################################################################

PolygonOffsetEXT(factor, bias)
	return		void
	param		factor	Float32 in value
	param		bias	Float32 in value
	category	EXT_polygon_offset
	version		1.0
	glxropcode	4098
	glxflags	EXT
	extension	soft
	glsopcode	0x0122
	offset		414

###############################################################################
#
# Extension #4
# EXT_texture commands
#
###############################################################################

# (none)
newcategory: EXT_texture

###############################################################################
#
# Extension #5 - skipped
#
###############################################################################

###############################################################################
#
# Extension #6
# EXT_texture3D commands
#
###############################################################################

# Arguably TexelInternalFormat, not PixelInternalFormat
TexImage3DEXT(target, level, internalformat, width, height, depth, border, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		depth		SizeI in value
	param		border		CheckedInt32 in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width/height/depth)]
	category	EXT_texture3D
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4114
	extension
	glsflags	pixel-null pixel-unpack
	glsopcode	0x013C
	alias		TexImage3D

TexSubImage3DEXT(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels)
	return		void
	param		target	TextureTarget in value
	param		level	CheckedInt32 in value
	param		xoffset CheckedInt32 in value
	param		yoffset CheckedInt32 in value
	param		zoffset CheckedInt32 in value
	param		width	SizeI in value
	param		height	SizeI in value
	param		depth	SizeI in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		pixels	Void in array [COMPSIZE(format/type/width/height/depth)]
	category	EXT_subtexture
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4115
	extension
	glsflags	pixel-unpack
	glsopcode	0x013D
	alias		TexSubImage3D

###############################################################################
#
# Extension #7
# SGIS_texture_filter4 commands
#
###############################################################################

GetTexFilterFuncSGIS(target, filter, weights)
	return		void
	param		target	TextureTarget in value
	param		filter	TextureFilterSGIS in value
	param		weights Float32 out array [COMPSIZE(target/filter)]
	category	SGIS_texture_filter4
	dlflags		notlistable
	version		1.0
	glxflags	SGI
	glxvendorpriv	4101
	extension
	glsflags	get
	glsopcode	0x0175
	offset		415

TexFilterFuncSGIS(target, filter, n, weights)
	return		void
	param		target	TextureTarget in value
	param		filter	TextureFilterSGIS in value
	param		n	SizeI in value
	param		weights Float32 in array [n]
	category	SGIS_texture_filter4
	glxflags	SGI
	version		1.0
	glxropcode	2064
	extension
	glsopcode	0x0176
	offset		416

###############################################################################
#
# Extension #8 - skipped
#
###############################################################################

###############################################################################
#
# Extension #9
# EXT_subtexture commands
#
###############################################################################

TexSubImage1DEXT(target, level, xoffset, width, format, type, pixels)
	return		void
	param		target	TextureTarget in value
	param		level	CheckedInt32 in value
	param		xoffset CheckedInt32 in value
	param		width	SizeI in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		pixels	Void in array [COMPSIZE(format/type/width)]
	category	EXT_subtexture
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4099
	extension
	glsflags	pixel-unpack
	glsopcode	0x0123
	alias		TexSubImage1D

TexSubImage2DEXT(target, level, xoffset, yoffset, width, height, format, type, pixels)
	return		void
	param		target	TextureTarget in value
	param		level	CheckedInt32 in value
	param		xoffset CheckedInt32 in value
	param		yoffset CheckedInt32 in value
	param		width	SizeI in value
	param		height	SizeI in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		pixels	Void in array [COMPSIZE(format/type/width/height)]
	category	EXT_subtexture
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4100
	extension
	glsflags	pixel-unpack
	glsopcode	0x0124
	alias		TexSubImage2D

###############################################################################
#
# Extension #10
# EXT_copy_texture commands
#
###############################################################################

# Arguably TexelInternalFormat, not PixelInternalFormat
CopyTexImage1DEXT(target, level, internalformat, x, y, width, border)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		border		CheckedInt32 in value
	category	EXT_copy_texture
	version		1.0
	glxflags	EXT
	glxropcode	4119
	extension
	glsopcode	0x014D
	alias		CopyTexImage1D

# Arguably TexelInternalFormat, not PixelInternalFormat
CopyTexImage2DEXT(target, level, internalformat, x, y, width, height, border)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		border		CheckedInt32 in value
	category	EXT_copy_texture
	version		1.0
	glxflags	EXT
	glxropcode	4120
	extension
	glsopcode	0x014E
	alias		CopyTexImage2D

CopyTexSubImage1DEXT(target, level, xoffset, x, y, width)
	return		void
	param		target	TextureTarget in value
	param		level	CheckedInt32 in value
	param		xoffset CheckedInt32 in value
	param		x	WinCoord in value
	param		y	WinCoord in value
	param		width	SizeI in value
	category	EXT_copy_texture
	version		1.0
	glxflags	EXT
	glxropcode	4121
	extension
	glsopcode	0x014F
	alias		CopyTexSubImage1D

CopyTexSubImage2DEXT(target, level, xoffset, yoffset, x, y, width, height)
	return		void
	param		target	TextureTarget in value
	param		level	CheckedInt32 in value
	param		xoffset CheckedInt32 in value
	param		yoffset CheckedInt32 in value
	param		x	WinCoord in value
	param		y	WinCoord in value
	param		width	SizeI in value
	param		height	SizeI in value
	category	EXT_copy_texture
	version		1.0
	glxflags	EXT
	glxropcode	4122
	extension
	glsopcode	0x0150
	alias		CopyTexSubImage2D

CopyTexSubImage3DEXT(target, level, xoffset, yoffset, zoffset, x, y, width, height)
	return		void
	param		target	TextureTarget in value
	param		level	CheckedInt32 in value
	param		xoffset CheckedInt32 in value
	param		yoffset CheckedInt32 in value
	param		zoffset CheckedInt32 in value
	param		x	WinCoord in value
	param		y	WinCoord in value
	param		width	SizeI in value
	param		height	SizeI in value
	category	EXT_copy_texture
	version		1.0
	glxflags	EXT
	glxropcode	4123
	extension
	glsopcode	0x0151
	alias		CopyTexSubImage3D

###############################################################################
#
# Extension #11
# EXT_histogram commands
#
###############################################################################

GetHistogramEXT(target, reset, format, type, values)
	return		void
	param		target	HistogramTargetEXT in value
	param		reset	Boolean in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		values	Void out array [COMPSIZE(target/format/type)]
	category	EXT_histogram
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxvendorpriv	5
	extension
	glsflags	get pixel-pack
	glsopcode	0x0132
	offset		417

GetHistogramParameterfvEXT(target, pname, params)
	return		void
	param		target	HistogramTargetEXT in value
	param		pname	GetHistogramParameterPNameEXT in value
	param		params	Float32 out array [COMPSIZE(pname)]
	category	EXT_histogram
	dlflags		notlistable
	version		1.0
	glxvendorpriv	6
	glxflags	EXT
	extension
	glsflags	get
	glsopcode	0x0133
	offset		418

GetHistogramParameterivEXT(target, pname, params)
	return		void
	param		target	HistogramTargetEXT in value
	param		pname	GetHistogramParameterPNameEXT in value
	param		params	Int32 out array [COMPSIZE(pname)]
	category	EXT_histogram
	dlflags		notlistable
	version		1.0
	glxvendorpriv	7
	glxflags	EXT
	extension
	glsflags	get
	glsopcode	0x0134
	offset		419

GetMinmaxEXT(target, reset, format, type, values)
	return		void
	param		target	MinmaxTargetEXT in value
	param		reset	Boolean in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		values	Void out array [COMPSIZE(target/format/type)]
	category	EXT_histogram
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxvendorpriv	8
	extension
	glsflags	get pixel-pack
	glsopcode	0x0135
	offset		420

GetMinmaxParameterfvEXT(target, pname, params)
	return		void
	param		target	MinmaxTargetEXT in value
	param		pname	GetMinmaxParameterPNameEXT in value
	param		params	Float32 out array [COMPSIZE(pname)]
	category	EXT_histogram
	dlflags		notlistable
	version		1.0
	glxvendorpriv	9
	glxflags	EXT
	extension
	glsflags	get
	glsopcode	0x0136
	offset		421

GetMinmaxParameterivEXT(target, pname, params)
	return		void
	param		target	MinmaxTargetEXT in value
	param		pname	GetMinmaxParameterPNameEXT in value
	param		params	Int32 out array [COMPSIZE(pname)]
	category	EXT_histogram
	dlflags		notlistable
	version		1.0
	glxvendorpriv	10
	glxflags	EXT
	extension
	glsflags	get
	glsopcode	0x0137
	offset		422

HistogramEXT(target, width, internalformat, sink)
	return		void
	param		target		HistogramTargetEXT in value
	param		width		SizeI in value
	param		internalformat	PixelInternalFormat in value
	param		sink		Boolean in value
	category	EXT_histogram
	version		1.0
	glxropcode	4110
	glxflags	EXT
	extension
	glsopcode	0x0138
	alias		Histogram

MinmaxEXT(target, internalformat, sink)
	return		void
	param		target		MinmaxTargetEXT in value
	param		internalformat	PixelInternalFormat in value
	param		sink		Boolean in value
	category	EXT_histogram
	version		1.0
	glxropcode	4111
	glxflags	EXT
	extension
	glsopcode	0x0139
	alias		Minmax

ResetHistogramEXT(target)
	return		void
	param		target	HistogramTargetEXT in value
	category	EXT_histogram
	version		1.0
	glxropcode	4112
	glxflags	EXT
	extension
	glsopcode	0x013A
	alias		ResetHistogram

ResetMinmaxEXT(target)
	return		void
	param		target	MinmaxTargetEXT in value
	category	EXT_histogram
	version		1.0
	glxropcode	4113
	glxflags	EXT
	extension
	glsopcode	0x013B
	alias		ResetMinmax

###############################################################################
#
# Extension #12
# EXT_convolution commands
#
###############################################################################

ConvolutionFilter1DEXT(target, internalformat, width, format, type, image)
	return		void
	param		target		ConvolutionTargetEXT in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		image		Void in array [COMPSIZE(format/type/width)]
	category	EXT_convolution
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4101
	extension
	glsflags	pixel-unpack
	glsopcode	0x0125
	alias		ConvolutionFilter1D

ConvolutionFilter2DEXT(target, internalformat, width, height, format, type, image)
	return		void
	param		target		ConvolutionTargetEXT in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		image		Void in array [COMPSIZE(format/type/width/height)]
	category	EXT_convolution
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4102
	extension
	glsflags	pixel-unpack
	glsopcode	0x0126
	alias		ConvolutionFilter2D

ConvolutionParameterfEXT(target, pname, params)
	return		void
	param		target	ConvolutionTargetEXT in value
	param		pname	ConvolutionParameterEXT in value
	param		params	CheckedFloat32 in value
	category	EXT_convolution
	version		1.0
	glxropcode	4103
	glxflags	EXT
	extension
	glsflags	gl-enum
	glsopcode	0x0127
	alias		ConvolutionParameterf

ConvolutionParameterfvEXT(target, pname, params)
	return		void
	param		target	ConvolutionTargetEXT in value
	param		pname	ConvolutionParameterEXT in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	EXT_convolution
	version		1.0
	glxropcode	4104
	glxflags	EXT
	extension
	glsflags	gl-enum
	glsopcode	0x0128
	alias		ConvolutionParameterfv

ConvolutionParameteriEXT(target, pname, params)
	return		void
	param		target	ConvolutionTargetEXT in value
	param		pname	ConvolutionParameterEXT in value
	param		params	CheckedInt32 in value
	category	EXT_convolution
	version		1.0
	glxropcode	4105
	glxflags	EXT
	extension
	glsflags	gl-enum
	glsopcode	0x0129
	alias		ConvolutionParameteri

ConvolutionParameterivEXT(target, pname, params)
	return		void
	param		target	ConvolutionTargetEXT in value
	param		pname	ConvolutionParameterEXT in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	category	EXT_convolution
	version		1.0
	glxropcode	4106
	glxflags	EXT
	extension
	glsflags	gl-enum
	glsopcode	0x012A
	alias		ConvolutionParameteriv

CopyConvolutionFilter1DEXT(target, internalformat, x, y, width)
	return		void
	param		target		ConvolutionTargetEXT in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	category	EXT_convolution
	version		1.0
	glxropcode	4107
	glxflags	EXT
	extension
	glsopcode	0x012B
	alias		CopyConvolutionFilter1D

CopyConvolutionFilter2DEXT(target, internalformat, x, y, width, height)
	return		void
	param		target		ConvolutionTargetEXT in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	param		height		SizeI in value
	category	EXT_convolution
	version		1.0
	glxropcode	4108
	glxflags	EXT
	extension
	glsopcode	0x012C
	alias		CopyConvolutionFilter2D

GetConvolutionFilterEXT(target, format, type, image)
	return		void
	param		target	ConvolutionTargetEXT in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		image	Void out array [COMPSIZE(target/format/type)]
	category	EXT_convolution
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxvendorpriv	1
	extension
	glsflags	get pixel-pack
	glsopcode	0x012D
	offset		423

GetConvolutionParameterfvEXT(target, pname, params)
	return		void
	param		target	ConvolutionTargetEXT in value
	param		pname	ConvolutionParameterEXT in value
	param		params	Float32 out array [COMPSIZE(pname)]
	category	EXT_convolution
	dlflags		notlistable
	version		1.0
	glxvendorpriv	2
	glxflags	EXT
	extension
	glsflags	get
	glsopcode	0x012E
	offset		424

GetConvolutionParameterivEXT(target, pname, params)
	return		void
	param		target	ConvolutionTargetEXT in value
	param		pname	ConvolutionParameterEXT in value
	param		params	Int32 out array [COMPSIZE(pname)]
	category	EXT_convolution
	dlflags		notlistable
	version		1.0
	glxvendorpriv	3
	glxflags	EXT
	extension
	glsflags	get
	glsopcode	0x012F
	offset		425

GetSeparableFilterEXT(target, format, type, row, column, span)
	return		void
	param		target	SeparableTargetEXT in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		row	Void out array [COMPSIZE(target/format/type)]
	param		column	Void out array [COMPSIZE(target/format/type)]
	param		span	Void out array [COMPSIZE(target/format/type)]
	category	EXT_convolution
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxvendorpriv	4
	extension
	glsflags	get pixel-pack
	glsopcode	0x0130
	offset		426

SeparableFilter2DEXT(target, internalformat, width, height, format, type, row, column)
	return		void
	param		target		SeparableTargetEXT in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		row		Void in array [COMPSIZE(target/format/type/width)]
	param		column		Void in array [COMPSIZE(target/format/type/height)]
	category	EXT_convolution
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4109
	extension
	glsflags	pixel-unpack
	glsopcode	0x0131
	alias		SeparableFilter2D

###############################################################################
#
# Extension #13
# EXT_color_matrix commands
#
###############################################################################

# (none)
newcategory: EXT_color_matrix

###############################################################################
#
# Extension #14
# SGI_color_table commands
#
###############################################################################

ColorTableSGI(target, internalformat, width, format, type, table)
	return		void
	param		target		ColorTableTargetSGI in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		table		Void in array [COMPSIZE(format/type/width)]
	category	SGI_color_table
	dlflags		handcode
	glxflags	client-handcode server-handcode SGI
	version		1.0
	glxropcode	2053
	extension
	glsflags	pixel-unpack
	glsopcode	0x0167
	alias		ColorTable

ColorTableParameterfvSGI(target, pname, params)
	return		void
	param		target	ColorTableTargetSGI in value
	param		pname	ColorTableParameterPNameSGI in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	SGI_color_table
	version		1.0
	glxropcode	2054
	glxflags	SGI
	extension
	glsopcode	0x0168
	alias		ColorTableParameterfv

ColorTableParameterivSGI(target, pname, params)
	return		void
	param		target	ColorTableTargetSGI in value
	param		pname	ColorTableParameterPNameSGI in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	category	SGI_color_table
	version		1.0
	glxropcode	2055
	glxflags	SGI
	extension
	glsopcode	0x0169
	alias		ColorTableParameteriv

CopyColorTableSGI(target, internalformat, x, y, width)
	return		void
	param		target		ColorTableTargetSGI in value
	param		internalformat	PixelInternalFormat in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	category	SGI_color_table
	version		1.0
	glxropcode	2056
	glxflags	SGI
	extension
	glsopcode	0x016A
	alias		CopyColorTable

GetColorTableSGI(target, format, type, table)
	return		void
	param		target	ColorTableTargetSGI in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		table	Void out array [COMPSIZE(target/format/type)]
	category	SGI_color_table
	dlflags		notlistable
	glxflags	client-handcode server-handcode SGI
	version		1.0
	glxvendorpriv	4098
	extension
	glsflags	get pixel-pack
	glsopcode	0x016B
	offset		427

GetColorTableParameterfvSGI(target, pname, params)
	return		void
	param		target	ColorTableTargetSGI in value
	param		pname	GetColorTableParameterPNameSGI in value
	param		params	Float32 out array [COMPSIZE(pname)]
	category	SGI_color_table
	dlflags		notlistable
	version		1.0
	glxflags	SGI
	glxvendorpriv	4099
	extension
	glsflags	get
	glsopcode	0x016C
	offset		428

GetColorTableParameterivSGI(target, pname, params)
	return		void
	param		target	ColorTableTargetSGI in value
	param		pname	GetColorTableParameterPNameSGI in value
	param		params	Int32 out array [COMPSIZE(pname)]
	category	SGI_color_table
	dlflags		notlistable
	version		1.0
	glxflags	SGI
	glxvendorpriv	4100
	extension
	glsflags	get
	glsopcode	0x016D
	offset		429

###############################################################################
#
# Extension #15
# SGIX_pixel_texture commands
#
###############################################################################

PixelTexGenSGIX(mode)
	return		void
	param		mode	PixelTexGenModeSGIX in value
	category	SGIX_pixel_texture
	version		1.0
	glxflags	SGI
	glxropcode	2059
	extension
	glsopcode	0x0170
	offset		430

###############################################################################
#
# Extension #15 (variant)
# SGIS_pixel_texture commands
# Both SGIS and SGIX forms have extension #15!
#
###############################################################################

PixelTexGenParameteriSGIS(pname, param)
	return		void
	param		pname	PixelTexGenParameterNameSGIS in value
	param		param	CheckedInt32 in value
	category	SGIS_pixel_texture
	version		1.0
	extension
	glxropcode	?
	glxflags	ignore
	glsflags	gl-enum
	glsopcode	0x0192
	offset		431

PixelTexGenParameterivSGIS(pname, params)
	return		void
	param		pname	PixelTexGenParameterNameSGIS in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	category	SGIS_pixel_texture
	version		1.0
	extension
	glxropcode	?
	glxflags	ignore
	glsflags	gl-enum
	glsopcode	0x0193
	offset		432

PixelTexGenParameterfSGIS(pname, param)
	return		void
	param		pname	PixelTexGenParameterNameSGIS in value
	param		param	CheckedFloat32 in value
	category	SGIS_pixel_texture
	version		1.0
	extension
	glxropcode	?
	glxflags	ignore
	glsflags	gl-enum
	glsopcode	0x0194
	offset		433

PixelTexGenParameterfvSGIS(pname, params)
	return		void
	param		pname	PixelTexGenParameterNameSGIS in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	SGIS_pixel_texture
	version		1.0
	extension
	glxropcode	?
	glxflags	ignore
	glsflags	gl-enum
	glsopcode	0x0195
	offset		434

GetPixelTexGenParameterivSGIS(pname, params)
	return		void
	param		pname	PixelTexGenParameterNameSGIS in value
	param		params	CheckedInt32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	SGIS_pixel_texture
	version		1.0
	extension
	glxvendorpriv	?
	glxflags	ignore
	glsflags	get
	glsopcode	0x0196
	offset		435

GetPixelTexGenParameterfvSGIS(pname, params)
	return		void
	param		pname	PixelTexGenParameterNameSGIS in value
	param		params	CheckedFloat32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	SGIS_pixel_texture
	version		1.0
	extension
	glxvendorpriv	?
	glxflags	ignore
	glsflags	get
	glsopcode	0x0197
	offset		436

###############################################################################
#
# Extension #16
# SGIS_texture4D commands
#
###############################################################################

TexImage4DSGIS(target, level, internalformat, width, height, depth, size4d, border, format, type, pixels)
	return		void
	param		target		TextureTarget in value
	param		level		CheckedInt32 in value
	param		internalformat	PixelInternalFormat in value
	param		width		SizeI in value
	param		height		SizeI in value
	param		depth		SizeI in value
	param		size4d		SizeI in value
	param		border		CheckedInt32 in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		pixels		Void in array [COMPSIZE(format/type/width/height/depth/size4d)]
	category	SGIS_texture4D
	dlflags		handcode
	glxflags	client-handcode server-handcode SGI
	version		1.0
	glxropcode	2057
	extension
	glsflags	pixel-null pixel-unpack
	glsopcode	0x016E
	offset		437

TexSubImage4DSGIS(target, level, xoffset, yoffset, zoffset, woffset, width, height, depth, size4d, format, type, pixels)
	return		void
	param		target	TextureTarget in value
	param		level	CheckedInt32 in value
	param		xoffset CheckedInt32 in value
	param		yoffset CheckedInt32 in value
	param		zoffset CheckedInt32 in value
	param		woffset CheckedInt32 in value
	param		width	SizeI in value
	param		height	SizeI in value
	param		depth	SizeI in value
	param		size4d	SizeI in value
	param		format	PixelFormat in value
	param		type	PixelType in value
	param		pixels	Void in array [COMPSIZE(format/type/width/height/depth/size4d)]
	category	SGIS_texture4D
	dlflags		handcode
	glxflags	client-handcode server-handcode SGI
	version		1.0
	glxropcode	2058
	extension
	glsflags	pixel-unpack
	glsopcode	0x016F
	offset		438

###############################################################################
#
# Extension #17
# SGI_texture_color_table commands
#
###############################################################################

# (none)
newcategory: SGI_texture_color_table

###############################################################################
#
# Extension #18
# EXT_cmyka commands
#
###############################################################################

# (none)
newcategory: EXT_cmyka

###############################################################################
#
# Extension #19 - skipped
#
###############################################################################

###############################################################################
#
# Extension #20
# EXT_texture_object commands
#
###############################################################################

AreTexturesResidentEXT(n, textures, residences)
	return		Boolean
	param		n		SizeI in value
	param		textures	Texture in array [n]
	param		residences	Boolean out array [n]
	category	EXT_texture_object
	glxflags	EXT
	glxvendorpriv	11
	dlflags		notlistable
	version		1.0
	extension
	glsflags	get
	glsopcode	0x0147
	offset		439

BindTextureEXT(target, texture)
	return		void
	param		target	TextureTarget in value
	param		texture Texture in value
	category	EXT_texture_object
	version		1.0
	glxflags	EXT
	glxropcode	4117
	extension
	glsopcode	0x0148
	alias		BindTexture

DeleteTexturesEXT(n, textures)
	return		void
	param		n		SizeI in value
	param		textures	Texture in array [n]
	category	EXT_texture_object
	dlflags		notlistable
	version		1.0
	glxflags	EXT
	glxvendorpriv	12
	extension
	glsopcode	0x0149

GenTexturesEXT(n, textures)
	return		void
	param		n		SizeI in value
	param		textures	Texture out array [n]
	category	EXT_texture_object
	dlflags		notlistable
	version		1.0
	glxflags	EXT
	glxvendorpriv	13
	extension
	glsopcode	0x014A
	offset		440

IsTextureEXT(texture)
	return		Boolean
	param		texture Texture in value
	category	EXT_texture_object
	dlflags		notlistable
	version		1.0
	glxflags	EXT
	glxvendorpriv	14
	extension
	glsflags	get
	glsopcode	0x014B
	offset		441

PrioritizeTexturesEXT(n, textures, priorities)
	return		void
	param		n		SizeI in value
	param		textures	Texture in array [n]
	param		priorities	ClampedFloat32 in array [n]
	category	EXT_texture_object
	glxflags	EXT
	version		1.0
	glxropcode	4118
	extension
	glsopcode	0x014C
	alias		PrioritizeTextures

###############################################################################
#
# Extension #21
# SGIS_detail_texture commands
#
###############################################################################

DetailTexFuncSGIS(target, n, points)
	return		void
	param		target	TextureTarget in value
	param		n	SizeI in value
	param		points	Float32 in array [n*2]
	category	SGIS_detail_texture
	glxflags	SGI
	version		1.0
	glxropcode	2051
	extension
	glsopcode	0x0163
	offset		442

GetDetailTexFuncSGIS(target, points)
	return		void
	param		target	TextureTarget in value
	param		points	Float32 out array [COMPSIZE(target)]
	category	SGIS_detail_texture
	dlflags		notlistable
	version		1.0
	glxflags	SGI
	glxvendorpriv	4096
	extension
	glsflags	get
	glsopcode	0x0164
	offset		443

###############################################################################
#
# Extension #22
# SGIS_sharpen_texture commands
#
###############################################################################

SharpenTexFuncSGIS(target, n, points)
	return		void
	param		target	TextureTarget in value
	param		n	SizeI in value
	param		points	Float32 in array [n*2]
	category	SGIS_sharpen_texture
	glxflags	SGI
	version		1.0
	glxropcode	2052
	extension
	glsopcode	0x0165
	offset		444

GetSharpenTexFuncSGIS(target, points)
	return		void
	param		target	TextureTarget in value
	param		points	Float32 out array [COMPSIZE(target)]
	category	SGIS_sharpen_texture
	dlflags		notlistable
	version		1.0
	glxflags	SGI
	glxvendorpriv	4097
	extension
	glsflags	get
	glsopcode	0x0166
	offset		445

###############################################################################
#
# EXT_packed_pixels commands
# Extension #23
#
###############################################################################

# (none)
newcategory: EXT_packed_pixels

###############################################################################
#
# Extension #24
# SGIS_texture_lod commands
#
###############################################################################

# (none)
newcategory: SGIS_texture_lod

###############################################################################
#
# Extension #25
# SGIS_multisample commands
#
###############################################################################

SampleMaskSGIS(value, invert)
	return		void
	param		value	ClampedFloat32 in value
	param		invert	Boolean in value
	category	SGIS_multisample
	version		1.0
	glxropcode	2048
	glxflags	SGI
	extension
	glsopcode	0x0160
	offset		446

SamplePatternSGIS(pattern)
	return		void
	param		pattern SamplePatternSGIS in value
	category	SGIS_multisample
	version		1.0
	glxropcode	2049
	glxflags	SGI
	extension
	glsopcode	0x0161
	offset		447

###############################################################################
#
# Extension #26 - no specification?
#
###############################################################################

###############################################################################
#
# Extension #27
# EXT_rescale_normal commands
#
###############################################################################

# (none)
newcategory: EXT_rescale_normal

###############################################################################
#
# Extension #28 - GLX_EXT_visual_info
# Extension #29 - skipped
#
###############################################################################

###############################################################################
#
# Extension #30
# EXT_vertex_array commands
#
###############################################################################

ArrayElementEXT(i)
	return		void
	param		i	Int32 in value
	category	EXT_vertex_array
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsopcode	0x013E
	alias		ArrayElement

ColorPointerEXT(size, type, stride, count, pointer)
	return		void
	param		size	Int32 in value
	param		type	ColorPointerType in value
	param		stride	SizeI in value
	param		count	SizeI in value
	param		pointer Void in array [COMPSIZE(size/type/stride/count)] retained
	category	EXT_vertex_array
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsflags	client
	glsopcode	0x013F
	offset		448

DrawArraysEXT(mode, first, count)
	return		void
	param		mode	BeginMode in value
	param		first	Int32 in value
	param		count	SizeI in value
	category	EXT_vertex_array
	dlflags		handcode
	glxflags	client-handcode server-handcode EXT
	version		1.0
	glxropcode	4116
	extension
	glsopcode	0x0140
	alias		DrawArrays

EdgeFlagPointerEXT(stride, count, pointer)
	return		void
	param		stride	SizeI in value
	param		count	SizeI in value
	param		pointer Boolean in array [COMPSIZE(stride/count)] retained
	category	EXT_vertex_array
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsflags	client
	glsopcode	0x0141
	offset		449

GetPointervEXT(pname, params)
	return		void
	param		pname	GetPointervPName in value
	param		params	VoidPointer out array [1]
	category	EXT_vertex_array
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsflags	client get
	glsopcode	0x0142
	alias		GetPointerv

IndexPointerEXT(type, stride, count, pointer)
	return		void
	param		type	IndexPointerType in value
	param		stride	SizeI in value
	param		count	SizeI in value
	param		pointer Void in array [COMPSIZE(type/stride/count)] retained
	category	EXT_vertex_array
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsflags	client
	glsopcode	0x0143
	offset		450

NormalPointerEXT(type, stride, count, pointer)
	return		void
	param		type	NormalPointerType in value
	param		stride	SizeI in value
	param		count	SizeI in value
	param		pointer Void in array [COMPSIZE(type/stride/count)] retained
	category	EXT_vertex_array
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsflags	client
	glsopcode	0x0144
	offset		451

TexCoordPointerEXT(size, type, stride, count, pointer)
	return		void
	param		size	Int32 in value
	param		type	TexCoordPointerType in value
	param		stride	SizeI in value
	param		count	SizeI in value
	param		pointer Void in array [COMPSIZE(size/type/stride/count)] retained
	category	EXT_vertex_array
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsflags	client
	glsopcode	0x0145
	offset		452

VertexPointerEXT(size, type, stride, count, pointer)
	return		void
	param		size	Int32 in value
	param		type	VertexPointerType in value
	param		stride	SizeI in value
	param		count	SizeI in value
	param		pointer Void in array [COMPSIZE(size/type/stride/count)] retained
	category	EXT_vertex_array
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.0
	extension
	glsflags	client
	glsopcode	0x0146
	offset		453

###############################################################################
#
# Extension #31
# EXT_misc_attribute commands
#
###############################################################################

# (none)
newcategory: EXT_misc_attribute

###############################################################################
#
# Extension #32
# SGIS_generate_mipmap commands
#
###############################################################################

# (none)
newcategory: SGIS_generate_mipmap

###############################################################################
#
# Extension #33
# SGIX_clipmap commands
#
###############################################################################

# (none)
newcategory: SGIX_clipmap

###############################################################################
#
# Extension #34
# SGIX_shadow commands
#
###############################################################################

# (none)
newcategory: SGIX_shadow

###############################################################################
#
# Extension #35
# SGIS_texture_edge_clamp commands
#
###############################################################################

# (none)
newcategory: SGIS_texture_edge_clamp

###############################################################################
#
# Extension #36
# SGIS_texture_border_clamp commands
#
###############################################################################

# (none)
newcategory: SGIS_texture_border_clamp

###############################################################################
#
# Extension #37
# EXT_blend_minmax commands
#
###############################################################################

BlendEquationEXT(mode)
	return		void
	param		mode		BlendEquationModeEXT in value
	category	EXT_blend_minmax
	version		1.0
	glxropcode	4097
	glxflags	EXT
	extension	soft
	glsopcode	0x0121
	alias		BlendEquation

###############################################################################
#
# Extension #38
# EXT_blend_subtract commands
#
###############################################################################

# (none)
newcategory: EXT_blend_subtract

###############################################################################
#
# Extension #39
# EXT_blend_logic_op commands
#
###############################################################################

# (none)
newcategory: EXT_blend_logic_op

###############################################################################
#
# Extension #40 - GLX_SGI_swap_control
# Extension #41 - GLX_SGI_video_sync
# Extension #42 - GLX_SGI_make_current_read
# Extension #43 - GLX_SGIX_video_source
# Extension #44 - GLX_EXT_visual_rating
#
###############################################################################

###############################################################################
#
# Extension #45
# SGIX_interlace commands
#
###############################################################################

# (none)
newcategory: SGIX_interlace

###############################################################################
#
# Extension #46
# SGIX_pixel_tiles commands
#
###############################################################################

# (none)
newcategory: SGIX_pixel_tiles

###############################################################################
#
# Extension #47 - GLX_EXT_import_context
# Extension #48 - skipped
# Extension #49 - GLX_SGIX_fbconfig
# Extension #50 - GLX_SGIX_pbuffer
#
###############################################################################

###############################################################################
#
# Extension #51
# SGIX_texture_select commands
#
###############################################################################

# (none)
newcategory: SGIX_texture_select

###############################################################################
#
# Extension #52
# SGIX_sprite commands
#
###############################################################################

SpriteParameterfSGIX(pname, param)
	return		void
	param		pname	SpriteParameterNameSGIX in value
	param		param	CheckedFloat32 in value
	category	SGIX_sprite
	version		1.0
	glxflags	SGI
	glxropcode	2060
	extension
	glsflags	gl-enum
	glsopcode	0x0171
	offset		454

SpriteParameterfvSGIX(pname, params)
	return		void
	param		pname	SpriteParameterNameSGIX in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	SGIX_sprite
	version		1.0
	glxflags	SGI
	glxropcode	2061
	extension
	glsflags	gl-enum
	glsopcode	0x0172
	offset		455

SpriteParameteriSGIX(pname, param)
	return		void
	param		pname	SpriteParameterNameSGIX in value
	param		param	CheckedInt32 in value
	category	SGIX_sprite
	version		1.0
	glxflags	SGI
	glxropcode	2062
	extension
	glsflags	gl-enum
	glsopcode	0x0173
	offset		456

SpriteParameterivSGIX(pname, params)
	return		void
	param		pname	SpriteParameterNameSGIX in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	category	SGIX_sprite
	version		1.0
	glxflags	SGI
	glxropcode	2063
	extension
	glsflags	gl-enum
	glsopcode	0x0174
	offset		457

###############################################################################
#
# Extension #53
# SGIX_texture_multi_buffer commands
#
###############################################################################

# (none)
newcategory: SGIX_texture_multi_buffer

###############################################################################
#
# Extension #54
# SGIS_point_parameters commands
#
###############################################################################

PointParameterfEXT(pname, param)
	return		void
	param		pname	PointParameterNameEXT in value
	param		param	CheckedFloat32 in value
	category	EXT_point_parameters
	version		1.0
	glxflags	SGI
	glxropcode	2065
	extension
	glsopcode	0x0177
	offset		458

PointParameterfvEXT(pname, params)
	return		void
	param		pname	PointParameterNameEXT in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	EXT_point_parameters
	version		1.0
	glxflags	SGI
	glxropcode	2066
	extension
	glsopcode	0x0178
	offset		459

PointParameterfSGIS(pname, param)
	return		void
	param		pname	PointParameterNameEXT in value
	param		param	CheckedFloat32 in value
	category	EXT_point_parameters
	version		1.0
	glxflags	SGI
	extension
	glsopcode	0x0177
	alias		PointParameterfEXT

PointParameterfvSGIS(pname, params)
	return		void
	param		pname	PointParameterNameEXT in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	EXT_point_parameters
	version		1.0
	glxflags	SGI
	extension
	glsopcode	0x0178
	alias		PointParameterfvEXT

###############################################################################
#
# Extension #55
# SGIX_instruments commands
#
###############################################################################

GetInstrumentsSGIX()
	return		Int32
	dlflags		notlistable
	category	SGIX_instruments
	version		1.0
	glxflags	SGI
	glxvendorpriv	4102
	extension
	glsflags	get
	glsopcode	0x017A
	offset		460

InstrumentsBufferSGIX(size, buffer)
	return		void
	param		size	SizeI in value
	param		buffer	Int32 out array [size] retained
	dlflags		notlistable
	category	SGIX_instruments
	version		1.0
	glxflags	SGI
	glxvendorpriv	4103
	extension
	glsflags	client
	glsopcode	0x017B
	aliasprivate	PipelineInstrumentsBufferSGIX
	offset		461

PollInstrumentsSGIX(marker_p)
	return		Int32
	param		marker_p	Int32 out array [1]
	dlflags		notlistable
	category	SGIX_instruments
	version		1.0
	glxflags	SGI
	glxvendorpriv	4104
	extension
	glsflags	get
	glsopcode	0x017C
	offset		462

ReadInstrumentsSGIX(marker)
	return		void
	param		marker Int32 in value
	category	SGIX_instruments
	version		1.0
	glxflags	SGI
	glxropcode	2077
	extension
	glsopcode	0x017D
	offset		463

StartInstrumentsSGIX()
	return		void
	category	SGIX_instruments
	version		1.0
	glxflags	SGI
	glxropcode	2069
	extension
	glsopcode	0x017E
	offset		464

StopInstrumentsSGIX(marker)
	return		void
	param		marker Int32 in value
	category	SGIX_instruments
	version		1.0
	glxflags	SGI
	glxropcode	2070
	extension
	glsopcode	0x017F
	offset		465

###############################################################################
#
# Extension #56
# SGIX_texture_scale_bias commands
#
###############################################################################

# (none)
newcategory: SGIX_texture_scale_bias

###############################################################################
#
# Extension #57
# SGIX_framezoom commands
#
###############################################################################

FrameZoomSGIX(factor)
	return		void
	param		factor	CheckedInt32 in value
	category	SGIX_framezoom
	version		1.0
	glxflags	SGI
	glxropcode	2072
	extension
	glsopcode	0x0182
	offset		466

###############################################################################
#
# Extension #58
# SGIX_tag_sample_buffer commands
#
###############################################################################

TagSampleBufferSGIX()
	return		void
	category	SGIX_tag_sample_buffer
	version		1.0
	glxropcode	2050
	glxflags	SGI
	extension
	glsopcode	0x0162
	offset		467

###############################################################################
#
# Extension #59 - skipped
#
###############################################################################

###############################################################################
#
# Extension #60
# SGIX_reference_plane commands
#
###############################################################################

ReferencePlaneSGIX(equation)
	return		void
	param		equation	Float64 in array [4]
	category	SGIX_reference_plane
	version		1.0
	glxflags	SGI
	glxropcode	2071
	extension
	glsopcode	0x0181
	offset		468

###############################################################################
#
# Extension #61
# SGIX_flush_raster commands
#
###############################################################################

FlushRasterSGIX()
	return		void
	category	SGIX_flush_raster
	version		1.0
	dlflags		notlistable
	glxflags	SGI
	glxvendorpriv	4105
	extension
	glsopcode	0x0180
	offset		469

###############################################################################
#
# Extension #62 - GLX_SGIX_cushion
#
###############################################################################

###############################################################################
#
# Extension #63
# SGIX_depth_texture commands
#
###############################################################################

# (none)
newcategory: SGIX_depth_texture

###############################################################################
#
# Extension #64
# SGIS_fog_function commands
#
###############################################################################

FogFuncSGIS(n, points)
	return		void
	param		n		SizeI in value
	param		points		Float32 in array [n*2]
	category	SGIS_fog_function
	version		1.1
	glxflags	SGI
	glxropcode	2067
	extension
	glsopcode	0x0179
	offset

# Need to insert GLX information
GetFogFuncSGIS(points)
	return		void
	param		points		Float32 in array [COMPSIZE()]
	category	SGIS_fog_function
	version		1.1
	dlflags		notlistable
	glxflags	ignore
	extension
	glsflags	get
	glsopcode	0x0191
	offset

###############################################################################
#
# Extension #65
# SGIX_fog_offset commands
#
###############################################################################

# (none)
newcategory: SGIX_fog_offset

###############################################################################
#
# Extension #66
# HP_image_transform commands
#
###############################################################################

ImageTransformParameteriHP(target, pname, param)
	return		void
	param		target	    ImageTransformTargetHP in value
	param		pname	    ImageTransformPNameHP in value
	param		param	    Int32 in value
	category	HP_image_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ImageTransformParameterfHP(target, pname, param)
	return		void
	param		target	    ImageTransformTargetHP in value
	param		pname	    ImageTransformPNameHP in value
	param		param	    Float32 in value
	category	HP_image_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ImageTransformParameterivHP(target, pname, params)
	return		void
	param		target	    ImageTransformTargetHP in value
	param		pname	    ImageTransformPNameHP in value
	param		params	    Int32 in array [COMPSIZE(pname)]
	category	HP_image_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ImageTransformParameterfvHP(target, pname, params)
	return		void
	param		target	    ImageTransformTargetHP in value
	param		pname	    ImageTransformPNameHP in value
	param		params	    Float32 in array [COMPSIZE(pname)]
	category	HP_image_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GetImageTransformParameterivHP(target, pname, params)
	return		void
	param		target	    ImageTransformTargetHP in value
	param		pname	    ImageTransformPNameHP in value
	param		params	    Int32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	HP_image_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GetImageTransformParameterfvHP(target, pname, params)
	return		void
	param		target	    ImageTransformTargetHP in value
	param		pname	    ImageTransformPNameHP in value
	param		params	    Float32 out array [COMPSIZE(pname)]
	category	HP_image_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #67
# HP_convolution_border_modes commands
#
###############################################################################

# (none)
newcategory: HP_convolution_border_modes

###############################################################################
#
# Extension #68
@@# INGR_palette_buffer commands
#
###############################################################################

#@ (Intergraph hasn't provided a spec)

###############################################################################
#
# Extension #69
# SGIX_texture_add_env commands
#
###############################################################################

# (none)
newcategory: SGIX_texture_add_env

###############################################################################
#
# Extension #70 - skipped
# Extension #71 - skipped
# Extension #72 - skipped
# Extension #73 - skipped
#
###############################################################################

###############################################################################
#
# Extension #74
# EXT_color_subtable commands
#
# This was probably never actually shipped as an EXT - just written up as a
# reference for OpenGL 1.2 ARB_imaging.
#
###############################################################################

ColorSubTableEXT(target, start, count, format, type, data)
	return		void
	param		target		ColorTableTarget in value
	param		start		SizeI in value
	param		count		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		data		Void in array [COMPSIZE(format/type/count)]
	category	EXT_color_subtable
	version		1.2
	alias		ColorSubTable

CopyColorSubTableEXT(target, start, x, y, width)
	return		void
	param		target		ColorTableTarget in value
	param		start		SizeI in value
	param		x		WinCoord in value
	param		y		WinCoord in value
	param		width		SizeI in value
	category	EXT_color_subtable
	version		1.2
	alias		CopyColorSubTable

###############################################################################
#
# Extension #75 - GLU_EXT_object_space_tess
#
###############################################################################

###############################################################################
#
# Extension #76
# PGI_vertex_hints commands
#
###############################################################################

# (none)
newcategory: PGI_vertex_hints

###############################################################################
#
# Extension #77
# PGI_misc_hints commands
#
###############################################################################

HintPGI(target, mode)
	return		void
	param		target		HintTargetPGI in value
	param		mode		Int32 in value
	category	PGI_misc_hints
	version		1.1
	offset		544

###############################################################################
#
# Extension #78
# EXT_paletted_texture commands
#
###############################################################################

ColorTableEXT(target, internalFormat, width, format, type, table)
	return		void
	param		target		ColorTableTarget in value
	param		internalFormat	PixelInternalFormat in value
	param		width		SizeI in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		table		Void in array [COMPSIZE(format/type/width)]
	category	EXT_paletted_texture
	version		1.1
	alias		ColorTable

GetColorTableEXT(target, format, type, data)
	return		void
	param		target		ColorTableTarget in value
	param		format		PixelFormat in value
	param		type		PixelType in value
	param		data		Void out array [COMPSIZE(target/format/type)]
	category	EXT_paletted_texture
	version		1.1
	offset		550

GetColorTableParameterivEXT(target, pname, params)
	return		void
	param		target		ColorTableTarget in value
	param		pname		GetColorTableParameterPName in value
	param		params		Int32 out array [COMPSIZE(pname)]
	category	EXT_paletted_texture
	version		1.1
	offset		551

GetColorTableParameterfvEXT(target, pname, params)
	return		void
	param		target		ColorTableTarget in value
	param		pname		GetColorTableParameterPName in value
	param		params		Float32 out array [COMPSIZE(pname)]
	category	EXT_paletted_texture
	version		1.1
	offset		552

###############################################################################
#
# Extension #79
# EXT_clip_volume_hint commands
#
###############################################################################

# (none)
newcategory: EXT_clip_volume_hint

###############################################################################
#
# Extension #80
# SGIX_list_priority commands
#
###############################################################################

# @@@ Needs vendorpriv opcodes assigned
GetListParameterfvSGIX(list, pname, params)
	return		void
	param		list	List in value
	param		pname	ListParameterName in value
	param		params	CheckedFloat32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	glxflags	ignore
	category	SGIX_list_priority
	version		1.0
	glxvendorpriv	?
	extension
	glsopcode	0x0188
	offset		470

# @@@ Needs vendorpriv opcodes assigned
GetListParameterivSGIX(list, pname, params)
	return		void
	param		list	List in value
	param		pname	ListParameterName in value
	param		params	CheckedInt32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	glxflags	ignore
	category	SGIX_list_priority
	version		1.0
	glxvendorpriv	?
	extension
	glsopcode	0x0189
	offset		471

ListParameterfSGIX(list, pname, param)
	return		void
	param		list	List in value
	param		pname	ListParameterName in value
	param		param	CheckedFloat32 in value
	dlflags		notlistable
	glxflags	ignore
	category	SGIX_list_priority
	version		1.0
	glxropcode	2078
	extension
	glsopcode	0x018A
	offset		472

ListParameterfvSGIX(list, pname, params)
	return		void
	param		list	List in value
	param		pname	ListParameterName in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	dlflags		notlistable
	glxflags	ignore
	category	SGIX_list_priority
	version		1.0
	glxropcode	2079
	extension
	glsopcode	0x018B
	offset		473

ListParameteriSGIX(list, pname, param)
	return		void
	param		list	List in value
	param		pname	ListParameterName in value
	param		param	CheckedInt32 in value
	dlflags		notlistable
	glxflags	ignore
	category	SGIX_list_priority
	version		1.0
	glxropcode	2080
	extension
	glsopcode	0x018C
	offset		474

ListParameterivSGIX(list, pname, params)
	return		void
	param		list	List in value
	param		pname	ListParameterName in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	dlflags		notlistable
	glxflags	ignore
	category	SGIX_list_priority
	version		1.0
	glxropcode	2081
	extension
	glsopcode	0x018D
	offset		475

###############################################################################
#
# Extension #81
# SGIX_ir_instrument1 commands
#
###############################################################################

# (none)
newcategory: SGIX_ir_instrument1

###############################################################################
#
# Extension #82
# SGIX_calligraphic_fragment commands
#
###############################################################################

# (none)
newcategory: SGIX_calligraphic_fragment

###############################################################################
#
# Extension #83 - GLX_SGIX_video_resize
#
###############################################################################

###############################################################################
#
# Extension #84
# SGIX_texture_lod_bias commands
#
###############################################################################

# (none)
newcategory: SGIX_texture_lod_bias

###############################################################################
#
# Extension #85 - skipped
# Extension #86 - GLX_SGIX_dmbuffer
# Extension #87 - skipped
# Extension #88 - skipped
# Extension #89 - skipped
#
###############################################################################

###############################################################################
#
# Extension #90
# SGIX_shadow_ambient commands
#
###############################################################################

# (none)
newcategory: SGIX_shadow_ambient

###############################################################################
#
# Extension #91 - GLX_SGIX_swap_group
# Extension #92 - GLX_SGIX_swap_barrier
#
###############################################################################

###############################################################################
#
# Extension #93
# EXT_index_texture commands
#
###############################################################################

# (none)
newcategory: EXT_index_texture

###############################################################################
#
# Extension #94
# EXT_index_material commands
#
###############################################################################

IndexMaterialEXT(face, mode)
	return		void
	param		face		MaterialFace in value
	param		mode		IndexMaterialParameterEXT in value
	category	EXT_index_material
	version		1.1
	extension	soft
	glxflags	ignore
	glsflags	ignore
	offset		538

###############################################################################
#
# Extension #95
# EXT_index_func commands
#
###############################################################################

IndexFuncEXT(func, ref)
	return		void
	param		func		IndexFunctionEXT in value
	param		ref		ClampedFloat32 in value
	category	EXT_index_func
	version		1.1
	extension	soft
	glxflags	ignore
	glsflags	ignore
	offset		539

###############################################################################
#
# Extension #96
# EXT_index_array_formats commands
#
###############################################################################

# (none)
newcategory: EXT_index_array_formats

###############################################################################
#
# Extension #97
# EXT_compiled_vertex_array commands
#
###############################################################################

LockArraysEXT(first, count)
	return		void
	param		first		Int32 in value
	param		count		SizeI in value
	category	EXT_compiled_vertex_array
	version		1.1
	dlflags		notlistable
	extension	soft
	glxflags	ignore
	glsflags	ignore
	offset		540

UnlockArraysEXT()
	return		void
	category	EXT_compiled_vertex_array
	version		1.1
	dlflags		notlistable
	extension	soft
	glxflags	ignore
	glsflags	ignore
	offset		541

###############################################################################
#
# Extension #98
# EXT_cull_vertex commands
#
###############################################################################

CullParameterdvEXT(pname, params)
	return		void
	param		pname		CullParameterEXT in value
	param		params		Float64 out array [4]
	category	EXT_cull_vertex
	version		1.1
	dlflags		notlistable
	extension	soft
	glxflags	ignore
	glsflags	ignore
	offset		542

CullParameterfvEXT(pname, params)
	return		void
	param		pname		CullParameterEXT in value
	param		params		Float32 out array [4]
	category	EXT_cull_vertex
	version		1.1
	dlflags		notlistable
	extension	soft
	glxflags	ignore
	glsflags	ignore
	offset		543

###############################################################################
#
# Extension #99 - skipped
# Extension #100 - GLU_EXT_nurbs_tessellator
#
###############################################################################

###############################################################################
#
# Extension #101
# SGIX_ycrcb commands
#
###############################################################################

# (none)
newcategory: SGIX_ycrcb

###############################################################################
#
# Extension #102
# SGIX_fragment_lighting commands
#
###############################################################################

FragmentColorMaterialSGIX(face, mode)
	return		void
	param		face	MaterialFace in value
	param		mode	MaterialParameter in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x019E
	offset		476

FragmentLightfSGIX(light, pname, param)
	return		void
	param		light	FragmentLightNameSGIX in value
	param		pname	FragmentLightParameterSGIX in value
	param		param	CheckedFloat32 in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x019F
	offset		477

FragmentLightfvSGIX(light, pname, params)
	return		void
	param		light	FragmentLightNameSGIX in value
	param		pname	FragmentLightParameterSGIX in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x01A0
	offset		478

FragmentLightiSGIX(light, pname, param)
	return		void
	param		light	FragmentLightNameSGIX in value
	param		pname	FragmentLightParameterSGIX in value
	param		param	CheckedInt32 in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x01A1
	offset		479

FragmentLightivSGIX(light, pname, params)
	return		void
	param		light	FragmentLightNameSGIX in value
	param		pname	FragmentLightParameterSGIX in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x01A2
	offset		480

FragmentLightModelfSGIX(pname, param)
	return		void
	param		pname	FragmentLightModelParameterSGIX in value
	param		param	CheckedFloat32 in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsflags	gl-enum
	glsopcode	0x01A3
	offset		481

FragmentLightModelfvSGIX(pname, params)
	return		void
	param		pname	FragmentLightModelParameterSGIX in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsflags	gl-enum
	glsopcode	0x01A4
	offset		482

FragmentLightModeliSGIX(pname, param)
	return		void
	param		pname	FragmentLightModelParameterSGIX in value
	param		param	CheckedInt32 in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsflags	gl-enum
	glsopcode	0x01A5
	offset		483

FragmentLightModelivSGIX(pname, params)
	return		void
	param		pname	FragmentLightModelParameterSGIX in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsflags	gl-enum
	glsopcode	0x01A6
	offset		484

FragmentMaterialfSGIX(face, pname, param)
	return		void
	param		face	MaterialFace in value
	param		pname	MaterialParameter in value
	param		param	CheckedFloat32 in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x01A7
	offset		485

FragmentMaterialfvSGIX(face, pname, params)
	return		void
	param		face	MaterialFace in value
	param		pname	MaterialParameter in value
	param		params	CheckedFloat32 in array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x01A8
	offset		486

FragmentMaterialiSGIX(face, pname, param)
	return		void
	param		face	MaterialFace in value
	param		pname	MaterialParameter in value
	param		param	CheckedInt32 in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x01A9
	offset		487

FragmentMaterialivSGIX(face, pname, params)
	return		void
	param		face	MaterialFace in value
	param		pname	MaterialParameter in value
	param		params	CheckedInt32 in array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsopcode	0x01AA
	offset		488

GetFragmentLightfvSGIX(light, pname, params)
	return		void
	param		light	FragmentLightNameSGIX in value
	param		pname	FragmentLightParameterSGIX in value
	param		params	Float32 out array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	dlflags		notlistable
	glxflags	ignore
	version		1.0
	extension
	glsflags	get
	glsopcode	0x01AB
	offset		489

GetFragmentLightivSGIX(light, pname, params)
	return		void
	param		light	FragmentLightNameSGIX in value
	param		pname	FragmentLightParameterSGIX in value
	param		params	Int32 out array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	dlflags		notlistable
	glxflags	ignore
	version		1.0
	extension
	glsflags	get
	glsopcode	0x01AC
	offset		490

GetFragmentMaterialfvSGIX(face, pname, params)
	return		void
	param		face	MaterialFace in value
	param		pname	MaterialParameter in value
	param		params	Float32 out array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	dlflags		notlistable
	glxflags	ignore
	version		1.0
	extension
	glsflags	get
	glsopcode	0x01AD
	offset		491

GetFragmentMaterialivSGIX(face, pname, params)
	return		void
	param		face	MaterialFace in value
	param		pname	MaterialParameter in value
	param		params	Int32 out array [COMPSIZE(pname)]
	category	SGIX_fragment_lighting
	dlflags		notlistable
	glxflags	ignore
	version		1.0
	extension
	glsflags	get
	glsopcode	0x01AE
	offset		492

LightEnviSGIX(pname, param)
	return		void
	param		pname	LightEnvParameterSGIX in value
	param		param	CheckedInt32 in value
	category	SGIX_fragment_lighting
	glxflags	ignore
	version		1.0
	extension
	glsflags	gl-enum
	glsopcode	0x01AF
	offset		493

###############################################################################
#
# Extension #103 - skipped
# Extension #104 - skipped
# Extension #105 - skipped
# Extension #106 - skipped
# Extension #107 - skipped
# Extension #108 - skipped
# Extension #109 - skipped
#
###############################################################################

###############################################################################
#
# Extension #110
# IBM_rasterpos_clip commands
#
###############################################################################

# (none)
newcategory: IBM_rasterpos_clip

###############################################################################
#
# Extension #111
# HP_texture_lighting commands
#
###############################################################################

# (none)
newcategory: HP_texture_lighting

###############################################################################
#
# Extension #112
# EXT_draw_range_elements commands
#
###############################################################################

# Spec entries to be written
DrawRangeElementsEXT(mode, start, end, count, type, indices)
	return		void
	param		mode		BeginMode in value
	param		start		UInt32 in value
	param		end		UInt32 in value
	param		count		SizeI in value
	param		type		DrawElementsType in value
	param		indices		Void in array [COMPSIZE(count/type)]
	category	EXT_draw_range_elements
	dlflags		handcode
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsopcode	?
	alias		DrawRangeElements

###############################################################################
#
# Extension #113
# WIN_phong_shading commands
#
###############################################################################

# (none)
newcategory: WIN_phong_shading

###############################################################################
#
# Extension #114
# WIN_specular_fog commands
#
###############################################################################

# (none)
newcategory: WIN_specular_fog

###############################################################################
#
# Extension #115 - skipped
# Extension #116 - skipped
#
###############################################################################

###############################################################################
#
# Extension #117
# EXT_light_texture commands
#
###############################################################################

# Spec entries to be written
ApplyTextureEXT(mode)
	return		void
	param		mode	    LightTextureModeEXT in value
	category	EXT_light_texture
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TextureLightEXT(pname)
	return		void
	param		pname	    LightTexturePNameEXT in value
	category	EXT_light_texture
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TextureMaterialEXT(face, mode)
	return		void
	param		face	    MaterialFace in value
	param		mode	    MaterialParameter in value
	category	EXT_light_texture
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #118 - skipped
#
###############################################################################

###############################################################################
#
# Extension #119
# SGIX_blend_alpha_minmax commands
#
###############################################################################

# (none)
newcategory: SGIX_blend_alpha_minmax

###############################################################################
#
# Extension #120 - skipped
# Extension #121 - skipped
# Extension #122 - skipped
# Extension #123 - skipped
# Extension #124 - skipped
# Extension #125 - skipped
# Extension #126 - skipped
# Extension #127 - skipped
# Extension #128 - skipped
#
###############################################################################

###############################################################################
#
# Extension #129
# EXT_bgra commands
#
###############################################################################

# (none)
newcategory: EXT_bgra

###############################################################################
#
# Extension #130 - skipped
# Extension #131 - skipped
# Extension #132 - skipped
# Extension #133 - skipped
# Extension #134 - skipped
# Extension #135 - skipped (INTEL_texture_scissor was never implemented)
#
###############################################################################

###############################################################################
#
# Extension #136
# INTEL_parallel_arrays commands
#
###############################################################################

VertexPointervINTEL(size, type, pointer)
	return		void
	param		size	    Int32 in value
	param		type	    VertexPointerType in value
	param		pointer     VoidPointer in array [4] retained
	category	INTEL_parallel_arrays
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.1
	glsflags	client
	glsopcode	?
	offset		?

NormalPointervINTEL(type, pointer)
	return		void
	param		type	    NormalPointerType in value
	param		pointer     VoidPointer in array [4] retained
	category	INTEL_parallel_arrays
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.1
	glsflags	client
	glsopcode	?
	offset		?

ColorPointervINTEL(size, type, pointer)
	return		void
	param		size	    Int32 in value
	param		type	    VertexPointerType in value
	param		pointer     VoidPointer in array [4] retained
	category	INTEL_parallel_arrays
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.1
	glsflags	client
	glsopcode	?
	offset		?

TexCoordPointervINTEL(size, type, pointer)
	return		void
	param		size	    Int32 in value
	param		type	    VertexPointerType in value
	param		pointer     VoidPointer in array [4] retained
	category	INTEL_parallel_arrays
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.1
	glsflags	client
	glsopcode	?
	offset		?


###############################################################################
#
# Extension #137
# HP_occlusion_test commands
#
###############################################################################

# (none)
newcategory: HP_occlusion_test

###############################################################################
#
# Extension #138
# EXT_pixel_transform commands
#
###############################################################################

PixelTransformParameteriEXT(target, pname, param)
	return		void
	param		target	    PixelTransformTargetEXT in value
	param		pname	    PixelTransformPNameEXT in value
	param		param	    Int32 in value
	category	EXT_pixel_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

PixelTransformParameterfEXT(target, pname, param)
	return		void
	param		target	    PixelTransformTargetEXT in value
	param		pname	    PixelTransformPNameEXT in value
	param		param	    Float32 in value
	category	EXT_pixel_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

PixelTransformParameterivEXT(target, pname, params)
	return		void
	param		target	    PixelTransformTargetEXT in value
	param		pname	    PixelTransformPNameEXT in value
	param		params	    Int32 in array [1]
	category	EXT_pixel_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

PixelTransformParameterfvEXT(target, pname, params)
	return		void
	param		target	    PixelTransformTargetEXT in value
	param		pname	    PixelTransformPNameEXT in value
	param		params	    Float32 in array [1]
	category	EXT_pixel_transform
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #139
# EXT_pixel_transform_color_table commands
#
###############################################################################

# (none)
newcategory: EXT_pixel_transform_color_table

###############################################################################
#
# Extension #140 - skipped
#
###############################################################################

###############################################################################
#
# Extension #141
# EXT_shared_texture_palette commands
#
###############################################################################

# (none)
newcategory: EXT_shared_texture_palette

###############################################################################
#
# Extension #142 - GLX_SGIS_blended_overlay
# Extension #143 - GLX_SGIS_shared_multisample
#
###############################################################################

###############################################################################
#
# Extension #144
# EXT_separate_specular_color commands
#
###############################################################################

# (none)
newcategory: EXT_separate_specular_color

###############################################################################
#
# Extension #145
# EXT_secondary_color commands
#
###############################################################################

SecondaryColor3bEXT(red, green, blue)
	return		void
	param		red		ColorB in value
	param		green		ColorB in value
	param		blue		ColorB in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3bvEXT
	version		1.1
	offset		?

SecondaryColor3bvEXT(v)
	return		void
	param		v		ColorB in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColor3dEXT(red, green, blue)
	return		void
	param		red		ColorD in value
	param		green		ColorD in value
	param		blue		ColorD in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3dvEXT
	version		1.1
	offset		?

SecondaryColor3dvEXT(v)
	return		void
	param		v		ColorD in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColor3fEXT(red, green, blue)
	return		void
	param		red		ColorF in value
	param		green		ColorF in value
	param		blue		ColorF in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3fvEXT
	version		1.1
	offset		?

SecondaryColor3fvEXT(v)
	return		void
	param		v		ColorF in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColor3iEXT(red, green, blue)
	return		void
	param		red		ColorI in value
	param		green		ColorI in value
	param		blue		ColorI in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3ivEXT
	version		1.1
	offset		?

SecondaryColor3ivEXT(v)
	return		void
	param		v		ColorI in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColor3sEXT(red, green, blue)
	return		void
	param		red		ColorS in value
	param		green		ColorS in value
	param		blue		ColorS in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3svEXT
	version		1.1
	offset		?

SecondaryColor3svEXT(v)
	return		void
	param		v		ColorS in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColor3ubEXT(red, green, blue)
	return		void
	param		red		ColorUB in value
	param		green		ColorUB in value
	param		blue		ColorUB in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3ubvEXT
	version		1.1
	offset		?

SecondaryColor3ubvEXT(v)
	return		void
	param		v		ColorUB in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColor3uiEXT(red, green, blue)
	return		void
	param		red		ColorUI in value
	param		green		ColorUI in value
	param		blue		ColorUI in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3uivEXT
	version		1.1
	offset		?

SecondaryColor3uivEXT(v)
	return		void
	param		v		ColorUI in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColor3usEXT(red, green, blue)
	return		void
	param		red		ColorUS in value
	param		green		ColorUS in value
	param		blue		ColorUS in value
	category	EXT_secondary_color
	vectorequiv	SecondaryColor3usvEXT
	version		1.1
	offset		?

SecondaryColor3usvEXT(v)
	return		void
	param		v		ColorUS in array [3]
	category	EXT_secondary_color
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

# pointer is really 'in'
SecondaryColorPointerEXT(size, type, stride, pointer)
	return		void
	param		size	Int32 in value
	param		type	ColorPointerType in value
	param		stride	SizeI in value
	param		pointer Void out array [COMPSIZE(size/type/stride)] retained
	category	EXT_secondary_color
	dlflags		notlistable
	glxflags	client-handcode server-handcode EXT
	version		1.1
	extension
	glsflags	client
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #146
# EXT_texture_env commands
#
###############################################################################

# Dead extension - never implemented (removed from registry!)
# (none)
# newcategory: EXT_texture_env

###############################################################################
#
# Extension #147
# EXT_texture_perturb_normal commands
#
###############################################################################

TextureNormalEXT(mode)
	return		void
	param		mode		TextureNormalModeEXT in value
	category	EXT_texture_perturb_normal
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #148
# EXT_multi_draw_arrays commands
#
###############################################################################

# first and count are really 'in'
MultiDrawArraysEXT(mode, first, count, primcount)
	return		void
	param		mode		BeginMode in value
	param		first		Int32 out array [COMPSIZE(count)]
	param		count		SizeI out array [COMPSIZE(primcount)]
	param		primcount	SizeI in value
	category	EXT_multi_draw_arrays
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

MultiDrawElementsEXT(mode, count, type, indices, primcount)
	return		void
	param		mode		BeginMode in value
	param		count		SizeI in array [COMPSIZE(primcount)]
	param		type		DrawElementsType in value
	param		indices		VoidPointer in array [COMPSIZE(primcount)]
	param		primcount	SizeI in value
	category	EXT_multi_draw_arrays
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #149
# EXT_fog_coord commands
#
###############################################################################

FogCoordfEXT(coord)
	return		void
	param		coord		CoordF in value
	category	EXT_fog_coord
	version		1.1
	offset		545

FogCoordfvEXT(coord)
	return		void
	param		coord		CoordF in array [1]
	category	EXT_fog_coord
	version		1.1
	offset		546

FogCoorddEXT(coord)
	return		void
	param		coord		CoordD in value
	category	EXT_fog_coord
	version		1.1
	offset		547

FogCoorddvEXT(coord)
	return		void
	param		coord		CoordD in array [1]
	category	EXT_fog_coord
	version		1.1
	offset		548

FogCoordPointerEXT(type, stride, pointer)
	return		void
	param		type		FogPointerTypeEXT in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(type/stride)] retained
	category	EXT_fog_coord
	version		1.1
	offset		549

###############################################################################
#
# Extension #150 - skipped
# Extension #151 - skipped
# Extension #152 - skipped
# Extension #153 - skipped
# Extension #154 - skipped
#
###############################################################################

###############################################################################
#
# Extension #155
# REND_screen_coordinates commands
#
###############################################################################

# (none)
newcategory: REND_screen_coordinates

###############################################################################
#
# Extension #156
# EXT_coordinate_frame commands
#
###############################################################################

Tangent3bEXT(tx, ty, tz)
	return		void
	param		tx		Int8 in value
	param		ty		Int8 in value
	param		tz		Int8 in value
	category	EXT_coordinate_frame
	vectorequiv	Tangent3bvEXT
	version		1.1
	offset		?

Tangent3bvEXT(v)
	return		void
	param		v		Int8 in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Tangent3dEXT(tx, ty, tz)
	return		void
	param		tx		CoordD in value
	param		ty		CoordD in value
	param		tz		CoordD in value
	category	EXT_coordinate_frame
	vectorequiv	Tangent3dvEXT
	version		1.1
	offset		?

Tangent3dvEXT(v)
	return		void
	param		v		CoordD in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Tangent3fEXT(tx, ty, tz)
	return		void
	param		tx		CoordF in value
	param		ty		CoordF in value
	param		tz		CoordF in value
	category	EXT_coordinate_frame
	vectorequiv	Tangent3fvEXT
	version		1.1
	offset		?

Tangent3fvEXT(v)
	return		void
	param		v		CoordF in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Tangent3iEXT(tx, ty, tz)
	return		void
	param		tx		Int32 in value
	param		ty		Int32 in value
	param		tz		Int32 in value
	category	EXT_coordinate_frame
	vectorequiv	Tangent3ivEXT
	version		1.1
	offset		?

Tangent3ivEXT(v)
	return		void
	param		v		Int32 in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Tangent3sEXT(tx, ty, tz)
	return		void
	param		tx		Int16 in value
	param		ty		Int16 in value
	param		tz		Int16 in value
	category	EXT_coordinate_frame
	vectorequiv	Tangent3svEXT
	version		1.1
	offset		?

Tangent3svEXT(v)
	return		void
	param		v		Int16 in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Binormal3bEXT(bx, by, bz)
	return		void
	param		bx		Int8 in value
	param		by		Int8 in value
	param		bz		Int8 in value
	category	EXT_coordinate_frame
	vectorequiv	Binormal3bvEXT
	version		1.1
	offset		?

Binormal3bvEXT(v)
	return		void
	param		v		Int8 in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Binormal3dEXT(bx, by, bz)
	return		void
	param		bx		CoordD in value
	param		by		CoordD in value
	param		bz		CoordD in value
	category	EXT_coordinate_frame
	vectorequiv	Binormal3dvEXT
	version		1.1
	offset		?

Binormal3dvEXT(v)
	return		void
	param		v		CoordD in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Binormal3fEXT(bx, by, bz)
	return		void
	param		bx		CoordF in value
	param		by		CoordF in value
	param		bz		CoordF in value
	category	EXT_coordinate_frame
	vectorequiv	Binormal3fvEXT
	version		1.1
	offset		?

Binormal3fvEXT(v)
	return		void
	param		v		CoordF in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Binormal3iEXT(bx, by, bz)
	return		void
	param		bx		Int32 in value
	param		by		Int32 in value
	param		bz		Int32 in value
	category	EXT_coordinate_frame
	vectorequiv	Binormal3ivEXT
	version		1.1
	offset		?

Binormal3ivEXT(v)
	return		void
	param		v		Int32 in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Binormal3sEXT(bx, by, bz)
	return		void
	param		bx		Int16 in value
	param		by		Int16 in value
	param		bz		Int16 in value
	category	EXT_coordinate_frame
	vectorequiv	Binormal3svEXT
	version		1.1
	offset		?

Binormal3svEXT(v)
	return		void
	param		v		Int16 in array [3]
	category	EXT_coordinate_frame
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TangentPointerEXT(type, stride, pointer)
	return		void
	param		type		TangentPointerTypeEXT in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(type/stride)] retained
	category	EXT_coordinate_frame
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsopcode	?
	offset		?

BinormalPointerEXT(type, stride, pointer)
	return		void
	param		type		BinormalPointerTypeEXT in value
	param		stride		SizeI in value
	param		pointer		Void in array [COMPSIZE(type/stride)] retained
	category	EXT_coordinate_frame
	dlflags		notlistable
	glxflags	client-handcode client-intercept server-handcode
	version		1.1
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #157 - skipped
#
###############################################################################

###############################################################################
#
# Extension #158
# EXT_texture_env_combine commands
#
###############################################################################

# (none)
newcategory: EXT_texture_env_combine

###############################################################################
#
# Extension #159
# APPLE_specular_vector commands
#
###############################################################################

# (none)
newcategory: APPLE_specular_vector

###############################################################################
#
# Extension #160
# APPLE_transform_hint commands
#
###############################################################################

# (none)
newcategory: APPLE_transform_hint

###############################################################################
#
# Extension #161
# SGIX_fog_scale commands
#
###############################################################################

# (none)
newcategory: SGIX_fog_scale

###############################################################################
#
# Extension #162 - skipped
#
###############################################################################

###############################################################################
#
# Extension #163
# SUNX_constant_data commands
#
###############################################################################

FinishTextureSUNX()
	return		void
	category	SUNX_constant_data
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #164
# SUN_global_alpha commands
#
###############################################################################

GlobalAlphaFactorbSUN(factor)
	return		void
	param		factor		Int8 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GlobalAlphaFactorsSUN(factor)
	return		void
	param		factor		Int16 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GlobalAlphaFactoriSUN(factor)
	return		void
	param		factor		Int32 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GlobalAlphaFactorfSUN(factor)
	return		void
	param		factor		Float32 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GlobalAlphaFactordSUN(factor)
	return		void
	param		factor		Float64 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GlobalAlphaFactorubSUN(factor)
	return		void
	param		factor		UInt8 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GlobalAlphaFactorusSUN(factor)
	return		void
	param		factor		UInt16 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

GlobalAlphaFactoruiSUN(factor)
	return		void
	param		factor		UInt32 in value
	category	SUN_global_alpha
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #165
# SUN_triangle_list commands
#
###############################################################################

ReplacementCodeuiSUN(code)
	return		void
	param		code		UInt32 in value
	category	SUN_triangle_list
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeusSUN(code)
	return		void
	param		code		UInt16 in value
	category	SUN_triangle_list
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeubSUN(code)
	return		void
	param		code		UInt8 in value
	category	SUN_triangle_list
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuivSUN(code)
	return		void
	param		code		UInt32 in array [COMPSIZE()]
	category	SUN_triangle_list
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeusvSUN(code)
	return		void
	param		code		UInt16 in array [COMPSIZE()]
	category	SUN_triangle_list
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeubvSUN(code)
	return		void
	param		code		UInt8 in array [COMPSIZE()]
	category	SUN_triangle_list
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodePointerSUN(type, stride, pointer)
	return		void
	param		type		ReplacementCodeTypeSUN in value
	param		stride		SizeI in value
	param		pointer		VoidPointer in array [COMPSIZE(type/stride)] retained
	category	SUN_triangle_list
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #166
# SUN_vertex commands
#
###############################################################################

Color4ubVertex2fSUN(r, g, b, a, x, y)
	return		void
	param		r		UInt8 in value
	param		g		UInt8 in value
	param		b		UInt8 in value
	param		a		UInt8 in value
	param		x		Float32 in value
	param		y		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Color4ubVertex2fvSUN(c, v)
	return		void
	param		c		UInt8 in array [4]
	param		v		Float32 in array [2]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Color4ubVertex3fSUN(r, g, b, a, x, y, z)
	return		void
	param		r		UInt8 in value
	param		g		UInt8 in value
	param		b		UInt8 in value
	param		a		UInt8 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Color4ubVertex3fvSUN(c, v)
	return		void
	param		c		UInt8 in array [4]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Color3fVertex3fSUN(r, g, b, x, y, z)
	return		void
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Color3fVertex3fvSUN(c, v)
	return		void
	param		c		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Normal3fVertex3fSUN(nx, ny, nz, x, y, z)
	return		void
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Normal3fVertex3fvSUN(n, v)
	return		void
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Color4fNormal3fVertex3fSUN(r, g, b, a, nx, ny, nz, x, y, z)
	return		void
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		a		Float32 in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

Color4fNormal3fVertex3fvSUN(c, n, v)
	return		void
	param		c		Float32 in array [4]
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fVertex3fSUN(s, t, x, y, z)
	return		void
	param		s		Float32 in value
	param		t		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fVertex3fvSUN(tc, v)
	return		void
	param		tc		Float32 in array [2]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord4fVertex4fSUN(s, t, p, q, x, y, z, w)
	return		void
	param		s		Float32 in value
	param		t		Float32 in value
	param		p		Float32 in value
	param		q		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	param		w		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord4fVertex4fvSUN(tc, v)
	return		void
	param		tc		Float32 in array [4]
	param		v		Float32 in array [4]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fColor4ubVertex3fSUN(s, t, r, g, b, a, x, y, z)
	return		void
	param		s		Float32 in value
	param		t		Float32 in value
	param		r		UInt8 in value
	param		g		UInt8 in value
	param		b		UInt8 in value
	param		a		UInt8 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fColor4ubVertex3fvSUN(tc, c, v)
	return		void
	param		tc		Float32 in array [2]
	param		c		UInt8 in array [4]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fColor3fVertex3fSUN(s, t, r, g, b, x, y, z)
	return		void
	param		s		Float32 in value
	param		t		Float32 in value
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fColor3fVertex3fvSUN(tc, c, v)
	return		void
	param		tc		Float32 in array [2]
	param		c		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fNormal3fVertex3fSUN(s, t, nx, ny, nz, x, y, z)
	return		void
	param		s		Float32 in value
	param		t		Float32 in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fNormal3fVertex3fvSUN(tc, n, v)
	return		void
	param		tc		Float32 in array [2]
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fColor4fNormal3fVertex3fSUN(s, t, r, g, b, a, nx, ny, nz, x, y, z)
	return		void
	param		s		Float32 in value
	param		t		Float32 in value
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		a		Float32 in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord2fColor4fNormal3fVertex3fvSUN(tc, c, n, v)
	return		void
	param		tc		Float32 in array [2]
	param		c		Float32 in array [4]
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord4fColor4fNormal3fVertex4fSUN(s, t, p, q, r, g, b, a, nx, ny, nz, x, y, z, w)
	return		void
	param		s		Float32 in value
	param		t		Float32 in value
	param		p		Float32 in value
	param		q		Float32 in value
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		a		Float32 in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	param		w		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoord4fColor4fNormal3fVertex4fvSUN(tc, c, n, v)
	return		void
	param		tc		Float32 in array [4]
	param		c		Float32 in array [4]
	param		n		Float32 in array [3]
	param		v		Float32 in array [4]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiVertex3fSUN(rc, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiVertex3fvSUN(rc, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiColor4ubVertex3fSUN(rc, r, g, b, a, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		r		UInt8 in value
	param		g		UInt8 in value
	param		b		UInt8 in value
	param		a		UInt8 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiColor4ubVertex3fvSUN(rc, c, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		c		UInt8 in array [4]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiColor3fVertex3fSUN(rc, r, g, b, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiColor3fVertex3fvSUN(rc, c, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		c		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiNormal3fVertex3fSUN(rc, nx, ny, nz, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiNormal3fVertex3fvSUN(rc, n, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiColor4fNormal3fVertex3fSUN(rc, r, g, b, a, nx, ny, nz, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		a		Float32 in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiColor4fNormal3fVertex3fvSUN(rc, c, n, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		c		Float32 in array [4]
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiTexCoord2fVertex3fSUN(rc, s, t, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		s		Float32 in value
	param		t		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiTexCoord2fVertex3fvSUN(rc, tc, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		tc		Float32 in array [2]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN(rc, s, t, nx, ny, nz, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		s		Float32 in value
	param		t		Float32 in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(rc, tc, n, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		tc		Float32 in array [2]
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN(rc, s, t, r, g, b, a, nx, ny, nz, x, y, z)
	return		void
	param		rc		ReplacementCodeTypeSUN in value
	param		s		Float32 in value
	param		t		Float32 in value
	param		r		Float32 in value
	param		g		Float32 in value
	param		b		Float32 in value
	param		a		Float32 in value
	param		nx		Float32 in value
	param		ny		Float32 in value
	param		nz		Float32 in value
	param		x		Float32 in value
	param		y		Float32 in value
	param		z		Float32 in value
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(rc, tc, c, n, v)
	return		void
	param		rc		ReplacementCodeTypeSUN in array [1]
	param		tc		Float32 in array [2]
	param		c		Float32 in array [4]
	param		n		Float32 in array [3]
	param		v		Float32 in array [3]
	category	SUN_vertex
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #167 - WGL_EXT_display_color_table
# Extension #168 - WGL_EXT_extensions_string
# Extension #169 - WGL_EXT_make_current_read
# Extension #170 - WGL_EXT_pixel_format
# Extension #171 - WGL_EXT_pbuffer
# Extension #172 - WGL_EXT_swap_control
#
###############################################################################

###############################################################################
#
# Extension #173
# EXT_blend_func_separate commands
#
###############################################################################

BlendFuncSeparateEXT(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha)
	return		void
	param		sfactorRGB	BlendFuncSeparateParameterEXT in value
	param		dfactorRGB	BlendFuncSeparateParameterEXT in value
	param		sfactorAlpha	BlendFuncSeparateParameterEXT in value
	param		dfactorAlpha	BlendFuncSeparateParameterEXT in value
	category	EXT_blend_func_separate
	glxropcode	4134
	glxflags	ignore
	version		1.0
	extension
	offset		537

###############################################################################
#
# Extension #174
# INGR_color_clamp commands
#
###############################################################################

# (none)
newcategory: INGR_color_clamp

###############################################################################
#
# Extension #175
# INGR_interlace_read commands
#
###############################################################################

# (none)
newcategory: INGR_interlace_read

###############################################################################
#
# Extension #176
# EXT_stencil_wrap commands
#
###############################################################################

# (none)
newcategory: EXT_stencil_wrap

###############################################################################
#
# Extension #177 - skipped
#
###############################################################################

###############################################################################
#
# Extension #178
# EXT_422_pixels commands
#
###############################################################################

# (none)
newcategory: EXT_422_pixels

###############################################################################
#
# Extension #179
# NV_texgen_reflection commands
#
###############################################################################

# (none)
newcategory: NV_texgen_reflection

###############################################################################
#
# Extension #???
# @   EXT_texture_cube_map commands
#
###############################################################################

# (none)

###############################################################################
#
# Extension #180 - skipped
# Extension #181 - skipped
#
###############################################################################

###############################################################################
#
# Extension #182
# SUN_convolution_border_modes commands
#
###############################################################################

# (none)
newcategory: SUN_convolution_border_modes

###############################################################################
#
# Extension #183 - GLX_SUN_transparent_index
# Extension #184 - skipped
#
###############################################################################

###############################################################################
#
# Extension #185
# EXT_texture_env_add commands
#
###############################################################################

# (none)
newcategory: EXT_texture_env_add

###############################################################################
#
# Extension #186
# EXT_texture_lod_bias commands
#
###############################################################################

# (none)
newcategory: EXT_texture_lod_bias

###############################################################################
#
# Extension #187
# EXT_texture_filter_anisotropic commands
#
###############################################################################

# (none)
newcategory: EXT_texture_filter_anisotropic

###############################################################################
#
# Extension #188
# EXT_vertex_weighting commands
#
###############################################################################

# GLX stuff to be written
VertexWeightfEXT(weight)
	return		void
	param		weight		Float32 in value
	category	EXT_vertex_weighting
	vectorequiv	VertexWeightfvEXT
	version		1.1
	extension	soft WINSOFT NV10
	glxflags	ignore
	glsflags	ignore
	offset		494

VertexWeightfvEXT(weight)
	return		void
	param		weight		Float32 in array [1]
	category	EXT_vertex_weighting
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4135
	glxflags	ignore
	glsflags	ignore
	offset		495

VertexWeightPointerEXT(size, type, stride, pointer)
	return		void
	param		size	SizeI in value
	param		type	VertexWeightPointerTypeEXT in value
	param		stride	SizeI in value
	param		pointer Void in array [COMPSIZE(type/stride)] retained
	category	EXT_vertex_weighting
	version		1.1
	extension	soft WINSOFT NV10
	dlflags		notlistable
	glxflags	ignore
	glsflags	client-state
	offset		496

###############################################################################
#
# Extension #189
# NV_light_max_exponent commands
#
###############################################################################

# (none)
newcategory: NV_light_max_exponent

###############################################################################
#
# Extension #190
# NV_vertex_array_range commands
#
###############################################################################

FlushVertexArrayRangeNV()
	return		void
	category	NV_vertex_array_range
	version		1.1
	extension	soft WINSOFT NV10
	dlflags		notlistable
	glxflags	client-handcode server-handcode ignore
	glsflags	client-state
	offset		497

VertexArrayRangeNV(size, pointer)
	return		void
	param		size	SizeI in value
	param		pointer Void in array [COMPSIZE(size)] retained
	category	NV_vertex_array_range
	version		1.1
	extension	soft WINSOFT NV10
	dlflags		notlistable
	glxflags	client-handcode server-handcode ignore
	glsflags	client-state
	offset		498

# wglAllocateMemoryNV(size, readfreq, writefreq, priority)
#	return		VoidPointer
#	param		size	SizeI in value
#	param		readfreq	Float32 in value
#	param		writefreq	Float32 in value
#	param		priority	Float32 in value
#	category	NV_vertex_array_range
#	version		1.1
#	extension	soft WINSOFT NV10
#	dlflags		notlistable
#	glxflags	client-handcode server-handcode ignore
#	glsflags	client-state
#
# wglFreeMemoryNV(pointer)
#	return		void
#	param		pointer Void out array [1]
#	category	NV_vertex_array_range
#	version		1.1
#	extension	soft WINSOFT NV10
#	dlflags		notlistable
#	glxflags	client-handcode server-handcode ignore
#	glsflags	client-state

###############################################################################
#
# Extension #191
# NV_register_combiners commands
#
###############################################################################

CombinerParameterfvNV(pname, params)
	return		void
	param		pname			CombinerParameterNV in value
	param		params			CheckedFloat32 in array [COMPSIZE(pname)]
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4137
	glxflags	ignore
	glsflags	ignore
	offset		499

CombinerParameterfNV(pname, param)
	return		void
	param		pname			CombinerParameterNV in value
	param		param			Float32 in value
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4136
	glxflags	ignore
	glsflags	ignore
	offset		500

CombinerParameterivNV(pname, params)
	return		void
	param		pname			CombinerParameterNV in value
	param		params			CheckedInt32 in array [COMPSIZE(pname)]
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4139
	glxflags	ignore
	glsflags	ignore
	offset		501

CombinerParameteriNV(pname, param)
	return		void
	param		pname			CombinerParameterNV in value
	param		param			Int32 in value
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4138
	glxflags	ignore
	glsflags	ignore
	offset		502

CombinerInputNV(stage, portion, variable, input, mapping, componentUsage)
	return		void
	param		stage			CombinerStageNV in value
	param		portion			CombinerPortionNV in value
	param		variable		CombinerVariableNV in value
	param		input			CombinerRegisterNV in value
	param		mapping			CombinerMappingNV in value
	param		componentUsage		CombinerComponentUsageNV in value
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4140
	glxflags	ignore
	glsflags	ignore
	offset		503

CombinerOutputNV(stage, portion, abOutput, cdOutput, sumOutput, scale, bias, abDotProduct, cdDotProduct, muxSum)
	return		void
	param		stage			CombinerStageNV in value
	param		portion			CombinerPortionNV in value
	param		abOutput		CombinerRegisterNV in value
	param		cdOutput		CombinerRegisterNV in value
	param		sumOutput		CombinerRegisterNV in value
	param		scale			CombinerScaleNV in value
	param		bias			CombinerBiasNV in value
	param		abDotProduct		Boolean in value
	param		cdDotProduct		Boolean in value
	param		muxSum			Boolean in value
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4141
	glxflags	ignore
	glsflags	ignore
	offset		504

FinalCombinerInputNV(variable, input, mapping, componentUsage)
	return		void
	param		variable		CombinerVariableNV in value
	param		input			CombinerRegisterNV in value
	param		mapping			CombinerMappingNV in value
	param		componentUsage		CombinerComponentUsageNV in value
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxropcode	4142
	glxflags	ignore
	glsflags	ignore
	offset		505

GetCombinerInputParameterfvNV(stage, portion, variable, pname, params)
	return		void
	param		stage			CombinerStageNV in value
	param		portion			CombinerPortionNV in value
	param		variable		CombinerVariableNV in value
	param		pname			CombinerParameterNV in value
	param		params			Float32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxvendorpriv	1270
	glxflags	ignore
	glsflags	ignore
	offset		506

GetCombinerInputParameterivNV(stage, portion, variable, pname, params)
	return		void
	param		stage			CombinerStageNV in value
	param		portion			CombinerPortionNV in value
	param		variable		CombinerVariableNV in value
	param		pname			CombinerParameterNV in value
	param		params			Int32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxvendorpriv	1271
	glxflags	ignore
	glsflags	ignore
	offset		507

GetCombinerOutputParameterfvNV(stage, portion, pname, params)
	return		void
	param		stage			CombinerStageNV in value
	param		portion			CombinerPortionNV in value
	param		pname			CombinerParameterNV in value
	param		params			Float32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxvendorpriv	1272
	glxflags	ignore
	glsflags	ignore
	offset		508

GetCombinerOutputParameterivNV(stage, portion, pname, params)
	return		void
	param		stage			CombinerStageNV in value
	param		portion			CombinerPortionNV in value
	param		pname			CombinerParameterNV in value
	param		params			Int32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxvendorpriv	1273
	glxflags	ignore
	glsflags	ignore
	offset		509

GetFinalCombinerInputParameterfvNV(variable, pname, params)
	return		void
	param		variable		CombinerVariableNV in value
	param		pname			CombinerParameterNV in value
	param		params			Float32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxvendorpriv	1274
	glxflags	ignore
	glsflags	ignore
	offset		510

GetFinalCombinerInputParameterivNV(variable, pname, params)
	return		void
	param		variable		CombinerVariableNV in value
	param		pname			CombinerParameterNV in value
	param		params			Int32 out array [COMPSIZE(pname)]
	dlflags		notlistable
	category	NV_register_combiners
	version		1.1
	extension	soft WINSOFT NV10
	glxvendorpriv	1275
	glxflags	ignore
	glsflags	ignore
	offset		511

###############################################################################
#
# Extension #192
# NV_fog_distance commands
#
###############################################################################

# (none)
newcategory: NV_fog_distance

###############################################################################
#
# Extension #193
# NV_texgen_emboss commands
#
###############################################################################

# (none)
newcategory: NV_texgen_emboss

###############################################################################
#
# Extension #194
# NV_blend_square commands
#
###############################################################################

# (none)
newcategory: NV_blend_square

###############################################################################
#
# Extension #195
# NV_texture_env_combine4 commands
#
###############################################################################

# (none)
newcategory: NV_texture_env_combine4

###############################################################################
#
# Extension #196
# MESA_resize_buffers commands
#
###############################################################################

ResizeBuffersMESA()
	return		void
	category	MESA_resize_buffers
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		512

###############################################################################
#
# Extension #197
# MESA_window_pos commands
#
###############################################################################

WindowPos2dMESA(x, y)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	category	MESA_window_pos
	vectorequiv	WindowPos2dv
	version		1.0
	offset		513

WindowPos2dvMESA(v)
	return		void
	param		v		CoordD in array [2]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		514

WindowPos2fMESA(x, y)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	category	MESA_window_pos
	vectorequiv	WindowPos2fv
	version		1.0
	offset		515

WindowPos2fvMESA(v)
	return		void
	param		v		CoordF in array [2]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		516

WindowPos2iMESA(x, y)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	category	MESA_window_pos
	vectorequiv	WindowPos2iv
	version		1.0
	offset		517

WindowPos2ivMESA(v)
	return		void
	param		v		CoordI in array [2]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		518

WindowPos2sMESA(x, y)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	category	MESA_window_pos
	vectorequiv	WindowPos2sv
	version		1.0
	offset		519

WindowPos2svMESA(v)
	return		void
	param		v		CoordS in array [2]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		520

WindowPos3dMESA(x, y, z)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	param		z		CoordD in value
	vectorequiv	WindowPos3dv
	category	MESA_window_pos
	version		1.0
	offset		521

WindowPos3dvMESA(v)
	return		void
	param		v		CoordD in array [3]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		522

WindowPos3fMESA(x, y, z)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	param		z		CoordF in value
	category	MESA_window_pos
	vectorequiv	WindowPos3fv
	version		1.0
	offset		523

WindowPos3fvMESA(v)
	return		void
	param		v		CoordF in array [3]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		524

WindowPos3iMESA(x, y, z)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	param		z		CoordI in value
	category	MESA_window_pos
	vectorequiv	WindowPos3iv
	version		1.0
	offset		525

WindowPos3ivMESA(v)
	return		void
	param		v		CoordI in array [3]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		526

WindowPos3sMESA(x, y, z)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	param		z		CoordS in value
	category	MESA_window_pos
	vectorequiv	WindowPos3sv
	version		1.0
	offset		527

WindowPos3svMESA(v)
	return		void
	param		v		CoordS in array [3]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		528

WindowPos4dMESA(x, y, z, w)
	return		void
	param		x		CoordD in value
	param		y		CoordD in value
	param		z		CoordD in value
	param		w		CoordD in value
	vectorequiv	WindowPos4dv
	category	MESA_window_pos
	version		1.0
	offset		529

WindowPos4dvMESA(v)
	return		void
	param		v		CoordD in array [4]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		530

WindowPos4fMESA(x, y, z, w)
	return		void
	param		x		CoordF in value
	param		y		CoordF in value
	param		z		CoordF in value
	param		w		CoordF in value
	category	MESA_window_pos
	vectorequiv	WindowPos4fv
	version		1.0
	offset		531

WindowPos4fvMESA(v)
	return		void
	param		v		CoordF in array [4]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		532

WindowPos4iMESA(x, y, z, w)
	return		void
	param		x		CoordI in value
	param		y		CoordI in value
	param		z		CoordI in value
	param		w		CoordI in value
	category	MESA_window_pos
	vectorequiv	WindowPos4iv
	version		1.0
	offset		533

WindowPos4ivMESA(v)
	return		void
	param		v		CoordI in array [4]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		534

WindowPos4sMESA(x, y, z, w)
	return		void
	param		x		CoordS in value
	param		y		CoordS in value
	param		z		CoordS in value
	param		w		CoordS in value
	category	MESA_window_pos
	vectorequiv	WindowPos4sv
	version		1.0
	offset		535

WindowPos4svMESA(v)
	return		void
	param		v		CoordS in array [4]
	category	MESA_window_pos
	version		1.0
	glxropcode	?
	glsopcode	?
	offset		536

###############################################################################
#
# Extension #198
# EXT_texture_compression_s3tc commands
#
###############################################################################

#@@ (none yet)

###############################################################################
#
# Extension #199
# IBM_cull_vertex commands
#
###############################################################################

# (none)
newcategory: IBM_cull_vertex

###############################################################################
#
# Extension #200
# IBM_multimode_draw_arrays commands
#
###############################################################################

MultiModeDrawArraysIBM(mode, first, count, primcount, modestride)
	return		void
	param		mode		BeginMode in value
	param		first		Int32 in array [COMPSIZE(primcount)]
	param		count		SizeI in array [COMPSIZE(primcount)]
	param		primcount	SizeI in value
	param		modestride	Int32 in value
	category	IBM_multimode_draw_arrays
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?


MultiModeDrawElementsIBM(mode, count, type, indices, primcount, modestride)
	return		void
	param		mode		BeginMode in array [COMPSIZE(primcount)]
	param		count		SizeI in array [COMPSIZE(primcount)]
	param		type		DrawElementsType in value
	param		indices		VoidPointer in array [COMPSIZE(primcount)]
	param		primcount	SizeI in value
	param		modestride	Int32 in value
	category	IBM_multimode_draw_arrays
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #201
# IBM_vertex_array_lists commands
#
###############################################################################

ColorPointerListIBM(size, type, stride, pointer, ptrstride)
	return		void
	param		size		Int32 in value
	param		type		ColorPointerType in value
	param		stride		Int32 in value
	param		pointer		VoidPointer in array [COMPSIZE(size/type/stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

SecondaryColorPointerListIBM(size, type, stride, pointer, ptrstride)
	return		void
	param		size		Int32 in value
	param		type		SecondaryColorPointerTypeIBM in value
	param		stride		Int32 in value
	param		pointer		VoidPointer in array [COMPSIZE(size/type/stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

EdgeFlagPointerListIBM(stride, pointer, ptrstride)
	return		void
	param		stride		Int32 in value
	param		pointer		BooleanPointer in array [COMPSIZE(stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

FogCoordPointerListIBM(type, stride, pointer, ptrstride)
	return		void
	param		type		FogPointerTypeIBM in value
	param		stride		Int32 in value
	param		pointer		VoidPointer in array [COMPSIZE(type/stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

IndexPointerListIBM(type, stride, pointer, ptrstride)
	return		void
	param		type		IndexPointerType in value
	param		stride		Int32 in value
	param		pointer		VoidPointer in array [COMPSIZE(type/stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

NormalPointerListIBM(type, stride, pointer, ptrstride)
	return		void
	param		type		NormalPointerType in value
	param		stride		Int32 in value
	param		pointer		VoidPointer in array [COMPSIZE(type/stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

TexCoordPointerListIBM(size, type, stride, pointer, ptrstride)
	return		void
	param		size		Int32 in value
	param		type		TexCoordPointerType in value
	param		stride		Int32 in value
	param		pointer		VoidPointer in array [COMPSIZE(size/type/stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

VertexPointerListIBM(size, type, stride, pointer, ptrstride)
	return		void
	param		size		Int32 in value
	param		type		VertexPointerType in value
	param		stride		Int32 in value
	param		pointer		VoidPointer in array [COMPSIZE(size/type/stride)] retained
	param		ptrstride	Int32 in value
	category	IBM_vertex_array_lists
	version		1.1
	glxropcode	?
	glsopcode	?
	offset		?

###############################################################################
#
# Extension #202
# SGIX_subsample commands
#
###############################################################################

# (none)
newcategory: SGIX_subsample

###############################################################################
#
# Extension #203
# SGIX_ycrcba commands
#
###############################################################################

# (none)
newcategory: SGIX_ycrcba

###############################################################################
#
# Extension #204
# SGIX_ycrcb_subsample commands
#
###############################################################################

# (none)
newcategory: SGIX_ycrcb_subsample

###############################################################################
#
# Extension #205
# SGIX_depth_pass_instrument commands
#
###############################################################################

# (none)
newcategory: SGIX_depth_pass_instrument

###############################################################################
#
# Extension #206
# 3DFX_texture_compression_FXT1 commands
#
###############################################################################

# (none)
newcategory: 3DFX_texture_compression_FXT1

###############################################################################
#
# Extension #207
# 3DFX_multisample commands
#
###############################################################################

# (none)
newcategory: 3DFX_multisample

###############################################################################
#
# Extension #208
# 3DFX_tbuffer commands
#
###############################################################################

TbufferMask3DFX(mask)
	return		void
	param		mask		UInt32 in value
	category	3DFX_tbuffer
	version		1.2
	glxropcode	?
	glsopcode	?
	offset		553

###############################################################################
#
# Extension #209
# EXT_multisample commands
#
###############################################################################

SampleMaskEXT(value, invert)
	return		void
	param		value	ClampedFloat32 in value
	param		invert	Boolean in value
	category	EXT_multisample
	version		1.1
	glxropcode	?
	extension
	glsopcode	?
	alias		SampleMaskSGIS

SamplePatternEXT(pattern)
	return		void
	param		pattern SamplePatternEXT in value
	category	EXT_multisample
	version		1.0
	glxropcode	?
	glxflags
	extension
	glsopcode	?
	alias		SamplePatternSGIS

###############################################################################
#
# Extension #210
# SGI_vertex_preclip commands
#
###############################################################################

# (none)
newcategory: SGI_vertex_preclip

###############################################################################
#
# Extension #211
# SGIX_convolution_accuracy commands
#
###############################################################################

# (none)
newcategory: SGIX_convolution_accuracy

###############################################################################
#
# Extension #212
# SGIX_resample commands
#
###############################################################################

# (none)
newcategory: SGIX_resample

###############################################################################
#
# Extension #213
# SGIS_point_line_texgen commands
#
###############################################################################

# (none)
newcategory: SGIS_point_line_texgen

###############################################################################
#
# Extension #214
# SGIS_texture_color_mask commands
#
###############################################################################

# (none)
newcategory: SGIS_texture_color_mask
TextureColorMaskSGIS(red, green, blue, alpha)
	return		void
	param		red		Boolean in value
	param		green		Boolean in value
	param		blue		Boolean in value
	param		alpha		Boolean in value
	category	SGIS_texture_color_mask
	version		1.1
	glxropcode	2082
	glsopcode	?
	offset		?
