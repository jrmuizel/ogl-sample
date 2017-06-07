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
** $Header: //depot/main/gfx/lib/opengl/glcore/dl_splst.c#5 $
*/

#ifdef NEW_LISTS
#include "dlistint.h"
#include "dlistopt.h"
#include "global.h"
#include "g_imports.h"
#include "context.h"
#include "g_lcomp.h"
#include "g_listop.h"

#include "spdlist.h"

void
__glSPExecuteDlist(__GLcontext *gc, __GLdlist *dlist) {
  extern void __glDoInterpret(GLubyte *PC);
  __glDoInterpret(dlist->segment+sizeof(GLdouble));
}

extern void __glEmptyExecuteDlist(__GLcontext *gc, __GLdlist *dlist);

static GLvoid __glSPDlistFreeSegments(__GLcontext *gc, GLvoid *ptr);

/*XXXblythe this is repeated in so_list.c and needs to be added to an include */
/* 
** Format of Free Function structures in Local data.
** This is only used by the default implementation.
*/
typedef struct __GLDlistFreeFnsRec {
    void (*freeFn)(__GLcontext *gc, GLubyte *);
    GLubyte *data;
} __GLDlistFreeFns;

/*
 * Add a new free function callout onto this display list
 */
void
__glSPDlistAddFree(__GLcontext *gc, void (*freeFn)(__GLcontext *gc, GLubyte *), GLubyte *data) {
    __GLdlistMachine *dlist = &gc->dlist;
    __GLDlistFreeFns *freeLst = (__GLDlistFreeFns *)dlist->spFreeLst;
    GLint freeCnt = dlist->spFreeCnt;
    freeLst = dlist->realloc(gc, freeLst, (freeCnt+1)*sizeof(__GLDlistFreeFns));
    if (!freeLst) return;
    freeLst[freeCnt].freeFn = freeFn;
    freeLst[freeCnt].data = data;
    dlist->spFreeLst = (void *)freeLst; /*XXXblythe*/
    dlist->spFreeCnt = freeCnt+1;
}

void
__glSPFreeDlist(__GLcontext *gc, __GLdlist *dlist) {
  GLuint freeCount;
  __GLDlistFreeFns *freeRec;

  freeCount = *(GLuint *)dlist->local;

  if(freeCount) { /* if non-zero, segment isn't empty */
    __glSPDlistFreeSegments(gc, dlist->segment);
    freeCount--;
  }

  if(freeCount)
    freeRec = (__GLDlistFreeFns *)((GLuint *)dlist->local + 1);

  /* call any free functions */

  while(freeCount--) {
    (*freeRec->freeFn)(gc, freeRec->data);
    freeRec++;
  }
  (*gc->dlist.free)(gc, dlist);
}

__GLdlist *__glSPDlistCompiler(__GLcontext*gc, __GLcompiledDlist *compDlist) {
    __GLdlist *dlist;
    __GLdlist temp;
    size_t memsize;
    GLint i;
    GLuint segsize = __gl_spdl->start != 0;
    GLint freeCount = gc->dlist.spFreeCnt;
    __GLDlistFreeFns *freeList, *ff;

    /*
    ** This is pretty ugly.  It does make this code portable, though.
    */
    memsize = (size_t) (((GLubyte *) temp.local) - ((GLubyte *) &temp))
	+ sizeof(GLuint) + gc->dlist.spFreeCnt * sizeof(__GLDlistFreeFns);
    /*XXXblythe almost guaranteed that freeList isn't double word aligned */

    dlist = (__GLdlist *) (*gc->dlist.malloc)(gc, memsize);
    if (dlist == NULL) return NULL;
    dlist->refcount = 1;
    dlist->free = __glSPFreeDlist; /*__glGenericFreeDlist;*/
    if(segsize) {
      GLvoid __glsplc_AddSentinel(GLvoid);
      __glsplc_AddSentinel();
      dlist->execute = __glSPExecuteDlist;
    } else /* an empty display list, do nothing when executed */
      dlist->execute = __glEmptyExecuteDlist;

    dlist->segment = (GLubyte *) compDlist->dlist; /*(*gc->dlist.malloc)(gc, segsize);*/
    if(segsize) /* count non-empty segment as 1 free function */
	freeCount++;
    /* !segsize => freeCount == 0 */
    assert(segsize || freeCount == 0);
    *(GLint *)dlist->local = freeCount;
    freeList = (__GLDlistFreeFns *)((GLuint *)dlist->local+1);
    ff = (__GLDlistFreeFns *)gc->dlist.spFreeLst;
    for(i = 0; i < freeCount-1; i++)
	freeList[i] = ff[i];
    if (freeCount > 1)
	gc->dlist.free(gc, gc->dlist.spFreeLst);
    return dlist;
}

