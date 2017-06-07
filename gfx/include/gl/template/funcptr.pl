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

# $Date$ $Revision$
# $Header: //depot/main/gfx/include/gl/template/funcptr.pl#6 $

@options = (
	"procprefix=s",
	"output=s",
	"formals",
	"future"
	);
require libspec;

##########################################################################

sub initialize {

    # these should be defined on the command line that invokes libspec
    $proc_prefix = $optionvars{"procprefix"};
    $output = $optionvars{"output"};
    $formals = $optionvars{"formals"};
    $future = $optionvars{"future"};

    # Initialization for sanitizing function names:
    $MaxIDLen = 31;

    $last_category = "";
}

sub main {
    if ($future) {
    	if (!defined $propListValues{"extension", "future"}) { return; }
    } else {
    	if (defined $propListValues{"extension", "future"}) { return; }
    }

    # Is the name distinguishable in its first MaxIDLen characters?
    $TruncatedName = substr($functionName, 0,
    		$MaxIDLen - length($proc_prefix));
    if (defined $OriginalNames{$TruncatedName}) {
	if ($functionName ne $OriginalNames{$TruncatedName}) {
	    error( sprintf("the name %s is indistinguishable from %s",
	    		$functionName, $OriginalNames{$TruncatedName}) );
	}
    } else {
    	$OriginalNames{$TruncatedName} = $functionName;
    }

    # OUTPUT should be defined on the command line that invokes libspec
    #
    #  Print the function declaration
    #
    $category = substr($propList{"category"}, 1,
    	length($propList{"category"})-1);
    $ext = substr($category, 0, 4);
    if (($ext eq "EXT_") ||
    	($ext eq "SGI_") ||
	($ext eq "WIN_") ||
	($ext eq "1_2") ||
	($ext eq "ARB_")) {
	if ($last_category ne $category) {
	    print "\n/* " . $category . " */\n";
	    $last_category = $category;
	}
	print "typedef " . $returnType . " ";
	$fnUpper = $functionName; $fnUpper =~ tr/[a-z]/[A-Z]/;
	printf "(APIENTRY * PFNGL%sPROC) ", $fnUpper;
	print "(";

	if ($formals) {
	    outputCArglist();
	} else {
	    outputCPrototype();
	}
	print ");\n"
    }
}

sub finalize {
}

