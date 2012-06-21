
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#ifndef INCLUDE_CCOMPX_SRC_BASE_H__
#define INCLUDE_CCOMPX_SRC_BASE_H__

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
// Src: http://src.chromium.org/viewvc/chrome/trunk/src/base/basictypes.h
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#endif // INCLUDE_CCOMPX_SRC_BASE_H__
