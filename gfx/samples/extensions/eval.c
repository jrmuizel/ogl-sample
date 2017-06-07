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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>


#define VORDER 10
#define CORDER 10
#define TORDER 3

#define VMAJOR_ORDER 2
#define VMINOR_ORDER 3

#define CMAJOR_ORDER 2
#define CMINOR_ORDER 2

#define TMAJOR_ORDER 2
#define TMINOR_ORDER 2

#define VDIM 4
#define CDIM 4
#define TDIM 2

#define ONE_D 1
#define TWO_D 2

#define EVAL 3
#define MESH 4


GLenum doubleBuffer;

float rotX = 0.0, rotY = 0.0, translateZ = -1.0;

GLenum arrayType = ONE_D;
GLenum colorType = GL_FALSE;
GLenum textureType = GL_FALSE;
GLenum polygonFilled = GL_FALSE;
GLenum lighting = GL_FALSE;
GLenum mapPoint = GL_FALSE;
GLenum mapType = EVAL;

double point1[10*4] = {
    -0.5, 0.0, 0.0, 1.0,
    -0.4, 0.5, 0.0, 1.0,
    -0.3,-0.5, 0.0, 1.0,
    -0.2, 0.5, 0.0, 1.0,
    -0.1,-0.5, 0.0, 1.0,
    0.0, 0.5, 0.0, 1.0,
    0.1,-0.5, 0.0, 1.0,
    0.2, 0.5, 0.0, 1.0,
    0.3,-0.5, 0.0, 1.0,
    0.4, 0.0, 0.0, 1.0,
};
double cpoint1[10*4] = {
    0.0, 0.0, 1.0, 1.0,
    0.3, 0.0, 0.7, 1.0,
    0.6, 0.0, 0.3, 1.0,
    1.0, 0.0, 0.0, 1.0,
    1.0, 0.3, 0.0, 1.0,
    1.0, 0.6, 0.0, 1.0,
    1.0, 1.0, 0.0, 1.0,
    1.0, 1.0, 0.5, 1.0,
    1.0, 1.0, 1.0, 1.0,
};
double tpoint1[11*4] = {
    0.0, 0.0, 0.0, 1.0,
    0.0, 0.1, 0.0, 1.0,
    0.0, 0.2, 0.0, 1.0,
    0.0, 0.3, 0.0, 1.0,
    0.0, 0.4, 0.0, 1.0,
    0.0, 0.5, 0.0, 1.0,
    0.0, 0.6, 0.0, 1.0,
    0.0, 0.7, 0.0, 1.0,
    0.0, 0.8, 0.0, 1.0,
    0.0, 0.9, 0.0, 1.0,
};
double point2[2*3*4] = {
    -0.5, -0.5,  0.5, 1.0, 
     0.0,  1.0,  0.5, 1.0, 
     0.5, -0.5,  0.5, 1.0,
    -0.5,  0.5, -0.5, 1.0, 
     0.0, -1.0, -0.5, 1.0, 
     0.5,  0.5, -0.5, 1.0,
};
double cpoint2[2*2*4] = {
    0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0,
    0.0, 1.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0,
};
double tpoint2[2*2*2] = {
    0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 1.0, 1.0, 
};
float textureImage[4*2*4] =  {
    1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 
    1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 
};


