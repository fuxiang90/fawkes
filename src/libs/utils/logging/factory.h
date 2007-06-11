
/***************************************************************************
 *  factory.h - Logger factory
 *
 *  Created: Mon Jun 04 10:54:35 2007
 *  Copyright  2007  Tim Niemueller [www.niemueller.de]
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

#ifndef __UTILS_LOGGING_FACTORY_H_
#define __UTILS_LOGGING_FACTORY_H_

#include <core/exception.h>
#include <core/exceptions/software.h>

#include <cstddef>

class UnknownLoggerTypeException : public Exception
{
 public:
  UnknownLoggerTypeException(const char *msg = NULL);
};

class Logger;
class MultiLogger;

class LoggerFactory
{
 public:
  static Logger * instance(const char *type, const char *as);
  static MultiLogger *multilogger_instance(const char *as);

  /** Get typed instance of logger.
   * Creates a new instance and converts it to the requested type. If the type
   * does not match the requested logger an exception is thrown.
   * @param type logger type
   * @param as logger argument string
   * @return typed logger instance
   * @exception TypeMismatchException thrown, if requested logger does not match
   * requested type.
   */
  template <class L>
    static L* instance(const char *type, const char *as);
};


template <class L>
L *
LoggerFactory::instance(const char *type, const char *as)
{
  Logger *l = LoggerFactory::instance(type, as);
  L *tl = dynamic_cast<L *>(l);
  if ( tl == NULL ) {
    throw TypeMismatchException();
  }
  return tl;
}

#endif