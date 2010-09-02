
/***************************************************************************
 *  laser_drawing_area.cpp - Laser drawing area derived from Gtk::DrawingArea
 *
 *  Created: Thu Oct 09 18:20:21 2008
 *  Copyright  2008-2010  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: laser_drawing_area.cpp 2839 2009-07-22 18:25:57Z doostdar $
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

#include "laser_drawing_area.h"
#include "visdisplay.h"
#include <interfaces/Laser720Interface.h>
#include <interfaces/Laser360Interface.h>
#include <interfaces/ObjectPositionInterface.h>
#include <interfaces/VisualDisplay2DInterface.h>
#include <utils/math/angle.h>
#include <gui_utils/robot/drawer.h>
#include <algorithm>
#include <utils/misc/string_conversions.h>

//#define LASERGUI_DEBUG_PRINT_TRACKS
#define CFG_PRINT_NR_TRACKELEMENTS 1

using namespace fawkes;

/** @class LaserDrawingArea "laser_drawing_area.h"
 * Laser drawing area.
 * Derived version of Gtk::DrawingArea that // // render
 values of a laser interface.
 * @author Tim Niemueller
 * @author Masrur Doostdar
 */

#ifdef HAVE_GLADEMM
/** Constructor.
 * Special ctor to be used with Glade's get_widget_derived().
 * @param cobject Gtk C object
 * @param refxml Glade's XML reference
 */
LaserDrawingArea::LaserDrawingArea(BaseObjectType* cobject,
				   const Glib::RefPtr<Gnome::Glade::Xml>& refxml)
  : Gtk::DrawingArea(cobject)
{
  __draw_mode = MODE_LINES;
  __zoom_factor = 50;
  __laser360_if = NULL;
  __laser720_if = NULL;
  __l_objpos_if_persons = NULL;
  __l_objpos_if_legs = NULL;
  __l_objpos_if_misc = NULL;
  __laser_segmentation_if = NULL;
  __l_track_if = NULL;
  __target_if = NULL;
  __switch_if = NULL;
  __line_if   = NULL;
  __visdisp_if = NULL;
  __robot_drawer = NULL;
  __resolution = 1;
  __rotation = 0;
  __break_drawing = false;
  __first_draw = true;

 __recording = false;
  __visdisp = new VisualDisplay2D();

  add_events(Gdk::SCROLL_MASK | Gdk::BUTTON_MOTION_MASK |
	     Gdk::BUTTON_PRESS_MASK );
  
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  signal_expose_event().connect(sigc::mem_fun(*this, &LaserDrawingArea::on_expose_event));
  signal_button_press_event().connect(sigc::mem_fun(*this, &LaserDrawingArea::on_button_press_event));
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &LaserDrawingArea::on_motion_notify_event));
#endif

  Glib::RefPtr<Gdk::Window> window = get_window();


  __fcd_save = new Gtk::FileChooserDialog("Save Graph",
					  Gtk::FILE_CHOOSER_ACTION_SAVE);
  __fcd_recording = new Gtk::FileChooserDialog("Recording Directory",
					       Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER);

  //Add response buttons the the dialog:
  __fcd_save->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  __fcd_save->add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  __fcd_recording->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  __fcd_recording->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

  __filter_pdf = new Gtk::FileFilter();
  __filter_pdf->set_name("Portable Document Format (PDF)");
  __filter_pdf->add_pattern("*.pdf");
  __filter_svg = new Gtk::FileFilter();
  __filter_svg->set_name("Scalable Vector Graphic (SVG)");
  __filter_svg->add_pattern("*.svg");
  __filter_png = new Gtk::FileFilter();
  __filter_png->set_name("Portable Network Graphic (PNG)");
  __filter_png->add_pattern("*.png");
  __fcd_save->add_filter(*__filter_pdf);
  __fcd_save->add_filter(*__filter_svg);
  __fcd_save->add_filter(*__filter_png);
  __fcd_save->set_filter(*__filter_pdf);

}
#endif

/** Constructor. */
LaserDrawingArea::LaserDrawingArea()
{
  __draw_mode = MODE_LINES;
  __zoom_factor = 50;
  __laser360_if = NULL;
  __laser720_if = NULL;
  __l_objpos_if_persons = NULL;
  __l_objpos_if_legs = NULL;
  __l_objpos_if_misc = NULL;
  __laser_segmentation_if = NULL;
  __l_track_if = NULL;
  __target_if = NULL;
  __switch_if = NULL;
  __line_if   = NULL;
  __visdisp_if = NULL;
  __robot_drawer = NULL;
  __resolution = 1;
  __rotation = 0;
  __break_drawing = false;

  __visdisp = new VisualDisplay2D();

  add_events(Gdk::SCROLL_MASK | Gdk::BUTTON_MOTION_MASK);

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  signal_expose_event().connect(sigc::mem_fun(*this, &LaserDrawingArea::on_expose_event));
  signal_button_press_event().connect(sigc::mem_fun(*this, &LaserDrawingArea::on_button_press_event));
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &LaserDrawingArea::on_motion_notify_event));
#endif

}

/** Constructor. */
LaserDrawingArea::~LaserDrawingArea()
{
  delete __fcd_save;
  delete __fcd_recording;
  delete __filter_pdf;
  delete __filter_svg;
  delete __filter_png;
}


/** Destructor. */
LaserDrawingArea::~LaserDrawingArea()
{
  delete __visdisp;
}

