
/***************************************************************************
 *  image_thread.h - OpenNI image provider thread
 *
 *  Created: Thu Mar 17 13:58:25 2011
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

#ifndef __PLUGINS_OPENNI_IMAGE_THREAD_H_
#define __PLUGINS_OPENNI_IMAGE_THREAD_H_

#include <core/threading/thread.h>
#include <core/utils/lockptr.h>
#include <aspect/logging.h>
#include <aspect/configurable.h>
#include <aspect/clock.h>
#include <aspect/blocked_timing.h>
#include <plugins/openni/aspect/openni.h>

#include <XnCppWrapper.h>

#include <map>

namespace firevision {
  class SharedMemoryImageBuffer;
}

class OpenNiImageThread
: public fawkes::Thread,
  public fawkes::BlockedTimingAspect,
  public fawkes::LoggingAspect,
  public fawkes::ConfigurableAspect,
  public fawkes::ClockAspect,
  public fawkes::OpenNiAspect
{
 public:
  OpenNiImageThread();
  virtual ~OpenNiImageThread();

  virtual void init();
  virtual void loop();
  virtual void finalize();

 /** Stub to see name in backtrace for easier debugging. @see Thread::run() */
 protected: virtual void run() { Thread::run(); }

 private:
  xn::ImageGenerator                  *__image_gen;
  xn::DepthGenerator                  *__depth_gen;

  xn::ImageMetaData                   *__image_md;
  xn::DepthMetaData                   *__depth_md;

  firevision::SharedMemoryImageBuffer *__image_buf;
  firevision::SharedMemoryImageBuffer *__depth_buf;

  typedef enum {
    DEBAYER_BILINEAR,
    DEBAYER_NEAREST_NEIGHBOR,
    CONVERT_YUV
  } CopyMode;
  CopyMode                             __cfg_copy_mode;

  size_t                               __depth_bufsize;

  unsigned short int                   __usb_vendor;
  unsigned short int                   __usb_product;
  unsigned int                         __image_width;
  unsigned int                         __image_height;
  unsigned int                         __depth_width;
  unsigned int                         __depth_height;
};

#endif
