#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>


GLenum doubleBuffer;

double plane[4] = {
    1.0, 0.0, -1.0, 0.0
};
float rotX = 5.0, rotY = -5.0, zTranslate = -65.0;
float fogDensity = 0.02;

GLint fogMode = GL_EXP;
#define DEFAULT_FOG_START -100.0
#define DEFAULT_FOG_END    100.0
float fogStart = DEFAULT_FOG_START;
float fogEnd = DEFAULT_FOG_END;

GLint cubeList = 1;

float scp[18][3] = {
    {
	1.000000, 0.000000, 0.000000
    },	
    {
	1.000000, 0.000000, 5.000000
    },
    {
	0.707107, 0.707107, 0.000000
    },	
    {
	0.707107, 0.707107, 5.000000
    },
    {
	0.000000, 1.000000, 0.000000
    },	
    {
	0.000000, 1.000000, 5.000000
    },
    {
	-0.707107, 0.707107, 0.000000
    },	
    {
	-0.707107, 0.707107, 5.000000
    },
    {
	-1.000000, 0.000000, 0.000000
    },	
    {
	-1.000000, 0.000000, 5.000000
    },
    {
	-0.707107, -0.707107, 0.000000
    },	
    {
	-0.707107, -0.707107, 5.000000
    },
    {
	0.000000, -1.000000, 0.000000
    },	
    {
	0.000000, -1.000000, 5.000000
    },
    {
	0.707107, -0.707107, 0.000000
    },	
    {
	0.707107, -0.707107, 5.000000
    },
    {
	1.000000, 0.000000, 0.000000
    },	
    {
	1.000000, 0.000000, 5.000000
    },
};

