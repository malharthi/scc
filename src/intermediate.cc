
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#include <sstream>

#include "str_helper.h"
#include "intermediate.h"

#include "code_gen.h";

// VariableOperand class implementation

std::string VariableOperand::GetAsmOperand(CodeGenerator& code_gen) {
  std::stringstream operand_stream;
  std::stringstream op1;

  const VariableSymbol* variable_symbol = GetSymbol();

  operand_stream << (variable_symbol->data_type() == INT_TYPE? "dword " : "byte ");
  // Regular local variable (Just return the value)
  if (variable_symbol->kind() == LOCAL) {
    op1 << "[ebp - " // "ptr [ebp - " 
        << (variable_symbol->offset() + variable_symbol->size())
        << "]";
    
    // if (variable_symbol->is_array()) {
    //   code_gen.EmitInstruction("lea", "ebx", op1.str());
    //   operand_stream << "ebx";
    // } else {
      operand_stream << op1.str();
    // }
  } else {
    // symbol kind == ARGUMENT
    // Variable past as an argument (Just access the value)
    
    op1 << "[ebp + " << (variable_symbol->offset() + 8) << "]";
    // "ptr [ebp - "
    // if (variable_symbol->is_array()) {
    //   code_gen.EmitInstruction("lea", "ebx", op1.str());
    //   operand_stream << "ebx";
    // } else {
      operand_stream << op1.str();
    //}
  }
  return operand_stream.str();
}

// ArrayOperand class implementation

std::string ArrayOperand::GetAsmOperand(CodeGenerator& code_gen) {
  std::stringstream operand_stream;
  const VariableSymbol* array_symbol = GetSymbol();

  if (array_symbol->kind() == LOCAL) {
    // Regular static array created locally (Access the value of the element)
    code_gen.LoadOperandToReg("esi", index_operand_);
    operand_stream << (array_symbol->data_type() == INT_TYPE? "dword " : "byte ");
    operand_stream << "[ebp + esi * "
                    // "ptr [ebp + esi * "
                   << array_symbol->element_size()
                   << " - " << (array_symbol->offset() + array_symbol->size())
                   << "]";
  } else {
    // symbol kind = ARGUMENT
    // Array passed as an argument, so we have a pointer
    // Load the address (which is the value passed) to ebx as the base address, then
    // access the value at the required index in esi
    //std::string asm_operand = VariableOperand::GetAsmOperand(code_gen);
    //std::string clean_operand = code_gen.RemoveSizeSpecifier(array_symbol, asm_operand);
    code_gen.EmitInstruction("mov", "ebx", VariableOperand::GetAsmOperand(code_gen));
    //code_gen.LoadEffectiveAddress("ebx", VariableOperand::GetAsmOperand(code_gen));
    //VariableOperand::GetAsmOperand(code_gen);

    code_gen.LoadOperandToReg("esi", index_operand_);
    operand_stream << (array_symbol->data_type() == INT_TYPE? "dword " : "byte ");
    operand_stream << "[ebx + esi * "
                    // "ptr [ebp + esi * "
                   << array_symbol->element_size()
                   //<< " - " << (array_symbol->offset() + array_symbol->size())
                   << "]";
  }

  return operand_stream.str();
}


std::string ArrayOperand::GetIntermediateOperand() {
  return str_helper::FormatString("%s[%s]", data().c_str(),
    index_operand_->GetIntermediateOperand().c_str());
}


// IntermediateInstr class implementation

