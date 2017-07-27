// start of the compiling where main function is 
#include <iostream>  
#include "load.hh"
#include <vector>
#include <string>

int main(){
	lcLoad loader("sample/test_load.lc");
	loader.loadcode();
	std::cout<<loader.CodeLine[1]<<std::endl;
	return 0;
}

