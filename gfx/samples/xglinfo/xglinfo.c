/*
 * 
 *           Copyright (c) Digital Equipment Corporation, 1995
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute  this software and its
 * documentation for any  purpose   and without fee  is  hereby  granted,
 * provided that the above copyright notice appear in all copies and that
 * both  that  copyright  notice  and  this  permission notice appear  in
 * supporting documentation, and that the name of Digital  not be used in
 * advertising or publicity  pertaining to distribution  of the  software
 * without specific, written prior permission.
 * 
 * DIGITAL DISCLAIMS   ALL  WARRANTIES WITH   REGARD   TO  THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT   SHALL  DIGITAL  BE   LIABLE  FOR  ANY SPECIAL,   INDIRECT   OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION  OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/*
** $Header: //depot/main/gfx/samples/xglinfo/xglinfo.c#1 $
*/
/*
 * HISTORY
 */
/*****************************************************************************/
/******************************** Documentation ******************************/
/*****************************************************************************/

/*
 * See man page for full documentation.
 *
 * Written by John Dennis, Digital Equipment Corporation.
 * jdennis@mlo.dec.com
 *
 * With thanks and credit to Jim Fulton who wrote the original version of
 * xdpyinfo while at MIT which served as a starting point for this code.
 */

/*****************************************************************************/
/******************************* Include Files *******************************/
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include "global.h"


/*****************************************************************************/
/****************************** Internal Defines *****************************/
/*****************************************************************************/

#define GL_GET_CONFIG(_config, _pValue)                                      \
{                                                                            \
  switch (result = glXGetConfig(dpy, visual, _config, (_pValue))) {          \
  case Success:                                                              \
    break;                                                                   \
  case GLX_NO_EXTENSION:                                                     \
    printf("ERROR glXGetConfig %s return GLX_NO_EXTENSION, visuaID 0x%lx\n", \
          "_config", visual->visualid);                                      \
    break;                                                                   \
  case GLX_BAD_SCREEN:                                                       \
    printf("ERROR glXGetConfig %s return GLX_BAD_SCREEN, visuaID 0x%lx\n",   \
          "_config", visual->visualid);                                      \
    break;                                                                   \
  case GLX_BAD_ATTRIBUTE:                                                    \
    printf("ERROR glXGetConfig %s return GLX_BAD_ATTRIBUTE, visuaID 0x%lx\n",\
          "_config", visual->visualid);                                      \
    break;                                                                   \
  case GLX_BAD_VISUAL:                                                       \
    printf("ERROR glXGetConfig %s return GLX_BAD_VISUAL, visuaID 0x%lx\n",   \
          "_config", visual->visualid);                                      \
    break;                                                                   \
  default:                                                                   \
    printf("ERROR glXGetConfig %s return unknown error %d, visuaID 0x%lx\n", \
          "_config", result, visual->visualid);                              \
    break;                                                                   \
  }                                                                          \
}

/*****************************************************************************/
/************************** Internal Type Definitions ************************/
/*****************************************************************************/

typedef struct GLXVisualInfo {
  int useGL;			       /* support GLX rendering */
  int bufferSize;		       /* depth of the color buffer */
  int level;			       /* level in plane stacking */
  int rgba;			       /* true if RGBA mode */
  int doubleBuffer;		       /* double buffering supported */
  int stereo;			       /* stereo buffering supported */
  int auxBuffers;		       /* number of aux buffers */
  int redSize;			       /* number of red component bits */
  int greenSize;		       /* number of green component bits */
  int blueSize;			       /* number of blue component bits */
  int alphaSize;		       /* number of alpha component bits */
  int depthSize;		       /* number of depth bits */
  int stencilSize;		       /* number of stencil bits */
  int accumRedSize;		       /* number of red accum bits */
  int accumGreenSize;		       /* number of green accum bits */
  int accumBlueSize;		       /* number of blue accum bits */
  int accumAlphaSize;		       /* number of alpha accum bits */
  int xVisualType;
  int transparentPixel;		       /* None or Pixel */
  int transparentIndex;		       /* transparent index value */
  int transparentRedValue;	       /* transparent color value */
  int transparentGreenValue;
  int transparentBlueValue;
  int transparentAlphaValue;
  int visualCaveats;		       /* None or Slow */
} GLXVisualInfo;

typedef enum 
{
  VID_NONE = 0,
  VID_RGB  = 1,
  VID_RGBA = 2,
  VID_CI   = 4,
  VID_ALL  = 7
} VidTypes;

/*****************************************************************************/
/**********************  External Function Declarations  *********************/
/*****************************************************************************/

/*****************************************************************************/
/**********************  Internal Function Declarations  *********************/
/*****************************************************************************/

static void
Usage(
  void
);

static void
ProcessArgs(
	     int argc   ,
	     char       *argv[]
);

void
PrintEventMask (
    int indent,				/* amount by which to indent */
    unsigned long mask				/* event mask */
);

void
PrintExtensionInfo(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
);

void
PrintPixmapFormats(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
);

void
PrintWindowFocus(
  Display    *dpy,
  int         fieldNameLen
);

void
PrintDisplayInfo(
  Display    *dpy,
  int         fieldNameLen
);

int
GetGLVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    GLXVisualInfo *glVisual
);

void
PrintCookedVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
);

void
PrintRawVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
);

int
PrintVisualIDs (
    Display    *dpy,
    XVisualInfo *visual
);

void
PrintScreenInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
);

void
PrintScreenVisualInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
);

static void
PrintIndentedStringWithBlanks(
  const char	* head,
  const char	* string,
  int	        fieldNameLen,
  int		indent
);

