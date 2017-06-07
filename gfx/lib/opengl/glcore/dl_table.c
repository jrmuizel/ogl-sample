/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
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
*/
/*
** Display list table management routines.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/dl_table.c#5 $
*/
#include "context.h"
#include <GL/gl.h>
#include <stdio.h>
#include "dlistint.h"
#include "namesint.h"
#include "global.h"
#include "g_imfncs.h"

/*
** The next three routines are used as callbacks by the 
** name space management code.
*/

/*
** Delete a specified display list.  This typically just means free it,
** but if it is refcounted we just decrement the ref count.
*/
void __glDisposeDlist(__GLcontext *gc, void *pData)
{
    __GLdlist *list;

    list = (__GLdlist *)pData;

    list->refcount--;
    assert(list->refcount >= 0); /* less than zero references? */
    if (list->refcount == 0) {
	/*
	** No one is using this list, we can nuke it.
	*/
	list->free(gc, list);
	return;
    }
}

/*
** These functions are used by the empty dlist of dlist array.
** empty dlist is used as a sentinel function.
*/

/* ARGSUSED */
void
__glEmptyFreeDlist(__GLcontext *gc, __GLdlist *dlist)
{
  /* 
  ** do nothing; empty dlist is a statically allocated structure and
  ** should not be freed.
  */
}

/* ARGSUSED */
void
__glEmptyExecuteDlist(__GLcontext *gc, __GLdlist *dlist)
{
  /* do nothing; empty dlist is a sentinel value */
}

/*
** Sets up a new names tree and returns a pointer to it.
*/
__GLdlistArray *__glDlistNewArray(__GLcontext *gc)
{
    __GLdlistArray *array;

    __GL_DLIST_SEMAPHORE_LOCK();

    array = (__GLdlistArray *) 
	    (*gc->dlist.malloc)(gc, sizeof(__GLdlistArray));
    if (array == NULL) {
	__glSetError(GL_OUT_OF_MEMORY);
	__GL_DLIST_SEMAPHORE_UNLOCK();
	return NULL;
    }
    array->refcount = 1;

    /* call machine-dependent function */
    /*XXXblythe should this be able to fail?*/
    if (gc->dlist.newArray)
	(*gc->dlist.newArray)(gc, array);

    __GL_DLIST_SEMAPHORE_UNLOCK();
    return array;
}

void __glDlistFreeArray(__GLcontext *gc, __GLdlistArray *array)
{
    __GL_DLIST_SEMAPHORE_LOCK();
    /* call machine-dependent function */
    if (gc->dlist.freeArray)
	(*gc->dlist.freeArray)(gc, array);

    (*gc->dlist.free)(gc, array);
    __GL_DLIST_SEMAPHORE_UNLOCK();
}

/*
** __glim entry points for list name management.
*/

GLboolean GLAPI __glim_IsList(GLuint list)
{
    __GLdlistMachine *dlstate;
    __GLnamesArray *array;
    GLboolean exists;

    __GL_SETUP_NOT_IN_BEGIN2();
    __GL_API_GET();

    dlstate = &gc->dlist;
    array = dlstate->namesArray;

    /*
    ** Lock access to dlarray.
    */
    __GL_DLIST_SEMAPHORE_LOCK();
    exists = __glNamesIsName(gc, array, list);
    __GL_DLIST_SEMAPHORE_UNLOCK();

    return exists;
}


GLuint GLAPI __glim_GenLists(GLsizei range)
{
    __GLdlistMachine *dlstate;
    __GLnamesArray *array;
    GLuint start;

    __GL_SETUP_NOT_IN_BEGIN2();
    __GL_API_BLAND();

    dlstate = &gc->dlist;
    array = dlstate->namesArray;

    if (range < 0) {
	__glSetError(GL_INVALID_VALUE);
	return 0;
    }
    if (range == 0) {
	return 0;
    }

    __GL_DLIST_SEMAPHORE_LOCK();

    start = __glNamesGenRange(gc, array, range);

    __GL_DLIST_SEMAPHORE_UNLOCK();

    return start;

}

void GLAPI __glim_ListBase(GLuint base)
{ 
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    gc->state.list.listBase = base;
}

void GLAPI __glim_DeleteLists(GLuint list, GLsizei range)
{
    __GLdlistMachine *dlstate;
    __GLnamesArray *array;

    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_BLAND();

    if (range < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    if (range == 0) return;

    dlstate = &gc->dlist;
    array = dlstate->namesArray;

    __GL_DLIST_SEMAPHORE_LOCK();

    __glNamesDeleteRange(gc, array, list, range);

    __GL_DLIST_SEMAPHORE_UNLOCK();

}
