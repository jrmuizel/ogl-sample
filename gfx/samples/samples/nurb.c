#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <GL/glut.h>


#define INREAL float

#define S_NUMPOINTS 13
#define S_ORDER     3   
#define S_NUMKNOTS  (S_NUMPOINTS + S_ORDER)
#define T_NUMPOINTS 3
#define T_ORDER     3 
#define T_NUMKNOTS  (T_NUMPOINTS + T_ORDER)
#define SQRT_TWO    1.41421356237309504880

typedef INREAL Point[4];


GLenum doubleBuffer;

GLenum expectedError;
GLint rotX = 40, rotY = 40;
INREAL sknots[S_NUMKNOTS] = {
    -1.0, -1.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0,
    4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 9.0, 9.0
};
INREAL glutnots[T_NUMKNOTS] = {
    1.0, 1.0, 1.0, 2.0, 2.0, 2.0
};
Point ctlpoints[S_NUMPOINTS][T_NUMPOINTS] = {
    {
	{
	    4.0, 2.0, 2.0, 1.0
	},
	{
	    4.0, 1.6, 2.5, 1.0
	},
	{
	    4.0, 2.0, 3.0, 1.0
	}
    },
    {
	{
	    5.0, 4.0, 2.0, 1.0
	},
	{
	    5.0, 4.0, 2.5, 1.0
	},
	{
	    5.0, 4.0, 3.0, 1.0
	}
    },
    {
	{
	    6.0, 5.0, 2.0, 1.0
	},
	{
	    6.0, 5.0, 2.5, 1.0
	},
	{
	    6.0, 5.0, 3.0, 1.0
	}
    },
    {
	{
	    SQRT_TWO*6.0, SQRT_TWO*6.0, SQRT_TWO*2.0, SQRT_TWO
	},
	{
	    SQRT_TWO*6.0, SQRT_TWO*6.0, SQRT_TWO*2.5, SQRT_TWO
	},
	{
	    SQRT_TWO*6.0, SQRT_TWO*6.0, SQRT_TWO*3.0, SQRT_TWO
	}  
    },
    {
	{
	    5.2, 6.7, 2.0, 1.0
	},
	{
	    5.2, 6.7, 2.5, 1.0
	},
	{
	    5.2, 6.7, 3.0, 1.0
	}
    },
    {
	{
	    SQRT_TWO*4.0, SQRT_TWO*6.0, SQRT_TWO*2.0, SQRT_TWO
	},
	{
	    SQRT_TWO*4.0, SQRT_TWO*6.0, SQRT_TWO*2.5, SQRT_TWO
	}, 
	{
	    SQRT_TWO*4.0, SQRT_TWO*6.0, SQRT_TWO*3.0, SQRT_TWO
	}  
    }, 
    {
	{
	    4.0, 5.2, 2.0, 1.0
	},
	{
	    4.0, 4.6, 2.5, 1.0
	},
	{
	    4.0, 5.2, 3.0, 1.0
	}  
    },
    {
	{
	    SQRT_TWO*4.0, SQRT_TWO*6.0, SQRT_TWO*2.0, SQRT_TWO
	},
	{
	    SQRT_TWO*4.0, SQRT_TWO*6.0, SQRT_TWO*2.5, SQRT_TWO
	},
	{
	    SQRT_TWO*4.0, SQRT_TWO*6.0, SQRT_TWO*3.0, SQRT_TWO
	}  
    },
    {
	{
	    2.8, 6.7, 2.0, 1.0
	},
	{
	    2.8, 6.7, 2.5, 1.0
	},
	{
	    2.8, 6.7, 3.0, 1.0
	}   
    },
    {
	{
	    SQRT_TWO*2.0, SQRT_TWO*6.0, SQRT_TWO*2.0, SQRT_TWO
	},
	{
	    SQRT_TWO*2.0, SQRT_TWO*6.0, SQRT_TWO*2.5, SQRT_TWO
	},
	{
	    SQRT_TWO*2.0, SQRT_TWO*6.0, SQRT_TWO*3.0, SQRT_TWO
	}  
    },
    {
	{
	    2.0, 5.0, 2.0, 1.0
	},
	{
	    2.0, 5.0, 2.5, 1.0
	},
	{
	    2.0, 5.0, 3.0, 1.0
	} 
    },
    {
	{
	    3.0, 4.0, 2.0, 1.0
	},
	{
	    3.0, 4.0, 2.5, 1.0
	},
	{
	    3.0, 4.0, 3.0, 1.0
	} 
    },
    {
	{
	    4.0, 2.0, 2.0, 1.0
	},
	{
	    4.0, 1.6, 2.5, 1.0
	},
	{
	    4.0, 2.0, 3.0, 1.0
	}    
    }
};
GLUnurbsObj *theNurbs;


static void ErrorCallback(GLenum which)
{

    if (which != expectedError) {
	fprintf(stderr, "Unexpected error occured (%d):\n", which);
	fprintf(stderr, "    %s\n", gluErrorString(which));
    }
}

static void Init(void)
{

    theNurbs = gluNewNurbsRenderer();
    gluNurbsCallback(theNurbs, GLU_ERROR, ErrorCallback);

    gluNurbsProperty(theNurbs, GLU_SAMPLING_TOLERANCE, 15.0);
    gluNurbsProperty(theNurbs, GLU_DISPLAY_MODE, GLU_OUTLINE_PATCH);

    expectedError = GLU_INVALID_ENUM;
    gluNurbsProperty(theNurbs, ~0, 15.0);
    expectedError = GLU_NURBS_ERROR13;
    gluEndSurface(theNurbs);
    expectedError = 0;

    glColor3f(1.0, 1.0, 1.0);
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2.0, 2.0, -2.0, 2.0, 0.8, 10.0);
    gluLookAt(7.0, 4.5, 4.0, 4.5, 4.5, 2.5, 6.0, -3.0, 2.0);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 27:
	exit(0);
    }
}

static void SpecialKey(int key, int x, int y)
{

    switch (key) {
      case GLUT_KEY_DOWN:
	rotX -= 5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_UP:
	rotX += 5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_LEFT:
	rotY -= 5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_RIGHT:
	rotY += 5;
	glutPostRedisplay();
	break;
    }
}

static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

    glTranslatef(4.0, 4.5, 2.5);
    glRotatef(rotY, 1, 0, 0);
    glRotatef(rotX, 0, 1, 0);
    glTranslatef(-4.0, -4.5, -2.5);

    gluBeginSurface(theNurbs);
    gluNurbsSurface(theNurbs, S_NUMKNOTS, sknots, T_NUMKNOTS, glutnots,
		    4*T_NUMPOINTS, 4, &ctlpoints[0][0][0], S_ORDER,
		    T_ORDER, GL_MAP2_VERTEX_4);
    gluEndSurface(theNurbs);

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

    Args(argc, argv);

    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(300, 300);
    glutCreateWindow("NURBS Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
