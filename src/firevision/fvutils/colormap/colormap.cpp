
/**************************************************************************
 *  colormap.h - colormap interface
 *
 *  Created: Sat Mar 29 12:45:29 2008
 *  Copyright  2005-2008  Tim Niemueller  [www.niemueller.de]
 *
 *  $Id$
 *
 ***************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#include <fvutils/colormap/colormap.h>

#include <fvutils/color/yuv.h>
#include <cstring>

/** @class Colormap <fvutils/colormap/colormap.h>
 * Colormap interface.
 * This C++ pure virtual class describes the interface to a generic colormap. It is
 * currently tailored to the YUV colorspace.
 *
 * @author Tim Niemueller
 *
 *
 * @fn color_t Colormap::determine(unsigned int y, unsigned int u, unsigned int v) const = 0
 * Determine color class for given YUV value.
 * @param y Y value from YUV colorspace
 * @param u U value from YUV colorspace
 * @param v V value from YUV colorspace
 * @return color class for the given YUV color
 *
 * @fn void Colormap::set(unsigned int y, unsigned int u, unsigned int v, color_t c) = 0
 * Set color class for given YUV value.
 * @param y Y value from YUV colorspace
 * @param u U value from YUV colorspace
 * @param v V value from YUV colorspace
 * @param c class for the given YUV color
 *
 * @fn void Colormap::reset() = 0
 * Reset colormap.
 * Resets all values to return C_UNKNOWN for every query with determine().
 *
 * @fn void Colormap::set(unsigned char *buffer) = 0
 * Set to the given raw buffer.
 * @param buffer buffer to copy data from
 *
 * @fn size_t Colormap::size() = 0
 * Size in bytes of buffer returned by get_buffer().
 *
 * @fn unsigned char * Colormap::get_buffer() const = 0
 * Get the raw buffer of this colormap.
 * @return raw buffer
 *
 * @fn Colormap &  Colormap::operator+=(const Colormap & cmlt) = 0
 * Adds the given colormap to this colormap.
 * This operator takes the given colormap and compares it to this colormap. If this colormap
 * has C_OTHER or C_BACKGROUND the value is compied from the other LUT, otherwise the
 * value is kept as is.
 * @param cmlt other colormap to add
 * @return reference to this
 *
 * @fn Colormap & Colormap::operator+=(const char *filename) = 0
 * Convenience method for the method above.
 * This adds the colormap as in the above method but instead of an instantiated colormap
 * it takes the path to a colormap file which is loaded and added.
 * @param filename file name of colormap to add
 * @return reference to this
 *
 * @fn unsigned int Colormap::width() const = 0
 * Get width of colormap.
 * @return colormap width, meaning depends on actual colormap implementation
 *
 * @fn unsigned int Colormap::height() const = 0
 * Get height of colormap.
 * @return colormap height, meaning depends on actual colormap implementation
 *
 * @fn unsigned int Colormap::depth() const = 0
 * Get depth of colormap.
 * @return colormap depth, meaning depends on actual colormap implementation
 *
 * @fn unsigned int Colormap::deepness() const = 0
 * Get deepness of colormap.
 * The deepness is the maximum value of depth().
 * @return colormap deepness, meaning depends on actual colormap implementation
 *
 * @fn std::list<ColormapFileBlock *>  Colormap::get_blocks() = 0
 * Get file blocks for this colormap.
 * @return list of colormap blocks for this colormap.
 *
 */

/** Virtual empty destructor. */
Colormap::~Colormap()
{
}

/** Create image from LUT.
 * Create image from LUT, useful for debugging and analysing.
 * This method produces a representation of the given level
 * (Y range with 0 <= level < depth) for visual inspection of the colormap.
 * The dimensions of the resulting image are 512x512 pixels. It uses standard strong
 * colors for the different standard color classes. C_UNKNOWN is grey, C_BACKGROUND
 * is black (like C_BLACK).
 * If the standard method does not suit your needs you can override this method.
 * @param yuv422_planar_buffer contains the image upon return, must be initialized
 * with the appropriate memory size before calling, dimensions are 512x512 pixels.
 * @param level the level to draw, it's a range in the Y direction and is in the
 * range 0 <= level < depth.
 */
void
Colormap::to_image(unsigned char *yuv422_planar_buffer, unsigned int level)
{
  unsigned int lwidth  = width();
  unsigned int lheight = height();

  unsigned char *yp = yuv422_planar_buffer;
  unsigned char *up = YUV422_PLANAR_U_PLANE(yuv422_planar_buffer, lwidth * 2, lheight * 2);
  unsigned char *vp = YUV422_PLANAR_V_PLANE(yuv422_planar_buffer, lwidth * 2, lheight * 2);

  unsigned int y = level * deepness() / depth();

  color_t c;
  for (unsigned int v = lwidth; v > 0 ; --v) {
    for (unsigned int u = 0; u < lheight; ++u) {
      c = determine(y, u, v-1);

      switch (c) {
      case C_ORANGE:     *yp++ = 128; *yp++ = 128; *up++ =  30; *vp++ = 220; break;
      case C_BLUE:       *yp++ =   0; *yp++ =   0; *up++ = 255; *vp++ = 128; break;
      case C_YELLOW:     *yp++ = 255; *yp++ = 255; *up++ =   0; *vp++ = 128; break;
      case C_CYAN:       *yp++ = 255; *yp++ = 255; *up++ = 128; *vp++ =   0; break;
      case C_MAGENTA:    *yp++ = 128; *yp++ = 128; *up++ = 128; *vp++ = 255; break;
      case C_WHITE:      *yp++ = 255; *yp++ = 255; *up++ = 128; *vp++ = 128; break;
      case C_BLACK:      *yp++ =   0; *yp++ =   0; *up++ = 128; *vp++ = 128; break;
      case C_BACKGROUND: *yp++ =  64; *yp++ =  64; *up++ = 128; *vp++ = 128; break;
      case C_GREEN:      *yp++ = 128; *yp++ = 128; *up++ =   0; *vp++ =   0; break;
      case C_RED:        *yp++ = 128; *yp++ = 128; *up++ =   0; *vp++ = 255; break;
      default:           *yp++ = 128; *yp++ = 128; *up++ = 128; *vp++ = 128; break;
      }
    }
    // Double line
    memcpy(yp, (yp - lwidth * 2), lwidth *2);
    yp += lwidth * 2;
    memcpy(up, (up - lwidth), lwidth);
    memcpy(vp, (vp - lwidth), lwidth);
    up += lwidth;
    vp += lwidth;
  }
}
