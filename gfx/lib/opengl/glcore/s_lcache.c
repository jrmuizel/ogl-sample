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
** $Header: //depot/main/gfx/lib/opengl/glcore/s_lcache.c#5 $
*/
#include <stdio.h>
#include "context.h"
#include "global.h"
#include "g_imports.h"

/*
** A simple few routines which saves a cache for specular and spotlight
** computation (rather than recomputing the tables every time the user
** changes the specular or spotlight exponents).
*/

/*
** Any more than TOO_MANY_LUT_ENTRIES entries, and we free any that 
** become unreferenced.
*/
#define TOO_MANY_LUT_ENTRIES	32

typedef struct {
    __GLfloat exp;
    __GLspecLUTEntry *table;
} __GLspecLUTEntryPtr;

typedef struct __GLspecLUTCache_Rec {
    GLint nentries;
    GLint allocatedSize;
    __GLspecLUTEntryPtr entries[1];
} __GLspecLUTCache;

void __glInitLUTCache(__GLcontext *gc)
{
    __GLspecLUTCache *lutCache;

    lutCache = gc->light.lutCache = (__GLspecLUTCache *) 
	    (*gc->imports.malloc)(gc, sizeof(__GLspecLUTCache));
    lutCache->nentries = 0;
    lutCache->allocatedSize = 1;
}

void __glFreeLUTCache(__GLcontext *gc)
{
    int i;
    GLint nentries;
    __GLspecLUTEntryPtr *entry;
    __GLspecLUTCache *lutCache;

    lutCache = gc->light.lutCache;
    nentries = lutCache->nentries;
    for (i = 0; i < nentries; i++) {
	entry = &(lutCache->entries[i]);
	entry->table->refcount--; /* cache no longer refers to table */
	if(entry->table->refcount == 0) { /* other gc's (through dlists) may */
	  (*gc->imports.free)(gc, entry->table);     /* reference this table */
	}
    }
    (*gc->imports.free)(gc, lutCache);
}

static __GLspecLUTEntry *findEntry(__GLspecLUTCache *lutCache, __GLfloat exp, 
				   GLint *location)
{
    GLint nentries;
    GLint bottom, half, top;
    __GLspecLUTEntry *table;
    __GLspecLUTEntryPtr *entry;

    nentries = lutCache->nentries;
    /* First attempt to find this entry in our cache */
    bottom = 0;
    top = nentries;
    while (top > bottom) {
	/* Entry might exist in the range [bottom, top-1] */
	half = (bottom+top)/2;
	entry = &(lutCache->entries[half]);
	if (entry->exp == exp) {
	    /* Found the table already cached! */
	    table = entry->table;
	    *location = half;
	    return table;
	}
	if (exp < entry->exp) {
	    /* exp might exist somewhere earlier in the table */
	    top = half;
	} else /* exp > entry->exp */ {
	    /* exp might exist somewhere later in the table */
	    bottom = half+1;
	}
    }
    *location = bottom;
    return NULL;
}

