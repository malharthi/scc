
using System;
using System.Collections;
using System.Diagnostics;

namespace CCompiler {

    public class Parser {
        private Scanner lexer;

        /* local variables offset */
        private int offset = 0;

        /* look ahead token */
        private Token curToken;

        public SymbolTable rootSymTab;
        /* currunt symbole table*/
        private SymbolTable curSymTab;

        /* three-adress code instructions */
        public ArrayList instructions;

        /* for generating temporary variables */
        private int tempCounter = 0;

        /* errors list */
        private ArrayList errorsList;

        /* for generating labels in the intermediate code */
        private int labelCounter = 0;

        /* target label for 'break' statement
         * in 'for', 'while', 'do' and 'switch' statements */
        private Stack breakStack;
        /* target label for 'continue' statement
         * in 'for', 'while' and 'do' statements */
        private Stack continueStack;

        private FunctionSymbol curFunction;

        public Parser(Scanner lexer, ArrayList errorsList) {
            this.lexer = lexer;
            instructions = new ArrayList();
            this.errorsList = errorsList;

            curSymTab = new SymbolTable(null);
            rootSymTab = curSymTab;

            breakStack = new Stack();
            breakStack.Push(null);
            continueStack = new Stack();
            continueStack.Push(null);

            //curSymTab.Insert("void", 300);
            curSymTab.Insert("int", TokenCode.Int);
            curSymTab.Insert("char", TokenCode.Char);
            curSymTab.Insert("void", TokenCode.Void);
            curSymTab.Insert("if", TokenCode.If);
            curSymTab.Insert("else", TokenCode.Else);
            curSymTab.Insert("for", TokenCode.For);
            curSymTab.Insert("do", TokenCode.Do);
            curSymTab.Insert("while", TokenCode.While);
            curSymTab.Insert("switch", TokenCode.Switch);
            curSymTab.Insert("case", TokenCode.Case);
            curSymTab.Insert("default", TokenCode.Default);
            curSymTab.Insert("return", TokenCode.Return);
            curSymTab.Insert("break", TokenCode.Break);
            curSymTab.Insert("continue", TokenCode.Continue);

            curSymTab.Insert("printInt", TokenCode.PrintInt);
            curSymTab.Insert("printStr", TokenCode.PrintStr);
            curSymTab.Insert("printChar", TokenCode.PrintChar);
            curSymTab.Insert("readStr", TokenCode.ReadStr);
            curSymTab.Insert("readInt", TokenCode.ReadInt);
        }

        private bool MatchIf(TokenCode token) {
            if (curToken.code == token) {
                Debug.WriteLine(curToken.lexeme);
                curToken = lexer.GetNextToken(curSymTab);
                return true;
            }
            return false;
        }

        private bool Match(TokenCode token) {
            if (curToken.code == token) {
                Debug.WriteLine(curToken.lexeme);
                curToken = lexer.GetNextToken(curSymTab);

                return true;
            } else {
                Error(token);
                //curToken = lexer.GetNextToken(curSymTab);

                return false;
            }
        }

        private bool Match(params TokenCode[] tokens) {
            string tokenName = "";

            foreach (TokenCode token in tokens) {
                if (MatchIf(token))
                    return true;

                if (tokenName != "")
                    tokenName += ", ";

                tokenName += token.ToString();
            }

            Error(tokenName + " expected.");
            //curToken = lexer.GetNextToken(curSymTab);

            return false;
        }

        public int GetStackSize() {
            while (offset % 4 != 0)
                offset++;

            return offset;
        }

        private void Error(TokenCode token) {
            errorsList.Add("{" + lexer.curLine + "}: " + token.ToString() + " expected.");
        }

        private void Error(string message) {
            errorsList.Add("{" + lexer.curLine + "}: " + message);
        }

        private VariableOperand NewTemp(Type type, bool isArray, int elems) {
            string t = "t" + (++tempCounter);
            DeclareVariable(type, t, isArray, elems);

            return new VariableOperand(t, curSymTab);
        }

