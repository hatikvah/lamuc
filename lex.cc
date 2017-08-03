#include <iostream>
#include <string>
#include <string.h>
#include "load.hh"
#include <stdlib.h>
#include "lex.hh"

lcLex :: lcLex(std::vector<std::string>& v) : vectorWord() , vectorLine(v){
	//this -> vectorLine = v;
	/*
	there will be two copies of v in memories , we should change a loader as a var in lexer ,therefore 
	only one copy will remain.
	*/
	this -> txtps.curx =  this ->txtps.forx  = 0;
	this ->txtps.cury= this->txtps.fory =0;
	std :: cout<<"Lex init .."<<std :: endl;
	this->keyNumber = sizeof(lcKeyWord)/sizeof(char*);
}

lcLex :: ~lcLex(){
	// free vectorWord
	std::cout<<"destructing lex obj .."<<std::endl;
	this -> vectorWord.clear();
	std :: vector <LexWord> () . swap (this -> vectorWord);
}

void lcLex::PushWord(enum lcType t , std::string content , int r , int c){
	struct LexWord temp = {
		.type = t,
		.content = content,
		.row = r,
		.col = c
	};
	this->vectorWord.push_back(temp);
}

/*
inline char getcharAt(TextPos* p, std::vector<std::string>& v){
	return v[p->curx][p->cury];
}
*/

int getNewPos(TextPos* p , std::vector< std:: string> & v){

	if(v.size() ==0){
		return 0;
	}

	while(p->cury ==v[p->curx].length() ||v[p->curx].length() == 0){
		// at the end of a line
		if(p->curx != v.size()-1 ){
			p->curx++;
			p->cury=0;  // lex from a new line
		} else{
			// finish lex
			return 0;
		}
	}

	return int(v[p->curx][p->cury]);
}

void  lcLex :: PushAnyEqual(enum lcType t , std::vector< std::string>&v , int len , TextPos* p, enum lcType t1){
	if(len-p->cury>1 && v[p->curx][p->cury+1] == '=' ){
		PushWord(t,"<=>=",p->curx,p->cury);
		p->cury=p->cury+2;
	}
	else if(t1 != NEQ){
		PushWord(t1,"<=>",p->curx,p->cury);
		p->cury++;
	}
	else{
		std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<" unknown char: "<<"!"<<std::endl;
		exit(1);
	}
}

void lcLex :: PushColonAny(std::vector<std::string> &v,int len, TextPos* p){
	if( (len-p->cury>2 && (v[p->curx][p->cury+2] ==' ' || v[p->curx][p->cury]== '	')) 
		|| (len-p->cury==2)){

		//std::cout<<"handling <:>"<<std::endl;
		if(v[p->curx][p->cury+1] == 'F'){
			PushWord(FUNC,":F", p->curx, p->cury);
		}else if( v[p->curx][p->cury+1] == 'L'){
			PushWord(LOOP,":L", p->curx, p->cury);
		}/*else if(v[p->curx][p->cury+1] == 'T'){
			PushWord(TABEL,":T",p->curx,p->cury);
		}*/else{
			std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<" undefined block with ':' "<<std::endl;
			exit(1);
		}

		p->cury=((p->cury+2)<len)? (p->cury+3):len;

	}else{
		std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<" meaningless symbol ':' "<<std::endl;
		exit(1);
	}
}

enum lcType lcLex::isKeyword(std::string s){
	for (int i =0;i< this->keyNumber;i++){
		if (s==lcKeyWord[i]){
			return lcType(MAX_OP_NUM+i);
		}
	}

	return ID;
}

void lcLex::PushID(int len, TextPos* p){
	char c =vectorLine[p->curx][p->cury];
	p->fory=p->cury;
	bool d = false;
	while((c>='a' && c<='z') || (c>='A' && c<='Z') ||(c>='0' && c<='9')|| c =='\'' || c=='_'){
		if(c =='\'' && d == false){
			d = true;
			p->fory++;
		}else if( d== true && c != '\''){
			std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<" malformed id with  extra ' "<<std::endl;
			exit(1);
		}else{
			p->fory++;
		}

		c = (len-p->fory>0)? vectorLine[p->curx][p->fory] : 0;
	}

	std::string stmp = vectorLine[p->curx].substr(p->cury,p->fory-p->cury);
	enum lcType seq=isKeyword(stmp);
	PushWord(seq, stmp, p->curx, p->cury);
	p->cury=p->fory;
}