/** Set ObjectPosition interfaces.
 * @param  l_objpos_if_persons list of objectposition interfaces for persons
 * @param  l_objpos_if_legs list of objectposition interfaces for legs
 * @param l_objpos_if_misc list of objectposition interfaces for miscellanous objects
 * @param laser_segmentation_if Laser interface indicating the segmentation-borfers of the legtracker
 * @param l_track_if list of track interfaces
 * @param target_if the current target
 * @param switch_if used to indicate that a drawing-run is finish (so e.g. new data can be sent)
 */
void
LaserDrawingArea::set_objpos_if(std::list<fawkes::ObjectPositionInterface*>* l_objpos_if_persons,
				std::list<fawkes::ObjectPositionInterface*>* l_objpos_if_legs,
				std::list<fawkes::ObjectPositionInterface*>* l_objpos_if_misc,
				fawkes::Laser720Interface* laser_segmentation_if ,
				std::list<fawkes::Position2DTrackInterface*>* l_track_if,
				fawkes::ObjectPositionInterface* target_if,
				fawkes::SwitchInterface* switch_if){
  __l_objpos_if_persons = l_objpos_if_persons;
  __l_objpos_if_legs = l_objpos_if_legs;
  __l_objpos_if_misc = l_objpos_if_misc;
  __laser_segmentation_if=laser_segmentation_if;
  __l_track_if = l_track_if;
  __target_if = target_if;
  __switch_if = switch_if;
}



/** Set 360 degree laser interface.
 * @param laser_if laser interface
 */
void
LaserDrawingArea::set_laser360_if(Laser360Interface *laser_if)
{
  __laser360_if = laser_if;
  __laser720_if = NULL;
}


/** Set 720 degree laser interface.
 * @param laser_if laser interface
 */
void
LaserDrawingArea::set_laser720_if(Laser720Interface *laser_if)
{
  __laser720_if = laser_if;
  __laser360_if = NULL;
}


/** Reset laser interfaces to "no laser available". */
void
LaserDrawingArea::reset_laser_ifs()
{
  __laser360_if = NULL;
  __laser720_if = NULL;
  __l_objpos_if_persons = NULL;
  __l_objpos_if_legs = NULL;
  __l_objpos_if_misc = NULL;
  __laser_segmentation_if = NULL;
  __l_track_if = NULL;
  __target_if = NULL;
  __switch_if = NULL;

  Gtk::Allocation allocation = get_allocation();
  const int width  = allocation.get_width();
  const int height = allocation.get_height();

  __xc = width / 2;
  __yc = height / 2;
  __zoom_factor = 50;

}

/** Set line interface.
 * @param line_if interface to use for line data to draw.
 */
void
LaserDrawingArea::set_line_if(ObjectPositionInterface *line_if)
{
  __line_if = line_if;
}


/** Set visual display interface.
 * @param visdisp_if interface to query for drawing ops
 */
void
LaserDrawingArea::set_visdisp_if(VisualDisplay2DInterface *visdisp_if)
{
  __visdisp_if = visdisp_if;
  __visdisp->set_interface(__visdisp_if);
}


/** Set robot drawer.
 * @param robot_drawer new robot drawer to use
 */
void
LaserDrawingArea::set_robot_drawer(fawkes::CairoRobotDrawer *robot_drawer)
{
  __robot_drawer = robot_drawer;
}

/** Set resolution.
 * Every n'th beam will be drawn where n is the resolution.
 * @param resolution new resolution
 */
void
LaserDrawingArea::set_resolution(unsigned int resolution)
{
  __resolution = resolution;
}


/** Set the drawing mode.
 * @param mode the new drawing mode
 */
void
LaserDrawingArea::set_draw_mode(draw_mode_t mode)
{
  __draw_mode = mode;
  queue_draw();
}

/** Zoom in.
 * Increases zoom factor by 20, no upper limit.
 */
void
LaserDrawingArea::zoom_in()
{
  __zoom_factor += 20;
  queue_draw();
}

/** Zoom out.
 * Decreases zoom factor by 20 with a minimum of 1.
 */
void
LaserDrawingArea::zoom_out()
{
  if ( __zoom_factor > 20 ) {
    __zoom_factor -= 20;
  } else {
    __zoom_factor = 1;
  }
  queue_draw();
}


/** Set rotation.
 * @param rot_rad rotation angle in rad
 */
void
LaserDrawingArea::set_rotation(float rot_rad)
{
  __rotation = rot_rad;
}



/** Expose event handler.
 * @param event event info structure.
 * @return signal return value
 */

bool
LaserDrawingArea::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window) {
    Gtk::Allocation allocation = get_allocation();
    __width = allocation.get_width();
    __height = allocation.get_height();
    
    if(__first_draw)
    {
      __first_draw = false;
    
      // coordinates for the center of the window
      __xc = __width / 2;
      __yc = __height / 2;
    }

  Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
  // clip to the area indicated by the expose event so that we only redraw
  // the portion of the window that needs to be redrawn
  cr->rectangle(event->area.x, event->area.y,
		event->area.width, event->area.height);
  cr->set_source_rgb(1, 1, 1);
  cr->fill_preserve();
  cr->clip();
  
  draw(cr);
  }

  record();

  return true;
}

/** Draw to the cairo-context
 * @param cr refpointer to the cairo-context to be drawn to
 * @return signal return value
 */
