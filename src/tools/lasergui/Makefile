#*****************************************************************************
#          Makefile Build System for Fawkes : Laser Visualization
#                            -------------------
#   Created on Thu Oct 09 13:05:34 2008
#   Copyright (C) 2008 by Tim Niemueller, AllemaniACs RoboCup Team
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
include $(BUILDSYSDIR)/gui.mk

CFLAGS  += $(CFLAGS_GUI)
LDFLAGS += $(LDFLAGS_GUI)

LIBS_lasergui = stdc++ m fawkescore fawkesutils fawkesblackboard \
		fawkesinterface fawkesnetcomm fawkesguiutils \
		Position2DTrackInterface Laser360Interface Laser720Interface \
		ObjectPositionInterface SwitchInterface VisualDisplay2DInterface

ifeq ($(BUILD_TYPE),maemo)
  ifeq ($(HAVE_HILDON_GUI),1)
    OBJS_lasergui = lasergui_hildon.o
    LIBS_lasergui += osso
    BUILD_LASERGUI = $(HAVE_GUI)
  else
    WARN_TARGETS += warning_hildonmm
  endif
else
  OBJS_lasergui = lasergui.o
  BUILD_LASERGUI = $(HAVE_GLADE_GUI)
endif
OBJS_lasergui += laser_drawing_area.o visdisplay.o
OBJS_all = $(OBJS_lasergui)

ifeq ($(BUILD_LASERGUI),1)
  BINS_gui = $(BINDIR)/lasergui
  INST_RESDIRS = guis/lasergui
  MANPAGES_all = $(MANDIR)/man1/lasergui.1
else
  WARN_TARGETS += warning_lasergui
endif

ifeq ($(OBJSSUBMAKE),1)
gui: $(WARN_TARGETS)

.PHONY: $(WARN_TARGETS)
warning_lasergui:
	$(SILENT)echo -e "$(INDENT_PRINT)--- $(TRED)Not building lasergui$(TNORMAL) $(GUI_ERROR)";
endif

include $(BUILDSYSDIR)/base.mk

