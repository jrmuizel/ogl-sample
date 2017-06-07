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
# $Header: //depot/main/gfx/lib/opengl/GNUmakefile#23 $

COMMONPREF = standard
include defs/ALL.defs

# OGL root directory.
GFXDIR = $(OGLLIBSRC)

# Define this to the os that this GL is to be compiled for
ifndef GLOS
GLOS = NONE
endif

MAKE := $(MAKE) --no-print-directory

# This is a list of the directories used to build the GL
DIRS = drivers/$(GLPRODUCT) glcore generated

# This is a list of the Makefiles in each of the directories
GLMAKEFILES =				 \
	glcore/GNUmakefile		 \
	generated/GNUmakefile		 \
	drivers/$(GLPRODUCT)/GNUmakefile \
	$(NULL)

ifeq ($(GLOS),"NONE")
DIRS += $(GLOS)
GLMAKEFILES += $(GLOS)/GNUmakefile
endif

# List of all product types supported by this GL tree
ALLPRODUCTS = XSERVER

######################################################################

.PRECIOUS: GNUmakefile

default:
	@if test '$(GLPRODUCT)'X = 'X'; then		 \
	    $(ECHO) 'Must set GLPRODUCT before using make'; \
	    exit 255;					 \
	else						 \
	if test '$(GLPRODUCT)'X = 'ALLX'; then		 \
	    for i in $(ALLPRODUCTS); do			 \
		$(MAKE) GLPRODUCT=$i targets;		 \
	    done;					 \
	else						 \
	    $(MAKE) targets;				 \
	fi;						 \
	fi;

targets: drivers/$(GLPRODUCT).O/GNUmakefile
	cd drivers/$(GLPRODUCT).O; $(MAKE)

clean:
	@if test '$(GLPRODUCT)'X = 'X'; then		 \
	    $(ECHO) 'Must set GLPRODUCT before using make'; \
	    exit 255;					 \
	else						 \
	if test '$(GLPRODUCT)'X = 'ALLX'; then		 \
	    for i in $(ALLPRODUCTS); do			 \
		$(MAKE) GLPRODUCT=$i clean;		 \
	    done;					 \
	else						 \
	    ( cd $(GLPRODUCT) && $(MAKE) clean ) ;	 \
	    ( cd drivers/$(GLPRODUCT).O && $(MAKE) clean ) ; \
	fi;						 \
	fi;

clobber:
	@if test '$(GLPRODUCT)'X = 'X'; then		 \
	    $(ECHO) 'Must set GLPRODUCT before using make'; \
	    exit 255;					 \
	else						 \
	if test '$(GLPRODUCT)'X = 'ALLX'; then		 \
	    for i in $(ALLPRODUCTS); do			 \
		$(ECHO) $(RM) -r drivers/$$i'.O';	 \
		$(RM) -r drivers/$$i'.O';		 \
	    done;					 \
	else						 \
	    $(ECHO) $(RM) -r drivers/$(GLPRODUCT).O;	 \
	    $(RM) -r drivers/$(GLPRODUCT).O;		 \
	fi;						 \
	fi;

######################################################################

mkfile: $(_FORCE)
	@if test -f '$(GLPRODUCT)'X = 'X'; then			      \
	    $(ECHO) 'Must set GLPRODUCT before using make';	      \
	    exit 255;						      \
	else							      \
	    $(MAKE) generated_makefile				      \
			GENMK=drivers$(GLPRODUCT).O/GNUmakefile;      \
	    cd drivers/$(GLPRODUCT).O; rm -f Makedepend; $(MAKE) depend; \
	fi

drivers/$(GLPRODUCT).O/GNUmakefile: GNUmakefile $(GLMAKEFILES)
	$(MAKE) generated_makefile GENMK=$@
	cd drivers/$(GLPRODUCT).O; rm -f Makedepend; $(MAKE) depend

######################################################################

