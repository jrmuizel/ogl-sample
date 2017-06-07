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
#	c2fh - convert simple C header files to Fortran header files
#
#  SYNOPSIS
#	c2fh <map-file> <header-file>
#
#  DESCRIPTION
#	c2fh will convert a very simple C header file to a Fortran style
#	header file for use in Fortran programs.  The C header file should
#	not do any type definitions.  Specifically, c2fh will convert
#	cpp style "#define" constants to Fortran "PARAMETER" definitions.
#
#	This script checks the generated names to make sure they do not
#	conflict with any other generated names.  If they do conflict,
#	an error diagnostic is issued.	If the names do conflict, the
#	developer must either:
#
#		(1) Change any or all of the conflicting names so they
#		    are unique as per the algorithm above.
#
#		(2) Any or all of the conflicting names should have
#		    maps created for the names in the <map-file> so
#		    the C names map to non-conflicting Fortran names.
#
#	The <map-file> provides the mechanism for mapping C #define
#	names and #define values to the appropriate Fortran versions.
#	The <map-file> must have the extension (suffix) ".map" so it
#	can be identified as the <map-file>.  The <map-file> has the
#	Following format.
#
#		name
#			<C-#define-name>	<Fortran-PARAMETER-name>
#			. . .
#		value
#			<C-#define-name>	<Fortran-PARAMETER-value>
#			. . .
#
#	The "name" section is used to define the name maps.  In some cases,
#	the C #define name exands to an expression.  Since c2fh is a simple
#	script and cannot parse nor expand the expression correctly,
#	the "value" section provides a way of mapping the expression of
#	a C #define name to a valid Fortran expression.
#
#	If a #define is C specific and should not be defined in the Fortran
#	version of the include file, the name map can map the name to
#	"**NOTUSED**".	This will prevent the definition from appearing
#	in the resulting file.
#
#	Since most Fortran file inclusion mechanisms including "$INCLUDE"
#	do not understand C preprocessor directives like #ifdef, these are
#	simply removed.
#
#	cpp style comments "/* ... */" which are at the beginning of a line
#	are converted to Fortran "C ... " comments.  cpp style comments
#	appearing anywhere else in the file to be converted are just removed
#	since Fortran has no notion of in-line comments.
#
#	C hexadecimal constants are converted to decimal constants to work
#	with ANSI-standard FortranS.
#
#  AUTHOR
#	Dave Ciemiewicz (ciemo@sgi)

#  BUGS
#	Instead of stripping cpp directives, maybe "unifdef" should be used
#	to removed them incase there is C specific stuff which should not
#	be included in Fortran specific headers.
#
#  $Date$ $Revision$
#  $Header$
#

BEGIN {
    true = 1
    false = 0
    stderr = "cat 1>&2"

    hex2dec["0"] = 0
    hex2dec["1"] = 1
    hex2dec["2"] = 2
    hex2dec["3"] = 3
    hex2dec["4"] = 4
    hex2dec["5"] = 5
    hex2dec["6"] = 6
    hex2dec["7"] = 7
    hex2dec["8"] = 8
    hex2dec["9"] = 9
    hex2dec["A"] = 10
    hex2dec["a"] = 10
    hex2dec["B"] = 11
    hex2dec["b"] = 11
    hex2dec["C"] = 12
    hex2dec["c"] = 12
    hex2dec["D"] = 13
    hex2dec["d"] = 13
    hex2dec["E"] = 14
    hex2dec["e"] = 14
    hex2dec["F"] = 15
    hex2dec["f"] = 15
}

#
#  Read map file entries
#
(FILENAME ~ /.map$/) && ($1 ~ /^name$/) {
    mapnames = true
    mapvalues = false
    next
}

(FILENAME ~ /.map$/) && ($1 ~ /^value$/) {
    mapnames = false
    mapvalues = true
    next
}

