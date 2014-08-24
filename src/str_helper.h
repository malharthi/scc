
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

//
// String Helper Functions Header
//

#ifndef INLCUDE_CCOMPX_SRC_STR_HELPER_H__
#define INLCUDE_CCOMPX_SRC_STR_HELPER_H__

#include <string>

namespace str_helper
{

// A simple string format function
std::string FormatString(std::string format_str, ...);

// Replaces all occurences of old_value with new_value in source
void FindAndReplaceAll(std::string& source,
                       const char* old_value,
                       const char* new_value);


std::string RemoveExtensionFromFileName(const std::string& file_name);

} // namespace

#endif // INLCUDE_CCOMPX_SRC_STR_HELPER_H__
