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
*/
/*
** cc -o tex3d tex3d.c -lGLU -lGL -lXext -lX11 -lm
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

int numSlices = 64;
int useSmooth = False;
int moveTexture = False;
int moveObject = False;
GLenum drawStyle = GL_QUADS;
GLfloat textureRotX, textureRotY;
GLfloat objectRotX, objectRotY;
GLfloat textureScaleX = 0.85, textureScaleY = 0.85, textureScaleZ = 0.85;

#define MAX(a,b)	((a) >= (b) ? (a) : (b))
#define MIN(a,b)	((a) <= (b) ? (a) : (b))

/* (int)floor(log2((int)x)) */
static int
intFloorLog2(unsigned int x)
{
    int a = 0;
    while (x >>= 1) ++a;
    return a;
}

void
setNullTexture(void)
{
    GLubyte texPixel[4] = { 0xff, 0xff, 0xff, 0xff };
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 1, 1, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, texPixel);
}

void
setCheckTexture(void)
{
    int texWidth = 128;
    int texHeight = 128;
    GLubyte *texPixels, *p;
    int i, j;

    texPixels = (GLubyte *) malloc(texWidth*texHeight*4*sizeof(GLubyte));
    if (texPixels == NULL) {
	return;
    }

    p = texPixels;
    for (i=0; i<texHeight; ++i) {
	for (j=0; j<texWidth; ++j) {
	    if ((i ^ j) & 8) {
		p[0] = 0xff; p[1] = 0xff; p[2] = 0xff; p[3] = 0xff;
	    } else {
		p[0] = 0x08; p[1] = 0x08; p[2] = 0x08; p[3] = 0xff;
	    }
	    p += 4;
	}
    }

    glTexParameteri(GL_TEXTURE_2D,
		    GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
		    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, texWidth, texHeight,
		      GL_RGBA, GL_UNSIGNED_BYTE, texPixels);
    free(texPixels);
}

#define NUM_RAMP_COLORS (sizeof(rampColors) / sizeof(rampColors[0]))
GLubyte rampColors[][4] = {
    { 0x20, 0x00, 0x20, 0xff },
    { 0x00, 0x00, 0x40, 0xff },
    { 0x00, 0x20, 0x80, 0xff },
    { 0x30, 0x80, 0x10, 0xff },
    { 0x80, 0x80, 0x00, 0xff },
    { 0x80, 0x40, 0x00, 0xff },
    { 0x80, 0x00, 0x00, 0xff },
};

void
set3DMipmapRamp(void)
{
    int texWidth = 64;
    int texHeight = 64;
    int texDepth = 64;
    int texSize = texWidth * texHeight * texDepth;
    int border = 1;
    GLubyte *texPixels, *p;
    int level, numLevels;
    int i, j, k;

    texPixels = (GLubyte *) malloc(texSize*4*sizeof(GLubyte));
    if (texPixels == NULL) {
	return;
    }

    numLevels = intFloorLog2(MAX(MAX(texWidth, texHeight), texDepth)) + 1;

    for (level=0; level<numLevels; ++level) {
	int color = level * ((float) (NUM_RAMP_COLORS-1) / (numLevels-1));
	GLubyte *c = &rampColors[color][0];

	p = texPixels;
	for (i=0; i<texDepth; ++i) {
	    for (j=0; j<texHeight; ++j) {
		for (k=0; k<texWidth; ++k) {
		    if ((i < border || i > texDepth-border) ||
			(j < border || j > texHeight-border) ||
			(k < border || k > texWidth-border))
		    {
			p[0] = p[1] = p[2] = p[3] = 0x00;
		    } else {
			p[0] = c[0];
			p[1] = c[1];
			p[2] = c[2];
			p[3] = c[3];
		    }
		    p += 4;
		}
	    }
	}

	glTexParameteri(GL_TEXTURE_3D,
			GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexImage3D(GL_TEXTURE_3D, level, GL_RGBA,
		     texWidth, texHeight, texDepth, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, texPixels);

	if (texWidth > 1) texWidth >>= 1;
	if (texHeight > 1) texHeight >>= 1;
	if (texDepth > 1) texDepth >>= 1;
    }

    free(texPixels);
}

void
set3DColorCube(void)
{
    int texWidth = 64;
    int texHeight = 64;
    int texDepth = 64;
    int texSize = texWidth * texHeight * texDepth;
    int border = 2;
    GLubyte *texPixels, *p;
    int i, j, k;

    texPixels = (GLubyte *) malloc(texSize*4*sizeof(GLubyte));
    if (texPixels == NULL) {
	return;
    }

    p = texPixels;
    for (i=0; i<texDepth; ++i) {
	for (j=0; j<texHeight; ++j) {
	    for (k=0; k<texWidth; ++k) {
		if ((i < border || i > texDepth-border) ||
		    (j < border || j > texHeight-border) ||
		    (k < border || k > texWidth-border))
		{
		    p[0] = p[1] = p[2] = p[3] = 0x00;
		} else {
		    p[0] = k * 255.0/texWidth;
		    p[1] = j * 255.0/texHeight;
		    p[2] = i * 255.0/texDepth;
		    p[3] = 0xff;
		}
		p += 4;
	    }
	}
    }

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA,
		 texWidth, texHeight, texDepth, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, texPixels);
    free(texPixels);
}

