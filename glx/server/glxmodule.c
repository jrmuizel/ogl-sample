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
** $Header: //depot/main/glx/server/glxmodule.c#9 $
*/

#ifdef XF86
#include <xf86Version.h>
#define XF_Minimum_Version ( (3 * 10) + (9) )
#define XF_Version ( (XF86_VERSION_MAJOR * 10) + (XF86_VERSION_MINOR) )
#endif

#if XF86 && ( XF_Version >= XF_Minimum_Version)
#include "xf86Module.h"
#include "glxserver.h"
#include "glxext.h"

#define glxExtName	"GLX"

static MODULESETUPPROTO(glxSetup);
static MODULESETUPPROTO(libGLSetup);

extern void GlxInit(INITARGS);

ExtensionModule GlxExt =
{
    GlxExtensionInit,
    GLX_EXTENSION_NAME,
    NULL,
    NULL
};

static XF86ModuleVersionInfo glxVersRec =
{
        "glx",
        MODULEVENDORSTRING,
        MODINFOSTRING1,
        MODINFOSTRING2,
        XF86_VERSION_CURRENT,
        1, 0, 0,				/* 1.0 */
        ABI_CLASS_EXTENSION,
        ABI_EXTENSION_VERSION,
	MOD_CLASS_EXTENSION,
        {0,0,0,0}
};

static XF86ModuleVersionInfo glVersRec =
{
        "libGL",
        MODULEVENDORSTRING,
        MODINFOSTRING1,
        MODINFOSTRING2,
        XF86_VERSION_CURRENT,
        1, 0, 0,				/* 1.0 */
        ABI_CLASS_EXTENSION,
        ABI_EXTENSION_VERSION,
	MOD_CLASS_EXTENSION,
        {0,0,0,0}
};

XF86ModuleData glxModuleData = { &glxVersRec, glxSetup, NULL };
XF86ModuleData libGLModuleData = { &glVersRec, libGLSetup, NULL };

static pointer
libGLSetup(pointer module, pointer opts, int *errmaj, int *errmin)
{
    /** return non-NULL value to indicate success **/
    return (pointer) 1;
}

static pointer
glxSetup(pointer module, pointer opts, int *errmaj, int *errmin)
{
    pointer gl_lib = NULL;

    LoadExtension(&GlxExt);

    gl_lib = LoadSubModule(module, "libGL", NULL, NULL, NULL, NULL, 
                           errmaj, errmin);
    if (gl_lib == NULL) ErrorF("[GLX] couldn't load core opengl!!\n");

    /* Need a non-NULL return value to indicate success */
    /* Since we now load libGL as a sub-module, return  */
    /* the pointer to it as an indication of success.   */
    return gl_lib;
}

#endif /** XF86 **/
