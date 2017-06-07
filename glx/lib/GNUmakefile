#!gmake
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
# $Header: //depot/main/glx/lib/GNUmakefile#10 $

COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs
include $(ROOT)/usr/include/libspec/libspecdefs

TARGET = libGL.so

GEN_CFILES =		 \
	g_render.c	 \
	g_single.c	 \
	g_vendpriv.c	 \
	$(NULL)

CFILES =		 \
	clientattrib.c	 \
	compsize.c	 \
	eval.c		 \
	glxcmds.c	 \
	glxext.c	 \
	pixel.c		 \
	pixelstore.c	 \
	render2.c	 \
	renderpix.c	 \
	single2.c	 \
	singlepix.c	 \
	vertarr.c	 \
	$(GEN_CFILES)	 \
	$(NULL)

GENUTIL = listutil.ls
PROTOCOLGEN = lcomp.ls

ARFLAGS = crlu

LCINCS = -I -I. -I../include -I$(XTOPDIR)/xc/include/extensions \
	 -I$(TOPDIR)/gfx/lib/opengl/include -I$(ROOT)/usr/include

LCDEFS = -U__GL_USE_OUR_PROTOTYPES -D__STDC__=1 -U__EXTENSIONS__
LCDEFS += -D__GLX_ALIGN64

DSONAME = $(TARGET)

TARGETS = $(GEN_CFILES) $(TARGET)

default libs: $(TARGETS)

headers headers_install apps:

libs_install install: default
	$(INSTALL) -F $(LIB_LOCATION) $(TARGET)

distsi: $(GEN_CFILES)
	$(MAKE) $(COMMONPREF)$@

distoss:
	$(MAKE) $(COMMONPREF)$@

$(TARGET): $(OBJECTS)
	$(LD) $(LDDSOFLAGS) $(OBJECTS) -o $@

g_render.c: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) $(PROTOCOLGEN) \
	    $(GENUTIL) $(LIBSPEC_CUTILS) $(OSSCOPYRIGHT)
	rm -f $@ $@.tmp.ls
	awk 'BEGIN { print "/*" }; { print "**", $$0 }; END { print "*/\n"}' \
		$(OSSCOPYRIGHT) >> $@
	cat $(PROTOCOLGEN) $(GENUTIL) $(LIBSPEC_CUTILS) > $@.tmp.ls
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(C_TYPEMAP)" \
		-DFUNCS="transdraw" \
		-T $(TYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GL_SPEC)" >> $@
	rm -f $@.tmp.ls

g_single.c: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) \
	    $(PROTOCOLGEN) $(GENUTIL) $(LIBSPEC_CUTILS)
	rm -f $@.tmp.ls $@
	awk 'BEGIN { print "/*" }; { print "**", $$0 }; END { print "*/\n"}' \
		$(OSSCOPYRIGHT) >> $@
	cat $(PROTOCOLGEN) $(GENUTIL) $(LIBSPEC_CUTILS) > $@.tmp.ls
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(C_TYPEMAP)" \
		-DFUNCS="transsingle" \
		-T $(TYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GL_SPEC)" >> $@
	rm -f $@.tmp.ls

g_vendpriv.c: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) \
	    $(PROTOCOLGEN) $(GENUTIL) $(LIBSPEC_CUTILS)
	rm -f $@.tmp.ls $@
	awk 'BEGIN { print "/*" }; { print "**", $$0 }; END { print "*/\n"}' \
		$(OSSCOPYRIGHT) >> $@
	cat $(PROTOCOLGEN) $(GENUTIL) $(LIBSPEC_CUTILS) > $@.tmp.ls
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(C_TYPEMAP)" \
		-DFUNCS="transvendpriv" \
		-T $(TYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GL_SPEC)" >> $@
	rm -f $@.tmp.ls

noship_compsize.c: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) \
		   compsize.ls $(GENUTIL) $(LIBSPEC_CUTILS) $(OSSCOPYRIGHT)
	rm -f $@.tmp.ls
	cat compsize.ls $(GENUTIL) $(LIBSPEC_CUTILS) > $@.tmp.ls
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(C_TYPEMAP)" \
		-T $(TYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GL_SPEC)" > $@.tmp.tmp.ls
	rm -f $@
	awk 'BEGIN { print "/*" }; { print "**", $$0 }; END { print "*/\n"}' \
		$(OSSCOPYRIGHT) >> $@
	echo "#include <GL/gl.h>" >> $@
	find $(TOPDIR)/gfx/lib/opengl/src -name '*.c' -print | \
		xargs -t -l awk -f $@.tmp.tmp.ls >> $@
	rm $@.tmp.ls $@.tmp.tmp.ls

include $(COMMONRULES)

$(COMMONTARGS): % : $(COMMONPREF)%
