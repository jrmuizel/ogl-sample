/*
** packedpix.c: Uses packed pixel types for DrawPixels, ReadPixels,
**              TexImage2D, and GetTexImage.
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

/*----------------------------------------------------------------------*/

#define NUM_IMAGES 4
char *rgbFile = NULL;

GLboolean doubleBuffer = GL_FALSE;
GLboolean queryExts = GL_TRUE;

int winWidth = 300;
int winHeight = 300;

RGBImageRec *fileImage;

GLboolean isUseDisplayLists= GL_FALSE;

/*----------------------------------------------------------------------*/

/*
** This code is used to create an image of specified type and format
** for use with DrawPixels and TexImage.  Instead of writing
** code for any possible source image, a fixed type/format image is
** generated and then used to produce the requested type/format images.
** The fixed image is float/rgba.
*/

typedef struct _ConvertImageRec {
    GLfloat *fltImage;		/* original float RGBA image */
    void *newImage;             /* converted image */
    GLint imageWidth;		/* number of active pixels in a row */
    GLint imageHeight;		/* number of pixels in a column */
    GLint imageDataWidth;	/* original number of pixels in a row */
    GLint imageDataHeight;	/* original number of pixels in a column */
    GLenum type;		/* type of converted image */
    GLenum format;		/* format of converted image */
    GLint imageSize;		/* number of bytes in the converted image */
    int components;		/* number of components per pixel */
    int bytesPerComp;		/* number of bytes per component */
} ConvertImageRec;

/*
** Creates the float RGBA image from the unsigned byte RGB one that typically
** comes from a file. 
*/
static void CreateFloatImage(RGBImageRec *imageIn, ConvertImageRec *imageOut)
{
    GLubyte *rawData;
    GLint x, y;
    GLfloat *fltImage, *ptr;
    GLfloat alpha;
    GLint imageWidth, imageHeight;
    int imageSize;

    imageWidth = imageIn->sizeX;
    imageHeight = imageIn->sizeY;
    rawData = imageIn->data;

    imageSize = imageWidth * imageHeight * sizeof(GLfloat) * 4;
    fltImage = (float *)malloc(imageSize);
    if (NULL == fltImage) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    ptr = fltImage;
    for (y = 0; y < imageHeight; y++) {
        for (x = 0; x < imageWidth; x++) {
            *ptr++ = *rawData++ / 255.0;
	    /* make an alpha value by copying the green channel */
            alpha = *ptr++ = *rawData++ / 255.0; 
            *ptr++ = *rawData++ / 255.0;
            *ptr++ = alpha;
        }
    }
    imageOut->fltImage = fltImage;
    imageOut->imageWidth = imageWidth;
    imageOut->imageHeight = imageHeight;
    imageOut->imageDataWidth = imageOut->imageWidth;
    imageOut->imageDataHeight = imageOut->imageHeight;
}

