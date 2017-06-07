/* vertarr.c
** This program demonstrates using the vertex array extension.
** it uses vertex array in immediate mode, and in display lists.
** "a" toggles vertex arrays
** "d" toggles display lists
** "m" toggles between compile and compile and execute mode
**      (works only if display lists are on)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>


#define COLOR_OFFSET_1 16
#define COLOR_OFFSET_2 32

#define CHECK_ERROR \
	err = glGetError(); \
	if (err) { \
	    printf("Error %d at line %d\n", err, __LINE__); \
	}

static GLubyte texture [768];

static GLint verts[] = {
    10, 10, 0,
    200, 100, 0,
    50, 200, 0
};
static GLfloat colors[] = {
    1.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.5, 0.2, 0.8
};
static GLfloat cv[] = { 
    1.0, 1.0, 0.0,
    430, 30,
    1.0, 0.0, 1.0,
    480, 300,
    1.0, 1.0, 1.0,
    380, 200
};
static GLshort tv[] = {
    0, 0,
    5, 400,
    1, 0,
    100, 400,
    1, 1,
    100, 495,
    0, 1,
    5, 495
};
static GLfloat verts2[] = {
    350.0, 450.0, 1.0, 1.0,
    350.0, 350.0, 1.0, 1.0,
    400.0, 325.0, 1.0, 1.0,
    450.0, 350.0, 1.0, 1.0,
    450.0, 450.0, 1.0, 1.0,
    400.0, 475.0, 1.0, 1.0
};
static GLfloat norms[] = {
    0, 0, 1, 
    0, 0, 1, 
    0, 0, 1, 
    0, 0, 1,
    0, 0, -1, 
    0, 0, -1, 
    0, 0, -1, 
    0, 0, -1
};
static GLint verts3[] = { 
    150, 200, 
    250, 200, 
    250, 300, 
    150, 300, 
    250, 200, 
    350, 200, 
    350, 300, 
    250, 300
};
static GLboolean edges[] = {
    GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE
};

GLenum rgb, err;
int drawArray = 0;
int dlist = 0; /* 0 = immediate mode, ~0 = display list */
GLenum dlMode = GL_COMPILE;


static void SetUpColorMap(void)
{
    long i;

    for (i = 0; i < 16; i++) {
	glutSetColor(i+COLOR_OFFSET_1, 0.0, 0.0, i/15.0);
	glutSetColor(i+COLOR_OFFSET_2, 0.0, i/15.0, 0.0);
    }
}

static void Init(void)
{
    int i;

    if (!rgb) {
	SetUpColorMap();
    }

    glClearColor(0, 0, 0, 0);

    glShadeModel(GL_SMOOTH);

    /* create texture image */
    for (i = 0; i < 192; i+=3) {
       texture[i] = 0xff; texture[i+1] = 0xff; texture[i+2] = 0x00;
    }
    for (; i < 2*192; i+=3) {
       texture[i] = 0xff; texture[i+1] = 0x00; texture[i+2] = 0xff;
    }
    for (; i < 3*192; i+=3) {
       texture[i] = 0x00; texture[i+1] = 0xff; texture[i+2] = 0xff;
    }
    for (; i < 4*192; i+=3) {
       texture[i] = 0x00; texture[i+1] = 0x00; texture[i+2] = 0xff;
    }
}

static void Reshape(int width, int height)
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 500.0, 0.0, 500.0, 0.0, -1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 'a':
	  drawArray = !drawArray;
	  glutPostRedisplay();
	  break;
      case 'd':
	  dlist = !dlist;
	  glutPostRedisplay();
	  break;
      case 'm':
	  if (dlist) {
	    if (dlMode == GL_COMPILE) {
	      dlMode = GL_COMPILE_AND_EXECUTE;
	    } else {
	      dlMode = GL_COMPILE;
	    }
	  }
	  glutPostRedisplay();
	  break;
      case 27:
	exit(0);
    }
}