bool
LaserDrawingArea::draw(Cairo::RefPtr<Cairo::Context> &cr) 
{

  
  cr->set_line_width(1.0);
  
  cr->set_source_rgb(0, 0, 0);
  //cr->set_source_rgba(0,0,0,1);

  //    __last_xc += __translation_x;
  //    __last_yc += __translation_y;
  cr->translate(__xc, __yc);
  cr->save();
  
  if ( (__laser360_if == NULL) && (__laser720_if == NULL) &&
       __l_objpos_if_legs == NULL && __l_track_if == NULL ){
    //	 (__l_objpos_if_legs->size() == 0 || (*__l_objpos_if_legs->begin()) == NULL )  &&
    //	 (__l_track_if->size() == 0 ||  (*__l_objpos_if_legs->begin()) == NULL )) {
    Cairo::TextExtents te;
    std::string t = "Not connected to BlackBoard";
    cr->set_source_rgb(1, 0, 0);
    cr->set_font_size(20);
    cr->get_text_extents(t, te);
    cr->move_to(- te.width / 2, -te.height / 2);
    cr->show_text(t);
  } else if ( ((__laser360_if && ! __laser360_if->has_writer()) ||
	       (__laser720_if && ! __laser720_if->has_writer()))  &&
	      (__l_objpos_if_legs!= NULL && __l_objpos_if_legs->size() > 0 && !(*__l_objpos_if_legs->begin())->has_writer()) &&
	      (__l_track_if != NULL && __l_track_if->size() > 0 && !(*__l_track_if->begin())->has_writer()) ){
    Cairo::TextExtents te;
    std::string t = "No writer for the interfaces";
    cr->set_source_rgb(1, 0, 0);
    cr->set_font_size(20);
    cr->get_text_extents(t, te);
    cr->move_to(- te.width / 2, -te.height / 2);
    cr->show_text(t);
    
  } else {
    cr->scale(__zoom_factor, __zoom_factor);
    cr->rotate(__rotation);
    cr->set_line_width(1. / __zoom_factor);
    
    
    Glib::RefPtr<Gdk::Window> window = get_window();
    if (__robot_drawer)  __robot_drawer->draw_robot(window,cr);
    
    if ( (__laser360_if &&  __laser360_if->has_writer()) ||
	 (__laser720_if &&  __laser720_if->has_writer())){
      
      if (! __break_drawing) {
	if (__laser360_if)  __laser360_if->read();
	if (__laser720_if)  __laser720_if->read();
      }
      
      draw_beams(cr);
      
      //	draw_segments(cr);
      
      const float radius = 4 / __zoom_factor;
      if (__line_if) {
	__line_if->read();
	cr->rectangle(-__line_if->world_y() - radius * 0.5, -__line_if->world_x() - radius * 0.5, radius, radius);
	cr->rectangle(-__line_if->relative_y() - radius * 0.5, -__line_if->relative_x() - radius * 0.5, radius, radius);
	cr->fill_preserve();
	cr->stroke();
	cr->move_to(-__line_if->world_y(), -__line_if->world_x());
	cr->line_to(-__line_if->relative_y(), -__line_if->relative_x());
	cr->stroke();
      }
    }
    
    cr->restore();

    cr->save();
    cr->rotate(0.5 * M_PI + __rotation);
    cr->scale(-__zoom_factor, __zoom_factor);
    cr->set_line_width(1. / __zoom_factor);
    if (__visdisp_if) {
      __visdisp->process_messages();
      __visdisp->draw(cr);
    }

    const float radius = 0.01;
    if (__line_if) {
      __line_if->read();
      if (__line_if->has_writer() &&
	  __line_if->is_valid() && __line_if->is_visible()) {

	cr->set_source_rgb(1, 0, 0);
	/*
	std::vector<double> dashes(1);
	dashes[0] = 0.1;
	cr->set_dash(dashes, 0);
	*/
	cr->rectangle(__line_if->world_x() - radius * 0.5, __line_if->world_y() - radius * 0.5, radius, radius);
	cr->rectangle(__line_if->relative_x() - radius * 0.5, __line_if->relative_y() - radius * 0.5, radius, radius);
	cr->fill_preserve();
	cr->stroke();
	cr->move_to(__line_if->world_x(), __line_if->world_y());
	cr->line_to(__line_if->relative_x(), __line_if->relative_y());
	cr->stroke();
      }
    }
    cr->restore();
    
    draw_persons_legs(cr);
    // if(__switch_if != NULL && __switch_if->has_writer()){
    // 	SwitchInterface::EnableSwitchMessage *esm = new SwitchInterface::EnableSwitchMessage();
    // 	__switch_if->msgq_enqueue(esm);
    // }
  }
  
  cr->restore();
  
  return true;
}


/** Draw scale box.
 * Draws a circle with a radius of 1m around the robot.
 * @param cr Cairo context to draw to. It is assumed that possible transformations
 * have been setup before.
 */
void
LaserDrawingArea::draw_scalebox(Cairo::RefPtr<Cairo::Context> &cr)
{
  cr->save();
  cr->set_source_rgba(0, 0, 0.8, 0.2);
  cr->arc(0, 0, 1.0, 0, 2 * M_PI);
  cr->stroke();
  cr->restore();
}


/** Draw Beams.
 * Draws the beams as lines, circles or hull, depending on draw mode.
 * @param cr Cairo context to draw to. It is assumed that possible transformations
 * have been setup before.
 */
