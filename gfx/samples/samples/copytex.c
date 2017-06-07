/*
** texobj.c: Draws textured polygons using the texture object extension.
**
** Default operation: Draws three squares using a different texture for
** each square.
**
** Use -h option to display a list of command line options. Use Key h 
** in the created window for key options.
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/glut.h>
#include <GL/rgb.h>

typedef struct {
    GLint x;
    GLint y;
    GLfloat width;
    GLfloat height;
    GLint border;
} boxStruct;
boxStruct copyBox;

int winWidth = 300;
int winHeight = 300;

GLenum target = GL_TEXTURE_2D;

#define NUM_LIST_TYPES 2
typedef enum {CUBE=0, SQUARE} listType;

listType currentList = SQUARE;

GLuint lists[NUM_LIST_TYPES];

GLenum doubleBuffer;

GLboolean multTex = GL_TRUE;
GLboolean immed = GL_FALSE;
GLboolean texObj = GL_TRUE;
GLboolean copyTex = GL_TRUE;
GLboolean subTex = GL_TRUE;
GLboolean queryTexobj = GL_TRUE;

#define NUM_TEXTURES 3

int texNum = 2;
GLuint texNames[NUM_TEXTURES];
GLfloat texPriorities[NUM_TEXTURES] = {0.1, 0.2, 0.3};
RGBImageRec *images[NUM_TEXTURES];
char *texFiles[NUM_TEXTURES] = {
	"1.rgb",
	"2.rgb",
	"3.rgb",
};

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
GLint sphereMap[] = {GL_SPHERE_MAP};

GLenum doSphere = GL_FALSE;
float xRotation = 0.0, yRotation = 0.0, zTranslate = 0.0;

float c[6][4][3] = {
    {
	{ 1.0, 1.0, -1.0 },
	{ -1.0, 1.0, -1.0 },
	{ -1.0, -1.0, -1.0 },
	{ 1.0, -1.0, -1.0 }
    },
    {
	{ 1.0, 1.0, 1.0 },
	{ 1.0, 1.0, -1.0 },
	{ 1.0, -1.0, -1.0 },
	{ 1.0, -1.0, 1.0 }
    },
    {
	{ -1.0, 1.0, 1.0 },
	{ 1.0, 1.0, 1.0 },
	{ 1.0, -1.0, 1.0 },
	{ -1.0, -1.0, 1.0 }
    },
    {
	{ -1.0, 1.0, -1.0 },
	{ -1.0, 1.0, 1.0 },
	{ -1.0, -1.0, 1.0 },
	{ -1.0, -1.0, -1.0 }
    },
    {
	{ -1.0, 1.0, 1.0 },
	{ -1.0, 1.0, -1.0 },
	{ 1.0, 1.0, -1.0 },
	{ 1.0, 1.0, 1.0 }
    },
    {
	{ -1.0, -1.0, -1.0 },
	{ -1.0, -1.0, 1.0 },
	{ 1.0, -1.0, 1.0 },
	{ 1.0, -1.0, -1.0 }
    }
};
static float n[6][3] = {
    { 0.0, 0.0, -1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { -1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, -1.0, 0.0 }
};
static float t[6][4][2] = {
    { { 1.1,  1.1 }, { -0.1, 1.1 }, { -0.1, -0.1 }, { 1.1,  -0.1 } },
    { { 1.1,  1.1 }, { -0.1, 1.1 }, { -0.1, -0.1 }, { 1.1,  -0.1 } },
    { { -0.1,  1.1 }, { 1.1, 1.1 }, { 1.1, -0.1 }, { -0.1,  -0.1 } },
    { { 1.1,  1.1 }, { -0.1, 1.1 }, { -0.1, -0.1 }, { 1.1,  -0.1 } },
    { { 1.1,  1.1 }, { -0.1, 1.1 }, { -0.1, -0.1 }, { 1.1,  -0.1 } },
    { { 1.1,  1.1 }, { -0.1, 1.1 }, { -0.1, -0.1 }, { 1.1,  -0.1 } },
};


static void SelectTexture(GLint i)
{
    RGBImageRec *texImage;

    if (texObj) {
	if (i<NUM_TEXTURES) {
	    glBindTexture(GL_TEXTURE_2D, texNames[i]);
	}else {
	    glBindTexture(GL_TEXTURE_2D, 0);
	}
	return;
    }

    if (i >= NUM_TEXTURES) {
	i = NUM_TEXTURES-1;
    }

    texImage = images[i];

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texImage->sizeX, texImage->sizeY,
		      GL_RGB, GL_UNSIGNED_BYTE, texImage->data);
}

static void DrawCube(void)
{
    GLint i;

    for (i = 0; i < 6; i++) {
	if (multTex) {
	    SelectTexture(i);
	}
	glBegin(GL_POLYGON);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][0]); glVertex3fv(c[i][0]);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][1]); glVertex3fv(c[i][1]);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][2]); glVertex3fv(c[i][2]);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][3]); glVertex3fv(c[i][3]);
	glEnd();
    }
}

static void BuildCube(void)
{
    GLint i;

    glNewList(lists[CUBE], GL_COMPILE);
    DrawCube();
    glEndList();
}

static void DrawSquare(void)
{
    glBegin(GL_POLYGON);
	glNormal3fv(n[2]); glTexCoord2fv(t[2][0]); glVertex3fv(c[2][0]);
	glNormal3fv(n[2]); glTexCoord2fv(t[2][1]); glVertex3fv(c[2][1]);
	glNormal3fv(n[2]); glTexCoord2fv(t[2][2]); glVertex3fv(c[2][2]);
	glNormal3fv(n[2]); glTexCoord2fv(t[2][3]); glVertex3fv(c[2][3]);
    glEnd();
}

static void DrawSquares(void)
{
    GLint i;
    GLfloat xTranslate = 0.0;

    glTranslatef(-2.0, 0.0, 0.0);
    for (i = 0; i < 3; i++) {
	if (multTex) {
	    SelectTexture(i);
	}
	DrawSquare();
	glTranslatef(2.0, 0.0, 0.0);
    }
}

static void CopyTexture(boxStruct *box, GLboolean texSubImage)
{
    GLenum texImageFormat = GL_RGB;
    GLint xOffset, yOffset;

    xOffset = yOffset = 32;

    glBindTexture(GL_TEXTURE_2D, texNames[texNum]);

    if (copyTex) {
	if (texSubImage) {
	    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset,
				   box->x, box->y, box->width, box->height);
	} else {
	    glCopyTexImage2D(GL_TEXTURE_2D, 0, texImageFormat, box->x,
				box->y, box->width, box->height, box->border);
	}
    }
    else {
	GLenum components;
	static GLfloat *pixels = NULL;
	static int numPixels = 0, numAlloced = 0;

	numPixels = box->width * box->height;

	switch (texImageFormat) {
	    case GL_COLOR_INDEX:
	    case GL_RED:
	    case GL_GREEN:
	    case GL_BLUE:
	    case GL_ALPHA:
	    case GL_LUMINANCE:
		components = 1;
		break;
	    case GL_LUMINANCE_ALPHA:
		components = 2;
		break;
	    case GL_RGB:
		components = 3;
		break;
	    case GL_RGBA:
		components = 4;
		break;
	} 

	if (NULL == pixels) {
	    pixels = (GLfloat *)malloc(numPixels*components*sizeof(GLfloat));
	} else if (numPixels > numAlloced) {
	    pixels = (GLfloat *)realloc(pixels,
					numPixels*components*sizeof(GLfloat));
	}
	if (NULL == pixels) {
	    numPixels = numAlloced = 0;
	    return;
	}
	numAlloced = numPixels;

	glReadPixels(box->x, box->y, box->width, box->height, texImageFormat, 
		     GL_FLOAT, pixels);
	if (texSubImage) {
	    glTexSubImage2D(GL_TEXTURE_2D, 0, box->x, box->y, box->width,
			       box->height, texImageFormat, GL_FLOAT, pixels);
	} else {
	    glTexImage2D(GL_TEXTURE_2D, 0, components, box->width, box->height, 
			 box->border, texImageFormat, GL_FLOAT, pixels);
	}
    }
}

static void BuildSquare(void)
{
    GLint i;
    GLfloat xTranslate = 0.0;

    glNewList(lists[SQUARE], GL_COMPILE);
    DrawSquares();
    glEndList();
}

static void BuildLists(void)
{

    lists[CUBE] = glGenLists(1);
    BuildCube();
    lists[SQUARE] = glGenLists(1);
    BuildSquare();
}

static void UnBuildLists(void)
{
    int i;

    for (i = 0; i < NUM_LIST_TYPES; i++) {
	glDeleteLists(lists[i], 1);
    }
}

static void Init(void)
{

    doSphere = GL_FALSE;
    xRotation = 0.0;
    yRotation = 0.0;
    zTranslate = -3.425;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glEnable(GL_TEXTURE_2D);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, decal);

    if (multTex && texObj) {
	int i;

	glGenTextures(NUM_TEXTURES, texNames);
	for (i = 1; i < NUM_TEXTURES; i++) {
	    glBindTexture(GL_TEXTURE_2D, texNames[i]);
	    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, images[i]->sizeX,
			      images[i]->sizeY, GL_RGB, GL_UNSIGNED_BYTE,
			      images[i]->data);
	}
	glBindTexture(GL_TEXTURE_2D, texNames[0]);
	glPrioritizeTextures(NUM_TEXTURES, texNames, texPriorities);
    }
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, images[0]->sizeX, images[0]->sizeY,
		      GL_RGB, GL_UNSIGNED_BYTE, images[0]->data);

    glFrontFace(GL_CCW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    BuildLists();

    glClearColor(0.0, 0.0, 0.0, 0.0);

    magFilter = nr;
    minFilter = nr;
    sWrapMode = repeat;
    tWrapMode = repeat;
}

static void UnInit(void)
{

    if (multTex && texObj) {
	glDeleteTextures(NUM_TEXTURES, texNames);
    }
    UnBuildLists();
}

static void Reshape(int width, int height)
{

    winWidth = width; winHeight = height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, 1.0, 0.01, 1000);
    glMatrixMode(GL_MODELVIEW);
}

static char *keyList[][2] = {
    {"left arrow", "y rotation - 0.5"},
    {"right arrow", "y rotation + 0.5"},
    {"up arrow", "x rotation - 0.5"},
    {"down arrow", "x rotation + 0.5"},
    {"T", "z translate  + 0.25"},
    {"t", "z translate  - 0.25"},
    {"",""},
    {"s", "toggle texgen sphereMap"},
    {"R", "Reset draw routine"},
    {"C", "Draw Cube"},
    {"S", "Draw Squares"},
    {"G", "Get attrib test"},
    {"Y", "Copy(Yank) screen center area into current texture"},
    {"y", "Copy(Yank) screen center area into subarea of current texture"},
    {"",""},
    {"0", "Set current texture to 0"},
    {"1", "Set current texture to 1"},
    {"2", "Set current texture to 2"},
    {"3", "Set current texture to 3"},
    {"",""},
    {"h", "print this list"},
    {"ESC", "exit"},
    {NULL, NULL}
};

static void Key(unsigned char key, int x, int y)
{
    int i;

    switch (key) {
      case 'h':
	for (i = 0; keyList[i][0] != NULL; i++) {
	    printf("%-20s %s\n", keyList[i][0], keyList[i][1]);
	}
	break;

      case 'T':
	zTranslate += 0.25;
	glutPostRedisplay();
	break;
      case 't':
	zTranslate -= 0.25;
	glutPostRedisplay();
	break;

      case 's':
	doSphere = !doSphere;
	if (doSphere) {
	    glTexGeniv(GL_S, GL_TEXTURE_GEN_MODE, sphereMap);
	    glTexGeniv(GL_T, GL_TEXTURE_GEN_MODE, sphereMap);
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	} else {
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
	}
	glutPostRedisplay();
	break;

      case 'R':
	UnInit();
	Init();
	glutPostRedisplay();
	break;

      case 'C':
	currentList = CUBE;
	glutPostRedisplay();
	break;
      case 'S':
	currentList = SQUARE;
	glutPostRedisplay();
	break;

      case 'G':
	{
	    GLint name;
	    GLfloat priority;
	    GLboolean isTex, resident, residences[NUM_TEXTURES];

	    glGetIntegerv(GL_TEXTURE_BINDING_2D, &name);
	    glGetTexParameterfv(GL_TEXTURE_2D, 
				GL_TEXTURE_PRIORITY, &priority);
	    isTex = glIsTexture(name);
	    resident = glAreTexturesResident(NUM_TEXTURES, 
						texNames, residences);
	    fprintf(stdout, "\tname=%d priority=%f allResident=%d isTex=%d\n", 
	            name, priority, resident, isTex);
	}
	glutPostRedisplay();
        break;
	
      case 'Y':
	/*
	** Read the pixels from the center of the screen.
	*/
	copyBox.border = 0;
	copyBox.width = copyBox.height = 64 + 2 * copyBox.border;
	copyBox.x = (winWidth - copyBox.width) / 2.0;
	copyBox.y = (winHeight - copyBox.height) / 2.0;
	CopyTexture(&copyBox, GL_FALSE);
	glutPostRedisplay();
	break;

      case 'y':
	copyBox.border = 0;
	copyBox.width = copyBox.height = 64 + 2 * copyBox.border;
	copyBox.x = (winWidth - copyBox.width) / 2.0;
	copyBox.y = (winHeight - copyBox.height) / 2.0;
	CopyTexture(&copyBox, subTex);
	glutPostRedisplay();
	break;

      case '0':
	texNum = 0;
	glutPostRedisplay();
	break;
      case '1':
	texNum = 1;
	glutPostRedisplay();
	break;
      case '2':
	texNum = 2;
	glutPostRedisplay();
	break;
      case '3':
	texNum = 3;
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
	yRotation -= 0.5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_RIGHT:
	yRotation += 0.5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_UP:
	xRotation -= 0.5;
	glutPostRedisplay();
	break;
      case GLUT_KEY_DOWN:
	xRotation += 0.5;
	glutPostRedisplay();
	break;
    }
}