static int
ProcessScreen(
  Display *dpy,
  int screenNum,
  int fieldNameLen,
  int indent
);

/*****************************************************************************/
/*************************  External Global Variables  ***********************/
/*****************************************************************************/

char       *ProgramName;

/*****************************************************************************/
/*************************  Internal Global Variables  ***********************/
/*****************************************************************************/

static char       *displayname = NULL;	/* server to contact */
static int         printDisplayInfo = GL_TRUE;
static int         printScreenInfo = GL_TRUE;
static int         printVisualInfo = GL_TRUE;
static int         printCookedVisualInfo = GL_TRUE;
static int         printRawVisualInfo = GL_FALSE;
static int         printSpecficScreen = GL_FALSE;
static int         specficScreenNum = 0;
static VidTypes    printVids = VID_NONE;

static int         pageWidth = 80;
static int         glErrorBase, glEventBase;
static int         glXMajorVersion, glXMinorVersion;
static const char  *glXExtensionString = "";
static int         glXVisRating = GL_FALSE;
static int         glXVisInfo = GL_FALSE;

static Bool        dpyHasGL = GL_FALSE;

/*****************************************************************************/
/***************************  Internal Functions  ****************************/
/*****************************************************************************/

static void
Usage(
  void
)
{
  fprintf(stderr, "usage:  %s [-help] [-display displayname] [-vid rgb | rgba | ci | all ] [-screen n]\n\n",
	  ProgramName);
  fprintf(stderr, "  the following options take a '+' or '-' switch\n");
  fprintf(stderr, "  a '+' turns the option on, a '-' turns the option off\n\n");
  fprintf(stderr, "  [+-printDisplayInfo] [+-printScreenInfo] [+-printVisualInfo]\n");
  fprintf(stderr, "  [+-printCookedVisualInfo]");
  fprintf(stderr, "\n");
  exit(1);
}

static void
ProcessArgs(
	     int argc   ,
	     char       *argv[]
)
{
  int         i;
    char       *arg;
  char        switchChar;

  ProgramName = argv[0];

  for (i = 1; i < argc; i++) {
    arg = argv[i];
    switchChar = arg[0];

    if (switchChar == '-' || switchChar == '+') {
      arg++;
      if (strcmp(arg, "display") == 0) {
	if (switchChar == '-') {
	  if (++i >= argc) goto missing;
	  displayname = argv[i];
	}
	else goto unknown;
      }
      else if (strcmp(arg, "screen") == 0) {
	if (switchChar == '-') {
	  if (++i >= argc) goto missing;
	  if (sscanf(argv[i], "%d", &specficScreenNum) != 1) goto unknown;
          printSpecficScreen = GL_TRUE;
	}
	else goto unknown;
      }
      else if (strcmp(arg, "printDisplayInfo") == 0
	    || strcmp(arg, "printdisplayinfo") == 0) {
	if (switchChar == '+')
	  printDisplayInfo = GL_TRUE;
	else
	  printDisplayInfo = GL_FALSE;
      }
      else if (strcmp(arg, "printScreenInfo") == 0
	    || strcmp(arg, "printscreeninfo") == 0) {
	if (switchChar == '+')
	  printScreenInfo = GL_TRUE;
	else
	  printScreenInfo = GL_FALSE;
      }
      else if (strcmp(arg, "printVisualInfo") == 0
	    || strcmp(arg, "printvisualinfo") == 0) {
	if (switchChar == '+')
	  printVisualInfo = GL_TRUE;
	else
	  printVisualInfo = GL_FALSE;
      }
      else if (strcmp(arg, "printCookedVisualInfo") == 0
	    || strcmp(arg, "printcookedvisualinfo") == 0) {
	if (switchChar == '+')
	  printCookedVisualInfo = GL_TRUE;
	else
	  printCookedVisualInfo = GL_FALSE;
      }
      else if (strcmp(arg, "vid") == 0) {
	if (switchChar == '-') {
	  if (++i >= argc) goto missing;
          if (strcmp(argv[i], "rgb") == 0 || strcmp(argv[i], "RGB") == 0) {
            printVids = VID_RGB;
          }
          else if (strcmp(argv[i], "rgba") == 0 || strcmp(argv[i], "RGBA") == 0) {
            printVids = VID_RGBA;
          }
          else if (strcmp(argv[i], "ci") == 0 || strcmp(argv[i], "CI") == 0) {
            printVids = VID_CI;
          }
          else if (strcmp(argv[i], "all") == 0 || strcmp(argv[i], "ALL") == 0) {
            printVids = VID_ALL;
          }
          else {
            goto unknown;
          }
          printDisplayInfo = GL_FALSE;
          printScreenInfo = GL_FALSE;
          printVisualInfo = GL_TRUE;
          printCookedVisualInfo = GL_FALSE;
        }
	else
          goto unknown;
      }
      else if (strcmp(arg, "help") == 0
        || strcmp(arg, "?") == 0) {
        Usage();
      }
      else goto unknown;
    }
    else {
      goto unknown;
    }
  }
  return;

  unknown:
      fprintf(stderr, "unknown arg: %s\n", argv[i]);
      Usage();
    
  missing:
      fprintf(stderr, "missing arg for: %s\n", argv[i]);
      Usage();
}

