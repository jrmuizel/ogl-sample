
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
# $Header$

#
# Generate header files related to the dispatch tables.  The dispatch
# tables only contain entries that are display listable.  Things which
# are immediate mode only will not be represented here. For example:
# any of the gets, glNewList,  glPixelStoref, etc.
#

#
# if you change this, don't forget to update the device-specific ltab.ls's!!!
#

function initialize()
{
    typeMapFile = TYPEMAP;
    _readWireFile();

    # What kind of output is desired
    output = OUTPUT;

    outputHeader = (output ~ /g_ltab_h/);
    outputCompile = (output ~ /g_ltab_c/);
    outputExecute = !outputHeader && !outputCompile;

    #
    # Patterns for deciding when to generate glle function definition
    #

    noexec["Lighti"] = "true"
    noexec["Lightf"] = "true"
    noexec["LightModeli"] = "true"
    noexec["LightModelf"] = "true"
    noexec["Materiali"] = "true"
    noexec["Materialf"] = "true"
    noexec["TexParameteri"] = "true"
    noexec["TexParameterf"] = "true"
    noexec["TexEnvi"] = "true"
    noexec["TexEnvf"] = "true"
    noexec["TexGeni"] = "true"
    noexec["TexGenf"] = "true"
    noexec["TexGend"] = "true"

#
# Function Names <fname> that have <fname>v$ forms. Don't generate __glle_
#  forms of these functions.
#

    beginend = "Color|Normal|TexCoord|Index|EdgeFlag|Vertex|EvalCoord|MultiTexCoord"
    other = "TexGen|TexParameter|TexEnv|LightModel|Rect|RasterPos"
    sameargs = "Light|LightModel|Material|TexParameter|TexGen|TexEnv"
    degree = "[1234]?"
    datatype = "u?[bdfis]?"
    arb = "(ARB)?"

    vform = "^" "(" beginend "|" other ")" degree datatype arb "$"
    argmatch = "^" "(" sameargs ")" "[ifd]v" "$"

#
#   Remap the Map function
#

    remap["Map1f"] = "Map1"	
    remap["Map2f"] = "Map2"	
    remap["Map1d"] = "Map1"	
    remap["Map2d"] = "Map2"	
    remap["DeformationMap3fSGIX"] = "DeformationMap3SGIX"	
    remap["DeformationMap3dSGIX"] = "DeformationMap3SGIX"	



    printf("#include \"dlist.h\"\n");
    printf("#include \"g_lcfncs.h\"\n");
    printf("#include \"g_lcomp.h\"\n");

    printf("\n");

    printf("/*\n");
    printf("** Table of display list execution routines. This table is\n");
    printf("** common for all implementations because the display list\n");
    printf("** execution function is actually compiled into the display\n");
    printf("** list, so an implementation cannot simply change one of\n");
    printf("** these entries and expect that existing display lists will\n");
    printf("** use the new function pointer.\n");
    printf("*/\n");

    printf("__GLlistExecFunc *__glListExecTable[] = {\n");
}

#############################################################################
#
# Generated _gllc_*_Rec structs and __gllc() and __glle() function prototypes
# for lcomp.h (now g_ltab.c)
#
#############################################################################

function main(i, j, param, cmdlen)
{
    if (("dlflags", "notlistable") in propListValues) return;
    if (("extension", "not_implemented") in propListValues) return;
    if(noexec[functionName]) return;

    # handle "Begin" in special way

    if(functionName ~ "^Begin$") {
	printf("\t0,\n");
	return;
    }

    # toss extra map function definitions

    if(functionName ~ "^Map[12]d$" || functionName ~ "DeformationMap3dSGIX$")
	return;

    # put in redundant definitions

    if(functionName ~ "^Map[12]f$" ||
       functionName ~ "DeformationMap3fSGIX$")
        printf("\t__glle_%s, /* redundant on purpose */\n",remap[functionName]);

    if(functionName ~ argmatch)
	printf("\t__glle_%s, /* redundant on purpose */\n", functionName);


    # main definitions

    if(functionName !~ vform) {
	if(functionName ~ "^Map[12][f]$" ||
	   functionName ~ "DeformationMap3fSGIX$")
	    printf("\t__glle_%s,\n", remap[functionName]);
	else
	    printf("\t__glle_%s,\n", functionName);
    }

}

function finalize()
{
	printf("};\n");
}
