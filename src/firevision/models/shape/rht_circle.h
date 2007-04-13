
/***************************************************************************
 *  rht_circle.h - Header of circle shape model
 *                 using Randomized Hough Transform
 *
 *  Generated: Tue Jun 28 2005
 *  Copyright  2005  Hu Yuxiao      <Yuxiao.Hu@rwth-aachen.de>
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

#ifndef __FIREVISION_RHT_CIRCLE_H_
#define __FIREVISION_RHT_CIRCLE_H_

#include <vector>
#include <iostream>

#include <fvutils/types.h>
#include <models/shape/circle.h>
#include <models/shape/accumulators/ht_accum.h>

class ROI;

class RhtCircleModel: public ShapeModel
{
 private:
  std::vector<Circle> m_Circles;
  RhtAccumulator accumulator;
  static const float RHT_MIN_RADIUS;
  static const float RHT_MAX_RADIUS;

 public:
  RhtCircleModel(void);
  virtual ~RhtCircleModel(void);

  std::string	getName(void) const {return std::string("RhtCircleModel");}
  int		parseImage(unsigned char* buffer, ROI *roi);
  int		getShapeCount(void) const;
  Circle*	getShape(int id) const;
  Circle*	getMostLikelyShape(void) const;
  
 private:
  void		calcCircle(	// for calculating circles from 3 points
			   const point_t& p1,
			   const point_t& p2,
			   const point_t& p3,
			   center_in_roi_t& center,
			   float& radius);
};

#endif // __FIREVISION_RHT_CIRCLE_H_
