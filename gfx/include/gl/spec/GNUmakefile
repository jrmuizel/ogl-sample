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
# $Header: //depot/main/gfx/include/gl/spec/GNUmakefile#12 $

COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs

TARGETS = gl.api

SUBDIRS = typemaps

default headers headers_install install distsi:
	cd $(TYPEMAPSDIR); $(MAKE)
	$(MAKE) $(TARGETS)

# Can't use SUBDIRS_MAKERULE for now
distoss:
	$(MAKE) $(COMMONPREF)$@
	@for d in $(SUBDIRS) bin ; do \
	    (cd $$d; ${MAKE} $${RULE:=$@}); \
	done

libs libs_install apps:

BINDIR = ./bin

GL_SPEC_CAT = gl.spec.cat
GL_SPEC_RTN = gl.spec.rtn

include $(ROOT)/usr/include/libspec/libspecdefs

TYPEMAP_NOP = nop.tm	# lets all types in spec pass unchanged, no mapping
C_TYPEMAP = capi.tm

# CHECKFLAG = -d

APIGEN = api.ls
SPECSORT_CAT = $(BINDIR)/specsort_cat
SPECSORT_RTN = $(BINDIR)/specsort_rtn

gl.spec.cat: gl.spec
	$(SPECSORT_CAT) gl.spec $@

gl.spec.rtn: gl.spec
	$(SPECSORT_RTN) gl.spec $@

gl.api: $(GL_SPEC_CAT) $(TYPEMAPSDIR)/$(C_TYPEMAP) $(APIGEN) $(LIBSPEC_CUTILS)
	rm -f $@.tmp.ls
	cat $(APIGEN) $(LIBSPEC_CUTILS) > $@.tmp.ls
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(C_TYPEMAP)" -DOUTPUT="$@" -T \
		$(TYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(GL_SPEC_CAT)" > $@
	rm $@.tmp.ls
	awk 'BEGIN { printf "%-18s %-16s %-8s %s\n\n\n", "category", \
		"routine", "return", "params"; \
		bar = "------------------------------------------------"; \
		printf "%s%s%s\n", bar,bar,bar; }' $@ > $@.tmp
	awk '{ print; printf "\n";}' $@ >> $@.tmp
	mv $@.tmp $@

gl.api.rtn: $(GL_SPEC_RTN) $(TYPEMAPSDIR)/$(C_TYPEMAP) $(APIGEN) \
		$(LIBSPEC_CUTILS)
	rm -f $@.tmp.ls
	cat $(APIGEN) $(LIBSPEC_CUTILS) > $@.tmp.ls
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(C_TYPEMAP)" -DOUTPUT="$@" -T \
		$(TYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(GL_SPEC_RTN)" > $@
	rm $@.tmp.ls
	awk 'BEGIN { printf "%-18s %-16s %-8s %s\n\n\n", "category", \
		"routine", "return", "params"; \
		bar = "------------------------------------------------"; \
		printf "%s%s%s\n", bar,bar,bar; }' $@ > $@.tmp
	awk '{ print; printf "\n";}' $@ >> $@.tmp
	mv $@.tmp $@

# local dirt, cpp output files and assembler listings
LDIRT = *.[Eis] $(GENFILES) *.tmp.ls $(GL_SPEC_CAT) $(GL_SPEC_RTN)

include $(COMMONRULES)

$(COMMONTARGS): % : $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)
