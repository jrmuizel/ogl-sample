#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/rgb.h>


GLenum doubleBuffer;
int winW = 300, winH = 300;

RGBImageRec *earthImage = NULL, *skyImage = NULL;
GLint skyList, earthList;

float *minFilter, *magFilter, *sWrapMode, *tWrapMode;
float decal[] = {GL_DECAL};
float modulate[] = {GL_MODULATE};
float repeat[] = {GL_REPEAT};
float clamp[] = {GL_CLAMP};
float nr[] = {GL_NEAREST};
float ln[] = {GL_LINEAR};
float nr_mipmap_nr[] = {GL_NEAREST_MIPMAP_NEAREST};
float nr_mipmap_ln[] = {GL_NEAREST_MIPMAP_LINEAR};
float ln_mipmap_nr[] = {GL_LINEAR_MIPMAP_NEAREST};
float ln_mipmap_ln[] = {GL_LINEAR_MIPMAP_LINEAR};

int horizon;
float texMinX, texMinY, texMaxX, texMaxY;


static void Init(void)
{

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    magFilter = nr;
    minFilter = nr;
    sWrapMode = repeat;
    tWrapMode = repeat;

    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, decal);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapMode);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapMode);
    glEnable(GL_TEXTURE_2D);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    horizon = winH / 2;

    texMinX = 0.25;
    texMaxX = 0.75;

    texMinY = 0.25;
    texMaxY = 0.75;

    skyList = glGenLists(1);
    glNewList(skyList, GL_COMPILE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, skyImage->sizeX, skyImage->sizeY,
		      GL_RGB, GL_UNSIGNED_BYTE, skyImage->data);
    glEndList();

    earthList = glGenLists(1);
    glNewList(earthList, GL_COMPILE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, earthImage->sizeX, earthImage->sizeY,
		      GL_RGB, GL_UNSIGNED_BYTE, earthImage->data);
    glEndList();
}

static void Reshape(int width, int height)
{

    winW = width;
    winH = height;

    glViewport(0, 0, winW, winH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case '1':
	horizon -= 5;
	texMinY -= 5.0 / (float)winH;
	texMinY += 5.0 / (float)winH;
	texMaxY += 5.0 / (float)winH;
	glutPostRedisplay();
	break;
      case '2':
	horizon += 5;
	texMinY += 5.0 / (float)winH;
	texMinY -= 5.0 / (float)winH;
	texMaxY -= 5.0 / (float)winH;
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
	texMinX -= 5.0 / (float)winW;
	texMaxX -= 5.0 / (float)winW;
	glutPostRedisplay();
	break;
      case GLUT_KEY_RIGHT:
	texMinX += 5.0 / (float)winW;
	texMaxX += 5.0 / (float)winW;
	glutPostRedisplay();
	break;
      case GLUT_KEY_UP:
	texMinY += 5.0 / (float)winH;
	texMaxY += 5.0 / (float)winH;
	glutPostRedisplay();
	break;
      case GLUT_KEY_DOWN:
	texMinY -= 5.0 / (float)winH;
	texMaxY -= 5.0 / (float)winH;
	glutPostRedisplay();
	break;
    }
}

static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

    glCallList(skyList); 
    glBegin(GL_POLYGON);
	glTexCoord2f(texMinX, texMinY);
	glVertex2i(0, horizon);
	glTexCoord2f(texMaxX, texMinY);
	glVertex2i(winW, horizon);
	glTexCoord2f(texMaxX, texMaxY);
	glVertex2i(winW, winH);
	glTexCoord2f(texMinX, texMaxY);
	glVertex2i(0, winH);
    glEnd();

    glCallList(earthList); 
    glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex2i(0, 0);
	glTexCoord2f(1.0, 0.0);
	glVertex2i(winW, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex2i(winW, horizon);
	glTexCoord2f(0.0, 1.0);
	glVertex2i(0, horizon);
    glEnd();

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
	} else if (strcmp(argv[i], "-earth") == 0) {
	    if (i+1 >= argc || argv[i+1][0] == '-') {
		printf("-earth (No file name).\n");
		exit(1);
	    } else {
		earthImage = rgbImageLoad(argv[++i]);
		if (earthImage == NULL) {
		    printf("-earth (Bad file name).\n");
		    exit(1);
		}
	    }
	} else if (strcmp(argv[i], "-sky") == 0) {
	    if (i+1 >= argc || argv[i+1][0] == '-') {
		printf("-sky (No file name).\n");
		exit(1);
	    } else {
		skyImage = rgbImageLoad(argv[++i]);
		if (skyImage == NULL) {
		    printf("-sky (Bad file name).\n");
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

    if (earthImage == NULL) {
	printf("No earth texture file.\n");
	exit(1);
    }
    if (skyImage == NULL) {
	printf("No sky texture file.\n");
	exit(1);
    }

    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Two Texture Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
