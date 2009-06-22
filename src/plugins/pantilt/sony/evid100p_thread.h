
/***************************************************************************
 *  evid100p_thread.h - Sony EviD100P pan/tilt unit act thread
 *
 *  Created: Sun Jun 21 12:30:59 2009
 *  Copyright  2006-2009  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#ifndef __PLUGINS_PANTILT_SONY_EVID100P_THREAD_H_
#define __PLUGINS_PANTILT_SONY_EVID100P_THREAD_H_

#include "../act_thread.h"

#include <blackboard/interface_listener.h>

#ifdef USE_TIMETRACKER
#  include <utils/time/tracker.h>
#endif
#include <string>
#include <memory>

namespace fawkes {
  class PanTiltInterface;
}

class SonyEviD100PVisca;

class PanTiltSonyEviD100PThread
: public PanTiltActThread,
  public fawkes::BlackBoardInterfaceListener
{
 public:
  PanTiltSonyEviD100PThread(std::string &pantilt_cfg_prefix,
			    std::string &ptu_cfg_prefix,
			    std::string &ptu_name);

  virtual void init();
  virtual void finalize();
  virtual void loop();

  // For BlackBoardInterfaceListener
  virtual bool bb_interface_message_received(fawkes::Interface *interface,
					     fawkes::Message *message) throw();

  void update_sensor_values();

 /** Stub to see name in backtrace for easier debugging. @see Thread::run() */
 protected: virtual void run() { Thread::run(); }

 private:
  fawkes::PanTiltInterface *__pantilt_if;

  fawkes::RefPtr<SonyEviD100PVisca> __cam;

  std::string  __pantilt_cfg_prefix;
  std::string  __ptu_cfg_prefix;
  std::string  __ptu_name;
  std::string  __cfg_device;
  unsigned int __cfg_read_timeout_ms;


  class WorkerThread : public fawkes::Thread
  {
  public:
    WorkerThread(std::string ptu_name, fawkes::Logger *logger,
		 fawkes::RefPtr<SonyEviD100PVisca> cam,
		 const float &pan_min, const float &pan_max,
		 const float &tilt_min, const float &tilt_max);

    ~WorkerThread();
    void goto_pantilt(float pan, float tilt);
    void get_pantilt(float &pan, float &tilt);
    void set_velocities(float pan_vel, float tilt_vel);
    bool is_final();
    void stop_motion();
    bool has_fresh_data();

    virtual void once();
    virtual void loop();

  private:
    void exec_goto_pantilt(float pan, float tilt);

  private:
    fawkes::RefPtr<SonyEviD100PVisca>  __cam;
    fawkes::Logger        *__logger;

    float         __pan_min;
    float         __pan_max;
    float         __tilt_min;
    float         __tilt_max;

    fawkes::Mutex *__move_mutex;
    bool  __move_pending;
    float __target_pan;
    float __target_tilt;
    bool  __velo_pending;
    float __pan_vel;
    float __tilt_vel;

    float __cur_pan;
    float __cur_tilt;

    bool __fresh_data;
  };

  WorkerThread *__wt;
};

#endif
