
/***************************************************************************
 *  blackboard.h - BlackBoard aspect for Fawkes
 *
 *  Created: Thu Jan 11 16:28:58 2007
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

#ifndef __ASPECT_BLACKBOARD_H_
#define __ASPECT_BLACKBOARD_H_

class BlackBoardInterfaceManager;

class BlackBoardAspect
{
 public:
  virtual ~BlackBoardAspect();

  void setInterfaceManager(BlackBoardInterfaceManager *im);

 protected:
  BlackBoardInterfaceManager *interface_manager;
};

#endif