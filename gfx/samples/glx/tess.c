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
** $Header: //depot/main/gfx/samples/glx/tess.c#3 $
*/
#include <GL/glx.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/keysym.h>
#include <assert.h>

static int RGB_SB_attributes[] = {
    GLX_RGBA,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    None,
};

static int RGB_DB_attributes[] = {
    GLX_RGBA,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_DOUBLEBUFFER,
    GLX_DEPTH_SIZE, 1,
    GLX_STENCIL_SIZE, 1,
    None,
};

static int CI_SB_attributes[] = {
    GLX_DEPTH_SIZE, 1,
    GLX_STENCIL_SIZE, 1,
    None,
};

static int CI_DB_attributes[] = {
    GLX_DOUBLEBUFFER,
    GLX_DEPTH_SIZE, 1,
    GLX_STENCIL_SIZE, 1,
    None,
};

int rgb = 1;
int W = 500;
int H = 500;

/*
** A contour.
*/
struct vert {
    GLdouble v[3];
    struct vert *next;
    GLuint name;
};

/*
** A polygon (multiple contours).
*/
struct polygon {
    struct vert *contour;
    struct polygon *next;
};

#define SELECT_LINE	1
#define SELECT_VERTEX	2

GLuint vertName = 2;
struct polygon *thePoly = NULL;
struct vert *foundVertex = NULL;
int leftDown, middleDown, rightDown;
int tessIt;
int showTess;
int doEdgeFlags;
int gotError;
int doubleBuf = 1;
GLUtesselator *tobj;
Display *dpy;
Window window;
Colormap cmap;

