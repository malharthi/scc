
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

//
// Symbol Table Representation
//

#include "symbol_table.h"



std::string GetTokenString(TokenCode token_code)
{
  static struct TokenNames {
    hash_map<TokenCode, const char*> token_names;
    TokenNames() {
      token_names[END_OF_FILE] =    "End Of File";
      token_names[PLUS] =         "+";
      token_names[MINUS]=         "-";
      token_names[ASTERISK]=      "*";
      token_names[FORWARD_SLASH]=  "/";
      token_names[PERCENT]=       "%";
      token_names[EQUAL]=         "=";
      token_names[LESS]=          "<";
      token_names[GREATER]=       ">";
      token_names[OPEN_BRACE]=     "{";
      token_names[CLOSE_BRACE]=    "}";
      token_names[OPEN_PAREN]=     "(";
      token_names[CLOSE_PAREN]=    ")";
      token_names[OPEN_BRACKET]=   "[";
      token_names[CLOSE_BRACKET]=  "]";
      token_names[COMMA]=         ",";
      token_names[COLON]=         ":";
      token_names[SEMICOLON]=     ";";
      token_names[EXCLAMATION]=   "!";
      token_names[LESS_OR_EQUAL]=      "<=";
      token_names[GREATER_OR_EQUAL]=   ">=";
      token_names[EQUAL_EQUAL]=    "==";
      token_names[NOT_EQUAL]=         "!=";
      token_names[OR]=            "||";
      token_names[AND]=           "&&";
      token_names[PLUS_PLUS]=      "++";
      token_names[MINUS_MINUS]=    "--";
      token_names[ID]=            "identifier";
      token_names[NUM_LITERAL]=    "number literal";
      token_names[STRING_LITERAL]= "string literal";

      token_names[VOID]=          "void";
      //token_names[BYTE]=          "byte";
      //token_names[SHORT]=         "short";
      token_names[INT]=           "int";
      //token_names[FLOAT]=         "float";
      //token_names[DOUBLE]=        "double";
      token_names[CHAR]=          "char";
      token_names[IF]=            "if";
      token_names[ELSE]=          "else";
      token_names[FOR]=           "for";
      token_names[DO]=            "do";
      token_names[WHILE]=         "while";
      token_names[SWITCH]=        "switch";
      token_names[CASE]=          "case";
      token_names[DEFAULT]=       "default";
      token_names[RETURN]=        "return";
      token_names[BREAK]=         "break";
      token_names[CONTINUE]=      "continue";
      token_names[PRINT_INT]=      "printInt";
      token_names[PRINT_STR]=      "printStr";
      token_names[PRINT_CHAR]=     "printChar";
      token_names[READ_INT]=       "readInt";
      token_names[READ_STR]=       "readStr";

      token_names[GOTO]=          "goto";
    }
  } token_names;
  
  return token_names.token_names.find(token_code)->second;
}



bool SymbolTable::IsInCurrentScope(const std::string& lexeme) const
{
  return table_.find(lexeme) != table_.end();
}



SymbolTable::~SymbolTable()
{
  std::vector<SymbolTable*>::iterator vector_it;

  // Deleting child scopes (tables)
  for (vector_it = inner_scopes_.begin();
       vector_it != inner_scopes_.end();
       vector_it++)
    delete *vector_it;

  // Deleting table symbols 
  for (HashIterator it = table_.begin(); it != table_.end(); it++)
    delete it->second;
}



bool SymbolTable::Insert(const std::string& lexeme, TokenCode code)
{
  bool is_inserted;
  Symbol* symbol = new Symbol(lexeme, code);
  
  if (!(is_inserted = Insert(symbol)))
    delete symbol;

  return is_inserted;
}



bool SymbolTable::Insert(Symbol* symbol)
{
  std::string lexeme = symbol->lexeme();
  HashIterator it = table_.find(lexeme);

  if (it != table_.end())
    return false;

  table_[lexeme] = symbol;
  return true;
}



Symbol* SymbolTable::operator [](const std::string& key)
{
  SymbolTable* current_scope = this;
  do {
    HashIterator it = current_scope->table_.find(key);
    if (it != current_scope->table_.end())
      return it->second;
    current_scope = current_scope->outer_scope_;
  } while (current_scope != NULL);

  return NULL;
}
