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
 *  list.c
 *  This program demonstrates how to make and execute a 
 *  display list.  Note that attributes, such as current 
 *  color and matrix, are changed.
 */
#include <GL/glut.h>
#include <stdlib.h>

GLuint listName;

static void init (void)
{
   listName = glGenLists (1);
   glNewList (listName, GL_COMPILE);
      glColor3f (1.0, 0.0, 0.0);  /*  current color red  */
      glBegin (GL_TRIANGLES);
      glVertex2f (0.0, 0.0);
      glVertex2f (1.0, 0.0);
      glVertex2f (0.0, 1.0);
      glEnd ();
      glTranslatef (1.5, 0.0, 0.0); /*  move position  */
   glEndList ();
   glShadeModel (GL_FLAT);
}

static void drawLine (void)
{
   glBegin (GL_LINES);
   glVertex2f (0.0, 0.5);
   glVertex2f (15.0, 0.5);
   glEnd ();
}

void display(void)
{
   GLuint i;

   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (0.0, 1.0, 0.0);  /*  current color green  */
   for (i = 0; i < 10; i++)    /*  draw 10 triangles    */
      glCallList (listName);
   drawLine ();  /*  is this line green?  NO!  */
                 /*  where is the line drawn?  */
   glFlush ();
}

void reshape(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h) 
      gluOrtho2D (0.0, 2.0, -0.5 * (GLfloat) h/(GLfloat) w, 
         1.5 * (GLfloat) h/(GLfloat) w);
   else 
      gluOrtho2D (0.0, 2.0 * (GLfloat) w/(GLfloat) h, -0.5, 1.5); 
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(650, 50);
   glutCreateWindow(argv[0]);
   init ();
   glutReshapeFunc (reshape);
   glutDisplayFunc (display);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
