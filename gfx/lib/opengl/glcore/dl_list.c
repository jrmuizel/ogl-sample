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
*/
/*
** Basic display list routines.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/dl_list.c#16 $
*/
#include "dlistint.h"
#include "dlistopt.h"
#include "namesint.h"
#include "global.h"
#include "g_imports.h"
#include "context.h"
#if defined(__GL_USE_ASMCODE) && defined(mips)
#include "mips.h"
#endif /* __GL_USE_ASMCODE && mips */
#include "g_disp.h"
#include "g_lcomp.h"
#include "g_listop.h"
#include "g_imfncs.h"
#include "g_lcefncs.h"

/*
** Arbitrary limit for looking up multiple display lists at once 
** (with glCallLists()).  Any number from 128 to 1024 should work well.
** This value doesn't change the functionality of OpenGL at all, but
** will make minor variations to the performance characteristics.
*/
#define MAX_LISTS_CACHE 256

/* splists need this table too */
const GLubyte __GLdlsize_tab[] = {
      /* GL_BYTE		*/	1,
      /* GL_UNSIGNED_BYTE	*/	1,
      /* GL_SHORT		*/	2,
      /* GL_UNSIGNED_SHORT	*/	2,
      /* GL_INT			*/	4,
      /* GL_UNSIGNED_INT	*/	4,
      /* GL_FLOAT		*/	4,
      /* GL_2_BYTES		*/	2,
      /* GL_3_BYTES		*/	3,
      /* GL_4_BYTES		*/	4,
};

#define __glCallListsSize(type)				\
	((type) >= GL_BYTE && (type) <= GL_4_BYTES ?	\
	__GLdlsize_tab[(type)-GL_BYTE] : -1)

GLint __glCallLists_size(GLsizei n, GLenum type)
{
    return __glCallListsSize(type) * n;
}

