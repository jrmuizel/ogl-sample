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
/* vertarr.c
** This program demonstrates using the vertex array extension.
** it uses vertex array in immediate mode, and in display lists.
** "a" toggles vertex arrays
** "d" toggles display lists
** "m" toggles between compile and compile and execute mode
**      (works only if display lists are on)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>


#define COLOR_OFFSET_1 16
#define COLOR_OFFSET_2 32

#define CHECK_ERROR \
	err = glGetError(); \
	if (err) { \
	    printf("Error %d at line %d\n", err, __LINE__); \
	}

static GLubyte texture [768];

static GLint verts[] = {
    10, 10, 0,
    200, 100, 0,
    50, 200, 0
};
static GLfloat colors[] = {
    1.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.5, 0.2, 0.8
};
static GLfloat cv[] = { 
    1.0, 1.0, 0.0,
    430, 30,
    1.0, 0.0, 1.0,
    480, 300,
    1.0, 1.0, 1.0,
    380, 200
};
static GLshort tv[] = {
    0, 0,
    5, 400,
    1, 0,
    100, 400,
    1, 1,
    100, 495,
    0, 1,
    5, 495
};
static GLfloat verts2[] = {
    350.0, 450.0, 1.0, 1.0,
    350.0, 350.0, 1.0, 1.0,
    400.0, 325.0, 1.0, 1.0,
    450.0, 350.0, 1.0, 1.0,
    450.0, 450.0, 1.0, 1.0,
    400.0, 475.0, 1.0, 1.0
};
static GLfloat norms[] = {
    0, 0, 1, 
    0, 0, 1, 
    0, 0, 1, 
    0, 0, 1,
    0, 0, -1, 
    0, 0, -1, 
    0, 0, -1, 
    0, 0, -1
};
static GLint verts3[] = { 
    150, 200, 
    250, 200, 
    250, 300, 
    150, 300, 
    250, 200, 
    350, 200, 
    350, 300, 
    250, 300
};
static GLboolean edges[] = {
    GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE
};

GLenum rgb, err;
int drawArray = 0;
int dlist = 0; /* 0 = immediate mode, ~0 = display list */
GLenum dlMode = GL_COMPILE;


static void SetUpColorMap(void)
{
    long i;

    for (i = 0; i < 16; i++) {
	glutSetColor(i+COLOR_OFFSET_1, 0.0, 0.0, i/15.0);
	glutSetColor(i+COLOR_OFFSET_2, 0.0, i/15.0, 0.0);
    }
}

static GLboolean QueryExtension(char *extName)
{
    /*
    ** Search for extName in the extensions string. Use of strstr()
    ** is not sufficient because extension names can be prefixes of
    ** other extension names. Could use strtok() but the constant
    ** string returned by glGetString can be in read-only memory.
    */
    char *p = (char *)glGetString(GL_EXTENSIONS);
    char *end = p + strlen(p);
    while (p < end) {
	int n = strcspn(p, " ");
	if ((strlen(extName) == n) && (strncmp(extName, p, n) == 0)) {
	    return GL_TRUE;
	}
	p += (n + 1);
    }
    return GL_FALSE;
}

static void Init(void)
{
    int i;

    if (!rgb) {
	SetUpColorMap();
    }

    /* see if Vertex array extension is supported */
    if (!QueryExtension("GL_EXT_vertex_array")) {
       printf("Couldn't find vertex array extension.\n");
       exit(0);
    }

    glClearColor(0, 0, 0, 0);

    glShadeModel(GL_SMOOTH);

    /* create texture image */
    for (i = 0; i < 192; i+=3) {
       texture[i] = 0xff; texture[i+1] = 0xff; texture[i+2] = 0x00;
    }
    for (; i < 2*192; i+=3) {
       texture[i] = 0xff; texture[i+1] = 0x00; texture[i+2] = 0xff;
    }
    for (; i < 3*192; i+=3) {
       texture[i] = 0x00; texture[i+1] = 0xff; texture[i+2] = 0xff;
    }
    for (; i < 4*192; i+=3) {
       texture[i] = 0x00; texture[i+1] = 0x00; texture[i+2] = 0xff;
    }
}

