#*****************************************************************************
#            Makefile Build System for Fawkes : NetLog GUI
#                            -------------------
#   Created on Tue Nov 04 23:35:43 2008
#   Copyright (C) 2008 by Tim Niemueller, AllemaniACs RoboCup Team
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
include $(BASEDIR)/etc/buildsys/gui.mk

CFLAGS  += $(CFLAGS_GTKMM) $(CFLAGS_GLADEMM)
LDFLAGS += $(LDFLAGS_GTKMM) $(LDFLAGS_GLADEMM)

LIBS_netloggui = fawkescore fawkesutils fawkesnetcomm fawkesblackboard fawkesinterfaces fawkesguiutils
OBJS_netloggui = main.o netloggui.o
OBJS_all = $(OBJS_netloggui)

ifeq ($(HAVE_GTKMM)$(HAVE_GLADEMM),11)
  BINS_gui = $(BINDIR)/netloggui
else
  WARN_TARGETS += warning_netloggui
endif

ifeq ($(OBJSSUBMAKE),1)
gui: $(WARN_TARGETS)

.PHONY: $(WARN_TARGETS)
warning_skillgui:
	$(SILENT)echo -e "$(INDENT_PRINT)--- $(TRED)Not building netloggui$(TNORMAL) $(GUI_ERROR)";
endif

include $(BASEDIR)/etc/buildsys/base.mk
