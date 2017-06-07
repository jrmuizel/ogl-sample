#!gmake
#
# License Applicability. Except to the extent portions of this file are
# made subject to an alternative license as permitted in the SGI Free
# Software License B, Version 1.0 (the "License"), the contents of this
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
# Additional Notice Provisions: The application programming interfaces
# established by SGI in conjunction with the Original Code are The
# OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
# April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
# 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
# Window System(R) (Version 1.3), released October 19, 1998. This software
# was created using the OpenGL(R) version 1.2.1 Sample Implementation
# published by SGI, but has not been independently verified as being
# compliant with the OpenGL(R) version 1.2.1 Specification.
#
# $Date$ $Revision$
# $Header: //depot/main/gfx/samples/redbook_2ed/GNUmakefile#8 $

include $(ROOT)/usr/include/make/commondefs

CFILES = \
	aaindex.c \
	aapoly.c \
	aargb.c \
	accanti.c \
	accpersp.c \
	alpha.c \
	alpha3D.c \
	bezcurve.c \
	bezmesh.c \
	bezsurf.c \
	checker.c \
	clip.c \
	colormat.c \
	cube.c \
	dof.c \
	double.c \
	drawf.c \
	feedback.c \
	fog.c \
	fogindex.c \
	font.c \
	hello.c \
	image.c \
	light.c \
	lines.c \
	list.c \
	material.c \
	mipmap.c \
	model.c \
	movelight.c \
	pickdepth.c \
	picksquare.c \
	planet.c \
	polyoff.c \
	polys.c \
	quadric.c \
	robot.c \
	scene.c \
	select.c \
	smooth.c \
	stencil.c \
	stroke.c \
	surface.c \
	teapots.c \
	tess.c \
	tesswind.c \
	texbind.c \
	texgen.c \
	texprox.c \
	texsub.c \
	texturesurf.c \
	torus.c \
	trim.c \
	unproject.c \
	varray.c \
	wrap.c \
	$(NULL)

LLDLIBS = -lglut -lGLU -lGL -lXmu -lXt -lXext -lX11 -lm

TARGETS = $(subst .c,,$(CFILES))

default apps: $(TARGETS)

headers headers_install libs libs_install install:

%: %.c
	$(CCF) -o $@ $@.c $(LDFLAGS)

include $(COMMONRULES)
