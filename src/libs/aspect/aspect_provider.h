
/***************************************************************************
 *  aspect_provider.h - Aspect to provide a new aspect for Fawkes
 *
 *  Created: Thu Nov 25 12:05:29 2010 (Thanksgiving)
 *  Copyright  2006-2010  Tim Niemueller [www.niemueller.de]
 *
 ****************************************************************************/

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

#ifndef __ASPECT_ASPECT_PROVIDER_H_
#define __ASPECT_ASPECT_PROVIDER_H_

#include <aspect/aspect.h>

namespace fawkes {
#if 0 /* just to make Emacs auto-indent happy */
}
#endif

class AspectIniFin;

class AspectProviderAspect : public virtual Aspect
{
 public:
  AspectProviderAspect(const char *aspect_name, AspectIniFin *inifin);
  virtual ~AspectProviderAspect();

  const char *   aspect_provider_name() const;
  AspectIniFin * aspect_provider_inifin() const;

 private:
  const char   *__aspect_name;
  AspectIniFin *__aspect_inifin;
};

} // end namespace fawkes

#endif