void GLAPI __glim_NewList(GLuint list, GLenum mode)
{
    __GLdlistMachine *dlstate;
    __GLdispatchState *globalDispatch;
    __GL_SETUP_NOT_IN_BEGIN();
    __GL_API_FLUSH();
    
    dlstate = &gc->dlist;

    /* Valid mode? */
    switch(mode) {
      case GL_COMPILE:
      case GL_COMPILE_AND_EXECUTE:
	break;
      default:
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    if (dlstate->currentList) {
	/* Must call EndList before calling NewList again! */
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    if (list == 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (dlstate->listExec && dlstate->arena == NULL) {
	/*
	** Initialize the arena allocator when using the old 
	** dlist mechanism.
	*/
	if (!(dlstate->arena = (*gc->procs.memory.newArena)(gc))) {
	    /*
	    ** Spec says nothing about this, but it seems like the right 
	    ** approach to me.
	    */
	    __glSetError(GL_OUT_OF_MEMORY);
	    return;
	}
    }

    /*
    ** Save current dispatch pointers into saved state in context.  Then
    ** switch to the list tables.
    */
    globalDispatch = &__gl_dispatch;
    __glCopyDispatch(globalDispatch, &gc->savedDispatchState);
#ifdef NEW_LISTS
    /* only use the compile and execute function if we are really using the
    ** direct renderer
    */
    if (mode == GL_COMPILE_AND_EXECUTE && !dlstate->listExec) {
        __glCopyDispatch(gc->listCompExecState, globalDispatch);
    } else {
        __glCopyDispatch(&gc->listCompState, globalDispatch);
    }
#else
    __glCopyDispatch(&gc->listCompState, globalDispatch);
#endif
    gc->dispatchState = &gc->savedDispatchState;
    
    dlstate->currentList = list;
    dlstate->mode = mode;
    dlstate->listData.genericFlags = 0;
    dlstate->listData.machineFlags = 0;
    dlstate->listData.dlist = NULL;
    dlstate->listData.lastDlist = NULL;

    (*dlstate->initState)(gc);
}

void GLAPI __glce_EndList(void)
{
    __GL_SETUP();

    __gl_dispatch.dispatch.EndList = gc->savedDispatchState.dispatch.EndList;
    /* 
    ** have to dispatch through the active state, since glEndList()
    ** will dispatch thru the NT dispatch table, which we did not load
    */
    (*__gl_dispatch.dispatch.EndList)();
}

void GLAPI __glim_EndList(void)
{
    __GLdlistMachine *dlstate;
    __GLdlist *dlist;
    __GLcompiledDlist *compDlist;
    __GL_SETUP_NOT_IN_BEGIN();

    dlstate = &gc->dlist;

    /* Must call NewList() first! */
    if (dlstate->currentList == 0) {
	__glSetError(GL_INVALID_OPERATION);
	return;
    }

    /*
    ** First we optimize the display list by invoking the machine specific
    ** optimizer.
    */
    compDlist = &dlstate->listData;
    (*dlstate->optimizer)(gc, compDlist);

    /*
    ** Now we call the implementation-specific compiler. It does the following:
    **
    ** compute and allocate space for compiled dlist
    **
    ** creates an implmentation-specific compiled dlist using information
    ** from linked list.
    **
    ** creates a dlist node of type __GLdlist
    **
    ** fills in all elements of __GLdlist structure appropriately;
    **
    **    attaching the compiled display list as one or more segments,
    ** 
    **    attaching an execute function to display the compiled list,
    **
    **    allocating and filling in any local data needed by the execute
    **    or free functions,
    **
    **    attaching a free function that will delete the list, local data, and
    **    __GLdlist node.
    **
    ** returns a pointer to the decorated __GLdlist structure
    */

    dlist = (*dlstate->compiler)(gc, compDlist);

    /* free linked list memory */

    if (dlstate->listExec) /* XXXblythe */
	(*gc->procs.memory.freeAll)(dlstate->arena);
    compDlist->dlist = NULL;
    compDlist->lastDlist = NULL;


    /*
    ** Now we connect the new, compiled dlist to the display list tree
    */

    __GL_DLIST_SEMAPHORE_LOCK();
    if (!__glNamesNewData(gc, gc->dlist.namesArray, 
				dlstate->currentList, dlist)) {
	/* 
	** No memory!
	** Nuke the list! 
	*/
	if (dlist) {
	  (*dlist->free)(gc,dlist);
	}
    }
    __GL_DLIST_SEMAPHORE_UNLOCK();


    /* Switch back to saved dispatch state (compile mode -> immediate mode */
    __glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
    gc->dispatchState = &__gl_dispatch;

    dlstate->currentList = 0;
}

static void DoCallList(GLuint list)
{
    __GLdlist *dlist;
    __GL_SETUP();

    if (gc->dlist.nesting >= __GL_MAX_LIST_NESTING) {
	/* Force unwinding of the display list */
	gc->dlist.nesting = __GL_MAX_LIST_NESTING*2;
	return;
    }

    __GL_DLIST_SEMAPHORE_LOCK();

    if ((dlist = __glNamesLockData(gc, gc->dlist.namesArray, list)) == NULL) {
	__GL_DLIST_SEMAPHORE_UNLOCK();
	return;
    }

    gc->dlist.nesting++;

    /* no longer check for empty dlist: empty dlist does nothing */
    (*dlist->execute)(gc, dlist);

    gc->dlist.nesting--;

    __glNamesUnlockData(gc, (void *)dlist);

    __GL_DLIST_SEMAPHORE_UNLOCK();
}



/*
** Display list compilation and execution versions of CallList and CallLists
** are maintained here for the sake of sanity.  Note that __glle_CallList
** may not call __glim_CallList or it will break the infinite recursive
** display list prevention code.
*/

void GLAPI __gllc_CallList(GLuint list)
{
    __GLdlistOp *dlop;
    struct __gllc_CallList_Rec *data;
    __GL_SETUP();

    if (list == 0) {
	__gllc_InvalidEnum(gc);
	return;
    }

    dlop = __glDlistAllocOp2(gc, sizeof(struct __gllc_CallList_Rec));
    if (dlop == NULL) return;
    dlop->opcode = __glop_CallList;
    data = (struct __gllc_CallList_Rec *) dlop->data;
    data->list = list;
    __glDlistAppendOp(gc, dlop, __glle_CallList);
}

const GLubyte *__glle_CallList(const GLubyte *PC)
{
    const struct __gllc_CallList_Rec *data;

    data = (const struct __gllc_CallList_Rec *) PC;
    DoCallList(data->list);
    return PC + sizeof(struct __gllc_CallList_Rec);
}


void GLAPI __glce_CallList(GLuint list)
{
#ifndef NEW_LISTS
    __GLdispatchState compExecState = __gl_dispatch;
#endif /* NEW_LISTS */
    __GL_SETUP();

    /* compile... */
    __glCopyDispatch(&gc->listCompState, &__gl_dispatch);
    glCallList(list);
    /* and execute... */
    __glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
    gc->dispatchState = &__gl_dispatch;
    glCallList(list);
    __glCopyDispatch(&__gl_dispatch, &gc->savedDispatchState);
    gc->dispatchState = &gc->savedDispatchState;
    /* and go back to compiling and executing */
#ifdef NEW_LISTS
    __glCopyDispatch(gc->listCompExecState, &__gl_dispatch);
#else /* NEW_LISTS */
    __glCopyDispatch(&compExecState, &__gl_dispatch);
#endif /* NEW_LISTS */
}

void GLAPI __glce_CallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
#ifndef NEW_LISTS
    __GLdispatchState compExecState = __gl_dispatch;
#endif
    __GL_SETUP();

    /* compile... */
    __glCopyDispatch(&gc->listCompState, &__gl_dispatch);
    glCallLists(n, type, lists);
    /* and execute... */
    __glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
    gc->dispatchState = &__gl_dispatch;
    glCallLists(n, type, lists);
    __glCopyDispatch(&__gl_dispatch, &gc->savedDispatchState);
    gc->dispatchState = &gc->savedDispatchState;
    /* and go back to compiling and executing */
#ifdef NEW_LISTS
    __glCopyDispatch(gc->listCompExecState, &__gl_dispatch);
#else /* NEW_LISTS */
    __glCopyDispatch(&compExecState, &__gl_dispatch);
#endif /* NEW_LISTS */
}

void GLAPI __glim_CallList(GLuint list)
{
    __GL_SETUP();
    
    if (list == 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }
    DoCallList(list);
    gc->dlist.nesting = 0;
}

static void DoCallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
    __GLdlist *dlists[MAX_LISTS_CACHE];
    __GLdlist *dlist;
    __GLdlistMachine *dlstate;
    GLint i, dlcount, datasize;
    const GLubyte *listiter;
    __GL_SETUP();

    dlstate = &gc->dlist;

    datasize = __glCallListsSize(type);

    if (dlstate->nesting >= __GL_MAX_LIST_NESTING) {
	/* Force unwinding of the display list */
	dlstate->nesting = __GL_MAX_LIST_NESTING*2;
	return;
    }
    dlstate->nesting++;

    i=0;
    listiter = (const GLubyte *) lists;
    while (n) {
	dlcount = n;
	if (dlcount > MAX_LISTS_CACHE) dlcount = MAX_LISTS_CACHE;

	__GL_DLIST_SEMAPHORE_LOCK();
	__glNamesLockDataList(gc, gc->dlist.namesArray, dlcount, type, 
		gc->state.list.listBase, 
		(const GLvoid *) listiter, (void **)dlists);

	i = 0;
	while (i < dlcount) {
	    dlist = dlists[i];
	    (*dlist->execute)(gc, dlist);
	    i++;
	  }

	__glNamesUnlockDataList(gc, dlcount, (void **)dlists);
	__GL_DLIST_SEMAPHORE_UNLOCK();

	listiter += dlcount * datasize;
	n -= dlcount;
    }

    dlstate->nesting--;
}

/*
** Display list compilation and execution versions of CallList and CallLists
** are maintained here for the sake of sanity.  Note that __glle_CallLists
** may not call __glim_CallLists or it will break the infinite recursive
** display list prevention code.
*/
void GLAPI __gllc_CallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
    __GLdlistOp *dlop;
    GLuint size;
    GLint arraySize;
    struct __gllc_CallLists_Rec *data;
    __GL_SETUP();

    if (n < 0) {
	__gllc_InvalidValue(gc);
	return;
    }
    arraySize = n*__glCallListsSize(type);
    if (arraySize < 0) {
	__gllc_InvalidEnum(gc);
	return;
    }
    size = (GLuint)sizeof(struct __gllc_CallLists_Rec) + __GL_PAD(arraySize);
    dlop = __glDlistAllocOp2(gc, size);
    if (dlop == NULL) return;
    dlop->opcode = __glop_CallLists;
    data = (struct __gllc_CallLists_Rec *) dlop->data;
    data->n = n;
    data->type = type;
    size = (GLuint)sizeof(struct __gllc_CallLists_Rec);
    __GL_MEMCOPY(dlop->data + size, lists, arraySize);
    __glDlistAppendOp(gc, dlop, __glle_CallLists);
}

