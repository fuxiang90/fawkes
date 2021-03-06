
/***************************************************************************
 *  act_thread.h - Katana plugin act thread
 *
 *  Created: Mon Jun 08 17:59:57 2009
 *  Copyright  2006-2009  Tim Niemueller [www.niemueller.de]
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

#ifndef __PLUGINS_KATANA_ACT_THREAD_H_
#define __PLUGINS_KATANA_ACT_THREAD_H_

#include "motion_thread.h"
#include "calib_thread.h"
#include "goto_thread.h"
#include "gripper_thread.h"
#include "sensacq_thread.h"

#include <core/threading/thread.h>
#include <aspect/blocked_timing.h>
#include <aspect/logging.h>
#include <aspect/configurable.h>
#include <aspect/blackboard.h>
#include <blackboard/interface_listener.h>
#include <core/utils/refptr.h>
#ifdef USE_TIMETRACKER
#  include <utils/time/tracker.h>
#endif
#include <string>
#include <memory>

namespace fawkes {
  class KatanaInterface;
}

// Classes from libkni (KNI)
class CCdlCOM;
class CCplSerialCRC;
class CLMBase;
class CKatBase;
class CSctBase;

class KatanaActThread
: public fawkes::Thread,
  public fawkes::BlockedTimingAspect,
  public fawkes::LoggingAspect,
  public fawkes::ConfigurableAspect,
  public fawkes::BlackBoardAspect,
  public fawkes::BlackBoardInterfaceListener
{
 public:
  KatanaActThread();

  virtual void init();
  virtual void finalize();
  virtual void once();
  virtual void loop();

  // For BlackBoardInterfaceListener
  virtual bool bb_interface_message_received(fawkes::Interface *interface,
					     fawkes::Message *message) throw();

  void update_sensor_values();

 /** Stub to see name in backtrace for easier debugging. @see Thread::run() */
 protected: virtual void run() { Thread::run(); }

 private:
  void stop_motion();
  void update_position(bool refresh);
  void update_sensors(bool refresh);
  void start_motion(fawkes::RefPtr<KatanaMotionThread> motion_thread,
		    unsigned int msgid, const char *logmsg, ...);

 private:
  fawkes::KatanaInterface *__katana_if;

  std::string    __cfg_device;
  std::string    __cfg_kni_conffile;
  bool           __cfg_auto_calibrate;
  unsigned int   __cfg_defmax_speed;
  unsigned int   __cfg_read_timeout;
  unsigned int   __cfg_write_timeout;
  unsigned int   __cfg_gripper_pollint;
  unsigned int   __cfg_goto_pollint;
  float          __cfg_park_x;
  float          __cfg_park_y;
  float          __cfg_park_z;
  float          __cfg_park_phi;
  float          __cfg_park_theta;
  float          __cfg_park_psi;

  std::auto_ptr<KatanaSensorAcquisitionThread> __sensacq_thread;
  fawkes::RefPtr<KatanaMotionThread>           __actmot_thread;
  fawkes::RefPtr<KatanaCalibrationThread>      __calib_thread;
  fawkes::RefPtr<KatanaGotoThread>             __goto_thread;
  fawkes::RefPtr<KatanaGripperThread>          __gripper_thread;

  fawkes::RefPtr<CLMBase>        __katana;
  std::auto_ptr<CCdlCOM>         __device;
  std::auto_ptr<CCplSerialCRC>   __protocol;
  CKatBase                      *__katbase;
  CSctBase                      *__sensor_ctrl;
#ifdef USE_TIMETRACKER
  std::auto_ptr<fawkes::TimeTracker> __tt;
  unsigned int __tt_count;
  unsigned int __ttc_read_sensor;
#endif
};


#endif
