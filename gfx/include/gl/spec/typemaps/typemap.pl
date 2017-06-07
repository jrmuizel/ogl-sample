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
# $Header: //depot/main/gfx/include/gl/spec/typemaps/typemap.pl#3 $
#

#
# Here's the idea:
#  The spec file contains the "spec" types.
#
#  We first map those "spec" types into "wire" types, which are the types
#  that are used to transfer the OpenGL protocol down the wire (ie. the
#  client-server connection).
#
#  Then those "wire" types are mapped into the appropriate types, depending
#  on the language (C, FORTRAN, etc.)
#
#  'typelist' contains the types that we want to map (we extract those types
#   from the spec files themselves.
#
#  'spec2wire.map' contains the map between spec and wire types.
#
#  'wire2c.map' contains the map between wire and C language types.
#

$spec2wire = $ARGV[0];
$wire2c    = $ARGV[1];
$typelist  = $ARGV[2];


#input generated type equivalences
open(SPEC2WIRE, $spec2wire) || die("Cannot open spec2wire file\n");
while(<SPEC2WIRE>) { eval; }
close SPEC2WIRE;
open(WIRE2C, $wire2c) || die("Cannot open wire2c file\n");
while(<WIRE2C>) { eval; }
close WIRE2C;


$direction[0] = "in";
$direction[1] = "out";
$ndir = 2;
$transfer[0] = "value";
$transfer[1] = "array";
$transfer[2] = "reference";
$ntrans = 3;

open(TYPELIST, $typelist) || die("Cannot open typelist file\n");
while(<TYPELIST>) {
	chop;

	$spectype = $_;

	# map from a spec type to a wire type.
	if (defined $WireType{$spectype}) {
		$wiretype = $WireType{$spectype};
	} else {
		$wiretype = "UNKNOWN in WireType";
	}

	# map from a wire type to an API type
	if (defined $ApiType{$wiretype}) {
		$apitype = $ApiType{$wiretype};
	} else {
		$apitype = "UNKNOWN in ApiType";
	}

	if ($spectype eq "void") {
		$apitype = "*";
	}

	# What a lousy hack just to get Matrix to work
	if (($spectype eq "MatrixF") || ($spectype eq "MatrixD")) {
		$mappedtransfer = "array";
	} else {
		$mappedtransfer = "*";
	}

	for ($d = 0; $d < $ndir; $d++) {
		for ($t = 0; $t < $ntrans; $t++) {
			printf "%s,%s,%s, \t %s,*,%s\n",
				$spectype, $direction[$d], $transfer[$t],
				$apitype, $mappedtransfer;
		}
	}
}
