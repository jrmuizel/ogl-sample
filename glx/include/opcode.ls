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
# Inc. The Original Code is Copyright (c) 1989-2000 Silicon Graphics, Inc.
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
#   This awk script is used to generate glxopcodes.h
#
#   $Date$ $Revision$
#   $Header$
#

function initialize() {

    typeMapFile = TYPEMAP;
    section = SECTION;
    prefix = PREFIX;

    init_case();

    if (section ~ /single/) {
	printf "/* Opcodes for single commands (part of GLX command space) */\n\n";
	opcodeType = "glxsingle";
    } else if (section == "glx") {
	printf "/* Opcodes for GLX commands */\n\n";
	opcodeType = "glxopcode";
    } else if (section ~ /vendpriv/) {
	printf "/* Opcodes for Vendor Private commands */\n\n";
	opcodeType = "glxvendorpriv";
    } else if (section ~ /vendorglx/) {
	printf "/* Opcodes for GLX vendor private commands */\n\n";
	opcodeType = "glxvendorglx";
    } else {
	printf "/* Opcodes for rendering commands */\n\n";
	opcodeType = "glxropcode";
    }
    opcodeFormat = "#define %s%-25s  %3d\n" ;

} # function intialize

function main( i, param, cmdlen, invarsize) {

    if ("vectorequiv" in propList) return;
    if (!(opcodeType in propList)) return;

    opcode = propList[opcodeType];
    sub(",", "", opcode);
    printf opcodeFormat, prefix, functionName, opcode;

}

function finalize()
{
    printf "\n\n";
}

# Return a string that's 'str' but with the 'n' char lower case.
function lower_char( str, n) {
    len = length(str);
    prefix = substr(str,1,n-1);
    suffix = substr(str,n+1,len-n);
    newstr = "";
    c = substr(str,n,1);
    if (c in lowcase) {
	newstr = prefix lowcase[c] suffix;
    } else {
	newstr = str;
    }
    return newstr;
}

function init_case () {
    lowcase["A"] = "a"
    lowcase["B"] = "b"
    lowcase["C"] = "c"
    lowcase["D"] = "d"
    lowcase["E"] = "e"
    lowcase["F"] = "f"
    lowcase["G"] = "g"
    lowcase["H"] = "h"
    lowcase["I"] = "i"
    lowcase["J"] = "j"
    lowcase["K"] = "k"
    lowcase["L"] = "l"
    lowcase["M"] = "m"
    lowcase["N"] = "n"
    lowcase["O"] = "o"
    lowcase["P"] = "p"
    lowcase["Q"] = "q"
    lowcase["R"] = "r"
    lowcase["S"] = "s"
    lowcase["T"] = "t"
    lowcase["U"] = "u"
    lowcase["V"] = "v"
    lowcase["W"] = "w"
    lowcase["X"] = "x"
    lowcase["Y"] = "y"
    lowcase["Z"] = "z"
}







