
/***************************************************************************
 *  blocked_timing.h - Blocked timing aspect for Fawkes
 *
 *  Created: Thu Jan 11 16:49:25 2007
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

#ifndef __ASPECT_BLOCKED_TIMING_H_
#define __ASPECT_BLOCKED_TIMING_H_

class Thread;

class BlockedTimingAspect
{
 public:
  virtual ~BlockedTimingAspect();

  /** Type to define at which hook the thread is woken up.
   * See FawkesMainThread for information when and in which order the hooks
   * are called.
   * @see FawkesMainThread::loop()
   */
  typedef enum {
    WAKEUP_HOOK_PRE_LOOP,	/**< before each loop */
    WAKEUP_HOOK_SENSOR,		/**< sensor thread */
    WAKEUP_HOOK_WORLDSTATE,	/**< world state thread */
    WAKEUP_HOOK_THINK,		/**< think thread (agent) */
    WAKEUP_HOOK_SKILL,		/**< skill thread (skill module) */
    WAKEUP_HOOK_ACT,		/**< act thread (motor module etc.) */
    WAKEUP_HOOK_POST_LOOP	/**< run after loop */
  } WakeupHook;

  virtual WakeupHook blocked_timing_hook() const = 0;

};

#endif