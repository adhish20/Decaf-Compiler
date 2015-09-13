all: 
	clear
	bison -vd parser.y
	flex scanner.l
	gcc parser.tab.c lex.yy.c -lfl -o compilerV1
	#./compilerV1 test_program > output

clean: 
	rm -rf output compilerV1 lex.yy.c parser.output parser.tab.c parser.tab.h
