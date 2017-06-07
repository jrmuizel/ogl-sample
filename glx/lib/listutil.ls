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

##############################################################################
#
#  These functions are common to the scripts that generate list compilation
#  code and list execution code.  By and large they set variable that must be
#  global, because nawk does not allow scalars to be passed by reference.
#
##############################################################################


##############################################################################
#
# Compute the size of a command.
#
# Variables modified:
#	_alen[]		 size, in bytes, of each array
#	_acount[]	 size, in element count, of each array
#	invarcmdlen	 number of bytes in the variable length portion of cmd
#	infixcmdlen	 number of bytes in the fixed length portion of cmd
#	outvarcmdlen	 number of bytes in the variable length portion of cmd
#	outfixcmdlen	 number of bytes in the fixed length portion of cmd
#	padInScalars	 Boolean that says whether scalars need to be padded
#	padInScalarsSize number of bytes use to pad scalars
#
##############################################################################

##############################################################################
#
# Sort fixed-size parameters by the size of their elements, to avoid having
# to pad each individual parameter to a word-aligned boundary.
#
# Variables modified:
#	sortedInFixParams[]		sorted list of fixed size parameters
#
##############################################################################
function sortParams(s,i,count)
{
    # Sort params from largest elements to smallest.

    sortSizes[1] = 8;
    sortSizes[2] = 4;
    sortSizes[3] = 2;
    sortSizes[4] = 1;

    #
    # Sort fixed size params (scalars and fixed size arrays).
    #
    count = 1;
    for (s=1; s<=4; s++) {
	for (i=1; i<=paramCount; i++) {
	    param = paramName[i];
	    if (!isIn[i]) continue;
	    if (isArray[i] && !sizeIsConstant[i]) continue;
	    if (_xfer_size[i] == sortSizes[s]) {
		sortedInFixParams[count] = i;
		count++;
	    }
	}
    }

    #
    # Sort variable size params.
    #
    count = 1;
    for (s=1; s<=4; s++) {
	for (i=1; i<=paramCount; i++) {
	    param = paramName[i];
	    if (!isIn[i]) continue;
	    if (!isArray[i] || sizeIsConstant[i]) continue;
	    if (_xfer_size[i] == sortSizes[s]) {
		sortedInVarParams[count] = i;
		count++;
	    }
	}
    }

    #
    # Sort fixed size params.
    #
    count = 1;
    for (s=1; s<=4; s++) {
	for (i=1; i<=paramCount; i++) {
	    param = paramName[i];
	    if (isIn[i]) continue;
	    if (isArray[i] && !sizeIsConstant[i]) continue;
	    if (_xfer_size[i] == sortSizes[s]) {
		sortedOutFixParams[count] = i;
		count++;
	    }
	}
    }

    #
    # Sort variable size params.
    #
    count = 1;
    for (s=1; s<=4; s++) {
	for (i=1; i<=paramCount; i++) {
	    param = paramName[i];
	    if (isIn[i]) continue;
	    if (!isArray[i] || sizeIsConstant[i]) continue;
	    if (_xfer_size[i] == sortSizes[s]) {
		sortedOutVarParams[count] = i;
		count++;
	    }
	}
    }
}

