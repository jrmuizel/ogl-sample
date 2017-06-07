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
# $Header: //depot/main/tools/gfxinstall/gfxinstall.pl#11 $

#
# A fancy UNIX "install for both UNIX and Windows.
#   Copy/rename files/directories, set modes and ownership appropriately.
#

use Getopt::Long;
use File::Path;
use File::Basename;

# find out running environment 
$path = $ENV{PATH};
if ( $path =~ m/\\/) {
    $OS = "win";
} else {
    $OS = "unix";
}

#
# print usage
#
sub usage() {
    print "Usage: ";
    print "gfxinstall ";
    print "[-m mode]";
    print "[-g gid]";
    print "[-u uid]";
    print "[-F path]";
    print "[-srcdst]";
    print "[-debug]";
    print "[-nopath]";
    print "[-v]";
    print " files ...\n";
}

#
# get args
#
sub getargs() {
    @args = (@args, @_);
}


#
# Parse command line
#
%optctl = ();
$optctl{"m"} = "0755";
$optctl{"u"} = $<;
$optctl{"g"} = $(;
$optctl{"F"} = "";
$optctl{"debug"} = 0;
$optctl{"srcdst"} = 0;
$optctl{"nopath"} = 0;
$ret = GetOptions(\%optctl, 
		  "m=s",
		  "u=s",
		  "g=s", 
		  "F=s",
		  "debug",
		  "srcdst",
		  "nopath",
		  "v",
		  "<>", \&getargs);

if ($ret == 0) {
    usage();
    exit(1);
}

# check if destination directory was specified.
if ($optctl{"F"} eq "") {
    print "No -F option specified.\n";
    usage();
    exit(1);
}

# replace backslashes with slashes
$optctl{"F"} =~ s/\\/\//g;

# mkdir the destination directory
mkpath([$optctl{"F"}], $optctl{"debug"}, 0755);

# for each arg... install it!
if ($optctl{"debug"}) {
    print "Mode: ", $optctl{"m"}, "\n";
    print "Owner: ", $optctl{"u"}, "\n";
    print "Group: ", $optctl{"g"}, "\n";
    print "Directory: ", $optctl{"F"}, "\n";
    if ($optctl{"srcdst"} != 0) {
	print "Source-destination file pairing enabled\n";
    }
}
while($_ = shift(@args)) {
    $src = $_;
    if ($optctl{"srcdst"} == 0) {
	$dest = $src;
    } else {
	$dest = shift(@args);	# consume one more arg
	if ($dest eq '.') {
	    $dest = $src;	# alias '.' to mean "same name"
	}
    }
    if ($optctl{"nopath"}) {
    	# strip path from source while installing
	$dest = basename($dest);
    }
    $dest = $optctl{"F"} . "/" . $dest;

    # make sure destination directory exists
    mkpath([dirname($dest)], $optctl{"debug"}, 0755);

    # ignore if source file does not exist
    if (-e $src == 0) { 
	if ($optctl{"debug"} || $optctl{"v"}) {
	    print "installing $src (missing)\n";
	    next; 
	}
    }

    # update only if dest exists and older
    if (-e $dest && (-M $dest <= -M $src)) {
	if ($optctl{"debug"} || $optctl{"v"}) {
	    print "installing $src (ignored)\n";
	}
	next;
    }

    # chmod destination file, so that we can delete it.
    chmod 0777, $dest;
    # remove the destination, just to make sure.
    unlink $dest;

    if ($optctl{"debug"} || $optctl{"v"})  { 
    	print "installing $_ in $dest\n"; 
    }

    open (IN, $src) || die("cannot open " . $src . "\n");
    open (OUT, "> $dest") || die("cannot open " . $dest . "\n");
    if ($OS eq "win") {
	binmode IN;
	binmode OUT;
    }

    while(<IN>) { print OUT; }

    close(IN);
    close(OUT);

    chmod oct($optctl{"m"}), $dest;
    if ($OS eq "unix") {
	chown $optctl{"u"}, $optctl{"g"}, $dest;
    }
}