void TestGets(void)
{
    GLint size, stride, type, count;
    GLvoid *ptr;
    
    glGetIntegerv(GL_VERTEX_ARRAY_SIZE, &size);
    glGetIntegerv(GL_VERTEX_ARRAY_TYPE, &type);
    glGetIntegerv(GL_VERTEX_ARRAY_STRIDE, &stride);
    printf("VERTEX_ARRAY size = %d, type = %d, stride = %d\n\n",
	   size, type, stride);
    CHECK_ERROR

    glGetIntegerv(GL_NORMAL_ARRAY_TYPE, &type);
    glGetIntegerv(GL_NORMAL_ARRAY_STRIDE, &stride);
    printf("NORMAL_ARRAY type = %d, stride = %d\n\n", type, stride);
    CHECK_ERROR

    glGetIntegerv(GL_COLOR_ARRAY_SIZE, &size);
    glGetIntegerv(GL_COLOR_ARRAY_TYPE, &type);
    glGetIntegerv(GL_COLOR_ARRAY_STRIDE, &stride);
    printf("COLOR_ARRAY size = %d, type = %d, stride = %d\n\n",
	   size, type, stride);
    CHECK_ERROR

    glGetIntegerv(GL_INDEX_ARRAY_TYPE, &type);
    glGetIntegerv(GL_INDEX_ARRAY_STRIDE, &stride);
    printf("INDEX_ARRAY type = %d, stride = %d\n\n", type, stride);
    CHECK_ERROR

    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_SIZE, &size);
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_TYPE, &type);
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_STRIDE, &stride);
    printf("TEXTURE_COORD_ARRAY size = %d, type = %d, stride = %d\n\n",
	   size, type, stride);
    CHECK_ERROR

    glGetIntegerv(GL_EDGE_FLAG_ARRAY_STRIDE, &stride);
    printf("EDGE_FLAG_ARRAY stride = %d\n\n", stride);
    CHECK_ERROR

    glGetPointerv(GL_VERTEX_ARRAY_POINTER, &ptr);
    printf("VERTEX_ARRAY pointer = %x\n", ptr);
    glGetPointerv(GL_NORMAL_ARRAY_POINTER, &ptr);
    printf("NORMAL_ARRAY pointer = %x\n", ptr);
    glGetPointerv(GL_COLOR_ARRAY_POINTER, &ptr);
    printf("COLOR_ARRAY pointer = %x\n", ptr);
    glGetPointerv(GL_INDEX_ARRAY_POINTER, &ptr);
    printf("INDEX_ARRAY pointer = %x\n", ptr);
    glGetPointerv(GL_TEXTURE_COORD_ARRAY_POINTER, &ptr);
    printf("TEXTURE_COORD_ARRAY pointer = %x\n", ptr);
    glGetPointerv(GL_EDGE_FLAG_ARRAY_POINTER, &ptr);
    printf("EDGE_FLAG_ARRAY pointer = %x\n", ptr);
    CHECK_ERROR;
}