unsigned char font6x10[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x10, 0x38, 0x38, 0x7c, 0x38, 0x38, 0x10, 0x00, 
0x00, 0x00, 0x54, 0x28, 0x54, 0x28, 0x54, 0x28, 0x54, 0x00, 
0x08, 0x08, 0x08, 0x3c, 0x48, 0x48, 0x78, 0x48, 0x48, 0x00, 
0x10, 0x10, 0x18, 0x10, 0x5c, 0x40, 0x60, 0x40, 0x70, 0x00, 
0x24, 0x24, 0x38, 0x24, 0x38, 0x38, 0x40, 0x40, 0x38, 0x00, 
0x20, 0x20, 0x38, 0x20, 0x3c, 0x78, 0x40, 0x40, 0x40, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x28, 0x28, 0x38, 0x00, 
0x00, 0x00, 0x7c, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x00, 
0x3c, 0x20, 0x20, 0x20, 0x48, 0x58, 0x68, 0x68, 0x48, 0x00, 
0x08, 0x08, 0x08, 0x08, 0x3c, 0x20, 0x30, 0x48, 0x48, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x10, 0x10, 0x10, 0x10, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00, 0x00, 0x00, 0x00, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x1c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x10, 0x10, 0x10, 0x10, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x10, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x1c, 0x10, 0x10, 0x10, 0x10, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x10, 0x10, 0x10, 0x10, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x10, 0x10, 0x10, 0x10, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x00, 0x00, 
0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 
0x00, 0x7c, 0x04, 0x08, 0x20, 0x40, 0x20, 0x08, 0x04, 0x00, 
0x00, 0x7c, 0x40, 0x20, 0x08, 0x04, 0x08, 0x20, 0x40, 0x00, 
0x00, 0x00, 0x28, 0x28, 0x28, 0x28, 0x7c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x40, 0x20, 0x7c, 0x10, 0x7c, 0x08, 0x04, 0x00, 
0x00, 0x20, 0x6c, 0x38, 0x20, 0x78, 0x20, 0x24, 0x18, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x28, 0x28, 0x00, 
0x00, 0x00, 0x28, 0x28, 0x7c, 0x28, 0x7c, 0x28, 0x28, 0x00, 
0x00, 0x00, 0x10, 0x38, 0x14, 0x38, 0x50, 0x38, 0x10, 0x00, 
0x00, 0x00, 0x48, 0x54, 0x28, 0x10, 0x28, 0x54, 0x24, 0x00, 
0x00, 0x00, 0x34, 0x48, 0x54, 0x20, 0x50, 0x50, 0x20, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x10, 0x18, 0x00, 
0x00, 0x00, 0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00, 
0x00, 0x00, 0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00, 
0x00, 0x00, 0x00, 0x44, 0x28, 0x7c, 0x28, 0x44, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x00, 
0x00, 0x20, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x10, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x40, 0x40, 0x20, 0x10, 0x08, 0x04, 0x04, 0x00, 
0x00, 0x00, 0x10, 0x28, 0x44, 0x44, 0x44, 0x28, 0x10, 0x00, 
0x00, 0x00, 0x7c, 0x10, 0x10, 0x10, 0x50, 0x30, 0x10, 0x00, 
0x00, 0x00, 0x7c, 0x40, 0x20, 0x18, 0x04, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x04, 0x18, 0x08, 0x04, 0x7c, 0x00, 
0x00, 0x00, 0x08, 0x08, 0x7c, 0x48, 0x28, 0x18, 0x08, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x04, 0x64, 0x58, 0x40, 0x7c, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x64, 0x58, 0x40, 0x20, 0x18, 0x00, 
0x00, 0x00, 0x20, 0x20, 0x10, 0x08, 0x08, 0x04, 0x7c, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x30, 0x08, 0x04, 0x34, 0x4c, 0x44, 0x38, 0x00, 
0x00, 0x10, 0x38, 0x10, 0x00, 0x10, 0x38, 0x10, 0x00, 0x00, 
0x00, 0x20, 0x10, 0x18, 0x00, 0x10, 0x38, 0x10, 0x00, 0x00, 
0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x40, 0x20, 0x10, 0x08, 0x10, 0x20, 0x40, 0x00, 
0x00, 0x00, 0x10, 0x00, 0x10, 0x10, 0x08, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x38, 0x40, 0x58, 0x54, 0x4c, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x44, 0x44, 0x7c, 0x44, 0x44, 0x28, 0x10, 0x00, 
0x00, 0x00, 0x78, 0x24, 0x24, 0x38, 0x24, 0x24, 0x78, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x40, 0x40, 0x40, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x78, 0x24, 0x24, 0x24, 0x24, 0x24, 0x78, 0x00, 
0x00, 0x00, 0x7c, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7c, 0x00, 
0x00, 0x00, 0x40, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7c, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x4c, 0x40, 0x40, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x44, 0x44, 0x44, 0x7c, 0x44, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x04, 0x04, 0x04, 0x04, 0x1c, 0x00, 
0x00, 0x00, 0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x00, 
0x00, 0x00, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 
0x00, 0x00, 0x44, 0x44, 0x44, 0x54, 0x6c, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x44, 0x44, 0x4c, 0x54, 0x64, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x40, 0x40, 0x40, 0x78, 0x44, 0x44, 0x78, 0x00, 
0x00, 0x04, 0x38, 0x54, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x44, 0x48, 0x50, 0x78, 0x44, 0x44, 0x78, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x04, 0x38, 0x40, 0x44, 0x38, 0x00, 
0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7c, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x10, 0x28, 0x28, 0x28, 0x44, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x44, 0x6c, 0x54, 0x54, 0x44, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x44, 0x44, 0x28, 0x10, 0x28, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x28, 0x44, 0x44, 0x00, 
0x00, 0x00, 0x7c, 0x40, 0x20, 0x10, 0x08, 0x04, 0x7c, 0x00, 
0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00, 
0x00, 0x00, 0x04, 0x04, 0x08, 0x10, 0x20, 0x40, 0x40, 0x00, 
0x00, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x28, 0x10, 0x00, 
0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x30, 0x00, 
0x00, 0x00, 0x3c, 0x44, 0x3c, 0x04, 0x38, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x58, 0x64, 0x44, 0x64, 0x58, 0x40, 0x40, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x40, 0x44, 0x38, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x34, 0x4c, 0x44, 0x4c, 0x34, 0x04, 0x04, 0x00, 
0x00, 0x00, 0x38, 0x40, 0x7c, 0x44, 0x38, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x20, 0x20, 0x20, 0x78, 0x20, 0x24, 0x18, 0x00, 
0x38, 0x44, 0x38, 0x40, 0x30, 0x48, 0x34, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x44, 0x44, 0x44, 0x64, 0x58, 0x40, 0x40, 0x00, 
0x00, 0x00, 0x38, 0x10, 0x10, 0x10, 0x30, 0x00, 0x10, 0x00, 
0x30, 0x48, 0x48, 0x08, 0x08, 0x08, 0x18, 0x00, 0x08, 0x00, 
0x00, 0x00, 0x44, 0x48, 0x70, 0x48, 0x44, 0x40, 0x40, 0x00, 
0x00, 0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x30, 0x00, 
0x00, 0x00, 0x44, 0x54, 0x54, 0x54, 0x68, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x44, 0x44, 0x44, 0x64, 0x58, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 
0x40, 0x40, 0x40, 0x58, 0x64, 0x64, 0x58, 0x00, 0x00, 0x00, 
0x04, 0x04, 0x04, 0x34, 0x4c, 0x4c, 0x34, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x40, 0x40, 0x40, 0x64, 0x58, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x78, 0x04, 0x38, 0x40, 0x38, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x18, 0x24, 0x20, 0x20, 0x78, 0x20, 0x20, 0x00, 
0x00, 0x00, 0x34, 0x4c, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x10, 0x28, 0x28, 0x44, 0x44, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x28, 0x54, 0x54, 0x44, 0x44, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 
0x38, 0x44, 0x04, 0x34, 0x4c, 0x44, 0x44, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x7c, 0x20, 0x10, 0x08, 0x7c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x0c, 0x10, 0x08, 0x30, 0x08, 0x10, 0x0c, 0x00, 
0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 
0x00, 0x00, 0x60, 0x10, 0x20, 0x18, 0x20, 0x10, 0x60, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x54, 0x24, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

int fontbase;

void initializeFont(void)
{
    int i;

    fontbase=glGenLists(256);
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (i=0; i<128; i++) {
	glNewList(i+fontbase, GL_COMPILE);
	glBitmap(6, 10, 0.0, 0.0, 6.0, 0.0, font6x10+10*i);
	glEndList();
    }
}

void writeFont(char *string)
{
    glListBase(fontbase);
    glCallLists(strlen(string), GL_BYTE, (unsigned char *) string);
}

void myBegin(GLenum which)
{
    glBegin(which);
}

#define MAXTOBEFREED 100
static struct vert *toBeFreed[MAXTOBEFREED];
static int counter= 0;

void tessPoly(void)
{
    struct polygon *poly;
    struct vert *vert;

    gluTessBeginPolygon(tobj, NULL);
    for (poly=thePoly; poly; poly=poly->next) {
	gluTessBeginContour(tobj);
	for (vert=poly->contour; vert; vert=vert->next) {
	    gluTessVertex(tobj, vert->v, vert->v);
	}
	gluTessEndContour(tobj);
    }
    gluTessEndPolygon(tobj);

    {
       /* up= min(counter,MAXTOBEFREED); */
       int up= (counter < MAXTOBEFREED) ? counter : MAXTOBEFREED;
       int ii;

       /* only free what we saved */
       for (ii= 0; ii< up; ii++) {
	  free( (void *) toBeFreed[ii] );
       }
       if (counter > MAXTOBEFREED) {
	  printf("%d vertices not freed.\n",counter - MAXTOBEFREED); 
       }
       counter= 0;
    }
}

static void edgeFlag(int flag)
{
}

static void Redraw(GLenum mode)
{
    struct polygon *poly;
    struct vert *vert;

    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(3);

    gotError = 0;
    glInitNames();
    if (tessIt) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.5, 0.5, 0.5);
	glIndexf(15);
	tessPoly();
    }
    if (!gotError && showTess) {
	glColor3f(1.0, 1.0, 0.0);
	glIndexf(3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	tessPoly();
    }
    if (!gotError && doEdgeFlags) {
	glLineWidth(3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.0, 0.0, 1.0);
	glIndexf(4);
	gluTessCallback(tobj, GLU_TESS_EDGE_FLAG, &glEdgeFlag);
	tessPoly();
	gluTessCallback(tobj, GLU_TESS_EDGE_FLAG, &edgeFlag);
	glLineWidth(1);
    }

    if (gotError) {
	glColor3f(1.0, 1.0, 1.0);
	glIndexf(7);
	glRasterPos2f(5.0, 15.0);
	glColor3f(0.0, 0.0, 1.0);
	glIndexf(4);
	writeFont((char *) gluErrorString(gotError));
    }

    /* So glLoadName will be legal -- strange semantics */
    glPushName(1);
    for (poly=thePoly; poly; poly=poly->next) {
	glColor3f(0.0, 1.0, 0.0);
	glIndexf(2);
	glLoadName(SELECT_LINE);
	for (vert=poly->contour; vert; vert=vert->next) {
	    glPushName(vert->name);
	    glBegin(GL_LINES);
	    glVertex2dv(vert->v);
	    if (vert->next) {
		glVertex2dv(vert->next->v);
	    } else {
		glVertex2dv(poly->contour->v);
	    }
	    glEnd();
	    glPopName();
	}
	glColor3f(1.0, 0.0, 0.0);
	glIndexf(1);
	glLoadName(SELECT_VERTEX);
	for (vert=poly->contour; vert; vert=vert->next) {
	    glPushName(vert->name);
	    glBegin(GL_POINTS);
	    glVertex2dv(vert->v);
	    glEnd();
	    glPopName();
	}
    }

    if (mode == GL_RENDER) {
	glXSwapBuffers(dpy, window);
    }
}

