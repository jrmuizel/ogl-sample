/****************************************************************************
Copyright 1995, 1996 by Silicon Graphics Inc, Mountain View, California.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Silicon Graphics not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL SILICON GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

****************************************************************************/

/*
 * Derived from code written by Kurt Akeley, November 1992
 *
 *	Uses PolygonOffset to draw hidden-line images.  PolygonOffset
 *	    shifts the z values of polygons an amount that is
 *	    proportional to their slope in screen z.  This keeps
 *	    the lines, which are drawn without displacement, from
 *	    interacting with their respective polygons, and
 *	    thus eliminates line dropouts.
 *
 *	The left image shows an ordinary wireframe image.
 *	The center image shows a hidden-line image without
 *	    PolygonOffset.
 *	The right image shows a hidden-line image using
 *	    PolygonOffset to reduce artifacts.
 *
 *	Drag with a mouse button pressed to rotate the models.
 *	Press the escape key to exit.
 *
 *      'l' will toggle between pulling the line polygon away from the fill 
 *      polygon and pushing the fill polygon away from the line polygon.
 */

#include <GL/glx.h>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXQUAD 6

typedef float Vertex[3];

typedef Vertex Quad[4];

/* data to define the six faces of a unit cube */
Quad quads[MAXQUAD] = {
    0,0,0, 1,0,0, 1,1,0, 0,1,0,
    0,0,1, 1,0,1, 1,1,1, 0,1,1,
    0,0,0, 1,0,0, 1,0,1, 0,0,1,
    0,1,0, 1,1,0, 1,1,1, 0,1,1,
    0,0,0, 0,0,1, 0,1,1, 0,1,0,
    1,0,0, 1,0,1, 1,1,1, 1,1,0
};

#define WIREFRAME	0
#define HIDDEN_LINE	1
#define HIDDEN_LINE_VIA_POLYGON_OFFSET 2

static void error(const char* prog, const char* msg);
static void cubes(int mx, int my, int mode);
static void fill(Quad quad);
static void outline(Quad quad);
static void draw_hidden(Quad quad, int mode);
static void process_input(Display *dpy, Window win);

static int attributeList[] = { GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 1, None };

static int dimension = 3;
static int isPullLineAwayFromFill = 0;

main(int argc, char** argv) {
    Display *dpy;
    XVisualInfo *vi;
    XSetWindowAttributes swa;
    Window win;
    GLXContext cx;

    dpy = XOpenDisplay(0);
    if (!dpy) error(argv[0], "can't open display");

    vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);
    if (!vi) error(argv[0], "no suitable visual");

    cx = glXCreateContext(dpy, vi, 0, GL_TRUE);

    swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
                                   vi->visual, AllocNone);

    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask |
	ButtonPressMask | ButtonMotionMask;
    win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 900, 300,
			0, vi->depth, InputOutput, vi->visual,
			CWBorderPixel|CWColormap|CWEventMask, &swa);
    XStoreName(dpy, win, "hiddenline");
    XMapWindow(dpy, win);

    glXMakeCurrent(dpy, win, cx);

    /* set up viewing parameters */
    glMatrixMode(GL_PROJECTION);
    gluPerspective(20, 1, 0.1, 20);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0, 0, -15);

    /* set other relevant state information */
    glEnable(GL_DEPTH_TEST);

    /* process events until the user presses ESC */
    while (1) process_input(dpy, win);
}

static void
draw_scene(int mx, int my) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(-1.7, 0.0, 0.0);
    cubes(mx, my, WIREFRAME);
    glPopMatrix();

    glPushMatrix();
    cubes(mx, my, HIDDEN_LINE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.7, 0.0, 0.0);
    /*glEnable(GL_POLYGON_OFFSET_FILL);*/
    cubes(mx, my, HIDDEN_LINE_VIA_POLYGON_OFFSET);
    /*glDisable(GL_POLYGON_OFFSET_FILL);*/
    glPopMatrix();
}


static void
cubes(int mx, int my, int mode) {
    int x, y, z, i;

    /* track the mouse */
    glRotatef(mx / 2.0, 0, 1, 0);
    glRotatef(my / 2.0, 1, 0, 0);

    /* draw the lines as hidden polygons */
    glTranslatef(-0.5, -0.5, -0.5);
    glScalef(1.0/dimension, 1.0/dimension, 1.0/dimension);
    for (z = 0; z < dimension; z++) {
	for (y = 0; y < dimension; y++) {
	    for (x = 0; x < dimension; x++) {
		glPushMatrix();
		glTranslatef(x, y, z);
		glScalef(0.8, 0.8, 0.8);
		for (i = 0; i < MAXQUAD; i++)
		    draw_hidden(quads[i], mode);
		glPopMatrix();
	    }
	}
    }
}

