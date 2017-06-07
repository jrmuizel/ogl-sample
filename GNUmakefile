#!gmake
#
# $Header: //depot/main/GNUmakefile#14 $

# Allow interesting user-specific rules at top level
-include Makefile.local

ALLTARGS = headers headers_install libs libs_install install apps \
	   clean clobber

SUBDIRS = \
	glx \
	gfx \
	$(NULL)

OS = $(shell uname)
ifeq ($(subst 64,,$(OS)), IRIX)
SUBDIRS += doc
else
SUBDIRS += doc/man doc/release
endif

default $(ALLTARGS): $(_FORCE)
	@for d in $(SUBDIRS); do \
	    (cd $$d; ${MAKE} $${RULE:=$@}); \
	done

distsi: $(_FORCE)
	@for d in $(SUBDIRS) tools; do \
	    (cd $$d; ${MAKE} $${RULE:=$@}); \
	done

-include Distfile.int

distoss: $(_FORCE)
	perl tools/gfxinstall/gfxinstall.pl \
	    $(INSTALLFLAGS_OSS) -F $(DIST)/$(DISTDIR_OSS) $(DISTFILES_OSS)
	@for d in $(SUBDIRS) tools; do \
	    (cd $$d; ${MAKE} $${RULE:=$@}); \
	done

tools_install:
	cd tools; ${MAKE} install

World: tools_install headers_install libs_install default
