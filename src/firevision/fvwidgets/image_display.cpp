
/***************************************************************************
 *  image_display.cpp - widget to display an image based on SDL
 *
 *  Created: Mon Nov 05 14:19:26 2007
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

#include <fvwidgets/image_display.h>

#include <fvwidgets/sdl_keeper.h>
#include <SDL.h>

#include <core/exception.h>
#include <fvutils/color/conversions.h>
#include <fvutils/color/yuv.h>

/** @class ImageDisplay <fvwidgets/image_display.h>
 * Simple image display.
 * This is a simple thin wrapper around the SDL to display images in a standalone
 * window. Use this for instance for easy verification of vision results.
 * @author Tim Niemueller
 */

/** Constructor.
 * @param width width of image
 * @param height height of image
 */
ImageDisplay::ImageDisplay(unsigned int width, unsigned int height)
{

  SDLKeeper::init(SDL_INIT_VIDEO);

  _width  = width;
  _height = height;

  int bpp = SDL_VideoModeOK(_width, _height, 16, SDL_ANYFORMAT);
  _surface = SDL_SetVideoMode(width, height, bpp, /* flags */ SDL_HWSURFACE | SDL_ANYFORMAT);
  if ( ! _surface ) {
    throw Exception("SDL: cannot create surface");
  }

  // SDL_UYVY_OVERLAY
  _overlay = SDL_CreateYUVOverlay(width, height, SDL_UYVY_OVERLAY, _surface);
  if ( ! _overlay ) {
    throw Exception("Cannot create overlay");
  }

  _rect = new SDL_Rect;

  _rect->x = 0;
  _rect->y = 0;
  _rect->w = _width;
  _rect->h = _height;
}


/** Destructor. */
ImageDisplay::~ImageDisplay()
{
  delete _rect;

  SDL_FreeYUVOverlay(_overlay);
  SDL_FreeSurface(_surface);

  SDLKeeper::quit();
}


/** Show image from given colorspace.
 * @param colorspace colorspace of the supplied buffer
 * @param buffer image buffer
 */
void
ImageDisplay::show(colorspace_t colorspace, unsigned char *buffer)
{
  SDL_LockYUVOverlay(_overlay);
  convert(colorspace, YUV422_PACKED, buffer, _overlay->pixels[0], _width, _height);
  SDL_UnlockYUVOverlay(_overlay);
  SDL_DisplayYUVOverlay(_overlay, _rect);
}


/** Show image from YUV422_PLANAR colorspace.
 * @param yuv422_planar_buffer YUV422_PLANAR encoded image.
 */
void
ImageDisplay::show(unsigned char *yuv422_planar_buffer)
{
  SDL_LockYUVOverlay(_overlay);

  yuv422planar_to_yuv422packed(yuv422_planar_buffer, _overlay->pixels[0],
			       _width, _height);

  SDL_UnlockYUVOverlay(_overlay);
  SDL_DisplayYUVOverlay(_overlay, _rect);
}