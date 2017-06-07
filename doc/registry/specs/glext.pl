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
# Additional Notice Provisions: This software was created using the
# OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
# not been independently verified as being compliant with the OpenGL(R)
# version 1.2.1 Specification.
#
# $Date: 2000/05/30 07:11:57 $ $Revision: 1.2 $
# $Header: /home/pub/cvs/projects/ogl-sample/main/doc/registry/specs/glext.pl,v 1.2 2000/05/30 07:11:57 ljp Exp $
#

# -copyright - path to copyright file to insert (commented)
# -protect - preprocessor symbol to protect against multiple inclusion
# -file1,-file2 - enum and prototype files to insert

use Getopt::Long;

# get options
@options = (
	"copyright=s",
	"protect=s",
	"file1=s",
	"file2=s"
	);

%optionvars = ();
$optionstatus = GetOptions(\%optionvars, @options);

$copyright = $optionvars{"copyright"};
$protect = $optionvars{"protect"};
$file1 = $optionvars{"file1"};
$file2 = $optionvars{"file2"};

#
# function to cat a file
#
sub catFile {
    my ($file) = @_;

    if ($file) {
	open(FILE, $file) || die("Cannot open file " . $file . "\n");
	while(<FILE>) { print; }
	close(FILE);
    }
}

# boilerplate around the enums and prototypes

print "#ifndef " . $protect . "\n";
print "#define " . $protect . "\n";
print "\n";
print "#ifdef __cplusplus\n";
print "extern \"C\" {\n";
print "#endif\n";
print "\n";

# grab copyright
open(COPY, $copyright) || die ("cannot open copyright file\n");
print "/*\n";
while(<COPY>) {
    print "** " . $_;
}
print "*/\n";
close(COPY);

print "\n";

# Windows cruft
print "#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__)\n";
print "#define WIN32_LEAN_AND_MEAN 1\n";
print "#include <windows.h>\n";
print "#endif\n";
print "\n";
print "#ifndef APIENTRY\n";
print "#define APIENTRY\n";
print "#endif\n";
print "\n";

# cat remainder files.
catFile($file1);
catFile($file2);

print "\n";
print "#ifdef __cplusplus\n";
print "}\n";
print "#endif\n";
print "\n";
print "#endif\n";