        private VariableOperand NewTemp() {
            return NewTemp(Type.Integer, false, 1);
        }

        private LabelOperand NewLabel() {
            return new LabelOperand("L" + (++labelCounter));
        }

        /* emit labels */
        private void Emit(string label) {
            instructions.Add(label);
        }

        /* emit instructions */
        private void Emit(InterInstruction instr) {
            instructions.Add(instr);
        }

        public void Parse() {
            curToken = lexer.GetNextToken(curSymTab);
            /*Match(TokenCode.Id);
            Match(TokenCode.OpenParen); Match(TokenCode.CloseParen);
            ParseBlock();*/
            ParseFunctions();
        }

        private void ParseFunctions() {
            do {
                Type retType;
                TokenCode retTypeTok = curToken.code;

                if (retTypeTok == TokenCode.Char)
                    retType = Type.Char;
                else if (retTypeTok == TokenCode.Int)
                    retType = Type.Integer;
                else
                    retType = Type.Void;

                Match(TokenCode.Char, TokenCode.Int, TokenCode.Void);

                offset = 0;

                ArrayList progInstrs = instructions;
                ArrayList funcInstrs = new ArrayList();

                instructions = funcInstrs;

                string funId = curToken.lexeme;
                Match(TokenCode.Id);

                FunctionSymbol funcSymbol = new FunctionSymbol(funId);
                funcSymbol.returnType = retType;

                curFunction = funcSymbol;
                curSymTab.Insert(funcSymbol);

                Match(TokenCode.OpenParen);

                if (curToken.code == TokenCode.Void || curToken.code == TokenCode.Int ||
                    curToken.code == TokenCode.Char) {

                    do {
                        Type? paramType = null;
                        TokenCode paramTypeTok = curToken.code;
                        bool isArray = false;

                        Match(TokenCode.Int, TokenCode.Char);

                        if (paramTypeTok == TokenCode.Char)
                            paramType = Type.Char;
                        else if (paramTypeTok == TokenCode.Int)
                            paramType = Type.Integer;

                        if (paramType != null) {
                            string paramId = curToken.lexeme;
                            Match(TokenCode.Id);

                            if (curToken.code == TokenCode.OpenBracket) {
                                isArray = true;
                                Match(TokenCode.OpenBracket);
                                Match(TokenCode.CloseBracket);
                            }
                            funcSymbol.parameters.Add(new Parameter(paramType.Value, paramId, isArray));
                        } else {
                            //Error("Type expected.");
                            break;
                        }
                    }
                    while (MatchIf(TokenCode.Comma));
                }

                Match(TokenCode.CloseParen);

                ParseBlock(funcSymbol.parameters);

                instructions = progInstrs;

                Emit(funId);
                Emit(new InterInstruction(TokenCode.Enter, new NumberOperand(GetStackSize()), null, null));
                instructions.AddRange(funcInstrs);

                Emit(new InterInstruction(TokenCode.Return, null, null, null));

            } while (curToken.code == TokenCode.Void || curToken.code == TokenCode.Int ||
                curToken.code == TokenCode.Char);

            if (curSymTab.Lookup("_main") == null)
                Error("no _main function defined.");
        }

        private void ParseBlock(ArrayList arguments) {
            Match(TokenCode.OpenBrace);

            /* create new symbol table for this block (enter new scope)*/
            SymbolTable newScope = new SymbolTable(curSymTab);
            curSymTab.childScopes.Add(newScope);
            curSymTab = newScope;

            if (arguments != null) {
                int paramOffset = 0;

                foreach (Parameter arg in arguments) {
                    VariableSymbol symbol = new VariableSymbol(arg.id);
                    symbol.isArray = arg.isArray;
                    symbol.type = arg.type;
                    symbol.elemSize = 4;
                    symbol.size = 4;
                    symbol.offset = paramOffset;
                    symbol.kind = VariableKind.Argument;

                    curSymTab.Insert(symbol);

                    paramOffset += 4;
                }
            }

            if (curToken.code == TokenCode.Char || curToken.code == TokenCode.Int)
                ParseDecls();

            ParseStmts();

            /*  return to the previous symbole table
                exit the current scope & return to parent scope) */
            curSymTab = curSymTab.prev;

            Match(TokenCode.CloseBrace);
        }