void
LaserDrawingArea::draw_beams(Cairo::RefPtr<Cairo::Context> &cr)
{
  float *distances = __laser360_if ? __laser360_if->distances() : __laser720_if->distances();
  size_t nd = __laser360_if ? __laser360_if->maxlenof_distances() : __laser720_if->maxlenof_distances();
  const float nd_factor = 360.0 / nd;

  bool clockwise = __laser360_if ? __laser360_if->is_clockwise_angle() : __laser720_if->is_clockwise_angle();

  float *revdists = NULL;
  if (! clockwise) {
    // re-arrange to clockwise
    revdists = (float *)new float[nd];
    for (size_t i = 0; i < nd; ++i) {
      revdists[nd - i] = distances[i];
    }
    distances = revdists;
  }

  cr->scale(__zoom_factor, __zoom_factor);
  cr->rotate(__rotation);
  cr->set_line_width(1. / __zoom_factor);

  draw_scalebox(cr);

  if ( __draw_mode == MODE_LINES ) {
    for (size_t i = 0; i < nd; i += __resolution) {
      if ( distances[i] == 0 )  continue;
      const float anglerad = deg2rad(i * nd_factor);
      cr->move_to(0, 0);
      cr->line_to(distances[i] *  sin(anglerad),
		  distances[i] * -cos(anglerad));
    }
    cr->stroke();
  } else if ( __draw_mode == MODE_POINTS ) {
    const float radius = 4 / __zoom_factor;
    for (size_t i = 0; i < nd; i += __resolution) {
      if ( distances[i] == 0 )  continue;
      float anglerad = deg2rad(i * nd_factor);
      float x = distances[i] *  sin(anglerad);
      float y = distances[i] * -cos(anglerad);
      // circles replaced by rectangles, they are a *lot* faster
      //cr->move_to(x, y);
      //cr->arc(x, y, radius, 0, 2*M_PI);
      cr->rectangle(x, y, radius, radius);
    }
    cr->fill_preserve();
    cr->stroke();
  } else {
    cr->move_to(0, - distances[0]);
    for (size_t i = __resolution; i <= nd + __resolution; i += __resolution) {
      if ( distances[i] == 0 )  continue;
      const float anglerad    = normalize_rad(deg2rad(i * nd_factor));
      cr->line_to(distances[i % nd] *  sin(anglerad),
		  distances[i % nd] * -cos(anglerad));
    }
    cr->stroke();
  }

  if (revdists) delete[] revdists;
}


/** Draw person legs.
 * Draws the legs of persons
 * @param cr Cairo context to draw to. It is assumed that possible transformations
 * have been setup before.
 */
