/*
** cc -o texproxy texproxy.c -lglut -lGLU -lGL -lXext -lX11
**
** Demonstrates use of the proxy texture target to probe texture space
** to determine the largest texture which can be allocated for a given
** internal image format and aspect ratio.
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/* (int)floor(log2(x)) */
static int
intFloorLog2(unsigned int x)
{
    int a = 0;
    while (x >>= 1) ++a;
    return a;
}

/* true if x is a power of two */
static GLboolean
isPow2(unsigned int x)
{
    return ((x > 0) && (x & (x - 1) == 0));
}

/* find the largest texture with the specified internal format and aspect */
static void
findMaxTexture(GLenum internalFormat, int dim,
	       int xAspect, int yAspect, int border, int maxMipmapLevel,
	       int *widthOut, int *heightOut)
{
    int level = 0, levelOffset = 0;
    int width = 1, height = 1;
    int maxLevel = 0, maxWidth = 0, maxHeight = 0;

    if (xAspect > yAspect) {
	width = xAspect / yAspect;
	levelOffset = intFloorLog2(width);
    } else {
	height = yAspect / xAspect;
	levelOffset = intFloorLog2(height);
    }

    while (1) {
	GLint proxyComponents;

	switch (dim) {
	  case 1:
	    glTexImage1D(GL_PROXY_TEXTURE_1D,
			 level, internalFormat,
			 width+2*border, border,
			 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	    
	    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_1D, level,
				     GL_TEXTURE_COMPONENTS, &proxyComponents);
	    break;
	  case 2:
	    glTexImage2D(GL_PROXY_TEXTURE_2D,
			 level, internalFormat,
			 width+2*border, height+2*border, border,
			 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	    
	    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, level,
				     GL_TEXTURE_COMPONENTS, &proxyComponents);
	    break;
	  default:
	    return;
	}
	
	if (proxyComponents != internalFormat) {
	    /* proxy allocation failed -- we're done */
	    break;
	} else {
	    /* proxy allocation succeded -- see how we did */
	    if (level>maxLevel || width>maxWidth || height>maxHeight)
	    {
		maxLevel = level;
		maxWidth = width << level;
		maxHeight = height << level;
	    }
	}

	if (maxMipmapLevel == 0) {
	    /* try the next larger image size at level zero */
	    width <<= 1;
	    height <<= 1;
	} else {
	    /* try same image size at the next higher mipmap level */
	    ++level;
	    if (level+levelOffset > maxMipmapLevel) {
		/* can't query levels which don't exist -- we're done */
		break;
	    }
	}
    }

    if (border>0 && maxWidth>0 && maxHeight>0) {
        maxWidth += 2*border;
        maxHeight += 2*border;
    }
    *widthOut = maxWidth;
    *heightOut = maxHeight;
}

static void
displayTextureSizeInfo(void)
{
    GLint maxTextureSize, maxTextureLevel;
    int maxWidth, maxHeight;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    printf("GL_MAX_TEXTURE_SIZE: %d\n", maxTextureSize);

    maxTextureLevel = intFloorLog2(maxTextureSize);

    findMaxTexture(GL_RGB, 2,
		   1, 1, 0, 0, &maxWidth, &maxHeight);
    printf("RGB 1:1 (%d x %d)\n", maxWidth, maxHeight);

    findMaxTexture(GL_RGBA, 2,
		   2, 1, 0, 0, &maxWidth, &maxHeight);
    printf("RGBA 2:1 (%d x %d)\n", maxWidth, maxHeight);

    findMaxTexture(GL_RGB, 2,
		   1, 1, 0, maxTextureLevel, &maxWidth, &maxHeight);
    printf("RGB 1:1 (%d x %d) mipmap\n", maxWidth, maxHeight);

    findMaxTexture(GL_LUMINANCE_ALPHA, 2,
		   1, 1, 1, 0, &maxWidth, &maxHeight);
    printf("LUMINANCE_ALPHA 1:1 (%d x %d) w/border\n", maxWidth, maxHeight);
}

int
main(int argc, char **argv)
{
    char *name = "Texture Proxy Test";

    glutInitDisplayMode(GLUT_RGB);
    glutCreateWindow(name);

    displayTextureSizeInfo();

    exit(EXIT_SUCCESS);
}
