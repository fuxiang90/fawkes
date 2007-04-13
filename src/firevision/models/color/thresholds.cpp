
/***************************************************************************
 *  thresholds.cpp - Implementation of a thresholds color model
 *
 *  Generated: Wed May 18 13:59:18 2005
 *  Copyright  2005  Tim Niemueller  [www.niemueller.de]
 *                   Matrin Heracles <martin.heracles@rwth-aachen.de>
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

#include <iostream>

#include "models/color/thresholds.h"

using namespace std;

/** @class ColorModelThresholds <models/color/thresholds.h>
 * Really simple thresholds-based model with some hard-coded thresholds. Was
 * just for initial development of color models.
 */

color_t
ColorModelThresholds::determine(unsigned int y,
				unsigned int u,
				unsigned int v) const
{
  if ( y >= THRESHOLD_WHITE_Y_LOW) {
    return C_WHITE;
  }
  if ( u <= THRESHOLD_GREEN_U_HIGH &&
       v <= THRESHOLD_GREEN_V_HIGH) {
    return C_GREEN;
  }
  else if (THRESHOLD_ORANGE_U_LOW <= u &&
	   u <= THRESHOLD_ORANGE_U_HIGH &&
	   v >= THRESHOLD_ORANGE_V_LOW) {
    return C_ORANGE;
  }
  else if (u >= THRESHOLD_BLUE_U_LOW &&
	   v <= THRESHOLD_BLUE_V_HIGH) {
    return C_BLUE;
  }
  else if (u <= THRESHOLD_YELLOW_U_HIGH &&
	   v >= THRESHOLD_YELLOW_V_LOW) {
    return C_YELLOW;
  }
  else if (u >= THRESHOLD_MAGENTA_U_LOW &&
	   v >= THRESHOLD_MAGENTA_V_LOW) {
    return C_MAGENTA;
  }
  else if (THRESHOLD_CYAN_U_LOW <= u &&
	   u <= THRESHOLD_CYAN_U_HIGH &&
	   v <= THRESHOLD_CYAN_V_HIGH) {
    return C_CYAN;
  }
  else {
    return C_OTHER;
  }
}

const char *
ColorModelThresholds::getName()
{
  return "ColorModelThresholds";
}


/** Print the thresholds to stdout.
 */
void
ColorModelThresholds::printThresholds()
{
  cout << "ColorModelThresholds" << endl
       << "==========================================================" << endl
       << "Orange:  u_low=" << THRESHOLD_ORANGE_U_LOW
       << "  u_high=" << THRESHOLD_ORANGE_U_HIGH
       << " v_low=" << THRESHOLD_ORANGE_V_LOW
       << endl
       << "Yellow:  u_high=" << THRESHOLD_YELLOW_U_HIGH
       << " v_low=" << THRESHOLD_YELLOW_V_LOW
       << endl;
}