
/***************************************************************************
 *  module_manager.h - manager for modules (i.e. shared objects)
 *
 *  Generated: Wed Aug 23 16:15:02 2006
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

#ifndef __UTILS_SYSTEM_DYNAMIC_MODULE_MODULE_MANAGER_H_
#define __UTILS_SYSTEM_DYNAMIC_MODULE_MODULE_MANAGER_H_

#include <utils/system/dynamic_module/module.h>

namespace fawkes {

/** Manager interface to load and unload modules, keeps track of loaded modules
 * and does not reload modules if they are already loaded
 */
class ModuleManager {
 public:

  /** Virtual destructor for pure virtual class
   */
  virtual ~ModuleManager() {}

  /** Open a module
   * @param filename The file name of the module that should be
   * opened. If the ModuleManager implementation takes a base dir
   * argument (recommended) this filename is relative to that
   * base dir
   * @return Returns the module if the file was opened successfully
   * or NULL otherwise. Do NOT delete the module after usage but use
   * closeModule to close it.
   * @exception ModuleOpenException thrown if the module could not be opened
   */
  virtual Module *  open_module(const char *filename) = 0;

  /** Close a module by Module instance
   * @param module The module that is to be closed
   */
  virtual void      close_module(Module *module) = 0;

  /** Close a module by filename
   * @param filename the name of the module file that should be closed, this
   * is compared to loaded modules and must match what
   * Module::GetBaseFilename() returns
   */
  virtual void      close_module(const char *filename) = 0;

  /** Check if the module for the given filename is already
   * opened
   * @param filename the name of the module file to check if it is opened.
   * It is compared to loaded modules and must match what
   * Module::GetBaseFilename() returns
   * @return true if module has been opened, false otherwise
   */
  virtual bool      module_opened(const char *filename) = 0;

  /** Get a module if opened.
   * This will return a pointer to a module if it had already been opened! The
   * reference count is increased and you have to manually unref the module once
   * you are done with it! This method works similar to open_module() with the
   * difference that it is not tried to load the module if it is not open.
   * @param filename file name of the module
   * @return a pointer to the module with the reference cound incremented by one
   * if the module had been opened already or NULL if it was not opened.
   */
  virtual Module *  get_module(const char *filename) = 0;

  /** Get the file extension for the current module type
   * @return Returns a string with the file extension that has to
   * be used for modules on the current system (for example "so")
   */
  virtual const char * get_module_file_extension() = 0;

};

} // end namespace fawkes

#endif
