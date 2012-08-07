
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#include <cctype>
#include <cstdio>
#include <sstream>

#include "lexer.h"

Lexer::Lexer(const std::string& file_name, std::vector<Message>* errors)
  : lexical_errors_(errors),
    current_location_(SourceLocation(file_name, 1)) {
      
  input_stream_.open(file_name.c_str());
  //ReadChar();
}

Lexer::~Lexer() {
  input_stream_.close();
}


void Lexer::Error(const std::string& message) {
  std::stringstream message_stream;
  message_stream << "{ "
                 << current_location_.line()
                 << " }: Lexical error: " << message;

  lexical_errors_->push_back(message_stream.str());
}


void Lexer::ReadChar() {
  if (!input_stream_.get(current_character_))
    current_character_ = EOF;

  if (current_character_ == '\n')
    current_location_.set_line(current_location_.line() + 1);
}


char Lexer::PeekChar() {
  return input_stream_.peek();
}


void Lexer::StepBack(int steps) {
  std::ifstream::pos_type pos;

  if (steps < 1)
    return;

  for (int i = 0; i < steps; i++) {
    if (source_pointers_.empty())
      break;
    pos = source_pointers_.top();
    source_pointers_.pop();
  }
  input_stream_.seekg(pos);
}


Token Lexer::PeekToken(SymbolTable& symbol_table) {
  std::ifstream::pos_type pos = input_stream_.tellg();
  //pos-=1;
  Token token = ScanToken(symbol_table);
  input_stream_.seekg(pos);
  //readChar();
  return token;
}


Token Lexer::GetNextToken(SymbolTable& symbol_table) {
  std::ifstream::pos_type pos = input_stream_.tellg();
  source_pointers_.push(pos);
  return ScanToken(symbol_table);
}


Token Lexer::ScanToken(SymbolTable& symbol_table) {
  //bool is_comment;
  std::string lex_buffer;

start:
  //is_comment = false;
  lex_buffer = "";
  //readChar();

  // Skipping white spaces
  while (isspace(PeekChar()))
    ReadChar();

  // Analyzing an identifier or a keyword
  char next_char = PeekChar();
  if (isalpha(next_char) || next_char == '_') {
    ReadChar();
    lex_buffer += current_character_;
    
    next_char = PeekChar();
    while (isdigit(next_char) || isalpha(next_char) || next_char == '_') {
      ReadChar();
      lex_buffer += current_character_;
      next_char = PeekChar();
    }

    TokenCode token_code;
    const Symbol* symbol = symbol_table[lex_buffer];
    if (symbol == NULL)
      token_code = ID;
    else
      token_code = symbol->token_code();

    return Token(token_code, lex_buffer, current_location_);
  }

  // Analyzing a number literal
  //next_char = peekChar();
  if (isdigit(next_char)) {
    ReadChar();
    lex_buffer += current_character_;

    while (isdigit(PeekChar())){
      ReadChar();
      lex_buffer += current_character_;
    }

    return Token(NUM_LITERAL, lex_buffer,
                 atoi(lex_buffer.c_str()),
                 current_location_);
  }

  // Analyzing a char or string literal
  //next_char = peekChar();
  if (next_char == '\'' || next_char == '"') {
    const char terminator = next_char;
    ReadChar();

    next_char = PeekChar();
    while ((next_char != terminator) && (next_char != EOF)) {
      ReadChar();
      lex_buffer += current_character_;
      next_char = PeekChar();
    }
    if (PeekChar() == terminator)
      ReadChar(); // pass the closing qoutation symbol
    else
      Error("Missing the closing qoutation symbol"); 
    
    str_helper::FindAndReplaceAll(lex_buffer, "\\n", "\n");
    str_helper::FindAndReplaceAll(lex_buffer, "\\t", "\t");
    str_helper::FindAndReplaceAll(lex_buffer, "\\r", "\r");

    // More than single char in a char literal.
    if (terminator == '\'' && lex_buffer.length() > 1)
      Error("More than one character.");

    int token_value;
    if (terminator == '\'')
      token_value = lex_buffer[0];
    else
      token_value = lex_buffer.length();

    if (terminator == '\'') {
      return Token(NUM_LITERAL,
                   std::string(1, lex_buffer[0]),
                   token_value,
                   current_location_);
    } else 
      return Token(STRING_LITERAL, lex_buffer, token_value, current_location_);
  }

  // If none of the conditions above is true, then it's likely an operator
  // or a comment, so we check here for that.
  ReadChar();
  lex_buffer += current_character_ ;
  lex_buffer += PeekChar();
  

  // We assume that it is a two-character token, so we read the second 
  // character here. (I think it needs some fixes! Lines 135, 158)
  /*if (current_character_ != '\0')
    lex_buffer += current_character_;*/

  const TokenType token_type = GetTokenType(lex_buffer);
  switch (token_type) {
  case COMMENT:
    //is_comment = true;
    // if it's a one line comment, skip until the end of line
    ReadChar(); // pass the second char
    if (lex_buffer == "//") {
      next_char = PeekChar();
      while (next_char != '\0' && next_char != '\n' && next_char != EOF) {
        ReadChar();
        next_char = PeekChar();
      }
    }
    // It's a multi-line comment 
    else if (lex_buffer == "/*") {
      ReadChar(); // pass the second char
      do {
        if (PeekChar() != EOF) {
          ReadChar();
          lex_buffer += current_character_;
        }
      } while ((PeekChar() != EOF) && (lex_buffer.length() > 2) &&
        (lex_buffer.substr(lex_buffer.length() - 2, 2) != "*/"));
    }
    
    goto start;
    //break;

  case SINGLE_CHAR_SYMBOL:
    // It is not a two-char token, so we can remove the second character
    // to the unput stream.
    if (lex_buffer.length() > 1) {
      lex_buffer.erase(lex_buffer.length() - 1);
    }
    // Return the token as is, the token code is equal to its ASCII code.
    return Token(static_cast<TokenCode>(lex_buffer[0]),
                 lex_buffer,
                 current_location_);
    //break;

  default:
    // Two-char token
    ReadChar(); // pass the second char
    return Token(static_cast<TokenCode>(token_type),
                 lex_buffer,
                 current_location_);
  }

  // EOF token
  return Token(static_cast<TokenCode>(EOF), "", current_location_);
}


TokenType Lexer::GetTokenType(const std::string& lexeme) {
  const int length = 11;
  struct {
    std::string token;
    int value;
  } lookup_table[length] = {
    { std::string("<=") , LESS_OR_EQUAL },
    { std::string(">=") , GREATER_OR_EQUAL },
    { std::string("==") , EQUAL_EQUAL },
    { std::string("!=") , NOT_EQUAL },
    { std::string("||") , OR },
    { std::string("&&") , AND },
    { std::string("++") , PLUS_PLUS },
    { std::string("--") , MINUS_MINUS },
    { std::string("//") , COMMENT  },
    { std::string("/*") , COMMENT  },
    { std::string( "" ) , 0   }
  };

  for (int i = 0; i < length; i++) {
    if (lookup_table[i].token == lexeme)
      return static_cast<TokenType>(lookup_table[i].value);
  }

  return SINGLE_CHAR_SYMBOL;
}
