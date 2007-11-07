
/***************************************************************************
 *  qa_image_display.cpp - image display QA app
 *
 *  Created: Mon Nov 05 17:46:28 2007
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

/// @cond QA

#include <fvwidgets/image_display.h>
#include <fvwidgets/sdl_keeper.h>
#include <fvutils/readers/fvraw.h>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <SDL.h>

int
main(int argc, char **argv)
{
  const char *img_file;
  if ( argc > 1 ) {
    img_file = argv[1];
  } else {
    printf("Usage: %s <raw image file>\n", argv[0]);
    exit(-1);
  }


  FvRawReader *fvraw = new FvRawReader(img_file);
  unsigned char *buffer = malloc_buffer(fvraw->colorspace(),
					fvraw->pixel_width(), fvraw->pixel_height());

  fvraw->set_buffer(buffer);
  fvraw->read();

  ImageDisplay *display = new ImageDisplay(fvraw->pixel_width(), fvraw->pixel_height());
  display->show(fvraw->colorspace(), buffer);

  SDLKeeper::init(SDL_INIT_EVENTTHREAD);

  bool quit = false;
  while (! quit) {
    SDL_Event event;
    if ( SDL_WaitEvent(&event) ) {
      switch (event.type) {
      case SDL_QUIT:
	quit = true;
	break;
      default:
	break;
      }
    }
  }

  delete display;
  free(buffer);
  delete(fvraw);

  SDLKeeper::quit();

  return 0;
}



/// @endcond