__GLspecLUTEntry *__glCreateSpecLUT(__GLcontext *gc, __GLfloat exp)
{
    GLint nentries, allocatedSize;
    GLint location;
    __GLspecLUTCache *lutCache;
    __GLspecLUTEntryPtr *entry;
    __GLspecLUTEntry *table;
    __GLfloat *tableEntry;
    GLdouble threshold, scale, x, dx;
    GLint i;

    lutCache = gc->light.lutCache;

    table = findEntry(lutCache, exp, &location);
    if (table) {
	table->refcount++;
	return table;
    }
    /* 
    ** We failed to find our entry in our cache anywhere, and have to compute 
    ** it.
    */

    nentries = lutCache->nentries;
    if (nentries >= TOO_MANY_LUT_ENTRIES) {
        /* 
	** Free first unreferenced entry found (if any); i.e., look
	** for tables with a reference count of 1 (0 shouldn't happen,
	** since the cache itself counts as a reference). A reference
	** count of 1 of a table found in the cache means that only the
	** cache is referring to this entry.
	**
	** We like to leave them around in case they are needed in the
	** future, but if the cache gets big, we free them up.
	**
	** This requires reducing lutCache->nentries by one, and copying 
	** entries [location+1, nentries] to [location, nentries-1].
	*/

        for(i = 0; i < nentries; i++) {
	    table = (lutCache->entries[i]).table;
	    assert(table->refcount != 0); /* refcount 0 shouldn't be in cache */
	    if (table->refcount == 1) { /* a table only referred by cache */
		table->refcount--;
		lutCache->nentries = nentries = lutCache->nentries - 1;
		__GL_MEMMOVE(&(lutCache->entries[i]),
			 &(lutCache->entries[i+1]),
			 (nentries-i) * sizeof(__GLspecLUTEntryPtr));
		(*gc->imports.free)(gc, table); /* free the entry */
		break; /* break out of loop */
	    }
	}

	/*
	** We "refind" the entry because we just changed the size of the 
	** table, and we want "location" to contain something meaningful.
	*/
	table = findEntry(lutCache, exp, &location);

	/* Still shouldn't have found it */
	assert (table == NULL);

    }
    
    lutCache->nentries = nentries = 1 + lutCache->nentries;
    allocatedSize = lutCache->allocatedSize;

    if (nentries > allocatedSize) {
	/* Allocate space for another six entries (arbitrarily) */
	lutCache->allocatedSize = allocatedSize = allocatedSize + 6;
	gc->light.lutCache = lutCache = (__GLspecLUTCache *)
		(*gc->imports.realloc)(gc, lutCache, sizeof(__GLspecLUTCache) +
		allocatedSize * sizeof(__GLspecLUTEntryPtr));
    }

    /*
    ** We have enough space now.  So we stick the new entry in the array
    ** at entry 'location'.  The rest of the entries need to be moved up
    ** (move [location, nentries-2] up to [location+1, nentries-1]).
    */
    if (nentries-location-1) {
	assert(nentries > location);
	__GL_MEMMOVE(&(lutCache->entries[location+1]), 
		&(lutCache->entries[location]),
		(nentries-location-1) * sizeof(__GLspecLUTEntryPtr));
    }
    entry = &(lutCache->entries[location]);
    entry->exp = exp;
    table = entry->table = (__GLspecLUTEntry *) 
	    (*gc->imports.malloc)(gc, sizeof(__GLspecLUTEntry));

    /* Compute threshold */
    if (exp == 0.0) {
	threshold = 0.0;
    } else {
	threshold = __GL_POWF(0.0007, 1.0 / exp);
    }

    scale = (__GL_SPEC_LOOKUP_TABLE_SIZE - 1) / (1.0 - threshold);
    dx = 1.0 / scale;
    x = threshold;
    tableEntry = table->table;
    for (i = __GL_SPEC_LOOKUP_TABLE_SIZE; --i >= 0; ) {
	*tableEntry++ = __GL_POWF(x, exp);
	x += dx;
    }
    table->threshold = threshold;
    table->scale = scale;
    table->refcount = 2; /* both cache and calling code refer to table */
    table->exp = exp;
    return table;
}

void __glFreeSpecLUT(__GLcontext *gc, __GLspecLUTEntry *lut)
{
    if (lut == NULL) return;

    assert(lut->refcount != 0);

    lut->refcount--;

    if (lut->refcount > 0) return; /* someone is referring to table entry */

    /* refcount == 0, no-one has a pointer to this table, not even a
    ** specular cache.
    **
    ** NOTE: I made the context pointer NULL to make it clear that
    ** the context being passed may not be the context that was
    ** current when the space was allocated. In fact the context
    ** may have been destroyed between the malloc and the free.
    ** Currently, the argument isn't used, but this should be fixed,
    ** or at least the intended use of this argument should be documented.
    */
    (*gc->imports.free)((__GLcontext *)NULL, lut); 
}

