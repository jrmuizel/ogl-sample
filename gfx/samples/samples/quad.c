#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/rgb.h>


#define PI 3.141592654
#define	BLACK 0
#define	GRAY 128
#define	WHITE 255
#define RD 0xA4,0x00,0x00,0xFF
#define WT 0xFF,0xFF,0xFF,0xFF
#define	brickImageWidth 16
#define	brickImageHeight 16


GLenum doubleBuffer;

float black[3] = {
    0.0, 0.0, 0.0
};
float blue[3] =  {
    0.0, 0.0, 1.0
};
float gray[3] =  {
    0.5, 0.5, 0.5
};
float white[3] = {
    1.0, 1.0, 1.0
};

GLenum doDither = GL_TRUE;
GLenum shade = GL_TRUE;
GLenum texture = GL_TRUE;

float xRotation = 30.0, yRotation = 30.0, zRotation = 0.0;
GLint radius1, radius2;
GLdouble angle1, angle2;
GLint slices, stacks;
GLint height;
GLint orientation = GLU_OUTSIDE;
GLint whichQuadric;
GLUquadricObj *quadObj;

GLubyte brickImage[4*brickImageWidth*brickImageHeight] = {
    RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD,
    RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD,
    RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD,
    RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD,
    WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT,
    RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD,
    RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD,
    RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD,
    RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD, RD,
    WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT,
    RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD,
    RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD,
    RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD,
    RD, RD, RD, RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD,
    WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT,
    RD, RD, RD, RD, WT, RD, RD, RD, RD, RD, RD, RD, RD, RD, WT, RD
};
RGBImageRec *image = NULL;


static void ErrorHandler(GLenum which)
{

    fprintf(stderr, "Quad Error: %s\n", gluErrorString(which));
}

