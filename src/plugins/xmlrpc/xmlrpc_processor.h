
/***************************************************************************
 *  xmlrpc_processor.h - XML-RPC request processor
 *
 *  Created: Sun Aug 30 19:37:50 2009
 *  Copyright  2006-2009  Tim Niemueller [www.niemueller.de]
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

#ifndef __PLUGINS_XMLRPC_XMLRPC_PROCESSOR_H_
#define __PLUGINS_XMLRPC_XMLRPC_PROCESSOR_H_

#include <webview/request_processor.h>

#include <map>
#include <string>

namespace xmlrpc_c {
  class registry;
}

namespace fawkes {
  class Logger;
}

class XmlRpcRequestProcessor : public fawkes::WebRequestProcessor
{
 public:
  XmlRpcRequestProcessor(fawkes::Logger *logger);
  virtual ~XmlRpcRequestProcessor();

  virtual fawkes::WebReply * process_request(const char *url,
					     const char *method,
					     const char *version,
					     const char *upload_data,
					     size_t *upload_data_size,
					     void **session_data);

  xmlrpc_c::registry *  registry();

 private:
  fawkes::Logger       *__logger;
  xmlrpc_c::registry   *__xmlrpc_registry;
};

#endif