static void Usage(char *name)
{
    fprintf(stderr, "Usage: %s [-c] [-s]\n",name);
    exit(1);
}

static Bool WaitForMapNotify(Display *d, XEvent *e, char *arg)
{
    if ((e->type == MapNotify) && (e->xmap.window == (Window) arg)) {
	return GL_TRUE;
    }
    return GL_FALSE;
}

/*
** Create a new contour at x,y.
*/
void newContour(int x, int y)
{
    struct polygon *newPoly;
    struct vert *newCont;
    struct polygon **polyTail;

    newPoly = (struct polygon *) malloc(sizeof(struct polygon));
    newCont = (struct vert *) malloc(sizeof(struct vert));

    polyTail = &thePoly;
    while (*polyTail) {
	polyTail = &((*polyTail)->next);
    }

    *polyTail = newPoly;
    newPoly->next = NULL;
    newPoly->contour = newCont;

    newCont->next = NULL;
    newCont->v[0] = x;
    newCont->v[1] = y;
    newCont->v[2] = 0;
    newCont->name = vertName++;
}

/*
** Create a new edge at this vertex
*/
struct vert *addVertex(struct vert *where, int x, int y)
{
    struct vert *newVert;

    newVert = (struct vert *) malloc(sizeof(struct vert));

    newVert->next = where->next;
    where->next = newVert;
    newVert->v[0] = x;
    newVert->v[1] = y;
    newVert->v[2] = 0;
    newVert->name = vertName++;

