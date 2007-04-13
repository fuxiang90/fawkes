
/***************************************************************************
 *  shm_registry.h - definitions of images to avoid conflicts
 *
 *  Generated: Thu Jan 12 13:12:24 2006
 *  Copyright  2005-2007  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __FIREVISION_FVUTILS_IPC_SHM_REGISTRY_H_
#define __FIREVISION_FVUTILS_IPC_SHM_REGISTRY_H_


// Magic token to identify FireVision shared memory segments
#define FIREVISION_SHM_IMAGE_MAGIC_TOKEN "FireVision Image"
#define FIREVISION_SHM_LUT_MAGIC_TOKEN "FireVision LUT"

/* For images: Define well-known IDs for images
 * Ranges:
 * 1-9    front vision
 * 10-19  omni vision
 * 20-39  generated by fountain (add 20 to front/omni id)
 */
#define FIREVISION_SHM_IMAGE_INVALID            0xFFFFFFFF
#define FIREVISION_SHM_IMAGE_FRONT_RAW                   1
#define FIREVISION_SHM_IMAGE_FRONT_PROCESSED             2
#define FIREVISION_SHM_IMAGE_OMNI_RAW                   10
#define FIREVISION_SHM_IMAGE_FOUNTAIN_MINI_FRONT_RAW    21
#define FIREVISION_SHM_IMAGE_FOUNTAIN_MINI_FRONT_PROC   22
#define FIREVISION_SHM_IMAGE_FOUNTAIN_MINI_OMNI_RAW     30

/* For LUT: define well-known IDs for LUTs
 * Ranges:
 * 1-49    front vision
 * 50-99   omni vision
 */
#define FIREVISION_SHM_LUT_INVALID            0xFFFFFFFF
#define FIREVISION_SHM_LUT_FRONT_COLOR         1
#define FIREVISION_SHM_LUT_OMNI_COLOR         50
#define FIREVISION_SHM_LUT_OMNI_BULB          51

#endif