        private void ParseBlock() {
            ParseBlock(null);
        }

        private void DeclareVariable(Type type, string varId, bool isArray, int elems) {
            if (curSymTab.LookupInCurScope(varId))
                Error(varId + " is declared in the current scope.");
            else {
                VariableSymbol symbol = new VariableSymbol(varId);

                symbol.offset = offset;
                /* calculate the size */
                int elemSize = type == Type.Char ? 1 : 4;
                symbol.elemSize = elemSize;
                symbol.size = elemSize * elems;
                offset += symbol.size;

                symbol.type = type;
                //symbol.isTemp = false;
                symbol.isArray = isArray;
                symbol.kind = VariableKind.Local;

                curSymTab.Insert(symbol);
            }
        }

        private void CopyStringToBuffer(string arrId, string text) {
            for (int index = 0; index <= text.Length; index++) {
                /* getting the ascci number or null terminator */
                int value = index == text.Length ? 0 : (int)text[index];

                InterInstruction assInst = new InterInstruction(TokenCode.Equal,
                    new ArrayOperand(arrId, new NumberOperand(index), curSymTab),
                    new NumberOperand(value),
                    null);
                Emit(assInst);
            }
        }

        private void ParseDecls() {
            while (curToken.code == TokenCode.Char || curToken.code == TokenCode.Int) {
                /* match a type, char or int */
                Type type = curToken.code == TokenCode.Char ? Type.Char : Type.Integer;
                Match(curToken.code);

                int elems = 1;
                bool isArray;
                string varId = "";
                do {
                    isArray = false;

                    if (curToken.code == TokenCode.Id)
                        varId = curToken.lexeme;
                    Match(TokenCode.Id);

                    /* parse array brackets */
                    if (curToken.code == TokenCode.OpenBracket) {
                        isArray = true;
                        Match(TokenCode.OpenBracket);

                        /* constant array size*/
                        if (curToken.code == TokenCode.Num) {
                            elems = curToken.value;
                            Match(TokenCode.Num);
                        } else
                            Error(TokenCode.Num);
                        Match(TokenCode.CloseBracket);
                    }

                    DeclareVariable(type, varId, isArray, elems);

                    /* parse initialization for variables */
                    if (curToken.code == TokenCode.Equal)
                        ParseInit(varId);
                }
                while (MatchIf(TokenCode.Comma));

                Match(TokenCode.SemiColon);
            }
        }

        /* parse initialization for variables and arrays */
        private void ParseInit(string varId) {
            Match(TokenCode.Equal);

            if (((VariableSymbol)curSymTab.Lookup(varId)).isArray) {
                if (curToken.code == TokenCode.String) {
                    /* array initialization*/
                    string text = curToken.lexeme;
                    Match(TokenCode.String);

                    CopyStringToBuffer(varId, text);
                } else if (curToken.code == TokenCode.OpenBrace) {
                    int index = 0;
                    Match(TokenCode.OpenBrace);
                    do {
                        Operand value = ParseExpr();

                        InterInstruction assInst = new InterInstruction(TokenCode.Equal,
                            new ArrayOperand(varId, new NumberOperand(index), curSymTab),
                            value,// new NumberOperand(value),
                            null);
                        Emit(assInst);

                        ++index;

                    } while (MatchIf(TokenCode.Comma));

                    Match(TokenCode.CloseBrace);
                }
            } else {
                /* variable initialization */
                InterInstruction inst = new InterInstruction(TokenCode.Equal,
                    new VariableOperand(varId, curSymTab), ParseBool(), null);
                Emit(inst);
            }
        }

