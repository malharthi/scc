
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#include <iostream>
#include <fstream>
#include <vector>

#include "ccomp.h"
#include "lexer.h"
#include "parser.h"
#include "str_helper.h"

void PrintToken(Token tok) {
  std::cout << tok.lexeme() /*<< " | " << tok.code()*/ << std::endl;
}


void Lex(const std::string file, std::vector<Message>& errors_list) {
  Lexer lexer(file, &errors_list);
  
  SymbolTable symbol_table;
  Token token = lexer.GetNextToken(symbol_table);

  do {
   std::cout << token.lexeme() /*<< " | " << token.code()*/ << std::endl;
   token = lexer.GetNextToken(symbol_table);
  } while (token.code() != END_OF_FILE);
}


void Compile(const std::string file, std::vector<Message>& errors_list) {
  std::string output_file_name = str_helper::RemoveExtensionFromFileName(file);
  IntermediateInstrsList interm_code;

  Lexer lexer(file, &errors_list);
  Parser parser(&lexer, &interm_code, &errors_list);
  parser.Parse();

  if (errors_list.size() == 0) {
    std::ofstream output_file(output_file_name.c_str());
    IntermediateInstrsList::iterator it;
    for (it = interm_code.begin(); it != interm_code.end(); it++) {
      output_file << (*it)->GetAsString();
      //output_file << std::endl;
    }
    output_file.close();
  }
}


int main(int argc, char* argv[]) {
  std::cout << "Simple C Compiler (SCC)" << std::endl;
  if (argc < 2) {
    std::cout << "No sufficient arguments" << std::endl;
    return 1;
  }

  std::string args[argc];
  for (int i = 0; i < argc; i++)
    args[i] = argv[i];

  std::string file = argv[1];
  std::vector<Message> errors_list;

  if (argc > 2) {
    if (args[2] == std::string("lex"))
      Lex(file, errors_list);
  }
  // else if ()
  else
    Compile(file, errors_list);
  
  if (!errors_list.empty()) {
    std::vector<Message>::iterator it;
    for (it = errors_list.begin(); it != errors_list.end(); it++) {
      std::cout << it->location().line() << ": ";
      std::cout << it->message() << std::endl;
    }
  }

  return 0;
}
