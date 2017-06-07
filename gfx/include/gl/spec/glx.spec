# License Applicability. Except to the extent portions of this file are
# made subject to an alternative license as permitted in the SGI Free
# Software License B, Version 1.0 (the "License"), the contents of this
# file are subject only to the provisions of the License. You may not use
# this file except in compliance with the License. You may obtain a copy
# of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
# Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
#
# http://oss.sgi.com/projects/FreeB
#
# Note that, as provided in the License, the Software is distributed on an
# "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
# DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
# CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# Original Code. The Original Code is: OpenGL Sample Implementation,
# Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
# Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
# Copyright in any portions created by third parties is as indicated
# elsewhere herein. All Rights Reserved.
#
# Additional Notice Provisions: The application programming interfaces
# established by SGI in conjunction with the Original Code are The
# OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
# April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
# 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
# Window System(R) (Version 1.3), released October 19, 1998. This software
# was created using the OpenGL(R) version 1.2.1 Sample Implementation
# published by SGI, but has not been independently verified as being
# compliant with the OpenGL(R) version 1.2.1 Specification.
#
# $Date$ $Revision$
# $Header$

required-props:
param:		retval retained
dlflags:	notlistable handcode nop
glxflags:	client-intercept client-only client-handcode server-handcode
vectorequiv:	*
category:	pixel-rw bgn-end display-list drawing drawing-control feedback framebuf misc modeling pixel-op pixel-rw state-req xform glx glxopcode
glxopcode:		*
glxvendorglx:		*

Render()
	return		 void
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode server-handcode
	glxopcode	 1


RenderLarge()
	return		 void
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode server-handcode
	glxopcode	 2


CreateContext(gc_id, screen, visual, share_list)
	return		 void
	param		 gc_id		Int32 in value
	param		 screen		Int32 in value
	param		 visual		Int32 in value
	param		 share_list	Int32 in value
	glxflags	 client-handcode
	category	 glx
	dlflags		 notlistable
	glxopcode	 3


DestroyContext(context)
	return		 void
	param		 context       Int32 in value
	glxflags	 client-handcode
	category	 glx
	dlflags		 notlistable
	glxopcode	 4


MakeCurrent(drawable, context)
	return		 void
	param		 drawable	Int32 in value
	param		 context       Int32 in value
	glxflags	 client-handcode
	category	 glx
	dlflags		 notlistable
	glxopcode	 5


IsDirect(dpy, context)
	return		void
	param		dpy		Int32 in value
	param		context		Int32 in value
	glxflags	client-handcode
	category	glx
	dlflags		notlistable
	glxopcode	6


QueryVersion(major, minor)
	return		 void
	param		 major		Int32 out reference
	param		 minor		Int32 out reference
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode server-handcode
	glxopcode	 7


WaitGL(context)
	return		 void
	param		 context	Int32 in value
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode
	glxopcode	 8


WaitX()
	return		 void
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode
	glxopcode	 9


CopyContext(source, dest, mask)
	return		 void
	param		 source		Int32 in value
	param		 dest		Int32 in value
	param		 mask		Int32 in value
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode
	glxopcode	 10


SwapBuffers(drawable)
	return		 void
	param		 drawable	Int32 in value
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode client-intercept
	glxopcode	 11


UseXFont(font, first, count, list_base)
	return		 void
	param		 font		Int32 in value
	param		 first		Int32 in value
	param		 count		Int32 in value
	param		 list_base	Int32 in value
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode client-intercept
	glxopcode	 12


CreateGLXPixmap(visual, pixmap, glxpixmap)
	return		 void
	param		 visual		Int32 in value
	param		 pixmap		Int32 in value
	param		 glxpixmap	Int32 in value
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode client-intercept
	glxopcode	 13

GetVisualConfigs()
	return		 void
	category	 glx
	dlflags		 notlistable
	glxflags	 client-handcode server-handcode
	glxopcode	 14


DestroyGLXPixmap(pixmap)
	return		 void
	param		 pixmap		Int32 in value
	glxflags	 client-handcode
	category	 glx
	dlflags		 notlistable
	glxopcode	 15


VendorPrivate()
	return		void
	glxflags	client-handcode server-handcode
	category	glx
	dlflags		notlistable
	glxopcode	16


VendorPrivateWithReply()
	return		void
	glxflags	client-handcode server-handcode
	category	glx
	dlflags		notlistable
	glxopcode	17

QueryExtensionsString(screen)
	return		void
	param		screen		Int32 in value
	glxflags	client-handcode server-handcode
	category	glx
	dlflags		notlistable
	glxopcode	18

QueryServerString(screen, name)
	return		void
	param		screen		Int32 in value
	param		name		Int32 in value
	glxflags	client-handcode server-handcode
	category	glx
	dlflags		notlistable
	glxopcode	19

ClientInfo()
	return		void
	glxflags	client-handcode server-handcode
	category	glx
	dlflags		notlistable
	glxopcode	20


QueryContextInfoEXT()
	return		 void
	glxflags	 client-handcode server-handcode
	category	 glx
	dlflags		 notlistable
	glxvendorglx	 1024

