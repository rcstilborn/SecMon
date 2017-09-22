/*
 * MimeTypes.h
 *
 *  Created on: Aug 8, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#ifndef GUI_MIMETYPES_H_
#define GUI_MIMETYPES_H_

#include <string>

namespace http {
namespace mime_types {

/// Convert a file extension into a MIME type.
std::string extension_to_type(const std::string& extension);

}  // namespace mime_types
}  // namespace http

#endif // GUI_MIMETYPES_H_
