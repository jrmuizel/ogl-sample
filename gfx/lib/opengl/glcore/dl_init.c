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
** Display list init/destroy code.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/dl_init.c#5 $
*/
#include <stdlib.h>
#include "context.h"
#include "dlistint.h"
#include "dlistopt.h"
#include "namesint.h"

/*
** Used to share display lists between two different contexts.
*/
void __glShareDlist(__GLcontext *dst, __GLcontext *src)
{
    /* First get rid of our private display list state */
    __glFreeDlistState(dst);

    /*
    ** There are two structures for dlist array info, because
    ** the namesArray info is dlist independent, and there is
    ** some dlist-dependent memory management information left in
    ** the dlistArray structure.
    */
    dst->dlist.dlistArray = src->dlist.dlistArray;
    dst->dlist.dlistArray->refcount++;
    dst->dlist.namesArray = src->dlist.namesArray;
    dst->dlist.namesArray->refcount++;
}

void __glInitDlistState(__GLcontext *gc)
{
    __GLdlistMachine *dlist;

    dlist = &gc->dlist;

    dlist->nesting = 0;
    dlist->currentList = 0;

    /* By default use the regular memory routines for dlist memory too */
    dlist->malloc = gc->imports.malloc;
    dlist->realloc = gc->imports.realloc;
    dlist->free = gc->imports.free;

    /*
    ** To preserve some dlist memory usage housekeeping that was being 
    ** stored in the dlistArray structure, we need to still allocate
    ** and initialize the dlistArray.
    */
    if (dlist->dlistArray == NULL) {
	dlist->dlistArray = __glDlistNewArray(gc);
    }
    if (dlist->namesArray == NULL) {
	__GL_DLIST_SEMAPHORE_LOCK();
	dlist->namesArray = __glNamesNewArray(gc, __GL_NAMES_DLIST);
	__GL_DLIST_SEMAPHORE_UNLOCK();
    }
}

void __glFreeDlistState(__GLcontext *gc)
{
    gc->dlist.namesArray->refcount--;

    if (gc->dlist.namesArray->refcount == 0) {
	__GL_DLIST_SEMAPHORE_LOCK();
	__glNamesFreeArray(gc, gc->dlist.namesArray);
	__GL_DLIST_SEMAPHORE_UNLOCK();
    }
    gc->dlist.namesArray = NULL;

    /*
    ** The stuff in dlistArray takes care of memory management issues
    ** for display lists. 
    */
    gc->dlist.dlistArray->refcount--;

    if (gc->dlist.dlistArray->refcount == 0) {
	__glDlistFreeArray(gc, gc->dlist.dlistArray);
    }
    gc->dlist.dlistArray = NULL;

    /*
    ** finally, free the dlist arena
    */
    if (gc->dlist.arena != NULL) {
	(*gc->procs.memory.deleteArena)(gc->dlist.arena);
    }
}