/*
** Converts the float RGBA image pointed to by imageData->fltImage
** into an image of imageData->type and imageData->format pointed
** to by imageData->newImage.
*/
static void ConvertImage(ConvertImageRec *imageData)
{
    GLint imageWidth = imageData->imageDataWidth;
    GLint imageHeight = imageData->imageDataHeight;
    GLenum type = imageData->type;
    GLenum format = imageData->format;
    void *newImage;
    GLfloat *myImage = imageData->fltImage;
    int i, j, k;
    int imageSize;
    int components = imageData->components;

    /* Free the old image if there is one. */
    if (NULL != imageData->newImage) {
	free(imageData->newImage);
    }

    imageSize = imageWidth*imageHeight * components*imageData->bytesPerComp;

    newImage = (void *)malloc(imageSize);
    if (NULL == newImage) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    switch (type) {
	case GL_BITMAP:
	case GL_UNSIGNED_BYTE:
	case GL_BYTE:
	case GL_UNSIGNED_SHORT:
	case GL_SHORT:
	case GL_INT:
	case GL_UNSIGNED_INT:
	    break;

	case GL_FLOAT:
	    {
		GLfloat *data = newImage;

		for (i = 0; i < imageSize/4; i++) {
		    *data++ = *myImage++;
		}
	    }
	    break;

	case GL_UNSIGNED_BYTE_3_3_2_EXT:
	    {
		GLubyte *data = newImage;
		GLfloat *invalue;

		for (i = 0; i < imageHeight; i++) {
		    for (j = 0; j < imageWidth; j++) {

			/* first bitfield */
			invalue = myImage++;
			*data = ((GLubyte)(*invalue * 7) << 5) & 0xE0;

			/* second bitfield */
			invalue = myImage++;
			*data |= ((GLubyte)(*invalue * 7) << 2) & 0x1C;

			/* third bitfield */
			invalue = myImage++;
			*data |= (GLubyte)(*invalue * 3) & 0x03;

			data++;
			myImage += 1; /* skip the final component */
		    }
		}
	    }
	    break;

	case GL_UNSIGNED_SHORT_4_4_4_4_EXT:
	    {
		GLushort *data = newImage;
		GLfloat *invalue;

		for (i = 0; i < imageHeight; i++) {
		    for (j = 0; j < imageWidth; j++) {

			/* first bitfield */
			invalue = myImage++;
			*data = ((GLushort)(*invalue * 15) << 12) & 0xF000;

			/* second bitfield */
			invalue = myImage++;
			*data |= ((GLushort)(*invalue * 15) << 8) & 0x0F00;

			/* third bitfield */
			invalue = myImage++;
			*data |= ((GLushort)(*invalue * 15) << 4) & 0x00F0;

			/* fourth bitfield */
			invalue = myImage++;
			*data |= (GLushort)(*invalue * 15) & 0x000F;

			data++;
		    }
		}
	    }
	    break;

	case GL_UNSIGNED_SHORT_5_5_5_1_EXT:
	    {
		GLushort *data = newImage;
		GLfloat *invalue;

		for (i = 0; i < imageHeight; i++) {
		    for (j = 0; j < imageWidth; j++) {

			/* first bitfield */
			invalue = myImage++;
			*data = ((GLushort)(*invalue * 31) << 11) & 0xF800;

			/* second bitfield */
			invalue = myImage++;
			*data |= ((GLushort)(*invalue * 31) << 6) & 0x07C0;

			/* third bitfield */
			invalue = myImage++;
			*data |= ((GLushort)(*invalue * 31) << 1) & 0x003E;

			/* fourth bitfield */
			invalue = myImage++;
			*data |= (GLushort)(*invalue) & 0x0001;

			data++;
		    }
		}
	    }
	    break;

	case GL_UNSIGNED_INT_8_8_8_8_EXT:
	    {
		GLuint *data = newImage;
		GLfloat *invalue;

		for (i = 0; i < imageHeight; i++) {
		    for (j = 0; j < imageWidth; j++) {

			/* first bitfield */
			invalue = myImage++;
			*data = ((GLuint)(*invalue * 255) << 24) & 0xFF000000;

			/* second bitfield */
			invalue = myImage++;
			*data |= ((GLushort)(*invalue * 255) << 16) &
				 0x00FF0000;

			/* third bitfield */
			invalue = myImage++;
			*data |= ((GLuint)(*invalue * 255) << 8) & 0x0000FF00;

			/* fourth bitfield */
			invalue = myImage++;
			*data |= (GLuint)(*invalue * 255) & 0x000000FF;

			data++;
		    }
		}
	    }
	    break;

	case GL_UNSIGNED_INT_10_10_10_2_EXT:
	    {
		GLuint *data = newImage;
		GLfloat *invalue;

		for (i = 0; i < imageHeight; i++) {
		    for (j = 0; j < imageWidth; j++) {

			/* first bitfield */
			invalue = myImage++;
			*data = ((GLuint)(*invalue * 1023) << 22) & 0xFFC00000;

			/* second bitfield */
			invalue = myImage++;
			*data |= ((GLuint)(*invalue * 1023) << 12) & 0x003FF000;

			/* third bitfield */
			invalue = myImage++;
			*data |= ((GLuint)(*invalue * 1023) << 2) & 0x00000FFC;

			/* fourth bitfield */
			invalue = myImage++;
			*data |= (GLuint)(*invalue * 3) & 0x00000003;

			data++;
		    }
		}
	    }
	    break;
    } 
    imageData->newImage = newImage;
    imageData->imageSize = imageSize;
}
/*----------------------------------------------------------------------*/

ConvertImageRec imageData;

static int numTypes = 6;
static GLint components = 4;

void *tempbuf;

GLuint texSizeX, texSizeY;

/*----------------------------------------------------------------------*/

typedef struct cellStruct {
	int numRows;
	int numCols;
	int cellSizeX;
	int cellSizeY;
	int row;
	int col;
	int x;
	int y;
} cellStruct;

