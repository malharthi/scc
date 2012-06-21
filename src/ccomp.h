
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#ifndef INCLUDE_CCOMPX_SRC_CCOMP_H__
#define INCLUDE_CCOMPX_SRC_CCOMP_H__

// A location in a source file
class SourceLocation {
 public:
  SourceLocation(const std::string& file="", unsigned int line = 1, unsigned int column = 1)
    : file_(file), line_(line), column_(column) {
  }

  std::string file() {
    return file_;
  }
  unsigned int line() {
    return line_;
  }
  unsigned int column() {
    return column_;
  }
  void set_line(unsigned int line) {
    line_ = line;
  }
  void set_column(unsigned int col) {
    column_ = col;
  }

private:
  std::string file_;
  unsigned int line_;
  unsigned int column_;
};

// A compiler message associated with a location
class Message {
 public:
  Message(const std::string& message)
    : message_(message) {
  }
  Message(const std::string& message, const SourceLocation& loc)
    : message_(message), location_(loc) {
  }

  std::string message() {
    return message_;
  }
  SourceLocation location() {
    return location_;
  }

private:
  std::string message_;
  SourceLocation location_;
};

#endif // INCLUDE_CCOMPX_SRC_CCOMP_H__
