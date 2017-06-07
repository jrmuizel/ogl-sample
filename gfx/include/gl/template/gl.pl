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
# $Date$ $Revision$
# $Header: //depot/main/gfx/include/gl/template/gl.pl#6 $
#

#
# invoke with -s to do some argument parsing
#

$copyright = $COPYRIGHT;
$file1 = $FILE1;
$file2 = $FILE2;
$file3 = $FILE3;

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

#
# buld the GL header
#

print "#ifndef __gl_h_\n";
print "#define __gl_h_\n";
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

# print typedefs
print "typedef unsigned int GLenum;\n";
print "typedef unsigned char GLboolean;\n";
print "typedef unsigned int GLbitfield;\n";
print "typedef signed char GLbyte;\n";
print "typedef short GLshort;\n";
print "typedef int GLint;\n";
print "typedef int GLsizei;\n";
print "typedef unsigned char GLubyte;\n";
print "typedef unsigned short GLushort;\n";
print "typedef unsigned int GLuint;\n";
print "typedef float GLfloat;\n";
print "typedef float GLclampf;\n";
print "typedef double GLdouble;\n";
print "typedef double GLclampd;\n";
print "typedef void GLvoid;\n";
print "/* Internal convenience typedefs */\n";
print "typedef void (*_GLfuncptr)();\n";
print "\n";

# cat remainder files.
catFile($file1);
catFile($file2);
catFile($file3);

print "\n";
print "#ifdef __cplusplus\n";
print "}\n";
print "#endif\n";
print "\n";
print "#endif /* __gl_h_ */\n";

