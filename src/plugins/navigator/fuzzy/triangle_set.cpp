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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */
 
#include "triangle_set.h"

/** @class TriangleSet triangle_set.h 
 * Fuzzy Triangle Set.
 * This class provides a triangular shaped fuzzy set.
 *
 * @author Martin Liebenberg
 */
/** @var TriangleSet::m
 * The maximum of this set.
 */
 
/** Constructor.
 * @param l left border of the set
 * @param m the maximum of the set
 * @param r right border of the set
 */
TriangleSet::TriangleSet(double l, double m, double r)
{
  this->l = l;
  this->r = r;
  this->m = m;
  alphaLevel = 0;
}
 
/** Constructor.
 * @param label a lettering for the set
 * @param l left border of the set
 * @param m the maximum of the set
 * @param r right border of the set
 */
TriangleSet::TriangleSet(char* label, double l, double m, double r)
{
  this->label = label;
  this->l = l;
  this->r = r;
  this->m = m;
  alphaLevel = 0;
}
 
/** Destructor.
 */
TriangleSet::~TriangleSet()
{
}
 
/** Returns the grade of membership of x in this set.
 * @param x a value out of the univers of this set
 * @return double, between 0 and 1, containing the grade of membership of x in this set
 */
double TriangleSet::membershipGrade(double x)
{
  if(x < l)
    return 0;
  else if(l < x && x < m)
    return (x - l) / (m - l);
  else if(x == m)
    return 1;
  else if(m < x && x < r)
    return ((m - x) / (r - m)) + 1;
  else
    return 0;
}
 
/** Supports the borders of this set.
 * @param left reference which gets the left border
 * @param right reference which gets the right border
 */
void TriangleSet::supportBorders(double &left, double &right)
{
  left = l;
  right = r;
}
 
/** Returns the first value of the alpha-level set of this fuzzy set.
 * @return double containing the first value of the alpha-level set of this fuzzy set
 */
double TriangleSet::getFirstAlphaLevelValue()
{
  return alphaLevel * (m - l) + l;
}

/** Returns the middle value of the alpha-level set of this fuzzy set.
 * @return double containing the middle value of the alpha-level set of this fuzzy set
 */
double TriangleSet::getMiddleAlphaLevelValue()
{
  return ( -((alphaLevel - 1) * (r - m) - m) + (alphaLevel * (m - l) + l)) / 2. ;
}