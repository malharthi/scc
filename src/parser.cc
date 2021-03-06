
// Copyright (c) 2009 Mohannad Alharthi (mohannad.harthi@gmail.com)
// All rights reserved.
// This source code is licensed under the BSD license, which can be found in
// the LICENSE.txt file.

//
// Parser
//

#include "parser.h"



Parser::Parser(Lexer* lexer,
               IntermediateInstrsList* interm_list,
               std::vector<Message>* errors_list)
  : intermediate_code_(interm_list),
    errors_list_(errors_list),
    lexer_(lexer),
    root_symbol_table_(new SymbolTable(NULL)),
    temp_counter_(0),
    label_counter_(0),
    offset_(0)
{
  current_scope_table_ = root_symbol_table_;
  ReserveKeywords();
}



Parser::~Parser()
{
  // TODO: Deallocating symbol tables.
  //       Deallocating intermediate instructions objects
}



void Parser::ReportError(const std::string& message_str)
{
  SourceLocation loc(lexer_->source_file(),
                     lexer_->current_location().line(), 0);
  Message message(message_str, loc);
  errors_list_->push_back(message);
}



void Parser::ReportError(TokenCode tok)
{
  std::string msg =
    str_helper::FormatString("'%s' expected.", GetTokenString(tok).c_str());
  ReportError(msg);
}



void Parser::AdvanceToNextToken()
{
  current_token_ = lexer_->GetNextToken(*current_scope_table_);
}



bool Parser::MatchIf(TokenCode code)
{
  if (code == current_token_.code()) {
    current_token_ = lexer_->GetNextToken(*current_scope_table_);
    return true;
  }
  return false;
}



void Parser::Match(TokenCode code)
{
  // if (code == current_token_.code())
  //   current_token_ = lexer_->GetNextToken(*current_scope_table_);
  // else {
  //   ReportError(str_helper::FormatString("'%s' expected.", GetTokenString(code).c_str()));
  //   skipToToken(SEMICOLON);
  //  }
  if (code != current_token_.code()) {
    ReportError(str_helper::FormatString("'%s' expected.", GetTokenString(code).c_str()));
  }
  current_token_ = lexer_->GetNextToken(*current_scope_table_);
}



void Parser::Match(TokenCode codes[], int n, const std::string& msg)
{
  for (int i = 0; i < n; ++i){
    if (codes[i] == current_token_.code()) {
      current_token_ = lexer_->GetNextToken(*current_scope_table_);
      return;
    }
  }
  // No tokens were found
  ReportError(msg);
  // skipToToken(SEMICOLON);
}



void Parser::SkipToToken(TokenCode code)
{
  do {
    AdvanceToNextToken();
  } while (current_token_.code() != code ||
           current_token_.code() != END_OF_FILE);
  AdvanceToNextToken();
}



void Parser::Emit(IntermediateInstr* instr)
{
  intermediate_code_->push_back(instr);
}



void Parser::EmitLabel(LabelOperand* label)
{
  IntermediateInstr* instr = new IntermediateInstr(LABEL_OP, label);
  Emit(instr);
}



void Parser::EmitLabel(const std::string& label)
{
  LabelOperand* label_operand = new LabelOperand(label);
  IntermediateInstr* instr = new IntermediateInstr(LABEL_OP, label_operand);
  Emit(instr);
}



unsigned int Parser::GetStackSize()
{
  while (offset_ % 16 != 0)
      offset_++;
  return offset_;
}



LabelOperand* Parser::CreateLabel()
{
  std::string label_id = str_helper::FormatString("label_%d", label_counter_++);
  LabelOperand* label_operand = new LabelOperand(label_id);
  return label_operand;
}



VariableOperand* Parser::CreateTempVariable(DataType type,
                               							bool is_array,
                               						 	unsigned int elems)
{
  // Create a symbol for the temporary variable since we need it to be inserted
  // into the symbol table for the current scope.
  std::string temp_id = str_helper::FormatString("temp_%d", temp_counter_++);
  // VariableSymbol* temp_symbol;
  // temp_symbol = new VariableSymbol(temp_id);
  // temp_symbol->set_data_type(type);
  // temp_symbol->set_is_array(is_array);
  // temp_symbol->set_kind(LOCAL);
  // current_scope_table_->Insert(temp_symbol);
  DeclareVariable(type, temp_id, is_array, elems);
  VariableOperand* temp = new VariableOperand(temp_id, current_scope_table_);
  return temp;
}



