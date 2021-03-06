
/***************************************************************************
 *  min_circle.cpp - Laser data min circle data filter
 *
 *  Created: Sat Feb 19 00:23:27 2011
 *  Copyright  2006-2011  Tim Niemueller [www.niemueller.de]
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

#include "min_circle.h"

#include <utils/math/angle.h>
#include <cstdlib>

/** @class LaserMinCircleDataFilter "min_circle.h"
 * Erase beams below a certain minimum distance distance.
 * All beams shorter than a given radius are erase (set to 0).
 * @author Tim Niemueller
 */

/** Constructor.
 * @param radius radius of cut-off circle in meters
 * @param in_data_size number of entries input value arrays
 * @param in vector of input arrays
 */
LaserMinCircleDataFilter::LaserMinCircleDataFilter(float radius,
						   unsigned int in_data_size,
						   std::vector<float *> in)
  : LaserDataFilter(in_data_size, in, in.size())
{
  __radius = radius;
}


void
LaserMinCircleDataFilter::filter()
{
  const unsigned int vecsize = std::min(in.size(), out.size());
  const unsigned int arrsize = std::min(in_data_size, out_data_size);
  for (unsigned int a = 0; a < vecsize; ++a) {
    float *inbuf  = in[a];
    float *outbuf = out[a];
    for (unsigned int i = 0; i < arrsize; ++i) {
      if (inbuf[i] < __radius) {
	outbuf[i] = 0;
      } else {
	outbuf[i] = inbuf[i];
      }
    }
  }
}