static void Reshape(int width, int height)
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 500.0, 0.0, 500.0, 0.0, -1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 'a':
	  drawArray = !drawArray;
	  glutPostRedisplay();
	  break;
      case 'd':
	  dlist = !dlist;
	  glutPostRedisplay();
	  break;
      case 'm':
	  if (dlist) {
	    if (dlMode == GL_COMPILE) {
	      dlMode = GL_COMPILE_AND_EXECUTE;
	    } else {
	      dlMode = GL_COMPILE;
	    }
	  }
	  glutPostRedisplay();
	  break;
      case 27:
	exit(0);
    }
}

void TestGets(void)
{
#if GL_EXT_vertex_array
    GLint size, stride, type, count;
    GLvoid *ptr;
    
    glGetIntegerv(GL_VERTEX_ARRAY_SIZE_EXT, &size);
    glGetIntegerv(GL_VERTEX_ARRAY_TYPE_EXT, &type);
    glGetIntegerv(GL_VERTEX_ARRAY_STRIDE_EXT, &stride);
    glGetIntegerv(GL_VERTEX_ARRAY_COUNT_EXT, &count);
    printf("VERTEX_ARRAY size = %d, type = %d, stride = %d, count = %d\n\n",
	   size, type, stride, count);
    CHECK_ERROR

    glGetIntegerv(GL_NORMAL_ARRAY_TYPE_EXT, &type);
    glGetIntegerv(GL_NORMAL_ARRAY_STRIDE_EXT, &stride);
    glGetIntegerv(GL_NORMAL_ARRAY_COUNT_EXT, &count);
    printf("NORMAL_ARRAY type = %d, stride = %d, count = %d\n\n",
	   type, stride, count);
    CHECK_ERROR

    glGetIntegerv(GL_COLOR_ARRAY_SIZE_EXT, &size);
    glGetIntegerv(GL_COLOR_ARRAY_TYPE_EXT, &type);
    glGetIntegerv(GL_COLOR_ARRAY_STRIDE_EXT, &stride);
    glGetIntegerv(GL_COLOR_ARRAY_COUNT_EXT, &count);
    printf("COLOR_ARRAY size = %d, type = %d, stride = %d, count = %d\n\n",
	   size, type, stride, count);
    CHECK_ERROR

    glGetIntegerv(GL_INDEX_ARRAY_TYPE_EXT, &type);
    glGetIntegerv(GL_INDEX_ARRAY_STRIDE_EXT, &stride);
    glGetIntegerv(GL_INDEX_ARRAY_COUNT_EXT, &count);
    printf("INDEX_ARRAY type = %d, stride = %d, count = %d\n\n",
	   type, stride, count);
    CHECK_ERROR

    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_SIZE_EXT, &size);
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_TYPE_EXT, &type);
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_STRIDE_EXT, &stride);
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_COUNT_EXT, &count);
    printf("TEXTURE_COORD_ARRAY size = %d, type = %d, stride = %d, count = %d\n\n", size, type, stride, count);
    CHECK_ERROR

    glGetIntegerv(GL_EDGE_FLAG_ARRAY_STRIDE_EXT, &stride);
    glGetIntegerv(GL_EDGE_FLAG_ARRAY_COUNT_EXT, &count);
    printf("EDGE_FLAG_ARRAY stride = %d, count = %d\n\n",
	   stride, count);
    CHECK_ERROR

    glGetPointervEXT(GL_VERTEX_ARRAY_POINTER_EXT, &ptr);
    printf("VERTEX_ARRAY pointer = %x\n", ptr);
    glGetPointervEXT(GL_NORMAL_ARRAY_POINTER_EXT, &ptr);
    printf("NORMAL_ARRAY pointer = %x\n", ptr);
    glGetPointervEXT(GL_COLOR_ARRAY_POINTER_EXT, &ptr);
    printf("COLOR_ARRAY pointer = %x\n", ptr);
    glGetPointervEXT(GL_INDEX_ARRAY_POINTER_EXT, &ptr);
    printf("INDEX_ARRAY pointer = %x\n", ptr);
    glGetPointervEXT(GL_TEXTURE_COORD_ARRAY_POINTER_EXT, &ptr);
    printf("TEXTURE_COORD_ARRAY pointer = %x\n", ptr);
    glGetPointervEXT(GL_EDGE_FLAG_ARRAY_POINTER_EXT, &ptr);
    printf("EDGE_FLAG_ARRAY pointer = %x\n", ptr);
    CHECK_ERROR;
