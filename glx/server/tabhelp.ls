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
# $Header$

#
#   Generate dispatch tables and prototypes for their routines.
#

function initialize() {

    # TYPEMAP should be defined on the command line that invokes libspec
    typeMapFile = "nop.tm";
    _readWireFile();

    funcs = FUNCS;
    suffix = SUFFIX;

    #
    # This script is executed in multiple passes.  During
    # each pass, it is invoked with only one of the following
    # expressions evaluating to true on any given pass.
    #
    genRender = (funcs ~ /render/);
    genSingle = (funcs ~ /single/);
    genVendPriv = (funcs ~ /vendpriv/);
    genEXT = (suffix ~ /_EXT/);
    max_opcode = 0;
    min_opcode = 65536;

    #
    # Partial names of macros used to send each wire type.
    #
    cast["enum"] =	"INT"
    cast["boolean"] =	"BYTE"
    cast["bitfield"] =	"INT"
    cast["byte"] =	"BYTE"
    cast["short"] =	"SHORT"
    cast["int"] =	"INT"
    cast["sizei"] =	"INT"
    cast["ubyte"] =	"BYTE"
    cast["ushort"] =	"SHORT"
    cast["uint"] =	"INT"
    cast["float"] =	"FLOAT"
    cast["clampf"] =	"FLOAT"
    cast["double"] =	"DOUBLE"
    cast["clampd"] =	"DOUBLE"
    cast["void"] =	"VOID"

    #
    # Byte sizes of wire types.
    #
    xfer_size["enum"] =		"4"
    xfer_size["boolean"] =	"1"
    xfer_size["bitfield"] =	"4"
    xfer_size["byte"] =		"1"
    xfer_size["short"] =	"2"
    xfer_size["int"] =		"4"
    xfer_size["sizei"] =	"4"
    xfer_size["ubyte"] =	"1"
    xfer_size["ushort"] =	"2"
    xfer_size["uint"] =		"4"
    xfer_size["float"] =	"4"
    xfer_size["clampf"] =	"4"
    xfer_size["double"] =	"8"
    xfer_size["clampd"] =	"8"
    xfer_size["void"] =		"1"		# A lie, but good enough

} # function intialize

function main( i, param, cmdlen, invarsize) {

    if ("vectorequiv" in propList) return;

    if (("extension", "not_implemented") in propListValues) return;

    if (("glxflags", "ignore") in propListValues) return;

    if (genSingle) {
	if ("glxsingle" in propList) {
	    opcode = propList["glxsingle"];
	} else if ("glxopcode" in propList) {
	    opcode = propList["glxopcode"];
	} else {
	    return;
	}
    } else if (genVendPriv) {
	if ("glxvendorpriv" in propList) {
	    if (("glxflags", "EXT") in propListValues) {
		if (genEXT)
		    opcode = propList["glxvendorpriv"];
		else
		    return;
	    }
	} else {
	    return;
	}
    } else if (genRender) {
	if ("glxropcode" in propList) {
	    if (("glxflags", "EXT") in propListValues) {
		if (genEXT)
		    opcode = propList["glxropcode"];
		else
		    return;
	    } else {
		if ((!genEXT) )
		    opcode = propList["glxropcode"];
		else
		    return;
	    }
	} else {
	    return;
	}
    }
    fname = functionName;
    gsub(",","", opcode);

    if (genSingle) {
	printf "SingleTable[%3d] = \"%s\";\n", opcode, fname;
    } else if (genVendPriv) {
	printf "VendorPrivTable[%3d] = \"%s\";\n", opcode, fname;
    } else if (genRender) {
	str = sprintf("\"%s\"", fname);
	printf "RenderTable[%3d] = %-16s; ", opcode, str;

	if (!(("glxflags", "client-handcode") in propListValues)) {
	    #
	    # Everything below is to generate info needed for the size tables.
	    #

	    #
	    # Set some flags for each parameter for convenience.
	    # This function is in the list utils.
	    #
	    setPerParamFlags();

	    #
	    # Sort the fixed-size parameters, largest element size first, so that
	    # we do not have to word-align them individually.
	    # This function is in the list utils.
	    #
	    sortParams();

	    #
	    # Compute sizes of all incoming parameters and the total size of the
	    # command.
	    # (this function is in the list utils)
	    #
	    computeInCmdSize();
	} else {
	    inFixCmdSize = 0;
	}

	printf "RenSize[%3d]=%3d; ", opcode, inFixCmdSize+4;
	printf "RenConst[%3d]=", opcode;
	if (inVarCmdSize) {
	    printf "0; ";
	} else {
	    printf "1; ";
	}

	#
	# Set a flag that says whether it's handcoded.
	#
	if (("glxflags", "client-handcode") in propListValues) {
	    printf "HC[%3d]=1;", opcode;
	} else {
	    printf "HC[%3d]=0;", opcode;
	}
	print;
    }
    if ((opcode + 0) < (min_opcode + 0)) {
	min_opcode = opcode;
    }
    if ((opcode + 0) > (max_opcode + 0)) {
	max_opcode = opcode;
    }
}

function finalize()
{
    if (genSingle) {
	printf "\nmin_single_opcode%s = %d;\n\n", SUFFIX, min_opcode;
	printf "\nmax_single_opcode%s = %d;\n\n", SUFFIX, max_opcode;
    }else if (genVendPriv) {
	printf "\nmin_vendpriv_opcode%s = %d;\n\n", SUFFIX, min_opcode;
	printf "\nmax_vendpriv_opcode%s = %d;\n\n", SUFFIX, max_opcode;
    } else if (genRender) {
	printf "\nmin_render_opcode%s = %d;\n\n", SUFFIX, min_opcode;
	printf "\nmax_render_opcode%s = %d;\n\n", SUFFIX, max_opcode;
    }
}
