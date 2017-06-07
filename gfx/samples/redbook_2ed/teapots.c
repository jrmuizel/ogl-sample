/*
 * License Applicability. Except to the extent portions of this file are
 * made subject to an alternative license as permitted in the SGI Free
 * Software License B, Version 1.0 (the "License"), the contents of this
 * file are subject only to the provisions of the License. You may not use
 * this file except in compliance with the License. You may obtain a copy
 * of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
 * Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
 * 
 * http://oss.sgi.com/projects/FreeB
 * 
 * Note that, as provided in the License, the Software is distributed on an
 * "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
 * DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
 * CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 * 
 * Original Code. The Original Code is: OpenGL Sample Implementation,
 * Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
 * Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
 * Copyright in any portions created by third parties is as indicated
 * elsewhere herein. All Rights Reserved.
 * 
 * Additional Notice Provisions: The application programming interfaces
 * established by SGI in conjunction with the Original Code are The
 * OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
 * April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
 * 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
 * Window System(R) (Version 1.3), released October 19, 1998. This software
 * was created using the OpenGL(R) version 1.2.1 Sample Implementation
 * published by SGI, but has not been independently verified as being
 * compliant with the OpenGL(R) version 1.2.1 Specification.
 *
 */

/*
 *  teapots.c
 *  This program demonstrates lots of material properties.
 *  A single light source illuminates the objects.
 */
#include <stdlib.h>
#include <GL/glut.h>

GLuint teapotList;

/*
 * Initialize depth buffer, projection matrix, light source, and lighting
 * model.  Do not specify a material property here.
 */
void init(void)
{
   GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
   GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat position[] = {0.0, 3.0, 3.0, 0.0};

   GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
   GLfloat local_view[] = {0.0};

   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, position);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

   glFrontFace(GL_CW);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_AUTO_NORMAL);
   glEnable(GL_NORMALIZE);
   glEnable(GL_DEPTH_TEST); 
/*  be efficient--make teapot display list  */
   teapotList = glGenLists(1);
   glNewList (teapotList, GL_COMPILE);
   glutSolidTeapot(1.0);
   glEndList ();
}

/*
 * Move object into position.  Use 3rd through 12th 
 * parameters to specify the material property.  Draw a teapot.
 */
void renderTeapot(GLfloat x, GLfloat y,
   GLfloat ambr, GLfloat ambg, GLfloat ambb,
   GLfloat difr, GLfloat difg, GLfloat difb,
   GLfloat specr, GLfloat specg, GLfloat specb, GLfloat shine)
{
   GLfloat mat[4];

   glPushMatrix();
   glTranslatef(x, y, 0.0);
   mat[0] = ambr; mat[1] = ambg; mat[2] = ambb; mat[3] = 1.0;
   glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
   mat[0] = difr; mat[1] = difg; mat[2] = difb;
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
   mat[0] = specr; mat[1] = specg; mat[2] = specb;
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
   glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);
   glCallList(teapotList);
   glPopMatrix();
}

/**
 *  First column:  emerald, jade, obsidian, pearl, ruby, turquoise
 *  2nd column:  brass, bronze, chrome, copper, gold, silver
 *  3rd column:  black, cyan, green, red, white, yellow plastic
 *  4th column:  black, cyan, green, red, white, yellow rubber
 */
void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   renderTeapot(2.0, 17.0, 0.0215, 0.1745, 0.0215,
      0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6);
   renderTeapot(2.0, 14.0, 0.135, 0.2225, 0.1575,
      0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1);
   renderTeapot(2.0, 11.0, 0.05375, 0.05, 0.06625,
      0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3);
   renderTeapot(2.0, 8.0, 0.25, 0.20725, 0.20725,
      1, 0.829, 0.829, 0.296648, 0.296648, 0.296648, 0.088);
   renderTeapot(2.0, 5.0, 0.1745, 0.01175, 0.01175,
      0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6);
   renderTeapot(2.0, 2.0, 0.1, 0.18725, 0.1745,
      0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1);
   renderTeapot(6.0, 17.0, 0.329412, 0.223529, 0.027451,
      0.780392, 0.568627, 0.113725, 0.992157, 0.941176, 0.807843,
      0.21794872);
   renderTeapot(6.0, 14.0, 0.2125, 0.1275, 0.054,
      0.714, 0.4284, 0.18144, 0.393548, 0.271906, 0.166721, 0.2);
   renderTeapot(6.0, 11.0, 0.25, 0.25, 0.25,
      0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6);
   renderTeapot(6.0, 8.0, 0.19125, 0.0735, 0.0225,
      0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.1);
   renderTeapot(6.0, 5.0, 0.24725, 0.1995, 0.0745,
      0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4);
   renderTeapot(6.0, 2.0, 0.19225, 0.19225, 0.19225,
      0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4);
   renderTeapot(10.0, 17.0, 0.0, 0.0, 0.0, 0.01, 0.01, 0.01,
      0.50, 0.50, 0.50, .25);
   renderTeapot(10.0, 14.0, 0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392,
      0.50196078, 0.50196078, 0.50196078, .25);
   renderTeapot(10.0, 11.0, 0.0, 0.0, 0.0,
      0.1, 0.35, 0.1, 0.45, 0.55, 0.45, .25);
   renderTeapot(10.0, 8.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0,
      0.7, 0.6, 0.6, .25);
   renderTeapot(10.0, 5.0, 0.0, 0.0, 0.0, 0.55, 0.55, 0.55,
      0.70, 0.70, 0.70, .25);
   renderTeapot(10.0, 2.0, 0.0, 0.0, 0.0, 0.5, 0.5, 0.0,
      0.60, 0.60, 0.50, .25);
   renderTeapot(14.0, 17.0, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01,
      0.4, 0.4, 0.4, .078125);
   renderTeapot(14.0, 14.0, 0.0, 0.05, 0.05, 0.4, 0.5, 0.5,
      0.04, 0.7, 0.7, .078125);
   renderTeapot(14.0, 11.0, 0.0, 0.05, 0.0, 0.4, 0.5, 0.4,
      0.04, 0.7, 0.04, .078125);
   renderTeapot(14.0, 8.0, 0.05, 0.0, 0.0, 0.5, 0.4, 0.4,
      0.7, 0.04, 0.04, .078125);
   renderTeapot(14.0, 5.0, 0.05, 0.05, 0.05, 0.5, 0.5, 0.5,
      0.7, 0.7, 0.7, .078125);
   renderTeapot(14.0, 2.0, 0.05, 0.05, 0.0, 0.5, 0.5, 0.4,
      0.7, 0.7, 0.04, .078125);
   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho(0.0, 16.0, 0.0, 16.0*(GLfloat)h/(GLfloat)w,
              -10.0, 10.0);
   else
      glOrtho(0.0, 16.0*(GLfloat)w/(GLfloat)h, 0.0, 16.0,
              -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

/*
 * Main Loop 
 */
int main(int argc, char **argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(500, 600);
   glutInitWindowPosition(50,50);
   glutCreateWindow(argv[0]);
   init();
   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