void Parser::DeclareVariable(DataType type,
                             std::string var_id,
                             bool is_array,
                             unsigned int elems)
{

  if (current_scope_table_->IsInCurrentScope(var_id)) {
    ReportError(str_helper::FormatString("%s is declared in the current scope.",
                                   var_id.c_str()));
  } else {
    VariableSymbol* symbol = new VariableSymbol(var_id);
    symbol->set_offset(offset_);

    // Calculate the size
    unsigned int elem_size = type == CHAR_TYPE ? 1 : 4;
    unsigned int size = elem_size * elems;
    
    // Alignment
    unsigned int size_aligned = size;
    while (size_aligned % 4 != 0) {
      size_aligned++;
    }

    symbol->set_element_size(elem_size);
    symbol->set_size(size);
    offset_ += size_aligned;

    symbol->set_data_type(type);
    //symbol.isTemp = false;
    symbol->set_is_array(is_array);
    symbol->set_kind(LOCAL);
    current_scope_table_->Insert(symbol);
  }
}



// Generate intermediate code for copying a string into a memory buffer
void Parser::CopyStringToBuffer(const std::string& array_id,
                                const std::string& text)
{
  unsigned int length = text.length();
  
  for (unsigned int index = 0; index <= length; index++) {
    // Get the ASCII number of the character or a 0 as a terminator
    int value = index == length ? 0 : static_cast<int>(text[index]);
    IntermediateInstr* assign_instr = new IntermediateInstr(ASSIGN_OP,
        new ArrayOperand(array_id, new NumberOperand(index), current_scope_table_),
        new NumberOperand(value));
    Emit(assign_instr);
  }
}



void Parser::ReserveKeywords()
{
	const unsigned int keywords_count = 19;
	const char* keywords[] = { "int", "char", "void",
  													"if", "else", "for", "do", "while", "switch", "case",
														"default", "return", "break", "continue",
														"printInt", "printStr", "printChar", "readStr", "readInt" };											
	const TokenCode token_codes[] = { INT, CHAR, VOID,
  																	IF, ELSE, FOR, DO, WHILE, SWITCH, CASE,
  																	DEFAULT, RETURN, BREAK, CONTINUE,
  																	PRINT_INT, PRINT_STR, PRINT_CHAR, READ_STR, READ_INT };
	
	for (int i = 0; i < keywords_count; ++i) {
		root_symbol_table_->Insert(std::string(keywords[i]), token_codes[i]);
	}
}

void Parser::Parse()
{
  // Start the magic!
  current_token_ = lexer_->GetNextToken(*current_scope_table_);
  ParseFunctions();
}



void Parser::ParseFunctions()
{
  // Start parsing function bodies
  do {
    // Expecting a return type of a function
    DataType return_type;
    TokenCode return_type_tok = current_token_.code();

    if (return_type_tok == CHAR)
      return_type = CHAR_TYPE;
    else if (return_type_tok == INT)
      return_type = INT_TYPE;
    else
      return_type = VOID_TYPE;

    TokenCode ret_type_tokens[] = {INT, CHAR, VOID};
    Match(ret_type_tokens, 3, "function return type expected");

    // Initial stack offset for local variables
    offset_ = 0;

    // Changing the current pointer that points the actual list of intermediate 
    // instructions to point to a list that will contain the instructions for the
    // current function.
    IntermediateInstrsList* original_code = intermediate_code_;
    IntermediateInstrsList* function_code = new IntermediateInstrsList;
    intermediate_code_ = function_code;

    // Reading the function identifier
    std::string function_id = current_token_.lexeme();

    // Adding the function to the symbol table
    FunctionSymbol* function_symbol = new FunctionSymbol(function_id, return_type);
    current_scope_table_->Insert(function_symbol);
    current_function_ = function_symbol;

    Match(ID);
    Match(OPEN_PAREN);

    // Start parsing the parameters list
    if (current_token_.code() == VOID || current_token_.code() == INT ||
        current_token_.code() == CHAR) {
      do {
        // Here is the data type of the parameter in each iteration and a flag
        // that indicates if the data type token is present. Obviously it is
        // present in the first iteration since we checked in the enclosing
        // 'if', but it is used in the second iteration after a comma in the
        // parameters list.
        DataType param_type;
        bool param_type_found = false;
        TokenCode param_type_tok = current_token_.code();
        bool is_array = false;

        TokenCode parameters_types[] = {INT, CHAR};
        Match(parameters_types, 2, "parameter data type expected");

        if (param_type_tok == CHAR) {
          param_type = CHAR_TYPE;
          param_type_found = true;
        } else if (param_type_tok == INT) {
          param_type = INT_TYPE;
          param_type_found = true;
        }

        // Get the identifier of the parameter
        if (param_type_found) {
          std::string param_id = current_token_.lexeme();
          Match(ID);
          // An array parameter
          if (current_token_.code() == OPEN_BRACKET) {
            is_array = true;
            Match(OPEN_BRACKET);
            Match(CLOSE_BRACKET);
          }

          // Add the parameter info to the function symbol
          function_symbol->parameters_.push_back(
                    Parameter(param_type, param_id, is_array));
        } else {
          break;
        }
      } while (MatchIf(COMMA));
    }

    // Then end of the parameter list
    Match (CLOSE_PAREN);

    // The function block
    ParseBlock(function_symbol);

    // Restore the pointer to the original intermediate instructions list
    intermediate_code_ = original_code;
    // Emit a label of this function
    EmitLabel(function_id);
#if defined __APPLE__
    if (function_id == "main") {
      EmitLabel("_main");
    }
#endif

    unsigned int stack_size = GetStackSize();
    // Emit an Enter instruction with the size of the stack of this function
    IntermediateInstr* enter_instr =
      new IntermediateInstr(ENTER_OP, new NumberOperand(stack_size));
    Emit(enter_instr);
    // Add the function code the program code
    intermediate_code_->insert(intermediate_code_->end(),
                               function_code->begin(),
                               function_code->end());
    // And finally, the return instruction at the end of the function
    // IntermediateInstr* inc_stack_ptr =
    //   new IntermediateInstr(INC_STACK_PTR_OP, new NumberOperand(stack_size));
    // Emit(inc_stack_ptr);
    IntermediateInstr* ret_instr = new IntermediateInstr(RETURN_OP);
    Emit(ret_instr);

  } while (current_token_.code() == VOID || current_token_.code() == INT ||
           current_token_.code() == CHAR);
}



