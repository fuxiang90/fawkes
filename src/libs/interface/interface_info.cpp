
/***************************************************************************
 *  interface_info.h - BlackBoard Interface Info
 *
 *  Created: Mon Mar 03 15:44:46 2008
 *  Copyright  2006-2008  Tim Niemueller [www.niemueller.de]
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#include <interface/interface_info.h>
#include <interface/interface.h>
#include <utils/misc/strndup.h>

#include <cstdlib>
#include <cstring>

namespace fawkes {

/** @class InterfaceInfo <interface/interface_info.h>
 * Interface info.
 * This class holds information about a specific interface.
 * @author Tim Niemueller
 */

/** Constructor.
 * @param type type of the interface
 * @param id id of the interface
 * @param hash version hash
 * @param has_writer true if there is a writer, false otherwise
 * @param num_readers number of readers
 * @param serial instance serial
 */
InterfaceInfo::InterfaceInfo(const char *type, const char *id, const unsigned char *hash,
			     unsigned int serial, bool has_writer, unsigned int num_readers)
{
  __type = strndup(type, __INTERFACE_TYPE_SIZE);
  __id   = strndup(id, __INTERFACE_ID_SIZE);
  __hash = (unsigned char *)malloc(__INTERFACE_HASH_SIZE);
  memcpy(__hash, hash, __INTERFACE_HASH_SIZE);
  __has_writer = has_writer;
  __num_readers = num_readers;
  __serial = serial;
}


/** Copy constructor.
 * @param i info to copy
 */
InterfaceInfo::InterfaceInfo(const InterfaceInfo &i)
{
  __type = strndup(i.__type, __INTERFACE_TYPE_SIZE);
  __id   = strndup(i.__id, __INTERFACE_ID_SIZE);
  __hash = (unsigned char *)malloc(__INTERFACE_HASH_SIZE);
  memcpy(__hash, i.__hash, __INTERFACE_HASH_SIZE);
  __has_writer = i.__has_writer;
  __num_readers = i.__num_readers;
  __serial = i.__serial;
}


/** Destructor. */
InterfaceInfo::~InterfaceInfo()
{
  free(__type);
  free(__id);
  free(__hash);
}


/** Get interface type.
 * @return type string
 */
const char *
InterfaceInfo::type() const
{
  return __type;
}


/** Get interface ID.
 * @return ID string
 */
const char *
InterfaceInfo::id() const
{
  return __id;
}


/** Get interface version hash.
 * @return interface version hash
 */
const unsigned char *
InterfaceInfo::hash() const
{
  return __hash;
}


/** Check if there is a writer.
 * @return true if there is a writer, false otherwise
 */
bool
InterfaceInfo::has_writer() const
{
  return __has_writer;
}


/** Get number of readers.
 * @return number of readers
 */
unsigned int
InterfaceInfo::num_readers() const
{
  return __num_readers;
}


/** Get interface instance serial.
 * @return type string
 */
unsigned int
InterfaceInfo::serial() const
{
  return __serial;
}


/** < operator
 * This compares two interface infos with respect to the less than (<) relation
 * considering the type and id of an interface.
 * An interface info A is less than an interface info B (A < B) iff
 * (A.type < B.type) or ((A.type == B.type) && A.id < B.id).
 * @param ii interface info to compare this to
 * @return true if this instance is considered less than @p ii, false otherwise
 */
bool
InterfaceInfo::operator<(const InterfaceInfo &ii) const
{
  int td = strncmp(__type, ii.__type, __INTERFACE_TYPE_SIZE);
  if ( td < 0 ) {
    return true;
  } else if (td > 0) {
    return false;
  } else {
    return (strncmp(__id, ii.__id, __INTERFACE_ID_SIZE) < 0);
  }
}


/** @class InterfaceInfoList <interface/interface_info.h>
 * Interface information list.
 * List with InterfaceInfo instances.
 * @author Tim Niemueller
 */

/** Append an interface info.
 * @param type type of the interface
 * @param id id of the interface
 * @param hash version hash
 * @param has_writer true if there is a writer, false otherwise
 * @param num_readers number of readers
 * @param serial instance serial
 */
void
InterfaceInfoList::append(const char *type, const char *id, const unsigned char *hash,
			  unsigned int serial, bool has_writer, unsigned int num_readers)
{
  push_back(InterfaceInfo(type, id, hash, serial, has_writer, num_readers));
}

} // end namespace fawkes
