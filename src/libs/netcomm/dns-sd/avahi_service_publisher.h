
/***************************************************************************
 *  avahi_service_publisher.h - publish services via avahi
 *
 *  Created: Tue Nov 07 16:38:00 2006
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

#ifndef __NETCOMM_DNSSD_AVAHI_SERVICE_PUBLISHER_H_
#define __NETCOMM_DNSSD_AVAHI_SERVICE_PUBLISHER_H_

#include <netcomm/dns-sd/avahi_service.h>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <list>

class AvahiServicePublisher
{
 friend class AvahiThread;

 public:
  AvahiServicePublisher();
  ~AvahiServicePublisher();

  void publish(AvahiService *service);

 private:
  static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state,
				   void *instance);

  void create_services();
  void group_reset();
  void group_erase();
  void name_collision();

  std::list<AvahiService *> services;

  AvahiClient      *client;
  AvahiEntryGroup  *group;
};


#endif