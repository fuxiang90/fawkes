
/***************************************************************************
 *  netconf.h - Fawkes remote configuration access via Fawkes net
 *
 *  Created: Sun Jan 07 15:01:50 2007
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

#ifndef __CONFIG_NETCONF_H_
#define __CONFIG_NETCONF_H_

#include <config/config.h>
#include <netcomm/fawkes/client_handler.h>
#include <core/exception.h>

#include <map>
#include <list>
#include <string>

class Mutex;
class FawkesNetworkClient;
class SQLiteConfiguration;

class CannotEnableMirroringException : public Exception
{
 public:
  CannotEnableMirroringException();
};

class NetworkConfiguration : public Configuration, public FawkesNetworkClientHandler
{
 public:
  NetworkConfiguration(FawkesNetworkClient *c);
  virtual ~NetworkConfiguration();

  virtual void          copy(Configuration *copyconf);

  virtual void          add_change_handler(ConfigurationChangeHandler *h);
  virtual void          rem_change_handler(ConfigurationChangeHandler *h);

  virtual void          load(const char *filename, const char *defaults_filename,
			     const char *tag = NULL);

  virtual void          tag(const char *tag);
  virtual std::list<std::string> tags();

  virtual bool          exists(const char *comp, const char *path);
  virtual bool          is_float(const char *comp, const char *path);
  virtual bool          is_uint(const char *comp, const char *path);
  virtual bool          is_int(const char *comp, const char *path);
  virtual bool          is_bool(const char *comp, const char *path);
  virtual bool          is_string(const char *comp, const char *path);

  virtual std::string     get_type(const char *comp, const char *path);
  virtual float           get_float(const char *comp, const char *path);
  virtual unsigned int    get_uint(const char *comp, const char *path);
  virtual int             get_int(const char *comp, const char *path);
  virtual bool            get_bool(const char *comp, const char *path);
  virtual std::string     get_string(const char *comp, const char *path);
  virtual ValueIterator * get_value(const char *comp, const char *path);

  virtual void          set_float(const char *comp, const char *path,
				  float f);
  virtual void          set_uint(const char *comp, const char *path,
				 unsigned int uint);
  virtual void          set_int(const char *comp, const char *path,
				int i);
  virtual void          set_bool(const char *comp, const char *path,
				 bool b);
  virtual void          set_string(const char *comp, const char *path,
				   std::string s);
  virtual void          set_string(const char *comp, const char *path,
				   const char *s, unsigned int s_length);

  virtual void          erase(const char *comp, const char *path);

  virtual void          deregistered();
  virtual void          inboundReceived(FawkesNetworkMessage *msg);

  virtual void          setMirrorMode(bool mirror);

 class NetConfValueIterator : public Configuration::ValueIterator
  {
    friend class NetworkConfiguration;
   protected:
    NetConfValueIterator(Configuration::ValueIterator *i);
    NetConfValueIterator(FawkesNetworkMessage *m);
    NetConfValueIterator();
   public:
    virtual ~NetConfValueIterator();
    virtual bool          next();
    virtual bool          valid();
    
    virtual const char *  component();
    virtual const char *  path();
    virtual const char *  type();
    
    virtual bool          is_float();
    virtual bool          is_uint();
    virtual bool          is_int();
    virtual bool          is_bool();
    virtual bool          is_string();

    virtual float         get_float();
    virtual unsigned int  get_uint();
    virtual int           get_int();
    virtual bool          get_bool();
    virtual std::string   get_string();

   private:
    Configuration::ValueIterator *i;
    FawkesNetworkMessage  *msg;
    bool iterated_once;
    char *_component;
    char *_path;
  };

  ValueIterator * iterator();
  ValueIterator * search(const char *component, const char *path);

  void lock();
  bool tryLock();
  void unlock();

 private:
  void send_get(const char *comp, const char *path, unsigned int msgid);


  FawkesNetworkClient  *c;
  FawkesNetworkMessage *msg;
  Mutex *mutex;

  bool mirror;
  SQLiteConfiguration *mirror_config;
  char *tmp_volatile;
  char *tmp_default;

  std::map<std::string, std::list<ConfigurationChangeHandler *> >  change_handlers;
  std::map<std::string, std::list<ConfigurationChangeHandler *> >::iterator  chit;
  std::list<ConfigurationChangeHandler *>::iterator                cit;

};

#endif