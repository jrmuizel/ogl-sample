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
# $Header: //depot/main/gfx/include/gl/template/GNUmakefile#22 $

COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs

TARGETS		= glconsts.h gltemplate.h gl.h \
		  fglconsts.h fgltemplate.h fgl.h \
		  gl.h_noformals gltemplate.h_noformals \
		  gluconsts.h glutemplate.h glu.h
PUBLIC		= gl.h gl.h_noformals fgl.h glu.h

default headers: $(TARGETS)

headers_install install: default
	$(INSTALL) -F $(INCLUDE_LOCATION)/GL $(PUBLIC)

libs libs_install apps:

distsi: default
	$(MAKE) $(COMMONPREF)$@

distoss:
	$(MAKE) $(COMMONPREF)$@

include $(ROOT)/usr/include/libspec/libspecdefs

ENUMGEN		= enum.pl
TEMPLATEGEN	= template.pl
FUNCPTRGEN	= funcptr.pl
GLGEN		= gl.pl
FGLGEN		= fgl.pl
GLUGEN		= glu.pl
FTN_TEMPLATEGEN = ftemplate.pl

# Temporary override, until tools directory entirely converted to Perl
LIBSPEC = $(PERL) -I$(TOPDIR)/tools/libspec

# typedefs and enumerations
glconsts.h: $(ENUMGEN) $(SPECDIR)/$(ENUM_SPEC)
	$(RM) $@
	$(PERL) -s $(ENUMGEN) -ENUMTYPEPREFIX=GL -ENUMPREFIX=GL_ \
		-MASKTYPEPREFIX=GL -MASKPREFIX=GL_ \
		-DEFINEPREFIX=GL_ -DEFINETYPEPREFIX=GL \
		$(SPECDIR)/$(ENUM_SPEC) >> $@

$(TYPEMAPSDIR)/$(C_TYPEMAP): $(SPECDIR)/$(GL_SPEC)
	cd $(TYPEMAPSDIR); make $(C_TYPEMAP)

# function prototypes
gltemplate.h_noformals: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) \
			$(TEMPLATEGEN)
	$(RM) $@
	$(LIBSPEC) $(TEMPLATEGEN) \
		-typemap $(TYPEMAPSDIR)/$(C_TYPEMAP) \
		-spec $(SPECDIR)/$(GL_SPEC) \
		-formals no -procprefix gl \
		-declspec extern \
		-extensions yes > $@

# function prototypes with formals
gltemplate.h: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) \
		$(TEMPLATEGEN)
	$(RM) $@
	$(LIBSPEC) $(TEMPLATEGEN) \
		-typemap $(TYPEMAPSDIR)/$(C_TYPEMAP) \
		-spec $(SPECDIR)/$(GL_SPEC) \
		-formals yes -procprefix gl \
		-declspec extern \
		-extensions yes > $@

### Currently unused
#glfnptr.h: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) \
#		 $(FUNCPTRGEN)
#	 $(RM) $@
#	 $(LIBSPEC) $(FUNCPTRGEN) \
#		 -typemap $(TYPEMAPSDIR)/$(C_TYPEMAP) \
#		 -spec $(SPECDIR)/$(GL_SPEC) \
#		 -output $@ -formals -procprefix PFNGL	> $@

# public C language GL header
gl.h: glconsts.h gltemplate.h $(GLGEN)
	$(RM) $@
	$(PERL) -s $(GLGEN) -COPYRIGHT=$(OSSCOPYRIGHT) \
		-FILE1=glconsts.h -FILE2=gltemplate.h \
		-FILE3=/dev/null >> $@

# public C language GL header without formal declarations
gl.h_noformals: glconsts.h gltemplate.h_noformals $(GLGEN)
	$(RM) $@
	$(PERL) -s $(GLGEN) -COPYRIGHT=$(OSSCOPYRIGHT) \
		-FILE1=glconsts.h -FILE2=gltemplate.h_noformals \
		-FILE3=/dev/null >> $@

######################################################################

### FORTRAN headers need some work, as well as converting the fglconsts.h
### actions to a cleaner Perl script and using a FGLGEN script to
### generate fgl.h

$(TYPEMAPSDIR)/$(FTN_TYPEMAP): $(SPECDIR)/$(GL_SPEC)
	cd $(TYPEMAPSDIR); make $(FTN_TYPEMAP)

