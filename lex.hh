#ifndef LEX_HH
#define LEX_HH

#define MAX_OP_NUM 64
#include <string>
#include "load.hh"

static std :: string  lcKeyWord[]={
	/*the sequence of  newly added keywords must be consist to that of lcTypes */
	"if" , "else" , "ret" , "mul" , "div" , "and" , "or" , "not", "var" , "true" , "false" , "null"
};
enum lcType{
	GT,	//>
	LT,	//<
	VALUE,	//=
	NEQ,	//!=
	EQ,	//==
	NLT,	//>=
	NGT,	//<=
	ADD,	//+  NA
	SUB,	//-
	QMARK, 	//"
	COMMA,	//, NA
	DOT,	//. NA
	LBRK,	//[ NA
	RBRK,	//] NA
	LBRC,	//{ NA
	RBRC,	//} NA
	LPR,	//( NA
	RPR,	//) NA
	FUNC,		//:F
	LOOP,		//:L
	//TABEL,		//:T
	ID,	//<identifier>
	STR,	//<string>
	INT,	//<int>
	FLOAT,	//<float>

	IF = MAX_OP_NUM,	//if
	ELSE,	///else
	RET,	//ret
	MUL,	//mul
	DIV,	//div
	AND,	//and
	OR,	//or
	NOT,	//not
	VAR,	//local variabal
	T,
	F,
	NI
};

struct LexWord{
	enum lcType 	type;
	std :: string 	content;
	int 		row;
	int 		col;
};

struct TextPos{
	int curx;
	int cury;
	int forx;
	int fory;
};

class lcLex {
	public:
		std::vector <std :: string> vectorLine;  //initalized by CodeLine in Load.hh
		std:: vector <LexWord> vectorWord;	// target words stream
		TextPos txtps ;
		lcLex(std::vector<std::string>& v);
		~lcLex();
		void LexAnalyze();
		void PushWord(enum lcType, std::string ,int ,int);
		void PushAnyEqual(enum lcType t , std::vector< std::string>&v , int len , TextPos* p, enum lcType t1);
		void PushColonAny(std::vector<std::string> &v,int len, TextPos* p);
		void PushID(int len, TextPos* p);
		void PushNum(int len ,TextPos* p);
		enum lcType isKeyword(std::string s);
		void expect (std::string& s , struct TextPos* p , int len );
		void printTokens();

	private: 
		int keyNumber;
};
#endif 