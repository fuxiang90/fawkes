 
/***************************************************************************
 *  type_checker.h - Interface generator type checker
 *
 *  Generated: Wed Oct 11 15:35:26 2006
 *  Copyright  2006  Tim Niemueller [www.niemueller.de]
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

#ifndef __INTERFACES_GENERATOR_TYPE_CHECKER_H_
#define __INTERFACES_GENERATOR_TYPE_CHECKED_H_

#include <string>

class InterfaceDataTypeChecker
{
 public:
  static bool validType(const std::string &type);
  static bool validValue(const std::string &type, const std::string &value);
};


#endif