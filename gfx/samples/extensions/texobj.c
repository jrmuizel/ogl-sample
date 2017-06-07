/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.0 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
**
** $Date$ $Revision$
*/
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

#if defined(GL_EXT_texture_object)

#define NUM_LIST_TYPES 3
typedef enum {CUBE=0, SQUARE, PUSHPOP} listType;

listType currentList = SQUARE;

GLuint lists[NUM_LIST_TYPES];

GLenum doubleBuffer;

GLboolean multTex = GL_TRUE;
GLboolean immed = GL_FALSE;
GLboolean texObj = GL_TRUE;
GLboolean queryTexobj = GL_TRUE;

#define NUM_TEXTURES 3

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
float xRotation = 0.0, yRotation = 0.0, zTranslate = -3.125;

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
    {
	{ 1.1,  1.1 },
	{ -0.1, 1.1 },
	{ -0.1, -0.1 },
	{ 1.1,  -0.1 }
    },
    {
	{ 1.1,  1.1 },
	{ -0.1, 1.1 },
	{ -0.1, -0.1 },
	{ 1.1,  -0.1 }
    },
    {
	{ -0.1,  1.1 },
	{ 1.1, 1.1 },
	{ 1.1, -0.1 },
	{ -0.1,  -0.1 }
    },
    {
	{ 1.1,  1.1 },
	{ -0.1, 1.1 },
	{ -0.1, -0.1 },
	{ 1.1,  -0.1 }
    },
    {
	{ 1.1,  1.1 },
	{ -0.1, 1.1 },
	{ -0.1, -0.1 },
	{ 1.1,  -0.1 }
    },
    {
	{ 1.1,  1.1 },
	{ -0.1, 1.1 },
	{ -0.1, -0.1 },
	{ 1.1,  -0.1 }
    },
};


