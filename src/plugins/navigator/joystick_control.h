
/***************************************************************************
 *  joystick_control.h - Joystick Control
 *
 *  Generated: Tue Jun 05 14:52:10 2007
 *  Copyright  2007  Martin Liebenberg
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
 
#ifndef __NAVIGATOR_JOYSTICK_CONTROL_H_
#define __NAVIGATOR_JOYSTICK_CONTROL_H_

#include <utils/time/time.h>

class MotorInterface;
class Logger;
class Configuration;
class KickerInterface;
class Clock;

class JoystickControl
{
 public:
  JoystickControl(MotorInterface *motor_interface, KickerInterface *kicker_interface, 
  							Logger *logger, Configuration *config, Clock *clock);
  virtual ~JoystickControl();
    
  void enqueueCommand(double forward, double sideward, double rotation, double max_speed);

  void enqueueKick(bool left, bool center, bool right);
 private:
  Logger *logger;
  Configuration *config;

  double actual_speed;
  double actual_joystick_axis_scale;
  double actual_rotation_scale;
    
  MotorInterface *motor_interface;
  KickerInterface *kicker_interface;
  Clock *clock;
  Time last_kick_time;
        
  unsigned int logger_modulo_counter;
  unsigned int logger_modulo_counter2;
};


#endif /*JOYSTICK_CONTROL_H_*/