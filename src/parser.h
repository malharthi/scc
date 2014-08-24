
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

//
// Parser Header
//

#ifndef INCLUDE_CCOMPX_SRC_PARSER_H__
#define INCLUDE_CCOMPX_SRC_PARSER_H__

#include <stack>
#include <string>
#include <vector>

#include "base.h"
#include "ccomp.h"
#include "lexer.h"
#include "symbol_table.h"
#include "intermediate.h"

// #define RESERVE(lexeme, code) \
//   root_symbol_table_->Insert(lexeme, code);
// 
// #define RESERVE_KEYWORDS           \
//   RESERVE("int", INT)              \
//   RESERVE("char", CHAR)            \
//   RESERVE("void", VOID)            \
//   RESERVE("if", IF)                \
//   RESERVE("else", ELSE)            \
//   RESERVE("for", FOR)              \
//   RESERVE("do", DO)                \
//   RESERVE("while", WHILE)          \
//   RESERVE("switch", SWITCH)        \
//   RESERVE("case", CASE)            \
//   RESERVE("default", DEFAULT)      \
//   RESERVE("return", RETURN)        \
//   RESERVE("break", BREAK)          \
//   RESERVE("continue", CONTINUE)    \
//   RESERVE("printInt", PRINT_INT)   \
//   RESERVE("printStr", PRINT_STR)   \
//   RESERVE("printChar", PRINT_CHAR) \
//   RESERVE("readStr", READ_STR)     \
//   RESERVE("readInt", READ_INT)



class Parser
{
 public:
  Parser(Lexer* lexer, IntermediateInstrsList* interm_list, std::vector<Message>* errors_list);
  ~Parser();

  void Parse();

 private:
  void ReportError(const std::string& message_str);
  void ReportError(TokenCode tok);

  // Get the next token from the lexer
  void AdvanceToNextToken();

  // Skip until a specified token is found
  void SkipToToken(TokenCode code);

  // Match if the given token is found, no error message is generated.
  bool MatchIf(TokenCode code);
  // Match the token and get the next, or generate an error message.
  void Match(TokenCode code);
  // Try to match an array of expected tokens, if non is founed generate an error
  void Match(TokenCode codes[], int n, const std::string& msg);

  // Emits an intermediate instruction into the intermediate code list
  void Emit(IntermediateInstr* instr);
  // Emits a label into the instructions code list
  void EmitLabel(const std::string& label);
  void EmitLabel(LabelOperand* label);

  unsigned int GetStackSize();

  LabelOperand* CreateLabel();
  VariableOperand* CreateTempVariable(DataType type = INT_TYPE,
                           			 			bool is_array = false,
                           			 		 	unsigned int elems = 1);

  void DeclareVariable(DataType type, std::string var_id, bool is_array, unsigned int elems);
  void CopyStringToBuffer(const std::string& array_id, const std::string& text);
  
	void ReserveKeywords();
	
  // Parsing functions

  void ParseFunctions();
  void ParseBlock(FunctionSymbol* func_symbol = NULL);
  void ParseDeclarations();
  void ParseInitialization(const std::string& var_id);
  void ParseStatements();
  void ParseStatement();
  void ParseAssignment(Operand* lhs_operand = NULL);
	void ParseIfStatement();
	void ParseForStatement();
	void ParseWhileStatement();
	void ParseDoStatement();
	void ParseSwitchStatement();
	void ParseBreakStatement();
	void ParseContinueStatement();
	void ParseReturnStatement();
	
	void ParseReadStrStatement();
	void ParseReadIntStatement();
	void ParsePrintCharIntStatement();
	void ParsePrintStrStatement();
	
  Operand* ParseBooleanExpr();
  Operand* ParseAndExpr();
  Operand* ParseEqualityExpr();
  Operand* ParseRelationalExpr();
  Operand* ParseExpression();
  Operand* ParseTermExpr();
  Operand* ParseFactorExpr();
  Operand* ParseIdentifier(bool allow_func);
  Operand* ParseFunctionCall(const std::string& func_id);
  std::vector<Operand*>* ParseArgumentList();

 private:
  // Private members
  unsigned int temp_counter_;
  unsigned int offset_;
  unsigned int label_counter_;

  Lexer* lexer_;

  std::stack<LabelOperand*> break_stack_;
  std::stack<LabelOperand*> continue_stack_;

  SymbolTable* current_scope_table_;
  SymbolTable* root_symbol_table_;
  FunctionSymbol* current_function_;

  Token current_token_;

  std::vector<Message>* errors_list_;
  IntermediateInstrsList* intermediate_code_;

  DISALLOW_COPY_AND_ASSIGN(Parser);
};



#endif // INCLUDE_CCOMPX_SRC_PARSER_H__
