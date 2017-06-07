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
# $Header$

BEGIN {
    true = 1;
    false = 0;
    min_single_opcode = 0;
    for (i in SingleTable) {
	delete SingleTable[i];
    }
    for (i in VendorPrivTable) {
	delete VendorPrivTable[i];
    }
    for (i in RenderTable) {
	delete RenderTable[i];
    }
    for (i in RenderSize) {
	delete RenderSize[i];
    }
    for (i in RenderConst) {
	delete RenderConst[i];
    }
    for (i in HC) {
	delete HC[i];
    }
    TableHelper();
    if (SUFFIX ~ /_EXT/) {
	_min_single_opcode = min_vendpriv_opcode_EXT;
	_max_single_opcode = max_vendpriv_opcode_EXT;
	_min_render_opcode = min_render_opcode_EXT;
	_max_render_opcode = max_render_opcode_EXT;
	genSingle = false;
	genVendPriv = true;
    } else {
	_min_single_opcode = min_single_opcode;
	_max_single_opcode = max_single_opcode;
	_min_render_opcode = min_render_opcode;
	_max_render_opcode = max_render_opcode;
	genSingle = true;
	genVendPriv = false;
    }
    main();
}

function main() {

    if (OUTPUT ~ /tables/) {
	GenTables();
    }
    if (OUTPUT ~/prototypes/) {
	GenPrototypes(SUFFIX);
    }
    if (OUTPUT ~/rensizes/) {
	GenRenderSizes(SUFFIX);
    }
}

function GenPrototypes(suffix) {
    if (genSingle) {
	GenSinglePrototype(false);
	print;
    }
    if (genVendPriv) {
	GenVendPrivPrototype(false);
	print;
    }
    GenRenderPrototype(false);
    print;
    if (genSingle) {
	GenSinglePrototype(true);
	print;
    }
    if (genVendPriv) {
	GenVendPrivPrototype(true);
	print;
    }
    GenRenderPrototype(true);
    print;
    if (suffix == "") {
	printf "#define __GLX_MIN_GLXCMD_OPCODE 1\n";
	printf "#define __GLX_MAX_GLXCMD_OPCODE 20\n";
    }

    # what to do in case of empty tables
    if (_min_render_opcode == 65536) {
	_min_render_opcode = 0;
    }
    if (_min_single_opcode == 65536) {
	_min_single_opcode = 0;
    }

    printf "#define __GLX_MIN_RENDER_OPCODE%s %d\n", suffix,
			_min_render_opcode;
    printf "#define __GLX_MAX_RENDER_OPCODE%s %d\n", suffix,
			_max_render_opcode;
    if (genSingle) {
	printf "#define __GLX_MIN_SINGLE_OPCODE %d\n", _min_single_opcode;
	printf "#define __GLX_MAX_SINGLE_OPCODE %d\n", _max_single_opcode;
	printf "#define __GLX_SINGLE_TABLE_SIZE %d\n", \
	  # add 1 for range, add another 1 because min is 1 rather than 0
	  _max_single_opcode - _min_single_opcode + 2;
    }
    if (genVendPriv) {
	printf "#define __GLX_MIN_VENDPRIV_OPCODE%s %d\n", suffix, \
			_min_single_opcode;
	printf "#define __GLX_MAX_VENDPRIV_OPCODE%s %d\n", suffix, \
			_max_single_opcode;
	if (_max_single_opcode - _min_single_opcode <= 0) {
	    printf "#define __GLX_VENDPRIV_TABLE_SIZE%s 1\n", suffix;
	} else {
	    printf "#define __GLX_VENDPRIV_TABLE_SIZE%s %d\n", suffix, \
		_max_single_opcode - _min_single_opcode + 1;
	}
    }
    if (suffix == "") {
	# The core dispatch table has an extra entry for opcode == 0
	printf "#define __GLX_RENDER_TABLE_SIZE%s %d\n", suffix, \
	    _max_render_opcode - _min_render_opcode + 2;
    } else {
	printf "#define __GLX_RENDER_TABLE_SIZE%s %d\n", suffix, \
	    _max_render_opcode - _min_render_opcode + 1;
    }
    printf "%s %s%s%s%s%s;\n",
	"extern __GLXdispatchRenderProcPtr", "__glXRenderTable", suffix,
	"[__GLX_RENDER_TABLE_SIZE", suffix, "]";
    if (genSingle) {
	printf "%s %s;\n",
	    "extern __GLXdispatchSingleProcPtr",
	    "__glXSingleTable[__GLX_SINGLE_TABLE_SIZE]";
    }
    if (genVendPriv) {
	printf "%s %s%s%s%s%s;\n",
	    "extern __GLXdispatchVendorPrivProcPtr",
	    "__glXVendorPrivTable", suffix, "[__GLX_VENDPRIV_TABLE_SIZE", suffix, "]";
    }
    printf "%s %s%s%s%s%s;\n",
	"extern __GLXdispatchRenderProcPtr",
	"__glXSwapRenderTable", suffix, "[__GLX_RENDER_TABLE_SIZE", suffix, "]";
    if (genSingle) {
	printf "%s %s%s%s%s%s;\n",
	    "extern __GLXdispatchSingleProcPtr",
	    "__glXSwapSingleTable", suffix, "[__GLX_SINGLE_TABLE_SIZE", suffix, "]";
    }
    if (genVendPriv) {
	printf "%s %s%s%s%s%s;\n",
	    "extern __GLXdispatchVendorPrivProcPtr",
	    "__glXSwapVendorPrivTable", suffix, "[__GLX_VENDPRIV_TABLE_SIZE", \
		suffix, "]";
    }
}