static void Init(void)
{
    static GLint colorIndexes[3] = {0, 200, 255};
    static float ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float diffuse[] = {0.5, 1.0, 1.0, 1.0};
    static float position[] = {90.0, 90.0, 150.0, 0.0};
    static float front_mat_shininess[] = {30.0};
    static float front_mat_specular[] = {0.2, 0.2, 0.2, 1.0};
    static float front_mat_diffuse[] = {0.5, 0.28, 0.38, 1.0};
    static float back_mat_shininess[] = {50.0};
    static float back_mat_specular[] = {0.5, 0.5, 0.2, 1.0};
    static float back_mat_diffuse[] = {1.0, 1.0, 0.2, 1.0};
    static float lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
    static float lmodel_twoside[] = {GL_TRUE};
    static float decal[] = {GL_DECAL};
    static float modulate[] = {GL_MODULATE};
    static float repeat[] = {GL_REPEAT};
    static float nearest[] = {GL_NEAREST};

    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
    glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess);
    glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);

    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, decal);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nearest);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nearest);
    if (image) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->sizeX, image->sizeY,
			  GL_RGB, GL_UNSIGNED_BYTE, image->data);
    } else {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, brickImageWidth, brickImageHeight,
		     0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)brickImage);
    }

    quadObj = gluNewQuadric();
    gluQuadricCallback(quadObj, GLU_ERROR, ErrorHandler);

    radius1 = 10;
    radius2 = 5;
    angle1 = 90;
    angle2 = 180;
    slices = 16;
    stacks = 10;
    height = 20;
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 10);
    gluLookAt(2, 2, 2, 0, 0, 0, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 'X':
	zRotation += 5;
	glutPostRedisplay();
	break;
      case 'x':
	zRotation -= 5;
	glutPostRedisplay();
	break;

      case '1':
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	glutPostRedisplay();
	break;
      case '2':
	gluQuadricDrawStyle(quadObj, GLU_POINT);
	glutPostRedisplay();
	break;
      case '3':
	gluQuadricDrawStyle(quadObj, GLU_LINE);
	glutPostRedisplay();
	break;
      case '4':
	gluQuadricDrawStyle(quadObj, GLU_SILHOUETTE);
	glutPostRedisplay();
	break;

      case '0':
	shade = !shade;
	if (shade) {
	    glShadeModel(GL_SMOOTH);
	    gluQuadricNormals(quadObj, GLU_SMOOTH);
	} else {
	    glShadeModel(GL_FLAT);
	    gluQuadricNormals(quadObj, GLU_FLAT);
	}
	glutPostRedisplay();
	break;

      case 'A':
	stacks++;
	glutPostRedisplay();
	break;
      case 'a':
	stacks--;
	glutPostRedisplay();
	break;
    
      case 'S':
	slices++;
	glutPostRedisplay();
	break;
      case 's':
	slices--;
	glutPostRedisplay();
	break;

      case 'd':
	switch(orientation) {
	  case GLU_OUTSIDE:
	    orientation = GLU_INSIDE;
	    break;
	  case GLU_INSIDE:
	  default:
	    orientation = GLU_OUTSIDE;
	    break;
	}
	gluQuadricOrientation(quadObj, orientation);
	glutPostRedisplay();
	break;

      case 'f':
	whichQuadric = (whichQuadric + 1) & 0x03;
	glutPostRedisplay();
	break;

      case 'G':
	radius1 += 1;
	glutPostRedisplay();
	break;
      case 'g':
	radius1 -= 1;
	glutPostRedisplay();
	break;

      case 'J':
	radius2 += 1;
	glutPostRedisplay();
	break;
      case 'j':
	radius2 -= 1;
	glutPostRedisplay();
	break;

      case 'H':
	height += 2;
	glutPostRedisplay();
	break;
      case 'h':
	height -= 2;
	glutPostRedisplay();
	break;

      case 'K':
	angle1 += 5;
	glutPostRedisplay();
	break;
      case 'k':
	angle1 -= 5;
	glutPostRedisplay();
	break;

      case 'L':
	angle2 += 5;
	glutPostRedisplay();
	break;
      case 'l':
	angle2 -= 5;
	glutPostRedisplay();
	break;

      case 'z':
        texture = !texture;
	if (texture) {
	    gluQuadricTexture(quadObj, GL_TRUE);
	    glEnable(GL_TEXTURE_2D);
	} else {
	    gluQuadricTexture(quadObj, GL_FALSE);
	    glDisable(GL_TEXTURE_2D);
	}
	glutPostRedisplay();
	break;

      case 'q':
	glDisable(GL_CULL_FACE);
	glutPostRedisplay();
	break;
      case 'w':
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glutPostRedisplay();
	break;
      case 'e':
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glutPostRedisplay();
	break;

      case 'r':
	glFrontFace(GL_CW);
	glutPostRedisplay();
	break;
      case 't': 
	glFrontFace(GL_CCW);
	glutPostRedisplay();
	break;

      case 'y':
	doDither = !doDither;
	(doDither) ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
	glutPostRedisplay();
	break;

      case 27:
	exit(0);
    }
}

static void SpecialKey(int key, int x, int y)
{

    switch (key) {
      case GLUT_KEY_LEFT:
	yRotation += 5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_RIGHT:
	yRotation -= 5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_UP:
	xRotation += 5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_DOWN:
	xRotation -= 5;
	glutPostRedisplay();
	break;
    }
}

static void Draw(void)
{

    glLoadIdentity();
    glRotatef(xRotation, 1, 0, 0);
    glRotatef(yRotation, 0, 1, 0);
    glRotatef(zRotation, 0, 0, 1);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);
    switch (whichQuadric) {
      case 0:
	glTranslatef(0, 0, -height/20.0);
	gluCylinder(quadObj, radius1/10.0, radius2/10.0, height/10.0, 
		    slices, stacks);
	break;
      case 1:
	gluSphere(quadObj, radius1/10.0, slices, stacks);
	break;
      case 2:
	gluPartialDisk(quadObj, radius2/10.0, radius1/10.0, slices, 
		       stacks, angle1, angle2);
	break;
      case 3:
	gluDisk(quadObj, radius2/10.0, radius1/10.0, slices, stacks);
	break;
    }

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
	} else if (strcmp(argv[i], "-f") == 0) {
	    if (i+1 >= argc || argv[i+1][0] == '-') {
		printf("-f (No file name).\n");
		exit(1);
	    } else {
		image = rgbImageLoad(argv[++i]);
		if (image == NULL) {
		    printf("-f (Bad file name).\n");
		    exit(1);
		}
	    }
	}
    }
}

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    if (image == NULL) {
	printf("No texture file.\n");
	exit(1);
    }

    type = GLUT_RGB | GLUT_DEPTH;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Quad Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
