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
** This program is a simplistic example of how glXImportContextEXT
** can be used.  A context1 is created on dpy1, context2 is an imported
** version of context1 on dpy2.  Drawing is done using context1, and
** color is set using context2.
**
** $Header: //depot/main/gfx/samples/glx/importcx.c#3 $
*/

#define NeedFunctionPrototypes 1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** Convenience routines for creating and configuring windows.
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

int attribList[] = {
    GLX_RGBA, 
    GLX_DEPTH_SIZE, 1,
    None,
};

int attribList2[] = {
    GLX_RGBA,
    GLX_DOUBLEBUFFER,
    GLX_DEPTH_SIZE, 1,
    None,
};

Window SetupWindow(Display *dpy, int width, int height, 
		   unsigned long background, XVisualInfo **pVisualInfo)
{
    int scr;
    unsigned long mask;
    Visual *vis;
    XVisualInfo *visualInfo;
    Colormap cmap;
    Window win, parent;
    XSetWindowAttributes wa;
    
    scr = DefaultScreen(dpy);
    parent = RootWindow(dpy, scr);

    visualInfo = glXChooseVisual(dpy, scr, attribList);
    if (!visualInfo) {
	visualInfo = glXChooseVisual(dpy, scr, attribList2);
    }
    if (!visualInfo) {
	fprintf(stderr, "Can't find suitable visual\n");
	return (Window) 0;
    }
    vis = visualInfo->visual;

    cmap = XCreateColormap(dpy, RootWindow(dpy, scr), vis, AllocNone);

    wa.colormap = cmap;
    wa.background_pixel = background;
    wa.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
    mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;
    
    /*
    ** Create window
    */
    win = XCreateWindow(dpy, parent, 0, 0, width, height, 0, visualInfo->depth,
			InputOutput, vis, mask, &wa);
    XMapWindow(dpy, win);

    *pVisualInfo = visualInfo;
    return win;
}

#define WIDTH 200
#define HEIGHT 200

static int screen = 0;
static Window win;
static Display *dpy1;
static Display *dpy2;
static XVisualInfo *vis;

static GLXContext dpy1Context, dpy2Context;
static GLXContextID dpy1ContextId;

void setColor(GLfloat r, GLfloat g, GLfloat b) {
    if (!glXMakeCurrent(dpy2, win, dpy2Context)) {
	printf("glXMakeCurrent on dpy2 context failed\n");
	return;
    }
    glColor3f(r, g, b);
}

void init(void)
{
    if (!glXMakeCurrent(dpy1, win, dpy1Context)) {
	printf("glXMakeCurrent on dpy1 context failed\n");
	return;
    }
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);

    glViewport(10, 10, WIDTH-20, HEIGHT-20);
    glScissor(10, 10, WIDTH-20, HEIGHT-20);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_BLEND);

    glDrawBuffer(GL_FRONT);
}

void draw(void)
{
    setColor(0.0, 1.0, 0.0);
    if (!glXMakeCurrent(dpy1, win, dpy1Context)) {
	printf("glXMakeCurrent on dpy1 context failed\n");
	return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
	glVertex3f( 0.9, -0.9, -30.0);
	glVertex3f( 0.9,  0.9, -30.0);
	glVertex3f(-0.9,  0.0, -30.0);
    glEnd();
    glFlush();

    setColor(0.0, 0.0, 1.0);
    if (!glXMakeCurrent(dpy1, win, dpy1Context)) {
	printf("glXMakeCurrent on dpy1 context failed\n");
    }

    glBegin(GL_TRIANGLES);
	glVertex3f(-0.9, -0.9, -40.0);
	glVertex3f(-0.9,  0.9, -40.0);
	glVertex3f( 0.9,  0.0, -25.0);
    glEnd();
    glFlush();
}

static GLboolean queryExtension(char *extName)
{
    /*
    ** Search for extName in the extensions string.  Use of strstr()
    ** is not sufficient because extension names can be prefixes of
    ** other extension names.  Could use strtok() but the constant
    ** string returned by glGetString can be in read-only memory.
    */
    char *p;
    char *end;
    int extNameLen;

    extNameLen = strlen(extName);

    p = (char *)glXQueryExtensionsString(dpy1, screen);
    if (NULL == p) return GL_FALSE;

    end = p + strlen(p);

    while (p < end) {
	int n = strcspn(p, " ");
	if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
	    return GL_TRUE;
	}
	p += (n + 1);
    }
    return GL_FALSE;
}

