
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#include <sstream>

#include "code_gen.h"

#if defined __APPLE__
  const char* printf_str = "_printf";
  const char* scanf_str  = "_scanf";
  const char* gets_str   = "_gets";
#else
  const char* printf_str = "printf";
  const char* scanf_str  = "scanf";
  const char* gets_str   = "gets";
#endif

// Emiting an intermediate instruction as a comment before the its translation
// to assembler code.
void CodeGenerator::EmitComment(std::string comment) {
  //assembler_code.push_back(str_helper::FormatString("\t%s", comment.c_str()));
  
  // Remove the tab character at the begining of the intermediate instruction
  comment.erase(0, 1);
  // Remove the new line character at the end of the intermediate instruction
  comment.erase(comment.length() - 1, 1);

  std::stringstream s;
  s << "\t;" << comment << "\n";
  assembler_code_.push_back(s.str());
}


void CodeGenerator::EmitLabel(const std::string& label) {
  std::stringstream s;
  s << label << ":\n";
  assembler_code_.push_back(s.str());
}


void CodeGenerator::EmitDirective(const std::string& directive) {
  assembler_code_.push_back(str_helper::FormatString("%s\n", directive.c_str()));
}


void CodeGenerator::EmitInstruction(const std::string& mnem) {
  assembler_code_.push_back(str_helper::FormatString("\t%s\n", mnem.c_str()));
}


void CodeGenerator::EmitInstruction(const std::string& mnem,
                                    const std::string& p) {
  std::string instruction;
  instruction = str_helper::FormatString("\t%s \t%s\n", mnem.c_str(), p.c_str());
  assembler_code_.push_back(instruction);
}


void CodeGenerator::EmitInstruction(const std::string& mnem,
                                    const std::string& p1,
                                    const std::string& p2) {
  std::string instruction;
  instruction = str_helper::FormatString("\t%s \t%s, %s\n", mnem.c_str(),
                                         p1.c_str(), p2.c_str());
  assembler_code_.push_back(instruction);
}


void CodeGenerator::EmitInstruction(const std::string& mnem,
                                    const std::string& p1,
                                    const std::string& p2,
                                    const std::string& p3) {
  std::string instruction;
  instruction = str_helper::FormatString("\t%s \t%s, %s, %s\n", mnem.c_str(),
                                         p1.c_str(), p2.c_str(), p3.c_str());
  assembler_code_.push_back(instruction);
}


// Generates a code that loads the content of a memory location
// into a register
void CodeGenerator::LoadOperandToReg(const std::string& reg,
                                     Operand* operand) {
  
  std::string move_instr = "mov";
  
  VariableOperand* var_op = dynamic_cast<VariableOperand*>(operand);
  if (var_op != NULL) {
    VariableSymbol* symbol = var_op->GetSymbol();

    if (symbol->is_array()) {
      LoadEffectiveAddress(reg, operand);
      return;
    }

    if (symbol->data_type() == CHAR_TYPE) {
      move_instr = "movsx";
    }
  }

  EmitInstruction(move_instr, reg, operand->GetAsmOperand(*this));
}


// Generates a code that transfters the content of a register into
// a memory location (Do not use any index registers here)
void CodeGenerator::StoreRegToAddress(Operand* operand,
                                      const std::string& reg) {
  std::string src_register = reg;

  VariableOperand* var_op = dynamic_cast<VariableOperand*>(operand);
  if (var_op != NULL) {
    if (var_op->GetSymbol()->data_type() == CHAR_TYPE) {
      src_register = reg[1] + "l";
    }
  }

  EmitInstruction("mov", operand->GetAsmOperand(*this), src_register);
}

// This function does a dirty trick, which is removing the 'dword' or 'byte'
// keywords from the assembler operands returned from GetAsmOperand.
void CodeGenerator::LoadEffectiveAddress(const std::string& reg, Operand* operand) {
  VariableOperand* var_op = dynamic_cast<VariableOperand*>(operand);
  std::string asm_operand = var_op->GetAsmOperand(*this);

  std::string nasm_keyword;
  nasm_keyword = var_op->GetSymbol()->data_type() == INT_TYPE? "dword " : "byte ";

  str_helper::FindAndReplaceAll(asm_operand, nasm_keyword.c_str(), "");

  EmitInstruction("lea", reg, asm_operand);
}


