
/***************************************************************************
 *  or.cpp - Implementation for "or'ing" images together
 *
 *  Generated: Fri May 13 14:57:10 2005
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

#include "filters/or.h"

#include <cstddef>
#include <ippi.h>

/** @class FilterOr <filters/or.h>
 * Or filter.
 */

/** Constructor. */
FilterOr::FilterOr()
{
  src[0] = src[1] = dst = NULL;
  src_roi[0] = src_roi[1] = dst_roi = NULL;
}


void
FilterOr::setSrcBuffer(unsigned char *buf, ROI *roi, orientation_t ori, unsigned int buffer_num)
{
  if (buffer_num >= 2) {
    buffer_num = 0;
    // cout << "FilterOr: Warning, buffer_num in setSrcBuffer() out of range, assuming 0." << endl;
  }

  src[buffer_num] = buf;
  src_roi[buffer_num] = roi;

}

void
FilterOr::setSrcBuffer(unsigned char *buf, ROI *roi, unsigned int buffer_num)
{
  if (buffer_num >= 2) {
    buffer_num = 0;
    // cout << "FilterOr: Warning, buffer_num in setSrcBuffer() out of range, assuming 0." << endl;
  }

  src[buffer_num] = buf;
  src_roi[buffer_num] = roi;

}

void
FilterOr::setDstBuffer(unsigned char *buf, ROI *roi, orientation_t ori)
{
  dst = buf;
  dst_roi = roi;
}

void
FilterOr::setOrientation(orientation_t ori)
{
}


const char *
FilterOr::getName()
{
  return "FilterOr";
}


void
FilterOr::apply()
{
  IppiSize size;
  size.width = src_roi[0]->width;
  size.height = src_roi[0]->height;

  IppStatus status;

  if ( (dst == NULL) || (dst == src[1]) ) {
    // In-place
    status = ippiOr_8u_C1IR(src[0] + (src_roi[0]->start.y * src_roi[0]->line_step) + (src_roi[0]->start.x * src_roi[0]->pixel_step),
			    src_roi[0]->line_step,
			    src[1] + (src_roi[1]->start.y * src_roi[1]->line_step) + (src_roi[1]->start.x * src_roi[1]->pixel_step),
			    src_roi[1]->line_step,
			    size);
    
  } else {
    status = ippiOr_8u_C1R(src[0] + (src_roi[0]->start.y * src_roi[0]->line_step) + (src_roi[0]->start.x * src_roi[0]->pixel_step),
			   src_roi[0]->line_step,
			   src[1] + (src_roi[1]->start.y * src_roi[1]->line_step) + (src_roi[1]->start.x * src_roi[1]->pixel_step),
			   src_roi[1]->line_step,
			   dst + (dst_roi->start.y * dst_roi->line_step) + (dst_roi->start.x * dst_roi->pixel_step),
			   dst_roi->line_step,
			   size);
  }

  /*
  cout << "FilterOr: ippiFilterOr exit code: " << flush;
  switch (status) {
  case ippStsNoErr:
    cout << "ippStsNoErr";
    break;
  case ippStsNullPtrErr:
    cout << "ippStsNullPtrErr";
    break;
  case ippStsSizeErr:
    cout << "ippStsSizeErr";
    break;
  case ippStsStepErr:
    cout << "ippStsStepErr";
    break;
  case ippStsMaskSizeErr:
    cout << "ippStsMaskSizeErr";
    break;
  default:
    cout << "Unknown status";
  }
  cout << endl;
  */
}