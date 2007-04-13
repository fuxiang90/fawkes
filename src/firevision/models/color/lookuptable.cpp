
/***************************************************************************
 *  lookuptable.cpp - Implementation of a lookup table color model
 *
 *  Generated: Wed May 18 13:59:18 2005
 *  Copyright  2005  Tim Niemueller  [www.niemueller.de]
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

#include <models/color/lookuptable.h>
#include <fvutils/color/yuv.h>
#include <fvutils/ipc/shm_lut.h>
#include <fvutils/ipc/shm_registry.h>

#include <iostream>
#include <sys/utsname.h>

using namespace std;

/** @class ColorModelLookupTable <models/color/lookuptable.h>
 * Color model based on a lookup table.
 * Very fast and easy implementation of a lookup table. It ignores
 * the luminance and determines the classification just based on the U and
 * V chrominance values. This model is very versatile as you can generate
 * the lookuptable with many different methods.
 *
 * VERY IMPORTANT NOTE:
 * The given width and height are _not_ the dimensions of the image but they are the size
 * of the lookup table. For example if you want to generate a lut for the UV values of the YUV color
 * space and you have one byte per U and V value then you would have width = height = 256!
 */

/** Create a lookup table with given dimensions _not_ using shared memory.
 * @param width width of lookup table
 * @param height height of lookup table
 */
ColorModelLookupTable::ColorModelLookupTable(unsigned int width, unsigned int height)
{
  this->width            = width;
  this->height           = height;
  this->lut_id           = FIREVISION_SHM_LUT_INVALID;
  this->destroy_on_free  = false;

  create();
}

/** Create a lookup table with given dimensions _not_ using shared memory, load contents
 * from file
 * @param file name of the file to load from
 * @param width width of lookup table
 * @param height height of lookup table
 */
ColorModelLookupTable::ColorModelLookupTable(const char *file,
					     unsigned int width, unsigned int height)
{
  this->width            = width;
  this->height           = height;
  this->lut_id           = FIREVISION_SHM_LUT_INVALID;
  this->destroy_on_free  = false;

  create();
  load(file);
}


/** Create a lookup table with given dimensions using shared memory
 * @param width width of lookup table
 * @param height height of lookup table
 * @param lut_id ID of the LUT in shared memory, use a constant from utils/shm_registry.h
 * @param destroy_on_free true to destroy lookup table in shmem on delete
 */
ColorModelLookupTable::ColorModelLookupTable(unsigned int width, unsigned int height,
					     unsigned int lut_id, bool destroy_on_free)
{
  this->width            = width;
  this->height           = height;
  this->lut_id           = lut_id;
  this->destroy_on_free  = destroy_on_free;

  create();
}


/** Create a lookup table with given dimensions using shared memory, load contents
 * from file
 * @param file name of the file to load from
 * @param width width of lookup table
 * @param height height of lookup table
 * @param lut_id ID of the LUT in shared memory, use a constant from utils/shm_registry.h
 * @param destroy_on_free true to destroy lookup table in shmem on delete
 */
ColorModelLookupTable::ColorModelLookupTable(const char *file,
					     unsigned int width, unsigned int height,
					     unsigned int lut_id, bool destroy_on_free)
{
  this->width            = width;
  this->height           = height;
  this->lut_id           = lut_id;
  this->destroy_on_free  = destroy_on_free;

  create();
  load(file);
}


/** Creates the memory segment.
 */
void
ColorModelLookupTable::create()
{
  bytes_per_sample = 1;

  if ( lut_id != FIREVISION_SHM_LUT_INVALID ) {
    shm_lut   = new SharedMemoryLookupTable( lut_id,
					     width, height,
					     bytes_per_sample);
    shm_lut->setDestroyOnDelete( destroy_on_free );
    lut       = shm_lut->getBuffer();
    lut_bytes = shm_lut->getDataSize();
  } else {
    lut_bytes = width * height * bytes_per_sample;
    lut = (unsigned char *)malloc( lut_bytes );
  }
  memset(lut, C_OTHER, lut_bytes);
}


/** Load LUT from file.
 * @param file filename
 */
void
ColorModelLookupTable::load(const char *file)
{
  FILE *f = fopen(file, "r");
  if (f == NULL) {
    cout << "ColorModelLookupTable: Cannot open file '" << file << "' for loading" << endl;
    return;
  }

  int err = 0;
  if ( (fread(lut, lut_bytes, 1, f) == 0) && (! feof(f)) && ((err = ferror(f)) != 0)) {
    cout << "ColorModelLookupTable: Cannot not read file '" << file << "' (" << strerror(err) << ")!" << endl;
  }

  fclose(f);
}


/** Save LUT to file.
 * @param file filename
 */
