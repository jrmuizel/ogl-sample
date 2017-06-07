#ifndef __gl_dlist_h_
#define __gl_dlist_h_

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
** Display list state descriptions.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/dlist.h#7 $
*/
#include "types.h"

typedef const GLubyte *__GLlistExecFunc(const GLubyte *);

/* 
** Maximum recursive nesting of display list calls.
*/
#define __GL_MAX_LIST_NESTING		64

/*
** Stub macros that can later be filled in to allow multi-threaded 
** applications to share the same display list space.  This problem needs
** to be thought out carefully, because we don't really want to 
** slow an application down to perform semaphore locking if it isn't 
** sharing any display lists (maybe it isn't too bad, only a couple locks
** are required per glCallLists() call).
*/
#define __GL_DLIST_SEMAPHORE_LOCK()
#define __GL_DLIST_SEMAPHORE_UNLOCK()

/* 
** Machine specific opcodes should start here.  All opcodes lower than 
** this are reserved by the generic code.
*/
#define __GL_MACHINE_DLIST_OPCODE	10000

/*
** A compiled, unoptimized dlist.  Conceptually a linked list of operations.
** An optimizer may work through the operations and delete, add, or change 
** them.
**
** These are only stored transiently.  They are created, optimized, and 
** converted into optimized dlists.
**
** This structure *MUST* be set up so that data is doubleword aligned!
*/
typedef struct __GLdlistOpRec __GLdlistOp;
struct __GLdlistOpRec {
    __GLdlistOp *next;		/* Linked list chain */
    void (*dlistFree)(__GLcontext *gc, GLubyte *);
				/* This dlist free function is called when
				** the entire dlist is freed.  It is passed
				** a pointer to data.  It should *not* free
				** data, but only any memory that has been
				** allocated and is pointed to by the
				** structure contained in data (which will
				** be freed after this function returns).
				*/ 
    GLuint size;		/* Actual size of data */
    GLshort opcode;		/* Opcode for this operation */
    GLboolean aligned;		/* GL_TRUE if data needs to be doubleword 
				** aligned.
				*/
    GLboolean pad1;		/* Padding */
    GLubyte data[4];		/* Variable size */
};

typedef struct __GLcompiledDlistRec {
    GLint freeCount;		/* Number of free functions defined */
    GLuint genericFlags;	/* Flags needed by generic optimizers */
    GLuint machineFlags;	/* Machine controlled flags */
    __GLdlistOp *dlist;		/* The linked list of operations */
    __GLdlistOp *lastDlist;	/* For quick appends */
} __GLcompiledDlist;

/*
** Function used to execute a segment
*/
typedef 
void (*__GLDlistSegExecFn)(__GLcontext *gc, GLubyte *data, GLubyte *segment);

/*
** Structure for display list segments. The contents field is variable
** length.
** XXX not used
*/

/*
typedef struct __GLdlistSegRec {
    __GLDlistSegExecFn *segexec; * function to execute this segment *
    GLubyte contents[4]; * contents of segment (implementation specific) *
} __GLdlistSeg;
*/


/* needed up here for dlist references */

typedef struct __GLdlistRec __GLdlist;


/*
** Implementation-specific function called to free display list
*/
typedef
void (*__GLDlistFreeFn)(__GLcontext *gc, __GLdlist *dlist);


/*
** Implementation-specific function called to attempt to get more
** blocks when block allocator fails (ie by swapping a list out).  See
** more complete description below.
*/
typedef
int (*__GLDlistMoreBlocksFn)(__GLcontext *gc);


/*
** Function called to validate (if necessary) and execute display lists. 
** This function is supplied by the implementation
*/
typedef 
void (*__GLDlistExecFn)(__GLcontext *gc, __GLdlist *dlist);


/* 
** A fully optimized dlist.  One of these is stored for every permanent
** dlist.
**
** NOTE: 'local' is assumed to start at word offset, but NOT a double word
** offset!
*/
struct __GLdlistRec {
    GLint refcount; /* number of references; created with 1; delete when 0 */
    __GLDlistFreeFn free; /* function called to free display list */
    __GLDlistExecFn execute; /* called to execute this display list */
    GLubyte *segment; /* points to first segment of display list */
    GLubyte local[4];  /* local data (variable size) used by execute function */
};

