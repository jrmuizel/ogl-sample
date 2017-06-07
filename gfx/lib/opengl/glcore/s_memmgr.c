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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_memmgr.c#3 $
*/
#include <stdio.h>
#include "context.h"

/*
** Arena style memory management.
**
** This memory manager works as follows:
**
** procs.memory.newArena is called once to allocate an arena from which memory
**   will be allocated.
** procs.memory.alloc is called many times to allocate blocks of memory from 
**   the arena.
** procs.memory.freeAll is called once to free every block ever allocated from
**   the arena.
** procs.memory.deleteArena is called once to deallocate the arena.
**
** The value to this memory management style is that it can be implemented
** very efficiently.
*/

/*
** Structure for arena style memory management.  Currently this structure
** is defined here because this memory manager fits into one file, and this
** structure should be private.  Later it might make sense to break the
** memory manager into a few files, at which point this structure may need
** to be exposed.
*/

/*
** Quarter megabyte default block size.
*/
#define MINBLOCKSIZE	262144

/*
** An arena is implemented as a linked list of large blocks of memory from
** which smaller blocks are cut.  Each block has a block size, it has an
** amount of the block which has already been allocated, it has a data 
** pointer to the block, and a link to the next block in the linked list.
*/
typedef struct __GLarenaBlockRec {
    unsigned int size;
    unsigned int allocated;
    void *data;
    struct __GLarenaBlockRec *next;
} __GLarenaBlock;

struct __GLarenaRec {
    __GLcontext *gc;
    __GLarenaBlock *firstBlock;
    __GLarenaBlock *lastBlock;
};

static __GLarenaBlock *NewBlock(__GLcontext *gc, unsigned int size)
{
    __GLarenaBlock *block;

    block = (*gc->imports.malloc)(gc, sizeof(__GLarenaBlock));
    if (block == NULL) return NULL;
    block->next = NULL;
    block->size = size;
    block->allocated = 0;
    block->data = (*gc->imports.malloc)(gc, size);
    if (block->data == NULL) {
	(*gc->imports.free)(gc, block);
	return NULL;
    }
    return block;
}

static void DeleteBlock(__GLcontext *gc, __GLarenaBlock *block)
{
    (*gc->imports.free)(gc, block->data);
    (*gc->imports.free)(gc, block);
}

/*
** Allocate a brand new arena.
*/
__GLarena *__glNewArena(__GLcontext *gc)
{
    __GLarena *arena;
    __GLarenaBlock *block;

    arena = (*gc->imports.malloc)(gc, sizeof(__GLarena));
    if (arena == NULL) return NULL;
    arena->gc = gc;
    block = NewBlock(gc, MINBLOCKSIZE);
    if (block == NULL) {
	(*gc->imports.free)(gc, arena);
	return NULL;
    }
    arena->firstBlock = arena->lastBlock = block;
    return arena;
}

/*
** Delete an old arena (and free all memory ever allocated from it).
*/
void __glDeleteArena(__GLarena *arena)
{
    __GLcontext *gc;
    __GLarenaBlock *block, *next;

    gc = arena->gc;
    for (block = arena->firstBlock; block; block=next) {
	next = block->next;
	DeleteBlock(gc, block);
    }
    (*gc->imports.free)(gc, arena);
}

/*
** Allocate block of memory from an arena.  This function needs to be
** as fast as possible.
*/
void *__glArenaAlloc(__GLarena *arena, unsigned int size)
{
    __GLcontext *gc;
    __GLarenaBlock *block, *newblock;
    unsigned int bsize, allocated;

    block = arena->lastBlock;
    bsize = block->size;
    allocated = block->allocated;

    /* increase size to next highest double word aligned value    *
     * to maintain double word alignment. This assumes that the   *
     * base address of each block is at least double word aligned *
     * this will increase blocks allocated, and waste some memory *
     * but it shouldn't cause a memory leak.                      */

    size = size + (8 - (size & 0x7));

    if (size <= (bsize-allocated)) {
	block->allocated = allocated+size;
	return ((char *) block->data) + allocated;
    }

    /*
    ** Need to allocate a new block.
    */

    bsize = size;
    if (bsize < MINBLOCKSIZE) bsize = MINBLOCKSIZE;

    gc = arena->gc;
    newblock = NewBlock(gc, bsize);
    block->next = newblock;
    arena->lastBlock = newblock;
    newblock->allocated = size;
    return newblock->data;
}

/*
** Free all memory ever allocated from this arena.  This function should
** be reasonably fast.
*/
void __glArenaFreeAll(__GLarena *arena)
{
    __GLcontext *gc;
    __GLarenaBlock *block, *next;

    gc = arena->gc;
    block = arena->firstBlock;
    next = block->next;
    block->next = NULL;
    block->allocated = 0;
    arena->lastBlock = block;
    for (block = next; block; block = next) {
	next = block->next;
	DeleteBlock(gc, block);
    }
}