static cellStruct cell = {4,6, 0,0, 0,0, 0,0};

static void updateXY(cellStruct *cell)
{
    cell->x = cell->col * cell->cellSizeX;
    cell->y = cell->row * cell->cellSizeY;
}

static void setCell(cellStruct *cell, int col, int row)
{
    cell->col = col; cell->row = row;
    updateXY(cell);
}

static void nextCell(cellStruct *cell)
{
    cell->col ++;
    if (cell->col == cell->numCols) {
	cell->col = 0;
	cell->row ++;
	if (cell->row == cell->numRows) {
	    cell->row = 0;
	}
    }
    updateXY(cell);
}

/*----------------------------------------------------------------------*/

/*
** Calculates the largest power of two that's less than the value.
*/
static unsigned int maxTexSize(unsigned int value)
{
    int power;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &power);
    if (value < power) {
	while(!(power & value)) power >>= 1;
    }
    return power;
}

/*
** Makes a ramped unsigned byte RGB image.
*/
static RGBImageRec *InitRampedImage(void)
{
    int i,j;
    int sizeX = 128;
    int sizeY = 128;
    int imageSize = sizeX*sizeY*3;
    GLubyte *byteImage, *pImg;
    RGBImageRec *rgbImage;

    rgbImage = (RGBImageRec *)malloc(sizeof(RGBImageRec));
    byteImage = (GLubyte *)malloc(imageSize);
    pImg = byteImage;

    for (i=0; i < sizeX; i++) {
	for (j=0; j < sizeY; j++) {
	    *pImg++ = 255 * (float)i/sizeX;
	    *pImg++ = 255 * (1-(float)j/sizeY);
	    *pImg++ = 255 * (1-(float)i/sizeX);
	}
    }
    rgbImage->data = byteImage;
    rgbImage->sizeX = sizeX;
    rgbImage->sizeY = sizeY;
    return rgbImage;
}

static void Init(void)
{
    int i;
    GLfloat quant4[4];

    /*
    ** Create a float RGBA image from the file data.
    */
    CreateFloatImage(fileImage, &imageData);
    free(fileImage->data);
    free(fileImage);
    fileImage = NULL;

    /*
    ** Now create a working image with the float image.
    */
    imageData.type = GL_FLOAT;
    imageData.format = GL_RGBA;
    imageData.newImage = NULL;
    imageData.components = components = 4;
    imageData.bytesPerComp = 4;
    ConvertImage(&imageData);

    /*
    ** Allocate a buffer to read images into.
    */
    tempbuf = malloc(imageData.imageSize+3);
    if (NULL == tempbuf) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    /*
    ** Find the largest texture size that will work with this image.
    */
    texSizeX = maxTexSize((unsigned int) imageData.imageWidth);
    texSizeY = maxTexSize((unsigned int) imageData.imageHeight);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, imageData.imageDataWidth);
    glPixelStorei(GL_PACK_ROW_LENGTH, imageData.imageDataWidth);

    glScissor(0,0,winWidth,winHeight);
    glEnable(GL_SCISSOR_TEST);
    glColor3f(1.0, 1.0, 1.0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}


/*----------------------------------------------------------------------*/

/*
** Allows the caller to run through a list of types,
** setting the format, components, and bytesPerComp accordingly.
*/
static void nextType(ConvertImageRec *imageData)
{
    /*
    ** Note that packed pixel types are designed so that
    ** the image size can be computed by making the number
    ** of components = 1 and the bytesPerComponent = bytes
    ** per group.  This allows the arithmatic to be performed
    ** uniformly without introducing floating point values.
    ** imageData->components is used to compute the image size.
    ** components is passed to TexImage.
    */
    switch(imageData->type) {
      case GL_FLOAT:
	imageData->type = GL_UNSIGNED_BYTE_3_3_2_EXT;
	imageData->format = GL_RGB;
	imageData->components = 1;
	imageData->bytesPerComp = 1;
	components = 3;
	break;
      case GL_UNSIGNED_BYTE_3_3_2_EXT:
	imageData->type = GL_UNSIGNED_SHORT_4_4_4_4_EXT;
	imageData->format = GL_RGBA;
	imageData->components = 1;
	imageData->bytesPerComp = 2;
	components = 4;
	break;
      case GL_UNSIGNED_SHORT_4_4_4_4_EXT:
	imageData->type = GL_UNSIGNED_SHORT_5_5_5_1_EXT;
	imageData->format = GL_RGBA;
	imageData->components = 1;
	imageData->bytesPerComp = 2;
	components = 4;
	break;
      case GL_UNSIGNED_SHORT_5_5_5_1_EXT:
	imageData->type = GL_UNSIGNED_INT_8_8_8_8_EXT;
	imageData->format = GL_RGBA;
	imageData->components = 1;
	imageData->bytesPerComp = 4;
	components = 4;
	break;
      case GL_UNSIGNED_INT_8_8_8_8_EXT:
	imageData->type = GL_UNSIGNED_INT_10_10_10_2_EXT;
	imageData->format = GL_RGBA;
	imageData->components = 1;
	imageData->bytesPerComp = 4;
	components = 4;
	break;
      case GL_UNSIGNED_INT_10_10_10_2_EXT:
	imageData->type = GL_FLOAT;
	imageData->format = GL_RGBA;
	imageData->components = 4;
	imageData->bytesPerComp = 4;
	components = 4;
	break;
    }
}

