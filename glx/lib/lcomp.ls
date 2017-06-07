##############################################################################
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
##############################################################################
#
#   Generate the list compilation routines and the GLX client transport
#   routines.
#
#   $Date$ $Revision$
#   $Header$
#
##############################################################################

function initialize() {

    # TYPEMAP should be defined on the command line that invokes libspec
    typeMapFile = TYPEMAP;
    _readWireFile();

    # Functions that will be generated, should be defined on command line.
    funcs = FUNCS;

    noship = 		(NOSHIP ~ /yes/);
    genTransDraw =    	(funcs ~ /transdraw/);
    genTransSingle =  	(funcs ~ /transsingle/);
    genTransVendpriv = 	(funcs ~ /transvendpriv/);
    genSpecRender =   	(funcs ~ /specrender/);
    genEncodeRender = 	(funcs ~ /encoderender/);
    genSpecSingle =   	(funcs ~ /specsingle/);
    genSpecVendpriv = 	(funcs ~ /specvendpriv/);
    genEncodeSingle = 	(funcs ~ /encodesingle/);
    genEncodeVendpriv =	(funcs ~ /encodevendpriv/);
    genTrans = 	     	genTransDraw || genTransSingle || genTransVendpriv;
    genSpec = 		genSpecRender || genSpecSingle || genSpecVendpriv;
    genEncode = 	genEncodeRender || genEncodeSingle || genEncodeVendpriv;
    
    #
    # Try to encapsulate the differences between different output here.
    #
    
    if (genTransDraw) {
	functionPrefix = "gl";
	opcodePrefix = "X_GLrop_";
	macroPrefix = "__GLX";
	shortMacroPrefix = "__GLX";
	GLXheaders = 1;
    } else if (genTransSingle) {
	functionPrefix = "gl";
	opcodePrefix = "X_GLsop_";
	macroPrefix = "__GLX_SINGLE";
	shortMacroPrefix = "__GLX";
	GLXheaders = 1;
    } else if (genTransVendpriv) {
	functionPrefix = "gl";
	opcodePrefix = "X_GLvop_";
	macroPrefix = "__GLX_VENDPRIV";
	shortMacroPrefix = "__GLX";
	GLXheaders = 1;
    }

    #
    # Set the maximum size of fixed-sized commands; use to optimize space
    # checking of the transport buffer or list buffer.
    #
    MAX_FIXED_SIZE = 80;
    
    #
    # Partial names of macros used to send each wire type.
    #
    cast["enum"] =	"LONG"
    cast["boolean"] =	"CHAR"
    cast["bitfield"] =	"LONG"
    cast["byte"] =	"CHAR"
    cast["short"] =	"SHORT"
    cast["int"] =	"LONG"
    cast["sizei"] =	"LONG"
    cast["ubyte"] =	"CHAR"
    cast["ushort"] =	"SHORT"
    cast["uint"] =	"LONG"
    cast["float"] =	"FLOAT"
    cast["clampf"] =	"FLOAT"
    cast["double"] =	"DOUBLE"
    cast["clampd"] =	"DOUBLE"
    cast["void"] =      "VOID"
      
    #
    # X protocol wire types
    #
    proto["enum"] =	"ENUM"
    proto["boolean"] =	"BOOL"
    proto["bitfield"] =	"BITFIELD"
    proto["byte"] =	"INT8"
    proto["short"] =	"INT16"
    proto["int"] =	"INT32"
    proto["sizei"] =	"INT32"
    proto["ubyte"] =	"CARD8"
    proto["ushort"] =	"CARD16"
    proto["uint"] =	"CARD32"
    proto["float"] =	"FLOAT32"
    proto["clampf"] =	"FLOAT32"
    proto["double"] =	"FLOAT64"
    proto["clampd"] =	"FLOAT64"
    proto["void"] =     "VOID"		# these will have to be handcoded
    #
    # Define the return types for the few routines that have them (and are
    # generated).
    #
    protoReturn["boolean"] =	"BOOL32"
    protoReturn["int"] =	"INT32"
    protoReturn["uint"] =	"CARD32"
      
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
    xfer_size["void"] =         "1"		# A lie, but good enough

    #
    # Include header files.
    #
    if (GLXheaders) {
	if (genTransSingle) {
	    printf "#include \"packsingle.h\"\n\n";
	} else if (genTransDraw) {
	    printf "#include \"packrender.h\"\n\n";
	} else if (genTransVendpriv) {
	    printf "#include \"packvendpriv.h\"\n\n";
	}
    }
}