void
PrintEventMask (
    int indent,				/* amount by which to indent */
    unsigned long mask				/* event mask */
)
{
  char buf[1024];
    int i, pad;
    int nameWidth = 21;
    unsigned long bit;
    int len = indent;
    int nameLen;
    int bitsfound = 0;
    char *name;

    for (i = 0; i < indent; i++) buf[i] = ' ';

    for (bit = 1; mask; bit <<= 1) {
      if (bit & mask) {
	mask ^= bit;		       /* clear bit from mask */
	name = EventMaskName((long)bit);
	nameLen = strlen(name);
	if (nameLen >= nameWidth) nameLen = nameWidth - 1; /* -1 for pad */
	pad = nameWidth - nameLen;
	if (len + nameLen > pageWidth) {
	  puts (buf);
	  len = indent;
	}
	strncpy(buf+len, name, nameLen);
	len += nameLen;
	if (len + pad > pageWidth) {
	  buf[len] = 0;
	  puts (buf);
	  len = indent;
	}
	else {
	  for (i = 0; i < pad; i++) buf[len++] = ' ';
	}
	buf[len] = 0;
	bitsfound++;
      }
    }

    if (bitsfound) puts (buf);
}

void
PrintExtensionInfo(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
)
{
  int         n = 0;
  int         len, lineLen;
  char      **extlist = XListExtensions(dpy, &n);


  printf("%*s%d total\n",
	 fieldNameLen, "Server Extensions:",
	 n);
  if (extlist) {
    register int i;

    printf("%*s", indent, "");
    lineLen = indent;
    for (i = 0; i < n; i++) {
      len = strlen(extlist[i]);
      len += 2;			       /* for ", " */
      if ((lineLen + len) >= pageWidth) {
	printf("\n%*s", indent, "");
	lineLen = indent;
      }
      printf("%s, ", extlist[i]);
      lineLen += len;
    }
    if (lineLen > indent) printf("\n");
    XFreeExtensionList(extlist);
  }
}

void
PrintPixmapFormats(
  Display    *dpy,
  int         fieldNameLen,
  int	      indent
)
{
  int i, n;
  XPixmapFormatValues *pmf;

  pmf = XListPixmapFormats(dpy, &n);
  printf("%*s%d total\n",
	 fieldNameLen, "pixmap formats:",
	 n);
  if (pmf) {
    for (i = 0; i < n; i++) {
      printf("%*sdepth %2d, bits_per_pixel %2d, scanline_pad %2d\n",
	     indent, "",
	     pmf[i].depth, pmf[i].bits_per_pixel, pmf[i].scanline_pad);
    }
    XFree((char *) pmf);
  }
}

void
PrintWindowFocus(
  Display    *dpy,
  int         fieldNameLen
)
{
  Window      focusWin;
  int         focusRevert;

  XGetInputFocus(dpy, &focusWin, &focusRevert);
  switch (focusWin) {
  case PointerRoot:
  case None:
    printf("%*s%s\n",
	   fieldNameLen, "focus:",
	   WindowFocusName(focusWin));
    break;
  default:
    printf("%*s0x%lx, revert to %s\n",
	   fieldNameLen, "focus window:",
	   focusWin, WindowFocusRevertName(focusRevert));
    break;
  }
}

void
PrintDisplayInfo(
  Display    *dpy,
  int         fieldNameLen
)
{
  char       *cp;
  const char *ccp;
  int         minkeycode, maxkeycode;
  char        title[256];
  int         i;
  int         titleLen, fillLen;
  int	      indent;

  if (fieldNameLen < 0) indent = -fieldNameLen;
  else indent = fieldNameLen;
  
  sprintf(title, " Display %s ", DisplayString(dpy));
  titleLen = strlen(title);
  fillLen = (pageWidth - titleLen) / 2;
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("%s", title);
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("\n");  

  printf("%*s%s\n",
	 fieldNameLen, "name of display:",
	 DisplayString(dpy));
  printf("%*s%d.%d\n",
	 fieldNameLen, "version number:",
	 ProtocolVersion(dpy), ProtocolRevision(dpy));
  printf("%*s%s\n",
	 fieldNameLen, "vendor string:",
	 ServerVendor(dpy));
  printf("%*s%d\n",
	 fieldNameLen, "vendor release:",
	 VendorRelease(dpy));
  printf("%*s%ld bytes\n",
	 fieldNameLen, "max request size:",
	 XMaxRequestSize(dpy) * 4);
  printf("%*s%d\n",
	 fieldNameLen, "motion buffer size:",
	 XDisplayMotionBufferSize(dpy));

  cp = ByteOrderName(BitmapBitOrder(dpy));
  printf("%*sunit = %d, bit order = %s, padding = %d\n",
	 fieldNameLen, "bitmap:",
	 BitmapUnit(dpy), cp, BitmapPad(dpy));

  cp = ByteOrderName(ImageByteOrder(dpy));
  printf("%*s%s\n",
	 fieldNameLen, "image byte order:",
	 cp);

  XDisplayKeycodes(dpy, &minkeycode, &maxkeycode);
  printf("%*sminimum %d, maximum %d\n",
	 fieldNameLen, "keycode range:",
	 minkeycode, maxkeycode);

  PrintWindowFocus(dpy, fieldNameLen);

  printf("%*s%d\n",
	 fieldNameLen, "default screen num:",
	 DefaultScreen(dpy));
  printf("%*s%d\n",
	 fieldNameLen, "number of screens:",
	 ScreenCount(dpy));


  PrintPixmapFormats(dpy, fieldNameLen, indent);
  PrintExtensionInfo(dpy, fieldNameLen, indent);

  printf("%*s",
	 fieldNameLen, "GLX Extension:");
  if (dpyHasGL) {
    printf("error base = %d, event base = %d, ",
	   glErrorBase, glEventBase);
    if (glXQueryVersion(dpy, &glXMajorVersion, &glXMinorVersion)) {
      printf("Version %d.%d\n",
	     glXMajorVersion, glXMinorVersion);
    }
    else {
      printf("glXQueryVersion returned error\n");
      exit(1);
    }
  }
  else {
    printf("NONE\n");
  }
#if defined(GLX_VERSION_1_1)
  ccp = glXGetClientString (dpy, GLX_VENDOR);
  if (ccp) {
    printf ("%*sVendor = %s\n", fieldNameLen, "GLX Library:", ccp);
  }
  else {
    printf ("%*sNo Vendor string\n", fieldNameLen, "GLX Library:");
  }
  ccp = glXGetClientString (dpy, GLX_VERSION);
  if (ccp) {
    printf ("%*sVersion = %s\n", indent, "", ccp);
  }
  else {
    printf ("%*sNo Version string\n", indent, "");
  }
  ccp = glXGetClientString (dpy, GLX_EXTENSIONS);
  PrintIndentedStringWithBlanks ("Extensions = ", ccp,
	fieldNameLen, 2);
#endif
#if defined (GLU_VERSION_1_1)
  ccp = (const char *) gluGetString (GLU_VERSION);
  if (ccp) {
    printf ("%*sVersion = %s\n", fieldNameLen, "GLU Library:", ccp);
  }
  else {
    printf ("%*sNo Version string\n", fieldNameLen, "GLU Library:");
  }
  ccp = (const char *) gluGetString (GLU_EXTENSIONS);
  PrintIndentedStringWithBlanks ("Extensions = ", ccp,
	fieldNameLen, 2);
#endif
}