/*----------------------------------------------------------------------*/

static void Reshape(int width, int height)
{
    winWidth = width; winHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glScissor(0,0,width,height);

    /* Update the cell sizes to match the new window size. */
    cell.cellSizeX = width/cell.numCols;
    cell.cellSizeY = height/cell.numRows;
    imageData.imageWidth = cell.cellSizeX-4;
    imageData.imageHeight = cell.cellSizeY-4;

    /* Make sure we don't go stomping off the edges of our image data. */
    if (imageData.imageWidth > imageData.imageDataWidth) {
	imageData.imageWidth = imageData.imageDataWidth;
    }
    if (imageData.imageHeight > imageData.imageDataHeight) {
	imageData.imageHeight = imageData.imageDataHeight;
    }
}


/*
** Runs the packed pixel types through DrawPixels/ReadPixels 
** and TexImage/GetTexImage.
*/
static void Draw(void)
{
    int i;

    glClear(GL_COLOR_BUFFER_BIT);

    setCell(&cell, 0,0);

    /* DrawPixels */

    for (i=0; i < numTypes; i++) {
	glRasterPos3f(0,0,-1);
	glBitmap(0,0,0,0, cell.x, cell.y, NULL);
	ConvertImage(&imageData);

	if (isUseDisplayLists) {
	   glNewList(1,GL_COMPILE);
	   assert(glGetError() == GL_NO_ERROR);
	   glDrawPixels(imageData.imageWidth, imageData.imageHeight, 
			imageData.format, imageData.type, imageData.newImage);
	   assert(glGetError() == GL_NO_ERROR);
	   glEndList();
	   assert(glGetError() == GL_NO_ERROR);
	   glCallList(1);
	   assert(glGetError() == GL_NO_ERROR);
	   
	}
	else {	
	   glDrawPixels(imageData.imageWidth, imageData.imageHeight, 
			imageData.format, imageData.type, imageData.newImage);
        }

	nextType(&imageData);
	nextCell(&cell);
    }

    /* ReadPixels */

    for (i=0; i < numTypes; i++) {
	glReadPixels(0,0, imageData.imageWidth, imageData.imageHeight, 
		imageData.format, imageData.type, tempbuf);
	glRasterPos3f(0,0,-1);
	glBitmap(0,0,0,0, cell.x, cell.y, NULL);
	glDrawPixels(imageData.imageWidth, imageData.imageHeight, 
		imageData.format, imageData.type, tempbuf);

	nextType(&imageData);
	nextCell(&cell);
    }

    /* TexImage */

    glEnable(GL_TEXTURE_2D);

    for (i=0; i < numTypes; i++) {
	ConvertImage(&imageData);

	if (isUseDisplayLists) {
	   glNewList(2,GL_COMPILE);
	   assert(glGetError() == GL_NO_ERROR);
	   glTexImage2D(GL_TEXTURE_2D, 0, components, texSizeX, texSizeY,
			0, imageData.format, imageData.type, 
			imageData.newImage);

	   assert(glGetError() == GL_NO_ERROR);	
	   glEndList();
	   assert(glGetError() == GL_NO_ERROR);

	   glCallList(2);
	}
	else {
	   glTexImage2D(GL_TEXTURE_2D, 0, components, texSizeX, texSizeY,
			0, imageData.format, imageData.type, 
			imageData.newImage);
	}

	glBegin(GL_QUADS);
	    glTexCoord2f(0.0, 0.0);
	    glVertex2i(cell.x, cell.y);
	    glTexCoord2f(1.0, 0.0);
	    glVertex2i(cell.x+imageData.imageWidth-1, cell.y);
	    glTexCoord2f(1.0, 1.0);
	    glVertex2i(cell.x+imageData.imageWidth-1, 
			cell.y+imageData.imageHeight-1);
	    glTexCoord2f(0.0, 1.0);
	    glVertex2i(cell.x, cell.y+imageData.imageHeight-1);
	glEnd();

	nextType(&imageData);
	nextCell(&cell);
    }

    /* GetTexImage */

    for (i=0; i < numTypes; i++) {
	glTexImage2D(GL_TEXTURE_2D, 0, 4, texSizeX, texSizeY,
			0, GL_RGBA, GL_FLOAT, imageData.fltImage);
	glGetTexImage(GL_TEXTURE_2D, 0, imageData.format, 
			imageData.type, tempbuf);
	glTexImage2D(GL_TEXTURE_2D, 0, components, texSizeX, texSizeY,
			0, imageData.format, imageData.type, tempbuf);
	glBegin(GL_QUADS);
	    glTexCoord2f(0.0, 0.0);
	    glVertex2i(cell.x, cell.y);
	    glTexCoord2f(1.0, 0.0);
	    glVertex2i(cell.x+imageData.imageWidth-1, cell.y);
	    glTexCoord2f(1.0, 1.0);
	    glVertex2i(cell.x+imageData.imageWidth-1, 
			cell.y+imageData.imageHeight-1);
	    glTexCoord2f(0.0, 1.0);
	    glVertex2i(cell.x, cell.y+imageData.imageHeight-1);
	glEnd();

	nextType(&imageData);
	nextCell(&cell);
    }
    glDisable(GL_TEXTURE_2D);

    if (doubleBuffer) {
	glutSwapBuffers();
    } else {
	glFlush();
    }
}