const GLubyte *__glle_CallLists(const GLubyte *PC)
{
    GLuint size;
    GLuint arraySize;
    const struct __gllc_CallLists_Rec *data;

    data = (const struct __gllc_CallLists_Rec *) PC;
    (void) DoCallLists(data->n, data->type, (const GLvoid *) (data+1));
    arraySize = __GL_PAD(data->n*__glCallListsSize(data->type));
    size = (GLuint)sizeof(struct __gllc_CallLists_Rec) + arraySize;
    return PC + size;
}

void GLAPI __glim_CallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
    __GL_SETUP();

    if (n < 0) {
	__glSetError(GL_INVALID_VALUE);
	return;
    }

    if (__glCallListsSize(type) == -1) {
	__glSetError(GL_INVALID_ENUM);
	return;
    }

    DoCallLists(n, type, lists);
    gc->dlist.nesting = 0;
}

void GLAPI __glce_DrawArraysEXT(GLenum mode, GLint first, GLsizei count)
{
#ifndef NEW_LISTS
    __GLdispatchState compExecState = __gl_dispatch;
#endif
    __GL_SETUP();

    /* compile... */
    __glCopyDispatch(&gc->listCompState, &__gl_dispatch);
    glDrawArrays(mode, first, count);
    /* and execute... */
    __glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
    gc->dispatchState = &__gl_dispatch;
    glDrawArrays(mode, first, count);
    __glCopyDispatch(&__gl_dispatch, &gc->savedDispatchState);
    gc->dispatchState = &gc->savedDispatchState;
    /* and go back to compiling and executing */
#ifdef NEW_LISTS
    if (!__gl_dispatchOverride)
      __glCopyDispatch(gc->listCompExecState, &__gl_dispatch);
    else 
      __glCopyDispatch(__gl_dispatchOverride, &__gl_dispatch);
#else
    __glCopyDispatch(&compExecState, &__gl_dispatch);
#endif
}

