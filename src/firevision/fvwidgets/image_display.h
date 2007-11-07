
/***************************************************************************
 *  image_display.h - widget to display an image based on SDL
 *
 *  Created: Mon Nov 05 13:49:20 2007
 *  Copyright  2007  Tim Niemueller [www.niemueller.de]
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

#ifndef __FIREVISION_FVWIDGETS_IMAGE_DISPLAY_H_
#define __FIREVISION_FVWIDGETS_IMAGE_DISPLAY_H_

#include <fvutils/color/colorspaces.h>

typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Overlay SDL_Overlay;
typedef struct SDL_Rect    SDL_Rect;

class ImageDisplay
{
 public:
  ImageDisplay(unsigned int width, unsigned int height);
  ~ImageDisplay();

  void show(colorspace_t colorspace, unsigned char *buffer);
  void show(unsigned char *yuv422_planar_buffer);

 private:
  SDL_Surface *_surface;
  SDL_Overlay *_overlay;
  SDL_Rect    *_rect;

  unsigned int _width;
  unsigned int _height;
};

#endif