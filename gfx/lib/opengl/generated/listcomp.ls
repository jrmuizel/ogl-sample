
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
# $Header: //depot/main/gfx/lib/opengl/generated/listcomp.ls#6 $

#
# Generate header files related to the dispatch tables.  The dispatch
# tables only contain entries that are display listable.  Things which
# are immediate mode only will not be represented here. For example:
# any of the gets, glNewList,  glPixelStoref, etc.
#

function initialize()
{
    typeMapFile = TYPEMAP;
    _readWireFile();

    # What kind of output is desired
    output = OUTPUT;

    outputHeader = (output ~ /g_lcomp_h/);
    outputCompile = (output ~ /g_lcomp_c/);
    outputExecute = !outputHeader && !outputCompile;

    #
    # Byte sizes of implementation types.
    #

    xfer_size["GLenum"] =         "4"
    xfer_size["GLboolean"] =      "1"
    xfer_size["GLbitfield"] =     "4"
    xfer_size["GLbyte"] =         "1"
    xfer_size["GLshort"] =        "2"
    xfer_size["GLint"] =          "4"
    xfer_size["GLsizei"] =        "4"
    xfer_size["GLubyte"] =        "1"
    xfer_size["GLushort"] =       "2"
    xfer_size["GLuint"] =         "4"
    xfer_size["GLfloat"] =        "4"
    xfer_size["GLclampf"] =       "4"
    xfer_size["GLdouble"] =       "8"
    xfer_size["GLclampd"] =       "8"
    xfer_size["GLvoid"] =         "1"     # A lie, but it works


    #
    # Array to map the type of last variable in struct to type of
    # padding variable; pad_type[<type of last variable>] = <type for padding>
    #

    pad_type["GLubyte"] = "GLubyte"
    pad_type["GLbyte"] = "GLubyte"
    pad_type["GLushort"] = "GLushort"
    pad_type["GLshort"] = "GLushort"
    pad_type["GLboolean"] = "GLubyte"

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
# XXX Hack Alert!! There should be a way of figuring out this information
# from the gl.spec file, or an algorithm that doesn't use the function name
#
    forcestruct["Enable"] = "true";
    forcestruct["Disable"] = "true";
    forcestruct["PolygonStipple"] = "true";
    forcestruct["TexImage1D"] = "true";			imagesize["TexImage1D"] = "true";
    forcestruct["TexImage2D"] = "true";			imagesize["TexImage2D"] = "true";
    forcestruct["DrawPixels"] = "true";			# probably should use imagesize (db)

    forcestruct["TexImage3D"] = "true";			imagesize["TexImage3D"] = "true";
    forcestruct["TexSubImage1D"] = "true";		imagesize["TexSubImage1D"] = "true";
    forcestruct["TexSubImage2D"] = "true";		imagesize["TexSubImage2D"] = "true";
    forcestruct["TexSubImage3D"] = "true";		imagesize["TexSubImage3D"] = "true";
    forcestruct["ConvolutionFilter1D"] = "true";	imagesize["ConvolutionFilter1D"] = "true";
    forcestruct["ConvolutionFilter2D"] = "true";	imagesize["ConvolutionFilter2D"] = "true";
    forcestruct["ColorTable"] = "true";			imagesize["ColorTable"] = "true";
    forcestruct["ColorSubTable"] = "true";		imagesize["ColorSubTable"] = "true";
    forcestruct["SeparableFilter2D"] = "true";
    forcestruct["Histogram"] = "true";

#
# Function Names <fname> that have <fname>v$ forms. Don't generate __glle_
#  forms of these functions.
#

    beginend = "Color|Normal|TexCoord|Index|EdgeFlag|Vertex|EvalCoord|MultiTexCoord"
    other = "TexGen|TexParameter|TexEnv|LightModel|Rect|RasterPos"
    degree = "[1234]?"
    datatype = "u?[bdfis]?"
    arb = "(ARB)?"

    vform = "^" "(" beginend "|" other ")" degree datatype arb "$"

#
#   Remap the Map function
#

    remap["Map1f"] = "Map1"	
    remap["Map2f"] = "Map2"	
    remap["DeformationMap3fSGIX"] = "DeformationMap3SGIX"	

    printf "#if (_MIPS_SZPTR == 64)\n";
    printf "#define __GL_PAD(x) (((x) + 7) & ~7) /* round to doubleword multiple */\n";
    printf "#define __4_BYTE_PAD  GLbyte __4_byte_pad[4];\n";
    printf "#define __GL64PAD(x)  __GL_PAD(x)    /* optional double word padding */\n";
    printf "#else\n";
    printf "#define __GL_PAD(x) (((x) + 3) & ~3) /* round to word multiple */\n";
    printf "#define __4_BYTE_PAD\n";
    printf "#define __GL64PAD(x) (x)             /* not needed for 32-bit case */\n";
    printf "#endif\n\n\n";

    clearArray(sortparam);

    supportDoubles = 1;         # don't coerce all double precision to float
}