static int 
QueryExtension(
    char *extName
)
{
    /*
    ** Search for extName in the extensions string.  Use of strstr()
    ** is not sufficient because extension names can be prefixes of
    ** other extension names.  Could use strtok() but the constant
    ** string returned by glXGetString can be in read-only memory.
    */
    char *p = (char *)glXExtensionString;
    char *end;
    int extNameLen;

    extNameLen = strlen(extName);

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

int
GetGLVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    GLXVisualInfo *glVisual
)
{
  int result;

  if (!dpyHasGL) {
    glVisual->useGL = GL_FALSE;
    return(0);
  }

  GL_GET_CONFIG(GLX_USE_GL, &glVisual->useGL);

  if (!glVisual->useGL) return(0);

  GL_GET_CONFIG(GLX_BUFFER_SIZE, &glVisual->bufferSize);
  GL_GET_CONFIG(GLX_LEVEL, &glVisual->level);
  GL_GET_CONFIG(GLX_RGBA, &glVisual->rgba);
  GL_GET_CONFIG(GLX_DOUBLEBUFFER, &glVisual->doubleBuffer);
  GL_GET_CONFIG(GLX_STEREO, &glVisual->stereo);
  GL_GET_CONFIG(GLX_AUX_BUFFERS, &glVisual->auxBuffers);
  GL_GET_CONFIG(GLX_RED_SIZE, &glVisual->redSize);
  GL_GET_CONFIG(GLX_GREEN_SIZE, &glVisual->greenSize);
  GL_GET_CONFIG(GLX_BLUE_SIZE, &glVisual->blueSize);
  GL_GET_CONFIG(GLX_ALPHA_SIZE, &glVisual->alphaSize);
  GL_GET_CONFIG(GLX_DEPTH_SIZE, &glVisual->depthSize);
  GL_GET_CONFIG(GLX_STENCIL_SIZE, &glVisual->stencilSize);
  GL_GET_CONFIG(GLX_ACCUM_RED_SIZE, &glVisual->accumRedSize);
  GL_GET_CONFIG(GLX_ACCUM_GREEN_SIZE, &glVisual->accumGreenSize);
  GL_GET_CONFIG(GLX_ACCUM_BLUE_SIZE, &glVisual->accumBlueSize);
  GL_GET_CONFIG(GLX_ACCUM_ALPHA_SIZE, &glVisual->accumAlphaSize);

#ifdef GLX_VERSION_1_1
    if (glXMinorVersion > 0 || glXMajorVersion > 1)
      glXExtensionString = glXQueryExtensionsString(dpy, visual->screen);

#ifdef GLX_EXT_visual_rating
  if ( QueryExtension("GLX_EXT_visual_rating") ) {
      GL_GET_CONFIG(GLX_VISUAL_CAVEAT_EXT, &glVisual->visualCaveats);
      glXVisRating = GL_TRUE;
  }
#endif

#ifdef GLX_EXT_visual_info
  if ( QueryExtension("GLX_EXT_visual_info") ) {
      GL_GET_CONFIG(GLX_X_VISUAL_TYPE_EXT, &glVisual->xVisualType);
      GL_GET_CONFIG(GLX_TRANSPARENT_TYPE_EXT, &glVisual->transparentPixel);
      GL_GET_CONFIG(GLX_TRANSPARENT_INDEX_VALUE_EXT, &glVisual->transparentIndex);
      GL_GET_CONFIG(GLX_TRANSPARENT_RED_VALUE_EXT, &glVisual->transparentRedValue);
      GL_GET_CONFIG(GLX_TRANSPARENT_GREEN_VALUE_EXT, &glVisual->transparentGreenValue);
      GL_GET_CONFIG(GLX_TRANSPARENT_BLUE_VALUE_EXT, &glVisual->transparentBlueValue);
      GL_GET_CONFIG(GLX_TRANSPARENT_ALPHA_VALUE_EXT, &glVisual->transparentAlphaValue);
      glXVisInfo = GL_TRUE;
  }
#endif
#endif

  return(1);
}