void
initialize(void)
{
    glMatrixMode(GL_PROJECTION);
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 3.0);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0, 0.0, -2.0);

    /*
    setCheckTexture();
    set3DMipmapRamp();
    */
    set3DColorCube();
}

void
redraw(void)
{
    int i, j, k;

    glClearColor(0.01, 0.01, 0.01, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.5, 0.5, 0.5);
    glRotatef(-textureRotY, 0, 1, 0);
    glRotatef(-textureRotX, 1, 0, 0);
    glScalef(textureScaleX, textureScaleY, textureScaleZ);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glRotatef(objectRotY, 0, 1, 0);
    glRotatef(objectRotX, 1, 0, 0);

    glColor4f(1.0, 1.0, 1.0, 2.0/numSlices);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_3D);

    switch (drawStyle) {
      case GL_POINTS:
	if (useSmooth) glEnable(GL_POINT_SMOOTH);
	glPointSize(2.0);
	glBegin(GL_POINTS);
	for (i=0; i<=numSlices; ++i) {
	    GLfloat z = i * 2.0/numSlices - 1.0;
	    for (j=0; j<=numSlices; ++j) {
		GLfloat y = j * 2.0/numSlices - 1.0;
		for (k=0; k<numSlices; ++k) {
		    GLfloat x = k * 2.0/numSlices - 1.0;
		    glTexCoord3f(x, y, z); glVertex3f(x, y, z);
		}
	    }
	}
	glEnd();
	if (useSmooth) glDisable(GL_POINT_SMOOTH);
	break;
      case GL_LINES:
	if (useSmooth) glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0);
	for (i=0; i<=numSlices; ++i) {
	    GLfloat z = i * 2.0/numSlices - 1.0;
	    for (j=0; j<=numSlices; ++j) {
		GLfloat y = j * 2.0/numSlices - 1.0;
		glBegin(GL_LINES);
		for (k=0; k<numSlices; ++k) {
		    GLfloat x = k * 2.0/numSlices - 1.0;
		    glTexCoord3f(x, y, z); glVertex3f(x, y, z);
		}
		glEnd();
	    }
	}
	if (useSmooth) glDisable(GL_LINE_SMOOTH);
	break;
      case GL_QUADS:
      default:
	if (useSmooth) glEnable(GL_POLYGON_SMOOTH);
	glBegin(GL_QUADS);
	for (i=0; i<numSlices; ++i) {
	    GLfloat z = i * 2.0/numSlices - 1.0;
	    glTexCoord3f(-1.0,-1.0, z); glVertex3f(-1.0, -1.0, z);
	    glTexCoord3f( 1.0,-1.0, z); glVertex3f( 1.0, -1.0, z);
	    glTexCoord3f( 1.0, 1.0, z); glVertex3f( 1.0,  1.0, z);
	    glTexCoord3f(-1.0, 1.0, z); glVertex3f(-1.0,  1.0, z);
	}
	glEnd();
	if (useSmooth) glDisable(GL_POLYGON_SMOOTH);
	break;
    }

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_3D);

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void
checkErrors(void)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
	fprintf(stderr, "Error: %s\n", (char *) gluErrorString(error));
    }
}

static Bool
waitForMapNotify(Display *dpy, XEvent *ev, XPointer arg)
{
    return (ev->type == MapNotify && ev->xmap.window == *((Window *) arg));
}

void
usage(int argc, char **argv)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [ options ]\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -sb  single buffered\n");
    fprintf(stderr, "    -db  double buffered\n");
    fprintf(stderr, "\n");
}

