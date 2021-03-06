
/***************************************************************************
 *  module_dl.h - representation of a module (i.e. shared object) using
 *                dl of glibc, applicable for Linux systems
 *
 *  Generated: Wed Aug 23 15:48:23 2006
 *  Copyright  2006  Tim Niemueller [www.niemueller.de]
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

#ifndef __UTILS_SYSTEM_DYNAMIC_MODULE_MODULE_DL_H_
#define __UTILS_SYSTEM_DYNAMIC_MODULE_MODULE_DL_H_

#include <utils/system/dynamic_module/module.h>
#include <string>

namespace fawkes {

class ModuleDL : public Module {
 public:

  ModuleDL(std::string filename, Module::ModuleFlags flags = Module::MODULE_FLAGS_DEFAULT);

  virtual ~ModuleDL();

  virtual void    open();
  virtual bool    close();

  virtual void    ref();
  virtual void    unref();
  virtual bool    notref();
  virtual unsigned int get_ref_count();


  virtual bool    has_symbol(const char *symbol_name);
  virtual void *  get_symbol(const char *symbol_name);

  virtual bool    operator==(ModuleDL &cmod);

  virtual std::string get_filename();
  virtual std::string get_base_filename();

  static const char * get_file_extension();

 private:
  static const char *FILE_EXTENSION;

  void *       handle;
  std::string  filename;
  ModuleFlags  flags;
  bool         file_found;
  bool         is_resident;
  unsigned int ref_count;
};

} // end namespace fawkes

#endif
