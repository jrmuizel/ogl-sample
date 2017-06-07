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
#   $Date$ $Revision$
#   $Header$
#

#
#  Declare the language and type map to use and initialize any special tables
#  required for processing.
#
function initialize() {

    # TYPEMAP should be defined on the command line that invokes libspec
    typeMapFile = TYPEMAP;

    OtherLibs["circle-lib"] = "circle-lib";
    OtherLibs["nurbs-lib"] = "nurbs-lib";
    OtherLibs["sgi-only"] = "sgi-only";
    OtherLibs["ui-lib"] = "ui-lib";
    OtherLibs["window-lib"] = "window-lib";
    OtherLibs["convenience-lib"] = "convenience-lib";
    OtherLibs["postponed-lib"] = "postponed-lib";
	
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

} # function intialize

function main( i, param, cmdlen, varsize) {

    # OUTPUT should be defined on the command line that invokes libspec
    #
    #  Print the function declaration
    #

    if (OUTPUT == "gl.api" || OUTPUT == "gl.api.rtn") {
	category = propList["category"];
	sub(",", "", category);
	if (category == "") category="unknown";
	    #
	    #  Print the function declaration
	    #
	if (returnType == "GLstring") returnString = "const\\0GLubyte\\0*";
	else returnString = returnType;
	printf "%-18s %-16s %-7s ", category, functionName, returnString;
	printf "( ";
	outputCArglist();
	printf " )\n";
    }
    if (OUTPUT == "glproto.h") {

    	fname = one_char_lower(functionName,1);

	printf "extern %s ", returnType
	printf "GL%s ", fname;
	printf "("
	outputCArglist()
	printf ");\n"
    }
    if (OUTPUT == "glcmds.c") {
	printf "%s %s", returnType, functionName;
	printf "( ";
	outputCArglist();
	printf ")\n";
	printf "{\n";
	printf "}\n\n";
    }

}

function finalize()
{
}

# Return a string that's 'str' but with the 'n' char lower case.
function one_char_lower( str, n) {
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

