#! /bin/sh

################################################################################
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
#  NAME
#	libspec - library specification translator
#
#  SYNOPSIS
#	libspec [-D] [-T typemappath] [-S parser] [typemap] script specfile
#
#  DESCRIPTION
#	libspec allows user programs to read a library specification file
#	and generate what ever information is desired.  Typical applications
#	are generation of external specifications for different languages,
#	documentation information, etc.
#
#	libspec is based on the new version of awk.  A specfile parser
#	written in awk is used to parse the specfile translating the file
#	into a form that a user's script can deal with.
#
#  OPTIONS
#	-d		debug mode, save temporary files (used for development).
#
#	-D assignment	define a name/value pair.  The assignment is of the
#			form <name>=<value>.
#
#	-P parser	use another parser (used for development). 
#
#	-T typemappath	path to look for typeMapFile.
#
#  FILES
#	/usr/lib/libspec.awk		specfile parser
#	$TMPDIR/libspecxxxxx.pgm	temporary file which has specfile parser
#					prepended to the user's script
#
#  SEE ALSO
#	awk(1), nawk(1).
#	"The AWK Programming Language"; Aho, Kernighan, and Weinberger.
#
#  AUTHOR
#	Dave Ciemiewicz (ciemo)
#
################################################################################

#
#  Termination routines
#
cleanup() {
    if test ! "$DEBUG"; then
	rm -f $PROGRAM
    fi
}

abnormalstop() {
    cleanup
    exit 1
}

trap abnormalstop 1 2 3 9 15

#
#  Constants
#
AWK=/usr/bin/awk
TMPDIR=${TMPDIR:-/usr/tmp}
PROGRAM=$TMPDIR/libspec$$.pgm
PARSER=$TOOLROOT/usr/lib/libspec.awk
USAGE="Usage: "$0" [-d] [-D assignment] [-T typemappath] [-P parser] [typemap] script specfile"

#
#  Parse command line
#
while getopts "dD:T:P:" f
do
    case $f in
    P)		PARSER=$OPTARG;;
    T)		TYPEMAPPATH=$OPTARG;;
    D)		DEFINES=$DEFINES" -v "$OPTARG;;
    d)		DEBUG=1;;
    \?)		echo $USAGE 1>&2
		exit 2;;
    esac
done
shift `expr $OPTIND - 1`

case $# in
    2)		SCRIPT=$1 SPEC=$2;;
    3)		TYPEMAP=$1 SCRIPT=$2 SPEC=$3;;
    *)		echo $USAGE 1>&2
		exit 2;;
esac

#
#  Combine specfile parser with the user's program
#
cat $PARSER $SCRIPT > $PROGRAM

#
#  Run the program.  Filter stderr for more meaningful error messages.
#  The strange redirections permit the swapping of stderr and stdout so
#  stderr can be piped through the awk filter without need of an intermediate
#  file for stdout.  Stderr and stdout are swapped back afterward so
#  everything appears normal.
#
(
$AWK -f $PROGRAM -v typeMapPath=$TYPEMAPPATH -v typeMapArg=$TYPEMAP -v libspecProgram=$SCRIPT $DEFINES $SPEC 2>&1 1>&3 |
$AWK '	#
	#  Parser error output for more meaningful errors
	#
BEGIN {
    stderr = "cat 1>&2"
}

#
#  Print helpful messages about required functions
#
#
function errorReqFunc(fname) {
    print "ERROR: file", usersProgram, "does not implement required function",\
	    fname | stderr
    print "ERROR:", $0 | stderr
}

/ undefined function initialize/ {
    errorReqFunc("initialize")
    next
}

/ undefined function main/ {
    errorReqFunc("main")
    next
}

/ undefined function finalize/ {
    errorReqFunc("finalize")
    next
}

#
#  Grab awk generated error line numbers and resolve them
#  between the users program and the PARSER file
#
/ source line (number )?[0-9]+/ {
    for (i=1; i<=NF; i++) {
	if ($i ~ /^[0-9]+/) {
	    lineNumber = $i
	    position = i
	    break 
	}
    }
    if (lineNumber <= parserLength) {
	print "ERROR:", $0 ", file", parserFile | stderr
    } else {
	lineNumber -= parserLength
	$position = lineNumber ", file " usersProgram ","
	print "ERROR:", $0 | stderr
    }
    next
}

/^(ERROR|Error|error):/ {
    print | stderr
    next
}

{
    print "ERROR:", $0 | stderr
    next
}
' usersProgram=$SCRIPT parserFile=$PARSER parserLength="`wc -l < $PARSER`" -
) 3>&1 1>&2


cleanup