void lcLex::PushNum(int len ,TextPos* p){
	char c =vectorLine[p->curx][p->cury];
	enum lcType NumType= INT;
	p->fory = p->cury;
	while((c>='0' && c<='9') || c=='.'){
		if(c =='.' && NumType == INT ){
			NumType = FLOAT;
			p->fory++;
		}
		else if(c != '.'){
			p->fory++;
		}else{
			std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<" extra symbol '.' "<<std::endl;
			exit(1);
		}

		c =(len-p->fory>0)? vectorLine[p->curx][p->fory] : 0;
	}

	std::string stmp = vectorLine[p->curx].substr(p->cury,p->fory-p->cury);
	PushWord(NumType, stmp , p->curx, p->cury);
	p->cury=p->fory;
}

void lcLex :: LexAnalyze(){
	int linenum = this->vectorLine.size();
	struct TextPos* p = &(this ->txtps);

	while(true){

		char tmpchar= getNewPos(p,vectorLine);
		int len=vectorLine[p->curx].length();
		//std::cout<<"<("<<int(tmpchar)<<")"<<tmpchar<<">"<<std::endl;
		if (tmpchar>0){
			//p->cury++;
			switch (tmpchar) {
			case ' ' 	:	
			case '	':  p->cury++; break;
			case '+':PushWord(ADD,"+",p->curx,p->cury); p->cury++;break;
			case ',':PushWord(COMMA,",",p->curx,p->cury);p->cury++;break;
			case '.':PushWord(DOT,".",p->curx,p->cury);p->cury++;break;
			case '{':PushWord(LBRC,"{",p->curx,p->cury);p->cury++;break;
			case '}':PushWord(RBRC,"}",p->curx,p->cury);p->cury++;break;
			case '[':PushWord(LBRK,"[",p->curx,p->cury);p->cury++;break;
			case ']':PushWord(RBRK,"]",p->curx,p->cury);p->cury++;break;
			case '(':PushWord(LPR,"(",p->curx,p->cury);p->cury++;break;
			case ')':PushWord(RPR,")",p->curx,p->cury);p->cury++;break;
			case '=':PushAnyEqual(EQ, vectorLine, len, p,VALUE);break;
			case '>':PushAnyEqual(NLT, vectorLine , len ,p,GT);break;
			case '<':PushAnyEqual(NGT, vectorLine,len,p,LT);break;
			case '!':PushAnyEqual(NEQ, vectorLine,len,p, NEQ);break;
			case ':':PushColonAny(vectorLine, len, p);break;
			case '-':
				if( len-p->cury>1 && vectorLine[p->curx][p->cury+1] == '-'){
					p->cury=len;
				}else{
					PushWord(SUB, "-", p->curx, p->cury);
					p->cury++;
				}

				break;

			case '\"':expect(vectorLine[p->curx],p,len);break;
			default: 
				//int len=vectorLine[p->curx].length();
				if(  (tmpchar>='a' && tmpchar <='z') ||
				     (tmpchar>='A' &&  tmpchar <='Z') 
				     ){
					//maybe a id or keyword
					PushID(len,p);
				}
				else if  (tmpchar>='0' && tmpchar <= '9') {
					PushNum(len,p);
				}
				else{
					std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<"unknown char"<<tmpchar<<std::endl;
					exit(1);
				}
				break;
			}
		}
		else{
			std::cout<<"lex completed .."<<std::endl;
			printTokens();
			return ;
		}
		//std::cout<<tmpchar<<std::endl;
	}
}

void lcLex::expect (std::string& s , struct TextPos* p , int len ){
	char* snew = new char [len];
	p->cury++;
	p->fory=p->cury;
	char c;
	int tail = 0;
	while(p->fory < len ){
		c = s[p->fory];
		//std::cout<<c<<" ";
		if( c == '\\'){
			if(len - p->fory > 1){
				// read next char 
				switch (s[p->fory+1])
				{
					case 'n': snew[tail] = '\n';break;
					case  'r':snew[tail] = '\r';break;
					case '\"':snew[tail] ='\"';break;
					case '\\':snew[tail] ='\\';break;
					default:
						std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<"unexpect char"<<std::endl;
						exit(1);
				}
				p->fory=p->fory+2;
				tail++;
			}else{
				std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<"expect \""<<std::endl;
				exit(1);
			}
		}

		else{
			if( c == '\"'){
				// finish handle string
				PushWord(STR, std::string(snew).substr(0,tail), p->curx, p->cury);
				p->cury=p->fory+1;
				return ;
			}else{
				snew[tail] = c;
				p->fory++;
				tail++;
			}
		}
	}

	std::cout<<"lex error: line: "<<p->curx<<" pos: "<<p->cury<<"expect \""<<std::endl;
	exit(1);

}

void lcLex::printTokens(){
	int curline = vectorWord[0].row;
	for(int i=0; i<vectorWord.size();i++){
		std::cout<<"<"<<vectorWord[i].content<<"> ";
		if((i<vectorWord.size()-1) && vectorWord[i+1].row>curline){
			std::cout<<std::endl;
			curline = vectorWord[i+1].row;
		}
	}
}