// Parses a block and implements scope. If parsing a function the 
// function symbol should be passed. If it just a regular scope (block) or the body 
// of a conditional statement, pass NULL, which is the default value.
void Parser::ParseBlock(FunctionSymbol* func_symbol)
{
  Match(OPEN_BRACE);
  
  // Each block has its own symbol table. Create one for this block
  SymbolTable* new_symbol_table = new SymbolTable(current_scope_table_);
  current_scope_table_->inner_scopes_.push_back(new_symbol_table);
  current_scope_table_ = new_symbol_table;

  // If this block is the body of a function, then we need to add information
  // about them in the current symbol table if there are any.
  if (func_symbol != NULL) {
    std::vector<Parameter>* params_vector = &func_symbol->parameters_;
    if (params_vector->size() != 0) {
      unsigned int param_offset = 0;
      
      std::vector<Parameter>::iterator it;
      for (it = params_vector->begin(); it != params_vector->end(); it++) {
        VariableSymbol* var_symb = new VariableSymbol(it->identifier());
        var_symb->set_is_array(it->is_array());
        var_symb->set_data_type(it->type());
        var_symb->set_element_size(it->type() == CHAR_TYPE? 1 : 4);
        var_symb->set_size(4);
        var_symb->set_offset(param_offset);
        var_symb->set_kind(ARGUMENT);
        current_scope_table_->Insert(var_symb);
        param_offset += 4;
      }
    }
  }

  if (current_token_.code() == CHAR || current_token_.code() == INT) {
    ParseDeclarations();
  }

  ParseStatements();

  // Exit this scope and return to the parent
  current_scope_table_ = current_scope_table_->outer();
  Match(CLOSE_BRACE);
}



void Parser::ParseDeclarations()
{
  // Expecting a type. As long as we have a type, parse identifiers separated
  // by commas. E.g., int x, y;
  while (current_token_.code() == CHAR || current_token_.code() == INT) {
    // Match the type (char or int)
    DataType type = current_token_.code() == CHAR ? CHAR_TYPE : INT_TYPE;
    Match(current_token_.code());

    int elems = 1;
    bool is_array;
    std::string var_id = "";
    
    do {
      is_array = false;

      if (current_token_.code() == ID) {
        var_id = current_token_.lexeme();
      }

      Match(ID);
      
      // Try parsing array brackets
      if (current_token_.code() == OPEN_BRACKET) {
        is_array = true;
        Match(OPEN_BRACKET);
        // Parse the constant array size
        if (current_token_.code() == NUM_LITERAL) {
          elems = current_token_.value();
          Match(NUM_LITERAL);
        } else {
          ReportError(NUM_LITERAL);
        }

        Match(CLOSE_BRACKET);
      }

      DeclareVariable(type, var_id, is_array, elems);
      
      // Parse initialization for variables if there is any
      if (current_token_.code() == EQUAL) {
        ParseInitialization(var_id);
      }
    }
    while (MatchIf(COMMA));

    Match(SEMICOLON);
  }
}



void Parser::ParseInitialization(const std::string& var_id)
{
  // Beginning from the assignment since the declaration is already parsed
  Match(EQUAL);
  
  VariableSymbol* var_symb =
    static_cast<VariableSymbol*>((*current_scope_table_)[var_id]);

  // Parse array initialization
  if (var_symb->is_array()) {
    if (current_token_.code() == STRING_LITERAL) {
      // A string
      std::string text = current_token_.lexeme();
      Match(STRING_LITERAL);
      CopyStringToBuffer(var_id, text);
    } else if (current_token_.code() == OPEN_BRACE) {
      // A numeric array (integers)
      unsigned int index = 0;
      Match(OPEN_BRACE);
      
      do {
        Operand* value = ParseExpression();
        IntermediateInstr* assign_instr = new IntermediateInstr(ASSIGN_OP,
            new ArrayOperand(var_id, new NumberOperand(index), current_scope_table_),
            value);
        Emit(assign_instr);
        ++index;
      } while (MatchIf(COMMA));

      Match(CLOSE_BRACE);
    }
  } else {
    // Variable initialization
    IntermediateInstr* inst = new IntermediateInstr(ASSIGN_OP,
        new VariableOperand(var_id, current_scope_table_), ParseBooleanExpr());
    Emit(inst);
  }
}



