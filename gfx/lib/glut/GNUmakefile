#!gmake
#
# $Header: //depot/main/gfx/lib/glut/GNUmakefile#8 $

include $(ROOT)/usr/include/make/commondefs

INSTALLFLAGS += -srcdst

TARGET = libglut.a

TARGETS = $(TARGET)

CFILES = \
	glut_8x13.c \
	glut_9x15.c \
	glut_bitmap.c \
	glut_bwidth.c \
	glut_cindex.c \
	glut_cursor.c \
	glut_dials.c \
	glut_dstr.c \
	glut_event.c \
	glut_ext.c \
	glut_fullscrn.c \
	glut_get.c \
	glut_hel10.c \
	glut_hel12.c \
	glut_hel18.c \
	glut_init.c \
	glut_input.c \
	glut_menu.c \
	glut_mesa.c \
	glut_modifier.c \
	glut_mroman.c \
	glut_overlay.c \
	glut_roman.c \
	glut_shapes.c \
	glut_space.c \
	glut_stroke.c \
	glut_swidth.c \
	glut_tablet.c \
	glut_teapot.c \
	glut_tr10.c \
	glut_tr24.c \
	glut_util.c \
	glut_vidresize.c \
	glut_warp.c \
	glut_win.c \
	glut_winmisc.c \
	layerutil.c \
	$(NULL)

default libs: $(TARGET)

headers headers_install apps:

libs_install install: default
	$(INSTALL) -F $(ROOT)/usr/include/GL glut.h;
	$(INSTALL) -F $(LIB_LOCATION) $(TARGET);

$(TARGET): $(OBJECTS)
	$(AR) crl $@ $(OBJECTS);

include $(COMMONRULES)