void
PrintCookedVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
)
{
  int result;
  int hasRGB, hasAlpha, hasDepth, hasStencil, hasAccum, hasAuxBuffers;
  GLXVisualInfo glVisualInfo;
  GLXVisualInfo *gl = &glVisualInfo;
  int listEmpty;

  printf ("%s visual: ID = 0x%lx (hex) %d (decimal) screen = %d\n",
	  VisualClassName(visual->class) , visual->visualid, visual->visualid,
	  visual->screen);

  if (result = GetGLVisualInfo(dpy, visual, gl)) {

    hasRGB = ((gl->redSize != 0)   ||
	      (gl->greenSize != 0) ||
	      (gl->blueSize != 0));
    hasAlpha = (gl->alphaSize != 0);
    hasDepth = (gl->depthSize != 0);
    hasStencil = (gl->stencilSize != 0);
    hasAccum = ((gl->accumRedSize != 0)   ||
		(gl->accumGreenSize != 0) ||
		(gl->accumBlueSize != 0)  ||
		(gl->accumAlphaSize != 0));
    hasAuxBuffers = (gl->auxBuffers != 0);


    printf("%*s", indent, "");
    if (gl->level < 0) {
      printf("UNDERLAY(%d) ", -gl->level);
    }
    if (gl->level > 0) {
      printf("OVERLAY(%d) ", gl->level);
    }
    if (gl->doubleBuffer)
      printf("DOUBLE buffered ");
    else
      printf("SINGLE buffered ");

    if (gl->stereo)
      printf("STEREO ");
    else
      printf("MONO ");

    /*
     * RGBA visual
     */
    if (gl->rgba) {		       /* rgba visual */
      printf("RGB visual ");

      listEmpty = GL_TRUE;
      if (hasAlpha || hasDepth || hasStencil || hasAccum || hasAuxBuffers) {
	printf("with (");
	if (hasAlpha) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Alpha");
	}
	if (hasDepth) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Z");
	}
	if (hasStencil) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Stencil");
	}
	if (hasAccum) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Accum");
	}
	if (hasAuxBuffers) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("%d Aux Buffers", gl->auxBuffers);
	}
	printf(")\n");
      }

      printf("%*s", indent, "");
      printf("GL Sizes: RGBA=(%d,%d,%d,%d), ",
	     gl->redSize, gl->greenSize, gl->blueSize, gl->alphaSize);
      if (hasDepth)
	printf("Z=%d, ", gl->depthSize);
      if (hasStencil)
	printf("Stencil=%d, ", gl->stencilSize);
      if (hasAccum)
	printf("Accum=(%d,%d,%d,%d)",
	       gl->accumRedSize, gl->accumGreenSize, gl->accumBlueSize,
	       gl->accumAlphaSize);
      printf("\n");

      /*
       * Do some simple sanity checks on rgba visual.
       */
      if (!hasRGB)
	printf("WARNING: RGBA visual, but RGB have zero size\n");
      if (hasAlpha && hasAccum && gl->accumAlphaSize == 0)
	printf("WARNING: RGBA visual with Alpha and Accum, but Accum Alpha size is zero\n");
      if (!hasAlpha && hasAccum && gl->accumAlphaSize != 0)
	printf("WARNING: RGBA visual without Alpha and with Accum, but Accum Alpha size is not zero\n");
    }				       /* end rgba visual */
    /*
     * COLOR INDEX visual
     */
    else {			       /* color index visual */
      printf("COLOR INDEX visual ");

      listEmpty = GL_TRUE;
      if (hasDepth || hasStencil || hasAuxBuffers) {
	printf("with (");
	if (hasDepth) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Z");
	}
	if (hasStencil) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("Stencil");
	}
	if (hasAuxBuffers) {
	  if (listEmpty) listEmpty = GL_FALSE; else printf(" ");
	  printf("%d Aux Buffers");
	}
	printf(")\n");
      }

      printf("%*s", indent, "");
      printf("GL Sizes: ColorIndex=%d, ", gl->bufferSize);
      if (hasDepth)
	printf("Z=%d, ", gl->depthSize);
      if (hasStencil)
	printf("Stencil=%d", gl->stencilSize);
      printf("\n");

      /*
       * Do some simple sanity checks on color index visual.
       */
      if (hasRGB)
	printf("WARNING: CI visual, but RGB has non-zero sizes (%d,%d,%d)\n",
	       gl->redSize, gl->greenSize, gl->blueSize);
      if (hasAlpha)
	printf("WARNING: CI visual, but Alpha has non-zero size = %d\n",
	       gl->alphaSize);
      if (hasAccum)
	printf("WARNING: CI visual, but Accum has non-zero sizes (%d,%d,%d,%d)\n",
	       gl->accumRedSize, gl->accumGreenSize,
	       gl->accumBlueSize, gl->accumAlphaSize);
    }				       /* end color index visual */

    if (glXVisRating || glXVisInfo) {
      printf("%*s", indent, "");
      printf("Extensions: ");
    }
#ifdef GLX_EXT_visual_rating
    if (glXVisRating) {
      if (gl->visualCaveats == GLX_NONE_EXT)
        printf("Caveats=None");
      else if (gl->visualCaveats == GLX_SLOW_VISUAL_EXT)
        printf("Caveats=Slow");
      else
        printf("\nERROR: Unrecognized token for GLX_VISUAL_CAVEATS attribute\n");
    }
#endif

#ifdef GLX_EXT_visual_info
    if (glXVisInfo) {
      if (glXVisRating) printf(", ");
      if (gl->transparentPixel == GLX_NONE_EXT) {
        printf("OPAQUE ");
      }
      else if (gl->transparentPixel == GLX_TRANSPARENT_RGB_EXT) {
        printf("TRANSPARENT PIXEL, ");
        printf("color value=(%d,%d,%d,%d)", gl->transparentRedValue,
	       gl->transparentGreenValue, gl->transparentBlueValue, 
	       gl->transparentAlphaValue);
      }
      else if (gl->transparentPixel == GLX_TRANSPARENT_INDEX_EXT) {
        printf("TRANSPARENT PIXEL, ");
        printf("index value=%d", gl->transparentIndex);
      }
      else
        printf("\nERROR: Unrecognized token for GLX_TRANSPARENT_PIXEL attribute\n");
    }