        private void ParseStmts() {
            /* FIRST(stmt) */
            while (curToken.code == TokenCode.Id || curToken.code == TokenCode.If ||
              curToken.code == TokenCode.For || curToken.code == TokenCode.While ||
              curToken.code == TokenCode.Do || curToken.code == TokenCode.Switch ||
              curToken.code == TokenCode.Break || curToken.code == TokenCode.Continue ||
              curToken.code == TokenCode.Return || curToken.code == TokenCode.OpenBrace ||
              curToken.code == TokenCode.SemiColon || curToken.code == TokenCode.PrintInt ||
              curToken.code == TokenCode.PrintStr || curToken.code == TokenCode.PrintChar ||
              curToken.code == TokenCode.ReadInt || curToken.code == TokenCode.ReadStr) {
                ParseStmt();
            }
        }

        private void ParseStmt() {
            switch (curToken.code) {
                /* assignment statement */
                case TokenCode.Id:
                    string id = curToken.lexeme;
                    Operand idOperand = ParseId(true);

                    if (idOperand == null)
                        ParseFunctionCall(id);
                    else
                        ParseAssignment(idOperand);

                    Match(TokenCode.SemiColon);
                    break;

                /* block of statements */
                case TokenCode.OpenBrace:
                    ParseBlock();
                    break;

                /* if statement */
                case TokenCode.If:
                    Match(TokenCode.If);
                    Match(TokenCode.OpenParen);

                    Operand ifCond = ParseBool();

                    LabelOperand ifNext = NewLabel();
                    LabelOperand ifFalse = ifNext;
                    LabelOperand ifTrue = NewLabel();

                    InterInstruction ifInst = new InterInstruction(TokenCode.If, ifCond, ifTrue, null);
                    InterInstruction gotoInst = new InterInstruction(TokenCode.Goto, ifFalse, null, null);
                    Emit(ifInst); Emit(gotoInst); Emit(ifTrue.label);

                    Match(TokenCode.CloseParen);

                    ParseStmt();

                    if (curToken.code == TokenCode.Else) {
                        ifNext = NewLabel();
                        Emit(new InterInstruction(TokenCode.Goto, ifNext, null, null));
                        Emit(ifFalse.label);

                        Match(TokenCode.Else);

                        ParseStmt();

                        Emit(ifNext.label);
                    } else
                        Emit(ifNext.label);

                    break;

                /* for statement */
                case TokenCode.For:
                    Match(TokenCode.For);
                    Match(TokenCode.OpenParen);

                    if (curToken.code == TokenCode.Id)
                        ParseAssignment();

                    Match(TokenCode.SemiColon);

                    LabelOperand forBegin = NewLabel();
                    LabelOperand forTrue = NewLabel();
                    LabelOperand forInc = NewLabel();
                    LabelOperand forNext = NewLabel();

                    Emit(forBegin.label);
                    breakStack.Push(forNext);
                    continueStack.Push(forInc);

                    if (curToken.code == TokenCode.Num || curToken.code == TokenCode.Id) {
                        Operand forCond = ParseBool();
                        Emit(new InterInstruction(TokenCode.If, forCond, forTrue, null));
                        Emit(new InterInstruction(TokenCode.Goto, forNext, null, null));
                        Emit(forTrue.label);
                    }

                    Match(TokenCode.SemiColon);

                    ArrayList assign3Inst = null;

                    if (curToken.code == TokenCode.Id) {
                        /* save the instructions array */
                        ArrayList temp = instructions;
                        /* new array for assignement instructions */
                        instructions = new ArrayList();

                        ParseAssignment();

                        assign3Inst = instructions;
                        instructions = temp;
                    }

                    Match(TokenCode.CloseParen);

                    ParseStmt();

                    Emit(forInc.label);
                    /* if third expression exist */
                    if (assign3Inst != null)
                        instructions.AddRange(assign3Inst);

                    Emit(new InterInstruction(TokenCode.Goto, forBegin, null, null));
                    Emit(forNext.label);

                    breakStack.Pop();
                    continueStack.Pop();
                    break;

                /* while statement */
                case TokenCode.While:
                    Match(TokenCode.While);
                    Match(TokenCode.OpenParen);

                    LabelOperand wBegin = NewLabel();
                    LabelOperand wTrue = NewLabel();
                    LabelOperand wNext = NewLabel();

                    breakStack.Push(wNext);
                    continueStack.Push(wBegin);

                    Emit(wBegin.label);
                    Operand wCond = ParseBool();

                    Match(TokenCode.CloseParen);

                    InterInstruction wInst = new InterInstruction(TokenCode.If, wCond, wTrue, null);
                    Emit(wInst);
                    Emit(new InterInstruction(TokenCode.Goto, wNext, null, null));
                    Emit(wTrue.label);

                    ParseStmt();

                    Emit(new InterInstruction(TokenCode.Goto, wBegin, null, null));
                    Emit(wNext.label);

                    breakStack.Pop();
                    continueStack.Pop();
                    break;

                /* do-while statement */
                case TokenCode.Do:
                    Match(TokenCode.Do);

                    LabelOperand doBegin = NewLabel();
                    LabelOperand doBoolCond = NewLabel();
                    LabelOperand doNext = NewLabel();

                    breakStack.Push(doNext);
                    continueStack.Push(doBoolCond);

                    Emit(doBegin.label);
                    ParseBlock();

                    Match(TokenCode.While); Match(TokenCode.OpenParen);

                    Emit(doBoolCond.label);

                    Operand doCond = ParseBool();
                    InterInstruction doInst = new InterInstruction(TokenCode.If, doCond, doBegin, null);
                    Emit(doInst);
                    Emit(doNext.label);

                    Match(TokenCode.CloseParen); Match(TokenCode.SemiColon);

                    breakStack.Pop();
                    continueStack.Pop();
                    break;

                /* switch statement */
                case TokenCode.Switch:
                    Match(TokenCode.Switch); Match(TokenCode.OpenParen);

                    LabelOperand swTest = NewLabel();
                    LabelOperand swDefault = NewLabel();
                    LabelOperand swNext = NewLabel();

                    breakStack.Push(swNext);

                    Operand value, swCond = ParseBool();
                    Emit(new InterInstruction(TokenCode.Goto, swTest, null, null));

                    Match(TokenCode.CloseParen); Match(TokenCode.OpenBrace);

                    ArrayList values = new ArrayList();
                    ArrayList labels = new ArrayList();

                    /* isDefault: indicates if current label is 'default'
                     * isDefParsed: indicates if 'default' label is already parsed */
                    bool isDefParsed = false;
                    bool isDefault = false;

                    while (curToken.code == TokenCode.Case || curToken.code == TokenCode.Default) {
                        /* case value*/
                        value = null;

                        isDefault = curToken.code == TokenCode.Default;

                        /* match 'case' or 'default' */
                        Match(curToken.code);

                        if (isDefault) {
                            if (isDefParsed)
                                Error("more than one default.");

                            isDefParsed = true;

                            Match(TokenCode.Colon);
                            Emit(swDefault.label);

                            ParseStmts();
                            //Emit(new InterInstruction(TokenCode.Goto, swNext, null, null));
                        } else {
                            value = ParseBool();

                            Match(TokenCode.Colon);

                            LabelOperand caseLabel = NewLabel();
                            Emit(caseLabel.label);

                            ParseStmts();
                            //Emit(new InterInstruction(TokenCode.Goto, swNext, null, null));

                            values.Add(value);
                            labels.Add(caseLabel);
                        }
                    }

                    Match(TokenCode.CloseBrace);

                    Emit(new InterInstruction(TokenCode.Goto, swNext, null, null));
                    Emit(swTest.label);

                    for (int i = 0; i < labels.Count; i++) {
                        Operand temp = NewTemp();

                        Emit(new InterInstruction(TokenCode.EqualEqual, temp, swCond, (Operand)values[i]));
                        Emit(new InterInstruction(TokenCode.If, temp, (Operand)labels[i], null));
                    }

                    if (isDefParsed)
                        Emit(new InterInstruction(TokenCode.Goto, swDefault, null, null));

                    Emit(swNext.label);

                    breakStack.Pop();
                    break;

                /* null statement (;) */
                case TokenCode.SemiColon:
                    Match(TokenCode.SemiColon);
                    break;

                /* break */
                case TokenCode.Break:
                    object brkTarget = breakStack.Peek();

                    if (brkTarget != null)
                        Emit(new InterInstruction(TokenCode.Goto, (LabelOperand)brkTarget, null, null));
                    else
                        Error("'break' statement is not allowed here.");

                    Match(TokenCode.Break); Match(TokenCode.SemiColon);
                    break;

                /* continue */
                case TokenCode.Continue:
                    object conTarget = continueStack.Peek();

                    if (conTarget != null)
                        Emit(new InterInstruction(TokenCode.Goto, (LabelOperand)conTarget, null, null));
                    else
                        Error("'continue' statement is not allowed here.");

                    Match(TokenCode.Continue); Match(TokenCode.SemiColon);
                    break;

                case TokenCode.Return:
                    Match(TokenCode.Return);

                    Operand retVal = null;
                    if (curFunction.returnType != Type.Void)
                        retVal = ParseBool();

                    if (curToken.code != TokenCode.SemiColon)
                        Error("can't return value in a function returning void.");

                    Match(TokenCode.SemiColon);

                    Emit(new InterInstruction(TokenCode.Return, retVal, null, null));
                    break;

                case TokenCode.ReadStr:
                    Match(TokenCode.ReadStr);
                    Match(TokenCode.OpenParen);
                    Operand inBuffId = ParseId(false);
                    Match(TokenCode.Comma);
                    Operand inBuffLim = ParseExpr();
                    Match(TokenCode.CloseParen);
                    Match(TokenCode.SemiColon);
                    Emit(new InterInstruction(TokenCode.ReadStr, inBuffId, inBuffLim, null));
                    break;

                case TokenCode.ReadInt:
                    Match(TokenCode.ReadInt);
                    Match(TokenCode.OpenParen);
                    Operand var = ParseId(false);
                    Match(TokenCode.CloseParen);
                    Match(TokenCode.SemiColon);
                    Emit(new InterInstruction(TokenCode.ReadInt, var, null, null));
                    break;

                case TokenCode.PrintChar:
                case TokenCode.PrintInt:
                    TokenCode func = curToken.code;
                    Match(curToken.code);
                    Match(TokenCode.OpenParen);
                    Operand expOp = ParseExpr();
                    Match(TokenCode.CloseParen);
                    Match(TokenCode.SemiColon);
                    Emit(new InterInstruction(func, expOp, null, null));
                    break;

                case TokenCode.PrintStr:
                    Match(TokenCode.PrintStr);
                    Match(TokenCode.OpenParen);

                    if (curToken.code == TokenCode.Id) {
                        /* print buffer */
                        Operand buffOp = ParseId(false);
                        Emit(new InterInstruction(TokenCode.PrintStr, buffOp /*arrOp*/, null, null));
                    } else {
                        /* print immidiate string */
                        string text = "";
                        if (curToken.code == TokenCode.String)
                            text = curToken.lexeme;
                        Match(TokenCode.String);

                        /* create temporary buffer for string (temp array) */
                        VariableOperand tempBuffer = NewTemp(Type.Char, true, text.Length + 1);
                        CopyStringToBuffer(tempBuffer.symbol.lexeme, text);
                        Emit(new InterInstruction(TokenCode.PrintStr, tempBuffer, null, null));
                    }

                    Match(TokenCode.CloseParen);
                    Match(TokenCode.SemiColon);
                    break;
            }
        }

