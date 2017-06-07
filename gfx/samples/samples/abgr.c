/*
** abgr.c - Demonstrates the use of the extension EXT_abgr.
**
** The same image data is used for both ABGR and RGBA formats
** in glDrawPixels and glTexImage2D.  The left side uses ABGR,
** the right side RGBA.  The top polygon demonstrates use of texture,
** and the bottom image is drawn with glDrawPixels.
**
** Note that the textures are defined as 3 component, so the alpha
** value is not used in applying the DECAL environment.
*/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <GL/glut.h>

#if defined(GL_EXT_abgr)

GLenum doubleBuffer;
GLubyte ubImage[65536];


static void Init(void)
{
    int i, j;
    GLubyte *img;
    GLsizei imgWidth = 128;

    glDisable(GL_DITHER);

    /* Create image */
    img = ubImage;
    for (j = 0; j < 32 * imgWidth; j++) {
	*img++ = 0xff;
	*img++ = 0x00;
	*img++ = 0x00;
	*img++ = 0xff;
    }
    for (j = 0; j < 32 * imgWidth; j++) {
	*img++ = 0xff;
	*img++ = 0x00;
	*img++ = 0xff;
	*img++ = 0x00;
    }
    for (j = 0; j < 32 * imgWidth; j++) {
	*img++ = 0xff;
	*img++ = 0xff;
	*img++ = 0x00;
	*img++ = 0x00;
    }
    for (j = 0; j < 32 * imgWidth; j++) {
	*img++ = 0x00;
	*img++ = 0xff;
	*img++ = 0x00;
	*img++ = 0xff;
    }
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{

    switch (key) {
      case 27:
	exit(0);
    }
}

void TexFunc(void)
{
    GLenum err;

    glTexImage2D(GL_TEXTURE_2D, 0, 3, 128, 128, 0, GL_ABGR_EXT,
                 GL_UNSIGNED_BYTE, ubImage);

    err = glGetError();
    if (err) {
	printf("err %d\n", err);
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    err = glGetError();
    if (err) {
	printf("err %d\n", err);
    }

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_POLYGON);
        glTexCoord2f(1.0, 1.0); glVertex3f(-0.2, 0.8, -100.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.8, 0.8, -2.0);
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.8, 0.2, -2.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(-0.2, 0.2, -100.0);
    glEnd();

    glTexImage2D(GL_TEXTURE_2D, 0, 3, 128, 128, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, ubImage);

    glBegin(GL_POLYGON);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.8, 0.8, -2.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.2, 0.8, -100.0);
        glTexCoord2f(0.0, 0.0); glVertex3f(0.2, 0.2, -100.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.8, 0.2, -2.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    err = glGetError();
    if (err) {
	printf("err %d\n", err);
    }
}

static void Draw(void)
{

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glRasterPos3f(-0.8, -0.8, -1.5);
    glDrawPixels(128, 128, GL_ABGR_EXT, GL_UNSIGNED_BYTE, ubImage);

    glRasterPos3f(0.2, -0.8, -1.5);
    glDrawPixels(128, 128, GL_RGBA, GL_UNSIGNED_BYTE, ubImage);

    TexFunc();

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

static GLboolean QueryExtension(char *extName)
{
    /*
    ** Search for extName in the extensions string. Use of strstr()
    ** is not sufficient because extension names can be prefixes of
    ** other extension names. Could use strtok() but the constant
    ** string returned by glGetString can be in read-only memory.
    */
    char *p = (char *)glGetString(GL_EXTENSIONS);
    char *end = p + strlen(p);
    while (p < end) {
	int n = strcspn(p, " ");
	if ((strlen(extName) == n) && (strncmp(extName, p, n) == 0)) {
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
    glutInitWindowSize(400, 400);
    glutCreateWindow("ABGR extension");

    if (!QueryExtension("GL_EXT_abgr")) {
	printf("Couldn't find ABGR extension.\n");
	exit(0);
    }

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutMainLoop();
}

#else

int main(int argc, char **argv)
{

    printf("Couldn't find GL_EXT_abgr extension.\n");
}

#endif