void CodeGenerator::GenerateCode() {
  // Generate data and code segments and initilize data
  //
#if defined __APPLE__
  EmitDirective("extern _printf, _scanf, _gets");
#else
  EmitDirective("extern printf, scanf, gets");
#endif

  EmitDirective("segment .data");
  EmitDirective("__print_read_Int_format: db \"%d\",0,0");
  EmitDirective("__printChar_format: db \"%c\",0,0");
  EmitDirective("__read_Str_format: db \"%s\",0,0");
  
  EmitDirective("segment .text");

#if defined __APPLE__ 
  EmitDirective("global _main");
#else
  EmitDirective("global main");
#endif

  IntermediateInstrsList::iterator it;
  for (it = intermediate_code->begin(); it != intermediate_code->end(); it++) {
    IntermediateInstr* interm_instr =  (*it);

    // Emit a commented intermediate instruction before
    // each set of assembler code, exclude labels
    if (interm_instr->operation() != LABEL_OP) {
      EmitComment(interm_instr->GetAsString());
    }

    switch (interm_instr->operation()) {
    case LABEL_OP:
      EmitLabel(interm_instr->operand1()->GetAsmOperand(*this));
      break;

    case ASSIGN_OP:
      // if operand2 is a VariableOperand
      if (dynamic_cast<VariableOperand*>(interm_instr->operand2()) != NULL) {
        LoadOperandToReg("eax", interm_instr->operand2());
        StoreRegToAddress(interm_instr->operand1(), "eax");
      } else {
        EmitInstruction("mov", interm_instr->operand1()->GetAsmOperand(*this),
                     interm_instr->operand2()->GetAsmOperand(*this));
      }
      break;

    case SUBTRACT_OP:
      if (interm_instr->operand3() == NULL) {
        // Negating operation
        // x = -y
        LoadOperandToReg("eax", interm_instr->operand2());
        EmitInstruction("neg", "eax");
        StoreRegToAddress(interm_instr->operand1(), "eax");
      } else {
        // x = y - z
        LoadOperandToReg("eax", interm_instr->operand2());
        EmitInstruction("sub", "eax", interm_instr->operand3()->GetAsmOperand(*this));
        StoreRegToAddress(interm_instr->operand1(), "eax");
      }
      break;

    case ADD_OP:
    case AND_OP:
    case OR_OP:
      {
        std::string instruction_mnem;
        switch (interm_instr->operation()) {
        case ADD_OP:
          instruction_mnem =  "add";
          break;

        case AND_OP:
          instruction_mnem = "and";
          break;

        case OR_OP:
          instruction_mnem = "or";
          break; 
        }
        LoadOperandToReg("eax", interm_instr->operand2());
        EmitInstruction(instruction_mnem, "eax",
                        interm_instr->operand3()->GetAsmOperand(*this));
        StoreRegToAddress(interm_instr->operand1(), "eax");
      }
      break;

    case MULTIPLY_OP:
      {
        std::string operand3;
        LoadOperandToReg("eax", interm_instr->operand2());

        // If operand 3 is a number operand (Immidiate operand)
        if (dynamic_cast<NumberOperand*>(interm_instr->operand3()) != NULL) {
          operand3 = "ecx";
          EmitInstruction("mov", "ecx",
                          interm_instr->operand3()->GetAsmOperand(*this));
        } else {
          operand3 = interm_instr->operand3()->GetAsmOperand(*this);
        }
        EmitInstruction("imul", operand3);
        StoreRegToAddress(interm_instr->operand1(), "eax");
      }
      break;

    case DIVIDE_OP:
    case DIV_REMINDER_OP:
      LoadOperandToReg("eax", interm_instr->operand2());
      // Extend eax sign to edx
      EmitInstruction("cdq");

      if (dynamic_cast<NumberOperand*>(interm_instr->operand3()) != NULL) {
        // Immidiate operands are not allowed in division
        EmitInstruction("mov", "ecx", interm_instr->operand3()->
                                                      GetAsmOperand(*this));
        EmitInstruction("idiv", "ecx");
      } else {
        EmitInstruction("idiv", interm_instr->operand3()->GetAsmOperand(*this));
      }

      if (interm_instr->operation() == DIVIDE_OP) {
        // Return the quotient
        StoreRegToAddress(interm_instr->operand1(), "eax");
      } else {
        // A DIV_REMINDER_OP operation, so we return the remainder
        StoreRegToAddress(interm_instr->operand1(), "edx");
      }
      break;

    case NOT_OP:
      EmitInstruction("xor", "eax", "eax");
      LoadOperandToReg("edx", interm_instr->operand2());
      EmitInstruction("cmp", "edx", "0");
      EmitInstruction("sete", "al");
      StoreRegToAddress(interm_instr->operand1(), "eax");       
      break;

    case LESS_THAN_OP:
    case GREATER_THAN_OP:
    case LESS_OR_EQUAL_OP:
    case GREATER_OR_EQUAL_OP:
    case EQUAL_EQUAL_OP:
    case NOT_EQUAL_OP:
      {
        std::string instruction_mnem;

        switch (interm_instr->operation()) {
        case LESS_THAN_OP:
          instruction_mnem = "setl";
          break;
        case GREATER_THAN_OP:
          instruction_mnem = "setg";
          break;
        case LESS_OR_EQUAL_OP:
          instruction_mnem = "setle";
          break;
        case GREATER_OR_EQUAL_OP:
          instruction_mnem = "setge";
          break;
        case EQUAL_EQUAL_OP:
          instruction_mnem = "sete";
          break;
        case NOT_EQUAL_OP:
          instruction_mnem = "setne";
          break;
        }

        EmitInstruction("xor", "edx", "edx");
        LoadOperandToReg("eax", interm_instr->operand2());
        LoadOperandToReg("ecx", interm_instr->operand3());
        EmitInstruction("cmp", "eax", "ecx");
        EmitInstruction(instruction_mnem, "dl");
        StoreRegToAddress(interm_instr->operand1(), "edx");
      }
      break;

    case IF_OP:
      LoadOperandToReg("eax", interm_instr->operand1());
      EmitInstruction("cmp", "eax", "0");
      EmitInstruction("jne", interm_instr->operand2()->GetAsmOperand(*this));
      break;

    case GOTO_OP:
      EmitInstruction("jmp", interm_instr->operand1()->GetAsmOperand(*this));
      break;

    case PRINT_INT_OP:
      LoadOperandToReg("eax", interm_instr->operand1());
      EmitInstruction("push", "eax");
      EmitInstruction("push", "dword __print_read_Int_format");
      EmitInstruction("call", printf_str);
      EmitInstruction("add", "esp", "8");
      break;

    case PRINT_CHAR_OP:
      LoadOperandToReg("eax", interm_instr->operand1());
      EmitInstruction("push", "eax");
      EmitInstruction("push", "dword __printChar_format");
      EmitInstruction("call", printf_str);
      EmitInstruction("add", "esp", "8");
      break;

    case PRINT_STR_OP:
      EmitInstruction("sub", "esp", "4");
      //EmitInstruction("lea", "eax", interm_instr->operand1()->GetAsmOperand(*this));
      LoadEffectiveAddress("eax", interm_instr->operand1());
      EmitInstruction("push", "eax");
      EmitInstruction("call", printf_str);
      EmitInstruction("add", "esp", "8");
      break;

    case READ_STR_OP:
      EmitInstruction("sub", "esp", "4");
      LoadEffectiveAddress("eax", interm_instr->operand1());
      EmitInstruction("push", "eax");
      EmitInstruction("call", gets_str);
      EmitInstruction("add", "esp", "8");
      // LoadEffectiveAddress("eax", interm_instr->operand1());
      // EmitInstruction("push", "eax");
      // EmitInstruction("push", "dword __read_Str_format");
      // EmitInstruction("call", "_scanf");
      // EmitInstruction("add", "esp", "8");
      break;

    case READ_INT_OP:
      //EmitInstruction("lea", "eax", interm_instr->operand1()->GetAsmOperand(*this));
      LoadEffectiveAddress("eax", interm_instr->operand1());
      EmitInstruction("push", "eax");
      EmitInstruction("push", "dword __print_read_Int_format");
      EmitInstruction("call", scanf_str);
      EmitInstruction("add", "esp", "8");
      break;

    case INC_STACK_PTR_OP:
      EmitInstruction("add", "esp", 
                      interm_instr->operand1()->GetAsmOperand(*this));
      break;

    case DEC_STACK_PTR_OP:
      EmitInstruction("sub", "esp",
                      interm_instr->operand1()->GetAsmOperand(*this));
      break;

    case ENTER_OP:
      // EmitInstruction("enter",
      //                 interm_instr->operand1()->GetAsmOperand(*this), "0");
      EmitInstruction("push", "ebp");
      EmitInstruction("mov", "ebp", "esp");
      //EmitInstruction("push", "ebx"); // From Apple docs
      EmitInstruction("sub", "esp", interm_instr->operand1()->GetAsmOperand(*this));
      break;

    case PARAM_OP:
      {
        VariableOperand* var_op =
          dynamic_cast<VariableOperand*>(interm_instr->operand1());
        if ((var_op != NULL) && (var_op->GetSymbol()->data_type() == CHAR_TYPE)) {
          LoadOperandToReg("eax", var_op);
          EmitInstruction("push", "eax");
        } else {
          EmitInstruction("push", interm_instr->operand1()->GetAsmOperand(*this));
        }
      }
      break;

    case CALL_OP:
      EmitInstruction("call", interm_instr->operand2()->GetAsmOperand(*this));
      StoreRegToAddress(interm_instr->operand1(), "eax");
      break;

    case RETURN_OP:
      if (interm_instr->operand1() != NULL) {
        LoadOperandToReg("eax", interm_instr->operand1());
      }

      //EmitInstruction("leave");

      EmitInstruction("mov", "esp", "ebp");
      //EmitInstruction("add", "esp", interm_instr->operand1()->GetAsmOperand(*this));

      //EmitInstruction("pop", "ebx");
      EmitInstruction("pop", "ebp");
      
      EmitInstruction("ret");
      break;

    }
  }

  WriteAssmblerCodeToStream();
}


void CodeGenerator::WriteAssmblerCodeToStream() {
  std::vector<std::string>::iterator it;
  for (it = assembler_code_.begin(); it != assembler_code_.end(); it++) {
    output_stream_ << (*it);
  }
  output_stream_.flush();
}