static void
fill(Quad quad) {
    /* draw a filled polygon */
    glBegin(GL_QUADS);
    glVertex3fv(quad[0]);
    glVertex3fv(quad[1]);
    glVertex3fv(quad[2]);
    glVertex3fv(quad[3]);
    glEnd();
}

static void
outline(Quad quad) {
    /* draw an outlined polygon */
    glBegin(GL_LINE_LOOP);
    glVertex3fv(quad[0]);
    glVertex3fv(quad[1]);
    glVertex3fv(quad[2]);
    glVertex3fv(quad[3]);
    glEnd();
}

static void
draw_hidden(Quad quad, int mode) {
    switch (mode) {
    case WIREFRAME:
       glColor3f(1, 1, 1);
       outline(quad);	/* line_loop doesn't interact with polygonmode */
       break;
    case HIDDEN_LINE:
       glColor3f(1, 1, 1);
       outline(quad);	/* line_loop doesn't interact with polygonmode */

       glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
       glDisable(GL_POLYGON_OFFSET_FILL);
       glDisable(GL_POLYGON_OFFSET_LINE);

       glColor3f(0, 0, 0);
       fill(quad);
       break;
    case HIDDEN_LINE_VIA_POLYGON_OFFSET:
       if (isPullLineAwayFromFill) {
	  /* pull line polygon away from fill polygon */
	  glPolygonOffset(-1.0,0.0);
	  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	  glDisable(GL_POLYGON_OFFSET_FILL);
	  glEnable(GL_POLYGON_OFFSET_LINE);

	  glColor3f(1, 1, 1);
	  fill(quad);

          /* draw fill polygon without displacement */
	  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	  glDisable(GL_POLYGON_OFFSET_FILL);
	  glDisable(GL_POLYGON_OFFSET_LINE);

	  glColor3f(0, 0, 0);
	  fill(quad);
       }
       else { 
          /* draw line polygon without displacement */
	  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	  glDisable(GL_POLYGON_OFFSET_FILL);
	  glDisable(GL_POLYGON_OFFSET_LINE);

	  glColor3f(1, 1, 1);
	  /* On machines (or soft) that support polygon offset
	   * line/point, we should use fill(quad). However, EXPRESS
	   * doesn't support polygon offset line/point, so to work on
	   * both soft & EXPRESS, we use outline(quad).  */
	  outline(quad);

	  /* push fill polygon away from line polygon */
	  glPolygonOffset(1.0,0.0);
	  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	  glEnable(GL_POLYGON_OFFSET_FILL);
	  glDisable(GL_POLYGON_OFFSET_LINE);

	  glColor3f(0, 0, 0);
	  fill(quad);
       }
       break;
    } 
}

static void
process_input(Display *dpy, Window win) {
    XEvent event;
    static int prevx, prevy;
    static int deltax = 90, deltay = 40;

    do {
	char buf[31];
	KeySym keysym;

	XNextEvent(dpy, &event);
	switch(event.type) {
	case Expose:
	    break;
	case ConfigureNotify: {
	    /* this approach preserves a 1:1 viewport aspect ratio */
	    int vX, vY, vW, vH;
	    int eW = event.xconfigure.width, eH = event.xconfigure.height;
	    if (eW >= eH) {
		vX = 0;
		vY = (eH - eW) >> 1;
		vW = vH = eW;
	    } else {
		vX = (eW - eH) >> 1;
		vY = 0;
		vW = vH = eH;
	    }
	    glViewport(vX, vY, vW, vH);
	    }
	    break;
	case KeyPress:
	    (void) XLookupString(&event.xkey, buf, sizeof(buf), &keysym, NULL);
	    switch (keysym) {
	    case XK_Escape:
		exit(EXIT_SUCCESS);
            case XK_l: case XK_L:
	        isPullLineAwayFromFill= !isPullLineAwayFromFill;
		if (isPullLineAwayFromFill) 
		   printf("Pull line polygon away from fill polygon on.\n");
		else printf("Pull line polygon away from fill polygon off.\n");
	        break;
	    default:
		break;
	    }
	case ButtonPress:
	    prevx = event.xbutton.x;
	    prevy = event.xbutton.y;
	    break;
	case MotionNotify:
	    deltax += (event.xbutton.x - prevx); prevx = event.xbutton.x;
	    deltay += (event.xbutton.y - prevy); prevy = event.xbutton.y;
	    break;
	default:
	    break;
	}
    } while (XPending(dpy));

    draw_scene(deltax, deltay);
    glXSwapBuffers(dpy, win);
}

static void
error(const char *prog, const char *msg) {
    fprintf(stderr, "%s: %s\n", prog, msg);
    exit(EXIT_FAILURE);
}
