
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

//
// Lecical Analyzer Header
//

#ifndef INCLUDE_CCOMPX_SRC_LEXER_H__
#define INCLUDE_CCOMPX_SRC_LEXER_H__

#include <fstream>
#include <stack>
#include <string>
#include <vector>

#include "ccomp.h"
#include "symbol_table.h"
#include "str_helper.h"



// Our lexical analyzer that do the actual analysis
class Lexer
{
 public:
  // Creates a new lexical analyzer object associated with the given
  // source file.
  Lexer(const std::string& file_name, std::vector<Message>* errors);
  ~Lexer();

  // Return the next token in the source file.
  Token GetNextToken(SymbolTable& symbol_table);
  Token PeekToken(SymbolTable& symbol_table);
  // Step back to a previous state 
  void StepBack(int steps = 1);

  // Return the current line number.
  SourceLocation current_location() {
    return current_location_;
  }
  std::string source_file() {
    return source_file_;
  }
  
 private:
  // Read the next character from the input stream
  void ReadChar();
  char PeekChar();
  // Scan the input stream and get a token
  Token ScanToken(SymbolTable& symbol_table);
  // Get the token type where it is a one-char token or a two-char token
  // (for the operators only).
  TokenType GetTokenType(const std::string& lexeme); 
  // Generate a lexical error
  void Error(const std::string& message);

  // The input file stream
  std::ifstream input_stream_;
  // Internal member variables
  char current_character_;
  SourceLocation current_location_;
  std::string source_file_;
  std::vector<Message>* lexical_errors_;

  // Pointer to the locations at which we started
  // looking for tokens. Used for stepping back to a
  // prevous state.
  std::stack<std::ifstream::pos_type> source_pointers_;
};

#endif // INCLUDE_CCOMPX_SRC_LEXER_H__