void GLAPI __glce_DrawElements(GLenum mode, GLsizei count, GLenum type,
			       const GLvoid *indices)
{
#ifndef NEW_LISTS
    __GLdispatchState compExecState = __gl_dispatch;
#endif
    __GL_SETUP();

    /* compile... */
    __glCopyDispatch(&gc->listCompState, &__gl_dispatch);
    glDrawElements(mode, count, type, indices);
    /* and execute... */
    __glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
    gc->dispatchState = &__gl_dispatch;
    glDrawElements(mode, count, type, indices);
    __glCopyDispatch(&__gl_dispatch, &gc->savedDispatchState);
    gc->dispatchState = &gc->savedDispatchState;
    /* and go back to compiling and executing */
#ifdef NEW_LISTS
    if (!__gl_dispatchOverride)
      __glCopyDispatch(gc->listCompExecState, &__gl_dispatch);
    else 
      __glCopyDispatch(__gl_dispatchOverride, &__gl_dispatch);
#else
    __glCopyDispatch(&compExecState, &__gl_dispatch);
#endif
}

void GLAPI __glce_DrawRangeElements(GLenum mode, GLuint start, GLuint end,
				    GLsizei count, GLenum type,
				    const GLvoid *indices)
{
#ifndef NEW_LISTS
    __GLdispatchState compExecState = __gl_dispatch;
#endif
    __GL_SETUP();

    /* compile... */
    __glCopyDispatch(&gc->listCompState, &__gl_dispatch);
    glDrawRangeElements(mode, start, end, count, type, indices);
    /* and execute... */
    __glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
    gc->dispatchState = &__gl_dispatch;
    glDrawRangeElements(mode, start, end, count, type, indices);
    __glCopyDispatch(&__gl_dispatch, &gc->savedDispatchState);
    gc->dispatchState = &gc->savedDispatchState;
    /* and go back to compiling and executing */
#ifdef NEW_LISTS
    if (!__gl_dispatchOverride)
      __glCopyDispatch(gc->listCompExecState, &__gl_dispatch);
    else 
      __glCopyDispatch(__gl_dispatchOverride, &__gl_dispatch);
#else
    __glCopyDispatch(&compExecState, &__gl_dispatch);
#endif
}

