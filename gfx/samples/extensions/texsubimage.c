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
** cc -o texsubimage texsubimage.c -lglut -lGLU -lGL -lXext -lX11 -lrgb
**
** Demonstrates how to replace a portion of a texture using glTexSubImage.
**
** Requires the GL_EXT_subtexture extension.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/rgb.h>

#if defined(GL_EXT_subtexture)

char *filename = NULL;
int winX = 0, winY = 0;
int winWidth = 300, winHeight = 300;
float curX = 0.0, curY = 0.0;
GLboolean doubleBuffered = GL_FALSE;
int *visualAttr = NULL;
GLboolean useLinear = GL_FALSE;
GLboolean useDisplayList = GL_FALSE;
GLboolean buttonPressed = GL_FALSE;
GLboolean needsRedraw = GL_TRUE;
int baseWidth = 256, baseHeight = 256;
GLubyte *baseImage = NULL;
int imageWidth = 0, imageHeight = 0;
GLubyte *imageData = NULL;

void
imgLoad(char *inFilename, int *outW, int *outH, GLubyte **outImg)
{
    RGBImageRec *rgbimage;

    if ((rgbimage = rgbImageLoad(inFilename)) == NULL) {
	fprintf(stderr, "unable to read %s\n", inFilename);
	exit(EXIT_FAILURE);
    }

    *outW = rgbimage->sizeX;
    *outH = rgbimage->sizeY;
    *outImg = rgbimage->data;
}

void
checkErrors(void)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
	fprintf(stderr, "Error: %s\n", (char *) gluErrorString(error));
    }
}

void
keyboard(unsigned char c, int x, int y)
{
    switch (c) {
      case 'l':
	useLinear = !useLinear;
	break;
      case 'd':
	useDisplayList = !useDisplayList;
	break;
      default:
	break;
    }
    glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	buttonPressed = GL_TRUE;
	curX = (float) x / winWidth;
	curY = (float) (winHeight - y) / winHeight;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
	buttonPressed = GL_FALSE;
	glutPostRedisplay();
    }
}

void
motion(int x, int y)
{
    if (buttonPressed) {
	curX = (float) x / winWidth;
	curY = (float) (winHeight - y) / winHeight;
	glutPostRedisplay();
    }
}

void
reshape(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, winWidth, 0, winHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, winWidth, winHeight);
    glutPostRedisplay();
}

void
display(void)
{
    int subX = (int) (curX * baseWidth);
    int subY = (int) (curY * baseHeight);
    int subWidth = imageWidth;
    int subHeight = imageHeight;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (useLinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    if (baseImage == NULL) {
	int i, j;

	baseImage = (GLubyte *) calloc(baseHeight, 4*baseWidth);

	for (i=0; i<baseHeight; ++i) {
	    GLubyte *p = baseImage + 4*baseWidth*i;
	    for (j=0; j<baseHeight; ++j) {
		if ((i ^ j) & 4) {
		    p[0] = 0x10;
		    p[1] = 0x10;
		    p[2] = 0x10;
		    p[3] = 0x10;
		} else {
		    p[0] = 0x20;
		    p[1] = 0x20;
		    p[2] = 0x20;
		    p[3] = 0x20;
		}
		p += 4;
	    }
	}
    }

    if (useDisplayList) {
	glNewList(1, GL_COMPILE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		 baseWidth, baseHeight, 0,
		 GL_RGB, GL_UNSIGNED_BYTE, baseImage);

    /* clip subimage to base image size */
    glPixelStorei(GL_UNPACK_ROW_LENGTH, subWidth);
    if (subX < 0) {
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, -subX);
	subWidth += subX;
	subX = 0;
    }
    if (subX > baseWidth) {
	subX = 0;
	subWidth = 0;
    }
    if (subX + subWidth > baseWidth) {
	subWidth -= (subX + subWidth) - baseWidth;
    }
    if (subWidth < 0) {
	subWidth = 0;
    }

    if (subY < 0) {
	glPixelStorei(GL_UNPACK_SKIP_ROWS, -subY);
	subHeight += subY;
	subY = 0;
    }
    if (subY > baseHeight) {
	subY = 0;
	subHeight = 0;
    }
    if (subY + subHeight > baseHeight) {
	subHeight -= (subY + subHeight) - baseHeight;
    }
    if (subHeight < 0) {
	subHeight = 0;
    }

    /* load subimage over base image */
    glTexSubImage2DEXT(GL_TEXTURE_2D, 0,
		       subX, subY, subWidth, subHeight,
		       GL_RGB, GL_UNSIGNED_BYTE, imageData);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    if (useDisplayList) {
	glEndList();
	glCallList(1);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    /* draw a textured polygon the size of the window */
    glEnable(GL_TEXTURE_2D);

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex2i(0, 0);
    glTexCoord2f(1.0, 0.0);
    glVertex2i(winWidth-1, 0);
    glTexCoord2f(1.0, 1.0);
    glVertex2i(winWidth-1, winHeight-1);
    glTexCoord2f(0.0, 1.0);
    glVertex2i(0, winHeight-1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    if (doubleBuffered) {
	glutSwapBuffers();
    } else {
	glFlush();
    }

    checkErrors();
}

void
usage(int argc, char **argv)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [ options ] file\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -sb    single buffered\n");
    fprintf(stderr, "    -db    double buffered\n");
    fprintf(stderr, "\n");
}

int
main(int argc, char **argv)
{
    char *name = "TexSubImage Test";
    int i;

    for (i=1; i<argc; ++i) {
	if (strcmp("-sb", argv[i]) == 0) {
	    doubleBuffered = GL_FALSE;

	} else if (strcmp("-db", argv[i]) == 0) {
	    doubleBuffered = GL_TRUE;

	} else if (i==argc-1 && argv[i][0] != '-') {
	    filename = argv[i];

	} else {
	    usage(argc, argv);
	    exit(EXIT_FAILURE);
	}
    }

    if (filename == NULL) {
	usage(argc, argv);
	exit(EXIT_FAILURE);
    }

    imgLoad(filename, &imageWidth, &imageHeight, &imageData);

    if (doubleBuffered) {
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    } else {
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    }

    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow(name);

    if (!glutExtensionSupported("GL_EXT_subtexture")) {
	fprintf(stderr, "missing extension: GL_EXT_subtexture\n");
	exit(EXIT_FAILURE);
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
}

#else

int main(int argc, char **argv)
{

    printf("Couldn't find GL_EXT_subtexture extension.\n");
}

#endif
