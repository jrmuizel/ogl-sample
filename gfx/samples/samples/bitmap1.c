#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>


#define OPENGL_WIDTH 24
#define OPENGL_HEIGHT 13


GLenum rgb, doubleBuffer;

float boxA[3] = {
    0, 0, 0
};
float boxB[3] = {
    -100, 0, 0
};
float boxC[3] = {
    100, 0, 0
};
float boxD[3] = {
    0, 95, 0
};
float boxE[3] = {
    0, -105, 0
};
GLubyte OpenGL_bits1[] = {
   0x00, 0x03, 0x00,
   0x7f, 0xfb, 0xff,
   0x7f, 0xfb, 0xff,
   0x00, 0x03, 0x00,
   0x3e, 0x8f, 0xb7,
   0x63, 0xdb, 0xb0,
   0x63, 0xdb, 0xb7,
   0x63, 0xdb, 0xb6,
   0x63, 0x8f, 0xf3,
   0x63, 0x00, 0x00,
   0x63, 0x00, 0x00,
   0x63, 0x00, 0x00,
   0x3e, 0x00, 0x00,
};
GLubyte OpenGL_bits2[] = {
   0x00, 0x00, 0x00,
   0xff, 0xff, 0x01,
   0xff, 0xff, 0x01, 
   0x00, 0x00, 0x00,
   0xf9, 0xfc, 0x01, 
   0x8d, 0x0d, 0x00,
   0x8d, 0x0d, 0x00, 
   0x8d, 0x0d, 0x00,
   0xcc, 0x0d, 0x00, 
   0x0c, 0x4c, 0x0a,
   0x0c, 0x4c, 0x0e, 
   0x8c, 0xed, 0x0e,
   0xf8, 0x0c, 0x00, 
};
GLubyte logo_bits[] = {
   0x00, 0x66, 0x66, 
   0xff, 0x66, 0x66, 
   0x00, 0x00, 0x00, 
   0xff, 0x3c, 0x3c, 
   0x00, 0x42, 0x40, 
   0xff, 0x42, 0x40, 
   0x00, 0x41, 0x40, 
   0xff, 0x21, 0x20, 
   0x00, 0x2f, 0x20, 
   0xff, 0x20, 0x20, 
   0x00, 0x10, 0x90, 
   0xff, 0x10, 0x90, 
   0x00, 0x0f, 0x10, 
   0xff, 0x00, 0x00, 
   0x00, 0x66, 0x66, 
   0xff, 0x66, 0x66, 
};


static void Init(void)
{

    if (!rgb) {
	glutSetColor(0, 0.0, 0.0, 0.0);
	glutSetColor(1, 1.0, 0.0, 0.0);
	glutSetColor(2, 0.0, 1.0, 0.0);
	glutSetColor(3, 1.0, 1.0, 0.0);
	glutSetColor(4, 0.0, 0.0, 1.0);
	glutSetColor(5, 1.0, 0.0, 1.0);
	glutSetColor(6, 0.0, 1.0, 1.0);
	glutSetColor(7, 1.0, 1.0, 1.0);
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearIndex(0.0);
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-175, 175, -175, 175);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 27:
	exit(0);
    }
}

static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

    glRasterPos3fv(boxA);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 24);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 8);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 2);
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBitmap(16, 12, 8.0, 0.0, 0.0, 0.0, logo_bits);
	     
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    (rgb) ? glColor3f(1.0, 1.0, 1.0) : glIndexi(7);
    glRasterPos3fv(boxB);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits1);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits2);

    (rgb) ? glColor3f(0.0, 1.0, 1.0) : glIndexi(6);
    glRasterPos3fv(boxC);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits1);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits2);

    (rgb) ? glColor3f(1.0, 0.0, 1.0) : glIndexi(5);
    glRasterPos3fv(boxD);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits1);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits2);

    (rgb) ? glColor3f(1.0, 1.0, 0.0) : glIndexi(3);
    glRasterPos3fv(boxE);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits1);
    glBitmap(OPENGL_WIDTH, OPENGL_HEIGHT, OPENGL_WIDTH, 0.0, OPENGL_WIDTH, 0.0,
	     OpenGL_bits2);

    if (doubleBuffer) {
	glutSwapBuffers();
    } else {
	glFlush();
    }
}

static void Args(int argc, char **argv)
{
    GLint i;

    rgb = GL_TRUE;
    doubleBuffer = GL_FALSE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-ci") == 0) {
	    rgb = GL_FALSE;
	} else if (strcmp(argv[i], "-rgb") == 0) {
	    rgb = GL_TRUE;
	} else if (strcmp(argv[i], "-sb") == 0) {
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

    type = (rgb) ? GLUT_RGB : GLUT_INDEX;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Bitmap Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