/*
** Some data structure for storing and retrieving display lists quickly.
** This structure is kept hidden so that a new implementation can be used
** if desired.
*/
typedef struct __GLdlistArrayRec __GLdlistArray;

typedef struct __GLdlistMachineRec {
    __GLdlistArray *dlistArray;
    __GLnamesArray *namesArray;

    /*
     ** implementation-specific function called when new dlist array is 
     ** created and destroyed
     */
    void (*newArray)(__GLcontext *gc, __GLdlistArray *array);
    void (*freeArray)(__GLcontext *gc, __GLdlistArray *array);


    /*
    ** The optimizer for the display list.  Runs through a __GLcompiledDlist
    ** and deletes, changes, adds operations.  Presumably, this optimizer
    ** will be a set of function calls to other optimizers (some provided
    ** by the generic dlist code, some by machine specific code).
    */
    void (*optimizer)(__GLcontext *gc, __GLcompiledDlist *);

    /*
    ** The compiler for the display list.  Runs through a __GLcompiledDlist
    ** and passes back a pointer to the implementation-dependent display list.
    ** This pointer is then attached to the 2-3 tree. If the compiler function
    ** fails, it must free any memory allocated and set the appropriate
    ** error message.
    **
    ** It allocates returns a pointer to a filled out __GLdlist structure
    */

    __GLdlist *(*compiler)(__GLcontext *gc, __GLcompiledDlist *input_list);

    /*
    ** This routine is called before puting each new command into the 
    ** display list at list compilation time.
    */
    void (*checkOp)(__GLcontext *gc, __GLdlistOp *);

    /*
    ** This routine is called when a new display list is about to be 
    ** compiled.
    */
    void (*initState)(__GLcontext *gc);

    /* 
    ** Array of functions pointers used for display list execution of 
    ** generic ops.
    */
    __GLlistExecFunc **baseListExec;

    /* 
    ** Array of functions pointers used for display list execution of 
    ** generic optimizations.
    */
    __GLlistExecFunc **listExec;

    /*
    ** The machine specific list execution routines.  These function
    ** pointers are bound into the display list at list compilation time,
    ** so it is illegal to be changing these dynamically based upon the 
    ** machine state.  Any optimizations based upon the current state need
    ** to be performed in the machine specific code.  The first entry of
    ** this array corresponds to opcode __GL_MACHINE_DLIST_OPCODE, and 
    ** subsequent entries correspond to subsequent opcodes.
    **
    ** machineListExec is a pointer to an array of function pointers.
    */
    __GLlistExecFunc **machineListExec;

    /*
    ** The machine specific dlist free routine.  Also bound to the display
    ** list at compile time.
    */
    __GLDlistFreeFn machineListFree;

    /* 
    ** Machine specific routine to try to get more space when the block
    ** allocator fails and we really need a place to put something.  Returns
    ** 0 if it was unable to help us, non-zero otherwise.  If this returns
    ** non-zero, the next call to the block allocator should succeed. 
    */
    __GLDlistMoreBlocksFn machineMoreBlocks;

    /*
    ** Machine specific routine to execute a display list which will be 
    ** set in the __GLdlist structure.  The pointer inside the dlist may
    ** be changed for subsequent calls to the display list.
    */
    __GLDlistExecFn machineListValidateExec;

    /*
    ** If a list is being executed (glCallList or glCallLists) then this
    ** is the current nesting of calls.  It is constrained by the limit
    ** __GL_MAX_LIST_NESTING (this prevents infinite recursion).
    */
    GLint nesting;

    /*
    ** GL_COMPILE or GL_COMPILE_AND_EXECUTE.
    */
    GLenum mode;

    /*
    ** List being compiled - 0 means none.
    */
    GLuint currentList;

    /*
    ** Data for the current list being compiled.
    */
    __GLcompiledDlist listData;

    /*
    ** For fast memory manipulation.  Check out soft/so_memmgr for details.
    */
    __GLarena *arena;

    /* 
    ** Memory managers for dlist objects.  All dlist operations will use 
    ** these to acquire the final block of memory which will store the display
    ** list.
    */

    /* 
    ** Allocate a block of memory of the given size.  A return value of NULL
    ** indicates an OUT_OF_MEMORY error.
    */
    void * (*malloc)(__GLcontext *gc, size_t size);

    /*
    ** Expand the size of a block of memory.  oldsize indicates the current
    ** size of the block (when allocated with alloc), newsize indicates the
    ** desired size.  The return value is the location of the new (possibly
    ** relocated) block.  A return value of NULL indicates an OUT_OF_MEMORY
    ** error.  In this case, the old block should be left intact.
    **
    ** Note that realloc is not alllowed to return NULL if the newsize
    ** is smaller than the old size.
    */
    void * (*realloc)(__GLcontext *gc, void *oldmem, size_t newsize);

    /*
    ** Deallocate a block of memory.  size indicates the size of the block.
    */
    void (*free)(__GLcontext *gc, void *memory);
#ifdef NEW_LISTS
    /* state variables for SP dlists */
    GLvoid *(*spBalloc)(__GLcontext *gc, GLuint *amount);	/* block allocator function */
    GLvoid (*spBfree)(__GLcontext *gc, void *ptr);	/* block free function */
    GLvoid (*spCompMachdep)(__GLcontext *gc, __GLdlist *dlist);
	/* post compilation machine-dependent hook */
    GLvoid **spExecOps;	/* pointer to table of execution operations */
    GLvoid *spPushExec;	/* pointer to "push" execution function */
    GLint spdlSize;	/* current chunk size */
    GLvoid *spdlStart;	/* current chunk start */
    GLvoid *spdlPtr;	/* current chunk pointer */
    GLint spdlPcnt;	/* current chunk "push" count */
    GLvoid **spFreeLst; /* pointer to list of free functions */
    GLint spFreeCnt;	/* count of free functions */
    GLuint spFlags;	/* bookkeeping flags for optimizations */
    GLvoid *spIlist;	/* pointer to interpreter list */
    GLuint spIcnt;	/* Ilist cnt */
#endif
} __GLdlistMachine;

