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
 *  varray.c
 *  This program demonstrates vertex arrays.
 */
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef GL_VERSION_1_1
#define POINTER 1
#define INTERLEAVED 2

#define DRAWARRAY 1
#define ARRAYELEMENT  2
#define DRAWELEMENTS 3

int setupMethod = POINTER;
int derefMethod = DRAWARRAY;

void setupPointers(void)
{
   static GLint vertices[] = {25, 25,
                       100, 325,
                       175, 25,
                       175, 325,
                       250, 25,
                       325, 325};
   static GLfloat colors[] = {1.0, 0.2, 0.2,
                       0.2, 0.2, 1.0,
                       0.8, 1.0, 0.2,
                       0.75, 0.75, 0.75,
                       0.35, 0.35, 0.35,
                       0.5, 0.5, 0.5};

   glEnableClientState (GL_VERTEX_ARRAY);
   glEnableClientState (GL_COLOR_ARRAY);

   glVertexPointer (2, GL_INT, 0, vertices);
   glColorPointer (3, GL_FLOAT, 0, colors);
}

void setupInterleave(void)
{
   static GLfloat intertwined[] =
      {1.0, 0.2, 1.0, 100.0, 100.0, 0.0,
       1.0, 0.2, 0.2, 0.0, 200.0, 0.0,
       1.0, 1.0, 0.2, 100.0, 300.0, 0.0,
       0.2, 1.0, 0.2, 200.0, 300.0, 0.0,
       0.2, 1.0, 1.0, 300.0, 200.0, 0.0,
       0.2, 0.2, 1.0, 200.0, 100.0, 0.0};
   
   glInterleavedArrays (GL_C3F_V3F, 0, intertwined);
}

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   setupPointers ();
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);

   if (derefMethod == DRAWARRAY) 
      glDrawArrays (GL_TRIANGLES, 0, 6);
   else if (derefMethod == ARRAYELEMENT) {
      glBegin (GL_TRIANGLES);
      glArrayElement (2);
      glArrayElement (3);
      glArrayElement (5);
      glEnd ();
   }
   else if (derefMethod == DRAWELEMENTS) {
      GLuint indices[4] = {0, 1, 3, 4};

      glDrawElements (GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
   }
   glFlush ();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
}

void mouse (int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
            if (setupMethod == POINTER) {
               setupMethod = INTERLEAVED;
               setupInterleave();
            }
            else if (setupMethod == INTERLEAVED) {
               setupMethod = POINTER;
               setupPointers();
            }
            glutPostRedisplay();
         }
         break;
      case GLUT_MIDDLE_BUTTON:
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN) {
            if (derefMethod == DRAWARRAY) 
               derefMethod = ARRAYELEMENT;
            else if (derefMethod == ARRAYELEMENT) 
               derefMethod = DRAWELEMENTS;
            else if (derefMethod == DRAWELEMENTS) 
               derefMethod = DRAWARRAY;
            glutPostRedisplay();
         }
         break;
      default:
         break;
   }
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (350, 350); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
#else
int main(int argc, char** argv)
{
    fprintf (stderr, "This program demonstrates a feature which is not in OpenGL Version 1.0.\n");
    fprintf (stderr, "If your implementation of OpenGL Version 1.0 has the right extensions,\n");
    fprintf (stderr, "you may be able to modify this program to make it run.\n");
    return 0;
}
#endif
