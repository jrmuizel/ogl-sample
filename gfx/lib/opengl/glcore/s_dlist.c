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
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/s_dlist.c#9 $
*/
#include "context.h"
#include "global.h"
#include "g_imports.h"
#include "dlistopt.h"



/* Start of Generic (Default Implementation) Routines */


/* 
** Format of Free Function structures in Local data.
** This is only used by the default implementation.
*/
typedef struct __GLDlistFreeFnsRec {
    void (*freeFn)(__GLcontext *gc, GLubyte *);
    GLubyte *data;
} __GLDlistFreeFns;


void
__glGenericFreeDlist(__GLcontext *gc, __GLdlist *dlist)
{
  GLuint freeCount;
  GLboolean freeSegment = GL_FALSE;
  __GLDlistFreeFns *freeRec = (__GLDlistFreeFns *) NULL;

  freeCount = *(GLuint *)dlist->local;

  if(freeCount) { /* if non-zero, segment isn't empty */
    freeSegment = GL_TRUE;
    freeCount--;
  }

  if(freeCount)
    freeRec = (__GLDlistFreeFns *)((GLuint *)dlist->local + 1);

  /* call any free functions */

  while(freeCount--) {
    (*freeRec->freeFn)(gc, freeRec->data);
    freeRec++;
  }

  /*
  ** Free the segment after calling the free functions in case 
  ** any of them depend on data stored in the segment.
  */
  if(freeSegment) {
      (*gc->dlist.free)(gc, dlist->segment); /* free the segment */
  }

  (*gc->dlist.free)(gc, dlist);
}

#if defined(__GL_USE_ASMCODE) && defined(mips)

extern const GLubyte *__glle_Sentinel(const GLubyte *PC);
extern void __glDoInterpret(GLubyte *PC);

#else

const GLubyte *__glle_Sentinel(const GLubyte *PC)
{
    return NULL;
}

void __glDoInterpret(GLubyte *PC)
{
    __GLlistExecFunc *func = *((__GLlistExecFunc **) PC);
    while (func != __glle_Sentinel) {
	PC = (GLubyte *) (*func)(PC + sizeof(__GLlistExecFunc *));
        func = *((__GLlistExecFunc **) PC);
    }
}

#endif /* __GL_USE_ASMCODE && mips */

/* ARGSUSED */
void
__glGenericExecuteDlist(__GLcontext *gc, __GLdlist *dlist)
{
  __glDoInterpret(dlist->segment);
}

/* this function is defined in dl_table.c */
void 
__glEmptyExecuteDlist(__GLcontext*, __GLdlist*);

__GLdlist *
__glGenericAllocDlist(__GLcontext *gc, GLuint segsize, GLuint freeCount)
{
    __GLdlist *dlist;
    __GLdlist temp;
    size_t memsize;
    GLuint *tmp;

    /*
    ** This is pretty ugly.  It does make this code portable, though.
    */
    memsize = (size_t) (((GLubyte *) temp.local) - ((GLubyte *) &temp) + 
			(sizeof(__GLDlistFreeFns) * freeCount) +
			sizeof(GLuint));
    dlist = (__GLdlist *) (*gc->dlist.malloc)(gc, memsize);
    if (dlist == NULL) return NULL;
    dlist->refcount = 1; /* calling function is 1st reference to dlist */
    dlist->free = __glGenericFreeDlist;
    if(segsize)
      dlist->execute = __glGenericExecuteDlist;
    else /* an empty display list, do nothing when executed */
      dlist->execute = __glEmptyExecuteDlist;

    dlist->segment = (GLubyte *) (*gc->dlist.malloc)(gc, segsize);
    if(segsize) /* count non-empty segment as 1 free function */
      freeCount++;
    tmp = (GLuint *)dlist->local; /* to keep compiler happy */
    *tmp = freeCount;
    return dlist;
}


/*
** Used to pad display list entries to double word boundaries where needed
** (for those few OpenGL commands which take double precision values).
*/
const GLubyte *__glle_Nop(const GLubyte *PC)
{
    return PC;
}

