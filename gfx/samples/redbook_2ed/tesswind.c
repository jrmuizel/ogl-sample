/*
 * License Applicability. Except to the extent portions of this file are
 * made subject to an alternative license as permitted in the SGI Free
 * Software License B, Version 1.1 (the "License"), the contents of this
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
 *  tesswind.c
 *  This program demonstrates the winding rule polygon 
 *  tessellation property.  Four tessellated objects are drawn, 
 *  each with very different contours.  When the w key is pressed, 
 *  the objects are drawn with a different winding rule.
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef CALLBACK
#define CALLBACK
#endif

GLdouble currentWinding = GLU_TESS_WINDING_ODD;
int currentShape = 0;
GLUtesselator *tobj;
GLuint list;

/*  Make four display lists, 
 *  each with a different tessellated object. 
 */
void makeNewLists (void) {
   int i;
   static GLdouble rects[12][3] = 
      {50.0, 50.0, 0.0, 300.0, 50.0, 0.0, 
       300.0, 300.0, 0.0, 50.0, 300.0, 0.0,
       100.0, 100.0, 0.0, 250.0, 100.0, 0.0, 
       250.0, 250.0, 0.0, 100.0, 250.0, 0.0,
       150.0, 150.0, 0.0, 200.0, 150.0, 0.0, 
       200.0, 200.0, 0.0, 150.0, 200.0, 0.0};
   static GLdouble spiral[16][3] = 
      {400.0, 250.0, 0.0, 400.0, 50.0, 0.0, 
       50.0, 50.0, 0.0, 50.0, 400.0, 0.0, 
       350.0, 400.0, 0.0, 350.0, 100.0, 0.0, 
       100.0, 100.0, 0.0, 100.0, 350.0, 0.0, 
       300.0, 350.0, 0.0, 300.0, 150.0, 0.0, 
       150.0, 150.0, 0.0, 150.0, 300.0, 0.0, 
       250.0, 300.0, 0.0, 250.0, 200.0, 0.0, 
       200.0, 200.0, 0.0, 200.0, 250.0, 0.0};
   static GLdouble quad1[4][3] = 
      {50.0, 150.0, 0.0, 350.0, 150.0, 0.0, 
      350.0, 200.0, 0.0, 50.0, 200.0, 0.0};
   static GLdouble quad2[4][3] =
      {100.0, 100.0, 0.0, 300.0, 100.0, 0.0, 
       300.0, 350.0, 0.0, 100.0, 350.0, 0.0};
   static GLdouble tri[3][3] =
      {200.0, 50.0, 0.0, 250.0, 300.0, 0.0,
       150.0, 300.0, 0.0};
 
   gluTessProperty(tobj, GLU_TESS_WINDING_RULE, 
                   currentWinding);

   glNewList(list, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 4; i < 8; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 8; i < 12; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();

   glNewList(list+1, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 7; i >= 4; i--)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 11; i >= 8; i--)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();

   glNewList(list+2, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 16; i++)
            gluTessVertex(tobj, spiral[i], spiral[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();

   glNewList(list+3, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, quad1[i], quad1[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, quad2[i], quad2[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 0; i < 3; i++)
            gluTessVertex(tobj, tri[i], tri[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();
}

void display (void) {
   glClear(GL_COLOR_BUFFER_BIT);
   glColor3f(1.0, 1.0, 1.0);
   glPushMatrix(); 
   glCallList(list);
   glTranslatef(0.0, 500.0, 0.0);
   glCallList(list+1);
   glTranslatef(500.0, -500.0, 0.0);
   glCallList(list+2);
   glTranslatef(0.0, 500.0, 0.0);
   glCallList(list+3);
   glPopMatrix(); 
   glFlush();
}

void CALLBACK beginCallback(GLenum which)
{
   glBegin(which);
}

void CALLBACK errorCallback(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf(stderr, "Tessellation Error: %s\n", estring);
   exit(0);
}

void CALLBACK endCallback(void)
{
   glEnd();
}

/*  combineCallback is used to create a new vertex when edges
 *  intersect.  coordinate location is trivial to calculate,
 *  but weight[4] may be used to average color, normal, or texture 
 *  coordinate data.
 */
void CALLBACK combineCallback(GLdouble coords[3], GLdouble *data[4],
                     GLfloat weight[4], GLdouble **dataOut )
{
   GLdouble *vertex;
   vertex = (GLdouble *) malloc(3 * sizeof(GLdouble));

   vertex[0] = coords[0];
   vertex[1] = coords[1];
   vertex[2] = coords[2];
   *dataOut = vertex;
}

void init(void) 
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);    

   tobj = gluNewTess();
   gluTessCallback(tobj, GLU_TESS_VERTEX, 
                   glVertex3dv);
   gluTessCallback(tobj, GLU_TESS_BEGIN, 
                   beginCallback);
   gluTessCallback(tobj, GLU_TESS_END, 
                   endCallback);
   gluTessCallback(tobj, GLU_TESS_ERROR, 
                   errorCallback);
   gluTessCallback(tobj, GLU_TESS_COMBINE, 
                   combineCallback);

   list = glGenLists(4);
   makeNewLists();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      gluOrtho2D(0.0, 1000.0, 0.0, 1000.0 * (GLdouble)h/(GLdouble)w);
   else
      gluOrtho2D(0.0, 1000.0 * (GLdouble)w/(GLdouble)h, 0.0, 1000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'w':
      case 'W':
         if (currentWinding == GLU_TESS_WINDING_ODD)
            currentWinding = GLU_TESS_WINDING_NONZERO;
         else if (currentWinding == GLU_TESS_WINDING_NONZERO)
            currentWinding = GLU_TESS_WINDING_POSITIVE;
         else if (currentWinding == GLU_TESS_WINDING_POSITIVE)
            currentWinding = GLU_TESS_WINDING_NEGATIVE;
         else if (currentWinding == GLU_TESS_WINDING_NEGATIVE)
            currentWinding = GLU_TESS_WINDING_ABS_GEQ_TWO;
         else if (currentWinding == GLU_TESS_WINDING_ABS_GEQ_TWO)
            currentWinding = GLU_TESS_WINDING_ODD;
         makeNewLists();
         glutPostRedisplay();
         break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(500, 500);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;  
}
