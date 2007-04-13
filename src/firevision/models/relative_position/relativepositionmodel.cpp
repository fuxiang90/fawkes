
/***************************************************************************
 *  relativepositionmodel.cpp - Abstract class defining a position model for
 *                            calculation of relative position
 *
 *  Generated: Wed Mar 21 15:54:42 2007
 *  Copyright  2005-2007  Tim Niemueller [www.niemueller.de]
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

#include <models/relative_position/relativepositionmodel.h>

/** @class RelativePositionModel <models/relative_position/relativepositionmodel.h>
 * Relative Position Model Interface.
 * This interfaces defines a relative position model.
 *
 * @fn const char * RelativePositionModel::getName() const
 * Get name of relative position model.
 * @return name of relative position model
 *
 * @fn void RelativePositionModel::setRadius(float r)
 * Set radius of a found circle.
 * This is especially used for ball position implementations.
 * @param r radius
 *
 * @fn void RelativePositionModel::setCenter(float x, float y)
 * Set center of a found circle.
 * This is especially used for ball position implementations.
 * @param x x position in image (pixels)
 * @param y y position in image (pixels)
 *
 * @fn void RelativePositionModel::setCenter(const center_in_roi_t& c)
 * Set center of a found circle.
 * This is especially used for ball position implementations.
 * @param c center
 *
 * @fn void RelativePositionModel::setPanTilt(float pan, float tilt)
 * Set camera pan and tilt.
 * @param pan pan value (rad)
 * @param tilt tilt value (rad)
 *
 * @fn void RelativePositionModel::getPanTilt(float *pan, float *tilt) const
 * Get camera pan tilt.
 * @param pan contains pan value (rad) upon return
 * @param tilt contains tilt value (rad) upon return
 *
 * @fn void RelativePositionModel::calc()
 * Calculate position data.
 * Call this method if all relevant data (set(Radius|Center|PanTilt))
 * has been set, after this valid data can be retrieved via get*
 *
 * @fn void RelativePositionModel::calc_unfiltered()
 * Calculate data unfiltered.
 * Same as calc(), but without any filtering (i.e. no Kalman filter).
 *
 * @fn void RelativePositionModel::reset()
 * Reset all data.
 * This must be called if the object is not visible.
 *
 * @fn float RelativePositionModel::getDistance() const
 * Get distance to object.
 * @return distance to object in meters.
 *
 * @fn float RelativePositionModel::getBearing() const
 * Get bearing (horizontal angle) to object.
 * @return bearing in rad
 *
 * @fn float RelativePositionModel::getSlope() const
 * Get slope (vertical angle) to object.
 * @return slope in rad
 *
 * @fn float RelativePositionModel::getX() const
 * Get relative X coordinate of object.
 * @return relative X coordinate in local metric cartesian coordinate system
 *
 * @fn float RelativePositionModel::getY() const
 * Get relative Y coordinate of object.
 * @return relative Y coordinate in local metric cartesian coordinate system
 *
 * @fn bool RelativePositionModel::isPosValid() const
 * Check if position is valid.
 * @return true, if the calculated position is valid, false otherwise
 *
 * @author Tim Niemueller
 */


/** Destructor. */
RelativePositionModel::~RelativePositionModel()
{
}