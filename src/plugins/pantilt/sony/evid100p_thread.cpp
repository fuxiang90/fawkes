
/***************************************************************************
 *  evid100p_thread.h - Sony EviD100P pan/tilt unit act thread
 *
 *  Created: Sun Jun 21 12:38:34 2009
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

#include "evid100p_thread.h"
#include "evid100p.h"

#include <core/threading/mutex_locker.h>
#include <interfaces/PanTiltInterface.h>

#include <cstdarg>
#include <cmath>

using namespace fawkes;

/** @class PanTiltSonyEviD100PThread "evid100p_thread.h"
 * PanTilt act thread for the PTU part of the Sony EviD100P camera.
 * This thread integrates into the Fawkes main loop at the ACT_EXEC hook and
 * interacts via the Visca protocol with the controller of the Sony EviD100P.
 * @author Tim Niemueller
 */

/** Constructor.
 * @param pantilt_cfg_prefix pantilt plugin configuration prefix
 * @param ptu_cfg_prefix configuration prefix specific for the PTU
 * @param ptu_name name of the PTU configuration
 */
PanTiltSonyEviD100PThread::PanTiltSonyEviD100PThread(std::string &pantilt_cfg_prefix,
						     std::string &ptu_cfg_prefix,
						     std::string &ptu_name)
  : PanTiltActThread("PanTiltSonyEviD100PThread"),
    BlackBoardInterfaceListener("PanTiltSonyEviD100PThread")
{
  set_name("PanTiltSonyEviD100PThread(%s)", ptu_name.c_str());

  __pantilt_cfg_prefix = pantilt_cfg_prefix;
  __ptu_cfg_prefix     = ptu_cfg_prefix;
  __ptu_name           = ptu_name;

  __cam = NULL;
}


void
PanTiltSonyEviD100PThread::init()
{
  // Note: due to the use of auto_ptr and RefPtr resources are automatically
  // freed on destruction, therefore no special handling is necessary in init()
  // itself!

  __cfg_device           = config->get_string((__ptu_cfg_prefix + "device").c_str());
  __cfg_read_timeout_ms  = config->get_uint((__ptu_cfg_prefix + "read_timeout_ms").c_str());

  try {
    __cam = new SonyEviD100PVisca(__cfg_device.c_str(), __cfg_read_timeout_ms);
  } catch (Exception &e) {
    e.print_trace();
    e.print_backtrace();
    throw;
  }

  // If you have more than one interface: catch exception and close them!
  std::string bbid = "PanTilt " + __ptu_name;
  __pantilt_if = blackboard->open_for_writing<PanTiltInterface>(bbid.c_str());
  __pantilt_if->set_calibrated(true);
  __pantilt_if->set_min_pan(SonyEviD100PVisca::MIN_PAN_RAD);
  __pantilt_if->set_max_pan(SonyEviD100PVisca::MAX_PAN_RAD);
  __pantilt_if->set_min_tilt(SonyEviD100PVisca::MIN_TILT_RAD);
  __pantilt_if->set_max_tilt(SonyEviD100PVisca::MAX_TILT_RAD);
  __pantilt_if->set_enabled(true); // Cannot be turned off

  float pan_smin, pan_smax, tilt_smin, tilt_smax;
  __cam->get_speed_limits(pan_smin, pan_smax, tilt_smin, tilt_smax);
  __pantilt_if->set_max_pan_velocity(pan_smax);
  __pantilt_if->set_max_tilt_velocity(tilt_smax);
  __pantilt_if->set_pan_velocity(pan_smax);
  __pantilt_if->set_tilt_velocity(tilt_smax);
  __pantilt_if->write();

  __wt = new WorkerThread(__ptu_name, logger, __cam,
			  SonyEviD100PVisca::MIN_PAN_RAD, SonyEviD100PVisca::MAX_PAN_RAD,
			  SonyEviD100PVisca::MIN_TILT_RAD, SonyEviD100PVisca::MAX_TILT_RAD);
  __wt->start();

  __wt->set_velocities(pan_smax, tilt_smax);

  bbil_add_message_interface(__pantilt_if);
  blackboard->register_listener(this, BlackBoard::BBIL_FLAG_MESSAGES);

#ifdef USE_TIMETRACKER
  __tt.reset(new TimeTracker());
  __tt_count = 0;
  __ttc_read_sensor = __tt->add_class("Read Sensor");
#endif  

}


void
PanTiltSonyEviD100PThread::finalize()
{
  blackboard->unregister_listener(this);
  blackboard->close(__pantilt_if);

  __wt->cancel();
  __wt->join();
  delete __wt;

  // Setting to NULL deletes instance (RefPtr)
  __cam = NULL;
}


/** Update sensor values as necessary.
 * To be called only from PanTiltSensorThread. Writes the current pan/tilt
 * data into the interface.
 */
void
PanTiltSonyEviD100PThread::update_sensor_values()
{
  if (__wt->has_fresh_data()) {
    float pan = 0, tilt = 0;
    __wt->get_pantilt(pan, tilt);
    __pantilt_if->set_pan(pan);
    __pantilt_if->set_tilt(tilt);
    __pantilt_if->set_final(__wt->is_final());
    __pantilt_if->write();
  }
}