void Parser::ParseStatements()
{
  // Detecting initial tokens of statements
  // while (curToken.code == ID || curToken.code == IF ||
  //       curToken.code == FOR || curToken.code = WHILE ||
  //       curToken.code == DO || curToken.code == SWITCH ||
  //       curToken.code == BREAK || curToken.code == CONTINUE ||
  //       curToken.code == RETURN || curToken.code == OPEN_BRACE ||
  //       curToken.code == SEMICOLON || curToken.code == PRINT_INT ||
  //       curToken.code == PRINT_STR || curToken.code == PRINT_CHAR ||
  //       curToken.code == READ_INT || curToken.code == READ_STR) {

  //   ParseStatement();
  // }

  // Detecting initial tokens of statements.
  // Continue parsing statement as long as we
  // have the initial token of any statement.
  bool keep_going = true;
  while (keep_going) {
    switch (current_token_.code()) {
    case ID:        case IF:
    case FOR:       case WHILE:
    case DO:        case SWITCH:
    case BREAK:     case CONTINUE:
    case RETURN:    case OPEN_BRACE:
    case SEMICOLON: case PRINT_INT:
    case PRINT_STR: case PRINT_CHAR:
    case READ_INT:  case READ_STR:
      ParseStatement();
      break;
    default:
      keep_going = false;
      break;
    }
  }
}



// Parses a C statement, one at a time. Statements include:
// - Assignments --ideally in C are expression statements, but for the sake of simplicity, here it is
//   reduced to a simple assignment statement in the form: identifier = expression.
// - if, for, while, do, switch, break, continue, return and null statements.
// - Some IO functions expressed as statements of their own.
void Parser::ParseStatement()
{
  switch (current_token_.code()) {
  case ID:
	// We found an identifier. It could be either an assignment statement or a function call with
	// no assignment.
    {
      std::string id = current_token_.lexeme();
      Operand* id_operand = ParseIdentifier(true);

      if (id_operand == NULL)
        ParseFunctionCall(id);
      else
        ParseAssignment(id_operand);

      Match(SEMICOLON);
    }
    break;

  case OPEN_BRACE:
    ParseBlock();
    break;

  case IF:
		ParseIfStatement();
    break;

  case FOR:
		ParseForStatement();
    break;

  case WHILE:
		ParseWhileStatement();
    break;

  case DO:
		ParseDoStatement();
    break;

  case SWITCH:
		ParseSwitchStatement();
    break;

  case SEMICOLON:
		// The empty (or null) statement
    Match(SEMICOLON);
    break;

  case BREAK:
		ParseBreakStatement();
    break;

  case CONTINUE:
		ParseContinueStatement();
    break;

  case RETURN:
    ParseReturnStatement();
    break;

  case READ_STR:
		ParseReadStrStatement();
    break;

  case READ_INT:
		ParseReadIntStatement();
    break;

  case PRINT_CHAR:
  case PRINT_INT:
		ParsePrintCharIntStatement();
    break;
		
  case PRINT_STR:
		ParsePrintStrStatement();
    break;
  }
}



void Parser::ParseAssignment(Operand* lhs_operand)
{
  if (lhs_operand == NULL) {
    lhs_operand = ParseIdentifier(false);
  }

  TokenCode token = current_token_.code();
  if (token == PLUS_PLUS || token == MINUS_MINUS) {
    // Syntax: var++ or var--
    IntermediateOp op = token == PLUS_PLUS ? ADD_OP : SUBTRACT_OP;
    Match(token);
    IntermediateInstr* assign_inst = new IntermediateInstr(op, lhs_operand,
                                                          lhs_operand,
                                                          new NumberOperand(1));
    Emit(assign_inst);
  } else {
    // Syntax: var = expression
    Match(EQUAL);
    IntermediateInstr* assign_inst =
      new IntermediateInstr(ASSIGN_OP, lhs_operand, ParseBooleanExpr());
    Emit(assign_inst);
  }
}



void Parser::ParseIfStatement()
{
  Match(IF);
  Match(OPEN_PAREN);

  Operand* if_condition = ParseBooleanExpr();
  LabelOperand* if_next = CreateLabel();
  LabelOperand* if_false = if_next;
  LabelOperand* if_true = CreateLabel();
	
  IntermediateInstr* if_inst = new IntermediateInstr(IF_OP, if_condition, if_true);
  IntermediateInstr* goto_inst = new IntermediateInstr(GOTO_OP, if_false);

  Emit(if_inst);
  Emit(goto_inst);
  EmitLabel(if_true);

  Match(CLOSE_PAREN);
  ParseStatement();

  if (current_token_.code() == ELSE) {
    if_next = CreateLabel();
    Emit(new IntermediateInstr(GOTO_OP, if_next));
    EmitLabel(if_false);
    
		Match(ELSE);
		ParseStatement();
		EmitLabel(if_next);
  } else {
    EmitLabel(if_next);
  }
}


