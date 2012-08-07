
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#ifndef INCLUDE_CCOMPX_SRC_INTERMEDIATE_H__
#define INCLUDE_CCOMPX_SRC_INTERMEDIATE_H__

#include <string>
#include <sstream>
#include <vector>

// #include "code_gen.h"
#include "symbol_table.h"

class CodeGenerator;

// Intermediate opcodes. Number of operands and their order explained in the
// comments in front of each opcode, or on top of each group of them.
enum IntermediateOp {
  // Three-parameter instructions
  // res = operand1 op operand3
  ASSIGN_OP = (int)'=',
  ADD_OP = (int)'+',
  SUBTRACT_OP = (int)'-',
  MULTIPLY_OP = (int)'*',
  DIVIDE_OP = (int)'/',

  NOT_OP = (int)'!', // res = not operand1

  // Three-parameter
  DIV_REMINDER_OP = (int)'%',
  LESS_THAN_OP = (int)'<',
  GREATER_THAN_OP = (int)'>',
  LESS_OR_EQUAL_OP = 400,
  GREATER_OR_EQUAL_OP = 401,
  EQUAL_EQUAL_OP = 402,
  NOT_EQUAL_OP = 403,
  OR_OP = 404,
  AND_OP = 405,

  IF_OP,                // if operand1 goto operand2(label)
  GOTO_OP,              // goto operand1(label)
  LABEL_OP,             // no operands
  INC_STACK_PTR_OP,     // inc operand1
  DEC_STACK_PTR_OP,     // inc operand2
  PARAM_OP,             // param operand1
  ENTER_OP,             // param operand2
  CALL_OP,              // call ret_Op(operand1), operand2(label-dest) i.e.
                        // ret_op = call label
  RETURN_OP,            // return operand1(ret value)

  PRINT_INT_OP,
  PRINT_STR_OP,
  PRINT_CHAR_OP,
  READ_INT_OP,
  READ_STR_OP
};

// This interface must be implemented by all classes that represents the 
// intermediate code structure.
class Operand {
 public:
  // Return the textual representation of the operand in the assembler language
  virtual std::string GetAsmOperand(CodeGenerator& code_gen) = 0;
  // Return the textual representation of the operand for our intermediate
  // language.
  virtual std::string GetIntermediateOperand() = 0;
};

template<typename T>
class BasicOperand : public Operand {
 public:
  BasicOperand(const T& data)
    : data_(data) {
  }

  // Overrides the base class
  virtual std::string GetAsmOperand(CodeGenerator& code_gen) {
    return GetIntermediateOperand();
  }

  virtual std::string GetIntermediateOperand() {
    std::stringstream string_stream;
    string_stream << data();
    return string_stream.str();
  }

  // Return a copy of the actual value
  T data() const {
    return data_;
  }

 private:
  // The actual data
  T data_;
};

// Represents a label operand in the intermediate language
typedef BasicOperand<std::string> LabelOperand;
// Represents a label operand in the intermediate language (number literal)
typedef BasicOperand<int> NumberOperand;
// Represents a function operand in the intermediate language
typedef BasicOperand<std::string> FunctionOperand;

// Represents a variable operand in the intermediate language
class VariableOperand : public BasicOperand<std::string> {
 public:
  VariableOperand(const std::string& identifier, SymbolTable* symbol_table)
   : BasicOperand<std::string>(identifier),
     symbol_table_(symbol_table) {
  }
  
  // Gets the Symbol object from the associated symbol table
  const VariableSymbol* GetSymbol() {
    return static_cast<const VariableSymbol*>( (*symbol_table_)[data()] );
  }

  // Overrides the base class 
  virtual std::string GetAsmOperand(CodeGenerator& code_gen);

 protected:
  SymbolTable* symbol_table_;
};

// Represents an array operand in the intermediate language
class ArrayOperand : public VariableOperand {
 public:
  ArrayOperand(const std::string& identifier, Operand* index,
               SymbolTable* symbol_table)
    : VariableOperand(identifier, symbol_table),
      index_operand_(index) {
  }
  
  // Overrides the base class
  virtual std::string GetAsmOperand(CodeGenerator& code_gen);
  virtual std::string GetIntermediateOperand();

private:
  Operand* index_operand_;
};

// Represents a single instruction in the intermediate language
class IntermediateInstr {
 public:
  IntermediateInstr(IntermediateOp op, Operand* operand1 = NULL,
                    Operand* operand2 = NULL, Operand* operand3 = NULL)
  : operation_(op),
    operand1_(operand1),
    operand2_(operand2),
    operand3_(operand3) {
  }

  IntermediateOp operation() {
    return operation_;
  }
  Operand* operand1() {
    return operand1_;
  }
  Operand* operand2() {
    return operand2_;
  }
  Operand* operand3() {
    return operand3_;
  }

  // Gets the textual instruction representation 
  std::string GetAsString();

 private:
  IntermediateOp operation_;
  Operand* operand1_;
  Operand* operand2_;
  Operand* operand3_;
};

typedef std::vector<IntermediateInstr*> IntermediateInstrsList;

#endif // INCLUDE_CCOMPX_SRC_INTERMEDIATE_H__
