
/***************************************************************************
 *  net_thread.h - Fawkes WorldModel Plugin Network Thread
 *
 *  Created: Fri Jun 29 16:55:52 2007 (on flight to RoboCup 2007, Atlanta)
 *  Copyright  2006-2007  Tim Niemueller [www.niemueller.de]
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

#ifndef __PLUGINS_WORLDMODEL_NET_THREAD_H_
#define __PLUGINS_WORLDMODEL_NET_THREAD_H_

#include <core/threading/thread.h>
#include <aspect/blackboard.h>
#include <aspect/logging.h>
#include <aspect/configurable.h>
#include <aspect/clock.h>
#include <aspect/network.h>
#include <netcomm/worldinfo/handler.h>
#include <core/utils/lock_map.h>


#include <map>
#include <string>

namespace fawkes {
  class WorldInfoTransceiver;
  class ObjectPositionInterface;
  class GameStateInterface;
}

class WorldModelNetworkThread
: public fawkes::Thread,
  public fawkes::LoggingAspect,
  public fawkes::ConfigurableAspect,
  public fawkes::ClockAspect,
  public fawkes::NetworkAspect,
  public fawkes::BlackBoardAspect,
  public fawkes::WorldInfoHandler
{
 public:
  WorldModelNetworkThread();
  virtual ~WorldModelNetworkThread();

  virtual void init();
  virtual void loop();
  virtual void finalize();

  fawkes::WorldInfoTransceiver* get_transceiver();

  /* WorldInfoHandler methods follow */
  virtual void pose_rcvd(const char *from_host,
			 float x, float y, float theta,
			 float *covariance);

  virtual void velocity_rcvd(const char *from_host, float vel_x,
			     float vel_y, float vel_theta, float *covariance);

  virtual void ball_pos_rcvd(const char *from_host,
			     bool visible, int visibility_history,
			     float dist, float bearing, float slope,
			     float *covariance);

  virtual void global_ball_pos_rcvd(const char *from_host,
				    bool visible, int visibility_history,
				    float x,  float y, float z,
				    float *covariance);

  virtual void ball_velocity_rcvd(const char *from_host,
				  float vel_x, float vel_y, float vel_z,
				  float *covariance);
  
  virtual void global_ball_velocity_rcvd(const char *from_host,
					 float vel_x, float vel_y, float vel_z,
					 float *covariance);

  virtual void opponent_pose_rcvd(const char *from_host,
				  unsigned int uid,
				  float distance, float bearing,
				  float *covariance);

  virtual void opponent_disapp_rcvd(const char *from_host, unsigned int uid);

  virtual void gamestate_rcvd(const char *from_host,
			      unsigned int game_state,
			      fawkes::worldinfo_gamestate_team_t state_team,
			      unsigned int score_cyan, unsigned int score_magenta,
			      fawkes::worldinfo_gamestate_team_t our_team,
			      fawkes::worldinfo_gamestate_goalcolor_t our_goal_color,
			      fawkes::worldinfo_gamestate_half_t half);

  virtual void penalty_rcvd(const char *from_host,
			    unsigned int player, unsigned int penalty,
			    unsigned int seconds_remaining);

 private:
  fawkes::WorldInfoTransceiver *__worldinfo_transceiver;

  unsigned int __cfg_sleep_time_msec;
  unsigned int __cfg_max_msgs_per_recv;
  unsigned int __cfg_flush_time_sec;
  bool         __cfg_multicast_loopback;

  typedef std::pair<fawkes::Time, fawkes::ObjectPositionInterface *> TimeObjPosPair;
  typedef std::map<unsigned int, TimeObjPosPair> UidTimeObjPosMap;

  // host -> if
  fawkes::LockMap<std::string, fawkes::ObjectPositionInterface *>  __pose_ifs;
  fawkes::LockMap<std::string, fawkes::ObjectPositionInterface *>  __ball_ifs;
  //host -> (uid -> if)
  fawkes::LockMap<std::string, UidTimeObjPosMap >                  __opponent_ifs;
  fawkes::GameStateInterface *                                     __gamestate_if;

  unsigned int                                                     __opponent_id;

  // host -> time
  fawkes::LockMap<std::string, fawkes::Time>                       __last_seen;
};


#endif
