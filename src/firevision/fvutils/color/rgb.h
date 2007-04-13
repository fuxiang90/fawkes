
/***************************************************************************
 *  rgb.h - RGB specific methods, macros and constants
 *
 *  Created: Sat Aug 12 14:58:02 2006
 *  based on colorspaces.h from Tue Feb 23 13:49:38 2005
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

#ifndef __FIREVISION_UTILS_COLOR_RGB_H
#define __FIREVISION_UTILS_COLOR_RGB_H


#define RGB_PIXEL_SIZE 3
#define RGB_PIXEL_AT(RGB, width, x, y)    ((RGB_t *)(RGB + ((y) * (width) * RGB_PIXEL_SIZE) + (x) * RGB_PIXEL_SIZE))
#define RGB_CLEAR_PIXEL(RGB, width, x, y) memset(RGB + ((y) * (width) * RGB_PIXEL_SIZE) + (x) * RGB_PIXEL_SIZE, 0, RGB_PIXEL_SIZE);
#define RGB_RED_AT(RGB, width, x, y)      (RGB_PIXEL_AT(RGB, (width), (x), (y))->R)
#define RGB_GREEN_AT(RGB, width, x, y)    (RGB_PIXEL_AT(RGB, (width), (x), (y))->G)
#define RGB_BLUE_AT(RGB, width, x, y)     (RGB_PIXEL_AT(RGB, (width), (x), (y))->B)
#define RGB_SET_RED(RGB, width, x, y)     {RGB_t *p=RGB_PIXEL_AT(RGB, (width), (x), (y)); p->R=255; p->G=0;   p->B=0; }
#define RGB_SET_GREEN(RGB, width, x, y)   {RGB_t *p=RGB_PIXEL_AT(RGB, (width), (x), (y)); p->R=0;   p->G=255; p->B=0; }
#define RGB_SET_BLUE(RGB, width, x, y)    {RGB_t *p=RGB_PIXEL_AT(RGB, (width), (x), (y)); p->R=0;   p->G=0;   p->B=255; }


/** Structure defining an RGB pixel (in R-G-B byte ordering). */
typedef struct {
  unsigned char R;	/**< R value */
  unsigned char G;	/**< G value */
  unsigned char B;	/**< B value */
} RGB_t;

/** Structure defining an RGB pixel (in B-G-R byte ordering). */
typedef struct {
  unsigned char B;	/**< B value */
  unsigned char G;	/**< G value */
  unsigned char R;	/**< R value */
} BGR_t;


void rgb_to_rgb_with_alpha_plainc(unsigned char *rgb, unsigned char *rgb_alpha,
				  unsigned int width, unsigned int height);


void rgb_to_bgr_with_alpha_plainc(unsigned char *rgb, unsigned char *bgr_alpha,
				  unsigned int width, unsigned int height);

void bgr_to_rgb_plainc(unsigned char *BGR, unsigned char *RGB,
		       unsigned int width, unsigned int height);


#endif