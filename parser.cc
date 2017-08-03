#include <string.h>
#include "lex.hh"
#include "parser.hh"
#include <vector>
#include <iostream>
#include <stdlib.h>

/*
this parser is not strict , some type checking is needed in dynamic check
and upvalue is only allowed defining in eval exp
e.g (a+b) = c or fun((a,b),c) or (1+2) is not permitted  
*/

bool lcParser::checkIDLIST(AST*){
	return true;
}

void lcParser::error(std::string str,LexWord*p){
	std::cout<<"syntex error: near line "<<p->row<<", col "<<p->col<<":"<<str<<std::endl;
	exit(1);
}

bool lcParser::match(lcType t , lcType t1){
	if(t==t1){
		pos++;
		return true;
	}else{
		error("wrong token",&w[pos]);
		return false;
	}
}

AST* lcParser::parseAnony(){
	match(w[pos].type , LPR);
	pos --;
	AST* arglist = parseEXP();
	pos --;
	checkIDLIST(arglist);
	match(w[pos].type ,RPR);
	match(w[pos].type,LBRC);
	AST* block = parseBLOCK(1);
	match(w[pos].type,RBRC);
	AST* AnonyNode = newAST(AST_FUNCTION, NULL);
	AnonyNode->children.push_back(arglist);
	AnonyNode->children.push_back(block);
	return AnonyNode;
}

AST* lcParser::parseBLOCK(int flag){
	AST* branch ;
	AST* block = newAST(AST_BLOCK , NULL);
	while(w[pos].type != ENDFILE && w[pos].type != RBRC){

		switch(w[pos].type){
		//std::cout<<w[pos].content<<std::endl;
		case FUNC: branch = parseFUNC(); break;
		case IF: branch = parseIFELSE();break;
		case RET: branch = parseRET();break;// ret a , b , c 
		case VAR:  branch = parseSTATE();break;// var a , b , c
		case LBRK: branch = parseBRC();break;//{ BLOCK }
		case LOOP: branch = parseLOOP();break;
		default: branch = parseEVAL();break;
		}

		block->children.push_back(branch);

	}

	if((flag ==1 && w[pos].type == RBRC) || (flag ==0 && w[pos].type != RBRC))
		return block;
	else{
		error("block not matching",&w[pos]);
		return NULL;
	}
}

AST* lcParser::parseBRC(){
	pos++;
	AST* brc = newAST(AST_BLOCK,NULL);
	brc->children.push_back(parseBLOCK(1));
	match(w[pos].type , RBRC);
	return brc;
}
AST* lcParser::parseLOOP(){
	AST* loop =newAST(AST_LOOP,NULL);
	pos++;
	match(w[pos].type , LPR);
	pos--;
	AST* condlist = parseEXP();
	pos--;
	match(w[pos].type , RPR);
	match(w[pos].type, LBRC);
	AST* block = parseBLOCK(1);
	match(w[pos].type , RBRC);
	loop->children.push_back(condlist);
	loop->children.push_back(block);
	return loop;
}
AST* lcParser::parseIFELSE(){

	AST* ifelse = newAST(AST_LOOP,NULL);
	pos++;
	match(w[pos].type , LPR);
	pos--;
	AST* condlist = parseEXP();
	pos--;
	match(w[pos].type , RPR);
	match(w[pos].type , LBRC);
	AST* ifblock = parseBLOCK(1);
	match(w[pos].type  , RBRC);
	AST* elseblock = NULL;
	if(w[pos].type == ELSE){
		pos++;
		match(w[pos].type ,LBRC);
		elseblock = parseBLOCK(1);
		match(w[pos].type , RBRC);
	}

	ifelse->children.push_back(condlist);
	ifelse->children.push_back(ifblock);
	ifelse->children.push_back(elseblock);
	return ifelse;
}

AST* lcParser::parseFUNC(){
	AST* fun =newAST(AST_FUNCTION,NULL);
	pos++;
	match(w[pos].type , ID);
	match(w[pos].type , LPR);
	pos--;
	AST* arglist = parseEXP();
	pos--;
	match(w[pos].type , RPR);
	match(w[pos].type, LBRC);
	AST* block = parseBLOCK(1);
	match(w[pos].type , RBRC);
	fun->children.push_back(arglist);
	fun->children.push_back(block);
	return fun;
}

AST* lcParser::parseRET(){
	AST* ret = newAST(AST_RET,NULL);
	pos++;
	ret->children.push_back(parseEXP());
	return ret;
}

AST* lcParser::parseEVAL(){
	AST* value = newAST(AST_EVAL , NULL);
	value->children.push_back(parseEXP());
	if(w[pos].type == VALUE){
		pos++;
		value->children.push_back(parseEXP());
	}
	return value;
}
AST* lcParser::parseSTATE(){
	AST* state = newAST(AST_STATE , NULL);
	pos++;
	state->children.push_back(parseEXP());
	if(w[pos].type == VALUE){
		pos++;
		state->children.push_back(parseEXP());
	}
	return state;
}