    return newVert;
}

void deleteVertex(void)
{
    struct polygon *poly, *poly2;
    struct vert *vert;

    if (!foundVertex) return;

    for (poly=thePoly; poly; poly=poly->next) {
	if (poly->contour == foundVertex) {
	    poly->contour = foundVertex->next;
	    free(foundVertex);
	    foundVertex = NULL;
	    if (poly->contour == NULL) {
		if (poly == thePoly) {
		    thePoly = poly->next;
		    free(poly);
		    return;
		}
		for (poly2=thePoly; poly2; poly2=poly2->next) {
		    if (poly2->next == poly) {
			poly2->next = poly->next;
			free(poly);
			return;
		    }
		}
	    }
	    return;
	}
	for (vert=poly->contour; vert; vert=vert->next) {
	    if (vert->next == foundVertex) {
		vert->next = foundVertex->next;
		free(foundVertex);
		foundVertex = NULL;
		return;
	    }
	}
    }
}

struct vert *lookupVertex(GLuint name)
{
    struct polygon *poly;
    struct vert *vert;

    for (poly=thePoly; poly; poly=poly->next) {
	for (vert=poly->contour; vert; vert=vert->next) {
          if (vert->name == name) return (vert);
	}
    }
    return(NULL);
}

void freePolygon(struct polygon *thePoly)
{
    
    struct polygon *poly, *nextPoly;
    struct vert *vert, *nextVert;

    for (poly=thePoly; poly; poly=nextPoly) {
        nextPoly = poly->next;
	for (vert=poly->contour; vert; vert=nextVert) {
          nextVert = vert->next;
          free(vert);
	}
        free(poly);
    }
}

