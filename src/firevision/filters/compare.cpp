
/***************************************************************************
 *  compare.cpp - implementation of compare filter
 *
 *  Generated: Mon Jun 05 16:57:57 2006
 *  Copyright  2005-2006  Tim Niemueller [www.niemueller.de]
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

#include <filters/compare.h>

#include <fvutils/color/yuv.h>
#include <iostream>

/** Background image. */
const unsigned int FilterCompare::BACKGROUND = 0;
/** Foreground image. */
const unsigned int FilterCompare::FOREGROUND = 1;

/** @class FilterCompare <filters/compare.h>
 * Comparison filter.
 */ 

/** Constructor. */
FilterCompare::FilterCompare()
{
  src[BACKGROUND] = src[FOREGROUND] = dst = NULL;
  src_roi[BACKGROUND] = src_roi[FOREGROUND] = dst_roi = NULL;
}


void
FilterCompare::setSrcBuffer(unsigned char *buf, ROI *roi,
			     orientation_t ori, unsigned int buffer_num)
{
  if ( buffer_num < 2 ) {
    src[buffer_num] = buf;
    src_roi[buffer_num] = roi;
  }
}


void
FilterCompare::setSrcBuffer(unsigned char *buf, ROI *roi, unsigned int buffer_num)
{
  if ( buffer_num < 2 ) {
    src[buffer_num] = buf;
    src_roi[buffer_num] = roi;
  }
}


void
FilterCompare::setDstBuffer(unsigned char *buf, ROI *roi, orientation_t ori)
{
  dst = buf;
  dst_roi = roi;
}


void
FilterCompare::setOrientation(orientation_t ori)
{
}


const char *
FilterCompare::getName()
{
  return "FilterCompare";
}


void
FilterCompare::apply()
{
  if ( src[BACKGROUND] == NULL ) return;
  if ( src[FOREGROUND] == NULL ) return;
  if ( src_roi[BACKGROUND] == NULL ) return;
  if ( src_roi[FOREGROUND] == NULL ) return;

  register unsigned int h = 0;
  register unsigned int w = 0;


  // y-plane
  register unsigned char *byp   = src[BACKGROUND] + (src_roi[BACKGROUND]->start.y * src_roi[BACKGROUND]->line_step) + (src_roi[BACKGROUND]->start.x * src_roi[BACKGROUND]->pixel_step);


  // y-plane
  register unsigned char *fyp   = src[FOREGROUND] + (src_roi[FOREGROUND]->start.y * src_roi[FOREGROUND]->line_step) + (src_roi[FOREGROUND]->start.x * src_roi[FOREGROUND]->pixel_step);
  // u-plane
  register unsigned char *fup   = YUV422_PLANAR_U_PLANE(src[FOREGROUND], src_roi[FOREGROUND]->image_width, src_roi[FOREGROUND]->image_height)
    + ((src_roi[FOREGROUND]->start.y * src_roi[FOREGROUND]->line_step) / 2 + (src_roi[FOREGROUND]->start.x * src_roi[FOREGROUND]->pixel_step) / 2) ;
  // v-plane
  register unsigned char *fvp   = YUV422_PLANAR_V_PLANE(src[FOREGROUND], src_roi[FOREGROUND]->image_width, src_roi[FOREGROUND]->image_height)
    + ((src_roi[FOREGROUND]->start.y * src_roi[FOREGROUND]->line_step) / 2 + (src_roi[FOREGROUND]->start.x * src_roi[FOREGROUND]->pixel_step) / 2);


  // destination y-plane
  register unsigned char *dyp  = dst + (dst_roi->start.y * dst_roi->line_step) + (dst_roi->start.x * dst_roi->pixel_step);
  // destination u-plane
  register unsigned char *dup   = YUV422_PLANAR_U_PLANE(dst, dst_roi->image_width, dst_roi->image_height)
    + ((dst_roi->start.y * dst_roi->line_step) / 2 + (dst_roi->start.x * dst_roi->pixel_step) / 2) ;
  // destination v-plane
  register unsigned char *dvp   = YUV422_PLANAR_V_PLANE(dst, dst_roi->image_width, dst_roi->image_height)
    + ((dst_roi->start.y * dst_roi->line_step) / 2 + (dst_roi->start.x * dst_roi->pixel_step) / 2);
  
  // line starts
  unsigned char *lbyp = byp;   // y-plane
  unsigned char *lfyp = fyp;   // y-plane
  unsigned char *lfup = fup;   // u-plane
  unsigned char *lfvp = fvp;   // v-plane
  unsigned char *ldyp = dyp;  // destination y-plane
  unsigned char *ldup = dup;  // destination u-plane
  unsigned char *ldvp = dvp;  // destination v-plane

  for (h = 0; (h < src_roi[FOREGROUND]->height) && (h < dst_roi->height); ++h) {
    for (w = 0; (w < src_roi[FOREGROUND]->width) && (w < dst_roi->width); w += 2) {
      if ( *byp < *fyp ) {
	*dyp++ = *byp;
      } else {
	*dyp++ = *fyp;
      }
      byp++;
      fyp++;

      if ( *byp < *fyp ) {
	*dyp++ = *byp;
      } else {
	*dyp++ = *fyp;
      }
      byp++;
      fyp++;

      *dup++ = *fup++;
      *dvp++ = *fvp++;
    }

    lbyp   += src_roi[BACKGROUND]->line_step;
    lfyp   += src_roi[FOREGROUND]->line_step;
    lfup   += src_roi[FOREGROUND]->line_step / 2;
    lfvp   += src_roi[FOREGROUND]->line_step / 2;
    ldyp  += dst_roi->line_step;
    ldup  += dst_roi->line_step / 2;
    ldvp  += dst_roi->line_step / 2;
    byp    = lbyp;
    fyp    = lfyp;
    fup    = lfup;
    fvp    = lfvp;
    dyp    = ldyp;
    dup    = ldup;
    dvp    = ldvp;
  }

}