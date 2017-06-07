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
# $Header$


#
# Generate prototypes for immediate mode routines.
#

function initialize() {

    typeMapFile = TYPEMAP;
    _readWireFile();

    linkage = "GLAPI";
    prefix = "__glim_";
    headerName = "_imfuncs_h_";

    printf "#include \"types.h\"\n";
    printf "#include \"apimacro.h\"\n";
    printf "\n\n";
}

function main( i, param, cmdlen, varsize) {

    if (("extension", "not_implemented") in propListValues) return;
    if (returnType == "GLstring") {
       returnType = "const GLubyte *";
    }

    printf "extern %s %s %s%s", returnType, linkage, prefix, functionName;
    printf "( ";
    outputCPrototype();
    printf ");\n";
}

function finalize()
{
    # Special optimized cases of immediate mode funcs.

    printf "\n\n";
    printf "extern void %s __glim_MipsVertex2fv(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex3fv(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex4fv(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex2fvFast(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex3fvFast(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex4fvFast(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex2fvFastest(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex3fvFastest(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsVertex4fvFastest(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsNoXFVertex2fv(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsNoXFVertex3fv(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsNoXFVertex4fv(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsNoXFVertex2fvFast(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsNoXFVertex3fvFast(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsNoXFVertex4fvFast(const GLfloat *);\n", linkage;
    printf "extern void %s __glim_MipsNoXFVertex2fvFast2D(const GLfloat *);\n", linkage;
    printf "";
    printf "extern void %s __glim_FastColor3ub(GLubyte, GLubyte, GLubyte);\n", linkage;
    printf "extern void %s __glim_FastColor3ubv(const GLubyte *);\n", linkage;
    printf "\n\n";
}