/*----------------------------------------------------------------------*/

static char *keyList[][2] = {
    {"R", "Reset draw routine"},
    {"d", "toggle use of display lists"},
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
      case 'd':
	if (isUseDisplayLists) {
	   printf("Not using display lists.\n");
	   isUseDisplayLists= GL_FALSE;
	}
	else {
	   printf("Using display lists in bottom row & 2nd row from top.\n");
	   isUseDisplayLists= GL_TRUE;
	}
	glutPostRedisplay();
	break;
      case 'R':
	glutPostRedisplay();
	break;

      case 27:
	exit(0);
    }
}

/*----------------------------------------------------------------------*/

static char *argList[][2] = {
    {"-sb", "single buffered"},
    {"-db", "double buffered"},
    {"-f filename", "filename for texture"},
    {"-nq", "skip querying extensions"},
    {"-h", "print this list"},
    {"Press H in app window to get key interpretations", ""},
    {NULL, NULL}
};

static void Args(int argc, char **argv)
{
    GLint i;

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
		rgbFile = argv[++i];
	    }
	} else if (strcmp(argv[i], "-nq") == 0) {
	    queryExts = GL_FALSE;
	} else if (strcmp(argv[i], "-h") == 0) {
	    for (i = 0; argList[i][0] != NULL; i++) {
		printf("%-30s %s\n", argList[i][0], argList[i][1]);
	    }
	    exit(0);
	}
    }
}

/*----------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------*/

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    /*
    ** The image is loaded before the window is opened to
    ** allow sizing the window based on the image size.
    */
    if (NULL == rgbFile) {
        fileImage = InitRampedImage();
    }
    else {
	fileImage = rgbImageLoad(rgbFile);
	if (NULL == fileImage) {
	    fprintf(stdout, "Warning: file %s not found.\n", rgbFile);
	    fileImage = InitRampedImage();
	}
    }
    cell.cellSizeX = fileImage->sizeX + 4;
    cell.cellSizeY = fileImage->sizeY + 4;
    winWidth = cell.numCols*cell.cellSizeX - 4;
    winHeight = cell.numRows*cell.cellSizeY - 4;

    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
    glutInitDisplayMode(type);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Packed Pixel Test");

    if (queryExts && !QueryExtension("GL_EXT_packed_pixels")) {
	fprintf(stdout, "Warning: packed pixel extension not found.\n");
	exit(0);
    }

    Init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutMainLoop();
}