static void SelectTexture(GLint i)
{
    RGBImageRec *texImage;

    if (texObj) {
	if (i<NUM_TEXTURES) {
	    glBindTextureEXT(GL_TEXTURE_2D, texNames[i]);
	} else {
	    glBindTextureEXT(GL_TEXTURE_2D, 0);
	}
	return;
    }

    if (i >= NUM_TEXTURES) {
	i = NUM_TEXTURES - 1;
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

static void BuildSquare(void)
{
    GLint i;
    GLfloat xTranslate = 0.0;

    glNewList(lists[SQUARE], GL_COMPILE);
    DrawSquares();
    glEndList();
}

static void DoPushPopTest(void)
{
    GLfloat xTranslate = 0.0;

    if (multTex) {
	SelectTexture(1);
    }
    glTranslatef(-4.0, 0.0, 0.0);
    DrawSquare();

    glPushAttrib(GL_TEXTURE_BIT);
    if (multTex) {
	SelectTexture(2);
    }
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
    glTranslatef(2.0, 0.0, 0.0);
    DrawSquare();

    glPopAttrib();
    glTranslatef(2.0, 0.0, 0.0);
    DrawSquare();

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
    glTranslatef(2.0, 0.0, 0.0);
    DrawSquare();

}

static void BuildPushPopTest(void)
{
    GLfloat xTranslate = 0.0;

    glNewList(lists[PUSHPOP], GL_COMPILE);
    DoPushPopTest();
    glEndList();
}

static void BuildLists(void)
{

    lists[CUBE] = glGenLists(1);
    BuildCube();
    lists[SQUARE] = glGenLists(1);
    BuildSquare();
    lists[PUSHPOP] = glGenLists(1);
    BuildPushPopTest();
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
    zTranslate = -3.125;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (multTex && texObj) {
	int i;

	glGenTexturesEXT(NUM_TEXTURES, texNames);
	for (i = 1; i < NUM_TEXTURES; i++) {
	    glBindTextureEXT(GL_TEXTURE_2D, texNames[i]);
	    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, images[i]->sizeX,
			      images[i]->sizeY, GL_RGB, GL_UNSIGNED_BYTE,
			      images[i]->data);
	}
	glBindTextureEXT(GL_TEXTURE_2D, texNames[0]);
	glPrioritizeTexturesEXT(NUM_TEXTURES, texNames, texPriorities);
    }
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, images[0]->sizeX, images[0]->sizeY,
		      GL_RGB, GL_UNSIGNED_BYTE, images[0]->data);

    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, decal);
    glEnable(GL_TEXTURE_2D);

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
	glDeleteTexturesEXT(NUM_TEXTURES, texNames);
    }
    UnBuildLists();
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(145.0, 1.0, 0.01, 1000);
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
    {"P", "Push Pop test"},
    {"G", "Get attrib test"},
    {"",""},
    {"0", "magfilter = nearest"},
    {"1", "magfilter = linear"},
    {"2", "minfilter = nearest"},
    {"3", "minfilter = linear"},
    {"4", "minfilter = nearest mipmap nearest"},
    {"5", "minfilter = nearest mipmap linear"},
    {"6", "minfilter = linear mipmap nearest"},
    {"7", "minfilter = linear mipmap linear"},
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

      case 'r':
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
      case 'P':
	currentList = PUSHPOP;
	glutPostRedisplay();
	break;

      case 'G':
	{
	    GLint name;
	    GLfloat priority;
	    GLboolean isTex, resident, residences[NUM_TEXTURES];

	    glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &name);
	    glGetTexParameterfv(GL_TEXTURE_2D, 
				GL_TEXTURE_PRIORITY_EXT, &priority);
	    isTex = glIsTextureEXT(name);
	    resident = glAreTexturesResidentEXT(NUM_TEXTURES, 
						texNames, residences);
	    fprintf(stdout, "\tname=%d priority=%f allResident=%d isTex=%d\n",
		    name, priority, resident, isTex);
	}
	glutPostRedisplay();
        break;
	
      case '0':
	magFilter = nr;
	glutPostRedisplay();
	break;
      case '1':
	magFilter = ln;
	glutPostRedisplay();
	break;
      case '2':
	minFilter = nr;
	glutPostRedisplay();
	break;
      case '3':
	minFilter = ln;
	glutPostRedisplay();
	break;
      case '4':
	minFilter = nr_mipmap_nr;
	glutPostRedisplay();
	break;
      case '5':
	minFilter = nr_mipmap_ln;
	glutPostRedisplay();
	break;
      case '6':
	minFilter = ln_mipmap_nr;
	glutPostRedisplay();
	break;
      case '7':
	minFilter = ln_mipmap_ln;
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
	    case PUSHPOP:
		DoPushPopTest();
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
	} else if (strcmp(argv[i], "-immed") == 0) {
	    immed = GL_TRUE;
	} else if (strcmp(argv[i], "-pushpop") == 0) {
	    currentList = PUSHPOP;
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

static GLboolean QueryExtension(char *extName)
{
    /*
    ** Search for extName in the extensions string. Use of strstr()
    ** is not sufficient because extension names can be prefixes of
    ** other extension names. Could use strtok() but the constant
    ** string returned by glGetString can be in read-only memory.
    */
    char *p;
    char *end;
    int extNameLen;

    if (!queryTexobj) {
	return GL_TRUE;
    }

    extNameLen = strlen(extName);

    p = (char *)glGetString(GL_EXTENSIONS);
    if (NULL == p) {
	return GL_FALSE;
    }

    end = p + strlen(p);

    while (p < end) {
	int n = strcspn(p, " ");
	if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
	    return GL_TRUE;
	}
	p += (n + 1);
    }
    return GL_FALSE;
}

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Texture Test");

    if (texObj && !QueryExtension("GL_EXT_texture_object")) {
	fprintf(stdout, "Warning: texture object extension not found.\n");
	fprintf(stdout, "         Disabling texture objects.\n");
	texObj = GL_FALSE;
    }

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

#else

int main(int argc, char **argv)
{

    printf("Couldn't find GL_EXT_texture_object extension.\n");
}

#endif