function main( i, param, cmdlen) {

    #
    # Filter out unwanted routines.
    #
    if (genTransSingle) {
	if (!("glxsingle" in propList)) return;
	if (!(("dlflags", "notlistable") in propListValues)) return;
	if (("glxflags", "client-only") in propListValues) return;
	if (("glxflags", "client-handcode") in propListValues) return;
    } else if (genTransVendpriv) {
	if (!("glxvendorpriv" in propList)) return;
	if (!(("dlflags", "notlistable") in propListValues)) return;
	if (("glxflags", "client-only") in propListValues) return;
	if (("glxflags", "client-handcode") in propListValues) return;
    } else if (genTransDraw) {
	if (("dlflags", "notlistable") in propListValues) return;
	if (("glxflags", "client-only") in propListValues) return;
	if (("glxflags", "client-handcode") in propListValues) return;
    } else if (genSpecRender || genEncodeRender) {
	if (("dlflags", "notlistable") in propListValues) return;
	if (("glxflags", "client-only") in propListValues) return;
	if (noship) {
	    if (!(("glxflags", "client-handcode") in propListValues)) return;
	} else {
	    if (("glxflags", "client-handcode") in propListValues) return;
	}
    } else if (genSpecSingle || genEncodeSingle) {
        if (!("glxsingle" in propList)) return;
	if (!(("dlflags", "notlistable") in propListValues)) return;
	if (("glxflags", "client-only") in propListValues) return;
	if (noship) {
	    if (!(("glxflags", "client-handcode") in propListValues)) return;
	} else {
	    if (("glxflags", "client-handcode") in propListValues) return;
	}
    } else if (genSpecVendpriv || genEncodeVendpriv) {
        # No longer handled with SpecSingles above, since that led
        #   to EXTensions creeping into the core GLX protocol specification.
	if (!("glxvendorpriv" in propList)) return;
	if (!(("dlflags", "notlistable") in propListValues)) return;
	if (("glxflags", "client-only") in propListValues) return;
	if (noship) {
	    if (!(("glxflags", "client-handcode") in propListValues)) return;
	} else {
	    if (("glxflags", "client-handcode") in propListValues) return;
	}
    }

    #
    # Write the opcode.  Routines that have vector equivalents get mapped to
    # them.  We have to check this here because the protocol generators need
    # to bail out now if there are vectorequivs.
    #
    if ("vectorequiv" in propList) {
	if (genSpec || genEncode) return;
	opcodeName = propList["vectorequiv"];
	sub( ",", "", opcodeName);
    } else {
	if ("glxropcode" in propList) {
	    opcodeName = functionName;
	    opcode = propList["glxropcode"];
	    sub( ",", "", opcode);
	} else if ("glxsingle" in propList) {
	    opcodeName = functionName;
	    opcode = propList["glxsingle"];
	    sub( ",", "", opcode);
	} else if ("glxvendorpriv" in propList) {
	    opcodeName = functionName;
	    opcode = propList["glxvendorpriv"];
	    sub( ",", "", opcode);
	} else {
	    specError("No glxropcode, glxvendorpriv, or glxsingle defined for " functionName);
	}
    }

    fname = functionName;

    #
    # Set some flags for each parameter for convenience.
    # (this function is in the list utils)
    #
    setPerParamFlags();

    if (genEncode && hasInCompSizeParam && !noship) {
	return;
    }

    #
    # Sort all parameters, largest elements first, so that we don't have to
    # word-align them individually. (this function is in the list utils)
    #
    sortParams();

    #
    # Compute the total size of the command going "in" to the server.
    #
    computeInCmdSize();
    computeOutCmdSize();

    #
    # Intercepted routines have a special prefix.
    # Note: glFlush is the only client-intercept so far.
    if (("glxflags", "client-intercept") in propListValues &&
	(genTransDraw || genTransSingle || genTransVendpriv)) {
	internalPrefix = "__";
    } else {
	internalPrefix = "";
    }

    #
    # Declare the procedure.
    #
    if (genTrans) {
    	printf "%s %s%s%s", returnType, internalPrefix, functionPrefix, fname;
    	printf "(";
    	outputCArglist();
    	printf ")\n";
    	printf "{\n";
    	if (genTransSingle) {
		printf "\t__GLX_SINGLE_DECLARE_VARIABLES();\n";
    	}else if (genTransVendpriv) {
		printf "\t__GLX_VENDPRIV_DECLARE_VARIABLES();\n";
    	} else if (genTransDraw) {
		printf "\t__GLX_DECLARE_VARIABLES();\n";
    	}
    } else if (genSpecRender) {
	printf ".Or %s\n", functionName;
    } else if (genSpecSingle) {
	printf ".Na %s\n", functionName;
	printf ".Rq\n";
	printf ".Pa tag GLX_CONTEXT_TAG\n";
    } else if (genSpecVendpriv) {
	printf ".Na %s\n", functionName;
	printf ".Rq\n";
	printf ".Pa tag GLX_CONTEXT_TAG\n";
    } else if (genEncode) {
        printf "\\Ao{%s}\n", functionName;
    }

    #
    # Declare local to store computed size of variable-sized arrays.
    #
    if (hasInCompSizeParam && genTrans) {
	printf "\tcompsize = %s;\n",
	    getCompSizeParamList(inCompSizeParamIndex);
    }

    #
    # Declare a local to store the return value.  Only genTransSingles do this.
    #
    if (hasReturn && genTrans) {
	printf "\t%s    retval = 0;\n", returnType;
    }

    if (genTransSingle && \
	(hasReturn || numOutFixParams > 0 || numOutVarParams > 0)) {
        printf "\txGLXSingleReply reply;\n";
    }

    if (genTransVendpriv && \
	(hasReturn || numOutFixParams > 0 || numOutVarParams > 0)) {
        printf "\txGLXVendorPrivReply reply;\n";
    }

    if (genTransSingle) {
	printf "\t__GLX_SINGLE_LOAD_VARIABLES();\n";
    } else if (genTransVendpriv) {
	printf "\t__GLX_VENDPRIV_LOAD_VARIABLES();\n";
    } else if (genTransDraw) {
	printf "\t__GLX_LOAD_VARIABLES();\n";
    }

    #
    # Make a list of non-redundant ParamSizeParams, because otherwise
    # the same size param will be checked more than once.
    #
    clearArray(inParamSizeParamCount);
    for (i in inParamSizeParamIndices) {
       inParamSizeParamCount[arrayCount[inParamSizeParamIndices[i]]];
    }

    #
    # If a size parameter is invalid, ignore the command.
    #
    for (count in inParamSizeParamCount) {
    	if (genTrans) {
	    if (hasReturn) {
		printf "\tif \(%s < 0\) return retval;\n", count;
	    }
	    else {
		printf "\tif \(%s < 0\) return;\n", count;
	    }
    	}
    }

    #
    # Compute sizes of all "in" parameters and the total size of the
    # command.  The fixed length is initialized to 4 to include opcode (2)
    # and size of command (2).
    #
    if (genTransDraw || genSpec || genEncode) {
        inFixCmdSize += 4;
    }
    if (numInVarParams > 0 ) {
	if (inVarParamsNeedPad) {
	    cmdlen = sprintf("%s_PAD\(%d%s\)", shortMacroPrefix, inFixCmdSize, \
		             inVarCmdSize);
	} else {
	    cmdlen = sprintf("%d%s", inFixCmdSize, inVarCmdSize);
	}
	if (genTrans) printf "\tcmdlen = %s;\n", cmdlen;
    } else {
	cmdlen = inFixCmdSize;
    }

    #
    # Do whatever is needed to initialize the buffer space before we start
    # writing parameters.
    #
    if (numInVarParams > 0) {
	str = "cmdlen";
    } else {
	str = cmdlen;
    }
    if (genTrans) {
	if (genTransVendpriv) {
	    if (hasReturn || ((numOutFixParams + numOutVarParams)) > 0) {
		printf "\t%s_BEGIN(%s,%s%s,%s);\n", macroPrefix,\
		     "X_GLXVendorPrivateWithReply", opcodePrefix, opcodeName, str;
	    } else {
		printf "\t%s_BEGIN(%s,%s%s,%s);\n", macroPrefix,\
		     "X_GLXVendorPrivate", opcodePrefix, opcodeName, str;
	    }
	} else {
	    printf "\t%s_BEGIN(%s%s,%s);\n", macroPrefix, opcodePrefix,\
		opcodeName, str;
	}
    }

    #
    # Write command header for protocol encoding.
    #
    if (genEncodeRender) {
        printf "    \\paramline{2}{%s}{rendering command length}\n", str;
        printf "    \\paramline{2}{%s}{rendering command opcode}\n", opcode;
    } else if (genEncodeSingle) {
        printf "    \\paramline{1}{\\glp{CARD8}}{opcode (X assigned)}\n";
        printf "    \\paramline{1}{%s}{GLX opcode}", opcode;
	# All singles have a fixed "in" command length, so we just divide by
	# 4 to get the length. Add 1 for the X header.
        printf "    \\paramline{2}{%s}{request length}\n", (str/4)+1;
        printf "    \\paramline{4}{\\glp{GLX\\_CONTEXT\\_TAG}}{context tag}\n", str;
    } else if (genEncodeVendpriv) {
        # Uses different protocol than Single commands - either
        # VendorPrivate or VendorPrivateWithReply.
        # XXX TO BE DONE
        printf "    \\paramline{1}{\\glp{CARD8}}{opcode (X assigned)}\n";
        printf "    \\paramline{1}{???}{VENDOR-PRIVATE OPCODE GOES HERE}\n", opcode;
        printf "    \\paramline{}{}{\\bfWarning: vendor-private Encode/Spec not done!}\n";
    }

    #
    # Write fixed size parameters.  These are scalars or fixed-size arrays.
    #
    if (genTransSingle || genTransVendpriv) {
	offset = 0;
    } else if (genTransDraw) {
	offset = 4;
    }
    for (j=1; j <= numInFixParams; j++) {

	i = sortedInFixParams[j];
	param = paramName[i];
	ttype = paramTransferType[param];

	if (isArray[i]) {
	    #
	    # Write fixed-size arrays.
	    #
	    if (arrayCount[i] <= 4) {
		#
	    	# For small arrays, just copy elements one at a time.
		#
		for (k=0; k < arrayCount[i]; k++) {
	    	    if (genTrans) {
			printf "\t%s_PUT_%s(%d,%s[%d]);\n", macroPrefix,
			    _cast[i], offset, param, k;
		    }
		    if (genSpecRender) {
			printf ".Op %s[%d] %s\n", param, k, _proto[i];
		    }
		    if (genSpecSingle || genSpecVendpriv) {
			printf ".Pa %s[%d] %s\n", param, k, _proto[i];
		    }
		    if (genEncode) {
                        printf "    \\paramline{%d}{\\glp{%s}}{%s[%d]}\n", \
                            _xfer_size[i], _proto[i], param, k;
		    }
		    offset += xfer_size[_wire[i]];
		}
	    } else {
	    	if (genTrans) {
		    printf "\t%s_PUT_%s_ARRAY(%d,%s,%s);\n", shortMacroPrefix,
			_cast[i], offset, param, arrayCount[i];
		}
		if (genSpecRender) {
		    printf ".Op %s LISTof%s\n", param, _proto[i];
		}
		if (genSpecSingle || genSpecVendpriv) {
		    printf ".Pa %s LISTof%s\n", param, _proto[i];
		}
		if (genEncode) {
                    printf "    \\paramline{%s}{\\glp{LISTof%s}}{%s}\n",
			_xfer_size[i]*arrayCount[i], _proto[i], param;
		}
		offset += xfer_size[_wire[i]] * arrayCount[i];
     	    }
	} else {
	    #
	    # Write scalars.
	    #
	    if (ttype == "reference") deref="*";
	    else deref="";
	    if (genTrans) {
		printf "\t%s_PUT_%s(%d,%s%s);\n", macroPrefix, _cast[i],
	           offset, deref, param;
	    }
	    if (genSpecRender) {
		printf ".Op %s %s\n", param, _proto[i];
	    }
	    if (genSpecSingle || genSpecVendpriv) {
		printf ".Pa %s %s\n", param, _proto[i];
	    }
	    if (genEncode) {
                printf "    \\paramline{%d}{\\glp{%s}}{%s}\n",
                    _xfer_size[i], _proto[i], param;
	    }
	    offset += xfer_size[_wire[i]];
	}
    }

    #
    # Pad scalars so that arrays will start on a 32-bit boundary.
    # But only need to push the PC here if another param follows, because
    # __GLX_END will reset it for the next command.
    #
    if (padInFixParams > 0) {
	offset += padInFixParams;
	if (genEncode) {
            printf "    \\paramline{%d}{}{unused}\n", padInFixParams;
	}
    }

    #
    # Write variable-sized arrays.
    #
    offsetString = offset;
    for (j=1; j <= numInVarParams; j++) {
	i = sortedInVarParams[j];
	param = paramName[i];
	if (genTrans) {
	    printf "\t%s_PUT_%s_ARRAY(%s,%s,%s);\n", shortMacroPrefix,
		_cast[i], offsetString, param, arrayCount[i];
	}
	if (genSpecRender) {
	    printf ".Op %s LISTof%s\n", param, _proto[i];
	}
	if (genSpecSingle || genSpecVendpriv) {
	    printf ".Pa %s LISTof%s\n", param, _proto[i];
	}
	if (genEncode) {
            printf "    \\paramline{%s}{\\glp{LISTof%s}}{%s}\n",
                arrayBytes[i], _proto[i], param;
	}
	offsetString = offsetString "+" arrayBytes[i];
    }

    if (genSpecRender) {
    	if (numInFixParams == 0 && numInVarParams == 0) {
        printf ".ti 0.5i\n";
    	    printf "\\fINo parameters.\\fP\n";
    	}
    }

    if (hasReturn || (numOutFixParams + numOutVarParams) > 0) {
    	if (genEncodeSingle || genEncodeVendpriv) {
            # printf "\\St\n";
            printf "\\replyarrow\n";
            printf "    \\paramline{1}{1}{Reply}\n";
            printf "    \\paramline{1}{}{unused}\n";
            printf "    \\paramline{2}{\\glp{CARD16}}{sequence number}\n";
	    read_return_data();
    	}
	if (genSpecSingle || genSpecVendpriv) {
	    printf ".Re\n";
	    read_return_data();
	}
    }

    #
    #  Finish writing the parameters; get the return data if needed.
    #
    if (genTransSingle || genTransVendpriv) {
	read_return_data();
	printf "\t%s_END();\n", macroPrefix;
    } else if (genTransDraw) {
	if (numInVarParams > 0) {
	    # The command size will be computed at runtime.
	    printf "\t%s_END(cmdlen);\n", macroPrefix;
	} else {
	    # The command size is a constant.
	    printf "\t%s_END(%s);\n", macroPrefix, cmdlen;
	}
    }

    #
    # Return a return value if needed.
    #
    if (hasReturn && genTrans) {
	printf "\treturn retval;\n";
    }

    if (genTrans) printf "}\n\n";
    if (genSpecRender) printf ".sp\n\n";
    if (genSpecSingle || genSpecVendpriv) printf ".Fe\n\n";
    if (genEncodeRender) printf "\\Ef\n";
    if (genEncodeSingle || genEncodeVendpriv) printf "\\Ef\n";

}