static void Init(void)
{
    static float ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float diffuse[] = {1.0, 1.0, 1.0, 1.0};
    static float position[] = {90.0, 90.0, 0.0, 0.0};
    static float front_mat_shininess[] = {30.0};
    static float front_mat_specular[] = {0.0, 0.0, 0.0, 1.0};
    static float front_mat_diffuse[] = {0.0, 1.0, 0.0, 1.0};
    static float back_mat_shininess[] = {50.0};
    static float back_mat_specular[] = {0.0, 0.0, 1.0, 1.0};
    static float back_mat_diffuse[] = {1.0, 0.0, 0.0, 1.0};
    static float lmodel_ambient[] = {0.0, 0.0, 0.0, 1.0};
    static float fog_color[] = {0.8, 0.8, 0.8, 1.0};
    
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
    glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess);
    glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);

    glEnable(GL_FOG);
    printf("Fog Color\t%f,%f,%f,%f\n\n",
	   fog_color[0],fog_color[1],fog_color[2],fog_color[3]);
    printf("Initial Fog Mode\tGL_EXP\n");
    printf("Initial Fog Density\t%f\n",fogDensity);
    printf("Type h for help.\n");
    glFogi(GL_FOG_MODE, fogMode);
    glFogf(GL_FOG_DENSITY, fogDensity);
    glFogfv(GL_FOG_COLOR, fog_color);
    assert(glGetError() == GL_NO_ERROR);

    glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]); 

    glNewList(cubeList, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	   glNormal3fv(scp[0]); glVertex3fv(scp[0]);
	   glNormal3fv(scp[0]); glVertex3fv(scp[1]);
	   glNormal3fv(scp[2]); glVertex3fv(scp[2]);
	   glNormal3fv(scp[2]); glVertex3fv(scp[3]);
	   glNormal3fv(scp[4]); glVertex3fv(scp[4]);
	   glNormal3fv(scp[4]); glVertex3fv(scp[5]);
	   glNormal3fv(scp[6]); glVertex3fv(scp[6]);
	   glNormal3fv(scp[6]); glVertex3fv(scp[7]);
	   glNormal3fv(scp[8]); glVertex3fv(scp[8]);
	   glNormal3fv(scp[8]); glVertex3fv(scp[9]);
	   glNormal3fv(scp[10]); glVertex3fv(scp[10]);
	   glNormal3fv(scp[10]); glVertex3fv(scp[11]);
	   glNormal3fv(scp[12]); glVertex3fv(scp[12]);
	   glNormal3fv(scp[12]); glVertex3fv(scp[13]);
	   glNormal3fv(scp[14]); glVertex3fv(scp[14]);
	   glNormal3fv(scp[14]); glVertex3fv(scp[15]);
	   glNormal3fv(scp[16]); glVertex3fv(scp[16]);
	   glNormal3fv(scp[16]); glVertex3fv(scp[17]);
	glEnd();
    glEndList();
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{
    GLfloat getFloat[1];

    switch (key) {
      case 'd':
	if (fogMode != GL_EXP) { /* switch fog mode iff different */
	  fogMode = GL_EXP; 
	  glFogi(GL_FOG_MODE, fogMode);
	  printf("Switching fog modes...\n");
	}

	fogDensity *= 1.10;
	glFogf(GL_FOG_DENSITY, fogDensity);

	printf("Fog Mode/Density\tGL_EXP/%f\n",fogDensity);

	assert(glGetError() == GL_NO_ERROR);

	glGetFloatv(GL_FOG_DENSITY,getFloat);assert(getFloat[0] == fogDensity);

	glutPostRedisplay();
	break;
      case 'D':
	if (fogMode != GL_EXP) { /* switch fog mode iff different */ 
	  fogMode = GL_EXP; 
	  glFogi(GL_FOG_MODE, fogMode);
	  printf("Switching fog modes...\n");
	}

	fogDensity /= 1.10;
	glFogf(GL_FOG_DENSITY, fogDensity);

	printf("Fog Mode/Density\tGL_EXP/%f\n",fogDensity);

	assert(glGetError() == GL_NO_ERROR);

	glGetFloatv(GL_FOG_DENSITY,getFloat);assert(getFloat[0] == fogDensity);

	glutPostRedisplay();
	break;
      case 's':
	if (fogMode != GL_LINEAR) { /* switch fog mode iff different */ 
	  fogMode = GL_LINEAR;
	  glFogi(GL_FOG_MODE,fogMode);
	  glFogf(GL_FOG_END,fogEnd);
	  printf("Switching fog modes...\n");
	}

	fogStart += 1.0;
	glFogf(GL_FOG_START,fogStart);

	printf("Fog Mode[Start,End]\tGL_LINEAR[%f,%f]\n",fogStart,fogEnd);

	assert(glGetError() == GL_NO_ERROR);

	glGetFloatv(GL_FOG_START,getFloat); assert(fogStart == getFloat[0]);
	glGetFloatv(GL_FOG_END,getFloat); assert(fogEnd == getFloat[0]);

	glutPostRedisplay();
	break;
      case 'S':
	if (fogMode != GL_LINEAR) { /* switch fog mode iff different */ 
	  fogMode = GL_LINEAR;
	  glFogi(GL_FOG_MODE,fogMode);
	  glFogf(GL_FOG_END,fogEnd);
	  printf("Switching fog modes...\n");
	}

	fogStart -= 1.0;
	glFogf(GL_FOG_START,fogStart);

	printf("Fog Mode[Start,End]\tGL_LINEAR[%f,%f]\n",fogStart,fogEnd);

	assert(glGetError() == GL_NO_ERROR);

	glGetFloatv(GL_FOG_START,getFloat); assert(fogStart == getFloat[0]);
	glGetFloatv(GL_FOG_END,getFloat); assert(fogEnd == getFloat[0]);

	glutPostRedisplay();
	break;
      case 'e':
	if (fogMode != GL_LINEAR) { /* switch fog mode iff different */ 
	  fogMode = GL_LINEAR;
	  glFogi(GL_FOG_MODE,fogMode);
	  glFogf(GL_FOG_START,fogStart);
	  printf("Switching fog modes...\n");
	}

	fogEnd += 1.0;
	glFogf(GL_FOG_END,fogEnd);

	printf("Fog Mode[Start,End]\tGL_LINEAR[%f,%f]\n",fogStart,fogEnd);

	assert(glGetError() == GL_NO_ERROR);

	glGetFloatv(GL_FOG_START,getFloat); assert(fogStart == getFloat[0]);
	glGetFloatv(GL_FOG_END,getFloat); assert(fogEnd == getFloat[0]);

	glutPostRedisplay();
	break;
      case 'E':
	if (fogMode != GL_LINEAR) { /* switch fog mode iff different */ 
	  fogMode = GL_LINEAR;
	  glFogi(GL_FOG_MODE,fogMode);
	  glFogf(GL_FOG_START,fogStart);
	  printf("Switching fog modes...\n");
	}

	fogEnd -= 1.0;
	glFogf(GL_FOG_END,fogEnd);

	printf("Fog Mode[Start,End]\tGL_LINEAR[%f,%f]\n",fogStart,fogEnd);

	assert(glGetError() == GL_NO_ERROR);

	glGetFloatv(GL_FOG_START,getFloat); assert(fogStart == getFloat[0]);
	glGetFloatv(GL_FOG_END,getFloat); assert(fogEnd == getFloat[0]);

	glutPostRedisplay();
	break;
      case 'h': case 'H': case '?':  
	printf("d\tset fog mode to GL_EXP and increase fog density by 10%%\n");
	printf("D\tset fog mode to GL_EXP and decrease fog density by 10%%\n");
        printf("s\tset fog mode to GL_LINEAR and increase fog start by 1.0\n");
        printf("S\tset fog mode to GL_LINEAR and decrease fog start by 1.0\n");
        printf("e\tset fog mode to GL_LINEAR and increase fog end by 1.0\n");
        printf("E\tset fog mode to GL_LINEAR and decrease fog end by 1.0\n");

	printf("r, R\trefresh\n");
        printf("\nh, H, ?\tthis help message\n");
        printf("ESC\texit\n");
	break;
      case 27:
	exit(0);
    }
}

static void SpecialKey(int key, int x, int y)
{

    switch (key) {
      case GLUT_KEY_UP:
	rotX -= 5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_DOWN:
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

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    glTranslatef(0, 0, zTranslate);
    glRotatef(rotY, 0,1,0);
    glRotatef(rotX, 1,0,0);
    glScalef(1.0, 1.0, 10.0);

    glCallList(cubeList);

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

    glutInit(&argc, argv);
    Args(argc, argv);

    type = GLUT_RGB | GLUT_DEPTH;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Fog Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