lcParser::lcParser(lcLex& le):lexer(le),w( le.vectorWord){
	//le.vectorWord[0].content="scek na ilyr robos";
	std::cout<<"parser init .."<<std::endl;
	pos = 0;
	depth = 0;
}

lcParser::~lcParser(){
	std::cout<<"destruct parser obj .."<<std::endl;
	DelTree();
}

void lcParser::DelTree(){

}


AST* lcParser::linsert(AST* currentPos , AST* newNode){
	if(currentPos == NULL){
		std::cout<<"currentPos is NULL"<<std::endl;
	}
	NodeType t= newNode->ntype;
	while(true){

		if(currentPos->ntype == AST_EXP || currentPos->ntype ==AST_BRK || currentPos->ntype ==AST_PRM 
			|| currentPos->ntype < t ){

			if(currentPos->children.size() == 0){
				return appendchild(currentPos, newNode);
			}else if(currentPos->children.size()  >0){
				AST* tmp = currentPos->children.back();
				currentPos->children.back() = newNode;
				newNode->parent = currentPos;
				newNode->children.push_back(tmp);
				tmp->parent = newNode;
				return newNode;
			}else{
				std::cout<<"too more children"<<std::endl;
			}
		}else{
			currentPos = currentPos->parent;
		}
	}
}

AST* lcParser::checknext(AST* node, lcType t, int flr, int& flag){

	int a =isOp(t);
	if(a <=0 || t ==NOT){
		if(flr ==0){
			flag = 1;
			return node;
		}else{
			error("expect matching",&w[pos]);
		}
	}
	if(a == 2){
		AST* funnode = linsert(node, newAST(AST_FUN , &w[pos+1]));
		funnode->ntype = AST_RES;
		return funnode;
	}
	else{
		return node;
	}
}

NodeType lcParser::convert(lcType t){
	if(t == COMMA)
		return AST_COMMA;
	else if(t == OR)
		return AST_OR;
	else if(t == AND)
		return AST_AND;
	else if(t>=GT && t<=NGT)
		return AST_COMP;
	else if(t== ADD || t == SUB)
		return AST_ADD;
	else if(t == MUL || t == DIV)
		return AST_MUL;
	else if(t == DOT)
		return AST_OF;
	else
		std::cout<<"<<what is it ?>> "<<std::endl;
}

AST* lcParser::appendchild(AST* currentPos , AST* newNode){

	if(currentPos ==NULL){
		std::cout<<"null pointer.."<<std::endl;
	}
	newNode->parent = currentPos;
	currentPos->children.push_back(newNode);
	return currentPos->children.back();
}

AST* lcParser::matchback(NodeType t,AST* node){

	while( true){
		//std::cout<<"."<<std::endl;
		if(node->ntype == AST_EXP || node->ntype - t ==1 || t - node->ntype ==1){
			error("unmatched brace",&w[pos]);
		}else if(node->ntype == t){
			return node->parent;
		}else{
			node = node->parent;
		}
	}
}

AST* lcParser::newAST(NodeType n, LexWord* plex){
	AST* ptr = new AST;
	ptr->ntype = n;
	ptr->parent=NULL;
	ptr->children=std::vector<AST*>();
	if(plex == NULL){
		ptr->lexinfo = NULL;
	}else{
		ptr->lexinfo = new LexWord;
		ptr->lexinfo->content=plex->content;
		ptr->lexinfo->type = plex->type;
		ptr->lexinfo->row = plex->row;
		ptr->lexinfo->col = plex -> col;
	}
	//std::cout<<"new AST node: "<<safecontent(ptr)<<std::endl;
	return ptr;
}

int lcParser::isOp(lcType t){
	//std::cout<<AND<<" "<<t<<std::endl;
	if((t>=ID && t<=FLOAT ) || (t>=T && t<=NI)){
		return 0;
	}else if( (t>=GT && t<=DOT) || (t>= MUL && t <=NOT ) ){
		return 1;
	}else if(t == LPR || t == LBRK){
		return 2;
	}else if(t == RBRK || t == RPR){
		return 3;
	}else{
		return -1;
	}
}

