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
** Display list init/destroy code.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/glcore/dl_block.c#8 $
*/
/*
 * dl_block.c -- simple block allocator
 */

#ifdef NEW_LISTS
#include <malloc.h>
#include <string.h>
#include "global.h"
#include "dlistint.h"
#include "dl_block.h"

static int block_grow_region(unsigned int new_region_size, 
			     __GLdlistBlockState *b);

#ifndef NDEBUG
static int block_is_free(__GLdlistBlockState *b, void *ptr);
static void block_sanity(__GLdlistBlockState *b);
#endif

#ifdef NDEBUG
#define BLOCK_SANITY(b) 
#else
#define BLOCK_SANITY(b)	block_sanity(b)
#endif

#define RETURN_UNLOCK(x) {__GL_DLIST_SEMAPHORE_UNLOCK(); return (x);}

int __glDlistBlockStateInit(__GLcontext *gc, unsigned int block_size, 
			    void *(*sbrk)(int, void *), 
			    void *(*memmove)(void *, const void *, size_t,
					     void *),
			    void *sbrk_state,
			    __GLdlistBlockState *b)
{
    __GL_DLIST_SEMAPHORE_LOCK();

#ifdef DLIST_DEBUG
    (*gc->imports.warning)(gc, "In BlockStateInit");
#endif

    assert(block_size);
    assert(sbrk);
    assert(memmove);
    assert(sbrk_state);
    assert(b);

    b->block_size = block_size;
    b->sbrk = sbrk;
    b->memmove = memmove;
    b->sbrk_state = sbrk_state;

    /* can't call sbrk here since it requires a graphics context which 
     * might not exist yet */
    b->region_addr = (void *)-1L;

    b->region_size = 0;
    /* if b->memmap is 0 it can confuse the compaction routine which calls 
     * realloc */
    b->memmap = (*gc->imports.malloc)(gc, sizeof b->memmap[0]);
    if (!b->memmap) RETURN_UNLOCK(0)
    b->memmap[0] = 0;
    
    b->blocks_free = 0;
    b->last_free = (void *)-1L;

    b->transmap = 0;
    b->blocks_transient = 0;
BLOCK_SANITY(b);

#ifdef DLIST_DEBUG
    (*gc->imports.warning)(gc, "Done BlockStateInit");
#endif

    RETURN_UNLOCK(1)
}

int __glDlistBlockStateFree(__GLcontext *gc, __GLdlistBlockState *b)
{
    __GL_DLIST_SEMAPHORE_LOCK();

    assert(b);
    BLOCK_SANITY(b);

    if ((*b->sbrk)(-b->region_size * b->block_size, b->sbrk_state) == 
	(void *)(long)-1) RETURN_UNLOCK(0)
    (*gc->imports.free)(gc, b->memmap);
    (*gc->imports.free)(gc, b->transmap);
    
#ifndef NDEBUG
    /* make it obvious that this has been freed */
    memset((void *)b, 0xab, sizeof(__GLdlistBlockState));
#endif

    RETURN_UNLOCK(1)

}

#define MAP_FLOOR(x)	((x) >> 5)
#define MAP_SHIFT(x)	((x) << 5)
#define MAP_MOD(x)	((x) & 31)
#define MAP_ROUND(x)	(((x)+31)>>5)

