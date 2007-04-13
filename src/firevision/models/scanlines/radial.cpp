
/***************************************************************************
 *  radial.cpp - Implementation of the radial scanline model
 *
 *  Generated: Tue Jul 19 12:46:52 2005
 *  Copyright  2005  Tim Niemueller [www.niemueller.de]
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

#include <models/scanlines/radial.h>

#include <utils/system/console_colors.h>

#include <cmath>

/** @class ScanlineRadial <models/scanlines/radial.h>
 * Radial scanlines.
 * Meant to be used with omnivision.
 */

/** Constructor.
 * @param width image width
 * @param height image height
 * @param center_x radial center center x
 * @param center_y radial center center y
 * @param radius_increment radius increment
 * @param step step
 * @param dead_radius inner radius to ignore
 */
ScanlineRadial::ScanlineRadial(unsigned int width, unsigned int height,
			       unsigned int center_x, unsigned int center_y,
			       unsigned int radius_increment,
			       unsigned int step,
			       unsigned int dead_radius
			       )
{
  this->width            = width;
  this->height           = height;
  this->center_x         = center_x;
  this->center_y         = center_y;
  this->radius_increment = radius_increment;
  this->step             = step;
  this->dead_radius      = dead_radius;

  reset();
}

point_t
ScanlineRadial::operator*()
{
  return coord;
}

point_t*
ScanlineRadial::operator->()
{
  return &coord;
}

point_t *
ScanlineRadial::operator++()
{

  if ( done ) return &coord;

  bool ok = false;

  do {

    tmp_x = 0;
    tmp_y = 0;

    if ( current_radius == 0 ) {
      // Special case, after first reset
      current_radius += radius_increment;
      x = 0;
      y = current_radius;
      ok = true;
    } else {

      if ( x < y ) {

	switch (sector) {
	case 0:
	  tmp_x = x;
	  tmp_y = -y;
	  break;

	case 1:
	  tmp_x = y;
	  tmp_y = -x;
	  break;

	case 2:
	  tmp_x = y;
	  tmp_y = x;
	  break;

	case 3:
	  tmp_x = x;
	  tmp_y = y;
	  break;

	case 4:
	  tmp_x = -x;
	  tmp_y = y;
	  break;

	case 5:
	  tmp_x = -y;
	  tmp_y = x;
	  break;

	case 6:
	  tmp_x = -y;
	  tmp_y = -x;
	  break;

	case 7:
	  tmp_x = -x;
	  tmp_y = -y;
	  break;

	default:
	  tmp_x = 0;
	  tmp_y = 0;
	  break;

	}

	x += step;
	y = (int)(sqrt( (float(current_radius * current_radius) - float(x * x)) ) + 0.5);

	ok = true;

      } else {
	//      cout << "x !< y" << endl;
	if (sector == 7) {
	  // Need to go to next circle
	  current_radius += radius_increment;
	  x = 0;
	  y = current_radius;
	  sector = 0;
	  if (current_radius >= max_radius) { done = true; ok = true; }
	} else {
	  sector += 1;
	  x = 0;
	  y = current_radius;
	}
      }

    }

    if ( (tmp_x < -(int)center_x) ||
	 (tmp_x > (int)(width - center_x)) ||
	 (tmp_y < -(int)center_y) ||
	 (tmp_y > (int)(height - center_y))
	 ) {
      coord.x = 0;
      coord.y = 0;
      // out of image, not ok
      ok = false;
      //done = true;
    } else {
      coord.x = center_x + tmp_x;
      coord.y = center_y + tmp_y;
    }

  } while (! ok);

  return &coord;
}

point_t *
ScanlineRadial::operator++(int)
{
  memcpy(&tmp_coord, &coord, sizeof(point_t));
  return &tmp_coord;
}

bool
ScanlineRadial::finished()
{
  return done;
}


/** Do a simple sort of the given array, sorted descending, biggest first
 * this sort is stable
 */
void
ScanlineRadial::simpleBubbleSort(unsigned int array[], unsigned int num_elements)
{
  bool modified = false;
  unsigned int end = num_elements;
  unsigned int tmp;
  do {
    modified = false;

    for (unsigned int i = 0; i < end-1; ++i) {
      if ( array[i] < array[i+1] ) {
	tmp        = array[i];
	array[i]   = array[i+1];
	array[i+1] = tmp;
	end -= 1;
	modified = true;
      }
    }

  } while ( modified );
}

void
ScanlineRadial::reset()
{
  current_radius = radius_increment;
  while (current_radius < dead_radius) {
    current_radius += radius_increment;
  }
  x = 0;
  y = current_radius;
  sector = 0;

  coord.x = center_x;
  coord.y = center_y;

  // Calculate distances to corners of image
  unsigned int dists[4];
  dists[0] = (unsigned int)sqrt( float(center_x * center_x) + float(center_y * center_y) );
  dists[1] = (unsigned int)sqrt( float((width - center_x) * (width - center_x)) + float(center_y * center_y) );
  dists[2] = (unsigned int)sqrt( float((width - center_x) * (width - center_x)) + float((height - center_y) * (height - center_y)) );
  dists[3] = (unsigned int)sqrt( float(center_x * center_x) + float((height - center_y) * (height - center_y)) );

  // now the maximum corner distance is the maximum radius
  simpleBubbleSort(dists, 4);
  max_radius = dists[0] - 1;

  done = false;

  if (radius_increment > max_radius) {
    // cout << msg_prefix << cred << "radius_increment > max_radius, resetting radius_increment to one!" << cnormal << endl;
    radius_increment = 1;
  }

  if (dead_radius > max_radius) {
    // cout << msg_prefix << cred << "dead_radius > max_radius, resetting dead_radius to zero!" << cnormal << endl;
    dead_radius = 0;
    current_radius = radius_increment;
  }

}

const char *
ScanlineRadial::getName()
{
  return "ScanlineModel::Radial";
}


unsigned int
ScanlineRadial::getMargin()
{
  return radius_increment;
}