##############################################################################
#
# Set some flags for each parameter so they can be conveniently used in the
# rest of the script.
#
# Variables modified:
#	_varsize[]	Boolean: whether each parameter is variable sized
#	invarsize	Boolean: whether the entire incoming command is
#				variable sized
#	outvarsize	Boolean: whether the entire outgoing command is
#				variable sized
#	isarray[]	Boolean: whether each parameter is an array
#	isInCompSize	Boolean: whether this call contains an "in" compsize
#				array
#	isOutCompSize	Boolean: whether this call contains an "out" compsize
#				array
#	isCompSize[]	Boolean: whether this variable sized array is compsize
#	compSizeIndex[] index of the compsized array
#	compSizeParam	string name of the compsized array
#	_cast[]		string name of macro used to write each parameter
#	_wire[]		wire type of each parameter
#	_xfer_size[]	transfer size, in bytes, of each parameter
#	nscalars	number of scalar params
#	narray		number of array params
#	nreturn		number of returned params excl. return value
#
##############################################################################
function setPerParamFlags(i,len1,len2)
{
    invarsize = false;	# true if a variable sized command
    outvarsize = false; # true if a variable sized command
    isInCompSize = false;
    isOutCompSize = false;

    numInFixParams = 0;
    numInVarParams = 0;
    numInCompSizeParams = 0;
    numInParamSizeParams = 0;
    numOutFixParams = 0;
    numOutVarParams = 0;
    numOutCompSizeParams = 0;
    numOutParamSizeParams = 0;

    for (i in inParamSizeParamIndices) {
	delete inParamSizeParamIndices[i];
    }
    for (i=1; i<=paramCount; i++) {
	param = paramName[i];
	dtype = paramDeclaredType[param];
	ttype = paramTransferType[param];
	udtype = paramUnmappedDeclaredType[param];
	dir   = paramDirection[param];
	dim = paramDimensions[param];

	paramIndex[param] = i;

	# Get the wiretype.
	if (udtype in wiretype) {
	    _wire[i] = wiretype[udtype]
	} else {
	    specError("wiretype[] has no map for " udtype)
	    exit 1
	}

	# Get the cast string.
	if (_wire[i] in cast) {
	    _cast[i] = cast[_wire[i]];
	} else {
	    specError("cast[] has no map for " udtype)
	    exit 1
	}

	# Get the protocol type string.
	if (_wire[i] in cast) {
	    _proto[i] = proto[_wire[i]];
	} else {
	    specError("proto[] has no map for " udtype)
	    exit 1
	}

	# Get the transfer size.
	if (_wire[i] in xfer_size) {
	    _xfer_size[i] = xfer_size[_wire[i]];
	} else {
	    specError("xfer_size[] has no map for " udtype)
	    exit 1
	}

	#
	# Set some flags that categorize the param as one of 5 types.
	#
	sizeIsConstant[i] = false;
	sizeIsComputed[i] = false;
	isArray[i] = (ttype == "array");
	if (isArray[i]) {
	    if (dim > 1) {
		specError("only 1D arrays are supported so far");
	    }
	    len = paramSubscripts[param,1];
	    sizeIsConstant[i] = isConstant(len);
	    if (sizeIsConstant[i]) {
		acount[i] = sprintf ("%d", len);
	    } else {
		sizeIsComputed[i] = (len ~ /COMPSIZE/);
		if (sizeIsComputed[i]) {
		    acount[i] = "compsize";
		} else {
		    acount[i] = len;
		}
	    }
	}

	#
	# Count the number of parameters in each direction.
	#
	isIn[i] = (dir == "in");
	if (isIn[i]) {
	    if (isArray[i]) {
		if (sizeIsConstant[i]) {
		    numInFixParams++;
		} else {
		    if (sizeIsComputed[i]) {
			numInCompSizeParams++;
			inCompSizeParamIndex = i;
		    } else {
			numInParamSizeParams++;
			inParamSizeParamIndices[numInParamSizeParams] = i;
		    }
		    numInVarParams++;
		}
	    } else {
		numInFixParams++;
	    }
	} else {
	    if (isArray[i]) {
		if (sizeIsConstant[i]) {
		    numOutFixParams++;
		} else {
		    if (sizeIsComputed[i]) {
			numOutCompSizeParams++;
			outCompSizeParamIndex = i;
		    } else {
			numOutParamSizeParams++;
			outParamSizeParamIndices[numOutParamSizeParams] = i;
		    }
		    numOutVarParams++;
		}
	    } else {
		numOutFixParams++;
	    }
	}
    }

    if (numInCompSizeParams > 1 || numOutCompSizeParams > 1) {
	specError("more than 1 COMPSIZE parameter");
    }
    hasInCompSizeParam = (numInCompSizeParams == 1);
    hasOutCompSizeParam = (numOutCompSizeParams == 1);
    hasInParamSizeParam = (numInParamSizeParams == 1);
    hasOutParamSizeParam = (numOutParamSizeParams == 1);

    hasReturn = (returnUnmappedType != "void");
}

function computeInParamPos()
{
    pos = 0;
    for (j=1; j <= numInFixParams; j++) {
	i = sortedInFixParams[j];
	paramPos[i] = pos;
	if (isArray[i]) {
	    #
	    # Fixed-size array.
	    #
	    pos += _xfer_size[i] * arrayCount[i];
	} else {
	    #
	    # Scalar.
	    #
	    pos += _xfer_size[i];
	}
    }

    pos = sprintf ("%d", inFixCmdSize);
    for (j=1; j <= numInVarParams; j++) {
	i = sortedInVarParams[j];
	paramPos[i] = pos;
	pos = sprintf ("%s+%s*%d", pos, arrayCount[i], _xfer_size[i]);
    }
}

