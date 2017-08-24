/*
 * MimeTypes.cpp
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include "MimeTypes.h"

#include <string>

namespace http {
namespace mime_types {

struct mapping {
  const char* extension;
  const char* mime_type;
} mappings[] = { { "gif", "image/gif" }, { "htm", "text/html" }, { "html", "text/html" },
    { "json", "application/json" }, { "jpg", "image/jpeg" }, { "png", "image/png" }, { "css", "text/css" }, { "mjpeg",
        "multipart/x-mixed-replace;boundary=<>" }, { 0, 0 }  // Marks end of list.
};

std::string extension_to_type(const std::string& extension) {
  for (mapping* m = mappings; m->extension; ++m) {
    if (m->extension == extension) {
      return m->mime_type;
    }
  }

  return "text/plain";
}

}  // namespace mime_types
}  // namespace http
