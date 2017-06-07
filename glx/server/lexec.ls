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
#   Generate the list execution routines and the GLX server transport routines.
#
#   $Date$ $Revision$
#   $Header: //depot/main/glx/server/lexec.ls#6 $
#
##############################################################################

function initialize() {

    # TYPEMAP should be defined on the command line that invokes libspec
    typeMapFile = TYPEMAP;
    _readWireFile();

    # Functions that will be generated, should be defined on command line.
    funcs = FUNCS;

    # Whether we;re generating byte swapping code.
    genSwap = (SWAP == "yes");

    genListExec = (funcs ~ /listexec/);
    genTransDraw = (funcs ~ /transdraw/);
    genTransGet = (funcs ~ /transget/);
    genTransGLX = (funcs ~ /transglx/);
    genTransSingle = genTransGet || genTransGLX;

    #
    # Try to encapsulate the differences between different output here.
    #
    
    if (genListExec) {
	prefix = "__glle_";
	functionPrefix = "__glim_";
	opcodePrefix = "__glop_";
	macroPrefix = "__GL";
    } else if (genTransDraw) {
	if (genSwap) {
	    prefix = "__glXDispSwap_";
	} else {
	    prefix = "__glXDisp_";
	}
	macroPrefix = "__GLX";
	functionPrefix = "gl";
	opcodePrefix = "X_GLrop_";
	GLXheaders = true;
    } else if (genTransGet) {
	if (genSwap) {
	    prefix = "__glXDispSwap_";
	} else {
	    prefix = "__glXDisp_";
	}
	macroPrefix = "__GLX";
	functionPrefix = "gl";
	opcodePrefix = "X_GLgop_";
	GLXheaders = true;
    } else if (genTransGLX) {
	macroPrefix = "__GLX";
	prefix = "__glXDisp_glX";
	functionPrefix = "glX";
	opcodePrefix = "X_GL_";
	GLXheaders = false;
    }

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
    cast["void"] =      "VOID"

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
    # List of GL singles that are pendable (should really be taggeg in gl.spec.
    #
    pend["NewList"] = "True";
    pend["DeleteLists"] = "True";
    pend["FeedbackBuffer"] = "True";
    pend["SelectBuffer"] = "True";
    pend["PixelStoref"] = "True";
    pend["PixelStorei"] = "True";

    #
    # Include header files, declare globals.
    #
    if (genListExec) {
	printf "#include <GL/gl.h>\n";
	printf "#include \"context.h\"\n";
	printf "#include \"global.h\"\n";
	printf "#include \"list.h\"\n";
	printf "#include \"listcomp.h\"\n";
	printf "#include \"immed.h\"\n";
	printf "#include \"g_lexec.h\"\n";
	printf "\n";
    } else if (GLXheaders) {
	printf "#define NEED_REPLIES\n";
	printf "#include \"glxserver.h\"\n";
	printf "#include \"glxext.h\"\n";
	printf "#include <stdio.h>\n";
	printf "#include <string.h>\n";
	printf "#include \"g_disptab.h\"\n";
	printf "#include \"g_disptab_EXT.h\"\n";
	printf "#include \"unpack.h\"\n";
	printf "#include \"impsize.h\"\n";
	printf "#include \"singlesize.h\"\n";
	printf "\n";
    }
}

