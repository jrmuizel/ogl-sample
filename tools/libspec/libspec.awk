##############################################################################
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
#  NAME
#	libspec.awk - specfile parsing awk script used by libspec
#
#  DESCRIPTION
#	This awk script parses the libspec format for each function interface,
#	generates values for a set of global variables, and then calls the
#	user program's function main() for generating output.  The main()
#	function gets the information it needs about the function from the
#	global variables.
#
#  AUTHOR
#	Dave Ciemiewicz (ciemo)
#	Gary Tarolli (gt)
#	David Mott (mott) - added checkparam and reqstates for gldebug 3/90
#
##############################################################################


##############################################################################
#
#  SECTION
#	Initialization
#
#  DESCRIPTION
#	Initialize useful constants.
#	Do the user's initialization and check it.
#	Initialize parsing.
#
#  SETS
#	true			-- Boolean value constant
#	false			-- Boolean value constant
#	stderr			-- For piping print[f] output to stderr
#	stripcomments		-- Strip commments from file for getline reads
#	validDirections		-- Set of valid directions
#	validTransferTypes	-- Set of valid transfer types
#
##############################################################################
BEGIN {
    true = 1
    false = 0

    validDirections["in"]
    validDirections["out"]
    validDirections["inout"]

    validTransferTypes["value"]
    validTransferTypes["reference"]
    validTransferTypes["array"]


    # 
    #	Special file methods
    #
    # Pipe to stderr on print or printf line
    stderr = "cat 1>&2"
    # sed script to remove comments from .map file
    stripcomments = "sed -e '/^[ \t]*\#/d' -e '/^[ \t]*$/d' -e 's/[ \t]*\#.*$//' "

    #
    #	Do user's initialization.
    #
    initialize()

    #
    #	Check if user initialized everything needed
    #
    if (typeMapArg != "") {
	typeMapFile = typeMapArg
    }
    if (typeMapPath != "") {
	typeMapFile = typeMapPath "/" typeMapFile
    }
    if (typeMapFile == "") {
	readError(libspecProgram,"no typeMapFile was specified in initialize()")
	exit 1
    }
    _readTypeMap(typeMapFile)
    _resetParseStates()
}

# END Initialization

##############################################################################
#
#  SECTION
#	Parser pattern-actions
#
#  DESCRIPTION
#	The following awk pattern-action statements match input lines from
#	the spec file and call the appropriate parsing functions to parse the
#	lines.  If the parsing function is trivial, then it is in-line coded
#
#  SPEC FILE BNF
#	<spec-file> :== <declaration-line>* <function-description>*
#	<declaration-line> :== <required-props-declaration> |
#				<valid-props-declaration>
#	<function-description> :== <function-prototype> <prop-lines>
#				   <end-of-function-description>
#
#	<return-prop-line> ::= <prop-indent> <return-prop>
#	<return-prop> ::= "return" <blank> <unmapped-return-type>
#
#	<param-prop-line> ::= <prop-indent> <param-prop>
#	<param-prop> ::= "param" <blank> <param-prop-body>
#
#	<prop-line> ::= <prop-indent> <prop-name> <meta-prop-value>*
#
##############################################################################
# NOTE: the following are ordered for optimal performance


