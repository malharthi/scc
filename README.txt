
Simple C-Subset Compiler
========================

Introductoin
============

Simple C-Subset Compiler is my graduation project in KAU Univiresty. It translates source programs written in a subset of the C language into Intel i386 assembly language, which will be then passed to assembler to generate an executable file.

The subset is simple, there is no pointers nor structures, just the known C statements, two data types includeing arrays and functions, with several built in functions for screen IO.

The goal of this project was providing a simple and full working compiler, which can be used to show the undergraduate students how compilers can be implemented.

Implementation
==============

The compiler is wriiten in C++, and both the scanner and the parser are hand-written. The parser is a recursive-descent parser. but I optimized most of the recursions and converted it to loops as it was possible.

The fornt-end is single pass, and generates intermediate code immediatly, and the backend is single pass too, which is take the intermediate code and translate each instruction to its equavelant instructions of the i386 assembly. You may wonder why the compiler is not a single pass compiler. I did that so i can add an optimization phase to one or both of the two passes later.  

The resulting assembly code is assembled and linked using the assembler and linker included with Microsoft Visual C++, Which is required to compile your code successfully.

Note
====
It is maybe a buggy compiler, if you find a bug, please feel free to report it to me (mohannad.harthi@gmail.com).


C Subset
========
	The compiler can compile a subset of the C language, which is defined as the following:

	Data types
	==========
	
	The subset supports two data types which is defined as the following: 
		1. int  : 32-bit signed integer.
		2. char : 8-bit unsigned integer.
	
	Static arrays is only supported.

	Note: Arrays is not implemented as pointers yet, if you pass an array to a function the first elemente will be passed instead. I will fix that as soon as possible.

	Literals
	========
		- Integers: 2343, -323 
		- Strings: "Hi, \r\n My name is Mohannad \t"
		- Charactars: 'a', '\n'
	

	Expressions
	===========
	These operators are only supported:
        +, -, post ++ and --,  *, /, %, <, <=, >, >=, ==, ||, && (short circuit is not supported).

	Statements
	==========
	- if-else statement.
	- switch statement.
	- while statement.
	- do-while statement.
	- for statement.
	- break and continue statements.

	Functions
	=========
	There is several built in functions for screen IO, which is described below:
	
	- printStr(char str[]);
		Prints a null-terminated string into the screen.
	- printChar(char c);
	- printInt(int n);
	- readStr(char buffer[], int bufferSize);
	- readInt(int n)
	
	Note: Theses functions is translated to calls to the Irvine32 library functions, which is statically linked to the executable file.

	Yo can define your own functions, but you have to know that you cannot pass arrays, and you cannot write forward declaritions for functions like this: int f(int x); .
	