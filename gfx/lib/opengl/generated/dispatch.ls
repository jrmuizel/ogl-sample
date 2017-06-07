
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
# $Header: //depot/main/gfx/lib/opengl/generated/dispatch.ls#6 $

#
# Generate header files related to the dispatch tables.
#
# This needs to be kept in sync with api.ls
#

function initialize()
{
    typeMapFile = TYPEMAP;
    _readWireFile();

    # What kind of output to generate
    output = OUTPUT;

    # Options to the output generation
    options = OPTIONS;

    # 
    linkage = "GLAPI";

    # prefix to use for generated names
    prefix = PREFIX;

    outputDispatchTableHeader = (output ~ /g_disp_h/);
    outputDispatchTable = (output ~ /g_disp_c/);
    outputGLXTable = (output ~ /g_glxtab_c/);
    outputNopTable = (output ~ /g_noptab_c/);

    extEntryNumber = 0;
    otherEntryNumber = 0;

    nopFuncNumber = 0;
}

function makePrototype()
{
    prototype = "";
    if (paramCount == 0) {
	prototype = "void";
    } else {
	for (i=1; i<=paramCount; i++) {
	    #
	    #  Print the argument type
	    #
	    param = paramName[i]
	    ttype = paramTransferType[param]
	    dtype = paramDeclaredType[param]

	    if (paramDirection[param] == "in" &&
		    (ttype == "array" || ttype == "reference")) {
		prototype = prototype "const ";
	    }

	    if (param == "...") {
		prototype = prototype param;
		break;		# must be the last
	    }

	    prototype = prototype dtype;
	    if (dtype == "...")
		break;		# must be the last

	    if ((ttype == "reference") || (ttype == "array")) {
		prototype = prototype " *";
	    }

	    if (i != paramCount) {
		prototype = prototype ", ";
	    }
	}
    }
}

#
# Parameter specific nop functions are needed for the x86 __stdcall and
# __fastcall calling conventions which have the callee clean up the
# stack.
#
# We reduce the number of nop functions by observing that pointer, char,
# short, and float arguments take the same amount of stack space as an
# int argument, and double arguemtents take the same amount of stack space
# as two int arguments.
#
function makeNopFuncName()
{
    nopFuncName = "__glnop_Nop";
    if (paramCount > 0) {
	nopFuncProto = "";
    } else {
	nopFuncProto = "void";
    }
    nopIntArgCount = 0;
    for (i=1; i<=paramCount; i++) {
	nopParamName = paramName[i];
	if (paramTransferType[nopParamName] == "reference" ||
	    paramTransferType[nopParamName] == "array")
	{
#	    nopFuncName = nopFuncName "P";
#	    nopFuncProto = nopFuncProto "void *p" i;
	    nopIntArgCount++;
	    nopFuncProto = nopFuncProto "GLint i" nopIntArgCount;
	} else
	if (paramDeclaredType[nopParamName] == "GLenum" ||
	    paramDeclaredType[nopParamName] == "GLbitfield" ||
	    paramDeclaredType[nopParamName] == "GLint" ||
	    paramDeclaredType[nopParamName] == "GLsizei" ||
	    paramDeclaredType[nopParamName] == "GLuint")
	{
#	    nopFuncName = nopFuncName "I";
#	    nopFuncProto = nopFuncProto "GLint i" i;
	    nopIntArgCount++;
	    nopFuncProto = nopFuncProto "GLint i" nopIntArgCount;
	} else 
	if (paramDeclaredType[nopParamName] == "GLboolean" ||
	    paramDeclaredType[nopParamName] == "GLbyte" ||
	    paramDeclaredType[nopParamName] == "GLubyte")
	{
#	    nopFuncName = nopFuncName "B";
#	    nopFuncProto = nopFuncProto "GLbyte b" i;
	    nopIntArgCount++;
	    nopFuncProto = nopFuncProto "GLint i" nopIntArgCount;
	} else 
	if (paramDeclaredType[nopParamName] == "GLshort" ||
	    paramDeclaredType[nopParamName] == "GLushort")
	{
#	    nopFuncName = nopFuncName "S";
#	    nopFuncProto = nopFuncProto "GLshort s" i;
	    nopIntArgCount++;
	    nopFuncProto = nopFuncProto "GLint i" nopIntArgCount;
	} else 
	if (paramDeclaredType[nopParamName] == "GLfloat" ||
	    paramDeclaredType[nopParamName] == "GLclampf")
	{
#	    nopFuncName = nopFuncName "F";
#	    nopFuncProto = nopFuncProto "GLfloat f" i;
	    nopIntArgCount++;
	    nopFuncProto = nopFuncProto "GLint i" nopIntArgCount;
	} else 
	if (paramDeclaredType[nopParamName] == "GLdouble" ||
	    paramDeclaredType[nopParamName] == "GLclampd")
	{
#	    nopFuncName = nopFuncName "D";
#	    nopFuncProto = nopFuncProto "GLdouble d" i;
	    nopIntArgCount++;
	    nopFuncProto = nopFuncProto "GLint i" nopIntArgCount ", ";
	    nopIntArgCount++;
	    nopFuncProto = nopFuncProto "GLint i" nopIntArgCount;
	}

	if (i < paramCount) {
	    nopFuncProto = nopFuncProto ", ";
	}
    }

    nopFuncName = nopFuncName nopIntArgCount;

    if (!(nopFuncName in nopFuncNameList)) {
	nopFuncNameList[nopFuncName] = true;
	nopFuncNameTable[nopFuncNumber] = nopFuncName;
	nopFuncProtoTable[nopFuncNumber++] = nopFuncProto;
    }
}