generated_makefile:
	@if test ! -d $(GENMK:.O/GNUmakefile=.O); then \
	    rm -f $(GENMK:.O/GNUmakefile=.O);	    \
	    mkdir $(GENMK:.O/GNUmakefile=.O);	    \
	fi
	rm -f $(GENMK)
	$(ECHO) '#! gmake' >> $(GENMK)
	$(ECHO) '# THIS MAKEFILE IS AUTOMATICALLY GENERATED - DO NOT EDIT\n' \
		>> $(GENMK)
	#
	$(ECHO) 'include $(GFXDIR)/defs/ALL.defs\n' >> $(GENMK)
	#
	$(ECHO) 'GENERATED_CFILES = \\' >> $(GENMK)
	for i in $(DIRS); do					      \
	    files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
		 list_generated_cfiles`; \
	    if test "$$files"X != 'NONE'X; then			      \
		for j in $$files; do				      \
		    $(ECHO) "\t$$j \\" >> $(GENMK);                   \
		done;						      \
	    fi;							      \
	done
	$(ECHO) '\t$$(NULL)\n' >> $(GENMK)
	#
	$(ECHO) 'GENERATED_SFILES = \\' >> $(GENMK)
	for i in $(DIRS); do					      \
	    files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
		list_generated_sfiles`; \
	    if test "$$files"X != 'NONE'X; then			      \
		for j in $$files; do				      \
		    $(ECHO) "\t$$j \\" >> $(GENMK);                   \
		done;						      \
	    fi;							      \
	done
	$(ECHO) '\t$$(NULL)\n' >> $(GENMK)
	#
	$(ECHO) 'GENERATED_HFILES = \\' >> $(GENMK)
	for i in $(DIRS); do					      \
	    files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
		list_generated_hfiles`; \
	    if test "$$files"X != 'NONE'X; then			      \
		for j in $$files; do				      \
		    $(ECHO) "\t$$j \\" >> $(GENMK);                   \
		done;						      \
	    fi;							      \
	done
	$(ECHO) '\t$$(NULL)\n' >> $(GENMK)
	#
	$(ECHO) 'CFILES = \\' >> $(GENMK)
	for i in $(DIRS); do				    \
	    files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
		list_cfiles`; \
	    if test "$$files"X != 'NONE'X; then		    \
		for j in $$files; do			    \
		    $(ECHO) "\t../../$$i/$$j \\" >> $(GENMK);       \
		done;					    \
	    fi;						    \
	done
	$(ECHO) '\t$$(GENERATED_CFILES)\n' >> $(GENMK)
	#
	$(ECHO) 'SFILES = \\' >> $(GENMK)
	for i in $(DIRS); do				    \
	    files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
		list_sfiles`; \
	    if test "$$files"X != 'NONE'X; then		    \
		for j in $$files; do			    \
		    $(ECHO) "\t../../$$i/$$j \\" >> $(GENMK);       \
		done;					    \
	    fi;						    \
	done
	$(ECHO) '\t$$(GENERATED_SFILES)\n' >> $(GENMK)
	#
	$(ECHO) 'TARGETS = $$(GLLIBNAME)\n' >> $(GENMK)
	#
	$(ECHO) 'default all:' >> $(GENMK)
	$(ECHO) '\t$$(MAKE) GLPRODUCT=$(GLPRODUCT) incdepend' >> $(GENMK)
	$(ECHO) '\t$$(MAKE) GLPRODUCT=$(GLPRODUCT) targets' >> $(GENMK)
	$(ECHO) >> $(GENMK)
	#
	$(ECHO) 'targets: $$(TARGETS)\n' >> $(GENMK)
	#
	$(ECHO) 'generated_srcs: $$(GENERATED_CFILES) $$(GENERATED_SFILES) $$(GENERATED_HFILES)\n' >> $(GENMK)
	#
	for i in $(DIRS); do				       \
	    if test -f $$i/Generate.mk; then		       \
		$(ECHO) "include ../../$$i/Generate.mk" >> $(GENMK); \
	    fi;						       \
	done
	$(ECHO) >> $(GENMK)
	#
	$(ECHO) 'OBJECTS = \\' >> $(GENMK)
	for i in $(DIRS); do				   \
	    for j in list_cfiles list_generated_cfiles; do \
		files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
			 $$j`;\
		if test "$$files"X != 'NONE'X; then	   \
		    for k in $$files; do		   \
			f=`basename $$k .c`;		   \
			$(ECHO) "\t$$f".o" \\" >> $(GENMK);   \
		    done;				   \
		fi;					   \
	    done;					   \
	    for j in list_sfiles list_generated_sfiles; do \
		files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
			$$j`; \
		if test "$$files"X != 'NONE'X; then	   \
		    for k in $$files; do		   \
			f=`basename $$k .s`;		   \
			$(ECHO) "\t$$f".o" \\" >> $(GENMK);   \
		    done;				   \
		fi;					   \
	    done;					   \
	done
	$(ECHO) '\t$$(NULL)\n' >> $(GENMK)
	#
	$(ECHO) '# Generate cross directory dependencies.' >> $(GENMK)
	for i in $(DIRS); do				    \
	    files=`$(MAKE) -s -C $$i -f GNUmakefile GFXDIR=$(GFXDIR)\
		list_cfiles`; \
	    if test "$$files"X != 'NONE'X; then		    \
		for j in $$files; do			    \
		    f=`basename $$j .c`;		   \
		    $(ECHO) "$$f.o: ../../$$i/$$j" >> $(GENMK);     \
		done;					    \
	    fi;						    \
	done
	$(ECHO) >> $(GENMK)
	#
	$(ECHO) 'LDIRT = $$(GENERATED_CFILES) $$(GENERATED_HFILES)\n' >> $(GENMK)
	#
	$(ECHO) 'incdepend: $$(GENERATED_CFILES) $$(GENERATED_HFILES)\n' \
		>> $(GENMK)
	$(ECHO) 'depend: $$(GENERATED_CFILES) $$(GENERATED_HFILES)\n' >> $(GENMK)
	#
	$(ECHO) '$$(GLLIBNAME): $$(OBJECTS)' >> $(GENMK)
	$(ECHO) '\trm -f $$@' >> $(GENMK)
	$(ECHO) '\t$$(AR) crlu $$@ $$(OBJECTS)' >> $(GENMK)
	$(ECHO) >> $(GENMK)
	$(ECHO) 'INSTALLFLAGS += -nopath' >> $(GENMK)
	$(ECHO) '-include ../../$$(DISTFILE_EXT)' >> $(GENMK)
	$(ECHO) >> $(GENMK)
	$(ECHO) 'include $$(COMMONRULES)' >>$(GENMK)

headers headers_install apps:

libs: default

libs_install install: default
	cd drivers/$(GLPRODUCT).O ; $(INSTALL) -F $(ROOT)/usr/lib $(GLLIBNAME)

distsi: drivers/$(GLPRODUCT).O/GNUmakefile
	cd drivers/$(GLPRODUCT).O; $(MAKE) $@

SUBDIRS = \
    defs \
    drivers \
    generated \
    glcore \
    include \
    lib

distoss:
	$(MAKE) $(COMMONPREF)$@
	@for d in $(SUBDIRS); do \
	    (cd $$d ; $(MAKE) -f GNUmakefile GFXDIR=$(GFXDIR) $@) ; \
	done

include $(COMMONRULES)
