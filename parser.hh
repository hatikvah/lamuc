#ifndef PARSER_HH
#define PARSER_HH
#include <vector>
#include <string>
#include "lex.hh"

enum NodeType{
	AST_COMMA, //0
	AST_OR, //1
	AST_AND, //2
	AST_COMP, //3
	AST_ADD, //4
	AST_MUL,//5
	AST_UNARY,//6
	AST_FUN, //7   e.g. f(a,b)  or  array[0] 
	AST_OF ,//10
	AST_BRK =32,
	AST_PRM, 
	AST_CHUNK,
	AST_BLOCK,
	AST_LOOP,
	AST_FUNC,
	AST_IFELSE,
	AST_STATE,
	AST_EXP,
	AST_EXPLIST,
	AST_TERM,
	AST_RES,
	AST_VARLIST,
	AST_RET,
	AST_EVAL,
	AST_FUNCTION
};
/*
union INFO{
	LexWord lexinfo;
	int i;
};*/
struct AST{
	NodeType ntype;
	AST* parent ;
	std::vector<AST*> children;
	LexWord* lexinfo;
};

class lcParser{

	public:
		lcParser(lcLex& le);
		lcLex& lexer; // a pointer to lex class
		AST* chunk;  // AST root pointer
		int pos;
		int depth;
		std::vector<LexWord>& w;
		~lcParser();
		void DelTree();
		void doParse();
		AST* parseEXP();
		AST* newAST(NodeType,LexWord* plex);
		int isOp(lcType t);

		AST* linsert(AST* currentPos , AST* newNode);
		AST* appendchild(AST* currentPos , AST* newNode);
		AST* matchback(NodeType,AST*);
		AST* doparseEXP(int flr,AST* node);
		AST* checknext(AST* , lcType , int , int&);
		NodeType convert(lcType);
		void debugAST(AST*);

		AST* parseAnony();
		//bool checkVar(AST*);
		AST* parseBLOCK(int flag);
		AST* parseLOOP();
		AST* parseIFELSE();
		AST* parseFUNC();
		AST* parseRET();
		AST* parseEVAL();
		AST* parseSTATE();
		AST* parseBRC();
		bool checkIDLIST(AST*);

		bool match(lcType t , lcType t1);
		void error(std::string , LexWord*);
};	
#endif