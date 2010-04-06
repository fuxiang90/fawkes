
/***************************************************************************
 *  skill_channel_view.cpp - TODO: Add Description
 *
 *  Created: Sat Apr 03 14:36:52 2010
 *  Copyright  2010  Patrick Podbregar [www.podbregar.com]
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

#include "skill_channel_view.h"

#include <gui_utils/interface_dispatcher.h>
#include <blackboard/remote.h>

#include <iostream>

using namespace fawkes;

SkillChannelView::SkillChannelView()
{
  ctor();
}

#ifdef HAVE_GLADEMM
SkillChannelView::SkillChannelView(BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml> ref_xml)
  : Gtk::TreeView(cobject)
{
  ctor();
}
#endif

void
SkillChannelView::ctor()
{
  __skiller_if = NULL;
  skill_string = NULL;

  skill_channel_list = Gtk::ListStore::create(skill_channel_record);
  set_model(skill_channel_list);
  set_rules_hint(true);
  append_column("Ch. #", skill_channel_record.channel_number);
  append_column("Status", skill_channel_record.status);
  append_column("Skill String", skill_channel_record.skill_string);
}

SkillChannelView::~SkillChannelView()
{
}

void
SkillChannelView::setup_channels(fawkes::SkillerInterface *skiller_if)
{
  __skiller_if = skiller_if;
  unsigned number_of_channels = __skiller_if->maxlenof_status();

  for(unsigned channel_number = 0; channel_number < number_of_channels; ++channel_number)
    skill_channel_list->append();

  update_channels();
}

void
SkillChannelView::update_channels()
{
  skill_string = new SkillString(__skiller_if->skill_string());
  unsigned channel_number = 0;
  Gtk::TreeModel::Children children = skill_channel_list->children();
  for(Gtk::TreeModel::Children::iterator iter = children.begin();
      iter != children.end(); ++iter)
  {
    Gtk::TreeModel::Row row = *iter;
    row[skill_channel_record.channel_number] = channel_number++;
    row[skill_channel_record.status] = get_status_text(__skiller_if->status(channel_number));
    row[skill_channel_record.skill_string] = skill_string->get_channel(channel_number);
  }
}

std::string
SkillChannelView::get_status_text(SkillerInterface::SkillStatusEnum status)
{
  std::string status_name;
  switch(status)
  {
  case SkillerInterface::S_INACTIVE:
    status_name = "INACTIVE";
    break;
  case SkillerInterface::S_RUNNING:
    status_name = "RUNNING";
    break;
  case SkillerInterface::S_FINAL:
    status_name = "FINAL";
    break;
  case SkillerInterface::S_FAILED:
    status_name = "FAILED";
    break;
  }
  return status_name;
}

void
SkillChannelView::clear_channels()
{
  skill_channel_list->clear();
  __skiller_if = NULL;
}
