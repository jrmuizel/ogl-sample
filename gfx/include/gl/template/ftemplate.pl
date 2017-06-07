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
#   $Date$ $Revision$
#   $Header$

#   This script is used to generate declarations for GL function calls.
#   Declarations are not needed for GL subroutines.

@options = (
    "formals=s",
    "procprefix=s",
    "declspec=s",
    "callspec=s",
    "extensions=s",
	);
require libspec;

sub initialize {
    # thses should be defined on the command line that invokes libspec
    $formals = $optionvars{"formals"};
    $proc_prefix = $optionvars{"procprefix"};
    $decl_spec = $optionvars{"declspec"};
    $call_spec = $optionvars{"callspec"};
    $extensions = $optionvars{"extensions"};
    $maxIDLen = 31;
}

sub main {
    #
    #  future and not_implemented routines are not public.
    #
    if (defined $propListValues{"extension","not_implemented"} ||
	defined $propListValues{"extension","future"}) {
	return;
    }

    #
    #  Map function name to specified Fortran name if needed
    #
    if (defined $propList{'fortranname'}) {
	$fields = (@fortranName = split(/,/, $propList{'fortranname'}, 9999));
	if ($fields == 2) {
	    #
	    #  First element of fortranname is "" thus second element
	    #  contains valid name to use
	    #
	    $functionName = $fortranName[2];
	}
	else {
	    &specError("Unexpected multiple names for \"fortranname\" property");
	}
    }

    # Is the name distinguishable in its first maxIDLen characters?
    $TruncatedName = substr($functionName, 1, $maxIDLen - length($proc_prefix));
    if (defined $OriginalNames{$TruncatedName}) {
	if ($functionName ne $OriginalNames{$TruncatedName}) {
	    $ErrorMessage = sprintf('the name %s is indistinguishable from %s',
		$functionName, $OriginalNames{$TruncatedName});
	    &Warning($ErrorMessage);
	}
    }
    else {
	$OriginalNames{$TruncatedName} = $functionName;
    }

    #
    #  Generate Fortran function declarations.	Subroutine declarations
    #  are not needed for Fortran.  returnType of "void" indicates subroutine.
    #
    if ($returnType ne "void") {
	if (defined $propListValues{"status", "obsolete"}) {
	    printf "C  %s: obsolete\n", $proc_prefix . $functionName;
	} elsif (defined $propListValues{"status", "not-recommended"}) {
	    printf "C  %s: not recommended\n", $proc_prefix . $functionName;
	}
	printf "%7s%-20s%s\n", '', $returnType, $proc_prefix . $functionName;
    }
}

sub finalize {
    print "";
}

sub Warning {
    local($message) = @_;
    print STDERR $message;
}