void
LaserDrawingArea::draw_persons_legs(Cairo::RefPtr<Cairo::Context> &cr)
{
  std::list<ObjectPositionInterface*>::iterator objpos_if_itt;;

  cr->save();
  if (__l_objpos_if_persons) {
    cr->set_source_rgb(0,0,1);
    for( objpos_if_itt = __l_objpos_if_persons->begin(); 
	 objpos_if_itt != __l_objpos_if_persons->end()  && (*objpos_if_itt)->has_writer();
	 objpos_if_itt++ ) {
      if(!__break_drawing)
	(*objpos_if_itt)->read();
      if ((*objpos_if_itt)->is_valid()){
	std::pair<float,float> pos = transform_coords_from_fawkes((*objpos_if_itt)->relative_x(), (*objpos_if_itt)->relative_y());
	float x=pos.first;
	float y=pos.second;
	cr->move_to(x, y);
	//      cr->arc(x, y, std::max((*objpos_if_itt)->extent_x(),(*objpos_if_itt)->extent_y()), 0, 2*M_PI);
	cr->arc(x, y, 0.2, 0, 2*M_PI);
      }
    }
    cr->stroke();
  }

  if (__l_objpos_if_legs) {
    cr->set_source_rgb(0,1,0);
    for( objpos_if_itt = __l_objpos_if_legs->begin(); 
	 objpos_if_itt != __l_objpos_if_legs->end() && (*objpos_if_itt)->has_writer() ; 
	 objpos_if_itt++ ) {
      if(!__break_drawing)
	(*objpos_if_itt)->read();
      if ((*objpos_if_itt)->is_valid()){
	std::pair<float,float> pos = transform_coords_from_fawkes((*objpos_if_itt)->relative_x(), (*objpos_if_itt)->relative_y());
	float x=pos.first;
	float y=pos.second;
	cr->move_to(x, y);
	cr->arc(x, y, 0.1, 0, 2*M_PI);
      }
    }
    cr->stroke();
  }
  
  if (__l_objpos_if_misc) {
    cr->set_source_rgb(0,1,1);
    for( objpos_if_itt = __l_objpos_if_misc->begin(); 
	 objpos_if_itt != __l_objpos_if_misc->end() && (*objpos_if_itt)->has_writer() ; 
	 objpos_if_itt++ ) {
      if(!__break_drawing)
	(*objpos_if_itt)->read();
      if ((*objpos_if_itt)->is_valid()){
	//      switch( (*objpos_if_itt)->object_type() ){
	//      case ObjectPositionInterface::TYPE_BALL:
	//TYPE_OPPONENT
	if((*objpos_if_itt)->object_type()==ObjectPositionInterface::TYPE_BALL){
	  std::pair<float,float> pos = transform_coords_from_fawkes((*objpos_if_itt)->relative_x(), (*objpos_if_itt)->relative_y());
	  float x=pos.first;
	  float y=pos.second;
	  pos = transform_coords_from_fawkes((*objpos_if_itt)->world_x(), (*objpos_if_itt)->world_y());
	  float begin_x=pos.first;
	  float begin_y=pos.second;
	  pos = transform_coords_from_fawkes((*objpos_if_itt)->world_x_velocity(), (*objpos_if_itt)->world_y_velocity());
	  float end_x= pos.first;
	  float end_y= pos.first;
	  float angle1=atan2(begin_y- y, begin_x - x);
	  float angle2=atan2(end_y- y, end_x - x);
	  float radius=(*objpos_if_itt)->relative_x_velocity();
	  float probability = (*objpos_if_itt)->relative_z_velocity();
	  cr->move_to(begin_x, begin_y);
	  cr->arc(x, y, radius, angle2, angle1);

	  //	Cairo::TextExtents te;
	  std::string t = StringConversions::to_string(probability);
	  t.erase(5);
	  //	  cr->set_source_rgb(0,1 ,1);
	  cr->set_font_size(0.08);
	  //	cr->get_text_extents(t, te);
	  //	cr->move_to(- te.width / 2, -te.height / 2);
	  cr->move_to(begin_x, begin_y);
	  cr->show_text(t);
	  //	  cr->set_source_rgb(0,0,1);
	
	  //	break;
	  //      case ObjectPositionInterface::TYPE_LINE:
	}else if((*objpos_if_itt)->object_type()==ObjectPositionInterface::TYPE_LINE){
	  std::pair<float,float> pos = transform_coords_from_fawkes((*objpos_if_itt)->world_x(), (*objpos_if_itt)->world_y());
	  float begin_x=pos.first;
	  float begin_y=pos.second;
	  pos = transform_coords_from_fawkes((*objpos_if_itt)->world_x_velocity(), (*objpos_if_itt)->world_y_velocity());
	  float end_x= pos.first;
	  float end_y= pos.first;
	  cr->move_to(begin_x, begin_y);
	  cr->line_to(end_x, end_y);
	  //break;
	}
      }
    }
    //  cr->fill_preserve();
    cr->stroke();
  }

  cr->set_source_rgb(1,0,1);

  float r,g,b;
  r=g=b=0.0;
  int color_it=0;
  float delta = 0.25;


  if (__l_track_if) {

    double original_line_width = cr->get_line_width();
    std::list<Position2DTrackInterface*>::iterator track_if_itt;;  
    const float radius (0.07);
    float* x_positions1 (NULL) ;
    float* y_positions1 (NULL);
    int* timestamps1 (NULL);
    float* x_positions2 (NULL);
    float* y_positions2 (NULL);
    unsigned int track_length1(0);
    unsigned int track_length2(0);
    int* timestamps2 (NULL);
    unsigned int id;
    char single_char_idx = 'a';
    char compound_char_idx = 'A';
    
#ifdef LASERGUI_DEBUG_PRINT_TRACKS
    printf("\n\n################################\n");
#endif
    for( track_if_itt = __l_track_if->begin(); 
	 track_if_itt != __l_track_if->end() && (*track_if_itt)->has_writer();) {
      cr->set_font_size(0.03);
      bool b_compound_track(false);
      if(!__break_drawing)
	(*track_if_itt)->read();
      if ((*track_if_itt)->is_valid()){
	x_positions1=(*track_if_itt)->track_x_positions();
	y_positions1=(*track_if_itt)->track_y_positions();
	timestamps1=(*track_if_itt)->track_timestamps();
	track_length1 = (*track_if_itt)->length();
	id = (*track_if_itt)->track_id();
	++track_if_itt;
#ifdef LASERGUI_DEBUG_PRINT_TRACKS
	printf("\n    trackid %d\n", id);
#endif
	if( track_if_itt != __l_track_if->end() && (*track_if_itt)->has_writer()){
	  if(!__break_drawing)
	    (*track_if_itt)->read();
	  if( (*track_if_itt)->is_valid() && (*track_if_itt)->track_id() == id ){
#ifdef LASERGUI_DEBUG_PRINT_TRACKS
	    printf("compound!\n");
#endif
	    b_compound_track = true;
	    x_positions2=(*track_if_itt)->track_x_positions();
	    y_positions2=(*track_if_itt)->track_y_positions();
	    timestamps2=(*track_if_itt)->track_timestamps();
	    track_length2 = (*track_if_itt)->length();
	    ++track_if_itt;
	  }
	}



	/*	if(b_compound_track){
	  while(j < track_length2 && timestamps2[j] < timestamps1[i]){
	    ++j;
	  }
	  if(j  <  track_length2 && timestamps2[j] == timestamps1[i]){
	    x = (x_positions1[i] + x_positions2[j])/2.0;
	    y = (y_positions1[i] + y_positions2[j])/2.0;
	  }
	  }*/
	float x,y;
	// Draw last elements of each Obs-Side
	std::string t = "S";
	if(b_compound_track){
	  t = "C";
	  cr->set_source_rgb(1,0,0);	 
	}
	else
	  cr->set_source_rgb(0,0,1);	 	  


	x = x_positions1[track_length1 - 1];
	y = y_positions1[track_length1 - 1];
	std::pair<float,float> pos = transform_coords_from_fawkes(x,y);
	cr->move_to(pos.first + radius, pos.second);
	cr->arc(pos.first, pos.second, radius, 0, 2*M_PI);
	cr->move_to(pos.first - (radius * 1/2) , pos.second   + (radius * 2/3) );
	cr->set_font_size(0.12);	  
	cr->show_text(t);
	cr->move_to(pos.first, pos.second);

	if(b_compound_track){
	  x = x_positions2[track_length2 - 1];
	  y = y_positions2[track_length2 - 1];
	  pos = transform_coords_from_fawkes(x,y);
	  /*	  if( last_compound_position.second == (int) track_length2-1 &&  last_compound_position.first == (int) track_length1-1 )
	    cr->line_to(pos.first, pos.second);
	  */
	  cr->move_to(pos.first + radius, pos.second);
	  cr->arc(pos.first, pos.second, radius, 0, 2*M_PI);
	  cr->move_to(pos.first - (radius * 1/2) , pos.second   + (radius * 2/3) );
	  cr->set_font_size(0.12);	  
	  cr->show_text(t);
	  cr->move_to(pos.first, pos.second);
	}

	  


	cr->set_line_width(original_line_width * 2.5);
	//	printf("line width orig: %f, new%f\n", original_line_width, cr->get_line_width());
	cr->stroke();
	cr->set_line_width(original_line_width);
	cr->set_font_size(0.03);


	// chose color    
	if (div(color_it,3).rem == 0) r+= delta;
	if (div(color_it,3).rem == 1) g+= delta;
	if (div(color_it,3).rem == 2) b+= delta;
	cr->set_source_rgb(r,g,b);
	std::pair<int,int> last_compound_position(-1,-1);

	char char_idx = b_compound_track ? compound_char_idx++ : single_char_idx++ ;
	//  Draw Track-Frist-Obsside
 	for(unsigned int i=1; i < track_length1;++i){
	  x = x_positions1[i];
	  y = y_positions1[i];
	  std::pair<float,float> pos = transform_coords_from_fawkes(x,y);
	  if(i==0)
	    cr->move_to(pos.first, pos.second);
	  else
	    cr->line_to(pos.first, pos.second);
	  std::string t = std::string(1,char_idx) + "¹" + StringConversions::to_string(timestamps1[i]);
	  cr->show_text(t);
	  cr->move_to(pos.first, pos.second);
#ifdef LASERGUI_DEBUG_PRINT_TRACKS
	  printf("( %f,%f,[%d] )", pos.first, pos.second, timestamps1[i] );
#endif
	}
	//  Draw Track-Second-Obsside
	//	char_idx = b_compound_track ? compound_char_idx++ : single_char_idx++ ;
 	for(unsigned int j=0; j < track_length2;++j){
	  x = x_positions2[j];
	  y = y_positions2[j];
	  std::pair<float,float> pos = transform_coords_from_fawkes(x,y);
	  if(j==0)
	    cr->move_to(pos.first, pos.second);
	  else
	    cr->line_to(pos.first, pos.second);
	  std::string t = std::string(1,char_idx) + "²" + StringConversions::to_string(timestamps2[j]);
	  cr->show_text(t);
	  cr->move_to(pos.first, pos.second);
#ifdef LASERGUI_DEBUG_PRINT_TRACKS
	  printf("( %f,%f,[%d] )", pos.first, pos.second, timestamps2[j] );
#endif
	}

	color_it++;
	cr->stroke();


	if(b_compound_track){
	  // Draw connections of compound-trackelements
	  unsigned int i(0);
	  unsigned int j(0);
	  bool b_last_was_track1(false);
	  bool b_last_was_track2(false);
	  
	  while(i < track_length1 || j < track_length2){
	    if(j >= track_length2 || (i < track_length1 && timestamps1[i] < timestamps2[j])){
	      if(b_last_was_track2 && !b_last_was_track2){
		std::pair<float,float> pos = transform_coords_from_fawkes(x_positions2[j-1],y_positions2[j-1]);
		cr->move_to(pos.first, pos.second);
		pos = transform_coords_from_fawkes(x_positions1[i],y_positions1[i]);
		cr->line_to(pos.first, pos.second);
	      }
	      b_last_was_track1 = true;
	      b_last_was_track2 = false;
	      ++i;
	      continue;
	    }
	    if(i >= track_length1 || (j < track_length2 && timestamps1[i] > timestamps2[j])){
	      if(b_last_was_track1 && !b_last_was_track2){
		std::pair<float,float> pos = transform_coords_from_fawkes(x_positions1[i-1],y_positions1[i-1]);
		cr->move_to(pos.first, pos.second);
		pos = transform_coords_from_fawkes(x_positions2[j],y_positions2[j]);
		cr->line_to(pos.first, pos.second);
	      }
	      b_last_was_track2 = true;
	      b_last_was_track1 = false;
	      ++j;
	      continue;
	    }
	    else if(timestamps1[i] == timestamps2[j]){
	      std::pair<float,float> pos = transform_coords_from_fawkes(x_positions1[i],y_positions1[i]);
	      cr->move_to(pos.first, pos.second);
	      pos = transform_coords_from_fawkes(x_positions2[j],y_positions2[j]);
	      cr->line_to(pos.first, pos.second);
	      
	      last_compound_position.first = (int) i;
	      last_compound_position.second = (int) j;
	      b_last_was_track2 = true;
	      b_last_was_track1 = true;
	      ++i;
	      ++j;
	    }
	  }
	  cr->set_source_rgb(0.9,0.9,0.9);
	  cr->set_line_width(original_line_width * 0.7);
	  cr->stroke();
	}



	/*
	i = std::max(0, (int) track_length1 - CFG_PRINT_NR_TRACKELEMENTS);
	j = 0;
	for (; i < track_length1; ++i){
	  x = x_positions1[i];
	  y = y_positions1[i];
	  if(b_compound_track){
	    while(j+1 < track_length2 && timestamps2[j] < timestamps1[i]){
	      ++j;
	    }
	  }
	  
	  if( !b_compound_track || i < track_length1 - 1 || timestamps2[j] != timestamps1[i]){
	    std::pair<float,float> pos = transform_coords_from_fawkes(x_positions1[i],y_positions1[i]);
	    cr->move_to(pos.first + radius, pos.second);
	    cr->arc(pos.first, pos.second, radius, 0, 2*M_PI);
	    
	    std::string t = "S";
	    if(b_compound_track)
	      t = "C";
	    
	    cr->move_to(pos.first - (radius * 1/2) , pos.second   + (radius * 2/3) );
	    cr->set_font_size(0.12);	  
	    cr->show_text(t);

	    
	    if(b_compound_track && timestamps2[j] == timestamps1[i]){
	      cr->move_to(pos.first, pos.second);
	      
	      std::pair<float,float> pos = transform_coords_from_fawkes(x_positions2[j],y_positions2[j]);
	      cr->line_to(pos.first, pos.second);
	      cr->move_to(pos.first + radius, pos.second);
	      cr->arc(pos.first, pos.second, radius, 0, 2*M_PI);

	      cr->move_to(pos.first - (radius * 1/2) , pos.second   + (radius * 2/3) );
	      t = "C2";
	      cr->show_text(t);
	      
	    }
	  }
	}
	double original_line_width = cr->get_line_width();
	cr->set_line_width(original_line_width * 2.5);
	//	printf("line width orig: %f, new%f\n", original_line_width, cr->get_line_width());
	cr->stroke();
	
	if ( last_compound_position.first != -1 ){
	  cr->set_source_rgb(1,0,0);
	  x = x_positions1[track_length1 - 1];
	  y = y_positions1[track_length1 - 1];
	  
	  std::pair<float,float> pos = transform_coords_from_fawkes(x_positions1[last_compound_position.first],y_positions1[last_compound_position.first]);
	  cr->move_to(pos.first + radius, pos.second);
	  cr->arc(pos.first, pos.second, radius, 0, 2*M_PI);
	

	  cr->move_to(pos.first - (radius * 1/2) , pos.second   + (radius * 2/3) );
	  cr->set_font_size(0.12);	  
	  std::string t = "C";
	  cr->show_text(t);

	  cr->move_to(pos.first, pos.second);
	  
	  pos = transform_coords_from_fawkes(x_positions2[last_compound_position.second],y_positions2[last_compound_position.second]);
	  cr->line_to(pos.first, pos.second);
	  cr->move_to(pos.first + radius, pos.second);
	  cr->arc(pos.first, pos.second, radius, 0, 2*M_PI);

	  cr->move_to(pos.first - (radius * 1/2) , pos.second   + (radius * 2/3) );
	  cr->set_font_size(0.12);	  
	  cr->show_text(t);

	  cr->stroke();
	}
	cr->set_line_width(original_line_width);
*/

	
      }
      else{
	break;
      }
    }
  }
  
  /*  DRAW TARGET */
  if(__target_if && __target_if->has_writer()){
    __target_if->read();
    if(__target_if->is_valid()){
      cr->set_source_rgb(1,0,0);
      std::pair<float,float> pos = transform_coords_from_fawkes(__target_if->relative_x(), __target_if->relative_y());
      float x=pos.first;
      float y=pos.second;
      float radius = 0.1;

      cr->move_to(x, y);
      cr->arc(x, y, radius, 0, 2*M_PI);
      cr->move_to(x - radius, y);
      cr->line_to(x + radius, y);
      cr->move_to(x, y - radius );
      cr->line_to(x, y + radius);
      cr->stroke();
    }
  }


  /*
  float r,g,b;
  r=g=b=0.0;
  float delta = 0.2;
  for (int i = 0; i< 15 ; i++){
    
    if (div(i,3).rem == 0) r+= delta;
    if (div(i,3).rem == 1) g+= delta;
    if (div(i,3).rem == 2) b+= delta;
    //    printf("i %d rem %d| r %f, g %f, b %f\n", i, div(i,3).rem,r,g,b);
    cr->move_to(0, (i+1)*0.125);
    cr->set_source_rgb(r,g,b);
    cr->rectangle(0, (i+1)*0.125, 0.1 , 0.1 );
    cr->fill_preserve();
    cr->stroke();
  }
  */
  //  cr->stroke();

  cr->restore();
}