__GLdlist *
__glGenericDlistCompiler(__GLcontext*gc, __GLcompiledDlist *compDlist)
{
  __GLdlistMachine *dlstate;
  __GLdlistOp *dlistop;
  GLubyte *data;
  __GLlistExecFunc *fp;
  __GLDlistFreeFns *freeFnArray;
  GLuint totalSize;
  GLint freeCount;
  __GLdlist *dlist;

  dlstate = &gc->dlist;

  /*
   ** Now we compress the chain of display list ops into an optimized dlist.
   */
  totalSize = 0;
  freeCount = 0;

  for (dlistop = compDlist->dlist; dlistop; dlistop = dlistop->next) {
    if (dlistop->aligned && ((totalSize & 7) != 4)) {
      /* 
      ** Need to stick a nop entry in here to doubleword align data
      ** The function pointer takes up 4 bytes, so if we *aren't*
      ** double word aligned now, the data will be.
      */
      totalSize += sizeof(__GLlistExecFunc *);
    }
    totalSize += dlistop->size + sizeof(__GLlistExecFunc *);
    if (dlistop->dlistFree) freeCount++;
  }
  /* space needed for sentinel function; __glle_Sentinel() */
  totalSize += sizeof(__GLlistExecFunc *); 

  dlistop = compDlist->dlist;

  if (dlistop) {
    dlist = __glGenericAllocDlist(/* context */
				  gc, 
				  /* execution code */
				  totalSize, 
				  /* free function array */
				  freeCount);

    /* local memory used to hold free functions, if any */
    freeFnArray = (__GLDlistFreeFns *) 
      ((GLubyte *)(dlist->local + sizeof(GLuint)));

    if (dlist == NULL) {
      /*
       ** Ack!  No memory!
       */
      (*gc->procs.memory.freeAll)(dlstate->arena);
      compDlist->dlist = NULL;
      compDlist->lastDlist = NULL;
      dlstate->currentList = 0;
      __glSetError(GL_OUT_OF_MEMORY);
      return NULL;
    }

    totalSize = 0;
    data = dlist->segment;
    for (; dlistop; dlistop = dlistop->next) {
      GLshort opcode;

      opcode = dlistop->opcode;

      if (dlistop->aligned && ((totalSize & 7) != 4)) {
	/* Need to stick a nop entry in here to doubleword align */

	*((__GLlistExecFunc **) data) = __glle_Nop;
	data += sizeof(__GLlistExecFunc **);
	totalSize += 4;
      }

      if (opcode < __GL_GENERIC_DLIST_OPCODE) {
	fp = dlstate->baseListExec[opcode];
      } else if (opcode < __GL_MACHINE_DLIST_OPCODE) {
	fp = dlstate->listExec
	  [opcode - __GL_GENERIC_DLIST_OPCODE];
      } else {
	fp = dlstate->machineListExec
	  [opcode - __GL_MACHINE_DLIST_OPCODE];
      }
      *((__GLlistExecFunc **) data) = fp;
      data += sizeof(__GLlistExecFunc **);
      if (dlistop->dlistFree) {
	freeFnArray->freeFn = dlistop->dlistFree;
	freeFnArray->data = data;
	freeFnArray++;
      }

      /*
       ** We don't use memcopy here because most display list entries 
       ** are small, and we cannot afford the function call overhead.
       */
      {   /* __GL_MEMCOPY(data, dlistop->data, dlistop->size); */
	GLuint *src, *dst;
	GLuint a,b,c,d;
	GLint copyWords, copyQuadWords;

	copyWords = (dlistop->size) >> 2;
	copyQuadWords = copyWords >> 2;
	src = (GLuint *) dlistop->data;
	dst = (GLuint *) data;
	while (--copyQuadWords >= 0) {
	  a = src[0];
	  b = src[1];
	  c = src[2];
	  d = src[3];
	  dst[0] = a;
	  dst[1] = b;
	  dst[2] = c;
	  dst[3] = d;
	  src += 4;
	  dst += 4;
	}
	copyWords &= 3;
	while (--copyWords >= 0) {
	  *dst++ = *src++;
	}
      }

      data += dlistop->size;

      totalSize += dlistop->size + sizeof(__GLlistExecFunc *);
    }
    /* append sentinel function to end of list */
    *((__GLlistExecFunc **) data) = __glle_Sentinel;
  } else /* this is an empty display list */
    dlist = __glGenericAllocDlist(gc, 0, 0); /* create empty dlist */


  return dlist;
}


/*
** The following NOP routines are included for documentation purposes.
** They compile, execute, and free a do-nothing display list. Use this
** code as documentation, or as a simple jumping-off point for writing
** your own implementation-specific display list
*/

/* ARGSUSED */
void
__glNopFreeDlist(__GLcontext *gc, __GLdlist *dlist)
{
  /*
  ** free the __GLdlist structure. There are no other allocated structures
  ** pointed to, so only one free is necessary. A more compilcated free
  ** function might be set up to make use of local data appended to the
  ** __GLdlist structure. It would have been put there by the dlist compiler
  ** function.
  */

  (*gc->dlist.free)(gc, dlist);
}

/* ARGSUSED */
void
__glNopExecuteDlist(__GLcontext *gc, __GLdlist *dlist)
{
  /* 
  ** NOP display list does nothing when executed. If it had, it
  ** would have looked at the segment pointer for the display list,
  ** and the data pointer for any local data it needed pertaining
  ** to this display list. An example of local might be pointers to
  ** parent display lists. The format of the local data is determined
  ** by the implementation.
  */
}

__GLdlist *
__glNopAllocDlist(__GLcontext *gc, GLuint size)
{
    __GLdlist *dlist;
    __GLdlist temp;
    size_t memsize;

    /*
    ** This is pretty ugly.  It does make this code portable, though.
    */
    memsize = (size_t) (((GLubyte *) temp.local) - ((GLubyte *) &temp) + size);
    dlist = (__GLdlist *) (*gc->dlist.malloc)(gc, memsize);
    if (dlist == NULL) return NULL;
    dlist->refcount = 1;
    dlist->free = __glNopFreeDlist;
    dlist->execute = __glNopExecuteDlist;
    dlist->segment = NULL;
    return dlist;
}


__GLdlist *
__glNopDlistCompiler(__GLcontext*gc, __GLcompiledDlist *input_list)
{
  __GLdlistMachine *dlstate;
  __GLdlist *compiled_output;

  dlstate = &gc->dlist;

  compiled_output = __glNopAllocDlist(gc, 0);

  if (compiled_output == NULL) {
    /*
     ** Ack!  No memory!
     */
    (*gc->procs.memory.freeAll)(dlstate->arena);
    input_list->dlist = NULL;
    input_list->lastDlist = NULL;
    dlstate->currentList = 0;
    __glSetError(GL_OUT_OF_MEMORY);
  }
  return compiled_output;
}

/* End of Nop routines */
