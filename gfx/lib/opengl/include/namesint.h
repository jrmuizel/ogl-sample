#ifndef __glnamesint_h
#define __glnamesint_h

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
** Display list internal structure description.
**
** $Date$ $Revision$
** $Header: //depot/main/gfx/lib/opengl/include/namesint.h#6 $
*/


/************************************************************************/
/*
** Names Manager Interface
**
** This file contains the Name Space Management types and structures.
**
** The Name Space Management code is used to store and retreive named
** data structures.  The data being stored is referred to with void 
** pointers to allow for the storage of any type of structure.
**
** The Name Space is implemented as a 2-3 tree.  For a detailed
** description of its implementation, see lib/opengl/soft/so_names.c.
**
** __GLnamesArray is declared in types.h.
*/
/************************************************************************/



typedef enum __GLnamesArrayTypeEnum {
    __GL_NAMES_DLIST,
    __GL_NAMES_TEXOBJ
} __GLnamesArrayType;

/*
** The number of spare branches and leaves that we keep about in case
** we run out of memory.  At that point, we complete the current operation
** by using the extra leaves and branches, and we report an OUT_OF_MEMORY
** error when a new operation is requested (unless we can fill our extras
** again!)
**
** These constants were not chosen terribly carefully.  As best as I can
** figure, we only need one spare branch per level in the tree (so 16
** supports a tree with 65536 leaves).  And even then, the user would have
** to be extremely devious to actually force 16 new branches to appear in
** the tree at just the same moment that the system runs out of memory.
**
** The number of spare leaves required, I believe, is one.  Three is chosen
** to allow for some slop.
*/
#define __GL_DL_EXTRA_BRANCHES          16
#define __GL_DL_EXTRA_LEAVES            3

/*
** This is the structure that contains information that is needed
** for each instance of a names tree.  It needs to be public
** so the refcount can be managed.
*/

typedef struct __GLnamesArrayTypeInfoRec __GLnamesArrayTypeInfo;
typedef struct __GLnamesBranchRec __GLnamesBranch;
typedef struct __GLnamesLeafRec __GLnamesLeaf;

struct __GLnamesArrayRec {
    __GLnamesBranch *tree;      /* points to the top of the names tree */
    GLuint depth;               /* depth of tree */
    GLint refcount; /*# ctxs using this array: create with 1, delete at 0*/
    __GLnamesArrayTypeInfo *dataInfo;   /* ptr to data type info */
    GLuint nbranches, nleaves;  /* should basically always be at max */
    __GLnamesBranch *branches[__GL_DL_EXTRA_BRANCHES];
    __GLnamesLeaf *leaves[__GL_DL_EXTRA_LEAVES];
};



/*
** Allocate and initialize a new array structure.
*/
extern __GLnamesArray *__glNamesNewArray(__GLcontext *gc, 
					 __GLnamesArrayType type);

/*
** Free an entire names tree
*/
extern void __glNamesFreeTree(__GLcontext *gc, __GLnamesArray *array,
			      __GLnamesBranch *tree, GLint depth);
/*
** Free the array structure.
*/
extern void __glNamesFreeArray(__GLcontext *gc, __GLnamesArray *array);

/*
** Save a new display list in the array.  A return value of GL_FALSE
** indicates and OUT_OF_MEMORY error, indicating that the list was 
** not stored.
*/
extern GLboolean __glNamesNewData(__GLcontext *gc, __GLnamesArray *array,
				  GLuint name, void *data);

/*
** Find and lock the list specified with "listnum".  A return value of NULL
** indicates that there was no such list.  __glNamesUnlockList() needs to
** be called to unlock the list otherwise.
*/
extern void *__glNamesLockData(__GLcontext *gc, __GLnamesArray *array,
			       GLuint name);

/*
** Unlock a list locked with __glNamesLockList().  If this is not called, then
** any memory associated with the list will never be freed when the list
** is deleted.
*/
extern void __glNamesUnlockData(__GLcontext *gc, void *data);

/*
** Same as __glNamesLockList() except that a bunch of lists are locked and
** returned simultaneously.  Any listbase previously specified is used as 
** an offset to the entries in the array.
*/
extern void __glNamesLockDataList(__GLcontext *gc, __GLnamesArray *array,
				  GLsizei n, GLenum type, GLuint base,
			          const GLvoid *names, void *dataList[]);

/*
** Same as __glNamesUnlockList() except that the entire array of names
** is unlocked at once.
*/
extern void __glNamesUnlockDataList(__GLcontext *gc, GLsizei n,
				    void *dataList[]);

/*
** Generates a list of names.
*/
extern GLuint __glNamesGenRange(__GLcontext *gc, __GLnamesArray *array, 
				GLsizei range);

/*
** Returns GL_TRUE if name has been generated for this array.
*/
extern GLboolean __glNamesIsName(__GLcontext *gc, __GLnamesArray *array, 
				 GLuint name);

/*
** Deletes a range of names.
*/
extern void __glNamesDeleteRange(__GLcontext *gc, __GLnamesArray *array, 
				 GLuint name, GLsizei range);

/*
** Generates a list of (not necessarily contiguous) names.
*/
extern void __glNamesGenNames(__GLcontext *gc, __GLnamesArray *array, 
			      GLsizei n, GLuint* names);

/*
** Deletes a list of (not necessarily contiguous) names.
*/
extern void __glNamesDeleteNames(__GLcontext *gc, __GLnamesArray *array, 
				 GLsizei n, const GLuint* names);

#endif /* __glnamesint_h */