typedef struct dlchunkh {	/* chunk head */
    GLvoid *next;	/* pointer to next chunk */
    GLint  align;	/* force doubleword alignment for data */
    GLuint data[1];	/* variable length block of data */
} dlchunkh_t;

typedef struct dlchunkt {	/* chunk tail */
    GLvoid *jmpf;	/* pointer block jumping function */
    GLvoid *next;	/* pointer to first function in next block */
} dlchunkt_t;

typedef struct dlpushblk {
    GLvoid *interp;	/* interpreter function */
    GLint count;	/* number of 32-bit words of data */
    GLuint data[1];	/* variable block of data */
} dlpushblk_t;

typedef struct dlchunkh_pushblk {
    GLvoid *next;	/* pointer to next chunk */
    GLint  align;	/* force doubleword alignment for data */
    GLvoid *interp;	/* interpreter function */
    GLint count;	/* number of 32-bit words of data */
    GLuint data[1];	/* variable length block of data */
} dlchunkh_pushblk_t;

/* fcn to jump to next chunk */
GLubyte *__glsple_Jmp(const GLvoid *PC) {
    dlchunkt_t *p = (dlchunkt_t *)PC;
    /* we already advanced the PC, so we return jmpf rather than next */
    return p->jmpf;
}


#define __GLCHUNKSIZE	1024

GLuint
__glSPDlistSpace(GLuint nopush) {
    dlchunkh_t *ptr = (dlchunkh_t *)__gl_spdl->start;
    GLuint amount = __GLCHUNKSIZE;
    __GL_SETUP();

    /*
     * heavily overloaded use of nopush, its both a flag and a
     * space request for really large requests
     */
    if (nopush > 1) {
	amount = nopush;
	/* add in amount we need for bookkeeping */
	amount += (sizeof(dlchunkh_t) + sizeof(dlchunkt_t) - sizeof(ptr->data)) >> 2;
    }
    /* allocate more space */
    __gl_spdl->start = (*gc->imports.malloc)(gc, amount*sizeof(GLuint));

    if (ptr) {
	dlchunkt_t *tail;

	ptr->next = __gl_spdl->start;

	if (nopush)
	    /* not in a push state, so ptr is right at end */
	    tail = (dlchunkt_t *)__gl_spdl->ptr;
	else if (!__gl_spdl->pcnt)
	    /* if haven't stored any pushmodel stuff, roll back */
	    tail = (dlchunkt_t *)((GLubyte *)__gl_spdl->ptr-(sizeof(dlpushblk_t)-sizeof ptr->data));
	else {
	    /* seal the push block */
	    GLubyte *p = __gl_spdl->ptr;
	    dlpushblk_t *pb = (dlpushblk_t *)(p - (__gl_spdl->pcnt*sizeof(GLuint) + sizeof(dlpushblk_t)-sizeof ptr->data));
#ifdef bit64
	    /* fix alignment */
	    tail = (dlchunkt_t *)(((long p)+4)&~4);
#else
	    tail = (dlchunkt_t *)p;
#endif
	    /* XXXblythe optimize for alignment */
	    pb->interp = gc->dlist.spPushExec;
	    pb->count = __gl_spdl->pcnt;
	}
	/* set bridge function */
	tail->jmpf = __glsple_Jmp;
	tail->next = &((dlchunkh_t *)__gl_spdl->start)->data;
    } else {
	/* on first allocation, need to set a pointer in the header block */
	gc->dlist.listData.dlist = __gl_spdl->start;
    }

    /* initialize link to 0 */
    ptr = (dlchunkh_t *)__gl_spdl->start;
    ptr->next = 0;

    __gl_spdl->pcnt = 0;
    if (nopush) {
	/* 1 word for cnt, 2 words for jmp */
	__gl_spdl->ptr = &((dlchunkh_t *)__gl_spdl->start)->data; /* skip first word */
	return __gl_spdl->size = __GLCHUNKSIZE -
		((sizeof(dlchunkh_t) + sizeof(dlchunkt_t) - sizeof(ptr->data))>>2);
    } else {
	/* initialize for push model */
	/* 1 word for cnt, 2 words for push preamble, 2 words for jmp */
	__gl_spdl->ptr = &((dlchunkh_pushblk_t *)__gl_spdl->start)->data; /* skip first 3 words */
	return __gl_spdl->size = __GLCHUNKSIZE -
		((sizeof(dlchunkh_pushblk_t) + sizeof(dlchunkt_t) - sizeof(ptr->data))>>2);
    }
}

static GLvoid
__glSPDlistFreeSegments(__GLcontext *gc, GLvoid *ptr) {
    dlchunkh_t *tmp;

    do {
	tmp = ((dlchunkh_t *)ptr)->next;
	(*gc->dlist.free)(gc, ptr);
	ptr = tmp;
    } while(ptr);
}
#endif /* NEW_LISTS */
