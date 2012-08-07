
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

#ifndef INCLUDE_CCOMPX_SRC_CODE_GEN_H__
#define INCLUDE_CCOMPX_SRC_CODE_GEN_H__

#include <string>
#include <ostream>
#include <vector>

#include "intermediate.h"
#include "str_helper.h"

class CodeGenerator {
 public:
  CodeGenerator(std::ostream& output,
                IntermediateInstrsList* interm_code)
    : output_stream_(output),
      intermediate_code(interm_code) {
  }

  void GenerateCode();

  void EmitComment(std::string comment);
  void EmitLabel(const std::string& label);
  void EmitDirective(const std::string& directive);
  
  void EmitInstruction(const std::string& mnem);
  void EmitInstruction(const std::string& mnem, const std::string& p);
  void EmitInstruction(const std::string& mnem, const std::string& p1,
                       const std::string& p2);
  void EmitInstruction(const std::string& mnem, const std::string& p1,
                       const std::string& p2, const std::string& p3);

  void LoadOperandToReg(const std::string& reg, Operand* operand);
  void StoreRegToAddress(Operand* operand, const std::string& reg);
  void LoadEffectiveAddress(const std::string& reg, Operand* operand);
  
 private:
  void WriteAssmblerCodeToStream();
 private:
  std::ostream& output_stream_;
  IntermediateInstrsList* intermediate_code;
  std::vector<std::string> assembler_code_;
};

#endif // INCLUDE_CCOMPX_SRC_CODE_GEN_H__