        private void ParseAssignment() {
            ParseAssignment(null);
        }

        private void ParseAssignment(Operand var) {
            if (var == null)
                var = ParseId(false);

            if (curToken.code == TokenCode.PlusPlus || curToken.code == TokenCode.MinusMinus) {
                /* var++ or var-- */
                TokenCode op = curToken.code == TokenCode.PlusPlus ? TokenCode.AddOp : TokenCode.SubOp;

                Match(curToken.code);

                InterInstruction assInst = new InterInstruction(op, var, var, new NumberOperand(1));
                Emit(assInst);
            } else {
                /* var = expression */
                Match(TokenCode.Equal);

                InterInstruction assignInst = new InterInstruction(TokenCode.Equal, var, ParseBool(), null);
                Emit(assignInst);
            }
        }

        private Operand ParseBool() {
            if (curToken.code == TokenCode.Id || curToken.code == TokenCode.Num ||
                curToken.code == TokenCode.OpenParen || curToken.code == TokenCode.SubOp ||
                curToken.code == TokenCode.Not) {
                Operand t, operand1 = ParseAnd();

                while (curToken.code == TokenCode.Or) {
                    Match(TokenCode.Or);

                    t = NewTemp();
                    InterInstruction inst = new InterInstruction(TokenCode.Or, t, operand1, ParseAnd());
                    Emit(inst);
                    operand1 = t;
                }

                return operand1;
            } else {
                Error("id, number, '(', '-' or '!' expected.");

                return null;
            }
        }