#endif
}

static void Draw(void)
{
#if GL_EXT_vertex_array
    int i;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);
    glLineWidth(1.0);
    glDisable(GL_VERTEX_ARRAY_EXT);
    glDisable(GL_NORMAL_ARRAY_EXT);
    glDisable(GL_COLOR_ARRAY_EXT);
    glDisable(GL_INDEX_ARRAY_EXT);
    glDisable(GL_TEXTURE_COORD_ARRAY_EXT);
    glDisable(GL_EDGE_FLAG_ARRAY_EXT);

    glClear(GL_COLOR_BUFFER_BIT);

    glVertexPointerEXT(3, GL_INT, 0, 3, verts);
    glColorPointerEXT(3, GL_FLOAT, 0, 3, colors);
    glEnable(GL_VERTEX_ARRAY_EXT);
    glEnable(GL_COLOR_ARRAY_EXT);
    CHECK_ERROR

    if (dlist) {
	glNewList(1, dlMode);
    }
    if (drawArray) {
        glDrawArraysEXT(GL_TRIANGLES, 0, 3);
    } else {
	glBegin(GL_TRIANGLES);
	    for (i = 0; i < 3; i++) {
		glArrayElementEXT(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(1);
    }

    CHECK_ERROR

    glVertexPointerEXT(2, GL_FLOAT, 20, 3, &cv[3]);
    glColorPointerEXT(3, GL_FLOAT, 20, 3, cv);

    if (dlist) {
	glNewList(2, dlMode);
    }
    if (drawArray) {
        glDrawArraysEXT(GL_TRIANGLES, 0, 3);
    } else {
	glBegin(GL_TRIANGLES);
	    for (i = 0; i < 3; i++) {
		glArrayElementEXT(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(2);
    }
    glDisable(GL_COLOR_ARRAY_EXT);

    if (dlist) {
	glDeleteLists(1, 2);
    }

    glVertexPointerEXT(2, GL_SHORT, 8, 4, &tv[2]);
    glTexCoordPointerEXT(2, GL_SHORT, 8, 4, tv);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_VERTEX_ARRAY_EXT);
    glEnable(GL_TEXTURE_COORD_ARRAY_EXT);

    if (dlist) {
	glNewList(3, dlMode);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE,
		 texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if (drawArray) {
	glDrawArraysEXT(GL_POLYGON, 0, 4);
    } else {
	glBegin(GL_POLYGON);
	    for (i = 0; i < 4; i++) {
		glArrayElementEXT(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(3);
    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_COORD_ARRAY_EXT);

    glEnable(GL_EDGE_FLAG_ARRAY_EXT);
    glVertexPointerEXT(4, GL_FLOAT, 0, 6, verts2);
    
    if (dlist) {
	glNewList(4, dlMode);
    }
    glEdgeFlagPointerEXT(0, 6, edges);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glShadeModel(GL_FLAT);
    glLineWidth(4.0);
    glColor3f(1.0, 0.0, 0.0);

    if (drawArray) {
        glDrawArraysEXT(GL_POLYGON, 0, 6);
    } else {
	glBegin(GL_POLYGON);
	    for (i = 0; i < 6; i++) {
		glArrayElementEXT(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(4);
    }

    glDisable(GL_EDGE_FLAG_ARRAY_EXT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    {
	GLfloat back_light_pos[] = {0.0, 0.0, -1.0, 1.0};
	GLfloat back_light_dir[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat amb[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, red);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, blue);
	glLightfv(GL_LIGHT1, GL_POSITION, back_light_pos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, back_light_dir);

    }
    glShadeModel(GL_SMOOTH);
    glVertexPointerEXT(2, GL_INT, 0, 8, verts3);
    glNormalPointerEXT(GL_FLOAT, 0, 8, norms);
    glEnable(GL_NORMAL_ARRAY_EXT);

    if (dlist) {
	glNewList(5, dlMode);
    }
    glBegin(GL_POLYGON);
	for (i = 0; i < 4; i++) {
	    glArrayElementEXT(i);
	}
    glEnd();
    glBegin(GL_POLYGON);
	for (; i < 8; i++) {
	    glArrayElementEXT(i);
	}
    glEnd();

    if (dlist) {
	glEndList();
	glCallList(5);
    }
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMAL_ARRAY_EXT);

    {
	static GLfloat stripVertex[128*2][2];
	static GLfloat stripColor[128*2][3];
	int numSteps = 128;
	int numSectors = 3;

	float stepSize = 2 * M_PI / (numSteps - 1);
	float sectorSize = 2 * M_PI / numSectors;
	float innerRadius = 40;
	float outerRadius = 80;
	float xCenter = 225;
	float yCenter = 400;

	for (i = 0; i < numSteps*2; i += 2) {
	    float angle = i/2 * stepSize;
	    float x = cos(angle);
	    float y = sin(angle);
	    float sector = angle / sectorSize;
	    float fade = sector - (int)sector;
	    float r, g, b;
	    stripVertex[i][0] = innerRadius * x + xCenter;
	    stripVertex[i][1] = innerRadius * y + yCenter;
	    stripVertex[i+1][0] = outerRadius * x + xCenter;
	    stripVertex[i+1][1] = outerRadius * y + yCenter;
	    switch ((int) sector) {
	      case 0: case 3:
		/* red fade to green */
		r = 1.00 * (1 - fade) + 0.25 * fade;
		g = 0.25 * (1 - fade) + 1.00 * fade;
		b = 0.25 * (1 - fade) + 0.25 * fade;
		break;
	      case 1:
		/* green fade to blue */
		r = 0.25 * (1 - fade) + 0.25 * fade;
		g = 1.00 * (1 - fade) + 0.25 * fade;
		b = 0.25 * (1 - fade) + 1.00 * fade;
		break;
	      case 2:
		/* blue fade to red */
		r = 0.25 * (1 - fade) + 1.00 * fade;
		g = 0.25 * (1 - fade) + 0.25 * fade;
		b = 1.00 * (1 - fade) + 0.25 * fade;
		break;
	    }
	    stripColor[i][0] = stripColor[i+1][0] = r;
	    stripColor[i][1] = stripColor[i+1][1] = g;
	    stripColor[i][2] = stripColor[i+1][2] = b;
	}

	glVertexPointerEXT(2, GL_FLOAT, 0, numSteps*2, stripVertex);
	glColorPointerEXT(3, GL_FLOAT, 0, numSteps*2, stripColor);
	glEnable(GL_COLOR_ARRAY_EXT);
	if (dlist) {
	    glNewList(6, dlMode);
	}
	if (drawArray) {
	    glDrawArraysEXT(GL_TRIANGLE_STRIP, 0, numSteps*2);
	} else {
	    glBegin(GL_TRIANGLE_STRIP);
	    for (i = 0; i < numSteps*2; i += 2) {
		glArrayElementEXT(i);
		glArrayElementEXT(i+1);
	    }
	    glEnd();
	}
	if (dlist) {
	    glEndList();
	    glCallList(6);
	}
	glDisable(GL_COLOR_ARRAY_EXT);
    }

    CHECK_ERROR
    glFlush();
    TestGets();
#endif
}

static void Args(int argc, char **argv)
{
    GLint i;

    rgb = GL_TRUE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-ci") == 0) {
	    rgb = GL_FALSE;
	} else if (strcmp(argv[i], "-rgb") == 0) {
	    rgb = GL_TRUE;
	}
    }
}

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    type = GLUT_SINGLE;
    type |= (rgb) ? GLUT_RGB : GLUT_INDEX;
    glutInitDisplayMode(type);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutMainLoop();
}

