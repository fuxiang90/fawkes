
/***************************************************************************
 *  clock.cpp - A central clock
 *
 *  Generated: Sun June 03 00:23:59 2007
 *  Copyright  2007  Daniel Beck 
 *             2007  Tim Niemueller [www.niemueller.de]
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

#include <utils/time/clock.h>
#include <utils/time/timesource.h>
#include <utils/logging/liblogger.h>

#include <cstdlib>

/** @class Clock <utils/time/clock.h>
 * This is supposed to be the central clock in Fawkes.
 * It is implemented as a singleton to ensure that there is only
 * one object. So-called TimeSources can be registered at the Clock
 * their current time can be retrieved through the Clock.
 * @author Daniel Beck
 */

/** initialize static members */
Clock* Clock::_instance = NULL;
bool Clock::destruct_ok = false;

/** Constructor. */
Clock::Clock()
{
  destruct_ok = false;
  ext_timesource = 0;
  ext_default = false;
}


/** Destructor.
 * Generates a log message if Clock::finalize wasn't called beforehand 
 */
Clock::~Clock()
{
  if ( !destruct_ok ) {
    LibLogger::log_error("Clock", "dtor called without calling Clock::finalize() first");
  }

  free(ext_timesource);
}


/** Clock initializer.
 * This one is static and has to be called to instantiate a Clock object.
 * In further calls it just returns a pointer to the Clock object.
 * @return a pointer to the Clock object
 */
Clock *
Clock::instance()
{
  if ( NULL == _instance )
    {
      _instance = new Clock();
    }
  
  return _instance;
}


/** Finalize.
 * Sets a boolean flag which prevents a log message from being generated in
 * the destructor.
 */
void
Clock::finalize()
{
  destruct_ok = true;
  delete _instance;
}


/** Register an external time source.
 * 
 * @param ts a pointer to the external time source
 * @param make_default if true, this time source is made the default
 * timesource which means that for every call of get_time() the time
 * of the external time source is returned
 */
void
Clock::register_ext_timesource(TimeSource* ts, bool make_default)
{
  free(ext_timesource);

  ext_timesource = ts;

  if (make_default) {
    ext_default = true;
  }
}


/** Set/unset the external time source as the default time source.
 * @param ext_is_default true to make external time source the default,
 * false to disable it as the default.
 */
void
Clock::set_ext_default_timesource(bool ext_is_default)
{
  if ( ext_is_default ) {
    if (NULL != ext_timesource) {
      ext_default = true;
    } else {
      throw Exception("Trying to make the external timesource the default timesource but there is no external timesource");
    }
  } else {
    ext_default = false;
  }
}


/** Checks whether the external time source is the default time soucre.
 * @return true if external time source is default time source
 */
bool
Clock::is_ext_default_timesource() const
{
  return ext_default;
}


/** Returns the time of the selected time source.
 * @param tv pointer to a timeval struct where the time is written to
 * @param sel allows to select the time source
 */
void
Clock::get_time(struct timeval* tv, TimesourceSelector sel) const
{
  if ( (DEFAULT == sel && !ext_default) ||
       REALTIME == sel)
    {
      gettimeofday(tv, 0);
    }
  else if ( (EXTERNAL == sel) && 
	    (NULL == ext_timesource) )
    {
      throw Exception("No external time source registered");
    }
  else
    {
      ext_timesource->get_time(tv);
    }
}


/** Returns the time of the selected time source.
 * @param tv pointer to a timeval struct where the time is written to
 */
void
Clock::get_time(struct timeval* tv) const
{
  if ( ext_default ) {
    if ( NULL == ext_timesource ) {
      throw Exception("No external time source registered");
    }
    ext_timesource->get_time(tv);
  } else {
    gettimeofday(tv, NULL);
  }
}


/** Returns the time of the selected time source.
 * @param time reference to a time where the result is stored
 */
void
Clock::get_time(Time &time) const
{
  get_time(&(time.time));
}




/** Returns the time of the selected time source.
 * @param time reference to a time where the result is stored
 * @param sel allows to select the time source
 */
void
Clock::get_time(Time &time, TimesourceSelector sel) const
{
  get_time(&(time.time), sel);
}


/** Returns the time of the selected time source.
 * @param time pointer to a Time instance
 */
void
Clock::get_time(Time *time) const
{
  get_time(&(time->time));
}




/** Returns the time of the selected time source.
 * @param time pointer to a Time instance where the time is stored
 * @param sel allows to select the time source
 */
void
Clock::get_time(Time *time, TimesourceSelector sel) const
{
  get_time(&(time->time), sel);
}


/** Returns the system time.
 * @param tv pointer to a timeval struct where the time is written to
 */
void
Clock::get_systime(struct timeval* tv) const
{
  gettimeofday(tv, 0);
}


/** Returns the time of the selected time source.
 * @param time reference to Time instance where the time is stored
 */
void
Clock::get_systime(Time &time) const
{
  get_systime(&(time.time));
}


/** Returns the time of the selected time source.
 * @param time pointer to Time instance where the time is stored
 */
void
Clock::get_systime(Time *time) const
{
  get_systime(&(time->time));
}


/** Get the current time.
 * @return current time
 */
Time
Clock::now() const
{
  Time t(_instance);
  return t.stamp();
}


/** Convert a time given w.r.t. the external time source into the system time.
 * @param t the time that is converted to the system time
 * @return the time in system time
 */ 
Time
Clock::ext_to_realtime(const Time& t)
{
  timeval tv;
  Time ret;
  
  if (NULL != ext_timesource) 
    {
      tv = ext_timesource->conv_to_realtime(t.get_timeval());
      ret.set_time(&tv); 
    }
  else 
    {
      ret = t;
    }

  return ret;
}


/** Check whether an external time source is registered.
 * @return true if an external time source is registered
 */
bool
Clock::has_ext_timesource() const
{
  if (0 != ext_timesource) {
    return true;
  } else {
    return false;
  }
}