// Parses the for statement
// Note: C statement lists are not supported.
// Syntax: for (assignment; bool_expr; assignment) statement
void Parser::ParseForStatement()
{
  Match(FOR);
  Match(OPEN_PAREN);

  // Parse the 1st assignment statement
  if (current_token_.code() == ID) {
    ParseAssignment();
  }
  Match(SEMICOLON);

  LabelOperand* for_begin = CreateLabel();
  LabelOperand* for_true = CreateLabel();
  LabelOperand* for_inc = CreateLabel();
  LabelOperand* for_next = CreateLabel();

  EmitLabel(for_begin);
	
  break_stack_.push(for_next);
  continue_stack_.push(for_inc);

  // Parse the for condition
  if (current_token_.code() == NUM_LITERAL || current_token_.code() == ID) {
    Operand* for_condition = ParseBooleanExpr();
    Emit(new IntermediateInstr(IF_OP, for_condition, for_true));
    Emit(new IntermediateInstr(GOTO_OP, for_next));
    EmitLabel(for_true);
  }
  Match(SEMICOLON);
  
  // The intermediate code of the 3rd instruction in the for
  // loop statement
  IntermediateInstrsList* assign_inst3;

  // Parse the 3rd assignment statement if exists
  // (Usually an increment or decrement statement)
  if (current_token_.code() == ID) {
    // Save the instructions list in a temporary list
    IntermediateInstrsList* temp = intermediate_code_;
    // A new list for assignment instructions
    intermediate_code_ = new IntermediateInstrsList();

    ParseAssignment();
    // Return everything into place
    assign_inst3 = intermediate_code_;
    intermediate_code_ = temp;
  }

  Match(CLOSE_PAREN);
  // Parse the body of the for loop
  ParseStatement();
  EmitLabel(for_inc);

  // if a third expression exists
  if (assign_inst3 != NULL) {
    // instructions.AddRange(assign3Inst);
    intermediate_code_->insert(intermediate_code_->end(),
                               assign_inst3->begin(),
                               assign_inst3->end());
  }
	
  Emit(new IntermediateInstr(GOTO_OP, for_begin));
  EmitLabel(for_next);

  break_stack_.pop();
  continue_stack_.pop();
}



void Parser::ParseWhileStatement()
{
  Match(WHILE);
  Match(OPEN_PAREN);

  LabelOperand* w_begin = CreateLabel();
  LabelOperand* w_true = CreateLabel();
  LabelOperand* w_next = CreateLabel();

  break_stack_.push(w_next);
  continue_stack_.push(w_begin);

  EmitLabel(w_begin);
  Operand* w_condition = ParseBooleanExpr();
  Match(CLOSE_PAREN);
  IntermediateInstr* w_inst = new IntermediateInstr(IF_OP, w_condition, w_true);
  Emit(w_inst);
  Emit(new IntermediateInstr(GOTO_OP, w_next));
  EmitLabel(w_true);

  // Parse the body of the while statement and emit the code
  ParseStatement();
  Emit(new IntermediateInstr(GOTO_OP, w_begin));
  EmitLabel(w_next);

  break_stack_.pop();
  continue_stack_.pop();
}



void Parser::ParseDoStatement()
{
  Match(DO);
  LabelOperand* do_begin = CreateLabel();
  LabelOperand* do_condition_label = CreateLabel();
  LabelOperand* do_next = CreateLabel();

  break_stack_.push(do_next);
  continue_stack_.push(do_condition_label);

  EmitLabel(do_begin);
  ParseBlock();
  Match(WHILE);
  Match(OPEN_PAREN);

  EmitLabel(do_condition_label);
  Operand* do_condition = ParseBooleanExpr();
  IntermediateInstr* do_inst = new IntermediateInstr(IF_OP, do_condition, do_begin);
  Emit(do_inst);
  EmitLabel(do_next);

  Match(CLOSE_PAREN);
  Match(SEMICOLON);

  break_stack_.pop();
  continue_stack_.pop();
}


