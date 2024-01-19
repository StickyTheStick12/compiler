compiler: lex.yy.c parser.tab.o main.cpp
		g++ -g -w -ocompiler parser.tab.o lex.yy.c main.cpp -std=c++14
parser.tab.o: parser.tab.cc
		g++ -g -w -c parser.tab.cc -std=c++14
parser.tab.cc: parser.yy
		bison parser.yy
lex.yy.c: MiniJavaLexer.l parser.tab.cc
		flex MiniJavaLexer.l
tree: 
		 dot -Tpdf tree.dot -otree.pdf
clean:
		rm -f parser.tab.* lex.yy.c* compiler stack.hh position.hh location.hh tree.dot tree.pdf
		rm -R compiler.dSYM
