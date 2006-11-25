 
/***************************************************************************
 *  message.h - Interface generator message representation
 *
 *  Generated: Wed Oct 11 22:21:16 2006 (Germany - Slowakia  4:1)
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

#ifndef __INTERFACES_GENERATOR_MESSAGE_H_
#define __INTERFACES_GENERATOR_MESSAGE_H_

#include <interfaces/generator/field.h>

#include <string>
#include <vector>

class InterfaceMessage
{
 public:
  InterfaceMessage(const std::string &name, const std::string &comment);

  std::string getName();
  std::string getComment();
  void setFields(const std::vector<InterfaceField> &fields);
  std::vector<InterfaceField> getFields();

 private:
  std::string name;
  std::string comment;
  std::vector<InterfaceField> fields;
};



#endif