        private Operand ParseAnd() {
            Operand t;
            Operand operand1 = ParseEquality();

            while (curToken.code == TokenCode.And) {
                Match(TokenCode.And);

                t = NewTemp();
                InterInstruction inst = new InterInstruction(TokenCode.And, t, operand1, ParseEquality());
                Emit(inst);
                operand1 = t;
            }

            return operand1;
        }

        private Operand ParseEquality() {
            Operand t;
            Operand operand1 = ParseRel();

            while (curToken.code == TokenCode.EqualEqual || curToken.code == TokenCode.NotEq) {
                TokenCode op = curToken.code;
                Match(curToken.code);

                t = NewTemp();
                InterInstruction inst = new InterInstruction(op, t, operand1, ParseRel());
                Emit(inst);
                operand1 = t;
            }
            return operand1;
        }

        private Operand ParseRel() {
            Operand t;
            Operand operand1 = ParseExpr();

            while (curToken.code == TokenCode.Less || curToken.code == TokenCode.LessOrEq ||
              curToken.code == TokenCode.Greater || curToken.code == TokenCode.GreaterOrEq) {
                TokenCode op = curToken.code;
                Match(curToken.code);

                t = NewTemp();
                InterInstruction inst = new InterInstruction(op, t, operand1, ParseExpr());
                Emit(inst);
                operand1 = t;
            }
            return operand1;
        }

