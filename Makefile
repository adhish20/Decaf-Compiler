all: 
	clear
	bison -vd parser.y
	flex scanner.l
	gcc parser.tab.c lex.yy.c -lfl -o compilerV1
	./compilerV1 test_program > v1out.txt
	cat v1out.txt

clean: 
	rm -rf v1out.txt