static void Init(void)
{
    static float ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float diffuse[] = {1.0, 1.0, 1.0, 1.0};
    static float position[] = {0.0, 0.0, -150.0, 0.0};
    static float front_mat_shininess[] = {30.0};
    static float front_mat_specular[] = {0.7, 0.7, 0.7, 1.0};
    static float front_mat_diffuse[] = {1.0, 0.2, 1.0, 1.0};
    static float back_mat_shininess[] = {50.0};
    static float back_mat_specular[] = {0.5, 0.5, 0.2, 1.0};
    static float back_mat_diffuse[] = {1.0, 1.0, 0.2, 1.0};
    static float lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
    static float lmodel_twoside[] = {GL_TRUE};
    static float decal[] = {GL_DECAL};
    static float modulate[] = {GL_MODULATE};
    static float repeat[] = {GL_REPEAT};
    static float clamp[] = {GL_CLAMP};
    static float nr[] = {GL_NEAREST};
    static float ln[] = {GL_LINEAR};

    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);

    glMap1d(GL_MAP1_VERTEX_4, 0.0, 1.0, VDIM, VORDER, point1);
    glMap1d(GL_MAP1_COLOR_4, 0.0, 1.0, CDIM, CORDER, cpoint1);

    glMap2d(GL_MAP2_VERTEX_4, 0.0, 1.0, VMINOR_ORDER*VDIM, VMAJOR_ORDER, 0.0,
	    1.0, VDIM, VMINOR_ORDER, point2);
    glMap2d(GL_MAP2_COLOR_4, 0.0, 1.0, CMINOR_ORDER*CDIM, CMAJOR_ORDER, 0.0,
	    1.0, CDIM, CMINOR_ORDER, cpoint2);
    glMap2d(GL_MAP2_TEXTURE_COORD_2, 0.0, 1.0, TMINOR_ORDER*TDIM,
	    TMAJOR_ORDER, 0.0, 1.0, TDIM, TMINOR_ORDER, tpoint2);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, modulate);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nr);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nr);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 2, 4, 0, GL_RGBA, GL_FLOAT,
		 (GLvoid *)textureImage);
}

static void DrawPoints1(void)
{
    GLint i;

    glColor3f(0.0, 1.0, 0.0);
    glPointSize(2);
    glBegin(GL_POINTS);
	for (i = 0; i < VORDER; i++) {
	    glVertex4dv(&point1[i*4]);
	}
    glEnd();
}

static void DrawPoints2(void)
{
    GLint i, j;

    glColor3f(1.0, 0.0, 1.0);
    glPointSize(2);
    glBegin(GL_POINTS);
	for (i = 0; i < VMAJOR_ORDER; i++) {
	    for (j = 0; j < VMINOR_ORDER; j++) {
		glVertex4dv(&point2[i*4*VMINOR_ORDER+j*4]);
	    }
	}
    glEnd();
}

static void DrawMapEval1(float du)
{
    float u;

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
	for (u = 0.0; u < 1.0; u += du) {
	    glEvalCoord1d(u);
	}
	glEvalCoord1d(1.0);
    glEnd();
}

static void DrawMapEval2(float du, float dv)
{
    float u, v, tmp;

    glColor3f(1.0, 0.0, 0.0);
    for (v = 0.0; v < 1.0; v += dv) {
	glBegin(GL_QUAD_STRIP);
	    for (u = 0.0; u <= 1.0; u += du) {
		glEvalCoord2d(u,v);
		tmp = (v + dv < 1.0) ? (v + dv) : 1.0;
		glEvalCoord2d(u, tmp);
	    }
	    glEvalCoord2d(1.0, v);
	    glEvalCoord2d(1.0, v+dv);
	glEnd();
    }
}

