
/***************************************************************************
 *  qa_shmem.h - QA for IPC shared memory
 *
 *  Generated: Sun Sep 17 16:32:25 2006
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

// Do not include in api reference
///@cond QA

#include <utils/ipc/shm.h>
#include <utils/ipc/shm_exceptions.h>

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>

#define MAGIC_TOKEN "FawkesShmemQAApp"

class QASharedMemoryHeader : public SharedMemoryHeader
{
 private:
  typedef struct {
    unsigned int type;
  } qashmem_header_t;

 public:
  QASharedMemoryHeader(unsigned int type)
  {
    header.type = type;
  }

  virtual bool matches(void *memptr)
  {
    return (memcmp(memptr, &header, sizeof(qashmem_header_t)) == 0);
  }

  virtual unsigned int size()
  {
    return sizeof(qashmem_header_t);
  }

  virtual bool create()
  {
    return true;
  }

  virtual void initialize(void *memptr)
  {
    memcpy(memptr, (char *)&header, sizeof(qashmem_header_t));
  }

  virtual void set(void *memptr)
  {
    memcpy((char *)&header, memptr, sizeof(qashmem_header_t));
  }

  virtual unsigned int dataSize()
  {
    return 1024;
  }

 private:
  qashmem_header_t header;
};


bool quit;

void
signal_handler(int signum)
{
  quit = true;
}


int
main(int argc, char **argv)
{
  quit = false;
  signal(SIGINT, signal_handler);

  QASharedMemoryHeader *h1 = new QASharedMemoryHeader(1);

  SharedMemory *s1, *s2;

  try {
    // This will create the shared memory segment
    s1 = new SharedMemory(MAGIC_TOKEN, h1,
			  /* read only */ false,
			  /* create    */ true,
			  /* destroy   */ true);

    // This will attach to the existing shmem segment,
    // use ipcs to check
    s2 = new SharedMemory(MAGIC_TOKEN, h1,
			  /* read only */ true,
			  /* create    */ false,
			  /* destroy   */ false);
  } catch ( ShmCouldNotAttachException &e ) {
    e.printTrace();
    exit(1);
  }

  int *m1 = (int *)s1->getMemPtr();
  int *m2 = (int *)s2->getMemPtr();

  int i = 0;

  while ( ! quit ) {
    *m1 = i;
    std::cout << "Wrote " << *m1 << " (should be " << i
	      << ") to b1, afterwards b2 reads: " << *m2
	      << std::endl;
    usleep(500000);
    ++i;
  };

  delete s2;
  delete s1;
  delete h1;
}

/// @endcond