static int block_grow_region(unsigned int new_region_size, 
			     __GLdlistBlockState *b)
{
    int i;
    void *addr;
    __GL_SETUP();

    BLOCK_SANITY(b);

#ifdef DLIST_DEBUG
    {
	char str[200];

	(*gc->imports.sprintf)(gc, str,
			       "Growning region to %d blocks.", 
			       new_region_size);
	(*gc->imports.warning)(gc, str);
    }
#endif

    assert(new_region_size);
    assert(new_region_size >= b->region_size);

    /* do the allocations */
    if ((addr = (*b->sbrk)((new_region_size - b->region_size) * b->block_size, 
		   b->sbrk_state)) == (void *)(long)-1) return 0;
    if (!(b->memmap = (*gc->imports.realloc)(gc, b->memmap, MAP_ROUND(new_region_size)*sizeof(int))))
	return 0;
    if (!(b->transmap = (*gc->imports.realloc)(gc, b->transmap, MAP_ROUND(new_region_size)*sizeof(int))))
	return 0;

    for (i = MAP_ROUND(b->region_size); i < MAP_ROUND(new_region_size); i++) 
	b->memmap[i] = b->transmap[i] = 0;
    
    b->blocks_free += new_region_size - b->region_size;
    b->region_size = new_region_size;
#if 1
    if (b->region_addr == (char *)-1L) b->region_addr = addr;
#endif
    BLOCK_SANITY(b);
    return 1;
}

int __glDlistPreallocBlocks(__GLdlistBlockState *b, int n)
{
    __GL_DLIST_SEMAPHORE_LOCK();

    if (n > b->blocks_free) {
	RETURN_UNLOCK(block_grow_region(b->region_size+n-b->blocks_free, b))
    } else {
	RETURN_UNLOCK(1)
    }
}

void *__glDlistAllocBlock(__GLdlistBlockState *b) 
{
    int i, j;
    unsigned int mask;
    void *block;

    __GL_DLIST_SEMAPHORE_LOCK();

    assert(b);

    if (b->region_addr == (char *)-1L) {
	b->region_addr = (char *)(*b->sbrk)(0, b->sbrk_state);
	if (b->region_addr == (char *)(long)-1) RETURN_UNLOCK((char *)(long)-1)
    }

    if (!b->blocks_free)
	if (!block_grow_region(b->region_size+32, b)) RETURN_UNLOCK((char *)(long)-1)

    for (j = 0; j < MAP_FLOOR(b->region_size) && b->memmap[j] == 0xffffffff; j++);
    i = MAP_SHIFT(j);
    mask = 1;
    while (b->memmap[j] & mask) {mask <<= 1; i++;}
    assert(i < b->region_size);
    assert(mask);
    assert((b->memmap[j] & mask) == 0);

    block = b->region_addr + b->block_size*i;
    assert(block_is_free(b, block));
    b->memmap[j] |= mask;
    b->blocks_free--;
    assert(!block_is_free(b, block));
    
    BLOCK_SANITY(b);

#ifdef DLIST_DEBUG
    {
	char str[200];

	(*gc->imports.sprintf)(gc, str,
			       "Done AllocBlock - returning 0x%x", (int)block);
	(*gc->imports.warning)(gc, str);
    }
#endif

    RETURN_UNLOCK(block)
}

void __glDlistFreeBlock(__GLdlistBlockState *b, void *ptr)
{
    int block;
    int i, shift;

    __GL_DLIST_SEMAPHORE_LOCK();

#ifdef DLIST_DEBUG
    {
	char str[200];

	(*gc->imports.sprintf)(gc, str, "In FreeBlock -- freeing 0x%x",
			       (int)ptr);
	(*gc->imports.warning)(gc, str);
    }
#endif

    if (b->last_free != (void *)-1L) {
	block = (int)((char *)b->last_free - b->region_addr);
	block /= b->block_size;
	i = block >> 5;
	shift = block & 31;
    
	assert(b->memmap[i] & (1 << shift));
	b->memmap[i] &= ~(1 << shift);
	b->blocks_free++;
    
	assert(block_is_free(b, b->last_free));
    }
    b->last_free = ptr;

    BLOCK_SANITY(b);

#ifdef DLIST_DEBUG
    {
	char str[200];

	(*gc->imports.warning)(gc, "Done FreeBlock");
    }
#endif

    __GL_DLIST_SEMAPHORE_UNLOCK();
}

