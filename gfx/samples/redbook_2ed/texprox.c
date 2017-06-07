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
 *  texprox.c
 *  The brief program illustrates use of texture proxies.
 *  This program only prints out some messages about whether
 *  certain size textures are supported and then exits.
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef GL_VERSION_1_1
void init(void) 
{
   GLint proxyComponents;

   glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGBA8,
                64, 64, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, 
                            GL_TEXTURE_COMPONENTS, &proxyComponents);
   printf ("proxyComponents are %d\n", proxyComponents);
   if (proxyComponents == GL_RGBA8)
      printf ("proxy allocation succeeded\n");
   else
      printf ("proxy allocation failed\n");

   glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGBA16,
                2048, 2048, 0,
                GL_RGBA, GL_UNSIGNED_SHORT, NULL);
   glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, 
                            GL_TEXTURE_COMPONENTS, &proxyComponents);
   printf ("proxyComponents are %d\n", proxyComponents);
   if (proxyComponents == GL_RGBA16)
      printf ("proxy allocation succeeded\n");
   else
      printf ("proxy allocation failed\n");
}

void display(void)
{
   exit(0);
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
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
