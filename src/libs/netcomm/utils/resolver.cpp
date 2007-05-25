
/***************************************************************************
 *  resolver.cpp - Fawkes network name resolver
 *
 *  Created: Tue Nov 14 14:25:52 2006
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

#include <netcomm/utils/resolver.h>
#include <netcomm/utils/resolver_thread.h>
#include <netcomm/dns-sd/avahi_thread.h>
#include <netcomm/dns-sd/avahi_resolver.h>
#include <core/exceptions/system.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>


/** @class NetworkNameResolver <netcomm/utils/resolver.h>
 * Network name and address resolver.
 * This class implements a facility to resolve host names to addresses
 * and vice versa. It provides a simplified interface that only supports
 * IPv4 and will return only the first answer received. It has
 * optional support for using mDNS via Avahi for lookups on the local
 * network in the .local domain.
 *
 * Quite some effort has been done to ensure the speediness of this
 * implementation. It is assumed that a fast lookup is the most important
 * thing for the resolver. This means especially that under some circumstances
 * no result can be supplied on the first call but just on a subsequent call
 * (it is not defined if or when a result will be available). So it is a
 * good choice to always call the resolver and let it do the right thing
 * and not cache names and addresses in your own application. This also
 * makes the cache more efficient since multiple threads may use this
 * single resolver. The resolver itself holds a resolver thread that
 * will do lookups concurrently which will update the cache with
 * results thus that subsequent calls will provide the correct
 * information from the cache.
 *
 * The resolver uses an internal cache of name to address and addres to
 * name mapping. If a valid lookup has happened this mapping is assumed
 * to be authoritative and that it will not change. If you want to flush
 * the cache from time to time you may use flush_cache() to do so.
 *
 * In general resolve_name() and resolve_address() immediately return. If no
 * answer is in the cache for resolve_address() it will just provide the textual
 * representation of the IP address. This is different for resolve_name(). If
 * no answer is available if will order a concurrent lookup and return a
 * lookup failure. Subsequent calls may succeed if the cache was successfully
 * updated by the concurrent resolver thread. If you need the answer to be
 * able to proceed use resolve_name_blocking(). This will wait until an
 * answer is available via the host lookup facilities of the system or
 * optional via mDNS.
 *
 * @author Tim Niemueller
 */


/** Constructor.
 * @param avahi_thread Optional avahi thread, Avahi is not used if NULL
 */
NetworkNameResolver::NetworkNameResolver(AvahiThread *avahi_thread)
{
  addr2name_cache.clear();
  name2addr_cache.clear();

  resolver_thread = new NetworkNameResolverThread(this, avahi_thread);
  resolver_thread->start();
  // Wait for thread to start
  usleep(0);
}


/** Destructor. */
NetworkNameResolver::~NetworkNameResolver()
{
  flush_cache();
  resolver_thread->cancel();
  resolver_thread->join();
  delete resolver_thread;
}


/** Flush cache.
 * Flushes the caches for name to address and address to name mappings.
 */
void
NetworkNameResolver::flush_cache()
{
  while ( ! addr2name_cache.empty() ) {
    a2ncit = addr2name_cache.begin();
    free((*a2ncit).second.first);
    addr2name_cache.erase(a2ncit);
  }
  while ( ! name2addr_cache.empty() ) {
    n2acit = name2addr_cache.begin();
    char *name = (*n2acit).first;
    free((*n2acit).second.first);
    name2addr_cache.erase(n2acit);
    free(name);
  }
  /* Leads to a segfault, if one element is in the queue it is deleted
   * two times, do not use
  for (n2acit = name2addr_cache.begin(); n2acit != name2addr_cache.end(); ++n2acit) {
    free((*n2acit).first);
    free((*n2acit).second.first);
  }
  */
}


/** Resolve name.
 * This will lookup a name from the cache and return the value if available.
 * If there is no entry in the cache this will order a concurrent lookup of the
 * name an return a failure.
 * @param name name to resolve
 * @param addr contains a pointer to the address record upon return, this record
 * is in the cache, so you may not free the resulting address! The address is
 * always of type struct sockaddr_in (IPv4) at the moment.
 * @param addrlen contains the length of addr in bytes upon return
 * @return true if resolution was successful, false otherwise
 */
bool
NetworkNameResolver::resolve_name(char *name,
				  struct sockaddr **addr, socklen_t *addrlen)
{
  if ( name2addr_cache.find( name ) != name2addr_cache.end() ) {
    // the name is in the cache, refetch?
    std::pair<struct sockaddr *, time_t> &nrec = name2addr_cache[name];
    if ( nrec.second >= time(NULL) ) {
      // entry outdated, retry
      resolver_thread->resolve_name(name);
    }
    *addr = nrec.first;
    *addrlen = sizeof(struct sockaddr_in);
    return true;
  } else {
    resolver_thread->resolve_name(name);
    return false;
  }
}


