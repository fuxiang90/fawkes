#*****************************************************************************
#        Makefile Build System for Fawkes: Laser Hough Transform Plugin
#                            -------------------
#   Created on Mon Dec 28 17:52:11 2009
#   Copyright (C) 2006-2009 by Tim Niemueller, AllemaniACs RoboCup Team
#
#   $Id: Makefile 2627 2009-06-25 18:08:09Z tim $
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

# Uncomment the following to get timing information about the most important
# stages.
#CFLAGS += -DLASERHT_TIMETRACKER

LIBS_laserht = m fawkescore fawkesutils fawkesaspects fawkesblackboard \
	       fawkesinterface \
	       Laser360Interface Laser720Interface ObjectPositionInterface \
	       VisualDisplay2DInterface
OBJS_laserht = $(patsubst %.cpp,%.o,$(patsubst qa/%,,$(subst $(SRCDIR)/,,$(wildcard $(SRCDIR)/*.cpp))))

PLUGINS_all  = $(PLUGINDIR)/laserht.so
OBJS_all     = $(OBJS_laserht)

include $(BUILDSYSDIR)/base.mk