std::string IntermediateInstr::GetAsString() { 
  switch (operation_) {
  case LABEL_OP:
    return str_helper::FormatString("%s:\n", operand1_->GetIntermediateOperand().c_str());
    //return operand1_->GetIntermediateOperand() + ":";
  case ENTER_OP:
    return str_helper::FormatString("\tenter %s\n",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "enter " + operand1_->GetIntermediateOperand();
  case PARAM_OP:
    return str_helper::FormatString("\tparam %s\n",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "param " + operand1_->GetIntermediateOperand();
  case CALL_OP:
    // return operand1_->GetIntermediateOperand(); + " = call " +
    //   operand2_->GetIntermediateOperand();
    return str_helper::FormatString("\t%s = call %s\n", operand1_->GetIntermediateOperand().c_str(),
      operand2_->GetIntermediateOperand().c_str());
  case RETURN_OP:
    {
      std::string ret = "\treturn ";
      if (operand1_ != NULL)
        ret += operand1_->GetIntermediateOperand();
      ret += "\n";
      return ret;
    }
  case PRINT_INT_OP:
    return str_helper::FormatString("\t%s %s\n", "printInt",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "printInt " + operand1_->GetIntermediateOperand();
  case PRINT_STR_OP:
    return str_helper::FormatString("\t%s %s\n", "printStr",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "printStr " + operand1_->GetIntermediateOperand();
  case PRINT_CHAR_OP:
    return str_helper::FormatString("\t%s %s\n", "printChar",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "printChar " + operand1_->GetIntermediateOperand();
  case READ_INT_OP:
    return str_helper::FormatString("\t%s %s\n", "readInt",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "readInt " + operand1_->GetIntermediateOperand();
  case READ_STR_OP:
    return str_helper::FormatString("\t%s %s\n", "readStr",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "readStr " + operand1_->GetIntermediateOperand();
  case INC_STACK_PTR_OP:
    return str_helper::FormatString("\t%s %s\n", "incStackPtr",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "incStackPtr " + operand1_->GetIntermediateOperand();
  case DEC_STACK_PTR_OP:
    return str_helper::FormatString("\t%s %s\n", "decStackPtr",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "decStackPtr " + operand1_->GetIntermediateOperand();
  case IF_OP:
    // Conditional jump
    return str_helper::FormatString("\tif %s goto %s\n",
                                    operand1_->GetIntermediateOperand().c_str(),
                                    operand2_->GetIntermediateOperand().c_str());
    // return "if " + operand1_->GetIntermediateOperand() +
    //        " goto " + operand2_->GetIntermediateOperand();
  case GOTO_OP:
    // Unconditional jump
    return str_helper::FormatString("\tgoto %s\n",
                                    operand1_->GetIntermediateOperand().c_str());
    //return "goto " + operand1_->GetIntermediateOperand();
  case ASSIGN_OP:
    // Copy instruction (assignment)
    return str_helper::FormatString("\t%s = %s\n",
                                    operand1_->GetIntermediateOperand().c_str(),
                                    operand2_->GetIntermediateOperand().c_str());
    // return operand1_->GetIntermediateOperand() + " = " +
    //   operand2_->GetIntermediateOperand();
  default:
    std::string operation_str;
    switch (operation_) {
    case AND_OP:
      operation_str = "&";
      break;
    case OR_OP:
      operation_str = "|";
      break;
    case EQUAL_EQUAL_OP:
      operation_str = "==";
      break;
    case NOT_EQUAL_OP:
      operation_str = "!=";
      break;
    case LESS_OR_EQUAL_OP:
      operation_str = "<=";
      break;
    case GREATER_OR_EQUAL_OP:
      operation_str = ">=";
      break;
    // Otherwise: one-char tokens, the token code is equal to its ASCII code
    default:
      operation_str = static_cast<char>(operation_);
      break;
    }
    // Arithmatic or logical operations
    if ((operation_ == SUBTRACT_OP || operation_ == NOT_OP) && operand3_ == NULL)
      // Unary operation
      return str_helper::FormatString("\t%s = %s%s\n", operand1_->GetIntermediateOperand().c_str(),
        operation_str.c_str(), operand2_->GetIntermediateOperand().c_str());
    else
      // Binary operation
      return str_helper::FormatString("\t%s = %s %s %s\n", operand1_->GetIntermediateOperand().c_str(),
        operand2_->GetIntermediateOperand().c_str(), operation_str.c_str(),
        operand3_->GetIntermediateOperand().c_str());
  }
}
