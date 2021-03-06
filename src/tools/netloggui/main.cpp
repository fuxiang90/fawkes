
/***************************************************************************
 *  main.cpp - NetLog GUI main
 *
 *  Created: Tue Nov 04 23:37:11 2008
 *  Copyright  2008  Tim Niemueller [www.niemueller.de]
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#include "netloggui.h"

/** This is the main program of the NetLog GUI.
 */
int
main(int argc, char **argv) {
  Gtk::Main gtk_main(argc, argv);

  Glib::RefPtr<Gnome::Glade::Xml> refxml;
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  refxml = Gnome::Glade::Xml::create(RESDIR"/guis/netloggui/netloggui.glade");
#else
  std::auto_ptr<Gnome::Glade::XmlError> error;
  refxml = Gnome::Glade::Xml::create(RESDIR"/guis/netloggui/netloggui.glade", "", "", error);
#endif

  NetLogGuiGtkWindow *window = NULL;
  refxml->get_widget_derived("wnd_netloggui", window);

  Gtk::Main::run(*window);

  delete window;

  return 0;
}
