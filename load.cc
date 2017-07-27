#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include "load.hh"

lcLoad :: lcLoad(char* filename):CodeLine(){
	this -> filename=filename;
}

lcLoad :: ~lcLoad(){
	this -> CodeLine.clear();
	std :: vector <std :: string> () . swap (this -> CodeLine);
}

int checkfilename (char* filename){
	int len=0;
	//std::cout<<len<<std::endl;
	if ((len = strlen(filename))>3 && 
		filename[len-1] == 'c' && 
		filename[len-2] == 'l' && 
		filename[len-3] == '.'){

		return 0;
	} else{
		std :: cout<<"Error :: Not a Lamuc code file."<<std::endl;
		return -1;
	}
}


void do_loadcode(std::ifstream &iob , lcLoad* p){
	std::string s;
	while (!iob.eof()){
		getline(iob,s);
		//std::cout<<s;
		p -> CodeLine.push_back(s);
	}
}

void lcLoad ::  loadcode (){
	if(checkfilename(this -> filename)==0){
		std::ifstream iob (this -> filename);
		do_loadcode(iob,this);
	}
}