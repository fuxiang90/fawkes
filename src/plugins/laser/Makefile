#*****************************************************************************
#            Makefile Build System for Fawkes: Laser Plugin
#                            -------------------
#   Created on Wed Oct 08 13:39:39 2008
#   Copyright (C) 2006-2008 by Tim Niemueller, AllemaniACs RoboCup Team
#
#   $Id$
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

LIBS_laser = fawkescore fawkesutils fawkesaspects fawkesinterfaces pcan
OBJS_laser = $(patsubst %.cpp,%.o,$(patsubst qa/%,,$(subst $(SRCDIR)/,,$(wildcard $(SRCDIR)/*.cpp))))

ifeq ($(HAVE_LIBPCAN),1)
  PLUGINS_all = $(PLUGINDIR)/laser.so
else
  WARN_TARGETS += warning_pcan
endif

OBJS_all = $(OBJS_laser)

ifeq ($(OBJSSUBMAKE),1)
all: $(WARN_TARGETS)

.PHONY: warning_pcan
warning_pcan:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting Laser Plugin$(TNORMAL) (Peak Systems' libpcan not found)"
endif

include $(BASEDIR)/etc/buildsys/base.mk