        private Operand ParseExpr() {
            Operand t;
            Operand operand1 = ParseTerm();

            while (curToken.code == TokenCode.AddOp || curToken.code == TokenCode.SubOp) {
                TokenCode op = curToken.code;
                Match(curToken.code);

                t = NewTemp();
                InterInstruction inst = new InterInstruction(op, t, operand1, ParseTerm());
                Emit(inst);
                operand1 = t;
            }
            return operand1;
        }

        private Operand ParseTerm() {
            Operand t;
            Operand operand1 = ParseFactor();

            while (curToken.code == TokenCode.MulOp || curToken.code == TokenCode.DivOp
                || curToken.code == TokenCode.ModOp) {
                TokenCode op = curToken.code;
                Match(curToken.code);

                t = NewTemp();
                InterInstruction inst = new InterInstruction(op, t, operand1, ParseFactor());
                Emit(inst);
                operand1 = t;

            }
            return operand1;
        }

        private Operand ParseFactor() {
            Operand ret = null;

            switch (curToken.code) {
                case TokenCode.OpenParen:
                    Match(TokenCode.OpenParen);
                    ret = ParseBool();
                    Match(TokenCode.CloseParen);
                    break;

                case TokenCode.Not:
                    Match(TokenCode.Not);
                    ret = NewTemp();
                    InterInstruction ninst = new InterInstruction(TokenCode.Not, ret, ParseFactor(), null);
                    Emit(ninst);
                    break;

                case TokenCode.SubOp:
                    Match(TokenCode.SubOp);
                    ret = NewTemp();
                    InterInstruction sInst = new InterInstruction(TokenCode.SubOp, ret, ParseFactor(), null);
                    Emit(sInst);
                    break;

                case TokenCode.Id:
                    string id = curToken.lexeme;
                    ret = ParseId(true);
                    if (ret == null)
                        ret = ParseFunctionCall(id);
                    break;

                case TokenCode.Num:
                    ret = new NumberOperand(curToken.value);
                    Match(TokenCode.Num);
                    break;

                default:
                    Error("id, number, '(', '-' or '!' expected.");
                    break;
            }

            return ret;
        }