# fortran typedefs and enumerations
fglconsts.h: glconsts.h
	$(RM) $@
	awk -f c2fh.ls glconsts.h | \
	sed -e 's|^/\*\(.*\)\*/|C \1|'	-e 's|\(..*\)/\*.*\*/|\1|' \
	    -e 's|^[ /]\*/*|C |' -e 's|^\(C.*\)\*/[	    ]*|\1|' \
	    -e 's/\([^=]\)==\([^=]\)/\1.EQ.\2/g'  -e 's/!=/.NE./g' \
	    -e 's/<=/.LE./g'  -e 's/>=/.GE./g' \
	    -e '/^C.*<</d'    -e '/^C.*>>/d' \
	    -e 's/</.LT./g'   -e 's/\([^-]\)>/\1.GT./g' \
	    -e 's/\([^"]\)!\([^"]\)/\1.NOT.\2/g'  -e 's/&&/.AND./g' -e 's/||/.OR./g' \
	    -e 's/0x\([A-Fa-f0-9]*\)/hex \1/g' >> $@

# Fortran function declarations
fgltemplate.h: $(SPECDIR)/$(GL_SPEC) $(TYPEMAPSDIR)/$(FTN_TYPEMAP) \
	       $(FTN_TEMPLATEGEN) $(LIBSPEC_CUTILS)
	$(RM) $@
	$(LIBSPEC) $(FTN_TEMPLATEGEN) \
		-typemap $(TYPEMAPSDIR)/$(FTN_TYPEMAP) \
		-spec $(SPECDIR)/$(GL_SPEC) \
		-formals no -procprefix fgl \
		-extensions yes > $@

# public Fortran-GL header
fgl.h:	fglconsts.h fgltemplate.h
	$(RM) $@
	$(PERL) -s $(FGLGEN) -COPYRIGHT=$(OSSCOPYRIGHT) \
		-CONSTS=fglconsts.h -TEMPLATE=fgltemplate.h > $@

######################################################################

# glext.h has definitions for extensions
GLEXTGEN = glext.pl
glext.h: glext.spec $(TYPEMAPSDIR)/$(C_TYPEMAP) \
	      $(GLEXTGEN)
	$(RM) $@
	$(LIBSPEC) $(GLEXTGEN) \
	      -typemap $(TYPEMAPSDIR)/$(C_TYPEMAP) \
	      -spec glext.spec \
	      -output $@ -formals -procprefix gl -ptrprefix PFNGL > $@

######################################################################

### GLU headers need some work too (currently include some extraneous gl.h
### material), and a GLUGEN script to generate glu.h

# typedefs and enumerations for glu.h
gluconsts.h: $(ENUMGEN) $(SPECDIR)/$(ENUMGLU_SPEC)
	$(RM) $@
	$(PERL) -s $(ENUMGEN) -ENUMTYPEPREFIX=GLU -ENUMPREFIX=GLU_ \
		-MASKTYPEPREFIX=GLU -MASKPREFIX=GLU_ \
		-DEFINEPREFIX=GLU_ -DEFINETYPEPREFIX=GLU \
		$(SPECDIR)/$(ENUMGLU_SPEC) >> $@

# function prototypes with formals for glu.h
glutemplate.h: $(SPECDIR)/$(GLU_SPEC) $(TYPEMAPSDIR)/$(C_TYPEMAP) \
	       $(TEMPLATEGEN)
	$(RM) $@
	$(LIBSPEC) $(TEMPLATEGEN) \
		    $(CHECKFLAG) \
		    -typemap $(TYPEMAPSDIR)/$(C_TYPEMAP) \
		    -spec $(SPECDIR)/$(GLU_SPEC) \
		    -formals yes -procprefix glu \
		    -declspec extern \
		    -extensions yes >> $@

#		    sed -e 's/GLstring/const GLubyte */' |
#		    sed -e 's/CallBackFunc/(*CallBackFunc)()/' > $@

# public C Language GLU header
glu.h: gluconsts.h glutemplate.h GNUmakefile
	$(RM) $@
	$(PERL) -s $(GLUGEN) -COPYRIGHT=$(OSSCOPYRIGHT) \
		-CONSTS=gluconsts.h -TEMPLATE=glutemplate.h > $@


$(COMMONTARGS): % : $(COMMONPREF)%

include $(COMMONRULES)