#endif
    if (glXVisRating || glXVisInfo)
      printf("\n");
    }
  else {
    printf("%*s", indent, "");
    printf("GL NOT SUPPORTED\n");
  }

  printf("%*sCore X: ", indent, "");
  printf ("depth=%d, ", visual->depth);
  printf ("colormapSize=%d ", visual->colormap_size);

  if ((visual->class == TrueColor) || (visual->class == DirectColor)) {
    printf ("RGB: masks=(0x%lx,0x%lx,0x%lx) ",
	    visual->red_mask, visual->green_mask, visual->blue_mask);
    printf ("bits=%d",
	      visual->bits_per_rgb);
  }
  printf("\n");
}

void
PrintRawVisualInfo (
    Display    *dpy,
    XVisualInfo *visual,
    int indent
)
{
  int result;
  GLXVisualInfo glVisualInfo;
  GLXVisualInfo *gl = &glVisualInfo;

  printf ("%s visual: ID = 0x%lx (hex) %d (decimal) screen = %d\n",
	  VisualClassName(visual->class) , visual->visualid, visual->visualid,
	  visual->screen);
  
  if (result = GetGLVisualInfo(dpy, visual, gl)) {

    printf("%*s", indent, "");
    printf("useGL=%d, level=%d, doubleBuffer=%d, rgba=%d, stereo=%d\n",
	   gl->useGL, gl->level, gl->doubleBuffer, gl->rgba, gl->stereo);

    printf("%*s", indent, "");
    printf("bufferSize=%d, number aux buffers=%d\n",
	   gl->bufferSize, gl->auxBuffers);

    printf("%*s", indent, "");
    printf("sizes: RGBA=(%d,%d,%d,%d), Z=%d, STENCIL=%d, ACCUM=(%d,%d,%d,%d)\n",
	   gl->redSize, gl->greenSize, gl->blueSize, gl->alphaSize,
	   gl->depthSize, gl->stencilSize, 
	   gl->accumRedSize, gl->accumGreenSize,
	   gl->accumBlueSize, gl->accumAlphaSize);


    printf("%*s", indent, "");
    printf("Extensions: ");
#ifdef GLX_EXT_visual_rating
    if (glXVisRating) {
      if (gl->visualCaveats == GLX_NONE_EXT)
        printf("visualCaveats=NONE, ");
      else if (gl->visualCaveats == GLX_SLOW_VISUAL_EXT)
        printf("visualCaveats=SLOW_VISUAL, ");
      else
        printf("\nERROR: Unrecognized token for GLX_VISUAL_CAVEATS attribute\n");
    } 
    else
        printf("EXT_visual_rating not supported, ");
#else
    printf("EXT_visual_rating not supported, ");
#endif

#ifdef GLX_EXT_visual_info
    if (glXVisInfo) {
      if (glXVisRating) printf(", ");
      if (gl->transparentPixel == GLX_NONE_EXT) {
        printf("transparentPixel=NONE");
      }
      else if (gl->transparentPixel == GLX_TRANSPARENT_RGB_EXT) {
        printf("transparentPixel=PIXEL, ");
        printf("color value=(%d,%d,%d,%d)", gl->transparentRedValue,
               gl->transparentGreenValue, gl->transparentBlueValue, 
               gl->transparentAlphaValue);
      }
      else if (gl->transparentPixel == GLX_TRANSPARENT_INDEX_EXT) {
        printf("transparentPixel=PIXEL, ");
        printf("index value=%d", gl->transparentIndex);
      }
      else
        printf("\nERROR: Unrecognized token for GLX_TRANSPARENT_PIXEL attribute\n");
    }
    else
        printf("EXT_visual_info not supported ");
#else
    printf("EXT_visual_info not supported ");
#endif
    printf("\n");

  }
  else {
    printf("%*s", indent, "");
    printf("GL NOT SUPPORTED\n");
  }

  printf("%*sCore X: ", indent, "");
  printf ("depth=%d, ", visual->depth);
  printf ("colormapSize=%d ", visual->colormap_size);
  printf ("RGB: masks=(0x%lx,0x%lx,0x%lx) ",
	  visual->red_mask, visual->green_mask, visual->blue_mask);
  printf ("bits=%d",
	  visual->bits_per_rgb);
  printf("\n");
}

int
PrintVisualIDs (
    Display    *dpy,
    XVisualInfo *visual
)
{
  GLXVisualInfo glVisualInfo;
  GLXVisualInfo *gl = &glVisualInfo;

  if (!GetGLVisualInfo(dpy, visual, gl)) return(0);
  switch(printVids) {
  case VID_NONE:
    return(0);
  case VID_RGB:
    if (!gl->rgba || gl->alphaSize != 0) return(0);
    break;
  case VID_RGBA:
    if (!gl->rgba || gl->alphaSize == 0) return(0);
    break;
  case VID_CI:
    if (gl->rgba) return(0);
    break;
  case VID_ALL:
    break;
  default:
    return(0);
  }
  
  printf ("%d\n", visual->visualid);
  return(0);
}

