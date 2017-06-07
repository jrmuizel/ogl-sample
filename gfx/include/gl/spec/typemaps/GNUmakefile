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
# $Header: //depot/main/gfx/include/gl/spec/typemaps/GNUmakefile#14 $

COMMONPREF = standard
include $(ROOT)/usr/include/make/commondefs
include $(ROOT)/usr/include/libspec/libspecdefs

TARGETS = capi.tm fapi.tm nop.tm gl.wire

default headers headers_install install: $(TARGETS)

libs libs_install apps distsi:

distoss:
	$(MAKE) $(COMMONPREF)$@

DIRT = typelist spec2wire.perlmap wire2c.perlmap wire2f.perlmap *.tmp

# A typemap for the C api
capi.tm: spec2wire.perlmap wire2c.perlmap typemap.pl typelist
	$(RM) $@
	$(PERL) typemap.pl spec2wire.perlmap wire2c.perlmap typelist > $@

# A typemap for the Fortran API.
fapi.tm: spec2wire.perlmap wire2f.perlmap typemap.pl typelist
	$(RM) $@
	$(PERL) typemap.pl spec2wire.perlmap wire2f.perlmap typelist > $@

# A typemap that just passes the spec types through, unchanged.
nop.tm: ../gl.spec $(SPECTYPESGEN) nop.pl
	$(PERL) $(SPECTYPESGEN) ../gl.spec > $@.tmp
	$(PERL) nop.pl $@.tmp > $@
	$(RM) $@.tmp

# Map spec types to wire types
spec2wire.perlmap: spec2wire.map mapper.pl
	$(RM) $@
	$(PERL) mapper.pl WireType spec2wire.map > $@

# Map wire types to C API types, a one-to-one mapping.
wire2c.perlmap: wire2c.map mapper.pl
	$(RM) $@
	$(PERL) mapper.pl ApiType wire2c.map > $@

# Map wire types to Fortran API types, a one-to-one mapping.
wire2f.perlmap: wire2f.map mapper.pl
	$(RM) $@
	$(PERL) mapper.pl ApiType wire2f.map > $@

# Wire map file used by libspec.
gl.wire: spec2wire.map
	ln -s spec2wire.map gl.wire

typelist: ../gl.spec ../glu.spec
	$(PERL) $(SPECTYPESGEN) ../gl.spec > $@.tmp
	$(PERL) $(SPECTYPESGEN) ../glu.spec >> $@.tmp
	sort < $@.tmp | uniq > $@
	$(RM) $@.tmp

# List types that are named in gl.spec but missing from spec2wire.map.
missing: spec2wire.map typelist
	awk '{ if (NF != 0) printf "%s\n", $$1 }' spec2wire.map | sort > $@.tmp
	comm -23 typelist $@.tmp
	rm -f $@.tmp

# List types in spec2wire.map that are not used in gl.spec.
extra: spec2wire.map typelist
	awk '{ if (NF != 0) printf "%s\n", $$1 }' spec2wire.map > $@.tmp
	comm -13 typelist $@.tmp
	rm -f $@.tmp

$(COMMONTARGS): % : $(COMMONPREF)%

include $(COMMONRULES)