#############################################################################
#
# Generated _gllc_*_Rec structs and __gllc() and __glle() function prototypes
# for lcomp.h (now g_lcomp.h)
#
#############################################################################
function main(i, j, param, cmdlen)
{
    if (("dlflags", "notlistable") in propListValues) return;
    if (("extension", "not_implemented") in propListValues) return;
    if (!outputHeader && noexec[functionName]) return;

    #
    # print __gllc_*_Rec structure
    #

    maxsort = 0;

    if(paramCount != 0 &&
        (propList["dlflags"] !~ "handcode" || forcestruct[functionName])) {
	printf("struct __gllc_%s_Rec {\n", functionName);
    	argsize = 0;
    	for(i = 1; i <= paramCount; i++) {
	    param = paramName[i]
	    ttype = paramTransferType[param]
	    dtype = paramDeclaredType[param]

	    if (!supportDoubles) {
		if (dtype == "GLdouble") dtype = "GLfloat";
		if (dtype == "GLclampd") dtype = "GLclampf";
	    }

	    if(ttype == "array") {
	    	argsize += xfer_size[dtype] * paramSubscripts[param,1];
	    } else {
	    	argsize += xfer_size[dtype];
	    }
	    if(ttype == "array") {
		array_type = paramSubscripts[param,1];
#		if(array_type ~ "COMPSIZE" || array_type ~ "mapsize")
		if(array_type + 0 == 0)
		    if(array_type == "COMPSIZE()")
			if(dtype ~ /(double|clampd)/)
			    printf("      %s %s[128];\n", dtype, param);
			else {
			    sortparam[i] = sprintf("%s %s[128];\n", dtype, 
					           param);
			    maxsort = i;
			}
		    else {
			    if (imagesize[functionName] == "true") {
				sortparam[i] = sprintf("GLint imageSize;\n");
				argsize += xfer_size["GLint"];
				i++;
			    }
			    sortparam[i] = sprintf("/*    %s    */\n", 
						   param);
			    maxsort = i;
		    }
	    	else
		    if(dtype ~ /(double|clampd)/)
	   	        printf("      %s %s[%d];\n", dtype, param, array_type);
		    else {
			sortparam[i] = sprintf("%s %s[%d];\n", dtype, 
					       param, array_type);
			maxsort = i;
		    }
	    } else { # non-array
	        if(dtype ~ /(double|clampd)/)
		    printf("      %s %s;\n", dtype, param);
		else {
		    sortparam[i] = sprintf("%s %s;\n", dtype, param);
		    maxsort = i;
		}
	    }
        }

	# print out the non-double params
	for (i = 1; i <= maxsort; i++) {
		printf("      %s", sortparam[i]);
		delete sortparam[i];
	}

        if (argsize % 4) { # padding is always last
	    for(j = 1; j <= 4 - argsize % 4;j += xfer_size[dtype]) {
	        printf("     %s pad%d;\n", pad_type[dtype], j);
	    }
	    argsize += 4 - argsize % 4;
        }
	if (argsize % 8) { # doubleword padiing
	    printf "    __4_BYTE_PAD\n";
	}
        printf("};\n");
    }

    #
    # print __gllc_* function
    #

    printf("extern void GLAPI __gllc_%s(", functionName);
    outputCArglist();
    printf(");\n");

    #
    # print __glle_* function
    #

    if(functionName !~ vform) {
        # Map functions is handled separately
	if(functionName ~ "^Map[12][fd]$" ||
	   functionName ~ "DeformationMap3[fd]SGIX$") {
	    if(remap[functionName])
		printf("extern const GLubyte *__glle_%s(const GLubyte *);\n",
		       remap[functionName]);
	} else { 
		printf("extern const GLubyte *__glle_%s(const GLubyte *);\n",
		       functionName);
	}
    }

    #
    # produce prototype for _size function, if needed 
    #

    lastSub = ""; # reset (used to print only one of identical set of _size fns)
    if(propList["dlflags"] !~ "handcode") {
	for(i = 1; i <= paramCount; i++) {
	    param = paramName[i];
	    subScript = paramSubscripts[param,1]; # assuming 1-d arrays only
	    if (subScript ~ /COMPSIZE/ &&
		subScript != lastSub) { # if COMPSIZE(<list>), process <list>
		lastSub = subScript;
	        gsub(/COMPSIZE\(/,"",subScript);
	        gsub(/\)/,"",subScript);
	        max = split(subScript, arrayArgs, "/");
	        printf("extern GLint __gl%s_size(", functionName);
	        for(j = 1; j < max; j++) {
		    printf("%s ", paramDeclaredType[arrayArgs[j]]);
		    printf("%s, ", arrayArgs[j]);
	        }
	        printf("%s ", paramDeclaredType[arrayArgs[j]]);
	        printf("%s);\n", arrayArgs[j]);
	    }
	}
    }
    #print blank line between groups, for clarity

    printf("\n");
}

    
function finalize()
{
printf("extern void GLAPI __gllc_UnimplementedExtension(void);\n");
printf("extern const GLubyte *__glle_UnimplementedExtension(const GLubyte *);\n");
}
