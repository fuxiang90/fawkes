
/***************************************************************************
 *  module_manager.h - manager for modules (i.e. shared objects)
 *
 *  Generated: Wed Aug 23 16:15:02 2006
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

#ifndef __UTILS_SYSTEM_DYNAMIC_MODULE_MODULE_MANAGER_H_
#define __UTILS_SYSTEM_DYNAMIC_MODULE_MODULE_MANAGER_H_

#include <utils/system/dynamic_module/module.h>
#include <string>

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
   */
  virtual Module *  openModule(std::string filename) = 0;

  /** Close a module by Module instance
   * @param module The module that is to be closed
   */
  virtual void      closeModule(Module *module) = 0;

  /** Close a module by filename
   * @param filename the name of the module file that should be closed, this
   * is compared to loaded modules and must match what
   * Module::GetBaseFilename() returns
   */
  virtual void      closeModule(std::string filename) = 0;

  /** Check if the module for the given filename is already
   * opened
   * @param filename the name of the module file to check if it is opened.
   * It is compared to loaded modules and must match what
   * Module::GetBaseFilename() returns
   */
  virtual bool      moduleOpened(std::string filename) = 0;

  /** Get the file extension for the current module type
   * @return Returns a string with the file extension that has to
   * be used for modules on the current system (for example "so")
   */
  virtual std::string getModuleFileExtension() = 0;

};


#endif