##############################################################################
#
#  Parse comment lines and comments from spec file
#
#  SPEC FILE BNF
#	<comment-line> ::= <comment>
#	<comment> ::= <blank> "#" <text-upto-newline> 
#
##############################################################################
/^[ \t]*#/ {next}		# Do this so it is not treated as a blank line.
{ sub(/[ \t]*#.*$/, "") }	# Trim off trailing comment.  The sub forces
				# awk to recompute NF automatically.

##############################################################################
#
#  Parse return prop
#
#  SPEC FILE BNF
#	<return-prop-line> ::= <prop-indent> <return-prop>
#	<return-prop> ::= "return" <blank> <unmapped-return-type>
#
##############################################################################
($1 == "return") {
    _parseReturnType()
    _parsedReturn = true
    next
}


##############################################################################
#
#  Parse param prop
#
#  SPEC FILE BNF
#	<param-prop-line> ::= <prop-indent> <param-prop>
#	<param-prop> ::= "param" <blank> <param-prop-body>
#
##############################################################################
($1 == "param") {
    _parseParamPropBody()
    next
}


##############################################################################
#
#  Parse prop lines
#
#  SPEC FILE BNF
#	<prop-line> ::= <prop-indent> <prop-name> <meta-prop-value>*
#
##############################################################################
/^\t/	{
    _parseProp()
    delete _unparsedRequiredProps[$1]	# Check off required props when parsed
    next
}


##############################################################################
#
#  Parse function prototype
#
#  SPEC FILE BNF
#	<function-prototype> :== <function-name> "(" <param-list> ")"
#
##############################################################################
/^[^\t]*\(/ {
    _parseFunctionPrototype()
    _parsedPrototype = true

    # setup list to determine which props haven't yet been parsed for function
    for (i in requiredProps) {
	_unparsedRequiredProps[i]
    }
    next
}


##############################################################################
#
#  Parse end of function description
#
#  SPEC FILE BNF
#	<end-of-function-description> ::= ( <empty-line> | <end-of-file> )
#
##############################################################################
/^$/ {
    if (!_parsingErrors()) {
    	main()		# User's main routine
    }
    _resetParseStates()
    clearArray(propList)
    clearArray(propListValues)
    clearArray(paramName)
    clearArray(paramList)
    clearArray(paramUnmappedDeclaredType)
    clearArray(paramUnmappedDirection)
    clearArray(paramUnmappedTransferType)
    clearArray(paramDeclaredType)
    clearArray(paramDirection)
    clearArray(paramTransferType)
    clearArray(paramDimensions)
    clearArray(paramSubscripts)
    clearArray(paramOptions)

    # mott - begin additions for gldebug
    statesMustBeIn = ""
    statesMustNotBeIn = ""
    clearArray(checkparamProps)
    clearArray(reqstatesSeverity)
    clearArray(checkparamSeverity)
    clearArray(paramLowerBoundValue)
    clearArray(paramLowerBoundRange)
    clearArray(paramUpperBoundValue)
    clearArray(paramUpperBoundRange)

    clearArray(checkparamSetSize)
    clearArray(checkparamOneOfSet)
    clearArray(severityCountReqstates)
    clearArray(severityCountCheckparam)
    # mott - end

    next
}


##############################################################################
#
#  Parse required props declaration
#
#  SPEC FILE BNF
#	<required-props-declaration> ::= "required-props:"<required-prop-list>
#	<required-prop-list> ::= (<blank> <prop-name>)*
#
#  SETS
#	requiredProps[propName] -- Set of required props
#
##############################################################################
/^required-props:/ {
    for (i=2; i<=NF; i++) {
	requiredProps[$i]
    }
    next
}

##############################################################################
#
#  Parse valid props declarations
#
#  SPEC FILE BNF
#	<valid-props-declaration> ::= <prop-name> ":" <prop-values>
#
##############################################################################
/^[^ 	]*:/ {
    _parseValidPropsDeclaration()
    next
}


END {
    if (!_parsingErrors()) {
    	main()		# User's main routine
    }
    finalize()
}

# END Parser pattern-actions

##############################################################################
#
#  Parsing functions
#
##############################################################################


##############################################################################
#
#  NAME
#	_parseValidPropsDeclaration()
#
#  SPEC FILE BNF
#	<valid-props-declaration> ::= <prop-name> ":" <prop-values>
#	<prop-values> ::= (<blank> <prop-value>)*
#
#  SETS
#	validPropList[propname]	-- Set of valid props
#				-- ... [] is a comma separated list of
#				-- ... valid prop values
#	validPropListValues[propname,value]
#				-- Set of valid (propname,values)
#
#	_parseErrorOccured	-- Flag indicating error in parsing
#
##############################################################################
function _parseValidPropsDeclaration(	i) {
    gsub(/:/, "", $1)		# Get rid of colon from list name
    for (i=2; i<=NF; i++) {
	if (! (($1,$i) in validPropListValues)) {
	    validPropListValues[$1, $i]
	    validPropList[$1] = validPropList[$1] "," $i
	}
    }
}


##############################################################################
#
#  NAME
#	_parseFunctionPrototype()
#
#  SPEC FILE BNF
#	<function-prototype> :== <function-name> "(" <param-list> ")"
#	<param-list> :== [<param-name> ["," <param-name>]* ]
#
#  SETS
#	functionName		-- Name of function currently being processed
#	paramCount		-- The number of parameters for function
#	paramName[i]		-- The name of the i-th parameter
#	paramList[name] 	-- Set of parameter names for function
#				-- ... [] is the parameter index
#
#
##############################################################################
function _parseFunctionPrototype(	protoFields, protoNF, i) {
    protoNF = split($0, protoFields, "[(), \t]*")
    functionName = protoFields[1]

    paramCount = 0
    clearArray(paramList)
    for (i=2; i<protoNF; i++) {
	paramCount++
	paramName[paramCount] = protoFields[i]
	paramList[protoFields[i]] = paramCount
    }
}


##############################################################################
#
#  NAME
#	_parseReturnType()
#
#  DESCRIPTION
#	Parse the return type.  A "void" type implies a procedure or
#	subroutine.  Return type is the <unmapped-declared-type>.
#	The <unmapped-direction> is implied to be "out" and the
#	<unmapped-transfer-type> is implied to be "value".
#
#  SPEC FILE BNF
#	<unmapped-return-type> :== <unmapped-declared-type>
#
#  SETS
#	returnUnmappedType	-- Return type from spec file
#	returnType		-- Converted or mapped return type
#	_parseErrorOccured	-- Flag indicating error in parsing
#
##############################################################################
function _parseReturnType() {
    returnUnmappedType = $2
    if (! ((returnUnmappedType,"out","value") in typeMap)) {
	#  If returnUnmappedType is not a valid type ...
specError("return type " returnUnmappedType " is not defined in " typeMapFile)
	_parseErrorOccured = true
    } else {
	returnType = typeMapDeclaredType[returnUnmappedType, "out", "value"]
    }
}


##############################################################################
#
#  NAME
#	_parseParamPropBody()
#
#  SPEC FILE BNF
#	<param-prop-body> :== <param-name> <blank> <unmapped-param-type>
#				[<blank> <length-descriptor>]
#				[<param-options-list>]
#	<unmapped-param-type> :== <unmapped-declared-type> <blank>
#				<unmapped-direction> <blank>
#				<unmapped-transfer-type>
#	<unmapped-declared-type> :== <type-name>
#	<unmapped-direction> :== "in" | "out" | "in/out"
#	<unmapped-transfer-type> :== "array" | "reference" | "value"
#	<length-descriptor> :== "[" <index-expr> ["," <index-expr> ]* "]"
#	<index-expr> :== <integer> | <param-name> |
#			["("] <index-expr> <operator> <index-expr> [")"]
#	<operator> :== "+" | "-" | "*" | "/"
#	<param-options-list> :== (<blank> <list-prop-value>)*
#
#  SETS
#	paramUnmappedDeclaredType[name]
#				-- Parameter type from spec file
#	paramUnmappedDirection[name]
#				-- Parameter direction [in,out,in/out] from
#				-- ... spec file
#	paramUnmappedTransferType[name]
#				-- Parameter transfer type
#				-- ... [value, reference, array] from spec file
#	paramDeclaredType[name] -- Parameter type from spec file
#	paramDirection[name]	-- Parameter direction [in,out,in/out] from
#				-- ... spec file
#	paramTransferType[name]	-- Parameter transfer type
#				-- ... [value, reference, array] from spec file
#	paramDimensions[name]	-- The number dimensions of the array/length
#				-- ... 0 if no array/length descriptor
#	paramSubscripts[name,i] -- The size/value of the i-th dimension
#	paramOptions[name]	-- Space separated list of parameter options
#	paramOptions[name,option]
#				-- Set of options for parameter
#	_parseErrorOccured	-- Flag indicating error in parsing
#	
##############################################################################
function _parseParamPropBody(	name,tmpLengthDesc,tmpSubscripts,i,operand,operands,processedLengthDesc) {
    name = $2
    if (! (name in paramList)) {
        specError("param " name " not in function prototype for " functionName)
        _parseErrorOccured = true
    }

    if (! (($3,$4,$5) in typeMap)) {    # Is the type mapped?
        specError($3 " " $4 " " $5 " is not defined in " typeMapFile)
        _parseErrorOccured = true
    }
    paramUnmappedDeclaredType[name] = $3
    paramUnmappedDirection[name] =    $4
    paramUnmappedTransferType[name] = $5

    paramDeclaredType[name] = typeMapDeclaredType[$3, $4, $5]
    paramDirection[name] =    typeMapDirection[$3, $4, $5]
    paramTransferType[name] = typeMapTransferType[$3, $4, $5]

    #
    #	If the parameter has a length/array descriptor 
    #   Break it up into the following variables:
    #
    #
    paramDimensions[name] = 0
    if ($6 ~ /^\[.*\]$/) {
        tmpLengthDesc = $6
        gsub(/[\[\]]/, "", tmpLengthDesc)
        paramDimensions[name] = split(tmpLengthDesc, tmpSubscripts, ",")
	for (i=1; i <= paramDimensions[name]; i++) {
	    if (tmpSubscripts[i] != "") {
		paramSubscripts[name,i] = tmpSubscripts[i]
		if (match(tmpSubscripts[i],/MAX/)) {
    paramSubscripts[name,i] = paramSubscripts[name,i] "," tmpSubscripts[i+1]
		    i++
		    paramDimensions[name]--
		}
	    } else {
		specError("subscript " i " is unspecified")
		_parseErrorOccured = true
	    }
	}

	#
	#   Check operands in subscripts to make sure they are legal
	#
        operands = $6
        gsub(/[-+\/*(),\[\]]/, " ", operands)	# remove all but operands
        split(operands, operand)

        for (i in operand) {
	    # Not an arg or a number 
	    if (!(operand[i] in paramList) && !isConstant(operand[i]))
	    if (operand[i] != "MAX" && operand[i] != "COMPSIZE") {
	        # allow MAX and COMPSIZE keywords
                specError("subscript size operand " operand[i] " is not a parameter")
		_parseErrorOccured = true

            }
        }
	processedLengthDesc = true
    } else {
	processedLengthDesc = false
    }

    #
    #   Process any parameter options if there are any
    #
    if (!processedLengthDesc && NF > 5) {
	i = 6				# If no len descr, start at field 6
    } else if (processedLengthDesc && NF > 6) {
	i = 7				# If len descr, start at field 7
    } else {
	i = NF + 1			# No parameter options
    }

    for (; i <= NF; i++) {
	if (("param",$i) in validPropListValues) {
	    paramOptions[name,$i]
	    paramOptions[name] = paramOptions[name] $i " "
	} else {
	    specError("unknown prop 'param' value: " $i)
	}
    }
}

# ---- mott begin changes

##############################################################################
#
#  NAME
#	_severityIsValid()
#
#  SPEC FILE BNF
#	<err-severity> :== else ("WARNING" | "ERROR" | "FATAL")
#
#  SETS
#	_parseErrorOccured	-- Flag indicating error in parsing
#
#  AUTHOR
#	David Mott
#
##############################################################################
function _severityIsValid()
{
    _num = NF
    severity = $NF
    if (_num == 1) {
	specError("empty reqstates list")
	_parseErrorOccured = true
    }
    else if ((severity != "WARNING") &&
             (severity != "ERROR") &&
             (severity != "FATAL")) {
        specError("unknown reqstate severity '" $_num "'")
        _parseErrorOccured = true
    }
    else {
        _num--
        if ($_num != "else") {
            specError("reqstate missing keyword 'else' before '" $NF "'")
            _parseErrorOccured = true
        }
    }

    return (_parseErrorOccured != true)
}

##############################################################################
#
#  NAME
#	_parseMetaReqStateValue(value, severity)
#
#  DESCRIPTION
#	Add value to either the statesMustNotBeIn list, or the statesMustBeIn
#	list, depending whether value is preceded by '!' or not.
#	Value will only be added to one of these lists if there is no error.
#	value, severity is added to the reqstatesSeverity array.
#	
#	NOTE: ^ is the complement operator. e.g., ^rgbMode means that
#	GL cannot be in RGB mode.
#
#  SPEC FILE BNF
#	<meta-req-value> :== <blank> ["!" | "^"] ("all" | <prop-value>)
#	<err-severity> :== ("WARNING" | "ERROR" | "FATAL")
#
#  SETS
#	statesMustBeIn		-- Comma separated list of states GL must
#				-- ... be in to call this function
#	statesMustNotBeIn	-- Comma separated list of states GL must
#				-- ... NOT be in to call this function
#	reqstatesSeverity[]	-- Array of severity levels for each state
#
#  EXAMPLE
#	Here is an example:
#		reqstates	windowOpen ^rgbMode ^pickMode
#	this means a window must be open, and we cannot be in either
#	RGB or pick mode.
#	this sets:
#		statesMustBeIn = ",windowOpen"
#		statesMustNotBeIn = ",rgbMode,pickMode"
#
#	Another example:
#		reqstates	all !bgnEndMode
#	this means GL must be in all states listed in the top of gl.spec,
#	except bgnEnd mode. It CAN be in bgnEnd mode, it's just not required.
#	this sets:
#		statesMustBeIn = ",depthBufferMode,...,projectionMatrixMode"
#		statesMustNotBeIn = ""
#
#  AUTHOR
#	David Mott
#
##############################################################################
function _parseMetaReqStateValue(value, severity)
{
    name = "reqstates"

    #
    # NOTE: ^all is not allowed, although !all is allowed.
    #
    if (value ~ /^\^/) {	# if complement ^, add to NotBeIn list
	gsub(/^\^/, "", value)	# remove complement mark ^
	if ((name,value) in validPropListValues) {
	    statesMustNotBeIn = statesMustNotBeIn "," value
            reqstatesSeverity[value] = severity
	    severityCountReqstates[severity] += 1
	}
	else return(false)	# propagate error up one level
    }
    else if (value ~ /^!/) {	# if negation, remove from BeIn list
	gsub(/^!/, "", value)	# remove negation mark (!)
	if (value == "all") {
	    statesMustBeIn = ""
	    nf = split(validPropList[name], reqlist, ",")
	    for (i = 2; i <= nf; i++)
                reqstatesSeverity[reqlist[i]] = ""
	    severityCountReqstates[severity] = ""
	}
	else if ((name,value) in validPropListValues) {
	    gsub("," value, "", statesMustBeIn)   # remove ",value" from list
            reqstatesSeverity[value] = ""
	}
	else return(false)	# propagate error up one level

    } else {			# if not negation, add to list
	if (value == "all") {
	    statesMustBeIn = validPropList[name]
	    nf = split(validPropList[name], reqlist, ",")
	    for (i = 2; i <= nf; i++)
                reqstatesSeverity[reqlist[i]] = severity
	    severityCountReqstates[severity] = nf - 1
	}
	else if ((name,value) in validPropListValues) {
	    statesMustBeIn = statesMustBeIn "," value
            reqstatesSeverity[value] = severity
	    severityCountReqstates[severity] += 1
	}
	else return(false)	# propagate error up one level
    }

    return(true)
}

##############################################################################
#
#  NAME
#	_parseReqStates()
#
#  SPEC FILE BNF
#	reqstates ::= <prop-indent> <meta-req-value>* <prop-indent> <err-severity>
#	<meta-req-value> :== <blank> ["!" | "^"] ("all" | <prop-value>)
#	<err-severity> :== else ("WARNING" | "ERROR" | "FATAL")
#
#  SETS
#	statesMustBeIn		-- Comma separated list of states GL must
#				-- ... be in to call this function
#	statesMustNotBeIn	-- Comma separated list of states GL must
#				-- ... NOT be in to call this function
#	reqstatesSeverity[]	-- Array of severity levels for each state
#	_parseErrorOccured	-- Flag indicating error in parsing
#
#  AUTHOR
#	David Mott
#
##############################################################################
function _parseReqStates()
{
    if (_severityIsValid()) {
        # get those properties!
        for (ii = 2; ii <= (NF - 2); ii++) {
            if (!_parseMetaReqStateValue($ii, $NF)) {
                specError("unknown reqstate : " $ii)
                _parseErrorOccured = true
            }
        }
	# make sure no state is in both lists
	if ((statesMustBeIn != "") && (statesMustNotBeIn != "")) {
	    nf = split(statesMustNotBeIn, notlist, ",")
	    for (ii = 2; ii <= nf; ii++) {
	        if (index(statesMustBeIn, notlist[ii]) != 0) {
                    specError(notlist[ii] " and NOT(" notlist[ii] ") are both required")
                    _parseErrorOccured = true
	        }
	    }
	}
    }
}

##############################################################################
#
#  NAME
#	_parseMetaCheckParamValue(value, severity)
#
#  DESCRIPTION
#	If value is 'range', fill in the bound and range globals.
#	
#  SPEC FILE BNF
#	<meta-param-value> :== <blank> (<param-range>|<param-oneof>|<prop-value>)
#	<param-range> :== range ("[" | "(") <number> "," ("]" | ")")
#	<param-oneof> :== oneof "(" <set-member> <more-set-members> ")"
#	<more-set-members> :== "," <set-member>
#	<err-severity> :== else ("WARNING" | "ERROR" | "FATAL")
#
#  SETS
#	checkparamSeverity[]	-- Array of severity levels for each param,
#				-- ... prop-value pair.
#	paramLowerBoundValue[]	-- Lower bounds value of each param.
#	paramLowerBoundRange[]	-- Lower range, either "inclusive" or "exclusive"
#	paramUpperBoundValue[]	-- Upper bounds value of each param.
#	paramUpperBoundRange[]	-- Upper range, either "inclusive" or "exclusive"
#				-- ... for the above, "" means no bound.
#	checkparamSetSize[]	-- Array of severity levels for each state
#	checkparamOneOfSet[]	-- Param must be equal to one of this set
#	_parseErrorOccured	-- Flag indicating error in parsing
#
#  AUTHOR
#	David Mott
#
##############################################################################
function _parseRange(paramname, value)
{
    # lower bound range
    _range = substr(value, 6, 1)  # 6 is the position of the char after 'range'
    if (_range == "[")
	paramLowerBoundRange[paramname] = "inclusive"
    else if (_range == "(")
	paramLowerBoundRange[paramname] = "exclusive"
    else return (false)

    # lower bound value
    comma_pos = index(value, ",")
    len = comma_pos - 7  # 7 is the char after 'range(' or 'range['
    if (len == 0)
	paramLowerBoundValue[paramname] = ""  # no lower bound
    else
        paramLowerBoundValue[paramname] = substr(value, 7, len)

    # upper bound range
    lastchar_pos = length(value)
    _range = substr(value, lastchar_pos, 1)   # last char is either ']' or ')'
    if (_range == "]")
	paramUpperBoundRange[paramname] = "inclusive"
    else if (_range == ")")
	paramUpperBoundRange[paramname] = "exclusive"
    else return (false)

    # upper bound value
    len = lastchar_pos - (comma_pos + 1)
    if (len == 0)
	paramUpperBoundValue[paramname] = ""  # no upper bound
    else
        paramUpperBoundValue[paramname] = substr(value, (comma_pos + 1), len)

    return (true)
}

##############################################################################
#
#  NAME
#	_parseOneOf()
#
#  SPEC FILE BNF
#	<param-oneof> :== oneof "(" <set-member> <more-set-members> ")"
#	<more-set-members> :== "," <set-member>
#
#  SETS
#	checkparamSetSize[]	-- Array of severity levels for each state
#	checkparamOneOfSet[]	-- Param must be equal to one of this set
#
#  AUTHOR
#	David Mott
#
##############################################################################
function _parseOneOf(paramname, value)
{
    # get set members
    _startset = 6   # 6 is the char position just after 'oneof'
    _endset = length(value)

    # make sure the syntax is correct: oneof(...)
    _setparen = substr(value, _startset, 1)
    if (_setparen != "(")
	return (false)
    _setparen = substr(value, _endset, 1)
    if (_setparen != ")")
	return (false)

    # grab that set!
    len = _endset - (_startset + 1)
    if (len == 0)
	return (false)
    else {
        _set = substr(value, (_startset + 1), len)
	checkparamSetSize[paramname] = split(_set, _tempArray, ",")
	for (jj = 1; jj <= checkparamSetSize[paramname]; jj++) {
	    checkparamOneOfSet[paramname, jj] = _tempArray[jj]
        }
    }

    return (true)
}

##############################################################################
#
#  NAME
#	_parseMetaCheckParamValue()
#
#  SPEC FILE BNF
#	<meta-param-value> :== <blank> (<param-range>|<param-oneof>|<prop-value>)
#	<param-range> :== range ("[" | "(") <number> "," ("]" | ")")
#
#  SETS
#	checkparamSeverity[]	-- Array of severity levels for each param,
#				-- ... prop-value pair.
#	paramLowerBoundValue[]	-- Lower bounds value of each param.
#	paramLowerBoundRange[]	-- Lower range, either "inclusive" or "exclusive"
#	paramUpperBoundValue[]	-- Upper bounds value of each param.
#	paramUpperBoundRange[]	-- Upper range, either "inclusive" or "exclusive"
#				-- ... for the above, "" means no bound.
#	checkparamSetSize[]	-- Array of severity levels for each state
#	checkparamOneOfSet[]	-- Param must be equal to one of this set
#	checkparamProps[]	-- All the props for this param. range[...)
#				-- ... is written "range", oneof(...) is "oneof"
#
#  AUTHOR
#	David Mott
#
##############################################################################
function _parseMetaCheckParamValue(paramname, value, severity)
{
    name = "checkparam"

    if (value ~ /^range/) {
	if (_parseRange(paramname, value)) {
            checkparamSeverity[paramname, "range"] = severity
	    checkparamProps[paramname] = checkparamProps[paramname] "," "range"
	    severityCountCheckparam[severity] += 1
	}
	else return (false)	# propagate error up one level
    }
    else if (value ~ /^oneof/) {
	if (_parseOneOf(paramname, value)) {
            checkparamSeverity[paramname, "oneof"] = severity
	    checkparamProps[paramname] = checkparamProps[paramname] "," "oneof"
	    severityCountCheckparam[severity] += 1
	}
	else return (false)	# propagate error up one level
    }
    else if ((name,value) in validPropListValues) {
        checkparamSeverity[paramname, value] = severity
	checkparamProps[paramname] = checkparamProps[paramname] "," value
	severityCountCheckparam[severity] += 1
    }
    else return(false)	# propagate error up one level

    return(true)
}

##############################################################################
#
#  NAME
#	_parseCheckParam()
#
#  SPEC FILE BNF
#	checkparam ::= <prop-indent> <param-name> <checkparam-body>
#	<checkparam-body> :== <meta-param-value>* <blank> <err-severity>
#	<meta-param-value> :== <blank> (<param-range>|<param-oneof>|<prop-value>)
#	<param-range> :== range ("[" | "(") <number> "," ("]" | ")")
#	<param-oneof> :== oneof "(" <set-member> <more-set-members> ")"
#	<more-set-members> :== "," <set-member>
#	<err-severity> :== else ("WARNING" | "ERROR" | "FATAL")
#
#  SETS
#	checkparamSeverity[]	-- Array of severity levels for each param,
#				-- ... prop-value pair.
#	paramLowerBoundValue[]	-- Lower bounds value of each param.
#	paramLowerBoundRange[]	-- Lower range, either "inclusive" or "exclusive"
#	paramUpperBoundValue[]	-- Upper bounds value of each param.
#	paramUpperBoundRange[]	-- Upper range, either "inclusive" or "exclusive"
#				-- ... for the above, "" means no bound.
#	checkparamSetSize[]	-- Array of severity levels for each state
#	checkparamOneOfSet[]	-- Param must be equal to one of this set
#	_parseErrorOccured	-- Flag indicating error in parsing
#
#  NOTES
#	Here is an example:
#		checkparam x range[0.0,1.0) else ERROR
#	this means 0.0 <= x < 1.0
#	this sets:
#		paramLowerBoundValue["x"] = 0.0
#		paramLowerBoundRange["x"] = "inclusive"
#		paramUpperBoundValue["x"] = 1.0
#		paramUpperBoundRange["x"] = "exclusive"
#
#	Another example:
#		checkparam y oneof(MSINGLE,MVIEWING,MPROJECTION) else ERROR
#	this means y must be one of the members of the set listed.
#	this sets:
#		checkparamSetSize["y"] = 3
#		checkparamOneOfSet["y", 1] = "MSINGLE"
#		checkparamOneOfSet["y", 2] = "MVIEWING"
#		checkparamOneOfSet["y", 3] = "MPROJECTION"
#
#	One more example:
#		checkparam z nonnull nonsingular
#	these properties are not checked for explicitly, so they had better
#	be members of the valid props for checkparam at the top of gl.spec.
#
#  AUTHOR
#	David Mott
#
##############################################################################
function _parseCheckParam()
{
    if (_severityIsValid()) {
	# make sure the param we are checking exists for this function
        if (! ($2 in paramList)) {
            specError("param " $2 " not in function prototype for " functionName)
            _parseErrorOccured = true
        }

        # get those properties!
        for (i = 3; i <= (NF - 2); i++) {
            if (!_parseMetaCheckParamValue($2, $i, $NF)) {
                specError("unknown checkparam property : " $i)
                _parseErrorOccured = true
            }
        }
    }
}

# ---- mott end changes


##############################################################################
#
#  NAME
#	_parseProp()
#
#  SPEC FILE BNF
#	<prop-line> ::= <prop-indent> <prop-name> <meta-prop-value>*
#	<meta-prop-value> :== <blank> ["!"] ("all" | <prop-value>)
#
#  SETS
#	propList[propname]	-- Set of properties for this function
#				-- ... [] is a comma separated list of
#				-- ... prop values
#	_parseErrorOccured	-- Flag indicating error in parsing
#
##############################################################################
function _parseProp(	i) {

    # mott - begin additions for gldebug
    if ($1 == "checkparam")
	_parseCheckParam()
    else if ($1 == "reqstates")
	_parseReqStates()
    # mott - end

    else if ($1 in validPropList) {		# if its a valid prop
	propList[$1]				# create it for this function
	for (i=2; i<=NF; i++) {
	    if (!_parseMetaPropValue($1, $i)) {
		specError("unknown prop '" $1 "' value: " $i)
		_parseErrorOccured = true
	    }
	}
    } else {
	specError("unknown prop '" $1 "'")
	_parseErrorOccured = true
    }
}


##############################################################################
#
#  NAME
#	_parseMetaPropValue(name, value)
#
#  DESCRIPTION
#	Add name and value to propList array if name is a valid property
#	If value is equal to all, all of the values from validPropList[]
#	are added to propListValues.  If the value begins with an exclamation
#	point (!), the name,value pair is deleted from the propListValues.
#	This ! negation permits "all but 'value'" short-hand notation.
#
#  SPEC FILE BNF
#	<meta-prop-value> :== <blank> ["!"] ("all" | <prop-value>)
#
#  SETS
#	propList[propname]	-- Set of properties for this function
#				-- ... [] is a comma separated list of
#				-- ... prop values
#	propListValues[propname,value]
#				-- Set of (propname,values) for this function
#
#
##############################################################################
function _parseMetaPropValue(name, value,	i, nf, values) {
    if (value ~ /^!/) {		# if negation, remove from list
	gsub(/^!/, "", value)	# remove negation mark (!)
	if (value == "all") {
	    nf = split(validPropList[name], values, ",")
	    for (i=2; i<=nf; i++) {
		delete propListValues[name,values[i]]
		gsub("," values[i], "", propList[name])
	    }
	} else if ((name,value) in validPropListValues) {
	    delete propListValues[name,value]
	    gsub("," value, "", propList[name])
	} else if ((name,"*") in validPropListValues) {
	    delete propListValues[name,value]
	    gsub("," value, "", propList[name])
	} else {
	    return(false)	# propagate error up one level
	}
    } else {			# if not negation, add to list
	if (value == "all") {
	    propList[name] = propList[name] validPropList[name]
	    nf = split(validPropList[name], values, ",")
	    for (i=2; i<=nf; i++) {
		propListValues[name,values[i]]
	    }
	} else if ((name,value) in validPropListValues) {
	    propListValues[name,value]
	    propList[name] = propList[name] "," value
	} else if ((name,"*") in validPropListValues) {
	    propListValues[name,value]
	    propList[name] = propList[name] "," value
	} else {
	    return(false)	# propagate error up one level
	}
    }
    return(true)
}


##############################################################################
#
#  NAME
#	_parsingErrors()
#
#  DESCRIPTION
#	Check for parsing errors;
#	
##############################################################################
function _parsingErrors(		i) {
    if (_parsedPrototype) {
	if (! _parsedReturn) {
	    specError("return type never _parsed for " functionName)
	    _parseErrorOccured = true
	}
	for (i in _unparsedRequiredProps) {
	    specError("required prop '" i "' never _parsed for " functionName)
	    _parseErrorOccured = true
	}
	if (_parseErrorOccured) {
	    return true
	} else {
	    return false
	}
    } else {
	return true
    }
}

##############################################################################
#
#  NAME
#	_resetParseStates()
#
#  DESCRIPTION
#	Reset the parsing States to false.  The parsing routines will
#	indicate errors or successful completion by setting the appropriate
#	parsing flag(s) to true.
#
##############################################################################
function _resetParseStates() {
    _parseErrorOccured = false
    _parsedPrototype = false
    _parsedReturn = false
}


##############################################################################
#
#  NAME
#	clearArray(array)
#
#  DESCRIPTION
#	Removes all values from the specified array.
#
##############################################################################
function clearArray(array,	i) {
    for (i in array) {
	delete array[i]
    }
}


##############################################################################
#
#  NAME
#	inParamOptions(name,option) : returns true/false value
#
##############################################################################
function inParamOptions(name, option) {
    return (name, option) in paramOptions
}


##############################################################################
#
#  NAME
#	isConstant(string): returns true/false value
#
#  DESCRIPTION
#	Returns true if argument is a constant (decimal) value and is not
#	an identifier.
#
##############################################################################
function isConstant(string) {
    if (string ~ /^[0-9]+$/) {
	return(true)
    } else {
	return(false)
    }
}


# END Parsing functions

##############################################################################
#
#  Wire Functions
#
##############################################################################


##############################################################################
#
#  NAME
#	_readWireFile()
#
#  DESCRIPTION
#	Create the wire map by reading the specified wire file.  The
#	format of the file is simple: 2 columns, left column is the
#	DeclaredType of the parameter,the right column is the wire
#	format for the parameter.
#
##############################################################################
function _readWireFile(	) {
    WireFile = typeMapPath "/gl.wire"
    while (stripcomments WireFile | getline) {
	wiretype[$1] = $2
    }
    close(WireFile)
}

# END Wire functions


##############################################################################
#
#  Type Map Functions
#
##############################################################################


##############################################################################
#
#  NAME
#	_readTypeMap(typeMapFile)
#
#  DESCRIPTION
#	Create the type map by reading the specified typeMapFile
#	invoking the _makeTypeMap() function.
#
#	The type map uses a contextual wildcard character "*" for
#	short-hand description of type maps.  The expansion of "*"
#	is as follows:
#
#		Field			"*" Wildcard Expansion
#		====================	====================================
#		unmapped-direction 	All directions (in, out, in/out)
#		unmapped-transfer-type 	All transfer-types (array, reference,
#					... and value)
#		mapped-direction	Corresponding unmapped-direction
#		mapped-transfer-type	Corresponding unmapped-transfer-type
#
#  TYPE MAP FILE BNF
#	<type-map-file> ::= <type-map-line>*
#	<type-map-line> :== [<type-map>] [<comment>]
#	<type-map> :== <unmapped-type> "," <mapped-type>
#	<meta-unmapped-type> :== <unmapped-declared-type> ","
#			<meta-unmapped-direction> ","
#			<meta-unmapped-transfer-type>
#	<unmapped-declared-type> :== <type-name>
#	<meta-unmapped-direction> :== <direction> | "*"
#	<meta-unmapped-transfer-type> :== <transfer-type> | "*"
#	<meta-mapped-type> :== <meta-mapped-declared-type> ","
#			<meta-mapped-direction> ","
#			<meta-mapped-transfer-type>
#	<meta-mapped-declared-type> :== <type-name>
#	<meta-mapped-direction> :== <direction> | "*"
#	<meta-mapped-transfer-type> :== <transfer-type> | "*"
#	<direction> :== "in" | "out" | "in/out"
#	<transfer-type> :== "array" | "reference" | "value"
#	
##############################################################################
function _readTypeMap(typeMapFile,	d,t,OLDFS) {
    #
    #	Build type map from short hand
    #

    OLDFS = FS
    FS = "[ \t]*,[ \t]*"	# Map table uses comma separators
    if (typeMapFile == "") {
	readError("/usr/lib/libspec.awk", "no typeMapFile was specified in _readTypeMap()")
	exit 1
    }
    while (stripcomments typeMapFile | getline) {
	if ($2 == "*") {
	    for (d in validDirections) {
		if ($3 == "*") {
		    for (t in validTransferTypes) {
			_makeTypeMap($1, d, t, $4, $5, $6)
		    }
		} else if ($3 in validTransferTypes) {
		    _makeTypeMap($1, d, $3, $4, $5, $6)
		} else {
		    readError(typeMapFile, "unknown transfer type: " $5)
		}
	    }
	} else if ($2 in validDirections) {
	    if ($3 == "*") {
	        for (t in validTransferTypes) {
	 	    _makeTypeMap($1, $2, t, $4, $5, $6)
		}
	    } else if ($3 in validTransferTypes) {
		_makeTypeMap($1, $2, $3, $4, $5, $6)
	    } else {
		readError(typeMapFile, "unknown transfer type: " $5)
	    }
	} else {
	    readError(typeMapFile, "unknown transfer type: " $5)
	}
    }
    FS = OLDFS			# Restore to use white space for spec
    close(typeMapFile)
}


##############################################################################
#
#  NAME
#	_makeTypeMap(type, direction, transfer, maptype, mapdir, maptrans)
#
#  SETS
#	typeMap[type,direction,transfer]
#				-- Set of raw spec file types which are mapped
#	typeMapDeclaredType[type,direction,transfer]
#				-- Mapped declared type indexed by raw types
#	typeMapDirection[type,direction,transfer]
#				-- Mapped direction indexed by raw types
#	typeMapTransferType[type,direction,transfer]
#				-- Mapped transfer type indexed by raw types
##############################################################################
function _makeTypeMap(type, direction, transfer, maptype, mapdir, maptrans) {
    typeMap[type,direction,transfer]

    if (maptype == "*") {
        typeMapDeclaredType[type,direction,transfer] = type
    } else {
        typeMapDeclaredType[type,direction,transfer] = maptype
    }

    if (mapdir == "*") {
        typeMapDirection[type,direction,transfer] = direction
    } else if (mapdir in validDirections) {
        typeMapDirection[type,direction,transfer] = mapdir
    } else {
        error("unknown direction: " mapdir)
    }

    if (maptrans == "*") {
        typeMapTransferType[type,direction,transfer] = transfer
    } else if (maptrans in validTransferTypes) {
        typeMapTransferType[type,direction,transfer] = maptrans
    } else {
        error("unknown transfer type: " maptrans)
    }
}


##############################################################################
#
#  NAME
#	_dumpTypeMap()
#
#  DESCRIPTION
#	Diagnostic/debugging function for printing the type map to stderr.
#
##############################################################################
function _dumpTypeMap(	i) {
    print "\nDump of type Map" | stderr
    printf "%-32s -> %s\n", "***** Input Type *****", \
		"***** Output Type *****" | stderr
    for(i in typeMapDeclaredType) {
	split(i, foo, SUBSEP)
	printf "%-32s", foo[1] " " foo[2] " " foo[3] | stderr
	print typeMapDeclaredType[i], typeMapDirection[i], \
		typeMapTransferType[i] | stderr
    }
    print "" | stderr
}

# END Type Map functions

##############################################################################
#
#	Error message routines
#
##############################################################################

##############################################################################
#
#  NAME
#	specError(message)
#
#  DECRIPTION
#	Print an error message for the spec file being read.
#
##############################################################################
function specError(message) {
    readError(FILENAME, FNR, message)
}

##############################################################################
#
#  NAME
#	readError(message)
#	readError(filename, message)
#	readError(filename, linenumber, message)
#
#  DESCRIPTION
#	Print a read error message for the file/linenumber specified.
#
##############################################################################
function readError(arg1, arg2, arg3) {
    if (arg2 == "") {
	error(arg1)
	return
    }
    if (arg3 == "") {
	error("file " arg1 ": " arg2)
	return
    }
    error("file " arg1 ", line " arg2 ": " arg3)
}

##############################################################################
#
#  NAME
#	error(message)
#
#  DESCRIPTION
#	Print an error message.
#
##############################################################################
function error(message) {
    print "ERROR: " message "." | stderr
    if ($0 != "") {
	print ">>>>" $0 "<<<<"| stderr
    } else {
	print ">>>> CHECK LINES ABOVE LINE INDICATED <<<<"| stderr
    }
}

# END Error routines