extern void __glDestroyDisplayLists(__GLcontext *gc);
extern void __glShareDlist(__GLcontext *gc, __GLcontext *share_cx);

/* 
** Create and destroy display list ops.  __glDlistAllocOp2() sets an
** out of memory error before returning NULL if there is no memory left.
*/
extern __GLdlistOp *__glDlistAllocOp(__GLcontext *gc, GLuint size);
extern __GLdlistOp *__glDlistAllocOp2(__GLcontext *gc, GLuint size);
extern void __glDlistFreeOp(__GLcontext *gc, __GLdlistOp *op);

/*
** Append the given op to the currently under construction list.
*/
extern void __glDlistAppendOp(__GLcontext *gc, __GLdlistOp *newop, 
			      __GLlistExecFunc *listExec);

/*
** Create and destroy optimized display lists.
*/
extern __GLdlist *__glAllocDlist(__GLcontext *gc, GLuint size);
extern void __glFreeDlist(__GLcontext *gc, __GLdlist *dlist);

/*
** Generic dlist memory manager.
*/
extern void *__glDlistAlloc(GLuint size);
extern void *__glDlistRealloc(void *oldmem, GLuint oldsize, GLuint newsize);
extern void __glDlistFree(void *memory, GLuint size);

/* s_dlist.c */
extern void __glGenericFreeDlist(__GLcontext *gc, __GLdlist *dlist);
extern const GLubyte *__glle_Sentinel(const GLubyte *PC);
extern void __glDoInterpret(GLubyte *PC);
extern void __glGenericExecuteDlist(__GLcontext *gc, __GLdlist *dlist);
extern __GLdlist *__glGenericAllocDlist(__GLcontext *gc, GLuint segsize,
				  GLuint freeCount);
extern const GLubyte *__glle_Nop(const GLubyte *PC);
extern void __glNopFreeDlist(__GLcontext *gc, __GLdlist *dlist);
extern void __glNopExecuteDlist(__GLcontext *gc, __GLdlist *dlist);
extern __GLdlist *__glNopAllocDlist(__GLcontext *gc, GLuint size);
extern __GLdlist *__glNopDlistCompiler(__GLcontext *gc, 
				       __GLcompiledDlist *input_list);

/*
** Generic table of display list execution routines.
*/
extern __GLlistExecFunc *__glListExecTable[];

/* 
** Table of glFoo function pointers to be used in compile and execute 
** display list routines
*/
extern __GLdispatchState __gl_apiDispatchState;

#endif /* __gl_dlist_h_ */
