#*****************************************************************************
#             Makefile Build System for Fawkes : RefBox Repeater
#                            -------------------
#   Created on Wed Apr 09 09:44:15 2008
#   Copyright (C) 2006-2008 by Tim Niemueller, AllemaniACs RoboCup Team
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

LIBS_ffrefboxrep = stdc++ fawkescore fawkesutils fawkesconfig fawkesnetcomm \
		   fawkesblackboard fawkesinterface GameStateInterface
#OBJS_ffrefboxrep = main.o refbox_state_sender.o midsize.o spl.o
OBJS_ffrefboxrep = main.o refbox_state_writer.o refbox_state_sender.o msl2007.o spl.o

OBJS_all =	$(OBJS_ffrefboxrep)
BINS_all =	$(BINDIR)/ffrefboxrep
MANPAGES_all =  $(MANDIR)/man1/ffrefboxrep.1

HAVE_LIBXMLPP    = $(if $(shell $(PKGCONFIG) --exists 'libxml++-2.6'; echo $${?/1/}),1,0)
ifeq ($(HAVE_LIBXMLPP),1)
  CFLAGS  += $(shell $(PKGCONFIG) --cflags libxml++-2.6) -DHAVE_LIBXMLPP -DHAVE_MSL2008 -DHAVE_MSL2010
  LDFLAGS += $(shell $(PKGCONFIG) --libs libxml++-2.6)
  OBJS_ffrefboxrep += msl2008.o msl2010.o
endif
  
include $(BUILDSYSDIR)/base.mk
