
/***************************************************************************
 *  setup.cpp - OpenNI utility methods: setup routines
 *
 *  Created: Thu Mar 24 10:23:27 2011
 *  Copyright  2006-2011  Tim Niemueller [www.niemueller.de]
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#include <plugins/openni/utils/setup.h>
#include <config/config.h>

namespace fawkes {
  namespace openni {
#if 0 /* just to make Emacs auto-indent happy */
  }
}
#endif

/** Get resolution from configuration.
 * This method reads the config values /plugins/openni/resolution and
 * sets the width and height fields appropriately.
 * @param config config to read values from
 * @param width upon return contains configured width
 * @param height upon return contains configured height
 */
void get_resolution(fawkes::Configuration *config,
		    unsigned int &width, unsigned int &height)
{
  
  std::string  cfg_resolution = config->get_string("/plugins/openni/resolution");

  XnResolution res = XN_RES_VGA;

  if (cfg_resolution == "QQVGA") {
    res = XN_RES_QQVGA;
  } else if (cfg_resolution == "CGA") {
    res = XN_RES_CGA;
  } else if (cfg_resolution == "QVGA") {
    res = XN_RES_QVGA;
  } else if (cfg_resolution == "VGA") {
    res = XN_RES_VGA;
  } else if (cfg_resolution == "SVGA") {
    res = XN_RES_SVGA;
  } else if (cfg_resolution == "XGA") {
    res = XN_RES_XGA;
  } else if (cfg_resolution == "720P") {
    res = XN_RES_720P;
  } else if (cfg_resolution == "SXGA") {
    res = XN_RES_SXGA;
  } else if (cfg_resolution == "UXGA") {
    res = XN_RES_UXGA;
  } else if (cfg_resolution == "1080P") {
    res = XN_RES_1080P;
  } else {
    throw Exception("get_resolution(): Unknown resolution '%s'",
		    cfg_resolution.c_str());
  }

  xn::Resolution resolution(res);
  width  = resolution.GetXResolution();
  height = resolution.GetYResolution();
}


/** Setup a map generator from configuration.
 * This method reads the config values /plugins/openni/resolution and
 * /plugins/openni/fps and uses it to setup the map output of the given map
 * generator.
 * @param generator generator to setup
 * @param config config to read values from
 */
void
setup_map_generator(xn::MapGenerator &generator,
		    fawkes::Configuration *config)
{
  unsigned int width = 0, height = 0;
  get_resolution(config, width, height);
  unsigned int cfg_fps  = config->get_uint("/plugins/openni/fps");

  XnMapOutputMode output_mode;
  output_mode.nXRes = width;
  output_mode.nYRes = height;
  output_mode.nFPS  = cfg_fps;
  XnStatus st;
  if ((st = generator.SetMapOutputMode(output_mode)) != XN_STATUS_OK) {
    throw Exception("OpenNI: failed to set map output mode: %s",
		    xnGetStatusString(st));
  }
}


} // end namespace fawkes::openni
} // end namespace fawkes
