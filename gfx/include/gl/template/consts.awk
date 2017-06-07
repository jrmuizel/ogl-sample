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
# $Date$ $Revision$
# $Header$

#
# Parse enum.spec file and output constants for cpp
#
BEGIN {
    PREFIX = "GL_";
}

{
    if ($1 != "#") {
	if (NF == 2) {
	    if ($1 == "See") {
		printf "/* See %s */\n", $2;
	    } else {
		categoryName = $1;
		categoryType = $2;
		maskValue = 1;
		printf "/* %s */\n", $1;
	    }
	} else
	if (NF == 0) {
	    printf "\n";
	} else
	if (NF == 1) {
	    if (categoryType == "mask:") {
		if ((v = alreadyHasValue[$1]) != "") {
		    printf "/* OOPS %s */\n", $1;
		    exit;
		}
		printf "#define %s%-20s 0x%08x\n", PREFIX, $1, maskValue;
		maskValue = maskValue * 2;
	    } else {
		if ((v = alreadyHasValue[$1]) != "") {
		    printf "/*      %s%-20s %s */\n", PREFIX, $1, v;
		} else {
		    v = nextInt++;
		    alreadyHasValue[$1] = v;
		    printf "#define %s%-20s %s\n", PREFIX, $1, v;
		}
	    }
	} else
	if (NF == 3) {
	    if ($2 != "=") {
		printf "/*OOPS SYNTAX*/ ", $1;
	    }
	    v1 = alreadyHasValue[$1];
	    v = $3;
	    if (substr(v,1,1) == "$") {
		v = PREFIX "" substr(v,2,length(v));
	    }
	    if (v1 && (v1 != v)) {
		printf "/*OOPS DUP*/";
		exit
	    }
	    if (v1 != v) {
		alreadyHasValue[$1] = v;
		printf "#define %s%-20s %s\n", PREFIX, $1, v;
	    } else {
		printf "/*      %s%-20s %s */\n", PREFIX, $1, v;
	    }
	} else {
	    printf "/*OOPS SYNTAX*/";
	    exit
	}
    }
}
