
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
#include "code_gen.h"
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

int Compile(const std::string& file, std::vector<Message>& errors_list) {
  // The executable output file name (no extension for *nix systems)
  std::string output_file_name_no_ext = str_helper::RemoveExtensionFromFileName(file);
  // The intermediate code output file name (.intermediate)
  std::string output_file_name_interm =
    str_helper::FormatString("%s.intermediate", output_file_name_no_ext.c_str());
  // The assembler code output file name (.s)
  std::string output_file_name_assembler =
    str_helper::FormatString("%s.s", output_file_name_no_ext.c_str());
  
  IntermediateInstrsList interm_code;
  Lexer lexer(file, &errors_list);
  Parser parser(&lexer, &interm_code, &errors_list);
  parser.Parse();

  int ret_code;

  if (errors_list.size() == 0) {
    // Output intermediate code into a file
    std::ofstream output_file_interm(output_file_name_interm.c_str());
    IntermediateInstrsList::iterator it;
    for (it = interm_code.begin(); it != interm_code.end(); it++) {
      output_file_interm << (*it)->GetAsString();
      //output_file_interm << std::endl;
    }
    output_file_interm.close();

    // Output assembler code into a file
    std::ofstream output_file_assembler(output_file_name_assembler.c_str());
    CodeGenerator code_gen(output_file_assembler, &interm_code);

    // Generate assembler code
    code_gen.GenerateCode();
    output_file_assembler.close();

    // The assembler command for nasm in Linux (ELF 32 bit)
    // For OS X, change output format to macho (32 bit)
    // For debug information, use -g
#if defined __APPLE__
    std::string assembler_cmd = str_helper::FormatString("nasm -f macho -o %s.o %s",
                                                      output_file_name_no_ext.c_str(),
                                                      output_file_name_assembler.c_str());
#else
    // Linux
    std::string assembler_cmd = str_helper::FormatString("nasm -f elf -o %s.o %s",
                                                      output_file_name_no_ext.c_str(),
                                                      output_file_name_assembler.c_str());
#endif
    
    std::string linker_cmd = str_helper::FormatString("gcc -m32 -o %s %s.o",
                                                      output_file_name_no_ext.c_str(),
                                                      output_file_name_no_ext.c_str());
    ret_code = system(assembler_cmd.c_str());
    if (ret_code == 0) {
      ret_code = system(linker_cmd.c_str());
    }
  } else {
    ret_code = 1;
  }

  return ret_code;
}

int main(int argc, char* argv[]) {
  std::cout << "Simple C Compiler (SCC)" << std::endl;
  if (argc < 2) {
    std::cout << "No sufficient input" << std::endl;
    return 1;
  }

  int ret_code = 0;

  std::string args[argc];
  for (int i = 0; i < argc; i++)
    args[i] = argv[i];

  std::string file = argv[1];
  std::vector<Message> errors_list;

  if (argc > 2) {
    if (args[2] == std::string("lex")) {
      Lex(file, errors_list);
    }
  }
  // else if ()
  else
    ret_code = Compile(file, errors_list);
  
  if (!errors_list.empty()) {
    ret_code = 1;
    std::vector<Message>::iterator it;
    for (it = errors_list.begin(); it != errors_list.end(); it++) {
      std::cout << it->location().line() << ": ";
      std::cout << it->message() << std::endl;
    }
  }

  return ret_code;
}