static void Draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapMode);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapMode);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

    glPushMatrix();

    glTranslatef(0.0, 0.0, zTranslate);
    glRotatef(xRotation, 1, 0, 0);
    glRotatef(yRotation, 0, 1, 0);

    if (immed) { 		
	switch(currentList) {
	case SQUARE:
	    DrawSquares();
	    break;
	case CUBE:
	    DrawCube();
	    break;
	}
    } else {
	glCallList(lists[currentList]);
    }

    glPopMatrix();

    if (doubleBuffer) {
	glutSwapBuffers();
    } else {
	glFlush();
    }
}

static char *argList[][2] = {
    {"-sb", "single buffered"},
    {"-db", "double buffered"},
    {"-dr", "direct rendering"},
    {"-ir", "indirect rendering"},
    {"-f filename", "filename for texture"},
    {"-texobj", "use texture objects"},
    {"-notexobj", "don't use texture objects"},
    {"-copytex", "use copy texture extension"},
    {"-nocopytex", "don't use copy texture extension"},
    {"-immed", "immediate mode"},
    {"-square", "draw squares"},
    {"-cube", "draw cube"},
    {"-pushpop", "push pop test"},
    {"-nq", "skip query texobj extension"},
    {"-h", "display this list"},
    {"Press H in app window to get key interpretations", ""},
    {NULL, NULL}
};

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
		texFiles[0] = argv[++i];
		multTex = GL_FALSE;
	    }
	} else if (strcmp(argv[i], "-flat") == 0) {
	    currentList = SQUARE;
	} else if (strcmp(argv[i], "-texobj") == 0) {
	    texObj = GL_TRUE;
	} else if (strcmp(argv[i], "-notexobj") == 0) {
	    texObj = GL_FALSE;
	} else if (strcmp(argv[i], "-copytex") == 0) {
	    copyTex = GL_TRUE;
	} else if (strcmp(argv[i], "-nocopytex") == 0) {
	    copyTex = GL_FALSE;
	} else if (strcmp(argv[i], "-immed") == 0) {
	    immed = GL_TRUE;
	} else if (strcmp(argv[i], "-cube") == 0) {
	    currentList = CUBE;
	} else if (strcmp(argv[i], "-nq") == 0) {
	    queryTexobj = GL_FALSE;
	} else if (strcmp(argv[i], "-h") == 0) {
	    for (i = 0; argList[i][0] != NULL; i++) {
		printf("%-30s %s\n", argList[i][0], argList[i][1]);
	    }
	    exit(0);
	}
    }
}

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Copy Texture Test");

    images[0] = rgbImageLoad(texFiles[0]);
    if (images[0] == NULL) {
	printf("Bad texture file.\n");
	exit(1);
    }
    if (multTex) {
	int i;
	for (i = 1; i < NUM_TEXTURES; i++) {
	    images[i] = rgbImageLoad(texFiles[i]);
	    if (images[i] == NULL) {
		printf("Bad texture file.\n");
		exit(1);
	    }
	}
    }

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