void
ColorModelLookupTable::save(const char *file)
{
  FILE *f = fopen(file, "w");

  if (f == NULL) {
    cout << "ColorModelLookupTable: Cannot open file '" << file << "' for writing" << endl;
    return;
  }

  if ( fwrite(lut, lut_bytes, 1, f) == 0) {
    cout << "ColorModelLookupTable: Could not write!" << endl;
  } 

  fclose(f);
}


/** Reset lookup table.
 * This will set all entries to C_OTHER.
 */
void
ColorModelLookupTable::reset()
{
  memset(lut, C_OTHER, lut_bytes);
}


/** Destructor. */
ColorModelLookupTable::~ColorModelLookupTable()
{
  if ( lut_id != FIREVISION_SHM_LUT_INVALID ) {
    delete shm_lut;
  } else {
    free(lut);
  }
  lut = NULL;
  lut_bytes = 0;
}


color_t
ColorModelLookupTable::determine(unsigned int y,
				 unsigned int u,
				 unsigned int v ) const
{
  return (color_t) *(lut + (v * width * bytes_per_sample) + (u * bytes_per_sample));
}


const char *
ColorModelLookupTable::getName()
{
  return "ColorModelLookupTable";
}


/** Set the appropriate value.
 * @param y y value of entry to set
 * @param u u value of entry to set
 * @param v v value of entry to set
 * @param c color class to set this entry to
 */
void
ColorModelLookupTable::set(unsigned int y,
			   unsigned int u,
			   unsigned int v,
			   color_t      c )
{
  *(lut + (v * width * bytes_per_sample) + (u * bytes_per_sample)) = c;
}


/** Copy another LUT buffer.
 * @param buffer buffer to copy, must be at least of the size as this
 * buffer. */
void
ColorModelLookupTable::set(unsigned char *buffer)
{
  memcpy(lut, buffer, lut_bytes);
}


/** Get size of LUT in bytes.
 * @return size of LUT in bytes. */
unsigned int
ColorModelLookupTable::size()
{
  return lut_bytes;
}


/** Get LUT buffer.
 * @return buffer */
unsigned char *
ColorModelLookupTable::getBuffer()
{
  return lut;
}


/** Create image from LUT.
 * Create image from LUT, useful for debugging and analysing.
 * @param yuv422_planar_buffer contains the image upon return, must be initialized
 * with the appropriate size before.
 */
void
ColorModelLookupTable::toImage(unsigned char *yuv422_planar_buffer)
{
  unsigned char *yp = yuv422_planar_buffer;
  unsigned char *up = YUV422_PLANAR_U_PLANE(yuv422_planar_buffer, 512, 512);
  unsigned char *vp = YUV422_PLANAR_V_PLANE(yuv422_planar_buffer, 512, 512);

  color_t c;
  for (unsigned int v = width; v > 0 ; --v) {
    for (unsigned int u = 0; u < width; ++u) {

      c = determine(128, u, v-1);

      switch (c) {
      case C_ORANGE:
	*yp++ = 128;
	*yp++ = 128;
	*up++ =  30;
	*vp++ = 220;
	break;

      case C_BLUE:
	*yp++ =   0;
	*yp++ =   0;
	*up++ = 255;
	*vp++ = 128;
	break;

      case C_YELLOW:
	*yp++ = 255;
	*yp++ = 255;
	*up++ =   0;
	*vp++ = 128;
	break;

      case C_CYAN:
	*yp++ = 255;
	*yp++ = 255;
	*up++ = 128;
	*vp++ =   0;
	break;

      case C_MAGENTA:
	*yp++ = 128;
	*yp++ = 128;
	*up++ = 128;
	*vp++ = 255;
	break;

      case C_WHITE:
	*yp++ = 255;
	*yp++ = 255;
	*up++ = 128;
	*vp++ = 128;
	break;

      case C_BLACK:
	*yp++ =   0;
	*yp++ =   0;
	*up++ = 128;
	*vp++ = 128;
	break;

      case C_GREEN:
	*yp++ = 128;
	*yp++ = 128;
	*up++ =   0;
	*vp++ =   0;
	break;

      case C_RED:
	*yp++ = 128;
	*yp++ = 128;
	*up++ =   0;
	*vp++ = 255;
	break;

      default:
	*yp++ = 128;
	*yp++ = 128;
	*up++ = 128;
	*vp++ = 128;
	break;
      }
    }
    // Double line
    memcpy(yp, (yp - 512), 512);
    yp += 512;
    memcpy(up, (up - 256), 256);
    memcpy(vp, (vp - 256), 256);
    up += 256;
    vp += 256;
  }

}


/** Compose filename.
 * In the format %g is replaced with the hostname.
 * @param format format for the filename
 */
string
ColorModelLookupTable::composeFilename(const string format)
{
  string rv = format;

  struct utsname uname_info;
  uname( &uname_info );

  unsigned int loc = rv.find( "%h" );
  while (loc != string::npos) {
    rv.replace( loc, 2, uname_info.nodename );
    loc = rv.find( "%h" );
  }

  return rv;
}