void GLAPI __glce_ArrayElementEXT(GLint i)
{
#ifndef NEW_LISTS
    __GLdispatchState compExecState = __gl_dispatch;
#endif
    __GL_SETUP();

    /* compile... */
    __glCopyDispatch(&gc->listCompState, &__gl_dispatch);
    glArrayElement(i);
    /* and execute... */
    __glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
    gc->dispatchState = &__gl_dispatch;
    glArrayElement(i);
    __glCopyDispatch(&__gl_dispatch, &gc->savedDispatchState);
    gc->dispatchState = &gc->savedDispatchState;
    /* and go back to compiling and executing */
#ifdef NEW_LISTS
    if (!__gl_dispatchOverride)
      __glCopyDispatch(gc->listCompExecState, &__gl_dispatch);
    else 
      __glCopyDispatch(__gl_dispatchOverride, &__gl_dispatch);
#else
    __glCopyDispatch(&compExecState, &__gl_dispatch);
#endif
}
/************************************************************************/

#ifdef OBSOLETE
/*
** This routine used to be used to allocate a dlist op without setting the
** OUT_OF_MEMORY error code if no memory was available.  It was never used,
** so has been merged into __glDlistAllocOp2 and deleted.
*/
__GLdlistOp *__glDlistAllocOp(__GLcontext *gc, GLuint size)
{
}
#endif

__GLdlistOp *__glDlistAllocOp2(__GLcontext *gc, GLuint size)
{
    __GLdlistOp *newDlistOp;
    size_t memsize;
    __GLdlistOp op;

    /* 
    ** This is pretty ugly.  It does make this code portable, though.
    */
    memsize = (size_t) (((GLubyte *) op.data) - ((GLubyte *) &op) + size);
    newDlistOp = (__GLdlistOp *) (*gc->procs.memory.alloc)(gc->dlist.arena, 
	    (unsigned int)memsize);

    if (newDlistOp == NULL) {
	__glSetError(GL_OUT_OF_MEMORY);
	return NULL;
    }

    /* Assumed below!  Assert here in case someone fails to read the 
    ** warnings in the header file! 
    */
    assert((((char *) (newDlistOp->data) - (char *) (newDlistOp)) & 7) == 0);

    newDlistOp->next = NULL;
    newDlistOp->size = size;
    newDlistOp->dlistFree = NULL;
    newDlistOp->aligned = GL_FALSE;

    return newDlistOp;
}

/* ARGSUSED */
void __glDlistFreeOp(__GLcontext *gc, __GLdlistOp *dlistOp)
{
    /*
    ** This used to do something, but no more.  With the arena style memory
    ** manager, it is obsolete.  It should still be called by display list 
    ** optimizers for both completeness and to allow this interface to be
    ** reworked as necessary.
    */
    return;
}

void __glDlistAppendOp(__GLcontext *gc, __GLdlistOp *newop, 
		       __GLlistExecFunc *listExec)
{
    __GLdlistMachine *dlstate;
    __GLcompiledDlist *list;
    __GLdispatchState *compExec;

    dlstate = &gc->dlist;
    list = &dlstate->listData;

    (*dlstate->checkOp)(gc, newop);

    if (dlstate->mode == GL_COMPILE_AND_EXECUTE) {
	/* switch to execute state */
	__glCopyDispatch(&gc->savedDispatchState, &__gl_dispatch);
	gc->dispatchState = &__gl_dispatch;

        (*listExec)(newop->data); /* slows down C & E, but makes exec fast */

	/* restore compilation state */
	__glCopyDispatch(&__gl_dispatch, &gc->savedDispatchState);
	compExec = &gc->listCompState;
	__glCopyDispatch(compExec, &__gl_dispatch);
	gc->dispatchState = &gc->savedDispatchState;
    }
    if (list->lastDlist) {
	list->lastDlist->next = newop;
    } else {
	list->dlist = newop;
    }
    list->lastDlist = newop;
}