static void PrintIndentedStringWithBlanks(
    const char	* head,
    const char	* string,
    int	        fieldNameLen,
    int		indent
)
{
    int			indentPos;
    const char		*cp;
    const char		*ncp;
    int			leftOnLine;
    
    if (fieldNameLen < 0) indentPos = -fieldNameLen;
    else indentPos = fieldNameLen;
    leftOnLine = pageWidth - indentPos - strlen (head);

    printf ("%*s%s", fieldNameLen, "", head);	/* first line */
    
    indentPos += indent;

    cp = string;
    
    if (cp == NULL)
    {
	printf ("(no string)\n");
	return;
    }
    if (*cp == 0)
    {
	printf ("(none)\n");
	return;
    }

    while (1)
    {
	if (strlen(cp) <= leftOnLine)
	{
	    printf ("%s\n", cp);
	    return;	    /* all done */
	}

	/* we must find a space at which to break the line */
	ncp = cp + leftOnLine;    /* last possible char */
	while (ncp > cp && *ncp != ' ') ncp--;
	if (ncp == cp)
	{
	    /* very long extension name; print it on one line */
	    ncp = cp + leftOnLine;
	    while (*ncp != 0 && *ncp != ' ') ncp++;
	}

	/* print from cp to ncp */
	printf ("%.*s\n", ncp - cp, cp);
	cp = ncp;

	/* skip to next non-blank */
	while (*cp == ' ') cp++;

	if (*cp == 0) return; /* all done */

	printf ("%*s", indentPos, "");
	leftOnLine = pageWidth - indentPos;
    }

}
void
PrintScreenInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
)
{
  Screen     *s = ScreenOfDisplay(dpy, scr);	/* opaque structure */
  int         i;		/* temp variable: iterator */
  static char *yes = "YES", *no = "NO", *when = "WHEN MAPPED";
  double      xres, yres;
  int         ndepths = 0, *depths = NULL;
  char        title[256];
  int         titleLen, fillLen;
  XVisualInfo	* vi;
  GLXContext	ctx;
  int		attrib_list[] = {0};
  Window	w;
  XSetWindowAttributes	wattribs;
  char	      *cp;
  const char	      *ccp;

  sprintf(title, " Screen %d ", scr);
  titleLen = strlen(title);
  fillLen = (pageWidth - titleLen) / 2;
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("%s", title);
  for (i = 0; i < fillLen; i++) printf("%c", '=');
  printf("\n");  

  /*
   * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
   * 
   * dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch)) = N pixels /
   * (M inch / 25.4) = N * 25.4 pixels / M inch
   */

  xres = ((((double) DisplayWidth(dpy, scr)) * 25.4) /
	  ((double) DisplayWidthMM(dpy, scr)));
  yres = ((((double) DisplayHeight(dpy, scr)) * 25.4) /
	  ((double) DisplayHeightMM(dpy, scr)));

  printf("%*s%d\n",
	 fieldNameLen, "screen:",
	 scr);
  printf("%*s%dx%d pixels (%dx%d millimeters)\n",
         fieldNameLen, "dimensions:",
	 DisplayWidth(dpy, scr), DisplayHeight(dpy, scr),
	 DisplayWidthMM(dpy, scr), DisplayHeightMM(dpy, scr));

  printf("%*s%dx%d dots per inch\n",
	 fieldNameLen, "resolution:",
	 (int) (xres + 0.5), (int) (yres + 0.5));
  depths = XListDepths(dpy, scr, &ndepths);
  if (!depths)
    ndepths = 0;
  printf("%*s(%d total):       ",
	 fieldNameLen, "depths:", ndepths);
  for (i = 0; i < ndepths; i++) {
    printf("%d", depths[i]);
    if (i < ndepths - 1) {
      putchar(',');
      putchar(' ');
    }
  }
  putchar('\n');
  if (depths)
    XFree((char *) depths);
  printf("%*s0x%lx\n",
	 fieldNameLen, "root window id:", RootWindow(dpy, scr));
  printf("%*s%d plane%s\n",
	 fieldNameLen, "depth of root window:",
	 DisplayPlanes(dpy, scr),
	 DisplayPlanes(dpy, scr) == 1 ? "" : "s");
  printf("%*s0x%lx\n",
	 fieldNameLen, "default visual id:",
	 XVisualIDFromVisual (DefaultVisual (dpy, scr)));
  printf("%*sminimum %d, maximum %d\n",
	 fieldNameLen, "number colormaps:",
	 MinCmapsOfScreen(s), MaxCmapsOfScreen(s));
  printf("%*s0x%lx, number colormap cells %d\n",
	 fieldNameLen, "default colormap:",
	 DefaultColormap(dpy, scr), DisplayCells(dpy, scr));
  printf("%*sblack 0x%lx, white 0x%lx\n",
	 fieldNameLen, "preallocated pixels:",  
	 BlackPixel(dpy, scr), WhitePixel(dpy, scr));
  printf("%*sbacking-store %s, save-unders %s\n",
	 fieldNameLen, "options:",
	 (DoesBackingStore(s) == NotUseful) ? no :
	 ((DoesBackingStore(s) == Always) ? yes : when),
	 DoesSaveUnders(s) ? yes : no);
  printf("%*s0x%lx\n",
	 fieldNameLen, "input event mask:", EventMaskOfScreen(s));
  PrintEventMask(fieldNameLen > 0 ? fieldNameLen : -fieldNameLen,
		 EventMaskOfScreen(s));

    if (dpyHasGL)
    {
	vi = glXChooseVisual (dpy, scr, attrib_list);
	if (vi == NULL)
	{
	    int attrib_list_again[] = {GLX_DOUBLEBUFFER, None};
	    vi = glXChooseVisual (dpy, scr, attrib_list_again);
	    if (vi == NULL)
	    {
		printf ("Error: could not find visual for OpenGL\n");
		return;	    
	    }
	}
	ctx = glXCreateContext (dpy, vi, NULL, False);
	wattribs.colormap = XCreateColormap (dpy, 
		RootWindow(dpy, scr), vi->visual, AllocNone);

#if defined(GLX_VERSION_1_1)
        ccp = glXQueryServerString (dpy, scr, GLX_VENDOR);
        if (ccp == NULL) ccp = "NOT AVAILABLE";
        printf ("%*sVendor = %s\n", fieldNameLen, "GLX Server:", ccp);
        ccp = glXQueryServerString (dpy, scr, GLX_VERSION);
        if (ccp == NULL) ccp = "NOT AVAILABLE";
        printf ("%*sVersion = %s\n", fieldNameLen, "", ccp);
        ccp = glXQueryServerString (dpy, scr, GLX_EXTENSIONS);
        if (ccp == NULL) ccp = "NOT AVAILABLE";
        PrintIndentedStringWithBlanks ("Extensions = ", ccp, fieldNameLen,
                                       indent);
        ccp = glXQueryExtensionsString (dpy, scr);
        PrintIndentedStringWithBlanks ("Useable Extensions = ", ccp, fieldNameLen,
                                       indent);
#endif

	w = XCreateWindow (dpy, RootWindow(dpy, scr),
	    0, 0, 1, 1, 0, vi->depth,
	    InputOutput, vi->visual, 
	    CWBackPixel | CWBorderPixel | CWColormap, 
	    &wattribs);
	glXMakeCurrent (dpy, w, ctx);
	cp = (char *) glGetString (GL_VENDOR);
	printf ("%*sVendor = %s\n", fieldNameLen, "GL Extension:", cp);
	cp = (char *) glGetString (GL_RENDERER);
	printf ("%*sRenderer = %s\n", fieldNameLen, "", cp);
	cp = (char *) glGetString (GL_VERSION);
	printf ("%*sVersion = %s\n", fieldNameLen, "", cp);
	cp = (char *) glGetString (GL_EXTENSIONS);
	PrintIndentedStringWithBlanks ("Extensions = ", cp, fieldNameLen,
		indent);
    }

  return;
}