function main( i, param, cmdlen, AlreadyBeenSwapped) {

    #
    # Filter out unwanted routines.
    #
    if (genTransSingle) {
	if (!(("dlflags", "notlistable") in propListValues) ||
	    (("glxflags", "force-opcode") in propListValues)) return;
	if ((("glxflags", "server-handcode") in propListValues) ||
	    (("glxflags", "client-intercept") in propListValues) ||
	    (("glxflags", "ignore") in propListValues) )
	  return;
    } else if (genTransDraw) {
	if ((("dlflags", "notlistable") in propListValues) &&
	    !(("glxflags", "force-opcode") in propListValues)) return
	if (("glxflags", "server-handcode") in propListValues)
	  return;
    } else if (genListExec) {
	if (("dlflags", "notlistable") in propListValues) return;
	if (("dlflags", "handcode") in propListValues) return;
    }

    genTransVendpriv = ("glxvendorpriv" in propList);
    
    #
    # Routines with vector equivalents get mapped to them before being
    # compiled into a list or written into the transport buffer.
    #
    if ("vectorequiv" in propList) return;
    
    fname = functionName;

    nreturn = 0;

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
    computeOutCmdSize();
    computeInParamPos();
    
    #
    # Count number of returned values.
    #
    if (returnUnmappedType != "void") nreturn++;
    if (isOutCompSize) nreturn++;

    #
    # If the command has doubles, and is variable-size, we need to compute
    # the total command length; it will be used for alignment code.
    #
    needTotalCommandLength = false;
    for (i=1; i<=paramCount; i++) {
	if (_xfer_size[i] == 8 && isIn[i] && numInVarParams > 0) {
	    needTotalCommandLength = true;
	}
    }
    
    #
    # Function declaration.
    #
    if (genTransSingle) {
	printf "int %s%s(__GLXclientState *cl, GLbyte *pc)\n", prefix, fname
    } else if (genTransDraw) {
	printf "void %s%s(GLbyte *pc)\n", prefix, fname;
    } else if (genListExec) {
	printf "GLint %s%s(GLbyte *pc)\n", prefix, fname;
    }

    printf "{\n";

    #
    # Make empty stubs for list-nop routines.
    #
    if (genListExec && ("dlflags", "nop") in propListValues){
	printf "\t/* This routine is a no-op in a list. */\n", fname;
	printf "\treturn 0;\n";
	printf "}\n\n";
	return;
    }

    #
    # For parameters that are variable-sized arrays, declare some local
    # variables to compute the array sizes.  When a following array is
    # addressed, this size is used to skip over the previous array.
    # Using the locals for sizes makes the generated code a little easier to
    # read.
    #

    #
    # Declare the locals.
    #
    getParamsUsedAsSizes();
    numLocals = 0;

    #
    # These locals depend on parameters in the input buffer.  We can't
    # initialize them yet because the buffer hasn't been byteswapped yet.
    #
    if (needTotalCommandLength || genSwap || genTransSingle ||
	(numOutParamSizeParams > 1) || (numInParamSizeParams > 1)) {
	for (i in paramsUsedAsSizes) {
	    param = paramName[i];
	    dtype = paramDeclaredType[param];
	    printf "\t%s %s;\n", dtype, param;
	    numLocals++;
	}
    }
    if (needTotalCommandLength) {
	printf "\tGLint cmdlen;\n";
	numLocals++;
    }
    if ((hasInCompSizeParam && (needTotalCommandLength || genSwap)) || 
	hasOutCompSizeParam) {
	printf "\tGLint compsize;\n";
	numLocals++;
    }
    
    if (hasReturn) {
	printf "\tGL%s retval;\n", wiretype[returnUnmappedType];
	numLocals++;
    }
    if (genTransGLX) {
	printf "\tGLint status;\n";
	numLocals++;
    }
    if (genTransGet) {
	printf "\t__GLXcontext *cx;\n";
	printf "\tClientPtr client = cl->client;\n";
	printf "\tint error;\n";
	if (numOutFixParams > 0) {
	    i = sortedOutFixParams[1];
	    param = paramName[i]
	    dtype = paramDeclaredType[param]
	    printf "\t%s answerBuffer[200];\n", dtype;
	    printf "\tchar *answer;\n";
	}
	if (numOutVarParams > 0) {
	    i = sortedOutVarParams[1];
	    param = paramName[i]
	    dtype = paramDeclaredType[param]
	    printf "\t%s answerBuffer[200];\n", dtype;
	    printf "\tchar *answer;\n";
	}
	numLocals++;
    }
    if (genSwap) {
	printf "\t__GLX_DECLARE_SWAP_VARIABLES;\n";
	numLocals++;
    }

    infixcmdlen = 0;

    #
    # Some space for clarity.
    #
    if (numLocals > 0) printf "\n";

    #
    # Swap context argument.
    #
    if (genSwap) {
	if (genTransGet) {
	    if (genTransVendpriv) {
		printf "\t__GLX_SWAP_INT(&((xGLXVendorPrivateReq *)pc)->contextTag);\n";
	    } else {
		printf "\t__GLX_SWAP_INT(&((xGLXSingleReq *)pc)->contextTag);\n";
	    }
	}
    }
    
    #
    # Generate a check for a current context.
    #
    if (genTransGet) {
	if (genTransVendpriv) {
	    printf "\tcx = __glXForceCurrent(cl, __GLX_GET_VENDPRIV_CONTEXT_TAG(pc), &error);\n";
	} else {
	    printf "\tcx = __glXForceCurrent(cl, __GLX_GET_SINGLE_CONTEXT_TAG(pc), &error);\n";
	}
	printf "\tif (!cx) {\n";
	printf "\t\treturn error;\n";
	printf "\t}\n";
    }

    #
    # Skip over X header.
    #
    if (genTransGet && paramCount > 0) {
	if (genTransVendpriv) {
	    printf "\tpc += __GLX_VENDPRIV_HDR_SIZE;\n";
	} else {
	    printf "\tpc += __GLX_SINGLE_HDR_SIZE;\n";
	}
    }
    
    if (genSwap) {
    	#
	# Swap parameters used as sizes.
    	#
	for (i in paramsUsedAsSizes) {
	    if (_xfer_size[i] == 1) continue;
	    if (isIn[i] && !isArray[i]) {
		printf "\t__GLX_SWAP_%s(pc + %d);\n", _cast[i], paramPos[i];
		AlreadyBeenSwapped[i] = "Sure has";
	    }
	}
    }

    #
    # Now that the buffer has been (possibly) swapped, set the locals.
    #
    if (needTotalCommandLength || genSwap || genTransSingle ||
	(numOutParamSizeParams > 1) || (numInParamSizeParams > 1)) {
	for (i in paramsUsedAsSizes) {
	    param = paramName[i];
	    dtype = paramDeclaredType[param];
	    printf "\t%s = *(%s *)(pc + %d);\n", param, dtype, paramPos[i];
	}
    }
    if (hasInCompSizeParam && (needTotalCommandLength || genSwap)) {
	printf "\tcompsize = %s;\n", \
	  getCompSizeParamList(inCompSizeParamIndex);
    }
    if (hasOutCompSizeParam) {
	printf "\tcompsize = %s;\n", \
	  	getCompSizeParamList(outCompSizeParamIndex);
    }
    #
    # If the computed size is negative, it is because of a bad enumerant.
    # Just set it to zero and issue the call anyway, so the error code gets
    # set.
    #
    if ((hasInCompSizeParam && (needTotalCommandLength || genSwap)) ||
	hasOutCompSizeParam) {
	printf "\tif (compsize < 0) compsize = 0;\n";
    }

    # Compute cmdlen for variable length commands
    if (needTotalCommandLength) {
	printf "\tcmdlen = %s_PAD(%d%s);\n", macroPrefix, inFixCmdSize,
		inVarCmdSize;
    }

    # Generate alignment code
    if (genTransDraw || genTransGLX) {
	for (i=1; i<=paramCount; i++) {
	    param = paramName[i];
	    dtype = paramDeclaredType[param];
	    dir   = paramDirection[param];

	    if (_xfer_size[i] == 8 && isIn[i]) {
		printf "\n#ifdef __GLX_ALIGN64\n";
		printf "\tif ((unsigned long)(pc) & 7) {\n";
		if (numInVarParams > 0) {
			printf "\t    __GLX_MEM_COPY(pc-4, pc, cmdlen);\n";
		} else {
			printf "\t    __GLX_MEM_COPY(pc-4, pc, %d);\n",
				inFixCmdSize;
		}
		printf "\t    pc -= 4;\n";
		printf "\t}\n";
		printf "#endif\n";
		break;
	    }
        }
    }
    
    if (genSwap) {
    	#
    	# Swap each parameter in the buffer.
    	#
    	for (i=1; i<=paramCount; i++) {
	    param = paramName[i];
	    dtype = paramDeclaredType[param];
	    udtype = paramUnmappedDeclaredType[param];
	    ttype = paramTransferType[param];
	    dir   = paramDirection[param];

	    if (_xfer_size[i] == 1) continue;
	    
 	    #
	    # Incoming scalars.
	    #
	    if (isIn[i] && !isArray[i]) {
		if (AlreadyBeenSwapped[i] != "Sure has") {
		    printf "\t__GLX_SWAP_%s(pc + %d);\n", _cast[i], paramPos[i];
		}
	    }
	    #
	    # Incoming arrays.
	    #
	    if (isIn[i] && isArray[i]) {
	    	arcast = sprintf( "\(%-8s *\)", dtype);
		# Use the array straight out of the X request.
		printf "\t__GLX_SWAP_%s_ARRAY(pc + %s, %s);\n",
			_cast[i], paramPos[i], arrayCount[i];
	    }
	}
    }

    #
    # Some space for clarity.
    #
    if (numLocals > 0) printf "\n";
    
    if (genTransGLX) {
	printf "\tstatus =\n";
    }

    if (genTransGet) {
    	for (i=1; i<=paramCount; i++) {
	    if (!isIn[i] && isArray[i]) {
		printf "\t__GLX_GET_ANSWER_BUFFER(answer,cl,%s,%d);\n",
			arrayBytes[i], _xfer_size[i];
		break;
	    }
	}
    }

    #
    # Call the routine.
    #
    if (numOutCompSizeParams > 0) {
        printf "\t__glXClearErrorOccured();\n";
    }
    if (hasReturn) {
	printf "\tretval =\n";
    }
    printf "\t%s%s( \n", functionPrefix, fname;

    #
    # Locate each parameter in the buffer as we pass it to the routine.
    #
    for (i=1; i<=paramCount; i++) {
	param = paramName[i]
	dtype = paramDeclaredType[param]
	udtype = paramUnmappedDeclaredType[param]
	ttype = paramTransferType[param]
	dir   = paramDirection[param]

 	#
	# Incoming scalars.
	#
	if (isIn[i] && !isArray[i]) {
	    if (ttype == "value") ref = "*";
	    else ref = "";
	    printf "\t\t%s(%-8s *)(pc + %d)", ref, dtype, paramPos[i];
	}
	#
	# Incoming arrays.
	#
	if (isIn[i] && isArray[i]) {
	    arcast = sprintf( "\(%-8s *\)", dtype);
	    printf "\t\t%s(pc + %s)", arcast, paramPos[i];
	}
	
 	#
	# Outgoing scalars.
	#
	if (!isIn[i] && !isArray[i]) {
	    printf "\t\t(%-8s *)&__glXRetScalars[%d].%s", dtype, i, _cast[i];
	}
 	#
	# Outgoing arrays.
	#
	if (!isIn[i] && isArray[i]) {
	    printf "\t\t\(%-8s *\) answer", dtype;
	}

        # Be careful not to put comma after last parameter.
	if (paramCount > 1 && i != paramCount) printf ",";
	if (genTransGLX && paramCount > 0 && i == paramCount) printf ",";

	printf "\n";
    }

    #
    # GLX routines need an extra parameter, the client.
    #
    if (genTransGLX) {
	printf "\t\t";
	printf "client\n";
    }
    printf "\t);\n";

    if (genTransGet) {
        if (pend[fname] == "True") {
            printf "\t__GLX_NOTE_UNFLUSHED_CMDS(cx);\n";
        }
    }

    if (genSwap) {
    	#
    	# Swap each parameter in the buffer.
    	#
    	for (i=1; i<=paramCount; i++) {
	    param = paramName[i];
	    dtype = paramDeclaredType[param];
	    udtype = paramUnmappedDeclaredType[param];
	    ttype = paramTransferType[param];
	    dir   = paramDirection[param];

	    if (_xfer_size[i] == 1) continue;
 	    #
	    # Outgoing scalars.
	    #
	    if (!isIn[i] && !isArray[i]) {
		printf "\t__GLX_SWAP_%s(pc + %d);\n", _cast[i], paramPos[i];
	    }

	    # Outgoing arrays.
	    #
	    if (!isIn[i] && isArray[i]) {
	    	arcast = sprintf( "\(%-8s *\)", dtype);
	    	if ((genTransDraw || genTransGLX) && _xfer_size[i] == 8) {
		    # Use a locally declared, 8-byte aligned buffer.
		    printf "\t__GLX_SWAP_%s_ARRAY((GLbyte *)pc%d);\n", \
		      _cast[i], i, arrayCount[i];
	    	} else {
		    # Swap the array we've allocated.
		    printf "\t__GLX_SWAP_%s_ARRAY(answer, %s);\n", \
		      _cast[i], arrayCount[i];
	    	}
	    }
	}
    }

    # NOTE: dont' return the length, it's never used.
    #
    # Return the total length of the command.
    #
    # if (genTransDraw || genListExec) {
    #	if (numInVarParams > 0) {
    #	    printf "\treturn cmdlen;\n";
    #	} else {
    #	    printf "\treturn %s;\n", inFixCmdSize;
    #	}
    # }

    #
    # Send returned data if needed.
    #
    if (numOutFixParams > 0 || numOutVarParams > 1) {
	printf "ERROR script not able to deal with given returned params\n";
    }

    if (hasReturn || numOutFixParams > 0 || numOutVarParams > 0) {

	if (outFixCmdSize > 0) {
	    replyLength = outFixCmdSize;
	} else {
	    if (numOutVarParams > 0) {
		replyLength = outVarCmdSize;
		sub(/[+]/, "", replyLength);
	    } else {
		replyLength = "0";
	    }
	}

	if (hasReturn) {
	    printf "\t__GLX_PUT_RETVAL(retval);\n";
	    if (genSwap) {
	    	printf "\t__GLX_SWAP_REPLY_RETVAL();\n";
	    }
	}

	if (numOutCompSizeParams > 0) {
            #
            # error occurred; send back zero length data
            #
	    printf "\tif (__glXErrorOccured()) {\n"; 
	    printf "\t    __GLX_BEGIN_REPLY(0);\n";
	    if (genSwap) {
	    	printf "\t    __GLX_SWAP_REPLY_HEADER();\n";
	    }
	    printf "\t    __GLX_PUT_SIZE(0);\n";
	    printf "\t    __GLX_SEND_HEADER();\n";

            #
            # send just one value
            #
            printf "\t} else if (compsize == 1) {\n"; 
	    printf "\t    __GLX_BEGIN_REPLY(0);\n";
	    if (genSwap) {
	    	printf "\t    __GLX_SWAP_REPLY_HEADER();\n";
	    }
	    printf "\t    __GLX_PUT_SIZE(1);\n";
	    if (genSwap) {
	    	printf "\t    __GLX_SWAP_REPLY_SIZE();\n";
	    }
	    printf "\t    __GLX_PUT_%s();\n", _cast[sortedOutVarParams[1]];
	    printf "\t    __GLX_SEND_HEADER();\n";

            #
            # send back an array of values after the 32 byte reply
            #
            printf "\t} else {\n";
	    printf "\t    __GLX_BEGIN_REPLY(%s);\n", replyLength;
	    if (genSwap) {
	    	printf "\t    __GLX_SWAP_REPLY_HEADER();\n";
	    }
	    printf "\t    __GLX_PUT_SIZE(compsize);\n";
	    if (genSwap) {
	    	printf "\t    __GLX_SWAP_REPLY_SIZE();\n";
	    }
	    printf "\t    __GLX_SEND_HEADER();\n";
	    # 
	    # Write variable size params.
    	    #	
	    for (j=1; j <= numOutVarParams; j++) {
		i = sortedOutVarParams[j];
		printf "\t    __GLX_SEND_%s_ARRAY(%s);\n", _cast[i], \
		  arrayCount[i];
	    }
	    printf "\t}\n";

	} else {

	    printf "\t__GLX_BEGIN_REPLY(%s);\n", replyLength;
	    if (genSwap) {
	    	printf "\t__GLX_SWAP_REPLY_HEADER();\n";
	    }
	    printf "\t__GLX_SEND_HEADER();\n";
	    if (numOutFixParams > 0) {
		i = sortedOutFixParams[1];
	    	printf "\t__GLX_SEND_%s_ARRAY(%s);\n", _cast[i], arrayCount[i];
	    } else if (numOutVarParams > 0) {
		i = sortedOutVarParams[1];
	    	printf "\t__GLX_SEND_%s_ARRAY(%s);\n", _cast[i], arrayCount[i];
	    }
        }

    }

    if (genTransGet)
      printf "\treturn Success;\n";
    if (genTransGLX)
      printf "\treturn status;\n";

    printf "}\n\n";
}

function finalize()
{
}