/** Draw laser segments as produced by leg tracker application.
 * @param cr Cairo context to draw to. It is assumed that possible transformations
 * have been setup before.
 */
void
LaserDrawingArea::draw_segments(Cairo::RefPtr<Cairo::Context> &cr)
{
  
  
  /* DRAW SEGMENTS (draw the segment interiors again with other color*/
  if( __laser_segmentation_if && __laser_segmentation_if->has_writer()){

    float *distances = __laser360_if ? __laser360_if->distances() : __laser720_if->distances();
    size_t nd = __laser_segmentation_if->maxlenof_distances();
    const float nd_factor = 360.0 / nd;    
    cr->save();


    if(!__break_drawing)
      __laser_segmentation_if->read();
    float * segmentations = __laser_segmentation_if->distances();
    //	cr->set_source_rgba(0,0,0,0.5);
    cr->set_source_rgb(1,1,0);

    if ( __draw_mode == MODE_POINTS ) {
      for (size_t i = 0; i < nd; i += __resolution) {
	if( segmentations[i]==0) continue;  // dont draw the segment borders
	if ( distances[i] == 0 )  continue;
	float anglerad = deg2rad(i * nd_factor);
	cr->move_to(0, 0);
	cr->line_to(distances[i] *  sin(anglerad),
		    distances[i] * -cos(anglerad));
      }
      cr->stroke();
    } else {//if ( __draw_mode == MODE_LINES ) {
      float radius = 4 / __zoom_factor;
      for (size_t i = 0; i < nd; i += __resolution) {
	if( segmentations[i]==0) continue;  // dont draw the segment borders
	if ( distances[i] == 0 )  continue;
	float anglerad = deg2rad(i * nd_factor);
	float x = distances[i] *  sin(anglerad);
	float y = distances[i] * -cos(anglerad);
	// circles replaced by rectangles, they are a *lot* faster
	//cr->move_to(x, y);
	//cr->arc(x, y, radius, 0, 2*M_PI);
	cr->rectangle(x, y, radius, radius);
      }
      cr->fill_preserve();
      cr->stroke();
    } 
    /*else {
      cr->move_to(0, - distances[0]);
      for (size_t i = __resolution; i <= nd + __resolution; i += __resolution) {
      if ( distances[i] == 0 )  continue;
      float anglerad    = deg2rad(i % 360);
      cr->line_to(distances[i % 360] *  sin(anglerad),
      distances[i % 360] * -cos(anglerad));
      }
      cr->stroke();
      }
    */
  }
  cr->restore();
}


