#!gmake
#
# $Date$ $Revision$
# $Header$

COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs

SUBDIRS = \
    DDX

default headers headers_install libs libs_install install apps:

distoss:
	$(MAKE) $(COMMONPREF)$@
	$(SUBDIRS_MAKERULE)

include $(COMMONRULES)