void
PrintScreenVisualInfo(
  Display    *dpy,
  int         scr,
  int         fieldNameLen,
  int	      indent
)
{
  XVisualInfo visualTemplate;		/* fill in for getting info */
  XVisualInfo *visuals;		/* retured info */
  int         nVisuals;		/* number of elements returned */
  int         i;
  char        title[256];
  int         titleLen, fillLen;

  nVisuals = 0;
  visualTemplate.screen = scr;
  visuals = XGetVisualInfo(dpy, VisualScreenMask, &visualTemplate, &nVisuals);

  if (printVids == VID_NONE) {
    sprintf(title, " %d Visuals for Screen %d (default = 0x%lx) ",
            nVisuals, scr, XVisualIDFromVisual(DefaultVisual(dpy, scr)));
    titleLen = strlen(title);
    fillLen = (pageWidth - titleLen) / 2;
    for (i = 0; i < fillLen; i++) printf("%c", '=');
    printf("%s", title);
    for (i = 0; i < fillLen; i++) printf("%c", '=');
    printf("\n");  
  }

  for (i = 0; i < nVisuals; i++) {
    if (printVids != VID_NONE) {
      PrintVisualIDs (dpy, &visuals[i]);
    }
    else {
      if (printCookedVisualInfo)
        PrintCookedVisualInfo (dpy, &visuals[i], indent);
      else
        PrintRawVisualInfo (dpy, &visuals[i], indent);
      printf("\n");
    }
  }
  
  if (visuals)
    XFree((char *) visuals);

}

int
ProcessScreen(
  Display *dpy,
  int screenNum,
  int fieldNameLen,
  int indent
)
{
  if (screenNum >= ScreenCount(dpy)) {
    fprintf(stderr, "%s: screen %d does not exist on display \"%s\".\nscreen count = %d\n",
	    ProgramName, screenNum, XDisplayName(displayname),
            ScreenCount(dpy));
    exit(1);
  }
  
  if (printScreenInfo)
    PrintScreenInfo(dpy, screenNum, fieldNameLen, indent);
  if (printVisualInfo)
    PrintScreenVisualInfo(dpy, screenNum, fieldNameLen, indent);
  return(0);
}

/*****************************************************************************/
/****************************  Exported Functions  ***************************/
/*****************************************************************************/

int
main(
      int argc,
      char *argv[]
)
{
  Display    *dpy;		/* X connection */
  int         screenNum;
  int         fieldNameLen = -24;
  int         indent = 2;

  ProcessArgs(argc, argv);

  dpy = XOpenDisplay(displayname);
  if (!dpy) {
    fprintf(stderr, "%s:  unable to open display \"%s\".\n",
	    ProgramName, XDisplayName(displayname));
    exit(1);
  }

  dpyHasGL = glXQueryExtension(dpy, &glErrorBase, &glEventBase);


  if (printSpecficScreen) {
    if (specficScreenNum >= ScreenCount(dpy)) {
      fprintf(stderr, "%s: screen %d does not exist on display \"%s\".\nscreen count = %d\n",
              ProgramName, specficScreenNum, XDisplayName(displayname),
              ScreenCount(dpy));
      exit(1);
    }
  
    if (printDisplayInfo) PrintDisplayInfo(dpy, fieldNameLen);
    ProcessScreen(dpy, specficScreenNum, fieldNameLen, indent);
  }
  else {
    if (printDisplayInfo) PrintDisplayInfo(dpy, fieldNameLen);
    for (screenNum = 0; screenNum < ScreenCount(dpy); screenNum++) {
      ProcessScreen(dpy, screenNum, fieldNameLen, indent);
    }
  }
  
  return(XCloseDisplay(dpy));
}
