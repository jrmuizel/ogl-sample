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
 * feedback.c
 * This program demonstrates use of OpenGL feedback.  First,
 * a lighting environment is set up and a few lines are drawn.
 * Then feedback mode is entered, and the same lines are 
 * drawn.  The results in the feedback buffer are printed.
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

/*  Initialize lighting.
 */
void init(void)
{
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
}

/* Draw a few lines and two points, one of which will 
 * be clipped.  If in feedback mode, a passthrough token 
 * is issued between the each primitive.
 */
void drawGeometry (GLenum mode)
{
   glBegin (GL_LINE_STRIP);
   glNormal3f (0.0, 0.0, 1.0);
   glVertex3f (30.0, 30.0, 0.0);
   glVertex3f (50.0, 60.0, 0.0);
   glVertex3f (70.0, 40.0, 0.0);
   glEnd ();
   if (mode == GL_FEEDBACK)
      glPassThrough (1.0);
   glBegin (GL_POINTS);
   glVertex3f (-100.0, -100.0, -100.0);  /*  will be clipped  */
   glEnd ();
   if (mode == GL_FEEDBACK)
      glPassThrough (2.0);
   glBegin (GL_POINTS);
   glNormal3f (0.0, 0.0, 1.0);
   glVertex3f (50.0, 50.0, 0.0);
   glEnd ();
}

/* Write contents of one vertex to stdout.	*/
void print3DcolorVertex (GLint size, GLint *count, 
                         GLfloat *buffer)
{
   int i;

   printf ("  ");
   for (i = 0; i < 7; i++) {
      printf ("%4.2f ", buffer[size-(*count)]);
      *count = *count - 1;
   }
   printf ("\n");
}

/*  Write contents of entire buffer.  (Parse tokens!)	*/
void printBuffer(GLint size, GLfloat *buffer)
{
   GLint count;
   GLfloat token;

   count = size;
   while (count) {
      token = buffer[size-count]; count--;
      if (token == GL_PASS_THROUGH_TOKEN) {
         printf ("GL_PASS_THROUGH_TOKEN\n");
         printf ("  %4.2f\n", buffer[size-count]);
         count--;
      }
      else if (token == GL_POINT_TOKEN) {
         printf ("GL_POINT_TOKEN\n");
         print3DcolorVertex (size, &count, buffer);
      }
      else if (token == GL_LINE_TOKEN) {
         printf ("GL_LINE_TOKEN\n");
         print3DcolorVertex (size, &count, buffer);
         print3DcolorVertex (size, &count, buffer);
      }
      else if (token == GL_LINE_RESET_TOKEN) {
         printf ("GL_LINE_RESET_TOKEN\n");
         print3DcolorVertex (size, &count, buffer);
         print3DcolorVertex (size, &count, buffer);
      }
   }
}

void display(void)
{
   GLfloat feedBuffer[1024];
   GLint size;

   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glOrtho (0.0, 100.0, 0.0, 100.0, 0.0, 1.0);

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT);
   drawGeometry (GL_RENDER);

   glFeedbackBuffer (1024, GL_3D_COLOR, feedBuffer);
   (void) glRenderMode (GL_FEEDBACK);
   drawGeometry (GL_FEEDBACK);

   size = glRenderMode (GL_RENDER);
   printBuffer (size, feedBuffer);
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

/*  Main Loop  */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (100, 100);
   glutInitWindowPosition (100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0; 
}