GLboolean CheckDpy(Display *dpy) 
{
    int major, minor, dontcare;
    int screen_num;
    const char *string;

    /* does the server know about OpenGL & GLX? */
    if(!XQueryExtension(dpy, "GLX", &dontcare, &dontcare, &dontcare)) {
      fprintf(stderr,"This system doesn't appear to support OpenGL\n");
      return GL_FALSE;
    }
    
    /* find the glx version */
    if(glXQueryVersion(dpy, &major, &minor))
      printf("GLX Version: %d.%d\n", major, minor);
    else {
      fprintf(stderr, "Error: glXQueryVersion() failed.\n");
      return GL_FALSE;
    }

    /* get screen number */
    screen_num = DefaultScreen(dpy);

    /* This #ifdef isn't redundant. It keeps the build from breaking
    ** if you are building on a machine that has an old (1.0) version
    ** of glx.
    **
    ** This program could still be *run* on a machine that has an old 
    ** version of glx, even if it was *compiled* on a version that has
    ** a new version.
    **
    ** If compiled on a system with an old version of glx, then it will 
    ** never recognize glx extensions, since that code would have been
    ** #ifdef'ed out.
    */
#ifdef GLX_VERSION_1_1

    /*
    ** This test guarantees that glx, on the display you are inquiring,
    ** suppports glXQueryExtensionsString().
    */
    if(minor > 0 || major > 1)
      string = glXQueryExtensionsString(dpy, screen_num);
    else
      string = "";
  
    if(string)
      printf("GLX Extensions (client & server): %s\n",
	     string);
    else {
      fprintf(stderr, "Error: glXQueryExtensionsString() failed.\n");
      return GL_FALSE;
    }

#else
    return -1;
#endif

    return GL_TRUE;
}

int main(int argc, char **argv)
{
    char buf[100];
    XEvent event;
    KeySym ks;
    GLboolean doQuery = GL_TRUE;
    

    dpy1 = XOpenDisplay(0);
    if (!dpy1) {
	fprintf(stderr, "Can't connect to display\n");
	return -1;
    }
    if (!CheckDpy(dpy1)) return -1;

    dpy2 = XOpenDisplay(0);
    if (!dpy2) {
	fprintf(stderr, "Can't connect to display\n");
	return -1;
    }
    if (!CheckDpy(dpy2)) return -1;

    win = SetupWindow(dpy1, WIDTH, HEIGHT, BlackPixel(dpy1,screen), &vis);

    dpy1Context = glXCreateContext(dpy1, vis, None, GL_FALSE);
    XSync(dpy1, False); /* This forces the CreateContext to go to the server */

#ifdef GLX_EXT_import_context
    if (doQuery && !queryExtension("GLX_EXT_import_context")) {
	fprintf(stderr, "Can't find import_context extension.\n");
	return -1;
    }
#endif

#ifdef GLX_EXT_import_context
    dpy1ContextId = glXGetContextIDEXT(dpy1Context);
    dpy2Context = glXImportContextEXT(dpy2, dpy1ContextId);
#endif
    if (!dpy2Context) {
	fprintf(stderr, "Import context failed.\n");
	return -1;
    }


    if (!glXMakeCurrent(dpy1, win, dpy1Context)) {
	printf("glXMakeCurrent on dpy1 context failed\n");
	return -1;
    }

    init();

    /*
    ** Redraw when you hit 'd'.
    */
    {
	GLboolean cont = GL_TRUE;
	while (cont) {
	    XNextEvent(dpy1, &event);
	    switch (event.type) {
	    case Expose:
		draw();
		break;
	    case KeyPress:
		XLookupString(&event.xkey, buf, sizeof(buf), &ks, 0);
		switch (ks) {
		case XK_g:
		case XK_G:
		    {
			VisualID vid;
			int screen;
			GLXContextID shareList;

#ifdef GLX_EXT_import_context
			glXQueryContextInfoEXT(dpy2, dpy2Context, 
					    GLX_VISUAL_ID_EXT, (int *)&vid);
			glXQueryContextInfoEXT(dpy2, dpy2Context, 
					    GLX_SCREEN_EXT, &screen);
			glXQueryContextInfoEXT(dpy2, dpy2Context, 
				    GLX_SHARE_CONTEXT_EXT, (int *)&shareList);
			fprintf(stdout, 
				"vid = %d, screen = %d, shareList = %d.\n", 
				(int)vid, screen, (int)shareList);
#endif
		    }
		    break;
		case XK_c:
		case XK_C:
		    break;
		case XK_d:
		case XK_D:
		    draw();
		    break;
		case XK_Escape:
		    glXMakeCurrent(dpy1, None, NULL);
		    glXMakeCurrent(dpy2, None, NULL);
		    glXDestroyContext(dpy1, dpy1Context);
#ifdef GLX_EXT_import_context
		    glXFreeContextEXT(dpy2, dpy2Context);
#endif
		    XCloseDisplay(dpy1);
		    XCloseDisplay(dpy2);
		    cont = GL_FALSE;
		    break;
		}
	    }
	}
    }
}
