// start of the compiling where main function is 
#include <iostream>  
#include "load.hh"
#include "lex.hh"
#include "parser.hh"
#include <vector>
#include <string>

int main(){
	lcLoad loader("sample/test_load.lc");
	loader.loadcode(); 
	lcLex lexer(loader.CodeLine);
	lexer.LexAnalyze();
	lcParser parser(lexer);
	parser.doParse();
	//std::cout<<lexer.vectorWord[0].content;
}

