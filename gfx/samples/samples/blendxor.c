/*
** blendxor.c - Demonstrates the use of the color logic op
**    extension to draw hilights. Using XOR to draw the same
**    image twice restores the background to its original value.
*/

#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <GL/glut.h>

static GLenum doubleBuffer = GL_FALSE;
static GLenum dithering = GL_FALSE;
static GLenum blending = GL_FALSE;

static void Init(void)
{
    glShadeModel(GL_FLAT);
}

static void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 400, 0, 400);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 'b':
	blending = !blending;
	if (blending == GL_TRUE) printf("Blend on\n");
	else printf("Blend off\n");
	if (dithering == GL_TRUE) printf("Dither on\n");
	else printf("Dither off\n");
	glutPostRedisplay();
	break;
      case 'd':
	dithering = !dithering;
	if (blending == GL_TRUE) printf("Blend on\n");
	else printf("Blend off\n");
	if (dithering == GL_TRUE) printf("Dither on\n");
	else printf("Dither off\n");
	glutPostRedisplay();
	break;
      case 'h':
	if (blending == GL_TRUE) printf("Blend on\n");
	else printf("Blend off\n");
	if (dithering == GL_TRUE) printf("Dither on\n");
	else printf("Dither off\n");
	printf("\n");

	printf("d - toggle dithering\n");
	printf("b - toggle blending\n");
	printf("h - this help message\n");
	printf("ESC - quit\n");
	glutPostRedisplay();
	break;
      case 27:
	exit(0);
    }
}

static void Draw(void)
{
    float xscale, yscale;
    GLfloat x, y;
    int i;

    glDisable(GL_COLOR_LOGIC_OP);

    (blending) ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    (dithering) ? glEnable(GL_DITHER) : glDisable(GL_DITHER);

    glClearColor(0.5, 0.6, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw background prims */
    glColor3f(0.1, 0.1, 1.0);
    glBegin(GL_TRIANGLES);
        glVertex2i(5, 5);
        glVertex2i(130, 50);
        glVertex2i(100,  300);
    glEnd();
    glColor3f(0.5, 0.2, 0.9);
    glBegin(GL_TRIANGLES);
        glVertex2i(200, 100);
        glVertex2i(330, 50);
        glVertex2i(340,  400);
    glEnd();

    glEnable(GL_COLOR_LOGIC_OP); /* glBlendEquationEXT(GL_LOGIC_OP); */
    glLogicOp(GL_XOR);

    /* Draw a set of rectangles across the window */
    glColor3f(0.9, 0.2, 0.8);
    for(i = 0; i < 400; i+=60) {
        glBegin(GL_POLYGON);
            glVertex2i(i, 100);
            glVertex2i(i+50, 100);
            glVertex2i(i+50, 200);
            glVertex2i(i, 200);
        glEnd();
    }
    if (doubleBuffer) {
	glutSwapBuffers();
	glDrawBuffer(GL_FRONT);	/* draw next prims in visible buffer */
    } else {
	glFlush();
    }

    sleep(2);			/* wait to see results */

    /* Redraw  the rectangles, which should erase them */
    for(i = 0; i < 400; i+=60) {
        glBegin(GL_POLYGON);
            glVertex2i(i, 100);
            glVertex2i(i+50, 100);
            glVertex2i(i+50, 200);
            glVertex2i(i, 200);
        glEnd();
    }

    glFlush();
    if (doubleBuffer) {
       glDrawBuffer(GL_BACK);	/* draw next frame in invisible buffer */
    }
    assert(glGetError() == GL_NO_ERROR);
}

static void Args(int argc, char **argv)
{
    GLint i;

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
    glutInit(&argc, argv);
    Args(argc, argv);

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
    if (doubleBuffer) glutInitDisplayString("red<=4 green<=4 blue<=4 double");
    else glutInitDisplayString("red<=4 green<=4 blue<=4");    
#else
    {
       GLenum type= GLUT_RGBA;
       type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;

       glutInitDisplayMode(type);
    }
#endif

    glutInitWindowSize(400, 400);
    glutCreateWindow("RGBA XOR");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
