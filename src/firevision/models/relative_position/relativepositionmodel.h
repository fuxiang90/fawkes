
/***************************************************************************
 *  relativepositionmodel.h - Abstract class defining a position model for
 *                            calculation of relative position
 *
 *  Generated: Tue May 31 13:50:12 2005
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

#ifndef __FIREVISION_RELATIVEPOSITIONMODEL_H_
#define __FIREVISION_RELATIVEPOSITIONMODEL_H_

#include <fvutils/types.h>

class RelativePositionModel
{

 public:
  virtual ~RelativePositionModel();

  virtual const char *  getName(void) const		            = 0;
  virtual void		setRadius(float r)		            = 0;
  virtual void		setCenter(float x, float y)	            = 0;
  virtual void		setCenter(const center_in_roi_t& c)         = 0;

  virtual void		setPanTilt(float pan, float tilt)           = 0;
  virtual void		getPanTilt(float *pan, float *tilt) const   = 0;

  virtual void          calc()                                      = 0;
  virtual void          calc_unfiltered()                           = 0;

  virtual void          reset()                                     = 0;

  virtual float		getDistance() const		            = 0;
  virtual float		getBearing() const	  	            = 0;
  virtual float		getSlope() const                            = 0;
  virtual float		getX() const			            = 0;
  virtual float		getY() const			            = 0;

  virtual bool          isPosValid() const                          = 0;

};

#endif