// Parses the magnificient switch statement
void Parser::ParseSwitchStatement()
{
  Match(SWITCH);
  Match(OPEN_PAREN);

  LabelOperand* switch_test = CreateLabel();
  LabelOperand* swich_default = CreateLabel();
  LabelOperand* switch_next = CreateLabel();
	
  break_stack_.push(switch_next);

  Operand* value;
  Operand* switch_condition = ParseBooleanExpr();
  Emit(new IntermediateInstr(GOTO_OP, switch_test));

  Match(CLOSE_PAREN);
  Match(OPEN_PAREN);

  // Labels of each case and their values
  std::vector<Operand*> values;
  std::vector<LabelOperand*> labels;

  // Indicates whether 'default' label is already parsed
  bool is_default_parsed = false;
  // Indicates whether the current label is 'default'
  bool is_default = false;

  while (current_token_.code() == CASE ||
         current_token_.code() == DEFAULT) {
    // Will hold a pointer to this case's value
    value = NULL;

    is_default = current_token_.code() == DEFAULT;

    // Match 'case' or 'default' tokens
    Match(current_token_.code());

    if (is_default) {
      if (is_default_parsed) {
        ReportError("more than one 'default' label found.");
      }
      is_default_parsed = true;

      Match(COLON);
      EmitLabel(swich_default);

      // Parse statements for the current case until the end
      // (break statement is optional)
      ParseStatements();
      // Emit(new IntermediateInstr(GOTO_OP, switch_next));
    } else {
      value = ParseBooleanExpr();
      Match(COLON);

      LabelOperand* case_label = CreateLabel();
      EmitLabel(case_label);
      ParseStatements();
      // Emit(new IntermediateInstr(TokenCode.Goto, switch_next));

      values.push_back(value);
      labels.push_back(case_label);
    }
  }
  Match(CLOSE_BRACE);

  Emit(new IntermediateInstr(GOTO_OP, switch_next));
  EmitLabel(switch_test);

  // TODO: Fix this loop to use iterators instead
  for (int i = 0; i < labels.size(); i++) {
    Operand* temp = CreateTempVariable();
    Emit(new IntermediateInstr(EQUAL_EQUAL_OP, temp, switch_condition, values[i]));
    Emit(new IntermediateInstr(IF_OP, temp, labels[i]));
  }

  if (is_default_parsed) {
    Emit(new IntermediateInstr(GOTO_OP, swich_default));
  }
  EmitLabel(switch_next);

  break_stack_.pop();
}



void Parser::ParseBreakStatement()
{
  // We know already that the current token is 'break' 
  if (!break_stack_.empty()) {
      LabelOperand* break_target = break_stack_.top();
      Emit(new IntermediateInstr(GOTO_OP, break_target));
  } else {
    ReportError("'break' statement is not allowed in this location.");
  }
	
  Match(BREAK);
  Match(SEMICOLON);
}



void Parser::ParseContinueStatement()
{
  // We know already that the current token is 'continue' 
  if (!continue_stack_.empty()) {
    LabelOperand* continue_target = continue_stack_.top();
    Emit(new IntermediateInstr(GOTO_OP, continue_target));
  } else {
    ReportError("'continue' statement is not allowed in this location.");
  }
	
  Match(CONTINUE);
  Match(SEMICOLON);
}



void Parser::ParseReturnStatement()
{
  Match(RETURN);

  Operand* return_value = NULL;
  if (current_function_->return_type() != VOID_TYPE) {
    return_value = ParseBooleanExpr();
  }

  if (current_token_.code() != SEMICOLON) {
    ReportError("can't return value in a function that returns void.");
  }

  Match(SEMICOLON);

  // Emit(new IntermediateInstr(INC_STACK_PTR_OP, new NumberOperand(GetStackSize())));
  Emit(new IntermediateInstr(RETURN_OP, return_value));
}



void Parser::ParseReadStrStatement()
{
  Match(READ_STR);
  Match(OPEN_PAREN);
  Operand* in_buff_id = ParseIdentifier(false);
  Match(COMMA);
  Operand* in_buff_lim = ParseExpression();
  Match(CLOSE_PAREN);
  Match(SEMICOLON);
  Emit(new IntermediateInstr(READ_STR_OP, in_buff_id, in_buff_lim));
}
		
		
		
void Parser::ParseReadIntStatement()
{
  Match(READ_INT);
  Match(OPEN_PAREN);
  Operand* var = ParseIdentifier(false);
  Match(CLOSE_PAREN);
  Match(SEMICOLON);
  Emit(new IntermediateInstr(READ_INT_OP, var));
}
		
		
		
void Parser::ParsePrintCharIntStatement()
{
  IntermediateOp print_op;

  if (current_token_.code() == PRINT_CHAR) {
    print_op = PRINT_CHAR_OP;
	} else {
    print_op = PRINT_INT_OP;
	}
	
  // We know for sure that it's either a PRINT_CHAR or PRINT_INT token, so
	// we are OK doing this.
	Match(current_token_.code());
  Match(OPEN_PAREN);

  Operand* expression = ParseExpression();
  Match(CLOSE_PAREN);
  Match(SEMICOLON);
  Emit(new IntermediateInstr(print_op, expression));
}
		
		
		
