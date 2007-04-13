
/***************************************************************************
 *  cornerhorizon.h - Scanline model "shrinker", takes the corner in front
 *                    of the robots and calculates a "fake horizon" from this
 *                    and only returns the scanline points which are below
 *                    that very horizon
 *
 *  Generated: Fri Apr 07 04:34:08 2006
 *  Copyright  2005-2006  Tim Niemueller [www.niemueller.de]
 *             2006       Stefan Schiffer
 *             2006       Christoph Mies
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

#ifndef __FIREVISION_MODELS_SCANLINE_CORNERHORIZON_H_
#define __FIREVISION_MODELS_SCANLINE_CORNERHORIZON_H_

#include <models/scanlines/scanlinemodel.h>
#include <fvutils/types.h>

class CornerHorizon : public ScanlineModel
{

 public:

  CornerHorizon( ScanlineModel *model,
		 float field_length, float field_width, float field_border,
		 unsigned int image_width, unsigned int image_height,
		 float camera_height, float camera_ori,
		 float horizontal_angle, float vertical_angle
		 );

  virtual ~CornerHorizon();

  point_t  operator*();
  point_t* operator->();
  point_t* operator++();
  point_t* operator++(int); 

  bool          finished();
  void          reset();
  const char *  getName();
  unsigned int  getMargin();

  void setRobotPose(float x, float y, float ori);
  void setPanTilt(float pan, float tilt);

  unsigned int getHorizon();

 protected:
  void calculate();

 private:
  ScanlineModel *model;

  bool calculated;

  float field_length;
  float field_width;
  float field_border;
  
  float pose_x;
  float pose_y;
  float pose_ori;

  float pan;
  float tilt;

  unsigned int image_width;
  unsigned int image_height;

  float camera_height;
  float camera_ori;

  float horizontal_angle;
  float vertical_angle;

  float pan_pixel_per_rad;
  float tilt_pixel_per_rad;

  point_t coord;
  point_t tmp_coord;

  unsigned int horizon;

  static const float M_PI_HALF;

};

#endif