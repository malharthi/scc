
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#ifndef INCLUDE_CCOMPX_SRC_SYMBOL_TABLE_H__
#define INCLUDE_CCOMPX_SRC_SYMBOL_TABLE_H__

#include <string>
#include <vector>
#include <cstdio>

#if defined __GNUC__ || defined __APPLE__
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
#endif

#include "base.h"
#include "ccomp.h"
#include "str_helper.h"

// Token codes, each token is identified by an integer.

// Token names chosen in this enum are the names of the plain symbols
// which have no relation with the meaning of the symbol in the context of the
// programming lanauge. At the level of lexical analysis, symbols
// are independent from their meaning, and the semantic analysis has yet to
// figure out the meaning of the symbols. For instance, the symbol '*' is called
// 'ASTERISK' or possibly a 'STAR'. It can refer to different language elements,
// such as a pointer, or a muliplication sign, so in the enum we call just a an
// asterisk or a star, until the parser or the semantic analyer figures out the
// meaning from the context.

enum TokenCode {
  END_OF_FILE = EOF,
  PLUS = (int)'+',
  MINUS = (int)'-',
  ASTERISK = (int)'*',
  FORWARD_SLASH = (int)'/',
  PERCENT = (int)'%',
  EQUAL = (int)'=',
  LESS = (int)'<',
  GREATER = (int)'>',
  OPEN_BRACE = (int)'{',
  CLOSE_BRACE = (int)'}',
  OPEN_PAREN = (int)'(',
  CLOSE_PAREN = (int)')',
  OPEN_BRACKET = (int)'[',
  CLOSE_BRACKET = (int)']',
  COMMA = (int)',',
  COLON = (int)':',
  SEMICOLON = (int)';',
  EXCLAMATION = (int)'!',

  LESS_OR_EQUAL = 400,
  GREATER_OR_EQUAL = 401,
  EQUAL_EQUAL = 402,
  NOT_EQUAL = 403,
  OR = 404,
  AND = 405,
  PLUS_PLUS = 406,
  MINUS_MINUS = 407,
  ID = 256,
  NUM_LITERAL = 257,
  STRING_LITERAL = 258,

  // Keyowrds go here
  VOID = 300,
  //BYTE,
  //SHORT,
  INT,
  //FLOAT,
  //DOUBLE,
  CHAR,
  IF,
  ELSE,
  FOR,
  DO,
  WHILE,
  SWITCH,
  CASE,
  DEFAULT,
  RETURN,
  BREAK,
  CONTINUE,
  PRINT_INT,
  PRINT_STR,
  PRINT_CHAR,
  READ_INT,
  READ_STR,
  // End of keywords

  GOTO
};

// Represents the type of variables and return types
enum DataType {
  INT_TYPE,
  CHAR_TYPE,
  VOID_TYPE
};

// Indicates if the variable is a local variable or a parameter
enum VariableKind {
  LOCAL,
  ARGUMENT
};

enum TokenType {
  COMMENT,
  SINGLE_CHAR_SYMBOL,
  TWO_CHAR_SYMBOL
};

// A hash function for hash_map<std::string, Token*>,
// since the stl does not provide one for maps with std::string keys
#if defined __GNUC__ || defined __APPLE__
namespace __gnu_cxx
#else
namespace std
#endif
{
template<>
struct hash<std::string> {
  size_t operator()(const std::string& x) const {
    return hash< const char* >()(x.c_str());
  }
};

template<>
struct hash<TokenCode> {
  size_t operator()(const TokenCode x) const {
    return hash<int>()(static_cast<int>(x));
  }
};
} // namesapce

std::string GetTokenString(TokenCode token_code);

// class DataTypeSpec {
//  public:
//   DataTypeSpec() { }
//   DataTypeSpec(DataType type, bool is_ptr, bool is_const)
//     : data_type_(type),
//       //is_pointer_(is_ptr),
//       is_const_(is_const) {
//   }

//   DataType data_type_;
//   bool is_pointer_;
//   bool is_const_;
// };

// Represents a token in the source.
class Token {
 public:
  // Creates a new Token object given a code and lexeme.
  Token() { }
  Token(TokenCode token_code, const std::string& lexeme, SourceLocation loc)
    : code_(token_code),
      lexeme_(lexeme),
      location_(loc) {
  }
  Token(TokenCode token_code, const std::string& lexeme, int value, SourceLocation loc)
    : code_(token_code),
      lexeme_(lexeme),
      value_(value),
      location_(loc) {
  }

  // Accessors
  TokenCode code() const {
    return code_;  
  }
  int value() const {
    return value_;
  }
  std::string lexeme() const {
    return lexeme_; 
  }
  SourceLocation location() const {
    return location_;
  }

