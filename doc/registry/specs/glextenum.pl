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
# $Date: 2000/04/26 02:10:53 $ $Revision: 1.1 $
# $Header: /home/pub/cvs/projects/ogl-sample/main/doc/registry/specs/glextenum.pl,v 1.1 2000/04/26 02:10:53 ljp Exp $
#

# invoke with -s to enable some command-line parsing

sub error {
    my ($msg) = @_;

    print STDERR "ERROR at line " . $line . ": " . $msg . "\n";
    exit(1);
}

use Getopt::Long;

# get options
@options = (
	"enumprefix=s",
	"maskprefix=s",
	"defineprefix=s",
	"protect"
	);

%optionvars = ();
$optionstatus = GetOptions(\%optionvars, @options);

# perform initialization
$enum_prefix = $optionvars{"enumprefix"};
$mask_prefix = $optionvars{"maskprefix"};
$define_prefix = $optionvars{"defineprefix"};
$protect = $optionvars{"protect"};

print STDERR "protect = $protect\n";

$enum_cmd = "enum:";
$mask_cmd = "mask:";
$float_cmd = "float:";
$define_cmd = "define:";
$use_cmd = "use";

$enum_mode = 1;
$mask_mode = 2;
$float_mode = 3;
$define_mode = 4;

$mymode = 0;

print "/*************************************************************/\n";

$line = 0;
while (<>) {
    $line++;
    chop;

    @f = split ' ', $_;

    # skip comments
    if (/^#/) { next; }

    # skip blank lines
    if ($#f == -1) { next; }

    # Pass through specified text
    if (/^passthru:/) {
	s/^passthru:[ \t]?//;
	print $_ . "\n";
	next;
    }

    # When switching out of enum mode, terminate the previous #ifdef
    # block. This assumes none of the other modes are ever invoked.
    if (($#f == 1) && ($f[1] eq $enum_cmd)) {
	if ($mymode eq $enum_mode) {
	    if ($protect) {
		print "#endif\n";
	    }
	}
	if ($protect == 1) {
	    print "\n#ifndef " . $enum_prefix . $f[0] . "\n";
	}
	$mymode = $enum_mode;
	$enumval = 0;
	next;
    }

    if (($#f == 1) && ($f[1] eq $mask_cmd)) {
	print "\n/* " . $f[0] . " */\n";
	$mymode = $mask_mode;
	$maskval = 1;
	next;
    }

    if (($#f == 1) && ($f[1] eq $float_cmd)) {
	print "\n/* " . $f[0] . " */\n";
	$mymode = $float_mode;
	$floatenum = 0;
	next;
    }

    if (($#f == 1) && ($f[1] eq $define_cmd)) {
	print "\n/* " . $f[0] . " */\n";
	$mymode = $define_mode;
	$definenum = 0;
	next;
    }

    if (($#f == 2) && ($f[0] eq $use_cmd)) {
	# print "/*      " . $enum_prefix . $f[2] . " */\n";
	print "/* reuse " . $enum_prefix . $f[2] . " */\n";
	next;
    }

    if ($mymode eq $enum_mode) {
	if ($#f > 0) {
	    # Do we do an assignment?
	    if ($f[1] eq "=") {
		# Do we assign to a known variable?
		if (substr($f[2], 1, 1) eq "\$") {
		    # Do we add to the known variable?
		    if ($f[3] eq "+") {
			$offset = $f[4];
		    } else {
			$offset = 0;
		    }
		    $value = $offset + $variables{substr($f[3],2)};
		} else {
		    $value = $f[2];
		}
	    }
	} else {
	    $value = $enumval;
	    $enumval++;
	}
	# Has this enumeration been defined?
	if (defined $variables{$f[0]}) {
	    # Is it being redefined to something else?
	    if ($value != $variables{$f[0]}) {
		error("enum symbol " . $f[0] . " redefined");
	    } else {
		next;
	    }
	}
	$variables{$f[0]} = $value;
	printf "#define %s%-30s %s\n", $enum_prefix, $f[0], $value;
    } elsif ($mymode eq $mask_mode) {
	# Has this enumeration been defined?
	if (defined $variables{$f[0]}) {
	    # Is it being redefined to something else?
	    if ($maskval != $variables{$f[0]}) {
		error("mask symbol " . $f[0] . " redefined");
	    } else {
		next;
	    }
	}
	printf "#define %s%-30s 0x%08x\n", $mask_prefix, $f[0], $maskval;
	$variables{$f[0]} = $maskval;
	$maskval = $maskval * 2;

    } elsif ($mymode eq $float_mode) {
	#
	# XXX:
	# Since these 2 formats (float_mode and define_mode) are not
	# currently used, they are not updated, and they are currently
	# broken.
	#

	$n = $floatenum;
	if ($f[1] eq "=") {
	    printf "#define %s%-30s %d\n", $define_prefix, $f[0], $f[2];
	} else {
	    printf "#define %s%-30s %d\n", $define_prefix, $f[0], $n;
	}
	$floatenum++;
    } elsif ($mymode eq $define_mode) {
	$n = $definenum;
	if ($f[1] eq "=") {
	    printf "#define %s%-30s %d\n", $define_prefix, $f[0], $f[2];
	} else {
	    printf "#define %s%-30s %d\n", $define_prefix, $f[0], $n;
	}
	$defineenum++;
    }
}

# Terminate final #ifdef block, if there is one
if ($mymode eq $enum_mode) {
    if ($protect == 1) {
	print "#endif\n";
    }
}

print "\n";

print "\n/*************************************************************/\n";
print "\n";
