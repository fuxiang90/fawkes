
/***************************************************************************
 *  aspect_initializer.h - Fawkes Aspect initializer
 *
 *  Created: Tue Jan 30 13:36:42 2007
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

#include <aspect/inifin.h>

#include <core/threading/thread.h>
#include <blackboard/blackboard.h>
#include <aspect/blackboard.h>
#include <aspect/blocked_timing.h>
#include <aspect/configurable.h>
#include <aspect/logging.h>
#include <aspect/fawkes_network.h>

/** @class AspectIniFin aspect/inifin.h
 * Fawkes Aspect Initializer/Finalizer.
 * Initializes certain thread aspects.
 * All aspects defined in the Fawkes tree are supported and properly
 * initialized such that guarantees are met.
 * @see Aspects
 * @author Tim Niemueller
 */


/** Constructor.
 * @param blackboard BlackBoard
 * @param config Configuration
 * @param logger Logger
 */
AspectIniFin::AspectIniFin(BlackBoard *blackboard,
			   Configuration *config,
			   Logger *logger)
{
  this->blackboard = blackboard;
  this->config     = config;
  this->logger     = logger;
  this->fnethub    = NULL;
}


/** Set Fawkes Network Hub.
 * Use this to set the Fawkes Network Hub. If you do not use the Fawkes Network
 * you do not need to call this function to set a hub. In that case threads that
 * demand the hub will cause an exception to be thrown that the thread cannot be
 * initialized.
 * @param fnethub Fawkes Network Hub
 */
void
AspectIniFin::set_fnet_hub(FawkesNetworkHub *fnethub)
{
  this->fnethub = fnethub;
}


/** Initialize thread.
 * @param thread thread to initialize
 */
void
AspectIniFin::init(Thread *thread)
{
  // printf("Initializing thread %s\n", thread->name());

  BlockedTimingAspect *blocked_timing_thread;
  if ( (blocked_timing_thread = dynamic_cast<BlockedTimingAspect *>(thread)) != NULL ) {
    if ( thread->opmode() != Thread::OPMODE_WAITFORWAKEUP ) {
      throw CannotInitializeThreadException("Thread not in WAITFORWAKEUP mode (required for BlockedTimingAspect)");
    }
  }

  BlackBoardAspect *blackboard_thread;
  if ( (blackboard_thread = dynamic_cast<BlackBoardAspect *>(thread)) != NULL ) {
    blackboard_thread->initBlackBoardAspect( blackboard->getInterfaceManager() );
  }

  ConfigurableAspect *configurable_thread;
  if ( (configurable_thread = dynamic_cast<ConfigurableAspect *>(thread)) != NULL ) {
    configurable_thread->initConfigurableAspect(config);
  }

  LoggingAspect *logging_thread;
  if ( (logging_thread = dynamic_cast<LoggingAspect *>(thread)) != NULL ) {
    logging_thread->initLoggingAspect(logger);
  }

  FawkesNetworkAspect *fnet_thread;
  if ( (fnet_thread = dynamic_cast<FawkesNetworkAspect *>(thread)) != NULL ) {
    if ( fnethub == NULL ) {
      throw CannotInitializeThreadException("Thread has FawkesNetworkAspect but no FawkesNetworkHub has been set in AspectIniFin");
    }
    fnet_thread->initFawkesNetworkAspect(fnethub);
  }

  try {
    thread->init();
  } catch (Exception &e) {
    e.append("AspectIniFin called Thread[%s]::init() which failed", thread->name());
    throw;
  } catch (...) {
    throw CannotInitializeThreadException("Thread::init() failed and thread threw unsupported exception");
  }

}


/** Finalize thread.
 * @param thread thread to finalize
 */
void
AspectIniFin::finalize(Thread *thread)
{
  try {
    thread->finalize();
  } catch (CannotFinalizeThreadException &e) {
    e.append("AspectIniFin called Thread[%s]::finalize() which failed", thread->name());
    throw;
  } catch (Exception &e) {
    e.append("AspectIniFin called Thread[%s]::finalize() which failed", thread->name());
    throw CannotFinalizeThreadException(e);
  } catch (...) {
    throw CannotFinalizeThreadException("Thread::finalize() failed and thread threw unsupported exception");
  }
}