function main( i, param, cmdlen, varsize)
{
    if (("extension") in propList) {
	if (("extension", "not_implemented") in propListValues) return;
	# Place implementation-specific cases here
    }

    makePrototype();
    prototypes[functionName] = prototype;
    if (returnType == "GLstring") {
	returnTypes[functionName] = "const GLubyte *";
    } else {
	returnTypes[functionName] = returnType;
    }

    makeNopFuncName();
    if ("extension" in propList) {
        extNopTable[extEntryNumber] = nopFuncName;
        extPrefix[extEntryNumber] = "gllc";
        if (("dlflags", "notlistable") in propListValues) {
            extPrefix[extEntryNumber] = "glim";
        }
        extTable[extEntryNumber++] = functionName;
    }

    otherNopTable[otherEntryNumber] = nopFuncName;
    otherPrefix[otherEntryNumber] = "gllc";
    if (("dlflags", "notlistable") in propListValues) {
       otherPrefix[otherEntryNumber] = "glim";
    }
    otherTable[otherEntryNumber++] = functionName;
}

function doDispatchTableHeader()
{
    printf "#if defined(i386)\n";
    printf "#define SZPTR (32>>3)\n";
    printf "#define SZINT 4\n";
    printf "#endif\n\n";

    printf "#if defined(mips)\n";
    printf "#define SZPTR (_MIPS_SZPTR>>3)\n";
    printf "#endif\n\n";

    # First output table indicies
    for (i = 0; i < otherEntryNumber; i++) {
	printf "#define __GL_TAB_%s %d\n", otherTable[i], i;
    }
    printf "#define __GL_TAB_SIZE (%d*SZPTR)\n", otherEntryNumber;
    printf "#define __GL_TAB_ENTRIES %d\n\n", otherEntryNumber;

    totalEntryNumber = (otherEntryNumber + extEntryNumber);
    dispatchStateSize = totalEntryNumber;

    printf "#define __GL_DISPATCHSTATE_SIZE (%d*SZPTR+8)\n",dispatchStateSize;
    printf "\n";

    # Now output table definitions for C
    printf "#if defined(_LANGUAGE_C) || defined(__cplusplus)\n";

    printf "struct __GLdispatchTableRec {\n";
    for (i = 0; i < otherEntryNumber; i++) {
	fn = otherTable[i];
	printf "    %s (%s*%s)(%s);\n", returnTypes[fn], linkage, fn, prototypes[fn];
    }
    printf "};\n\n";

    printf "typedef struct __GLdispatchTableRec __GLdispatchTable;\n\n";
    printf "struct __GLdispatchStateRec {\n";
    printf "    __GLdispatchTable dispatch;\n";
    printf "};\n\n";

    printf "#endif /* _LANGUAGE_C || __cplusplus */\n";
}

function doGLXTable()
{
    printf "#include \"g_glxfuncs.h\"\n\n";
    printf "#include \"g_disp.h\"\n\n";

    printf "__GLdispatchTable __glx_dispatchTable = {\n";
    for (i = 0; i < otherEntryNumber; i++) {
	fn = otherTable[i];
	printf "    %s%s,\n", prefix, fn;
    }
    printf "};\n\n";

    printf "__GLextensionsDispatchTable __glx_extensionsDispatchTable = {\n";
    for (i = 0; i < extensionsEntryNumber; i++) {
	fn = extTable[i];
	printf "    %s%s,\n", prefix, fn;
    }
    printf "};\n\n";

}

function doDispatchTable()
{
    printf "#include \"g_imfncs.h\"\n";
    printf "#include \"g_disp.h\"\n";
    printf "#include \"g_lcfncs.h\"\n";
    printf "\n";

    # output immediate mode dispatch table 
    printf "__GLdispatchState __glImmedState = {\n    {\n";
    for (i = 0; i < otherEntryNumber; i++) {
        printf "    __glim_%s,\n", otherTable[i];
    }
    printf "    }\n};\n\n/*"
    for (i=0; i < 75; i++) printf "*";
    printf "*/\n\n";

    # output list compilation mode dispatch table
    printf "__GLdispatchState __glListCompState = {\n    {\n";
    for (i = 0; i < otherEntryNumber; i++) {
        printf "    __%s_%s,\n", otherPrefix[i], otherTable[i];
    }
    printf "    }\n};\n\n"
}


function doNopTable()
{
    printf "#include \"types.h\"\n";
    printf "#include \"GL/gl.h\"\n";
    printf "#include \"g_disp.h\"\n\n";

    for (i = 0; i < nopFuncNumber; i++) {
	nfn = nopFuncNameTable[i];
	nfp = nopFuncProtoTable[i];
	printf "GLint GLAPI %s(%s);\n", nfn, nfp;
    }
    printf "\n\n";

    for (i = 0; i < nopFuncNumber; i++) {
	nfn = nopFuncNameTable[i];
	nfp = nopFuncProtoTable[i];
	printf "GLint GLAPI %s(%s)\n", nfn, nfp;
	printf "{\n    return 0;\n}\n\n";
    }

    printf "__GLdispatchState __glNopDispatchState = {\n    {\n";
    for (i = 0; i < otherEntryNumber; i++) {
        nfn = otherNopTable[i];
        fn = otherTable[i];
        pad = (length(nfn) < 18) ? "\t\t" : "\t";
        printf "    (void *) %s,%s/* %s */\n", nfn, pad, fn;
    }

    printf "    }\n};\n\n";
}

function finalize()
{
    if (outputDispatchTableHeader) {
	doDispatchTableHeader();
	return;
    }
    if (outputDispatchTable) {
        doDispatchTable();
        return;
    }
    if (outputGLXTable) {
	doGLXTable();
	return;
    }
    if (outputNopTable) {
	doNopTable();
	return;
    }
}