int
main(int argc, char **argv)
{
    int visualAttrSB[] = {
	GLX_RGBA, GLX_RED_SIZE, 1, None
    };
    int visualAttrDB[] = {
	GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, None
    };
    int *visualAttr = visualAttrSB;
    Display *dpy;
    int scrn;
    Window root, win;
    XVisualInfo *vis;
    Colormap cmap;
    XSetWindowAttributes swa;
    XSizeHints sizeHints;
    char *name = "Texture 3D";
    GLXContext ctx;
    XEvent ev;
    int x = 0, y = 0;
    int width = 300, height = 300;
    int needsRedraw = True;
    int curx, cury;
    int i;

    for (i=1; i<argc; ++i) {
	if (strcmp("-sb", argv[i]) == 0) {
	    visualAttr = visualAttrSB;

	} else if (strcmp("-db", argv[i]) == 0) {
	    visualAttr = visualAttrDB;

	} else {
	    usage(argc, argv);
	    exit(EXIT_FAILURE);
	}
    }

    if ((dpy = XOpenDisplay(NULL)) == NULL) {
	fprintf(stderr, "can't open display\n");
	exit(EXIT_FAILURE);
    }
    scrn = DefaultScreen(dpy);
    root = RootWindow(dpy, scrn);

    if ((vis = glXChooseVisual(dpy, scrn, visualAttr)) == NULL) {
	fprintf(stderr, "can't find visual\n");
	exit(EXIT_FAILURE);
    }

    if ((cmap = XCreateColormap(dpy, root, vis->visual, AllocNone)) == None) {
	fprintf(stderr, "can't create colormap\n");
	exit(EXIT_FAILURE);
    }

    swa.background_pixel = None;
    swa.border_pixel = None;
    swa.colormap = cmap;
    swa.event_mask = StructureNotifyMask | KeyPressMask | ExposureMask |
		     ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;

    win = XCreateWindow(dpy, root, x, y, width, height, 0,
			vis->depth, InputOutput, vis->visual,
			CWBackPixel | CWBorderPixel |
			CWColormap | CWEventMask, &swa);

    sizeHints.flags = PPosition | PSize;
    sizeHints.width = width;
    sizeHints.height = height;
    sizeHints.x = x;
    sizeHints.y = y;
    XSetStandardProperties(dpy, win, name, name, None, argv, argc, &sizeHints);
    
    XMapWindow(dpy, win);
    XIfEvent(dpy, &ev, waitForMapNotify, (XPointer) &win);

    if ((ctx = glXCreateContext(dpy, vis, NULL, True)) == NULL) {
	fprintf(stderr, "can't create context\n");
	exit(EXIT_FAILURE);
    }

    glXMakeCurrent(dpy, win, ctx);

    initialize();

    while (1) {
	do {
	    XNextEvent(dpy, &ev);
	    switch (ev.type) {
	      case KeyPress:
		{
		    KeySym ks;

		    XLookupString(&ev.xkey, NULL, 0, &ks, NULL);
		    switch (ks) {
		      case XK_Escape:
			exit(EXIT_SUCCESS);
			break;
		      case XK_space:
			needsRedraw = True;
			break;
		      case XK_p:
			drawStyle = GL_POINTS;
			needsRedraw = True;
			break;
		      case XK_l:
			drawStyle = GL_LINES;
			needsRedraw = True;
			break;
		      case XK_q:
			drawStyle = GL_QUADS;
			needsRedraw = True;
			break;
		      case XK_s:
			useSmooth = !useSmooth;
			needsRedraw = True;
			break;
		      default:
			break;
		    }
		}
		break;
	      case ButtonPress:
		if ((ev.xbutton.button == Button1) && !moveObject) {
		    moveTexture = True;
		    curx = ev.xbutton.x;
		    cury = ev.xbutton.y;
		} else if ((ev.xbutton.button == Button2) && !moveTexture) {
		    moveObject = True;
		    curx = ev.xbutton.x;
		    cury = ev.xbutton.y;
		}
		break;
	      case ButtonRelease:
		if ((ev.xbutton.button == Button1) && moveTexture) {
		    moveTexture = False;
		} else if ((ev.xbutton.button == Button2) && moveObject) {
		    moveObject = False;
		}
		break;
	      case MotionNotify:
		if (moveTexture || moveObject) {
		    GLfloat dx = (ev.xmotion.y - cury) * 360.0 / height;
		    GLfloat dy = (ev.xmotion.x - curx) * 360.0 / width;
		    if (moveTexture) {
			textureRotX += dx;
			if (textureRotX > 360) textureRotX -= 360;
			if (textureRotX < 0) textureRotX += 360;
			textureRotY += dy;
			if (textureRotY > 360) textureRotY -= 360;
			if (textureRotY < 0) textureRotY += 360;
		    } else if (moveObject) {
			objectRotX += dx;
			if (objectRotX > 360) objectRotX -= 360;
			if (objectRotX < 0) objectRotX += 360;
			objectRotY += dy;
			if (objectRotY > 360) objectRotY -= 360;
			if (objectRotY < 0) objectRotY += 360;
		    }
		    curx = ev.xmotion.x;
		    cury = ev.xmotion.y;
		    needsRedraw = True;
		}
		break;
	      case ConfigureNotify:
		if (ev.xconfigure.window == win) {
		    width = ev.xconfigure.width;
		    height = ev.xconfigure.height;
		    glViewport(0, 0, width, height);
		    needsRedraw = True;
		}
		break;
	      case Expose:
		if (ev.xexpose.window == win) {
		    needsRedraw = True;
		}
		break;
	      default:
		break;
	    }
	} while (XPending(dpy));

	if (needsRedraw) {
	    redraw();
	    glFlush();
	    glXSwapBuffers(dpy, win);
	    needsRedraw = False;
	}

	checkErrors();
    }
}