AST* lcParser::doparseEXP(int flr,AST* node){

	while(pos < this->w.size()-1){
		int t = isOp(w[pos].type);
		if(flr == 0 && w[pos].type == FUNC ){
			pos++;
			node = appendchild(node, parseAnony());
			if(w[pos].type == COMMA){
				continue;
			}
			else{
				return node;
			}
		}

		switch (t){
			case 2: // lbr
				{	
					NodeType nt ;
					if(w[pos].type == LPR){
						nt = AST_PRM;
					}else{
						nt = AST_BRK;
					}
					node = appendchild(node, newAST(nt, &w[pos]));
					pos++; flr++;
					break;
				}
			case 3: // rbr
				{	

					NodeType nt ;
					if(w[pos].type == RPR){
						nt = AST_PRM;
					}else{
						nt = AST_BRK;
					}
					node = matchback(nt,node);
					flr--;
					int flag = 0;
					node = checknext(node , w[pos+1].type , flr , flag);
					pos++;
					if(flag  == 1)
						return node ;
					break;
				}
			case 0: // term
				{
					node = appendchild(node,newAST(AST_TERM,&w[pos]));
					int flag = 0;
					node = checknext(node , w[pos+1].type , flr , flag);
					pos++;
					if(flag  == 1)
						return node ;
					break;
				}
			case -1:
				{	
					if(flr==0){
						return node;
					}else{	
						error("unmatched",&w[pos]);
					}
					break;
				}
			case 1:
				{
					//uary
					if(w[pos].type != NOT && w[pos].type != SUB &&
						(pos == 0 || isOp(w[pos-1].type) == 1 || isOp(w[pos-1].type) == 2)){
						error("expecting uary",&w[pos]);
					}else if((w[pos].type == NOT || w[pos].type == SUB) &&
						(pos == 0 || isOp(w[pos-1].type) == 1 || isOp(w[pos-1].type) == 2)){
						node = appendchild(node, newAST(AST_UNARY , &w[pos]));
						pos++;
					}else{
						if(w[pos].type == DOT && w[pos+1].type !=ID){
							error("expecting identifier",&w[pos]);
						}
						int nextop = isOp(w[pos+1].type);
						if(nextop == 2 || nextop == -1){
							if(w[pos].type == COMMA &&w[pos+1].type == FUNC && flr == 0){
								node = linsert(node , newAST(convert(w[pos].type) , &w[pos]));
								pos++;
								break;
							}
							error("expecting identifier",&w[pos]);
						}

						node = linsert(node , newAST(convert(w[pos].type) , &w[pos]));
						pos++;
					}

				}
		}
	}
}

AST* lcParser::parseEXP(){

	lexer.PushWord(ENDFILE, "eof", w.back().row,w.back().col ); // add <eof> to token stream

	if(pos>=w.size() -1){
		std::cout<<"syntax error: line:"<<w[pos].row+1<<" col:"<<w[pos].col+1<<std::endl;
		exit(1);
	}

	//now there are at least one token to parse
	int flr =0;
	AST* exproot = newAST(AST_EXP,NULL );
	//debugAST(exproot);
	AST* tmp = exproot;
	AST* res = doparseEXP(flr,tmp);
	return exproot;
}
void lcParser::doParse(){
	//AST* root = parseEXP();
	//debugAST(root);
	AST* root = newAST(AST_CHUNK,NULL);
	root->children.push_back(parseBLOCK(0));
	debugAST(root);
}
void node2str(AST* n){
	std::string str;
	if(n ==NULL){
		std::cout<<"null pointer"<<std::endl;
	}
	switch(n->ntype){
		case AST_ADD: str ="_ADD";break;
		case AST_MUL: str="_MUL";break;
		case AST_BRK: str= "_BRK";break;
		case AST_COMMA:str="_COMMA";break;
		case AST_COMP:str="_COMP";break;
		case AST_EXP:str="_EXP";break;
		case AST_FUN:str="_FUN";break;
		case AST_OF:str="_DOT";break;
		case AST_PRM:str="_PRM";break;
		case AST_AND:str="_AND";break;
		case AST_OR:str="_OR";break;
		case AST_UNARY:str="_UARY";break;
		case AST_TERM:str="_TERM" ;break;
		case AST_RES:str="_RES" ;break;
		case AST_BLOCK:str="_BLOCK";break;
		case AST_CHUNK: str="_CHUNK";break;
		case AST_EVAL:str="_EVAL";break;
		case AST_FUNCTION:str = "_FUNCTION" ; break;
		case AST_IFELSE:str="IF_ELSE";break;
		case AST_LOOP:str="LOOP";break;
		case AST_STATE:str="STATE";break;
		default:str="unknown..";break;
	}
	std::string info;
	if(n->lexinfo == NULL){
		info ="NULL";
	}
	else if(n->lexinfo->content.length()==0){
		info= "NULL";
	}else{
		info = n->lexinfo->content;
	}
	std::cout<<"<Type> "<<str<<" <Token> "<<info<<" <children num> "<<n->children.size()<<std::endl;
}
void lcParser::debugAST(AST* root){
	std::cout<<"debug>>";
	while(true){
		char c;
		std::cin>>c;
		if(c == 'l'){
			std::cout<<"go left"<<std::endl;
			root = root->children[0];
		}
		else if(c == 'p'){
			std::cout<<"go up"<<std::endl;
			root = root->parent;
		}
		else if(c == 'r'){
			std::cout<<"go right"<<std::endl;
			root = root ->children[1];
		}
		else if(c== 'n'){
			node2str(root);
		}
		else if(c=='c'){
			for (int i=0;i<root->children.size();i++){
				node2str(root->children[i]);
			}
		}
		else if(c=='q'){
			std::cout<<"quit"<<std::endl;
			return ;
		}
		else if(c >= '1' && c<='9'){
			std::cout<<c<<" child"<<std::endl;
			root = root ->children[int(c)-int('1')];
		}
		else{
			std::cout<<"not a command."<<std::endl;
		}
	}
}