void findVertex(int x, int y, int type)
{
    static GLuint selectBuf[1000];
    GLint hits;
    GLint viewport[4];
    int i;

    foundVertex = NULL;
    glSelectBuffer(1000, selectBuf);
    (void) glRenderMode(GL_SELECT);
    glInitNames();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluPickMatrix(x, y, 6, 6, viewport);
    gluOrtho2D(0,W,0,H);
    glMatrixMode(GL_MODELVIEW);

    Redraw(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    hits = glRenderMode(GL_RENDER);
    if (hits <= 0) return;

    /*
    ** Each entry looks like:
    **   count (2)
    **   min z, max z
    **   SELECT_LINE or SELECT_VERTEX
    **   pointer to vertex.
    */

    i = 0;
    while (hits) {
	hits --;
	if (selectBuf[i] == 0) {
	    i+= 3;
	    continue;
	}
	assert(selectBuf[i] == 2);
	i += 3;
	if (selectBuf[i] == type) {
	    foundVertex = lookupVertex(selectBuf[i+1]);
	    return;
	}
	i += 2;
    }
}

void moveVertex(int x, int y)
{
    if (foundVertex) {
	foundVertex->v[0] = x;
	foundVertex->v[1] = y;
    }
}

void errorHandler(GLenum error)
{
    if (gotError) {
	printf("Double errors!?!\n");
    }
    gotError = error;
}

void myEnd(void)
{
    glEnd();
}


static void combine( GLdouble coords[3], struct vert *data[4],
                     GLfloat weight[4], struct vert **dataOut )
{
   struct vert *vertex;
   vertex = (struct vert *) malloc(sizeof(struct vert));

   if (counter < MAXTOBEFREED) {
      toBeFreed[counter]= vertex;
   }
   counter++;

   vertex->v[0] = coords[0];
   vertex->v[1] = coords[1];
   vertex->name = vertName++;
   *dataOut = vertex;
}

static void Init(void)
{
    glViewport(0,0,W,H);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,W,0,H);
    glMatrixMode(GL_MODELVIEW);

    if (rgb) {
	glClearColor(0,0,0,0);
    } else {
	glClearIndex(0);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    tobj = gluNewTess();
    gluTessCallback(tobj, GLU_TESS_VERTEX, &glVertex2dv);
    gluTessCallback(tobj, GLU_TESS_BEGIN, &myBegin);
    gluTessCallback(tobj, GLU_TESS_END, &myEnd);
    gluTessCallback(tobj, GLU_TESS_ERROR, &errorHandler);
    gluTessCallback(tobj, GLU_TESS_COMBINE, &combine);
    initializeFont();
}

static void setColorMap(void)
{
    XColor *xc;
    long i;

    xc = (XColor *) malloc(16 * sizeof(XColor));
    for (i=0; i<16; i++) {
	xc[i].pixel = i;
	xc[i].red = (i & 1) ? 65535 : 0;
	xc[i].green = (i & 2) ? 65535 : 0;
	xc[i].blue = (i & 4) ? 65535 : 0;
	if (i > 8) {
	    xc[i].red /= 2;
	    xc[i].green /= 2;
	    xc[i].blue /= 2;
	}
	xc[i].flags = DoRed | DoGreen | DoBlue;
    }
    XStoreColors(dpy, cmap, xc, 16);

    free((void *) xc);
}

int main(int argc, char** argv)
{
    XVisualInfo *vi;
    XSetWindowAttributes swa;
    GLXContext cx;
    XEvent event;
    GLboolean needDisplay;
    XColor white;
    int i;

    rgb = 1;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
              case 'c':
                rgb = GL_FALSE;
                break;
	      case 's':
		doubleBuf = 0;
		break;
              default:
                Usage(argv[0]);
            }
        } else {
            Usage(argv[0]);
        }
    }

    dpy = XOpenDisplay(0);
    if (!dpy) {
	fprintf(stderr, "Can't connect to display \"%s\"\n", getenv("DISPLAY"));
	return -1;
    }

    /* compile-time check for GLU 1.2 */