void
PanTiltSonyEviD100PThread::loop()
{
  __pantilt_if->set_final(__wt->is_final());

  while (! __pantilt_if->msgq_empty() ) {
    if (__pantilt_if->msgq_first_is<PanTiltInterface::CalibrateMessage>()) {
      // ignored

    } else if (__pantilt_if->msgq_first_is<PanTiltInterface::GotoMessage>()) {
      PanTiltInterface::GotoMessage *msg = __pantilt_if->msgq_first(msg);

      __wt->goto_pantilt(msg->pan(), msg->tilt());
      __pantilt_if->set_msgid(msg->id());
      __pantilt_if->set_final(false);

    } else if (__pantilt_if->msgq_first_is<PanTiltInterface::ParkMessage>()) {
      PanTiltInterface::ParkMessage *msg = __pantilt_if->msgq_first(msg);

      __wt->goto_pantilt(0, 0);
      __pantilt_if->set_msgid(msg->id());
      __pantilt_if->set_final(false);

    } else if (__pantilt_if->msgq_first_is<PanTiltInterface::SetEnabledMessage>()) {
      PanTiltInterface::SetEnabledMessage *msg = __pantilt_if->msgq_first(msg);

      logger->log_warn(name(), "SetEnabledMessage ignored for Sony EviD100P");

    } else if (__pantilt_if->msgq_first_is<PanTiltInterface::SetVelocityMessage>()) {
      PanTiltInterface::SetVelocityMessage *msg = __pantilt_if->msgq_first(msg);

      logger->log_warn(name(), "SetVelocityMessage ignored for Sony EviD100P");

      if ((msg->pan_velocity() < 0) || (msg->tilt_velocity() < 0) ) {
	logger->log_warn(name(), "Ignoring pan/tilt velocities %f/%f, at least one "
			 " is negative", msg->pan_velocity(), msg->tilt_velocity());
      } else if (msg->pan_velocity() > __pantilt_if->max_pan_velocity()) {
	logger->log_warn(name(), "Desired pan velocity %f too high, max is %f",
			 msg->pan_velocity(), __pantilt_if->max_pan_velocity());
      } else if (msg->tilt_velocity() > __pantilt_if->max_tilt_velocity()) {
	logger->log_warn(name(), "Desired tilt velocity %f too high, max is %f",
			 msg->tilt_velocity(), __pantilt_if->max_tilt_velocity());
      } else {
	__wt->set_velocities(msg->pan_velocity(), msg->tilt_velocity());
	__pantilt_if->set_pan_velocity(msg->pan_velocity());
	__pantilt_if->set_tilt_velocity(msg->tilt_velocity());
      }

    } else {
      logger->log_warn(name(), "Unknown message received");
    }

    __pantilt_if->msgq_pop();
  }

  __pantilt_if->write();

}


bool
PanTiltSonyEviD100PThread::bb_interface_message_received(Interface *interface,
						 Message *message) throw()
{
  if (message->is_of_type<PanTiltInterface::StopMessage>()) {
    __wt->stop_motion();
    return false; // do not enqueue StopMessage
  } else if (message->is_of_type<PanTiltInterface::FlushMessage>()) {
    __wt->stop_motion();
    logger->log_info(name(), "Flushing message queue");
    __pantilt_if->msgq_flush();
    return false;
  } else {
    logger->log_info(name(), "Received message of type %s, enqueueing", message->type());
    return true;
  }
}


/** @class PanTiltSonyEviD100PThread::WorkerThread "sony/evid100p_thread.h"
 * Worker thread for the PanTiltSonyEviD100PThread.
 * This continuous thread issues commands to the camera. In each loop it
 * will first execute pending operations, and then update the sensor data (lengthy
 * operation). Sensor data will only be updated while either a servo in the chain
 * is still moving or torque is disabled (so the motor can be move manually).
 * @author Tim Niemueller
 */


/** Constructor.
 * @param ptu_name name of the pan/tilt unit
 * @param logger logger
 * @param cam Visca controller object
 * @param pan_min minimum pan in rad
 * @param pan_min maximum pan in rad
 * @param tilt_min minimum tilt in rad
 * @param tilt_max maximum tilt in rad
 */
PanTiltSonyEviD100PThread::WorkerThread::WorkerThread(std::string ptu_name,
						      fawkes::Logger *logger,
						      fawkes::RefPtr<SonyEviD100PVisca> cam,
						      const float &pan_min,
						      const float &pan_max,
						      const float &tilt_min,
						      const float &tilt_max)
  : Thread("", Thread::OPMODE_WAITFORWAKEUP)
{
  set_name("SonyEviD100PWorkerThread(%s)", ptu_name.c_str());
  set_coalesce_wakeups(true);

  __logger           = logger;

  __move_mutex       = new Mutex();

  __cam              = cam;
  __move_pending     = false;
  __target_pan       = 0;
  __target_tilt      = 0;

  __velo_pending     = false;
  __pan_vel          = 0;
  __tilt_vel         = 0;

  __pan_min          = pan_min;
  __pan_max          = pan_max;
  __tilt_min         = tilt_min;
  __tilt_max         = tilt_max;
}


