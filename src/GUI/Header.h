/*
 * RequestHandler.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef GUI_HEADER_H_
#define GUI_HEADER_H_

#include <string>

namespace http {

struct Header {
  explicit Header(const std::string& name = "", const std::string& value = "")
      : name(name),
        value(value) {
  }

  std::string name;
  std::string value;
};

}  // namespace http

#endif // GUI_HEADER_H_