(FILENAME ~ /.map$/) {
    mapfilename = FILENAME
    gsub(/\t+/, "\t")
    split($0, names, "\t")

    if (mapnames) {
	mapname[names[2]] = names[3]
    }
    else if (mapvalues) {
	mapvalue[names[2]] = names[3]
    }
    else {
	print "Error: file", FILENAME ", line", FNR ", no mapping type" | stderr
	print "Error: (either \"name\" or \"value\") has been specified." | stderr
    }
    next
}

#
#  Remove cpp macros
#
($1 ~ /#define/) && ($2 ~ /\(/)     {
    print "Warning: removed", $0 | stderr
    while ($NF ~ /\\$/) {
	getline
	print "Warning: removed", $0 | stderr
    }
    next
}

#
#  Convert #define constants to Fortran PARAMETERs
#
($1 ~ /#define/) && ($2 !~ /\(/) && ($3 != "")	    {
    $1 = "       parameter ("
    constantname = $2
    if (constantname in mapname) {
	constantname = mapname[constantname]
	if (constantname == "**NOTUSED**") next
    }

    #
    #  Check for conflicts
    #
    if (constantname in alreadyused) {
	print "Error: file \"" FILENAME "\", line", FNR | stderr
	print "Error: Fortran constant name \"" constantname "\" for \"" $2 "\" conflicts with the" | stderr
	print "Error: Fortran constant name \"" constantname "\" previously defined for \"" alreadyused[constantname] "\"." | stderr
	print "Error: You must either change the names of \"" $2 "\" and \"" alreadyused[constantname] "\"" | stderr
	print "Error: in \"" FILENAME "\" or you must add a map to \"" mapfilename "\"." | stderr
	print "" | stderr
    } else {
	alreadyused[constantname] = $2
    }

    #
    #  Map the value of the constant definition if it is a named constant
    #
    if ($3 ~ /^[A-Za-z]/) {
	if ($3 in mapname) {
	    $3 = mapname[$3]
	    if ($3 == "**NOTUSED**") {
		print "Error:", constantname, "references an unused constant" | stderr
		next
	    }
	}
	gsub(/_/, "", $3)
	$3 = substr($3, 1, 6)
    }

    if (($3 ~ /[*+\/()]/) && !($2 in mapvalue)) {
	print "Warning:", $2 "s value is an expr that may not convert correctly" | stderr
    } else if ($2 in mapvalue) {
	$3 = mapvalue[$2]
    }

    #
    #  Convert C hexadecimal constants to decimal constants
    #
    if ($3 ~ /^0x[0-9a-fA-F]+$/) {
	l = length($3)
	decimal = 0
	for (i = l; i >= 2; i--) {
	    decimal += hex2dec[substr($3, i, 1)] * (16 ^ (l-i))
	}
	$3 = decimal
    }

    #
    #  Pretty print formatting of output
    #
    if (length(constantname) < 4) {
	$3 = "      = " $3 " )"
    } else {
	$3 = "= " $3 " )"
    }
    $4 = "  "$4

    #
    #  Figure out the type of the parameter, a decimal point indicates
    #  a REAL, all others are supposed integer.
    #
    type = specialtype[$3]
    if (type == "") {
	if ($3 ~ /[0-9]*\.[0-9]/) {
	    type = "real*4   "
	} else {
	    type = "integer*4"
	}
    }

    $2 = constantname

    #
    #  Strip trailing comment if it exists
    #
    sub(/\/\*.*\*\//,"")
    continueTrailingComment = sub(/\/\*.*$/,"")     # True if comment conts

    print "       " type "   " constantname
    print
    next
}

#
#  Continue stripping trailing comment of a #define up to end of comment
#
(continueTrailingComment != false) {
    if (index($0, "*/")) {
	continueTrailingComment = false
	sub(/.*\*\//,"")
	print
    }
    next
}

#
#  Strip cpp directives
#
$1 ~ /^#/ {
    print "Info: removed", $0 | stderr
    next
}

{print}