/** Destructor. */
PanTiltSonyEviD100PThread::WorkerThread::~WorkerThread()
{
  delete __move_mutex;
}


/** Stop currently running motion. */
void
PanTiltSonyEviD100PThread::WorkerThread::stop_motion()
{
  float pan = 0, tilt = 0;
  get_pantilt(pan, tilt);
  goto_pantilt(pan, tilt);
}


/** Goto desired pan/tilt values.
 * @param pan pan in radians
 * @param tilt tilt in radians
 */
void
PanTiltSonyEviD100PThread::WorkerThread::goto_pantilt(float pan, float tilt)
{
  MutexLocker lock(__move_mutex);
  __target_pan   = pan;
  __target_tilt  = tilt;
  __move_pending = true;
  wakeup();
}


/** Get pan/tilt value.
 * @param pan upon return contains the current pan value
 * @param tilt upon return contains the current tilt value
 */
void
PanTiltSonyEviD100PThread::WorkerThread::get_pantilt(float &pan, float &tilt)
{
  pan  = __cur_pan;
  tilt = __cur_tilt;
}


/** Set desired velocities.
 * @param pan_vel pan velocity
 * @param tilt_vel tilt velocity
 */
void
PanTiltSonyEviD100PThread::WorkerThread::set_velocities(float pan_vel, float tilt_vel)
{
  __pan_vel      = pan_vel;
  __tilt_vel     = tilt_vel;
  __velo_pending = true;
}


/** Check if motion is final.
 * @return true if motion is final, false otherwise
 */
bool
PanTiltSonyEviD100PThread::WorkerThread::is_final()
{
  MutexLocker lock(__move_mutex);
  return __cam->is_nonblocking_finished(SonyEviD100PVisca::NONBLOCKING_PANTILT);
}


/** Check is fresh sensor data is available.
 * Note that this method will return true at once per sensor update cycle.
 * @return true if fresh data is available, false otherwise
 */
bool
PanTiltSonyEviD100PThread::WorkerThread::has_fresh_data()
{
  bool rv = __fresh_data;
  __fresh_data = false;
  return rv;
}


void
PanTiltSonyEviD100PThread::WorkerThread::once()
{
  // do some process cycles to process data returning back from set_address()
  // and clear calls
  for (int i = 0; i < 20; ++i) {
    try {
      __cam->process();
    } catch (Exception &e) { /* ignored */ }
  }
}


void
PanTiltSonyEviD100PThread::WorkerThread::loop()
{
  try {
    __cam->process();
  } catch (Exception &e) {
    __logger->log_warn(name(), "Data processing failed, exception follows");
    __logger->log_warn(name(), e);
  }

  if (__velo_pending) {
    try {
      __cam->set_speed_radsec(__pan_vel, __tilt_vel);
    } catch (Exception &e) {
      __logger->log_warn(name(), "Setting pan/tilt values failed, exception follows");
      __logger->log_warn(name(), e);
    }
    __velo_pending = false;
  }

  if (__move_pending) {
    __move_mutex->lock();
    __logger->log_debug(name(), "Executing goto to %f, %f", __target_pan, __target_tilt);
    exec_goto_pantilt(__target_pan, __target_tilt);
    __move_mutex->unlock();
  }

  //__cam->start_get_pan_tilt();
  try {
    __cam->get_pan_tilt_rad(__cur_pan, __cur_tilt);
    __fresh_data = true;
  } catch (Exception &e) {
    __logger->log_warn(name(), "Failed to get new pan/tilt data, exception follows");
    __logger->log_warn(name(), e);
  }

  if (! is_final() || ! __fresh_data) {
    // while moving or if data reception failed wake us up to get new servo data
    wakeup();
  }
}


/** Execute pan/tilt motion.
 * @param pan_rad pan in rad to move to
 * @param tilt_rad tilt in rad to move to
 */
void
PanTiltSonyEviD100PThread::WorkerThread::exec_goto_pantilt(float pan_rad, float tilt_rad)
{
  if ( (pan_rad < __pan_min) || (pan_rad > __pan_max) ) {
    __logger->log_warn(name(), "Pan value out of bounds, min: %f  max: %f  des: %f",
		       __pan_min, __pan_max, pan_rad);
    return;
  }
  if ( (tilt_rad < __tilt_min) || (tilt_rad > __tilt_max) ) {
    __logger->log_warn(name(), "Tilt value out of bounds, min: %f  max: %f  des: %f",
		       __tilt_min, __tilt_max, tilt_rad);
    return;
  }

  try {
    __cam->set_pan_tilt_rad(pan_rad, tilt_rad);
  } catch (Exception &e) {
    __logger->log_warn(name(), "Failed to execute pan/tilt to %f, %f, exception "
		       "follows", pan_rad, tilt_rad);
    __logger->log_warn(name(), e);
  }
  __move_pending = false;
}
