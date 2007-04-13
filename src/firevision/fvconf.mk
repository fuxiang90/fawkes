#*****************************************************************************
#            Makefile Build System for Fawkes: FireVision Config
#                            -------------------
#   Created on Sun Jan 14 23:00:47 2007
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
#
#           $Id$
# last modified: $Date$
#            by: $Author$
#
#*****************************************************************************

CAMS=LEUTRON FIREWIRE FILELOADER NETWORK SHMEM V4L

VISION_INCDIRS      = $(realpath $(BASEDIR)/src/firevision)

ifneq ($(realpath /usr/include/lv),)
HAVE_LEUTRON_CAM    = 1
HAVE_VISCA_CTRL     = 1
VISION_LIBDIRs     += /usr/lib/lv
VISION_INCDIRS     += /usr/include/lv
VISION_CAM_LIBS    += lvsds.3
endif
ifeq ($(HAVE_VISCA_CTRL),1)
HAVE_EVID100P_CTRL  = 1
endif

ifneq ($(realpath /usr/include/dc1394),)
HAVE_FIREWIRE_CAM   = 1
VISION_CAM_LIBS    += dc1394
else
$(warning Cannot build Firewire camera, libdc1394 headers not found)
endif

HAVE_DPPTU_CTRL     = 0
HAVE_FILELOADER_CAM = 1
HAVE_NETWORK_CAM    = 1
HAVE_SHMEM_CAM      = 1
HAVE_V4L_CAM        = 0

# Check for external libraries
IPP_DIR  = /opt/intel/ipp
HAVE_IPP = 0
ifneq ($(realpath $(IPP_DIR)),)
  # Check versions, use first one found
  IPP_VERSION = $(firstword $(shell ls $(IPP_DIR)))
  # We at least have a IPP, check if it matches our system
  ARCH = $(shell uname -m)
  INTEL_ARCH = ia32
  ifeq ($(ARCH),x86_64)
    INTEL_ARCH = em64t
  endif
  ifneq ($(realpath $(IPP_DIR)/$(IPP_VERSION)/$(INTEL_ARCH)/include/ipp.h),)
    HAVE_IPP = 1
    VISION_LIBDIRS += $(IPP_DIR)/$(IPP_VERSION)/$(INTEL_ARCH)/sharedlib
    VISION_INCDIRS += $(IPP_DIR)/$(IPP_VERSION)/$(INTEL_ARCH)/include
  endif
endif

VISION_CFLAGS       += $(foreach CAM,$(CAMS),$(if $(subst 0,,$(HAVE_$(CAM)_CAM)),-DHAVE_$(CAM)_CAM))

ifeq ($(MAKECMDGOALS),printconf)
VISION_CAM_PRINT     = $(foreach CAM,$(CAMS),"\b"$(CAM): $(if $(subst 0,,$(HAVE_$(CAM)_CAM)),"yes","no")"\n")
printconf:
	$(SILENT)echo "Cameras:"
	$(SILENT)echo -e $(VISION_CAM_PRINT)
	$(SILENT)echo VISION_LIBDIRS:  $(VISION_LIBDIRS)
	$(SILENT)echo VISION_INCDIRS:  $(VISION_INCDIRS)
	$(SILENT)echo VISION_CAM_LIBS: $(VISION_CAM_LIBS)
	$(SILENT)echo VISION_CFLAGS:   $(VISION_CFLAGS)
endif
