/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.0 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/glu/libutil/glue.c#4 $
*/

#include <stdlib.h>

static char *__glNurbsErrors[] = {
    ((char *)" "),
    ((char *)"spline order un-supported"),
    ((char *)"too few knots"),
    ((char *)"valid knot range is empty"),
    ((char *)"decreasing knot sequence knot"),
    ((char *)"knot multiplicity greater than order of spline"),
    ((char *)"endcurve() must follow bgncurve()"),
    ((char *)"bgncurve() must precede endcurve()"),
    ((char *)"missing or extra geometric data"),
    ((char *)"can't draw pwlcurves"),
    ((char *)"missing or extra domain data"),
    ((char *)"missing or extra domain data"),
    ((char *)"endtrim() must precede endsurface()"),
    ((char *)"bgnsurface() must precede endsurface()"),
    ((char *)"curve of improper type passed as trim curve"),
    ((char *)"bgnsurface() must precede bgntrim()"),
    ((char *)"endtrim() must follow bgntrim()"),
    ((char *)"bgntrim() must precede endtrim()"),
    ((char *)"invalid or missing trim curve"),
    ((char *)"bgntrim() must precede pwlcurve()"),
    ((char *)"pwlcurve referenced twice"),
    ((char *)"pwlcurve and nurbscurve mixed"),
    ((char *)"improper usage of trim data type"),
    ((char *)"nurbscurve referenced twice"),
    ((char *)"nurbscurve and pwlcurve mixed"),
    ((char *)"nurbssurface referenced twice"),
    ((char *)"invalid property"),
    ((char *)"endsurface() must follow bgnsurface()"),
    ((char *)"intersecting or misoriented trim curves"),
    ((char *)"intersecting trim curves"),
    ((char *)"UNUSED"),
    ((char *)"unconnected trim curves"),
    ((char *)"unknown knot error"),
    ((char *)"negative vertex count encountered"),
    ((char *)"negative byte-stride encounteed"),
    ((char *)"unknown type descriptor"),
    ((char *)"null control point reference"),
    ((char *)"duplicate point on pwlcurve")
};

const char *__glNURBSErrorString( int errno )
{
    return __glNurbsErrors[errno];
}

static char *__glTessErrors[] = {
    ((char *)" "),
    ((char *)"gluTessBeginPolygon() must precede a gluTessEndPolygon()"),
    ((char *)"gluTessBeginContour() must precede a gluTessEndContour()"),
    ((char *)"gluTessEndPolygon() must follow a gluTessBeginPolygon()"),
    ((char *)"gluTessEndContour() must follow a gluTessBeginContour()"),
    ((char *)"a coordinate is too large"),
    ((char *)"need combine callback"),
};

const char *__glTessErrorString( int errno )
{
    return __glTessErrors[errno];
} /* __glTessErrorString() */