static void RenderEval(void)
{

    if (colorType) {
	glEnable(GL_MAP1_COLOR_4);
	glEnable(GL_MAP2_COLOR_4);
    } else {
	glDisable(GL_MAP1_COLOR_4);
	glDisable(GL_MAP2_COLOR_4);
    }

    if (textureType) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
    } else {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_MAP2_TEXTURE_COORD_2);
    }

    if (polygonFilled) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glShadeModel(GL_SMOOTH);

    switch (mapType) {
      case EVAL:
	switch (arrayType) {
	  case ONE_D:
	    glDisable(GL_MAP2_VERTEX_4);
	    glEnable(GL_MAP1_VERTEX_4);
	    DrawPoints1();
	    DrawMapEval1(0.1/VORDER);
	    break;
	  case TWO_D:
	    glDisable(GL_MAP1_VERTEX_4);
	    glEnable(GL_MAP2_VERTEX_4);
	    DrawPoints2();
	    DrawMapEval2(0.1/VMAJOR_ORDER,0.1/VMINOR_ORDER);
	    break;
	}
	break;
      case MESH:
	switch (arrayType) {
	  case ONE_D:
	    DrawPoints1();
	    glDisable(GL_MAP2_VERTEX_4);
	    glEnable (GL_MAP1_VERTEX_4);
	    glColor3f(0.0, 0.0, 1.0);
	    glMapGrid1d(40, 0.0, 1.0);
	    if (mapPoint) {
		glPointSize(2);
		glEvalMesh1(GL_POINT, 0, 40);
	    } else {
		glEvalMesh1(GL_LINE, 0, 40);
	    }
	    break;
	  case TWO_D:
	    DrawPoints2();
	    glDisable(GL_MAP1_VERTEX_4);
	    glEnable(GL_MAP2_VERTEX_4);
	    glColor3f(0.0, 0.0, 1.0);
	    glMapGrid2d(20, 0.0, 1.0, 20, 0.0, 1.0);
	    if (mapPoint) {
		glPointSize(2);
		glEvalMesh2(GL_POINT, 0, 20, 0, 20);
	    } else if (polygonFilled) {
		glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	    } else {
		glEvalMesh2(GL_LINE, 0, 20, 0, 20);
	    }
	    break;
	}
	break;
    }
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -0.5, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case '1':
	arrayType = ONE_D;
	glDisable(GL_AUTO_NORMAL);
	glutPostRedisplay();
	break;
      case '2':
	arrayType = TWO_D;
	glEnable(GL_AUTO_NORMAL);
	glutPostRedisplay();
	break;
      case '3':
	mapType = EVAL;
	glutPostRedisplay();
	break;
      case '4':
	mapType = MESH;
	glutPostRedisplay();
	break;
      case '5':
	polygonFilled = !polygonFilled;
	glutPostRedisplay();
	break;
      case '6':
	mapPoint = !mapPoint;
	glutPostRedisplay();
	break;
      case '7':
	colorType = !colorType;
	glutPostRedisplay();
	break;
      case '8':
	textureType = !textureType;
	glutPostRedisplay();
	break;
      case '9':
	lighting =! lighting;
	if (lighting) {
	    glEnable(GL_LIGHTING);
	    glEnable(GL_LIGHT0);
	    if (arrayType == TWO_D) {
		glEnable(GL_AUTO_NORMAL);
	    } else {
		glDisable(GL_AUTO_NORMAL);
	    }
	} else {
	    glDisable(GL_LIGHTING);
	    glDisable(GL_LIGHT0);
	    glDisable(GL_AUTO_NORMAL);
	}
	glutPostRedisplay();
	break;
      case 27:
	exit(0);
    }
}

static void SpecialKey(int key, int x, int y)
{

    switch (key) {
      case GLUT_KEY_LEFT:
	rotY -= 30;
	glutPostRedisplay();
	break;
      case GLUT_KEY_RIGHT:
	rotY += 30;
	glutPostRedisplay();
	break;
      case GLUT_KEY_UP:
	rotX -= 30;
	glutPostRedisplay();
	break;
      case GLUT_KEY_DOWN:
	rotX += 30;
	glutPostRedisplay();
	break;
    }
}

static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    glTranslatef(0.0, 0.0 , translateZ);
    glRotatef(rotX, 1, 0, 0);
    glRotatef(rotY, 0, 1, 0);
    RenderEval();

    glPopMatrix();

    if (doubleBuffer) {
	glutSwapBuffers();
    } else {
	glFlush();
    }
}

static void Args(int argc, char **argv)
{
    GLint i;

    doubleBuffer = GL_FALSE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	}
    }
}

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    type = GLUT_RGB | GLUT_DEPTH;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Evaluator Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
