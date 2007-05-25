
/***************************************************************************
 *  qa_resolver.cpp - Fawkes QA for resolver
 *
 *  Created: Thu May 10 19:10:03 2007
 *  Copyright  2006-2007  Tim Niemueller [www.niemueller.de]
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

/// @cond QA

#include <netcomm/utils/resolver.h>
#include <utils/system/signal.h>
#include <utils/system/argparser.h>
#include <netcomm/dns-sd/avahi_thread.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>


class ResolverQAMain : public SignalHandler
{
 public:
  ResolverQAMain(ArgumentParser *argp)
  {
    this->argp = argp;
    quit = false;

    at = NULL;
    r = NULL;

    if ( argp->hasArgument("a") ) {
      printf("Instantiating Avahi thread\n");
      at = new AvahiThread();
      at->start();
      at->wait_initialized();
      usleep(0);
    }

    r = new NetworkNameResolver(at);

  }

  ~ResolverQAMain()
  {
    if ( at != NULL ) {
      at->cancel();
      at->join();
      delete at;
    }
    delete r;
  }

  void run()
  {
    struct sockaddr_in *s = NULL;
    socklen_t slen;

    char *test = (char *)malloc(strlen("127.0.0.1") + 1);
    strcpy(test, "127.0.0.1");
    r->resolve_name(test, (struct sockaddr **)&s, &slen);
    free(test);

    while ( ! quit && ! r->resolve_name("127.0.0.1", (struct sockaddr **)&s, &slen) ) {
      usleep(0);
    }
    if ( quit ) {
      return;
    }
    printf("Successfully resolved to 0x%x\n", s->sin_addr.s_addr);
  
    if ( ! r->resolve_name("127.0.0.1", (struct sockaddr **)&s, &slen) ) {
      printf("A second try to resolve failed after first success, cache broken\n");
    } else {
      printf("Successfully resolved to 0x%x again\n", s->sin_addr.s_addr);
    }

    char *name;
    if ( ! r->resolve_address((struct sockaddr *)s, slen, &name) ) {
      // printf("Resolving address failed\n");
    } else {
      printf("Successfully resolved address to '%s'\n", name);
    }

    char *tmp;
    if ( (tmp = argp->getArgument("h")) != NULL ) {
      printf("Trying to resolve %s\n", tmp);
      while ( ! quit && ! r->resolve_name(tmp, (struct sockaddr **)&s, &slen) ) {
	usleep(0);
      }
      if ( quit ) {
	return;
      }
      char addrp[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(s->sin_addr), addrp, sizeof(addrp));
      printf("Successfully resolved to 0x%x (%s)\n", s->sin_addr.s_addr, addrp);

      struct sockaddr_in so;
      slen = sizeof(so);
      so.sin_addr.s_addr = s->sin_addr.s_addr;
      r->resolve_address((struct sockaddr *)&so, slen, &tmp);
      printf("Waiting one second to allow resolver thread to suceed\n");
      sleep(1);
      if ( r->resolve_address((struct sockaddr *)&so, slen, &tmp) ) {
	printf("Successfully resolved 0x%x to %s\n", so.sin_addr.s_addr, tmp);
      }
      
    }
  }

  virtual void handle_signal(int signum)
  {
    quit = true;
  }

 private:
  ArgumentParser *argp;
  AvahiThread *at;
  NetworkNameResolver *r;
  bool quit;
};

int
main(int argc, char **argv)
{
  ArgumentParser argp(argc, argv, "ah:");

  ResolverQAMain m(&argp);
  SignalManager::register_handler(SIGINT, &m);
  SignalManager::ignore(SIGPIPE);

  m.run();

  SignalManager::finalize();

  return 0;
}


/// @endcond