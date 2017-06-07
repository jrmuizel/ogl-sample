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

#
# NAME:
#	libspeccutils.pm: libspec utility functions for generating C source
#
# AUTHOR:
#	Initial authors of awk script:
#		Herb Kuta (kuta)
#		numerous member of the OpenGL teams
#	Ported to perl:
#		George Kyriazis (kyriazis)
#


##############################################################################
#
# outputCPrototype
#   - Write the prototype for the current function to stdout.
#   - Does not print the enclosing parentheses.
#
##############################################################################
sub outputCPrototype {
    print _cutilsOutList(1);
}

##############################################################################
#
# outputCArglist
#   - Write the new-style argument list for the current function to stdout.
#   - Does not print the enclosing parentheses.
#
##############################################################################
sub outputCArglist {
    print _cutilsOutList(0);
}

##############################################################################
#
# outputCArglistInOut
#   - Write the new-style argument list for the current function to stdout.
#   - Argument type is preceded by IN/OUT
#   - Does not print the enclosing parentheses.
#
##############################################################################
sub outputCArglistInOut {
    print _cutilsOutList(4);
}

##############################################################################
#
# outputCOldArglist
#   - Write the old-style argument list for the current function to stdout.
#   - Does not print the enclosing parentheses.
#
##############################################################################
sub outputCOldArglist {
    print _cutilsOutList(2);
}

##############################################################################
#
# outputCOldArgDecls
#   - Write the old-style argument declarations for the current function to
#     stdout, one to a line with no leading space.
#
##############################################################################
sub outCOldArgDecls {
    print _cutilsOutList(3);
}

##############################################################################
#
# makeCPrototype
#   - return the prototype for the current function to stdout.
#   - Does not return the enclosing parentheses.
#
##############################################################################
sub makeCPrototype {
    return _cutilsOutList(1);
}

##############################################################################
#
# makeCArglist
#   - return the new-style argument list for the current function to stdout.
#   - Does not return the enclosing parentheses.
#
##############################################################################
sub makeCArglist {
    return _cutilsOutList(0);
}

##############################################################################
#
# makeCOldArglist
#   - return the old-style argument list for the current function to stdout.
#   - Does not return the enclosing parentheses.
#
##############################################################################
sub makeCOldArglist {
    return _cutilsOutList(2);
}

##############################################################################
#
# _cutilsOutList
#   - evaluates to the argument list for the current function in various
#     forms, depending on the value of the <kind> argument:
#       0: new-style (ANSI) argument list
#       1: new-style argument list without the identifiers, ie. a prototype
#       2: old-style argument list, i.e. no types
#       3: old-style argument declarations, one to a line with no leading
#          indentation
#       4: new-style (ANSI) argument list with IN/OUT declspec
#   - Does not include the enclosing parentheses.
#
##############################################################################
sub _cutilsOutList {
    my ($kind) = @_;
    my ($i);
    my ($param);
    my ($ttype);
    my ($dtype);
    my ($arglist);

    if ($paramCount == 0) {
	if ($kind <= 1) {
	    $arglist .= "void";
	}
	# else output nothing
    } else {
	for ($i = 1; $i <= $paramCount; $i++) {
	    #
	    # print the argument type
	    #
	    $param = $paramName{$i};
	    $ttype = $paramTransferType{$param};
	    $dtype = $paramDeclaredType{$param};

	    if ($kind == 4) {
		if ($paramDirection{$param} eq "in") {
		   $arglist .= "IN ";
		} elsif ($paramDirection{$param} eq "out") {
		   $arglist .= "OUT ";
		}
	    }

	    if (($paramDirection{$param} eq "in") &&
		(($ttype eq "array") || ($ttype eq "reference"))) {
		if ($kind != 2) {
		    $arglist .= "const ";
		}
	    }

	    if ($param eq '...') {
		$arglist .= $param;
		break;		# must be the last
	    }

	    if ($kind == 2) {
		$arglist .= $param;
	    } else {
		$arglist .= $dtype;
		if ($dtype eq '...') {
		    break;	# must be the last
		}

		if ($kind == 1) {
		    if (($ttype eq "reference") || ($ttype eq "array")) {
			$arglist .= " *";
		    }
		} else {
		    if (($ttype eq "reference") || ($ttype eq "array")) {
			$arglist .= " *" . $param;
		    } else {
			$arglist .= " " . $param;
		    }
		}
	    }

	    if ($kind == 3) {
		$arglist .= ";";
	    } else {
		if ($i != $paramCount) {
		    $arglist .= ", ";
		}
	    }
	}
    }

    $arglist;
}


#
# have the module return a true value so that compilation continues;
#
1;
