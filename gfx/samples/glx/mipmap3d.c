/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
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
** cc -o mipmap3d mipmap3d.c -lGLU -lGL -lXext -lX11 -lm
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

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
    int texWidth = 32;
    int texHeight = 32;
    int texDepth = 32;
    int texSize = texWidth * texHeight * texDepth;
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
		    p[0] = c[0];
		    p[1] = c[1];
		    p[2] = c[2];
		    p[3] = c[3];
		    p += 4;
		}
	    }
	}

	glTexParameteri(GL_TEXTURE_3D,
			GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
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
initialize(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    set3DMipmapRamp();
}

void
redraw(int width, int height)
{
    int numSteps = 100;
    GLfloat xBase = -1.0;
    GLfloat xStepSize = 2.0 / numSteps;
    GLfloat texBase0, texBase1, texBase, texStepSize;
    int texWidth, texHeight, texDepth;
    int i;

    glClearColor(0.01, 0.01, 0.01, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
			     GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
			     GL_TEXTURE_HEIGHT, &texHeight);
    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
			     GL_TEXTURE_DEPTH, &texDepth);
    
    texBase0 = (float) height / texHeight;
    texBase1 = (float) width / texDepth;

    texBase = texBase1 / numSteps;
    texStepSize = texBase * texDepth - texBase;

    glEnable(GL_TEXTURE_3D);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    for (i=0; i<numSteps; ++i) {
	GLfloat x = xBase + i * xStepSize;
	GLfloat a = (float) i / numSteps;
	GLfloat t0 = texBase0;
	GLfloat t1 = texBase + a*a * texStepSize;

	glTexCoord3f(0.5,  t0, 0.0); glVertex2f(x, 1.00);
	glTexCoord3f(0.5, 0.0, 0.0); glVertex2f(x,-1.00);
	glTexCoord3f(0.5, 0.0,  t1); glVertex2f(x+xStepSize,-1.00);
	glTexCoord3f(0.5,  t0,  t1); glVertex2f(x+xStepSize, 1.00);
    }
    glEnd();
    glDisable(GL_TEXTURE_3D);
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
    char *name = "Mipmap 3D";
    GLXContext ctx;
    XEvent ev;
    int x = 0, y = 0;
    int width = 300, height = 300;
    int needsRedraw = True;
    int cycleTexture = False;
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
	while (XPending(dpy)) {
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
		      case XK_g:
			cycleTexture = True;
			needsRedraw = True;
			break;
		      default:
			break;
		    }
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
	}

	if (cycleTexture) {
	    GLint texWidth, texHeight, texDepth, texBorder;
	    GLenum internalFormat;
	    GLvoid *buf;
	    int bufSize;

	    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
				     GL_TEXTURE_WIDTH, &texWidth);
	    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
				     GL_TEXTURE_HEIGHT, &texHeight);
	    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
				     GL_TEXTURE_DEPTH, &texDepth);
	    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
				     GL_TEXTURE_BORDER, &texBorder);
	    glGetTexLevelParameteriv(GL_TEXTURE_3D, 0,
				     GL_TEXTURE_COMPONENTS,
				     (GLint *) &internalFormat);

	    fprintf(stderr, "dimensions(%dx%dx%d) border(%d) components(0x%x)\n",
		    texWidth, texHeight, texDepth, texBorder, internalFormat);
	    
	    glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
	    glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
	    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	    glPixelStorei(GL_PACK_SKIP_IMAGES, 0);
	    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	    glPixelStorei(GL_PACK_ALIGNMENT, 4);
	    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
	    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
	    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	    glPixelStorei(GL_UNPACK_SKIP_IMAGES, 0);
	    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	    bufSize = texWidth * texHeight * texDepth * 4*sizeof(GLfloat);

	    buf = malloc((size_t) bufSize);

	    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, buf);

	    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat,
			 0, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	    glNewList(1, GL_COMPILE);
	    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat,
			 texWidth, texHeight, texDepth, texBorder,
			 GL_RGBA, GL_FLOAT, buf);
	    
	    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, texHeight);
	    glPixelStorei(GL_UNPACK_ROW_LENGTH, texWidth);
	    glPixelStorei(GL_UNPACK_SKIP_IMAGES, texDepth/3);
	    glPixelStorei(GL_UNPACK_SKIP_ROWS, texHeight/3);
	    glPixelStorei(GL_UNPACK_SKIP_PIXELS, texWidth/3);
	    glTexSubImage3D(GL_TEXTURE_3D, 0,
			    texWidth/3, texHeight/3, texDepth/3,
			    texWidth/3, texHeight/3, texDepth/3,
			    GL_RGBA, GL_FLOAT, buf);

	    glEndList();

	    glCallList(1);
	    
	    free(buf);

	    cycleTexture = False;
	    needsRedraw = True;
	}

	if (needsRedraw) {
	    redraw(width, height);
	    glFlush();
	    glXSwapBuffers(dpy, win);
	    needsRedraw = False;
	}

	checkErrors();
    }
}