##############################################################################
#
# Compute symbolic reply length and pad size in units of 32 bit words.
# This is used in read_return_data() below.
#
##############################################################################
function build_reply_pad(xfer_size) {
    padSize = sprintf("n*%d", xfer_size);
    if (xfer_size == 1) {
        replyLength = sprintf("(n+p)/4");
        padSize = "n";
    } else if (xfer_size == 2) {
        replyLength = sprintf("(n*2+p)/4");
    } else if (xfer_size == 4) {
        replyLength = sprintf("n");
    } else if (xfer_size == 8) {
        replyLength = sprintf("n*2");
    } else {
        replyLength = sprintf("(n*%d+p)/4", xfer_size[i]);
    }
}

##############################################################################
#
# Read a reply packet and return the data back in the parameters.
#
##############################################################################
function read_return_data() {

    if (hasReturn || (numOutFixParams + numOutVarParams) > 0) {
        if (genTrans) {
	    printf "\t%s_READ_XREPLY();\n", macroPrefix;
	}
    } else {
        return;
    }

    #
    # Get the return value, if any.
    #
    if (hasReturn) {
	wireReturn = wiretype[returnUnmappedType];
	if (genTrans) {
	    printf "\t%s_GET_RETVAL(retval, GL%s);\n", macroPrefix,
	  	wiretype[returnUnmappedType];
	}
	#
	# Return types for the few routines that have them are 4 bytes large,
	# and the type must be defined in protoReturn.
	#
        # XXX Are return types different for Single and Vendpriv?
	if (genEncodeSingle || genEncodeVendpriv) {
	    if (!(wireReturn in protoReturn)) {
		specError("No protocol return type defined for " \
			wireReturn " in " functionName);
	    }
            # Only generate the return value here if there
            #   are no output parameters.
            if (!numOutFixParams && !numOutVarParams) {
		printf "    \\paramline{4}{0}{reply length}\n";
		printf "    \\paramline{4}{\\glp{%s}}{return value}\n",
		    protoReturn[wireReturn];
		printf "    \\paramline{20}{}{unused}\n";
	    }
        }
	if (genSpecSingle || genSpecVendpriv) {
	    if (!(wireReturn in protoReturn)) {
		specError("No protocol return type defined for " \
			wireReturn " in " functionName);
	    }
	    printf ".Pa return_value %s\n", protoReturn[wireReturn];
	}
    }

    #
    # Get computed size of a returned array, if any.
    #
    if (hasOutCompSizeParam) {
	if (genTrans) printf "\t%s_GET_SIZE(compsize);\n", macroPrefix;
    }
     
    #
    # Note that there is no skipping over padding, because so far gl.spec
    # contains only routines with either all fixed size return values, or one
    # variable-size returned array.
    #

    if (!numOutFixParams && !numOutVarParams) return;

    if (numOutCompSizeParams > 0) {
	i = sortedOutVarParams[1];
	param = paramName[i];
	if (genTrans) {
    	    printf "\tif (compsize == 1) {\n"; 
	    printf "\t    %s_GET_%s(%s);\n", macroPrefix, _cast[i], param;
	    printf "\t} else {\n";
	    printf "\t    %s_GET_%s_ARRAY(%s,%s);\n", macroPrefix, 
		_cast[i], param, arrayCount[i];
	    printf "\t}\n";
	}
	if (genEncodeSingle || genEncodeVendpriv) {
            build_reply_pad(_xfer_size[i]);
            printf "    \\paramline{4}{m}{reply length, m = (n==1 ? 0 : %s)}\n",
                replyLength;
            if (!hasReturn) {
		printf "    \\paramline{4}{}{unused}\n";
            } else {
                printf "    \\paramline{4}{\\glp{%s}}{return value}\n",
                    protoReturn[wireReturn];
            }
            printf "    \\paramline{4}{\\glp{CARD32}}{n}\n";
            printf "    \\\\ \n";
            printf "    \\Etext{if (n=1) this follows:}\n";
            printf "    \\\\ \n";
            printf "    \\paramline{%d}{\\glp{%s}}{%s}\n",
                _xfer_size[i], _proto[i], param;
            printf "    \\paramline{%d}{}{unused}\n\n",
                16 - _xfer_size[i];
            printf "    \\\\ \n";
            printf "    \\Etext{otherwise this follows:}\n";
            printf "    \\\\ \n";
            printf "    \\paramline{16}{}{unused}\n";
            printf "    \\paramline{n*%d}{\\glp{LISTof%s}}{%s}\n",
                _xfer_size[i], _proto[i], param;
	    if (_xfer_size[i] < 4) {
                printf "    \\paramline{p}{}{unused, p=pad(%s)}\n", padSize;
	    }
            printf "    \\\\ \n";
            printf "    \\Etext{%s}\n", \
 	       "Note that n may be zero, indicating that a GL error occurred.";
	}
	if (genSpecSingle || genSpecVendpriv) {
	    printf ".Pa %s LISTof%s\n", param, _proto[i];
	}

    } else {
	if (numOutFixParams > 0) {
	    i = sortedOutFixParams[1];
	    if (!isArray[i]) {
		#
		# There are no such functions in the GL API yet.
		#
		specError("Script is not ready for returned scalars:" \
			paramName[i] " in function " functionName);
	    }
	} else if (numOutVarParams > 0) {
	    i = sortedOutVarParams[1];
	}
	param = paramName[i];
	if (genTrans) {
	    printf "\t%s_GET_%s_ARRAY(%s,%s);\n", macroPrefix, _cast[i],
		param, arrayCount[i];
	}
	if (genEncodeSingle || genEncodeVendpriv) {
            # Can't compute reply len/pad numerically. If we could:
            #   pad = 3 - ((arrayCount[i]*_xfer_size[i] + 3) % 4);
            #   replyLength = arrayCount[i]*_xfer_size[i]+pad;

            build_reply_pad(_xfer_size[i]);

            printf "    \\paramline{4}{%s}{reply length}\n",
                replyLength;
            if (!hasReturn) {
		printf "    \\paramline{24}{}{unused}\n";
            } else {
                printf "    \\paramline{4}{\\glp{%s}}{return value}\n",
                    protoReturn[wireReturn];
                printf "    \\paramline{20}{}{unused}\n";
            }
            printf "    \\paramline{%s*%d}{\\glp{LISTof%s}}{%s}\n",
		arrayCount[i], _xfer_size[i], _proto[i], param;
            if (_xfer_size[i] < 4) {
                printf "    \\paramline{p}{}{unused, p=pad(%s)}\n", padSize;
	    }
	}
	if (genSpecSingle || genSpecVendpriv) {
	    printf ".Pa %s LISTof%s\n", param, _proto[i];
	}
    }
}


function finalize()
{
}