#if !defined(GLU_VERSION_1_2)
#error "This requires GLU 1.2."    
    fprintf(stderr,"%s requires GLU 1.2\n",argv[0]);
    exit(1);
#endif
    /* run-time check for GLU 1.2 */
    {
       int major, minor;
       Bool isGLU_1_2= False;	/* assume GLU is not version 1.2 */

       /* gluGetString() exists iff GLX version >= 1.1 */
       glXQueryVersion(dpy,&major,&minor);
       if (minor > 0 || major > 1) {
	  /* is GLU version 1.2? */
	  const GLubyte *string= gluGetString(GLU_VERSION);
#define GLU_1_2_STRING "1.2"
	  if (strtod((const char *)string,NULL) >= 1.2) {
	     isGLU_1_2= True;	/* GLU version is 1.2 */
	  }
       }
       if (isGLU_1_2 == False) {
	  fprintf(stderr,"%s requires GLU 1.2\n",argv[0]);
	  exit(1);
       }
    }

    vi = glXChooseVisual(dpy, DefaultScreen(dpy),
	    doubleBuf ? (rgb ? RGB_DB_attributes : CI_DB_attributes) :
	    (rgb ? RGB_SB_attributes : CI_SB_attributes));
    if (!vi) {
	fprintf(stderr, "No singlebuffered rgba visual on \"%s\"\n",
		getenv("DISPLAY"));
	return -1;
    }

    cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual,
			   rgb ? AllocNone : AllocAll);
    white.red = ~0;
    white.green = ~0;
    white.blue = ~0;
    XAllocColor(dpy, cmap, &white);

    swa.border_pixel = 0;
    swa.background_pixel = white.pixel;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask
	| KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
	PointerMotionMask;
    window = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 10, 10,
			   W, H,
			   0, vi->depth, InputOutput, vi->visual,
			   CWBackPixel|CWBorderPixel|CWColormap|CWEventMask,
			   &swa);
    XSetWMColormapWindows(dpy, window, &window, 1);
    XMapWindow(dpy, window);
    XIfEvent(dpy, &event, WaitForMapNotify, (char*)window);

    cx = glXCreateContext(dpy, vi, 0, GL_TRUE);
    if (!glXMakeCurrent(dpy, window, cx)) {
	fprintf(stderr, "Can't make window current to context\n");
	return -1;
    }

    if (!rgb) {
	setColorMap();
    }

    Init();

    printf("\n\nc to create a new contour.\n");
    printf("d to delete a vertex.\n");
    printf("t to tessellate and draw the polygon.\n");
    printf("s to show the tessellation.\n");
    printf("e to draw the polygon in Polymode(GL_LINE) with edge flags.\n");
    printf("Left mouse moves a vertex.\n");
    printf("Middle mouse inserts a vertex into a contour.\n");

    leftDown = middleDown = rightDown = GL_FALSE;
    newContour(100, 100);
    findVertex(100, 100, SELECT_VERTEX);
    addVertex(foundVertex, 100, 400);
    findVertex(100, 400, SELECT_VERTEX);
    addVertex(foundVertex, 400, 400);
    findVertex(400, 400, SELECT_VERTEX);
    addVertex(foundVertex, 400, 100);
    foundVertex = NULL;
    for (;;) {
        needDisplay = GL_FALSE;
	do {
	    XNextEvent(dpy, &event);
	    switch (event.type) {
	      case Expose:
                needDisplay = GL_TRUE;	       
		break;
	      case ConfigureNotify:
		W = event.xconfigure.width;
		H = event.xconfigure.height;
		glViewport(0,0,W,H);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,W,0,H);
		glMatrixMode(GL_MODELVIEW);
                needDisplay = GL_TRUE;
		break;
	      case ButtonPress:
		/* 
		** Only effectively allow one mouse button down at a time.
		*/
		leftDown = middleDown = rightDown = GL_FALSE;
		switch(event.xbutton.button) {
		  case 1:	/* Left - move vertex */
		    findVertex(event.xbutton.x, H - event.xbutton.y, 
			    SELECT_VERTEX);
		    moveVertex(event.xbutton.x, H - event.xbutton.y);
		    leftDown = GL_TRUE;
		    break;
		  case 2:	/* Middle - insert vertex */
		    middleDown = GL_TRUE;
		    findVertex(event.xbutton.x, H - event.xbutton.y, 
			    SELECT_LINE);
		    if (foundVertex) {
			foundVertex = addVertex(foundVertex, 
				event.xbutton.x, H - event.xbutton.y);
		    }
		    break;
		  case 3:	/* Right */
		    rightDown = GL_TRUE;
		    break;
		}
		break;
	      case ButtonRelease:
		switch(event.xbutton.button) {
		  case 1:
		    if (leftDown && foundVertex) {
			moveVertex(event.xbutton.x, H - event.xbutton.y);
			leftDown = GL_FALSE;
			needDisplay = GL_TRUE;
		    } 
		    break;
		  case 2:
		    if (middleDown && foundVertex) {
			moveVertex(event.xbutton.x, H - event.xbutton.y);
			middleDown = GL_FALSE;
			needDisplay = GL_TRUE;
		    }
		    break;
		}
		break;
	      case MotionNotify:
		if (leftDown && foundVertex) {
		    moveVertex(event.xmotion.x, H - event.xmotion.y);
		    needDisplay = GL_TRUE;
		} else if (middleDown && foundVertex) {
		    moveVertex(event.xmotion.x, H - event.xmotion.y);
		    needDisplay = GL_TRUE;
		}
		break;
	      case KeyPress:
		{
		    char buf[100];
		    int rv;
		    KeySym ks;

		    rv = XLookupString(&event.xkey, buf, sizeof(buf), &ks, 0);
		    switch (ks) {
		      case XK_C: case XK_c:
			newContour(event.xkey.x, H - event.xkey.y);
			needDisplay = GL_TRUE;
			break;
		      case XK_D: case XK_d:
			findVertex(event.xbutton.x, H - event.xbutton.y, 
				SELECT_VERTEX);
			if (foundVertex) {
			    deleteVertex();
			    needDisplay = GL_TRUE;
			} 
			break;
		      case XK_T: case XK_t:
			tessIt = 1-tessIt;
			needDisplay = GL_TRUE;
			break;
		      case XK_S: case XK_s:
			showTess = 1-showTess;
			needDisplay = GL_TRUE;
			break;
		      case XK_E: case XK_e:
			doEdgeFlags = 1-doEdgeFlags;
			needDisplay = GL_TRUE;
			break;
		      case XK_Escape:
			gluDeleteTess(tobj);			
			freePolygon(thePoly);
			thePoly = NULL;
#if 0
			XFree(vi);
                        glXMakeCurrent(dpy, None, NULL);
                        glXDestroyContext(dpy, cx);
                        XCloseDisplay(dpy);
#endif
			return 0;
		      default:
			break;
		    }
		}
		break;
	      default:
		break;
	    }
	} while (XPending(dpy) != 0);

	if (needDisplay) {
	    needDisplay = GL_FALSE;
	    Redraw(GL_RENDER);
	}
    }
}


