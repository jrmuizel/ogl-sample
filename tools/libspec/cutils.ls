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
#  NAME
#	cutils.ls - libspec utility functions for generating C source
#
#  SYNOPSIS
#	cat <script> $TOOLROOT/usr/lib/cutil.ls > temp.ls
#	libspec [options] temp.ls <specfile>
#	rm temp.ls
#
#  DESCRIPTION
#	A collection of useful libspec functions for use when generating
#	C source.
#
#  AUTHOR
#	Herb Kuta (kuta)
#
#  $Date$ $Revision$
#  $Header$
#
##############################################################################


##############################################################################
#
# outputCPrototype
#   - Write the prototype for the current function to stdout.
#   - Does not print the enclosing parentheses.
#
##############################################################################

function outputCPrototype() {
    _cutilsOutList(1)
}


##############################################################################
#
# outputCArglist
#   - Write the new-style argument list for the current function to stdout.
#   - Does not print the enclosing parentheses.
#
##############################################################################

function outputCArglist() {
    _cutilsOutList(0)
}

##############################################################################
#
# outputCOldArglist
#   - Write the old-style argument list for the current function to stdout.
#   - Does not print the enclosing parentheses.
#
##############################################################################

function outputCOldArglist() {
    _cutilsOutList(2)
}

##############################################################################
#
# outputCOldArgDecls
#   - Write the old-style argument declarations for the current function to
#     stdout, one to a line with no leading space.
#
##############################################################################

function outputCOldArgDecls() {
    _cutilsOutList(3)
}

##############################################################################
#
# _cutilsOutList
#   - Write the argument list for the current function to stdout in various
#     forms, depending on the value of the <kind> argument:
#	0: new-style (ANSI) argument list
#	1: new-style argument list without the identifiers, ie. a prototype
#	2: old-style argument list, i.e. no types
#	3: old-style argument declarations, one to a line with no leading
#	   indentation
#   - Does not print the enclosing parentheses.
#
##############################################################################

function _cutilsOutList(kind) {
    if (paramCount == 0) {
	if (kind <= 1)
	    printf "void"
	# else output nothing
    }
    else {
	for (i=1; i<=paramCount; i++) {
	    #
	    #  Print the argument type
	    #

	    param = paramName[i]
	    ttype = paramTransferType[param]
	    dtype = paramDeclaredType[param]

	    if (paramDirection[param] == "in" &&
		    (ttype == "array" || ttype == "reference")) {
		if (kind != 2) {
		    printf "const "
		}
	    }

	    if (param == "...") {
		printf param
		break		# must be the last
	    }

	    if (kind == 2)
		printf "%s", param
	    else {
		printf dtype
		if (dtype == "...")
		    break		# must be the last

		if (kind == 1) {
		    if ((ttype == "reference") || (ttype == "array"))
			printf " *"
		}
		else {
		    if ((ttype == "reference") || (ttype == "array"))
			printf " *%s", param
		    else
			printf " %s", param
		}
	    }

	    if (kind == 3)
		print ";"
	    else {
		if (i != paramCount) {
		    printf ", "
		}
	    }
	}
    }
}
