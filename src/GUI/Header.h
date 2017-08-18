/*
 * RequestHandler.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 */

#ifndef GUI_HEADER_H_
#define GUI_HEADER_H_

#include <string>

namespace http {

struct Header
{
    Header(const std::string& name = "", const std::string& value = "")
        :
          name(name),
          value(value)
    {
    }

  std::string name;
  std::string value;
};

} // namespace http

#endif /* GUI_HEADER_H_ */