void Parser::ParsePrintStrStatement()
{
  Match(PRINT_STR);
  Match(OPEN_PAREN);

  if (current_token_.code() == ID) {
    // Print a string from a pointer or a buffer
    Operand* buffer = ParseIdentifier(false);
    Emit(new IntermediateInstr(PRINT_STR_OP, buffer /*arrOp*/));
  } else {
    // Print a string literal (a static string)
    std::string text;
    if (current_token_.code() == STRING_LITERAL) {
      text = current_token_.lexeme();
    }
    Match(STRING_LITERAL);

    // Create a temporary buffer for the string we have
    VariableOperand* temp_buffer = CreateTempVariable(CHAR_TYPE, true, text.length() + 1);
    // Emit instructions that copy the string into a buffer
		CopyStringToBuffer(temp_buffer->GetSymbol()->lexeme(), text);
		// Emit the print instruction
    Emit(new IntermediateInstr(PRINT_STR_OP, temp_buffer));
  }

  Match(CLOSE_PAREN);
  Match(SEMICOLON);
}
		


Operand* Parser::ParseBooleanExpr()
{
  if (current_token_.code() == ID || current_token_.code() == NUM_LITERAL ||
      current_token_.code() == OPEN_PAREN || current_token_.code() == MINUS ||
      current_token_.code() == EXCLAMATION) {
    Operand* t;
    Operand* operand1 = ParseAndExpr();
    
    while (current_token_.code() == OR) {
      Match(OR);
      t = CreateTempVariable();
      IntermediateInstr* inst = new IntermediateInstr(OR_OP, t, operand1, ParseAndExpr());
      Emit(inst);
      operand1 = t;
    }
    return operand1;
  } else {
    ReportError("id, number, '(', '-' or '!' expected.");
    return NULL;
  }
}



Operand* Parser::ParseAndExpr()
{
  Operand* t;
  Operand* operand1 = ParseEqualityExpr();
  
  while (current_token_.code() == AND) {
    Match(AND);
    t = CreateTempVariable();
    IntermediateInstr* inst = new IntermediateInstr(AND_OP, t, operand1,
                                                    ParseEqualityExpr());
    Emit(inst);
    operand1 = t;
  }

  return operand1;
}



Operand* Parser::ParseEqualityExpr()
{
  Operand* t;
  Operand* operand1 = ParseRelationalExpr();
  
  while (current_token_.code() == EQUAL_EQUAL ||
         current_token_.code() == NOT_EQUAL) {
    // The internal number of '==' and '!=' both as token codes or intermediate
    // opcodes are the same. So we just cast one to the other in order to get the
    // intermediate opcode from the token code.
    IntermediateOp op = static_cast<IntermediateOp>(current_token_.code());
    Match(current_token_.code());
    t = CreateTempVariable();
    IntermediateInstr* inst = new IntermediateInstr(op, t, operand1, ParseRelationalExpr());
    Emit(inst);
    operand1 = t;
  }

  return operand1;
}



Operand* Parser::ParseRelationalExpr()
{
  Operand* t;
  Operand* operand1 = ParseExpression();
  
  while (current_token_.code() == LESS || current_token_.code() == LESS_OR_EQUAL ||
         current_token_.code() == GREATER ||
         current_token_.code() == GREATER_OR_EQUAL) {
    // Same values, so we just cast one to the other
    IntermediateOp op = static_cast<IntermediateOp>(current_token_.code());
    Match(current_token_.code());
    t = CreateTempVariable();
    IntermediateInstr* inst = new IntermediateInstr(op, t, operand1, ParseExpression());
    Emit(inst);
    operand1 = t;
  }

  return operand1;
}



Operand* Parser::ParseExpression() {
  Operand* t;
  Operand* operand1 = ParseTermExpr();

  while (current_token_.code() == PLUS || current_token_.code() == MINUS) {
    // Same values, so we just cast one to the other
    IntermediateOp op = static_cast<IntermediateOp>(current_token_.code());
    Match(current_token_.code());
    t = CreateTempVariable();
    IntermediateInstr* inst = new IntermediateInstr(op, t, operand1, ParseTermExpr());
    Emit(inst);
    operand1 = t;
  }

  return operand1;
}



Operand* Parser::ParseTermExpr()
{
  Operand* t;
  Operand* operand1 = ParseFactorExpr();

  // Multiplication, division, and div reminder operators:
  // '*', '/', and '%'
  while (current_token_.code() == ASTERISK || current_token_.code() == FORWARD_SLASH
      || current_token_.code() == PERCENT) {
    // Same values, so we just cast one to the other
    IntermediateOp op = static_cast<IntermediateOp>(current_token_.code());
    Match(current_token_.code());
    t = CreateTempVariable();
    IntermediateInstr* inst = new IntermediateInstr(op, t, operand1, ParseFactorExpr());
    Emit(inst);
    operand1 = t;
  }
  return operand1;
}