/** Scroll event handler.
 * @param event event structure
 * @return signal return value
 */
bool
LaserDrawingArea::on_scroll_event(GdkEventScroll *event)
{
  if (event->direction == GDK_SCROLL_UP) {
    zoom_in();
  } else if (event->direction == GDK_SCROLL_DOWN) {
    zoom_out();
  }
  return true;
}

/** Set a member for breaking the drawing. */
void
LaserDrawingArea::toggle_break_drawing()
{
  __break_drawing = ! __break_drawing;
}



/** Button press event handler.
 * @param event event data
 * @return true
 */
bool
LaserDrawingArea::on_button_press_event(GdkEventButton *event)
{
  __last_mouse_x = event->x;
  __last_mouse_y = event->y;
  return true;
}


/** Mouse motion notify event handler.
 * @param event event data
 * @return true
 */
bool
LaserDrawingArea::on_motion_notify_event(GdkEventMotion *event)
{
  //  d__translation_x -= __last_mouse_x - event->x;
  //  double __translation_y -= __last_mouse_y - event->y;
  __xc -= __last_mouse_x - event->x;
  __yc -= __last_mouse_y - event->y;

  __last_mouse_x = event->x;
  __last_mouse_y = event->y;
  queue_draw();
  return true;
}



/**
 * Transform a position from the fawkes coordinate system to the Cairo
 * coordinate system.
 * @param p_x input x 
 * @param p_y input y
 * @return the transformed position
 */
