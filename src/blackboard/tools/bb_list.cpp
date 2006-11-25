
/***************************************************************************
 *  bb_list.cpp - list Fawkes BlackBoard shared memory segments
 *
 *  Generated: Thu Oct 19 14:56:13 2006 (Anne's 25th Birthday)
 *  Copyright  2005-2006  Tim Niemueller [www.niemueller.de]
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

#include <utils/ipc/shm.h>
#include <blackboard/shmem_header.h>
#include <blackboard/shmem_lister.h>
#include <blackboard/bbconfig.h>
#include <iostream>

using namespace std;

int
main(int argc, char **argv)
{
  BlackBoardSharedMemoryHeader *bbsh = new BlackBoardSharedMemoryHeader( BLACKBOARD_MEMORY_SIZE,
									 BLACKBOARD_VERSION );
  BlackBoardSharedMemoryLister *bblister = new BlackBoardSharedMemoryLister();
  SharedMemory::list(BLACKBOARD_MAGIC_TOKEN, bbsh, bblister);
  delete bblister;
  delete bbsh;
  return 0;
}