/** Resolve name and wait for the result.
 * This will lookup a name from the cache and return the value if available.
 * If there is no entry in the cache this will order a concurrent lookup of the
 * name and wait for the result.
 * @param name name to resolve
 * @param addr contains a pointer to the address record upon return, this record
 * is in the cache, so you may not free the resulting address! The address is
 * always of type struct sockaddr_in (IPv4) at the moment.
 * @param addrlen contains the length of addr in bytes upon return
 * @return true if resolution was successful, false otherwise
 */
bool
NetworkNameResolver::resolve_name_blocking(char *name,
					   struct sockaddr **addr, socklen_t *addrlen)
{
  if ( resolve_name(name, addr, addrlen) ) {
    return true;
  } else {
    struct sockaddr *_addr;
    socklen_t _addrlen;
    if ( resolver_thread->resolve_name_immediately(name, &_addr, &_addrlen) ) {
      name_resolved(strdup(name), _addr, _addrlen);
      *addr = _addr;
      *addrlen = _addrlen;
      return true;
    } else {
      return false;
    }
  }
}


/** Resolve address.
 * This will lookup an address from the cache and return the value if available.
 * If there is no entry in the cache this will order a concurrent lookup of the
 * address and return the textual representation of the address.
 * @param addr address to resolve
 * @param addr_len length of addr in bytes
 * @param name contains a pointer to the name upon return. Note that this record
 * resides in the cache and may not be freed.
 * @return true if resolution was successful, false otherwise
 */
bool
NetworkNameResolver::resolve_address(struct sockaddr *addr, socklen_t addr_len, char **name)
{
  struct sockaddr_in *saddr = (struct sockaddr_in *)addr;

  if ( addr2name_cache.find( saddr->sin_addr.s_addr ) != addr2name_cache.end() ) {
    // the name is in the cache, refetch?
    std::pair<char *, time_t> &nrec = addr2name_cache[saddr->sin_addr.s_addr];
    if ( nrec.second >= time(NULL) ) {
      // entry outdated, retry
      resolver_thread->resolve_address(addr, addr_len);
    }
    *name = nrec.first;
  } else {
    char tmp[INET_ADDRSTRLEN];
    if ( inet_ntop(AF_INET, &(saddr->sin_addr), tmp, sizeof(tmp)) ) {
      size_t s = strlen(tmp) + 1;
      char *n = (char *)malloc(s);
      memset(n, 0, s);
      strcpy(n, tmp);

      addr2name_cache[saddr->sin_addr.s_addr] = std::pair<char *, time_t>(n, time(NULL) + 10);
      *name = n;
    } else {
      return false;
    }
    
    resolver_thread->resolve_address(addr, addr_len);
  }

  return true;

  /*
  char hbuf[NI_MAXHOST];
  if ( getnameinfo(addr, addr_len, hbuf, sizeof(hbuf), NULL, 0, 0) == -1 ) {
    return false;
  } else {
    char *tmp = (char *)malloc(strlen(hbuf) + 1);
    if ( ! tmp ) {
      throw OutOfMemoryException();
    }
    strcpy(tmp, hbuf);
    *name = tmp;
    return true;
  }
  */
}


/** Name has been resolved by resolver thread.
 * This is an internal function, if you modify it, please make absolutely sure that you
 * understand the caches, especially when the key has to be freed! Also note that we
 * take over the ownership name and addr and are responsible for freeing at some
 * point!
 * @param name host name
 * @param addr address structure
 * @param addrlen length in bytes of addr
 */
void
NetworkNameResolver::name_resolved(char *name, struct sockaddr *addr, socklen_t addrlen)
{
  if ( (n2acit = name2addr_cache.find( name )) != name2addr_cache.end() ) {
    // delete old entry
    char *n = (*n2acit).first;
    free((*n2acit).second.first);
    name2addr_cache.erase(n2acit);
    free(n);
  }
  name2addr_cache[name] = std::pair<struct sockaddr *, time_t>(addr, 0);
}


void
NetworkNameResolver::addr_resolved(struct sockaddr *addr, socklen_t addrlen,
				   char *name, bool namefound)
{
  printf("DEB Addr resolved to %s\n", name);
  struct sockaddr_in *saddr = (struct sockaddr_in *)addr;
  if ( (a2ncit = addr2name_cache.find( saddr->sin_addr.s_addr )) != addr2name_cache.end() ) {
    // delete old entry
    free((*a2ncit).second.first);
    addr2name_cache.erase(a2ncit);
  }
  addr2name_cache[saddr->sin_addr.s_addr] =
    std::pair<char *, time_t>(name, (namefound ? 0 : time(NULL) + 10));
  free(addr);
}


void
NetworkNameResolver::name_resolution_failed(char *name)
{
  free(name);
}


void
NetworkNameResolver::address_resolution_failed(struct sockaddr *addr, socklen_t addrlen)
{
  free(addr);
}