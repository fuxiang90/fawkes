
/***************************************************************************
 *  plugin_gui.h - Plugin Tool Gui
 *
 *  Created: Thu Nov 09 20:15:27 2007
 *  Copyright  2007  Daniel Beck
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

#ifndef __TOOLS_PLUGIN_PLUGIN_GUI_PLUGIN_GUI_H_
#define __TOOLS_PLUGIN_PLUGIN_GUI_PLUGIN_GUI_H_

#include <gtkmm.h>
#include <libglademm/xml.h>

class PluginGuiBackendThread;

class PluginGui : public Gtk::Window
{
 public:
  PluginGui(Glib::RefPtr<Gnome::Glade::Xml> ref_xml);
  virtual ~PluginGui();

  Gtk::Window& get_window() const;

  void update_status();
  void update_list();
  void update_hosts();
  void update_connection();

  void register_backend(PluginGuiBackendThread* backend);

 private:
  class PluginRecord : public Gtk::TreeModelColumnRecord
  {
  public:
    PluginRecord() 
      { 
	add(m_index);
	add(m_name);
	add(m_status);
      }
    
    Gtk::TreeModelColumn<int> m_index;
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<bool> m_status;
  };

  class HostRecord : public Gtk::TreeModelColumnRecord
  {
  public:
    HostRecord() 
      { 
	add(m_index);
	add(m_host);
      }
    
    Gtk::TreeModelColumn<int> m_index;
    Gtk::TreeModelColumn<Glib::ustring> m_host;
  };

  // signal handler
  void clicked_connect();
  void toggled_status(const Glib::ustring path);

  // widgets
  Gtk::Window* m_wnd_main;
  Gtk::ComboBoxEntry* m_cbe_hosts;
  Gtk::Button* m_btn_connect;
  Gtk::TreeView* m_trv_plugins;
  Gtk::Statusbar* m_stb_status;

  PluginGuiBackendThread* m_backend;

  Glib::RefPtr<Gtk::ListStore> m_plugin_list;
  Glib::RefPtr<Gtk::ListStore> m_host_list;
  PluginRecord m_plugin_record;
  HostRecord m_host_record;
};

#endif /* __TOOLS_PLUGIN_PLUGIN_GUI_PLUGIN_GUI_H_ */