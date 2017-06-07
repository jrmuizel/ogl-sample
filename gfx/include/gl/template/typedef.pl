#!/usr/freeware/bin/perl
eval 'exec /usr/freeware/bin/perl -S $0 ${1+"$@"}'
    if $running_under_some_shell;
			# this emulates #! processing on NIH machines.
			# (remove #! line above if indigestible)

eval '$'.$1.'$2;' while $ARGV[0] =~ /^([A-Za-z_0-9]+=)(.*)/ && shift;
			# process any FOO=bar switches

#
# Copyright 1992-1998 Silicon Graphics, Inc.
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
# the contents of this file may not be disclosed to third parties, copied or
# duplicated in any form, in whole or in part, without the prior written
# permission of Silicon Graphics, Inc.
#
# RESTRICTED RIGHTS LEGEND:
# Use, duplication or disclosure by the Government is subject to restrictions
# as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
# and Computer Software clause at DFARS 252.227-7013, and/or in similar or
# successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
# rights reserved under the Copyright Laws of the United States.
#

# $Header: /home/pub/cvs/projects/ogl-sample/main/gfx/include/gl/template/typedef.pl,v 1.1 2000/04/26 05:53:59 ljp Exp $

#
# Generate the C typedefs from spec types to wire types.
#

$[ = 1;			# set array base to 1

# The CAPITALIZED variables should be defined on the command line.
$to_prefix = $TOPREFIX;
$from_prefix = $FROMPREFIX;

# Ignore blank lines.

line: while (<>) {
    chop;	# strip record separator
    @Fld = split(' ', $_, 9999);
    if (($#Fld == 0)) {
	next line;
    }

    if ($#Fld < 2) {
	printf "Error: only 1 field in line %d.\n", $.;
	$ExitValue = 1; last line;
    }

    # don't typedef something back to itself	
    if ($Fld[1] eq $Fld[2]) {	#???
	next;
    }
    if ($Fld[1] eq 'void' || $Fld[2] eq 'enum') {
	next;

	# matrix is special
	;
    }
    if ($Fld[1] eq 'MatrixF' || $Fld[1] eq 'MatrixD') {
	next;
    }
    printf 'typedef ';
    for ($i = 2; $i <= $#Fld; $i++) {
	printf '%s%s ', $to_prefix, $Fld[$i];
    }
    printf "%s%s;\n", $from_prefix, $Fld[1];
}

exit $ExitValue;
