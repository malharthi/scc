#!/usr/bin/env python

import sys
import subprocess

def main(argv):
	res = subprocess.call (["g++", "-o", "./build/scc", "-g3", "-fno-inline", "-O0", 
			"./src/ccomp.cc", "./src/symbol_table.cc", "./src/lexer.cc",
			"./src/parser.cc", "./src/intermediate.cc", "./src/code_gen.cc",
			"./src/str_helper.cc"])
	if res:
		print "Compilation failed. Make Sure you have GCC installed."
	else:
		print "SCC is compiled succeddfully!"
	pass

if __name__ == '__main__':
  main(sys.argv[1:])