function GenSinglePrototype(swap) {
    if (swap) prefix = "Swap";
    else prefix = "";

    for (i=_min_single_opcode; i <= _max_single_opcode; i++) {
	if (i in SingleTable) {
	    if (i < 100) {
		printf "extern int __glX%s%s(__GLXclientState*, GLbyte*);\n",
		prefix, SingleTable[i];
	    } else {
		printf \
		"extern int __glXDisp%s_%s(__GLXclientState*, GLbyte*);\n",
		prefix, SingleTable[i];
	    }
	}
    }
}

function GenVendPrivPrototype(swap) {
    if (swap) prefix = "Swap";
    else prefix = "";

    for (i=_min_single_opcode; i <= _max_single_opcode; i++) {
	if (i in VendorPrivTable) {
	    printf "extern int __glXDisp%s_%s(__GLXclientState*, GLbyte*);\n",
	    prefix, VendorPrivTable[i];
	}
    }
}

function GenRenderPrototype(swap) {
    if (swap) prefix = "Swap";
    else prefix = "";

    for (i=_min_render_opcode; i <= _max_render_opcode; i++) {
	if (i in RenderTable) {
	    printf "extern void __glXDisp%s_%s(GLbyte*);\n",
	    prefix, RenderTable[i];
	}
    }
}

function GenTables() {
    print "#include \"glxserver.h\"";
    print "#include \"glxext.h\"";
    printf "#include \"g_disptab%s.h\"", SUFFIX;
    print;
    if (genSingle) {
	GenSingleTable(false);
	print;
    }
    GenRenderTable(false, SUFFIX);
    if (genVendPriv) {
	GenVendPrivTable(false, SUFFIX);
	print;
    }
    print;
    if (genSingle) {
	GenSingleTable(true);
	print;
    }
    GenRenderTable(true, SUFFIX);
    if (genVendPriv) {
	GenVendPrivTable(true, SUFFIX);
	print;
    }
}

function GenSingleTable(swap)
{
    if (swap) prefix = "Swap";
    else prefix = "";

    printf "__GLXdispatchSingleProcPtr __glX%sSingleTable[] = {\n", prefix;
    for (i=0; i <= _max_single_opcode; i++) {
	if (i in SingleTable) {
	    if (i < 100) {
	       # these are glx commands
	       printf "\t__glX%s%s,\n", prefix, SingleTable[i];
	    } else {
	       printf "\t__glXDisp%s_%s,\n", prefix, SingleTable[i];
	    }
	} else {
	    printf "\t__glXNoSuchSingleOpcode,\n";
	}
    }
    print "};";
}

function GenVendPrivTable(swap, suffix)
{
    if (swap) prefix = "Swap";
    else prefix = "";

    if (_max_single_opcode - _min_single_opcode > 0) {
	printf "__GLXdispatchVendorPrivProcPtr __glX%sVendorPrivTable%s[] = {\n", prefix, suffix;
	for (i=_min_single_opcode; i <= _max_single_opcode; i++) {
	    if (i in VendorPrivTable) {
		printf "\t__glXDisp%s_%s,\n", prefix, VendorPrivTable[i];
	    } else {
		printf "\t__glXNoSuchVendorPrivOpcode,\n";
	    }
	}
	print "};";
    } else {
	printf "__GLXdispatchVendorPrivProcPtr __glX%sVendorPrivTable%s[1];\n", prefix, suffix;
    }
}

function GenRenderTable(swap, suffix)
{
    if (swap) prefix = "Swap";
    else prefix = "";

    printf "__GLXdispatchRenderProcPtr __glX%sRenderTable%s[] = {\n", prefix, suffix;
    if (suffix == "") {
	# The core dispatch table has an extra entry for opcode == 0
	printf "\t__glXNoSuchRenderOpcode,\n";
    }
    if (_min_render_opcode > _max_render_opcode) {
	# special case, if no entries
	printf "\tNULL,\n";
    }
    for (i=_min_render_opcode; i <= _max_render_opcode; i++) {
	if (i in RenderTable) {
	    printf "\t__glXDisp%s_%s,\n", prefix, RenderTable[i];
	} else {
	    printf "\t__glXNoSuchRenderOpcode,\n";
	}
    }
    print "};";
}

function GenRenderSizes(suffix)
{
    if (SUFFIX == "") {
	printf "#include \"glxserver.h\"\n";
    }
    printf "__glXRenderSizeData __glXRenderSizeTable%s[] = {\n", suffix;

    for (i=_min_render_opcode; i <= _max_render_opcode; i++) {
	if (!(i in RenderTable)) {
	    printf "\t/* %-16s    */", "no such opcode";
	    printf "\t{ %3d, ", 0;
	    printf "\t0                        ";
	    printf " },\n";
	    continue;
	}
	if (HC[i]) {
	    #
	    # Spit out a comment for those whose size must be handcoded.
	    #
	    hc = "HC";
	} else {
	    hc = "  ";
	}
	printf "\t/* %-16s %s */", RenderTable[i], hc;
	printf "\t{ %3d, ", RenSize[i];
	if (RenConst[i]) {
	    printf "\t0                        ";
	} else {
	    str = sprintf("__glX%sReqSize%s", RenderTable[i], suffix);
	    printf "\t%-25s", str;
	}
	printf " },\n";
    }
    print "};";
}







