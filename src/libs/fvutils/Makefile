#*****************************************************************************
#            Makefile Build System for Fawkes : FireVision Utils
#                            -------------------
#   Created on Sun Feb 04 22:32:29 2007
#   Copyright (C) 2006-2007 by Tim Niemueller, AllemaniACs RoboCup Team
#
#*****************************************************************************
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#*****************************************************************************

BASEDIR = ../../..

include $(BASEDIR)/etc/buildsys/config.mk
include $(BUILDSYSDIR)/fvconf.mk

CFLAGS   += $(VISION_CFLAGS)
LDFLAGS  += $(VISION_LDFLAGS)
INCDIRS  += $(VISION_INCDIRS)
LIBDIRS  += $(VISION_LIBDIRS)
LIBS     += $(VISION_LIBS)

ifneq ($(HAVE_LIBJPEG),1)
  WARN_TARGETS += warning_libjpeg
  FILTER_OUT   += readers/jpeg.o writers/jpeg.o compression/jpeg_compressor.o \
		compression/jpeg_decompressor.o
else
  UTILS_EXTRA_LIBS += jpeg
endif

ifneq ($(HAVE_LIBPNG),1)
  WARN_TARGETS += warning_libpng
  FILTER_OUT   += readers/png.o writers/png.o
else
  CFLAGS += $(CFLAGS_LIBPNG)
  LDFLAGS += $(LDFLAGS_LIBPNG)
endif

ifneq ($(HAVE_OPENCV),1)
WARN_TARGETS += warning_opencv
FILTER_OUT   += recognition/faces.o $(patsubst %.cpp,%.o,$(subst $(SRCDIR)/,,$(wildcard $(SRCDIR)/recognition/forest/*.cpp)))
FILTER_OUT   += adapters/iplimage.o
else
CFLAGS  += $(CFLAGS_OPENCV)
LDFLAGS += $(LDFLAGS_OPENCV)
  ifneq ($(wildcard $(LIBSRCDIR)/fvutils/recognition/forest/forest.h),)
    LDFLAGS += $(LDFLAGS_OPENCV_GUI)
  endif
endif

ifeq ($(OBJSSUBMAKE),1)
all: $(WARN_TARGETS)
warning_libjpeg:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting JPEG support$(TNORMAL) (No libjpeg[-devel] installed)"
warning_libpng:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting PNG support$(TNORMAL) (No libpng[-devel] installed)"
warning_opencv:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting OpenCV dependent stuff in the fvutils$(TNORMAL) (No opencv[-devel] installed)"
endif

# We are lazy in the utils...
OBJS_libfvutils := $(filter-out $(FILTER_OUT),$(patsubst %.cpp,%.o,$(patsubst qa/%,,$(subst $(SRCDIR)/,,$(realpath $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp $(SRCDIR)/*/*/*.cpp))))))
LIBS_libfvutils := m fawkescore fawkesutils fawkesnetcomm $(UTILS_EXTRA_LIBS)
HDRS_libfvutils := $(filter-out $(patsubst %.o,%.h,$(FILTER_OUT)),$(patsubst qa/%,,$(subst $(SRCDIR)/,,$(wildcard $(SRCDIR)/*.h $(SRCDIR)/*/*.h $(SRCDIR)/*/*/*.h))))

OBJS_all = $(OBJS_libfvutils)
LIBS_all = $(LIBDIR)/libfvutils.so

include $(BUILDSYSDIR)/base.mk