 private:
  TokenCode code_;
  int value_;
  std::string lexeme_;
  SourceLocation location_;
};

// Represents a symbol in the symbol table, which is the base class
// for the rest of the symbol table classes.
class Symbol {
 public:
  // Creates a new Symbol object given a lexeme and a token code.
  Symbol(const std::string& lexeme, TokenCode code)
    : lexeme_(lexeme),
      token_code_(code) {
  }

  virtual ~Symbol() { }

  // Accessors
  std::string lexeme() const {
    return lexeme_; 
  }
  TokenCode token_code() const {
    return token_code_; 
  }

  bool IsKeyword() const {
    return (token_code_ >= VOID && token_code_ <= READ_STR);
  }
private:
  std::string lexeme_;
  TokenCode token_code_;
};


// Represnts a symbole that holds a variable info.
class VariableSymbol : public Symbol {
 public:
  // Creates a new VariableSymbol given the variable's identifier.
  VariableSymbol(const std::string& identifier)
    : Symbol(identifier, ID) {
  }

  // Accessors
  unsigned int offset() const {
    return offset_;
  }
  unsigned int element_size() const {
    return element_size_;
  }
  unsigned int size() const {
    return size_;
  }
  bool is_array() const {
    return is_array_;
  }
  const DataType data_type() const {
    return data_type_;
  }
  VariableKind kind() const {
    return kind_;
  }

  // Mutators
  void set_offset(unsigned int value) {
    offset_ = value;  
  }
  void set_element_size(unsigned int value) {
   element_size_ = value;
  }
  void set_size(unsigned int value) {
   size_ = value;
  }
  void set_is_array(bool value) {
    is_array_ = value;
  }
  void set_data_type(DataType data_type) {
    data_type_ = data_type;
  }
  void set_kind(VariableKind kind) {
    kind_ = kind;
  }

 private:
  unsigned int offset_;
  unsigned int element_size_;
  unsigned int size_;
  bool is_array_;
  DataType data_type_;
  VariableKind kind_;
};

class Parameter;

// Represents the symbol that hold a function info.
class FunctionSymbol : public Symbol {
 public:
  // Create a new FunctionSymbol given the function name (Identifier)
  FunctionSymbol(const std::string& identifier,
                DataType return_type)
    : Symbol(identifier, ID),
      return_type_(return_type) {
  }
  //~FunctionSymbol();

  // Accessors
  DataType return_type() const {
    return return_type_;    
  }

public:
  std::vector<Parameter> parameters_;

 private:
  DataType return_type_;
  
};

// Represents a parameter to a function
class Parameter {
 public:
  // Creates a new Parameter object given a type, an identifier and if
  // it is an array or not.
  Parameter(DataType type,
            const std::string& identifier,
            bool is_array)
    : type_(type),
    identifier_(identifier),
    is_array_(is_array) {
  }

  // Accessors
  DataType type() const {
    return type_; 
  }
  std::string identifier() const {
    return identifier_;
  }
  bool is_array() const {
    return is_array_;
  }

 private:
  DataType type_;
  std::string identifier_;
  bool is_array_;
};


// Represnts our symbol table. This class is responsible for deallocating the
// associated symbol objects. The root table also is responsible for 
// deallocating its child symbol tables.
class SymbolTable {
 public:
  // Creates a new symbol table object given the parent table (the
  // parent scope). If it is the root table then pass NULL. 
  SymbolTable(SymbolTable* prev = NULL)
    : outer_scope_(prev) {
  }
  ~SymbolTable();

  // Insert a new symbol in the table
  bool Insert(Symbol* symbol);
  bool Insert(const std::string& lexeme, TokenCode code);

  // Lookup for a symbol and then return it.
  // TODO: Must be replaced with an overload for the [] operator.
  //const Symbol* Lookup(const std::string& lexeme) const;

  // Return true if the given lexeme corresponds to a symbol in the
  // current symbol table i.e., the current scope.
  bool IsInCurrentScope(const std::string& lexeme) const;

  // A replacement for the outdated Lookup function.
  Symbol* operator[] (const std::string& key);

  std::vector<SymbolTable*> inner_scopes_;
 
 // Accessors
  SymbolTable* outer() {
    return outer_scope_;
  }
  
 private:
   typedef hash_map<std::string, Symbol*>::iterator HashIterator;

   SymbolTable* outer_scope_;
   hash_map<std::string, Symbol*> table_;

   DISALLOW_COPY_AND_ASSIGN(SymbolTable);
};

#endif // INCLUDE_CCOMPX_SRC_SYMBOL_TABLE_H__