std::pair<float,float>
LaserDrawingArea::transform_coords_from_fawkes(float p_x, float p_y){
  std::pair<float,float> pos;
  pos.first =  -p_y ;
  pos.second=  -p_x ;
  return pos;
}








/** Enable/disable recording.
 * @param recording true to enable recording, false otherwise
 * @return true if recording is enabled now, false if it is disabled.
 * Enabling the recording may fail for example if the user chose to abort
 * the directory creation process.
 */
bool
LaserDrawingArea::set_recording(bool recording)
{
  
  if (recording) {
    Gtk::Window *w = dynamic_cast<Gtk::Window *>(get_toplevel());
    __fcd_recording->set_transient_for(*w);
    int result = __fcd_recording->run();
    if (result == Gtk::RESPONSE_OK) {
      __record_directory = __fcd_recording->get_filename();
      __recording = true;
    }
    __fcd_recording->hide();
  } else {
    __recording = false;
  }
  return __recording;
 
  return true;
}



/** save current figure to pdf,svg or png. */
void
LaserDrawingArea::save()
{
  Gtk::Allocation allocation = get_allocation();
  __width = allocation.get_width();
  __height = allocation.get_height();

  Gtk::Window *w = dynamic_cast<Gtk::Window *>(get_toplevel());
  __fcd_save->set_transient_for(*w);

  int result = __fcd_save->run();
  if (result == Gtk::RESPONSE_OK) {
    
    Gtk::FileFilter *f = __fcd_save->get_filter();
    std::string filename = __fcd_save->get_filename();
    if (filename != "") {
      Cairo::RefPtr<Cairo::Surface> surface;
      
      bool write_to_png = false;
      if (f == __filter_pdf) {
	surface = Cairo::PdfSurface::create(filename, __width, __height);
      } else if (f == __filter_svg) {
	surface = Cairo::SvgSurface::create(filename, __width, __height);
      } else if (f == __filter_png) {
	surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
					      (int)ceilf(__width),
					      (int)ceilf(__height));
	write_to_png = true;
      }



      if (surface) {
	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
	draw(cr);
	
	if (write_to_png) {
	  surface->write_to_png(filename);
	}
	
	cr.clear();
      }
      
    } else {
      Gtk::MessageDialog md(*w, "Invalid filename",
			    /* markup */ false, Gtk::MESSAGE_ERROR,
			    Gtk::BUTTONS_OK, /* modal */ true);
      md.set_title("Invalid File Name");
      md.run();
    }
  }

  __fcd_save->hide();
}



/** record current figure as png to a folder prespecified */
void
LaserDrawingArea::record()
{
  Gtk::Allocation allocation = get_allocation();
  __width = allocation.get_width();
  __height = allocation.get_height();


  if ( __recording ) {
    char *filename;
    timespec t;
    if (clock_gettime(CLOCK_REALTIME, &t) == 0) {
      struct tm tms;
      localtime_r(&t.tv_sec, &tms);

      if ( asprintf(&filename, "%s/%04i%02i%02i-%02i%02i%02i.%09li.png",
		    __record_directory.c_str(),
		    tms.tm_year + 1900, tms.tm_mon + 1, tms.tm_mday,
		    tms.tm_hour, tms.tm_min, tms.tm_sec, t.tv_nsec) != -1) {

	//printf("Would record to filename %s\n", filename);
	
	Cairo::RefPtr<Cairo::Surface> surface  = 
	  Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
				      (int)ceilf(__width),
				      (int)ceilf(__height));
	
	if (surface) {
	  Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
	  draw(cr);
	  surface->write_to_png(filename);
	  cr.clear();
	}
	
	free(filename);
      } else {
	printf("Warning: Could not create file name for recording, skipping figure\n");
      }
    } else {
      printf("Warning: Could not time recording, skipping graph\n");
    }
  }


}
