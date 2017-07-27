// module for loading code files
#ifndef LOAD_HH
#define LOAD_HH
#include <vector>
#include <string>

 //void loadcode(char*);
// extern std::vector< std:: string> CodeLine;
 /*
 if stating a vector as a shared var among c++ files, a "extern" keyword is needed and 
 an initialization for the vector in any file using it as well. 
 */

 class lcLoad{
 	public:
 		lcLoad(char* filename);  
 		void loadcode();   
 		std :: vector <std :: string> CodeLine; //a vector consists of lines of a file
 		~lcLoad();
 	private: 
 		char* filename;
 } ;
#endif