Operand* Parser::ParseFactorExpr()
{
  Operand* ret = NULL;
  IntermediateInstr* instr;
  std::string id;

  switch (current_token_.code()) {
  case OPEN_PAREN:
    Match(OPEN_PAREN);
    ret = ParseBooleanExpr();
    Match(CLOSE_PAREN);
    break;

  // The unary Not operator '!'
  case EXCLAMATION:
    Match(EXCLAMATION);
    ret = CreateTempVariable();
    instr = new IntermediateInstr(NOT_OP, ret, ParseFactorExpr());
    Emit(instr);
    break;

  // The unary '-' operator
  case MINUS:
    Match(MINUS);
    ret = CreateTempVariable();
    instr = new IntermediateInstr(SUBTRACT_OP, ret, ParseFactorExpr());
    Emit(instr);
    break;

  case ID:
    // Here we encounter an identifier in the expression.
    // It could be an identifier either for a variable or a function.
    // We try parsing as a variable ID first, If we don't
    // succeed we parse as a function call.
    id = current_token_.lexeme();
    ret = ParseIdentifier(true);
    if (ret == NULL)
      ret = ParseFunctionCall(id);
    break;

  // A number literal
  case NUM_LITERAL:
    ret = new NumberOperand(current_token_.value());
    Match(NUM_LITERAL);
    break;

  default:
    ReportError("id, number, '(', '-' or '!' expected.");
    break;
  }

  return ret;
}



Operand* Parser::ParseIdentifier(bool allow_func)
{
  std::string var_id = current_token_.lexeme();
  Symbol* symbol = (*current_scope_table_)[var_id];

  if (symbol == NULL)
    ReportError(var_id + " is an undeclared identifier.");

  Match(ID);

  // Test whether the identifier belongs to an array and parse the brackets
  // and the index expression.
  if (current_token_.code() == OPEN_BRACKET) {
    VariableSymbol* var_symbol = static_cast<VariableSymbol*>(symbol);
    if (!var_symbol->is_array()) {
      ReportError(var_id + " is not an array.");
    }

    Match(OPEN_BRACKET);
    Operand* array_operand = new ArrayOperand(var_id, ParseExpression(),
                                              current_scope_table_);
    Match(CLOSE_BRACKET);

    return array_operand;

  } else if ((current_token_.code() == OPEN_PAREN) && (allow_func)) {
    return NULL;
  } else {
    VariableSymbol* var_symbol = dynamic_cast<VariableSymbol*>(symbol);
    if (var_symbol == NULL)
      ReportError(var_id + " is not a variable.");
    // C#! if (!(symbol is VariableSymbol))
    //   ReportError(var_id + " is not a variable.");
    return new VariableOperand(var_id, current_scope_table_);
  }
}



Operand* Parser::ParseFunctionCall(const std::string& func_id)
{
  Symbol* symbol = (*current_scope_table_)[func_id];
  if (symbol == NULL) {
    ReportError(func_id + " is an undeclared identifier.");
  }

  FunctionSymbol* function_symbol = dynamic_cast<FunctionSymbol*>(symbol);
  if (function_symbol == NULL) {
    ReportError(func_id + " is not a function.");
  }
  
  // Parsing argument expressions, represented as a vector of 
  // Operand objects
  std::vector<Operand*>* arguments = ParseArgumentList();
  int arguments_count = arguments->size();

  if (function_symbol != NULL) {
    int params_count = function_symbol->parameters_.size();
    if (params_count != arguments_count)
      ReportError(str_helper::FormatString("the function %s takes %d arguments.",
                                     func_id.c_str(), params_count));
      // ReportError(funcId + " takes " + funcSymbol.parameters.Count + " argument(s).");

    // Push arguments on the stack in reversed order
    std::vector<Operand*>::reverse_iterator r_it;
    for (r_it = arguments->rbegin(); r_it != arguments->rend(); r_it++) {
      Emit(new IntermediateInstr(PARAM_OP, *r_it));
    }

    // for (int i = arguments_count - 1; i >= 0; i--) {
    //   Emit(new IntermediateInstr(PARAM_OP, arguments[i]));
    // }
  }

  // A temporary variable to receive the return value of the function
  Operand* ret_operand = CreateTempVariable();
  // Emit an instruction that perform the calling
  Emit(new IntermediateInstr(CALL_OP, ret_operand, new FunctionOperand(func_id)));
  // Restore the space allocated for pushed arguments in the stack. i.e., pop them
  // Emit an instruction that increases the stack pointer to the size of arguments
  // multiplied by 4 since the size of each argument is 4 bytes.
  Emit(new IntermediateInstr(INC_STACK_PTR_OP,
                             new NumberOperand(arguments_count * 4)));

  delete arguments;
  return ret_operand;
}



// Returns a pointer to vector that contains the arguments list.
// The caller should free up the memory allocated for the vector object after being
// done with it.
std::vector<Operand*>* Parser::ParseArgumentList()
{
  std::vector<Operand*>* arguments = new std::vector<Operand*>();
  Match(OPEN_PAREN);
  
  TokenCode token = current_token_.code();
  if (token == ID || token == NUM_LITERAL || token == OPEN_PAREN || token == MINUS ||
      token == EXCLAMATION) {
    do {
      arguments->push_back(ParseBooleanExpr());
    } while (MatchIf(COMMA));
  }

  Match(CLOSE_PAREN);
  return arguments;
}