        private Operand ParseId(bool allowFunc) {
            string varId = curToken.lexeme;
            Symbol symbol = curSymTab.Lookup(varId);

            if (symbol == null)
                Error(varId + " is undeclared identifier.");

            Match(TokenCode.Id);

            if (curToken.code == TokenCode.OpenBracket) {
                if (!((VariableSymbol)symbol).isArray)
                    Error(varId + " is not array.");

                return ParseIndex(varId);
            } else if ((curToken.code == TokenCode.OpenParen) && (allowFunc))
                return null;
            else {
                if (!(symbol is VariableSymbol))
                    Error(varId + " is not a variable.");

                return new VariableOperand(varId, curSymTab);
            }
        }

        private Operand ParseFunctionCall(string funcId) {
            Symbol symbol = curSymTab.Lookup(funcId);
            FunctionSymbol funcSymbol = null;

            if (symbol == null)
                Error(funcId + " is undeclared identifier.");

            if (!(symbol is FunctionSymbol))
                Error(funcId + " is not a function.");
            else
                funcSymbol = (FunctionSymbol)symbol;

            ArrayList args = ParseArguments();

            if (funcSymbol != null) {
                if (funcSymbol.parameters.Count != args.Count)
                    Error(funcId + " takes " + funcSymbol.parameters.Count + " argument(s).");

                /* push arguments on the stack in reverse order */
                for (int i = args.Count - 1; i >= 0; i--)
                    Emit(new InterInstruction(TokenCode.Param, (Operand)args[i], null, null));
            }

            Operand retOperand = NewTemp();
            Emit(new InterInstruction(TokenCode.Call, retOperand, new FunctionOperand(funcId), null));
            Emit(new InterInstruction(TokenCode.IncStackPtr, new NumberOperand(args.Count * 4), null, null));

            return retOperand;
        }

        private ArrayList ParseArguments() {
            ArrayList args = new ArrayList();

            Match(TokenCode.OpenParen);

            // if (curToken == FIRST(Expr)) 
            if (curToken.code == TokenCode.Id || curToken.code == TokenCode.Num ||
                curToken.code == TokenCode.OpenParen || curToken.code == TokenCode.SubOp ||
                curToken.code == TokenCode.Not) {
                do {
                    args.Add(ParseBool());

                } while (MatchIf(TokenCode.Comma));
            }

            Match(TokenCode.CloseParen);

            return args;
        }

        private Operand ParseIndex(string arrId) {
            Match(TokenCode.OpenBracket);
            Operand index = ParseExpr();
            Match(TokenCode.CloseBracket);

            return new ArrayOperand(arrId, index, curSymTab);
        }
    }
}