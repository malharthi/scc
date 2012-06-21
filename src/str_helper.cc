
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#include <cstdarg>

#include "str_helper.h"

namespace str_helper
{

void FindAndReplaceAll(std::string& source,
                       const char* old_value,
                       const char* new_value) {
  size_t old_len = strlen(old_value);
  size_t new_len = strlen(new_value);
  size_t position = 0;

  while ((position = source.find(old_value, position)) != std::string::npos) {
    source.replace(position, old_len, new_value);
    position += new_len;
  }
}


std::string FormatString(std::string format_str, ...) {
  va_list arg_list;
  va_start(arg_list, format_str);

  const int buffer_len = 256;
  char buffer[buffer_len];
  vsprintf(buffer, format_str.c_str(), arg_list);

  va_end(arg_list);
  return std::string(buffer);
}


std::string RemoveExtensionFromFileName(const std::string& file_name) {
  size_t last_index = file_name.find_last_of(".");
  if (last_index == std::string::npos)
    // No extension is present
    return file_name;
  else
    return file_name.substr(0, last_index); 
}

} // namespace