#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>


#define CI_OFFSET 16


GLenum rgb, doubleBuffer;

GLenum mode1, mode2;
GLint size;
float pntA[3] = {
    -160.0, 0.0, 0.0
};
float pntB[3] = {
    -130.0, 0.0, 0.0
};
float pntC[3] = {
    -40.0, -50.0, 0.0
};
float pntD[3] = {
    30.0, 60.0, 0.0
};


static void Init(void)
{
    GLint i;

    if (!rgb) {
	for (i = 0; i < 16; i++) {
	    glutSetColor(i+CI_OFFSET, i/15.0, i/15.0, 0.0);
	}
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glLineStipple(1, 0xF0E0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    mode1 = GL_FALSE;
    mode2 = GL_FALSE;
    size = 1;
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
      case '1':
	mode1 = !mode1;
	glutPostRedisplay();
	break;
      case '2':
	mode2 = !mode2;
	glutPostRedisplay();
	break;
      case 27:
	exit(0);
    }
}

static void SpecialKey(int key, int x, int y)
{

    switch (key) {
      case GLUT_KEY_UP:
	size++;
	glutPostRedisplay();
	break;
      case GLUT_KEY_DOWN:
	size--;
	if (size < 1) {
	    size = 1;
	}
	glutPostRedisplay();
	break;
    }
}

static void Draw(void)
{
    GLint ci, i;

    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(size);

    if (mode1) {
	glEnable(GL_LINE_STIPPLE);
    } else {
	glDisable(GL_LINE_STIPPLE);
    }
    
    if (mode2) {
	ci = CI_OFFSET;
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
    } else {
	ci = 3;
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
    }

    glPushMatrix();

    for (i = 0; i < 360; i += 5) {
	glRotatef(5.0, 0,0,1);

	(rgb) ? glColor3f(1.0, 1.0, 0.0) : glIndexi(ci);
	glBegin(GL_LINE_STRIP);
	    glVertex3fv(pntA);
	    glVertex3fv(pntB);
	glEnd();

	glPointSize(1);

	(rgb) ? glColor3f(0.0, 1.0, 0.0) : glIndexi(2);
	glBegin(GL_POINTS);
	    glVertex3fv(pntA);
	    glVertex3fv(pntB);
	glEnd();
    }

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

    Args(argc, argv);

    type = (rgb) ? GLUT_RGB : GLUT_INDEX;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Line Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
