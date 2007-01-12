
/***************************************************************************
 *  blocked_timing.h - Blocked timing aspect for Fawkes
 *
 *  Created: Thu Jan 11 16:52:28 2007
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

#include <aspect/blocked_timing.h>
#include <core/threading/thread.h>

/** @class BlockedTimingAspect aspect/blocked_timing.h
 * Thread aspect to use blocked timing
 * The Fawkes main application provides basic means to synchronize all
 * running thread with respect to several given hooks (see WakeupHook).
 * Threads of a woken up at a particular point in time. The hooks basically
 * correspond to an extended sense - plan - act kind of loop.
 * Your thread must run in Thread::OPMODE_WAITFORWAKEUP mode, otherwise it
 * is not started. This is a requirement for having the BlockedTimingAspect.
 *
 * @see Thread::OpMode
 * @ingroup Aspects
 * @author Tim Niemueller
 *
 * @fn BlockedTimingAspect::WakeupHook BlockedTimingAspect::blocked_timing_hook() const
 * Hook to wakeup this thread.
 * Implement this method to return the hook where this thread should be woken
 * up. It must be guaranteed that this method returns the very same value
 * for the complete life time of the thread.
 * @return hook when to wakeup thread
 */

// Side note: Overriding Thread::run() can make our requirement useless, but
// we believe in the best of the coder: laziness

/** Virtual empty destructor. */
BlockedTimingAspect::~BlockedTimingAspect()
{
}