function computeInCmdSize(i) {
    inFixCmdSize = 0;
    inVarCmdSize = "";
    inVarParamsNeedPad = false;

    for (i=1; i <= paramCount; i++) {
	param = paramName[i];
	dir   = paramDirection[param];
	dim = paramDimensions[param];

	if (!isIn[i]) continue;

	if (isArray[i]) {
	    if (sizeIsConstant[i]) {
		arrayBytes[i] = paramSubscripts[param,1] * _xfer_size[i];
		arrayCount[i] = paramSubscripts[param,1];
		inFixCmdSize += _xfer_size[i] * arrayCount[i];
	    } else {
		if (sizeIsComputed[i]) {
		    numInCompSizeParams++;
		    len = "compsize";
		} else {
		    len = paramSubscripts[param,1];
		}
		if (_xfer_size[i] == 1) {
		    #
		    # Do not generate a multiplication by one
		    #
		    arrayBytes[i] = sprintf ("%s", len);
		} else {
		    arrayBytes[i] = sprintf ("%s*%s", len, _xfer_size[i]);
		    if ((_xfer_size[i] % 4) != 0) {
			#
			# Padding needed only if we're a not a multiple of 4
			#
			inVarParamsNeedPad = true;
		    }
		}
		arrayCount[i] = len;
		inVarCmdSize = inVarCmdSize "+" arrayBytes[i];
	    }
	} else {
	    inFixCmdSize += _xfer_size[i];
	}
    }

    # Pad scalars to 32 bits.
    if (inFixCmdSize % 4) {
	padInFixParams = (4 - (inFixCmdSize % 4));
	inFixCmdSize += padInFixParams;
    } else {
	padInFixParams = 0;
    }
}


function computeOutCmdSize(i) {
    outFixCmdSize = 0;
    outVarCmdSize = "";

    for (i=1; i <= paramCount; i++) {
	param = paramName[i];
	dir   = paramDirection[param];
	dim = paramDimensions[param];

	if (isIn[i]) continue;

	if (isArray[i]) {
	    if (sizeIsConstant[i]) {
		len = paramSubscripts[param,1];
		arrayBytes[i] = len * _xfer_size[i];
		arrayCount[i] = len;
		outFixCmdSize += arrayBytes[i];
	    } else {
		if (sizeIsComputed[i]) {
		    len = "compsize";
		} else {
		    len = paramSubscripts[param,1];
		}
		if (_xfer_size[i] == 1) {
		#
		# Do not generate a multiplication by one
		#
		    arrayBytes[i] = sprintf ("%s", len);
		} else if (sizeIsConstant[i]) {
		    arrayBytes[i] = sprintf ("%d", len * _xfer_size[i]);
		} else {
		    arrayBytes[i] = sprintf ("%s*%s", len, _xfer_size[i]);
		}
		arrayCount[i] = len;
		outVarCmdSize = outVarCmdSize "+" arrayBytes[i];
	    }
	} else {
	    outFixCmdSize += _xfer_size[i];
	}
    }

    # Pad scalars to 32 bits.
    if (outFixCmdSize % 4) {
	padOutFixParams = (4 - (outFixCmdSize % 4));
	outFixCmdSize += padOutFixParams;
    } else {
	padOutFixParams = 0;
    }
}

function getCompSizeParamList(paramIndex)
{
    len = paramSubscripts[paramName[paramIndex],1];

    if (!(len ~ /COMPSIZE/)) {
	printf "paramIndex:%d\n", paramIndex;
	specError("getCompSizeParamList: not a compsize param");
    }

    # strip away everything but the params
    tmp = len;
    gsub(/[-+\/*(),\[\]]/, " ", tmp);
    sub( "COMPSIZE", "", tmp);
    gsub( " ", ",", tmp);
    plist = "__gl" functionName "_size(" tmp ")";
    gsub( /\(,/, "(", plist);
    gsub( /,\)/, ")", plist);

    return plist;
}

function getParamsUsedAsSizes(tmp,len,i)
{
    for (i in paramsUsedAsSizes) {
	delete paramsUsedAsSizes[i];
    }
    for (i=1; i <= paramCount; i++) {
	if (!isArray[i]) continue;
	if (sizeIsConstant[i]) continue;

	len = paramSubscripts[paramName[i],1];
	if (sizeIsComputed[i]) {
	    #
	    # Extract the parameter names out of the expression.
	    #
	    gsub(/[-+\/*(),\[\]]/, " ", len);
	    sub( "COMPSIZE", "", len);
	    split(len,tmp," ");
	    for (a in tmp) {
		paramsUsedAsSizes[paramIndex[tmp[a]]];
	    }
	} else {
	    #
	    # Extract the parameter names out of the expression.
	    #
	    gsub(/[-+\/*(),\[\]]/, " ", len);
	    split(len,tmp," ");
	    for (a in tmp) {
		if (!isConstant(tmp[a])) {
		    paramsUsedAsSizes[paramIndex[tmp[a]]];
		}
	    }
	}
    }
}
