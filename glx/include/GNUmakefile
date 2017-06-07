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
# $Header: //depot/main/glx/include/GNUmakefile#12 $

COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs
include $(ROOT)/usr/include/libspec/libspecdefs

NOPGEN = $(TYPEMAPSDIR)/nop.pl
TYPEMAP = nop.tm
LTYPEMAPSDIR = .
OPCODEGEN = opcode.ls

LDIRT = opcode_tmp.h nop.tm
PUBLIC = glxproto.h glxmd.h glxint.h
GEN_HFILES = glxproto.h
TARGETS = $(GEN_HFILES)

default headers: $(GEN_HFILES)

libs libs_install apps:

headers_install install: default
	$(INSTALL) -F $(ROOT)/usr/include/GL/internal $(PUBLIC)

distsi: default
	$(MAKE) $(COMMONPREF)$@

distoss:
	$(MAKE) $(COMMONPREF)$@

opcode_tmp.h: $(SPECDIR)/$(GLX_SPEC) $(SPECDIR)/$(GL_SPEC) \
		$(LTYPEMAPSDIR)/$(TYPEMAP) $(OPCODEGEN) $(LIBSPEC_CUTILS)
	rm -f $@.tmp.ls $@
	cat $(OPCODEGEN) $(LIBSPEC_CUTILS) > $@.tmp.ls
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(TYPEMAP)" -DSECTION="glx" \
		-DPREFIX="X_GLX" \
		-T $(LTYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GLX_SPEC)" >> $@
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(TYPEMAP)" -DSECTION="single" \
		-DPREFIX="X_GLsop_" \
		-T $(LTYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GL_SPEC)" >> $@
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(TYPEMAP)" -DSECTION="render" \
		-DPREFIX="X_GLrop_" \
		-T $(LTYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GL_SPEC)" >> $@
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(TYPEMAP)" -DSECTION="vendpriv" \
		-DPREFIX="X_GLvop_" \
		-T $(LTYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GL_SPEC)" >> $@
	$(LIBSPEC) $(CHECKFLAG) -DTYPEMAP="$(TYPEMAP)" -DSECTION="vendorglx" \
		-DPREFIX="X_GLXvop_" \
		-T $(LTYPEMAPSDIR) -P $(LIBSPEC_PARSER) $@.tmp.ls \
		"$(SPECDIR)/$(GLX_SPEC)" >> $@
	rm $@.tmp.ls

glxproto.h: opcode_tmp.h protostruct.h $(COPYRIGHT)
	rm -f $@
	echo "#ifndef _GLX_glxproto_h_" >> $@
	echo "#define _GLX_glxproto_h_" >> $@
	echo >> $@
	cat protostruct.h >> $@
	echo >> $@
	cat opcode_tmp.h >> $@
	echo "#endif /* _GLX_glxproto_h_ */" >> $@

nop.tm: $(SPECDIR)/$(GL_SPEC) $(SPECTYPESGEN) $(NOPGEN)
	$(PERL) $(SPECTYPESGEN) $(SPECDIR)/$(GL_SPEC) > $@.tmp
	$(PERL) $(NOPGEN) $@.tmp > $@
	$(RM) $@.tmp

include $(COMMONRULES)

$(COMMONTARGS): % : $(COMMONPREF)%
