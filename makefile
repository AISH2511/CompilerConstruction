.PHONY: stage1exe
stage1exe:
	gcc driver.c parser.c lexer.c stack.c ast.c symbolTable.c semanticAnalysis.c -o stage1exe