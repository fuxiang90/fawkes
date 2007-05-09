
/***************************************************************************
 *  liblogger.h - Fawkes lib logger
 *
 *  Created: Mon May 07 15:19:34 2007
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

#ifndef __UTILS_LOGGING_LIBLOGGER_H_
#define __UTILS_LOGGING_LIBLOGGER_H_

#include <core/exception.h>
#include <cstdarg>
#include <cstddef>

class MultiLogger;
class Logger;

class LibLogger
{
 public:
  static void init(MultiLogger *multi_logger = NULL);
  static void finalize();

  static void add_logger(Logger *logger);
  static void remove_logger(Logger *logger);

  static void log_debug(const char *component, const char *format, ...);
  static void log_info(const char *component, const char *format, ...);
  static void log_warn(const char *component, const char *format, ...);
  static void log_error(const char *component, const char *format, ...);

  static void vlog_debug(const char *component, const char *format, va_list va);
  static void vlog_info(const char *component, const char *format, va_list va);
  static void vlog_warn(const char *component, const char *format, va_list va);
  static void vlog_error(const char *component, const char *format, va_list va);

  static void log_debug(const char *component, Exception &e);
  static void log_info(const char *component, Exception &e);
  static void log_warn(const char *component, Exception &e);
  static void log_error(const char *component, Exception &e);

 private:
  LibLogger(){};

  static MultiLogger *logger;
};

#endif