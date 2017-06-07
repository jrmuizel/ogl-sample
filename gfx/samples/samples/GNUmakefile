#!gmake
#
# $Header: //depot/main/gfx/samples/samples/GNUmakefile#6 $

include $(ROOT)/usr/include/make/commondefs

CFILES = \
	abgr.c \
	accum.c \
	bitmap1.c \
	bitmap2.c \
	blendxor.c \
	copy.c \
	copytex.c \
	depth.c \
	eval.c \
	fog.c \
	line.c \
	logo.c \
	nurb.c \
	point.c \
	prim.c \
	quad.c \
	select.c \
	shell.c \
	sphere.c \
	star.c \
	stencil.c \
	stretch.c \
	surfgrid.c \
	teapot.c \
	texenv.c \
	texobj.c \
	texproxy.c \
	texsubimage.c \
	texture.c \
	tri.c \
	twotextures.c \
	varray.c \
	wave.c \
	$(NULL)

LLDLIBS = -lrgb -lglut -lGLU -lGL -lXmu -lXt -lXext -lX11 -lm

TARGETS = $(subst .c,,$(CFILES))

default apps: $(TARGETS)

headers headers_install libs libs_install install:

%: %.c
	$(CCF) -o $@ $@.c $(LDFLAGS);

include $(COMMONRULES)
