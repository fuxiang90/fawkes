
/***************************************************************************
 *  bb_shmem_header.h - BlackBoard shared memory header
 *
 *  Created: Thu Oct 19 14:19:06 2006 (Anne's 25th Birthday)
 *  Copyright  2006  Tim Niemueller [www.niemueller.de]
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

#ifndef __BLACKBOARD_SHMEM_HEADER_H_
#define __BLACKBOARD_SHMEM_HEADER_H_

#include <utils/ipc/shm.h>
#include <blackboard/memory_manager.h>

class SharedMemory;

class BlackBoardSharedMemoryHeader : public SharedMemoryHeader
{
 private:
  /** This struct determines the header in the shared memory segment
   */
  typedef struct {
    unsigned int  version;		/**< version of the BB */
    void         *shm_addr;             /**< base addr of shared memory */
    chunk_list_t *free_list_head;	/**< offset of the free chunks list head */
    chunk_list_t *alloc_list_head;	/**< offset of the allocated chunks list head */
  } BlackBoardSharedMemoryHeaderData;

 public:
  BlackBoardSharedMemoryHeader(unsigned int data_size, unsigned int version);
  virtual ~BlackBoardSharedMemoryHeader();
  void setSharedMemory(SharedMemory *shmem);
  virtual bool         matches(void *memptr);
  virtual unsigned int size();
  virtual void         initialize(void *memptr);
  virtual void         set(void *memptr);
  virtual unsigned int dataSize();
  chunk_list_t * getFreeListHead();
  chunk_list_t * getAllocListHead();
  void setFreeListHead(chunk_list_t *flh);
  void setAllocListHead(chunk_list_t *alh);

  unsigned int getVersion() const;

 private:
  unsigned int data_size;
  unsigned int version;
  BlackBoardSharedMemoryHeaderData *data;
  SharedMemory                     *shmem;
};

#endif