int __glDlistCompactBlocks(__GLdlistBlockState *b)
{
    int i, j;
    __GL_SETUP();

    __GL_DLIST_SEMAPHORE_LOCK();

    j = 0;
    for (i = 0; i < b->region_size; i++) {
	if (b->memmap[i/32] & (1 << (i%32))) {
	    for (; j < i && b->memmap[j/32] & (1 << (j%32)); j++);
	    if (j == i) continue;
	    assert(block_is_free(b, b->region_addr + b->block_size*j));
	    assert(!block_is_free(b, b->region_addr + b->block_size*i));
	    b->memmap[i/32] &= ~(1 << (i%32));
	    b->memmap[j/32] |= 1 << (j%32);
	    (*b->memmove)(b->region_addr + b->block_size * j,
			  b->region_addr + b->block_size * i,
			  b->block_size, b->sbrk_state);
	    assert(!block_is_free(b, b->region_addr + b->block_size*j));
	    assert(block_is_free(b, b->region_addr + b->block_size*i));
	}
    }
    BLOCK_SANITY(b);

    if ((*b->sbrk)(-b->blocks_free * b->block_size, b->sbrk_state) == 
	(void *)(long)-1) 
      RETURN_UNLOCK(0)

    b->region_size -= b->blocks_free;
    b->blocks_free = 0;

    b->memmap = (*gc->imports.realloc)(gc, b->memmap, (b->region_size / 32 + 1) * sizeof(int));
    if (!b->memmap) RETURN_UNLOCK(0)

    BLOCK_SANITY(b);

    RETURN_UNLOCK(1)
}

int __glDlistIsBlock(__GLdlistBlockState *b, void *p)
{
    int ret_val;

    __GL_DLIST_SEMAPHORE_LOCK();
    
    BLOCK_SANITY(b);
    ret_val = ((long)p >= (long)b->region_addr && 
	       (long)p < (long)b->region_addr + b->block_size*b->region_size);
    RETURN_UNLOCK(ret_val)
	
}

#ifndef NDEBUG 

int block_is_free(__GLdlistBlockState *b, void *ptr)
{
    int i = (int)((char *)ptr - b->region_addr) / b->block_size;;
    assert(i >= 0);
    assert(i < b->region_size);
    return ((b->memmap[i>>5] & (1 << (i & 31))) == 0);
}

void block_sanity(__GLdlistBlockState *b)
{
    void *sbrk_val;

    unsigned int free;
    int i;
    unsigned int mask;

    assert(b->block_size);

    assert(b->sbrk);
    assert(b->memmove);
    assert(b->sbrk_state);

    assert(b->blocks_free <= b->region_size);

    if (b->region_addr != (char *)-1L) {
	sbrk_val = (*b->sbrk)(0, b->sbrk_state);
	assert(sbrk_val == b->region_addr + b->block_size*b->region_size);
    }

    assert(b->memmap);

    free = 0;
    for (i = 0; i < MAP_ROUND(b->region_size); i++) {
	int limit = 32;
	if (i == MAP_ROUND(b->region_size) - 1 && MAP_MOD(b->region_size))
	    limit = MAP_MOD(b->region_size);
	for (mask = 0; mask < limit; mask++)
	    if (!(b->memmap[i] & (1 << mask))) free++;
    } 

    assert(free == b->blocks_free);
}
#endif

void
__glDlistTransientBlock(__GLdlistBlockState *b, void *ptr) {
    int block;
    int i, shift;

    __GL_DLIST_SEMAPHORE_LOCK();

    block = (int)((char *)ptr - b->region_addr);
    block /= b->block_size;
    i = block >> 5;
    shift = block & 31;

    assert(!(b->transmap[i] & (1 << shift)));
    b->transmap[i] |= 1 << shift;
    b->blocks_transient++;

    BLOCK_SANITY(b);
    __GL_DLIST_SEMAPHORE_UNLOCK();
}

void
__glDlistFreeTransient(__GLdlistBlockState *b) {
    int i;
    for(i = 0; i < MAP_ROUND(b->region_size); i++) {
	b->memmap[i] &= ~b->transmap[i];
	b->transmap[i] = 0;
    }
    b->blocks_free += b->blocks_transient;
    b->blocks_transient = 0;
}

#endif /* NEWLISTS */