static void Draw(void)
{
    int i;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);
    glLineWidth(1.0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);

    glClear(GL_COLOR_BUFFER_BIT);

    glVertexPointer(3, GL_INT, 0, verts);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    CHECK_ERROR

    if (dlist) {
	glNewList(1, dlMode);
    }
    if (drawArray) {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    } else {
	glBegin(GL_TRIANGLES);
	    for (i = 0; i < 3; i++) {
		glArrayElement(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(1);
    }

    CHECK_ERROR

    glVertexPointer(2, GL_FLOAT, 20, &cv[3]);
    glColorPointer(3, GL_FLOAT, 20, cv);

    if (dlist) {
	glNewList(2, dlMode);
    }
    if (drawArray) {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    } else {
	glBegin(GL_TRIANGLES);
	    for (i = 0; i < 3; i++) {
		glArrayElement(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(2);
    }
    glDisableClientState(GL_COLOR_ARRAY);

    if (dlist) {
	glDeleteLists(1, 2);
    }

    glVertexPointer(2, GL_SHORT, 8, &tv[2]);
    glTexCoordPointer(2, GL_SHORT, 8, tv);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if (dlist) {
	glNewList(3, dlMode);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE,
		 texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if (drawArray) {
	glDrawArrays(GL_POLYGON, 0, 4);
    } else {
	glBegin(GL_POLYGON);
	    for (i = 0; i < 4; i++) {
		glArrayElement(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(3);
    }
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableClientState(GL_EDGE_FLAG_ARRAY);
    glVertexPointer(4, GL_FLOAT, 0, verts2);
    
    if (dlist) {
	glNewList(4, dlMode);
    }
    glEdgeFlagPointer(0, edges);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glShadeModel(GL_FLAT);
    glLineWidth(4.0);
    glColor3f(1.0, 0.0, 0.0);

    if (drawArray) {
        glDrawArrays(GL_POLYGON, 0, 6);
    } else {
	glBegin(GL_POLYGON);
	    for (i = 0; i < 6; i++) {
		glArrayElement(i);
	    }
	glEnd();
    }
    if (dlist) {
	glEndList();
	glCallList(4);
    }

    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    {
	GLfloat back_light_pos[] = {0.0, 0.0, -1.0, 1.0};
	GLfloat back_light_dir[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat amb[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, red);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, blue);
	glLightfv(GL_LIGHT1, GL_POSITION, back_light_pos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, back_light_dir);

    }
    glShadeModel(GL_SMOOTH);
    glVertexPointer(2, GL_INT, 0, verts3);
    glNormalPointer(GL_FLOAT, 0, norms);
    glEnableClientState(GL_NORMAL_ARRAY);

    if (dlist) {
	glNewList(5, dlMode);
    }
    glBegin(GL_POLYGON);
	for (i = 0; i < 4; i++) {
	    glArrayElement(i);
	}
    glEnd();
    glBegin(GL_POLYGON);
	for (; i < 8; i++) {
	    glArrayElement(i);
	}
    glEnd();

    if (dlist) {
	glEndList();
	glCallList(5);
    }
    glDisable(GL_LIGHTING);
    glDisableClientState(GL_NORMAL_ARRAY);

    {
	static GLfloat stripVertex[128*2][2];
	static GLfloat stripColor[128*2][3];
	int numSteps = 128;
	int numSectors = 3;

	float stepSize = 2 * M_PI / (numSteps - 1);
	float sectorSize = 2 * M_PI / numSectors;
	float innerRadius = 40;
	float outerRadius = 80;
	float xCenter = 225;
	float yCenter = 400;

	for (i = 0; i < numSteps*2; i += 2) {
	    float angle = i/2 * stepSize;
	    float x = cos(angle);
	    float y = sin(angle);
	    float sector = angle / sectorSize;
	    float fade = sector - (int)sector;
	    float r, g, b;
	    stripVertex[i][0] = innerRadius * x + xCenter;
	    stripVertex[i][1] = innerRadius * y + yCenter;
	    stripVertex[i+1][0] = outerRadius * x + xCenter;
	    stripVertex[i+1][1] = outerRadius * y + yCenter;
	    switch ((int) sector) {
	      case 0: case 3:
		/* red fade to green */
		r = 1.00 * (1 - fade) + 0.25 * fade;
		g = 0.25 * (1 - fade) + 1.00 * fade;
		b = 0.25 * (1 - fade) + 0.25 * fade;
		break;
	      case 1:
		/* green fade to blue */
		r = 0.25 * (1 - fade) + 0.25 * fade;
		g = 1.00 * (1 - fade) + 0.25 * fade;
		b = 0.25 * (1 - fade) + 1.00 * fade;
		break;
	      case 2:
		/* blue fade to red */
		r = 0.25 * (1 - fade) + 1.00 * fade;
		g = 0.25 * (1 - fade) + 0.25 * fade;
		b = 1.00 * (1 - fade) + 0.25 * fade;
		break;
	    }
	    stripColor[i][0] = stripColor[i+1][0] = r;
	    stripColor[i][1] = stripColor[i+1][1] = g;
	    stripColor[i][2] = stripColor[i+1][2] = b;
	}

	glVertexPointer(2, GL_FLOAT, 0, stripVertex);
	glColorPointer(3, GL_FLOAT, 0, stripColor);
	glEnableClientState(GL_COLOR_ARRAY);
	if (dlist) {
	    glNewList(6, dlMode);
	}
	if (drawArray) {
	    glDrawArrays(GL_TRIANGLE_STRIP, 0, numSteps*2);
	} else {
	    glBegin(GL_TRIANGLE_STRIP);
	    for (i = 0; i < numSteps*2; i += 2) {
		glArrayElement(i);
		glArrayElement(i+1);
	    }
	    glEnd();
	}
	if (dlist) {
	    glEndList();
	    glCallList(6);
	}
	glDisableClientState(GL_COLOR_ARRAY);
    }

    CHECK_ERROR
    glFlush();
    TestGets();
}

static void Args(int argc, char **argv)
{
    GLint i;

    rgb = GL_TRUE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-ci") == 0) {
	    rgb = GL_FALSE;
	} else if (strcmp(argv[i], "-rgb") == 0) {
	    rgb = GL_TRUE;
	}
    }
}

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    type = GLUT_SINGLE;
    type |= (rgb) ? GLUT_RGB : GLUT_INDEX;
    glutInitDisplayMode(type);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Test");

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutMainLoop();
}

