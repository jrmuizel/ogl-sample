#ifndef __gl_dlistint_h
#define __gl_dlistint_h

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
** Display list internal structure description.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/dlistint.h#6 $
*/
#include "dlist.h"

#define __GL_DLIST_MIN_ARRAY_BLOCK	16
#define __GL_DLIST_MAX_ARRAY_BLOCK	1024

/* Priority heap data structure */
typedef struct __GLdlistHeapRec {
    GLuint n;			/* number of elems in the heap */
    GLuint arr_size;		/* amount of space allocated in heap */
    __GLdlist **arr;		/* array... */
} __GLdlistHeap;

struct __GLdlistArrayRec {
    GLint refcount; 			/*# ctxs using this array: create with 1, delete at 0*/
    __GLdlistHeap heap;			/* priority queue */
    void *machdep;			/* pointer to machine dependent state */
};


/*************************************************************
 *			 Priority Heap Routines              *
 *************************************************************/

/*
** Initialize the heap information that's stored in dlistArray.
*/
__GLdlistArray *__glDlistNewArray(__GLcontext *gc);

/*
** Free the heap information that's stored in dlistArray.
*/
void __glDlistFreeArray(__GLcontext *gc, __GLdlistArray *array);


/*
** Initialize a heap.  Returns true on success, false on failure. 
*/
GLint __glDlistHeapInit(__GLdlistHeap *);

/*
** Free memory associated with a heap, but not the heap itself.
*/
GLvoid __glDlistHeapFree(__GLdlistHeap *);

/*
** Return the element with the lowest priority, or NULL if the heap is
** empty.
*/
__GLdlist *__glDlistHeapQueryMin(__GLdlistHeap *);

/* 
** Remove and return the element with the lowest priority 
** (our heap is kinda backwards in that things with the lowest priority
** go at the top) 
*/
__GLdlist *__glDlistHeapRemoveMin(__GLdlistHeap *);

/* 
** Add an element to the heap.  this may fail if we are unable to grow
** arr. 
*/
GLint __glDlistHeapInsert(__GLdlistHeap *, __GLdlist *);

/* 
** Remove an item from the heap.  this routine assumes that the item 
** is in the heap. 
*/
GLvoid __glDlistHeapRemove(__GLdlistHeap *, __GLdlist *);

/* 
** Update the heap after an item changes priority
*/
GLvoid __glDlistHeapPriorityChanged(__GLdlistHeap *, __GLdlist *);

/* dl_table.c */

extern void __glDisposeDlist(__GLcontext *gc, void *pData);
extern void __glEmptyFreeDlist(__GLcontext *gc, __GLdlist *dlist);
extern void __glEmptyExecuteDlist(__GLcontext *gc, __GLdlist *dlist);
extern __GLdlistArray *__glDlistNewArray(__GLcontext *gc);
extern void __glDlistFreeArray(__GLcontext *gc, __GLdlistArray *array);


#endif /* __gl_dlistint_h */
