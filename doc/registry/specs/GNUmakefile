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
# Additional Notice Provisions: This software was created using the
# OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
# not been independently verified as being compliant with the OpenGL(R)
# version 1.2.1 Specification.
#
# $Date: 2000/05/30 07:11:57 $ $Revision: 1.5 $
# $Header: /home/pub/cvs/projects/ogl-sample/main/doc/registry/specs/GNUmakefile,v 1.5 2000/05/30 07:11:57 ljp Exp $

COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs

# Where to place oss.sgi.com web pages
WEBDIR		= $(ROOT)/web

TARGETS		= glext.h glxext.h wglext.h \
		  glext_proto.h glxext_proto.h wglext_proto.h
LDIRT		= *enums*.tmp *funcs*.tmp *ext*.h

default headers: $(TARGETS)

headers_install install: default
	$(INSTALL) -F $(INCLUDE_LOCATION)/GL $(TARGETS)
	$(INSTALL) -F $(WEBDIR)/ogl-sample/ABI $(TARGETS)

libs libs_install apps:

distsi: default
	$(MAKE) $(COMMONPREF)$@

distoss:
	$(MAKE) $(COMMONPREF)$@

include $(ROOT)/usr/include/libspec/libspecdefs

# Override libspecdefs until tools directory entirely converted to Perl
LIBSPEC = $(PERL) -I$(TOPDIR)/tools/libspec

# Suffix for intermediate files (overridden for _proto targets)
PREFIX =
TMPPREFIX = $(PREFIX).tmp

# Generator scripts
GLEXTGEN = glext.pl
TEMPLATEGEN = glexttemplate.pl
# Should collapse together with $(TOPDIR)/gfx/include/gl/template/enum.pl
ENUMGEN  = glextenum.pl

# Flags controlling generation of extension headers
# -protect puts #ifdef/#endif around enums and typedefs (default)
# -prototypes generates C prototypes
# -typedefs generates C function typedefs
PROTECT     = -protect
PROTOTYPES  = -prototypes
TYPEDEFS    = -typedefs
EXTFLAGS    = $(PROTECT) $(PROTOTYPES) $(TYPEDEFS)

# Typemaps for GL, GLX, and WGL (could be collapsed)
# Was $(TYPEMAPSDIR)/$(C_TYPEMAP), but that introduces nasty dependencies
GLTYPEMAP = gl.tm
GLXTYPEMAP = glx.tm
WGLTYPEMAP = wgl.tm

# GL extension enums/prototypes

glenums$(TMPPREFIX): $(ENUMGEN) enumext.spec
	$(PERL) $(ENUMGEN) $(PROTECT) -enumprefix GL_ -maskprefix GL_ \
	    -defineprefix GL_ enumext.spec > $@

glfuncs$(TMPPREFIX): gl.spec $(GLTYPEMAP) $(TEMPLATEGEN)
	$(LIBSPEC) $(TEMPLATEGEN) $(EXTFLAGS) \
	    -typemap $(GLTYPEMAP) -spec gl.spec -callspec APIENTRY \
	    -formals -procprefix gl -ptrprefix PFNGL \
	    -declspec extern -catprefix GL_ > $@

glext$(PREFIX).h: glenums$(TMPPREFIX) glfuncs$(TMPPREFIX) $(GLEXTGEN)
	$(PERL) $(GLEXTGEN) -copyright $(OSSCOPYRIGHT) \
	    -protect __glext_h_ \
	    -file1 glenums$(TMPPREFIX) -file2 glfuncs$(TMPPREFIX) > $@
	$(RM) glenums$(TMPPREFIX) glfuncs$(TMPPREFIX)

# GLX extension enums/prototypes

glxenums$(TMPPREFIX): $(ENUMGEN) glxenumext.spec
	$(PERL) $(ENUMGEN) $(PROTECT) -enumprefix GLX_ -maskprefix GLX_ \
	    -defineprefix GLX_ glxenumext.spec > $@

glxfuncs$(TMPPREFIX): glxext.spec $(GLXTYPEMAP) $(TEMPLATEGEN)
	$(LIBSPEC) $(TEMPLATEGEN) $(EXTFLAGS) \
	    -typemap $(GLXTYPEMAP) -spec glxext.spec \
	    -formals -procprefix glX -ptrprefix PFNGLX \
	    -declspec extern -catprefix GLX_ > $@

glxext$(PREFIX).h: glxenums$(TMPPREFIX) glxfuncs$(TMPPREFIX) $(GLEXTGEN)
	$(PERL) $(GLEXTGEN) -copyright $(OSSCOPYRIGHT) \
	    -protect __glxext_h_ \
	    -file1 glxenums$(TMPPREFIX) -file2 glxfuncs$(TMPPREFIX) > $@
	$(RM) glxenums$(TMPPREFIX) glxfuncs$(TMPPREFIX)

# WGL extension enums/prototypes
# No enumprefix/maskprefix/defineprefix is specified, since
#   the WGL prefix is imbedded in the specfiles. A passthru
#   mechanism could be added to the enum specfiles to allow
#   the exceptional cases (ERROR_ codes not prefixed by WGL).
wglenums$(TMPPREFIX): $(ENUMGEN) wglenumext.spec
	$(PERL) $(ENUMGEN) $(PROTECT) wglenumext.spec > $@

wglfuncs$(TMPPREFIX): wglext.spec $(WGLTYPEMAP) $(TEMPLATEGEN)
	$(LIBSPEC) $(TEMPLATEGEN) $(EXTFLAGS) \
	    -typemap $(WGLTYPEMAP) -spec wglext.spec -callspec WINAPI \
	    -formals -procprefix wgl -ptrprefix PFNWGL \
	    -declspec extern -catprefix WGL_ > $@

wglext$(PREFIX).h: wglenums$(TMPPREFIX) wglfuncs$(TMPPREFIX) $(GLEXTGEN)
	$(PERL) $(GLEXTGEN) -copyright $(OSSCOPYRIGHT) \
	    -protect __wglext_h_ \
	    -file1 wglenums$(TMPPREFIX) -file2 wglfuncs$(TMPPREFIX) > $@
	$(RM) wglenums$(TMPPREFIX) wglfuncs$(TMPPREFIX)

#

# For debugging headers - regenerate with no protection
# TARGPREFIX lets us use the parameterized rules above recursively:
#   on the initial run, TARGPREFIX=_proto and PREFIX="". This causes
#   recursive invocation where TARGPREFIX=ignore and PREFIX=_proto,
#   so the rules above are used to actually generate the _proto
#   files.
TARGPREFIX = _proto
glext$(TARGPREFIX).h glxext$(TARGPREFIX).h wglext$(TARGPREFIX).h: always
	$(MAKE) PROTECT= PREFIX=_proto TARGPREFIX=ignore $@

always:

$(COMMONTARGS): % : $(COMMONPREF)%

include $(COMMONRULES)
