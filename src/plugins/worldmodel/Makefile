#*****************************************************************************
#              Makefile Build System for Fawkes: WorldModel Plugin
#                            -------------------
#   Created on Fir Jun 29 11:53:13 2007 (on flight to RoboCup 2007, Atlanta)
#   copyright (C) 2006-2007 by Tim Niemueller, AllemaniACs RoboCup Team
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

LIBS_worldmodel = m fawkescore fawkesutils fawkesaspects fawkesgeometry \
		  fawkesblackboard fawkesinterface fawkesnetcomm \
		  GameStateInterface ObjectPositionInterface
OBJS_worldmodel = $(patsubst %.cpp,%.o,$(patsubst qa/%,,$(subst $(SRCDIR)/,,$(realpath $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp)))))

OBJS_all    = $(OBJS_worldmodel)
PLUGINS_all = $(PLUGINDIR)/worldmodel.so

include $(BUILDSYSDIR)/base.mk

