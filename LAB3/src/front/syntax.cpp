#include"front/syntax.h"

#include<iostream>

#include<vector>
#include<cassert>
using namespace std;
using frontend::Parser;

// #define DEBUG_PARSER
#define TODO assert(0 && "todo")
#define CUR_TOKEN_IS(tk_type) (token_stream[index].type == TokenType::tk_type)
#define PARSE_TOKEN(tk_type) root->children.push_back(parseTerm(root, TokenType::tk_type))
#define PARSE(name, type) auto name = new type(root); assert(parse##type(name)); root->children.push_back(name); 


Parser::Parser(const std::vector<frontend::Token>& tokens): index(0), token_stream(tokens) {}

Parser::~Parser() {}

frontend::CompUnit* Parser::get_abstract_syntax_tree(){
    CompUnit* root=new CompUnit;
    std::vector<std::string> symbol;//存文法符号
    std::vector<int> ops;//存操作
    int counts[100000]={0};
    symbol.push_back("CompUnit");
    int s=0;
    int p=100;
    while(symbol.size()!=0){
    	int select_op=0;
    
    	
    	if(symbol[symbol.size()-1]=="CompUnit"){
    		switch(token_stream[index].type){
    			case TokenType::CONSTTK: {
    				select_op=1;
					break;
				}
    			case TokenType::VOIDTK: {
    				select_op=2;
					break;
				}
    			case TokenType::INTTK:case TokenType::FLOATTK: {
							if(index+2<=token_stream.size()-1)
								if(token_stream[index+2].value=="(")
									select_op=2;
								else select_op=1;
							else select_op=1;
							break;
				};
			}
		}
		else if(symbol[symbol.size()-1]=="Decl"){
    		switch(token_stream[index].type){
    			case TokenType::CONSTTK: {
    				select_op=3;
					break;
				}
    			case TokenType::INTTK:case TokenType::FLOATTK:select_op=4;
			}
		}
		else if(symbol[symbol.size()-1]=="ConstDecl"){
    		switch(token_stream[index].type){
    			case TokenType::CONSTTK: select_op=5;
    			
			}
		}
    	else if(symbol[symbol.size()-1]=="BType"){
    		switch(token_stream[index].type){
    			case TokenType::INTTK:{
    				select_op=6;
					break;
				}
				case TokenType::FLOATTK:select_op=7;
			}
		}
    	else if(symbol[symbol.size()-1]=="ConstDef"){
    		switch(token_stream[index].type){
    			case TokenType::IDENFR:select_op=8;
			}
		}
    	else if(symbol[symbol.size()-1]=="ConstInitVal"){
    		switch(token_stream[index].type){
    			    case TokenType::IDENFR:case TokenType::INTLTR:case  TokenType::FLOATLTR: case TokenType::PLUS:case TokenType::MINU: case TokenType::LPARENT: case TokenType::NOT:{
    				select_op=9;
					break;
				}
    			    case TokenType::LBRACE:select_op=10;
			}
		}
    	else if(symbol[symbol.size()-1]=="VarDecl"){
    		switch(token_stream[index].type){
    			case TokenType::INTTK:case TokenType::FLOATTK:select_op=11;
			}
		}
    	else if(symbol[symbol.size()-1]=="VarDef"){
    		switch(token_stream[index].type){
    			case TokenType::IDENFR:select_op=12;
			}
		}
    	else if(symbol[symbol.size()-1]=="InitVal"){
    		switch(token_stream[index].type){
    			    case TokenType::IDENFR:case TokenType::INTLTR:case  TokenType::FLOATLTR: case TokenType::PLUS:case TokenType::MINU: case TokenType::LPARENT: case TokenType::NOT:{
    				select_op=13;
					break;
				}
    			    case TokenType::LBRACE:select_op=14;
			}
		}
    	else if(symbol[symbol.size()-1]=="FuncDef"){
    		switch(token_stream[index].type){
    			case TokenType::VOIDTK:case TokenType::INTTK:case TokenType::FLOATTK:select_op=15;
			}
		}
    	else if(symbol[symbol.size()-1]=="FuncType"){
    		switch(token_stream[index].type){
    			case TokenType::VOIDTK:{
    				select_op=16;
					break;
				}
				case TokenType::INTTK:{
					select_op=17;
					break;
				}
				case TokenType::FLOATTK:select_op=18;
			}
		}
    	else if(symbol[symbol.size()-1]=="FuncFParam"){
    		switch(token_stream[index].type){
    			case TokenType::INTTK:case TokenType::FLOATTK:select_op=19;
			}
		}
    	else if(symbol[symbol.size()-1]=="FuncFParams"){
    		switch(token_stream[index].type){
    			case TokenType::INTTK:case TokenType::FLOATTK:select_op=20;
			}
		}
    	else if(symbol[symbol.size()-1]=="Block"){
    		switch(token_stream[index].type){
    			case TokenType::LBRACE:select_op=21;
			}
		}
    	else if(symbol[symbol.size()-1]=="BlockItem"){
    		switch(token_stream[index].type){
    			case TokenType::IDENFR:case TokenType::INTLTR:case  TokenType::FLOATLTR: case TokenType::IFTK:case TokenType::WHILETK: case TokenType::CONTINUETK: 
    case TokenType::BREAKTK: 
    case TokenType::RETURNTK: 
    case TokenType::PLUS: 
    case TokenType::MINU:case TokenType::SEMICN:case TokenType::LPARENT:case TokenType::LBRACE:case TokenType::NOT:{
    				select_op=23;
					break;
				}
    case TokenType::VOIDTK:case TokenType::INTTK:case TokenType::FLOATTK:case TokenType::CONSTTK:select_op=22;
			}
		}
    	else if(symbol[symbol.size()-1]=="Stmt"){
    		switch(token_stream[index].type){
    			case TokenType::LBRACE:{
    				select_op=25;
					break;
				}
    			case TokenType::IFTK:{
    				select_op=26;
					break;
				}
    			case TokenType::WHILETK: {
    				select_op=27;
					break;
				}
				case TokenType::CONTINUETK: {
    				select_op=29;
					break;
				}
    			case TokenType::BREAKTK: {
    				select_op=28;
					break;
				}
    			case TokenType::RETURNTK: {
    				select_op=30;
					break;
				}
    			case TokenType::SEMICN:{
    				select_op=31;
					break;
				}
    			case TokenType::INTLTR:case  TokenType::FLOATLTR:case TokenType::PLUS: case TokenType::MINU:case TokenType::LPARENT:case TokenType::NOT:select_op=32;
    			case TokenType::IDENFR:{
    				if(token_stream[index+1].value=="(")
    					select_op=32;
    				else if(token_stream[index+1].value=="[")
    				select_op=24;
    				else{
    					for(int i=index+1;i<token_stream.size()-1;i++){
    						if(token_stream[i].value=="="){
							
    						select_op=24;
    						break;}
    						if(token_stream[i].value==";"){
							
    						select_op=32;
    						break;}
						}
					}
					break;
				}
			}
		}
    	else if(symbol[symbol.size()-1]=="Exp"){
    		select_op=33;
		}
    	else if(symbol[symbol.size()-1]=="Cond"){
    		select_op=34;
		}
		else if(symbol[symbol.size()-1]=="LVal"){
    		select_op=35;
		}
    	else if(symbol[symbol.size()-1]=="FuncRParams"){
    		select_op=47;
		}
		else if(symbol[symbol.size()-1]=="MulExp"){
    		select_op=48;
		}
		else if(symbol[symbol.size()-1]=="AddExp"){
    		select_op=49;
		}
		else if(symbol[symbol.size()-1]=="RelExp"){
    		select_op=50;
		}
		else if(symbol[symbol.size()-1]=="EqExp"){
    		select_op=51;
		}
		else if(symbol[symbol.size()-1]=="LAndExp"){
    		select_op=52;
		}
		else if(symbol[symbol.size()-1]=="LOrExp"){
    		select_op=53;
		}
		else if(symbol[symbol.size()-1]=="ConstExp"){
    		select_op=54;
		}
		else if(symbol[symbol.size()-1]=="Number"){
    		switch(token_stream[index].type){
    			case TokenType::INTLTR:{
    				select_op=36;
					break;
				}
				case  TokenType::FLOATLTR:select_op=37;
			}
		}
		else if(symbol[symbol.size()-1]=="PrimaryExp"){
    		switch(token_stream[index].type){
    			case TokenType::IDENFR:{
    				select_op=39;
					break;
				}
				case TokenType::INTLTR:case  TokenType::FLOATLTR:{
    				select_op=40;
					break;
				}
				case TokenType::LPARENT:select_op=38;
			}
		}
		else if(symbol[symbol.size()-1]=="UnaryOp"){
    		switch(token_stream[index].type){
    			    case TokenType::PLUS: {
    				select_op=44;
					break;
				}
    				case TokenType::MINU:{
    				select_op=45;
					break;
				}
    	    		case TokenType::NOT: select_op=46;
			}
		}
    	else if(symbol[symbol.size()-1]=="UnaryExp"){
    		switch(token_stream[index].type){
    			   case TokenType::INTLTR:case  TokenType::FLOATLTR:case TokenType::LPARENT:{
    				select_op=41;
					break;
				}
    			   case TokenType::PLUS: case TokenType::MINU:case TokenType::NOT: {
    				select_op=43;
					break;
				}
    			   case TokenType::IDENFR:{
    			   	if(index+1<=token_stream.size()-1)
    			   			if(token_stream[index+1].value=="(")
    			   				select_op=42;
    			   			else
    			   				select_op=41;
    			   	else
    			   			select_op=41;
					break;
				   }
			}
		}
    	else if(symbol[symbol.size()-1].substr(0,3)=="k1a"||symbol[symbol.size()-1].substr(0,3)=="k2a"){
    		if(index<token_stream.size()-1){
    			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
			symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
    			symbol.push_back("CompUnit");}
    		else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
    			symbol.pop_back();}
		}
		else if(symbol[symbol.size()-1].substr(0,3)=="w5a"){
			if(token_stream[index].value==","){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				symbol.push_back("ConstDef");
				symbol.push_back(",");
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,3)=="w8a"){
			if(token_stream[index].value=="["){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				symbol.push_back("]");
				symbol.push_back("ConstExp");
				symbol.push_back("[");
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k10a"){
			
			if(token_stream[index].value=="}"){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
			symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				string all_="w10";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
				symbol.push_back(all_);
				symbol.push_back("ConstInitVal");
				
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w10a"){
			if(token_stream[index].value==","){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
			symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				symbol.push_back("ConstInitVal");
				symbol.push_back(",");
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w11a"){
			if(token_stream[index].value==","){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				symbol.push_back("VarDef");
				symbol.push_back(",");
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w12a"){
			if(token_stream[index].value=="["){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				symbol.push_back("]");
				symbol.push_back("ConstExp");
				symbol.push_back("[");
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k14a"){
			if(token_stream[index].value=="}"){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			}
			else{
			
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				string all_="w14";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
				symbol.push_back(all_);
				symbol.push_back("InitVal");
				
				
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w14a"){
			if(token_stream[index].value==","){
			
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				symbol.push_back("InitVal");
				symbol.push_back(",");
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
				
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k12a"){
			if(token_stream[index].value=="="){
			
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
			symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				symbol.push_back("InitVal");
				symbol.push_back("=");
			
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k15a"){
			if(token_stream[index].value==")"){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
				
			
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("FuncFParams");
		}}
			else if(symbol[symbol.size()-1].substr(0,4)=="k19a"){
			if(token_stream[index].value=="["){
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				string all_="w19";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
				symbol.push_back(all_);
				symbol.push_back("]");
				symbol.push_back("[");
				
			
			
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w19a"){
			if(token_stream[index].value=="["){
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				
				symbol.push_back("]");
				symbol.push_back("Exp");
				symbol.push_back("[");
				
				
			
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w20a"){
			if(token_stream[index].value==","){
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				
				symbol.push_back("FuncFParam");
				symbol.push_back(",");
				
				
				
			
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w21a"){
			if(token_stream[index].value=="}"){
					
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				
				symbol.push_back("BlockItem");
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k26a"){
			if(token_stream[index].value=="else"){
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				
				symbol.push_back("Stmt");
				symbol.push_back("else");
				
			
			}
			else{
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k30a"){
			if(token_stream[index].value==";"){
				
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				
				symbol.push_back("Exp");
			
				
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k32a"){
			if(token_stream[index].value==";"){
				
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				
				symbol.push_back("Exp");
			
				
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w35a"){
			if(token_stream[index].value=="["){
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("]");
				symbol.push_back("Exp");
				symbol.push_back("[");
				
			
			}
			else{
					
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
				
			
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w42a"){
			if(token_stream[index].value==")"){
				
				int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("FuncRParams");
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w47a"){
			if(token_stream[index].value==","){
				
			
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("Exp");
				symbol.push_back(",");
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w48a"){
			if(token_stream[index].value=="*"||token_stream[index].value=="%"||token_stream[index].value=="/"){
				
			
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("UnaryExp");
				symbol.push_back("token_stream[index].value");
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w49a"){
			if(token_stream[index].value=="+"||token_stream[index].value=="-"){
				
			
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("MulExp");
				symbol.push_back("token_stream[index].value");
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w50a"){
			if(token_stream[index].value=="<"||token_stream[index].value==">"||token_stream[index].value=="<="||token_stream[index].value==">="){
				
			
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("AddExp");
				symbol.push_back("token_stream[index].value");
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="w51a"){
			if(token_stream[index].value=="=="||token_stream[index].value=="!="){
				
			
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("RelExp");
				symbol.push_back(token_stream[index].value);
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k52a"){
			if(token_stream[index].value=="&&"){
				
			
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("LAndExp");
				symbol.push_back("token_stream[index].value");
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else if(symbol[symbol.size()-1].substr(0,4)=="k53a"){
			if(token_stream[index].value=="||"){
				
			
			int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
				symbol[symbol.size()-1]=symbol[symbol.size()-1].substr(0,i+1)+std::to_string(std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1))+1);
				
				symbol.push_back("LOrExp");
				symbol.push_back("token_stream[index].value");
			}
			else{
					int i=0;
    			while(symbol[symbol.size()-1][i]!='c')
    				i++;
    			int j=0;
    			while(symbol[symbol.size()-1][j]!='a')
    				j++;
    			counts[std::stoi(symbol[symbol.size()-1].substr(j+1,i-j-1))]=std::stoi(symbol[symbol.size()-1].substr(i+1,symbol[symbol.size()-1].size()-i-1));
				symbol.pop_back();
			}
		}
		else{
			index++;
			symbol.pop_back();
		}
    	//
    	//
    	//
    	//
    	//
    	//
    	//

    	
    	if(select_op)
    		ops.push_back(select_op);
    	if(select_op==1){
    		string all_="k1";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("Decl");
    		
		}
		if(select_op==2){
			string all_="k2";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("FuncDef");
    		
		}
		if(select_op==3){
    		symbol.pop_back();
    		symbol.push_back("ConstDecl");
    		
    		
		}
		if(select_op==4){
    		symbol.pop_back();
    		symbol.push_back("VarDecl");
    		
		}
		if(select_op==5){
			string all_="w5";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(";");
    		symbol.push_back(all_);
    		symbol.push_back("ConstDef");
    		symbol.push_back("BType");
    		symbol.push_back("const");
		}
		if(select_op==6){
    		symbol.pop_back();
    		symbol.push_back("int");
    		
		}
		if(select_op==7){
    		symbol.pop_back();
    		symbol.push_back("float");
    		
		}
		if(select_op==8){
			string all_="w8";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back("ConstInitVal");
    		symbol.push_back("=");
    		symbol.push_back(all_);
    		symbol.push_back("ident");
    		
		}
		if(select_op==9){
    		symbol.pop_back();
    		symbol.push_back("ConstExp");
    	
    		
		}
		if(select_op==10){
			string all_="k10";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back("}");
    		symbol.push_back(all_);
    		symbol.push_back("{");
    		
		}if(select_op==11){
			string all_="w11";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(";");
    		symbol.push_back(all_);
    		symbol.push_back("VarDef");
    		symbol.push_back("BType");
    		
		}if(select_op==12){
			string all_="w12";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		string all_1="k12";
    		all_1+="a";
    		all_1+=std::to_string(s);
    		all_1+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_1);
    		symbol.push_back(all_);
    		symbol.push_back("ident");
    		
		}if(select_op==13){
    		symbol.pop_back();
    		symbol.push_back("Exp");
    		
		}if(select_op==14){
			string all_="k14";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back("}");
    		symbol.push_back(all_);
    		symbol.push_back("{");
		}if(select_op==15){
			string all_="k15";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back("Block");
    		symbol.push_back(")");
    		symbol.push_back(all_);
    		symbol.push_back("(");
    		symbol.push_back("Ident");
    		symbol.push_back("FuncType");
    		
		}if(select_op==16){
    		symbol.pop_back();
    		symbol.push_back("void");
   
    		
		}if(select_op==17){
    		symbol.pop_back();
    		symbol.push_back("int");
  
    		
		}if(select_op==18){
    		symbol.pop_back();
    		symbol.push_back("float");
    
    		
		}if(select_op==19){
			string all_="k19";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("ident");
    		symbol.push_back("BType");
    		
		}if(select_op==20){
			string all_="w20";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("FuncFParam");
    		
		}if(select_op==21){
			string all_="w21";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back("}");
    		symbol.push_back(all_);
    		symbol.push_back("{");
		}if(select_op==22){
    		symbol.pop_back();
    		
    		symbol.push_back("Decl");
    		
		}if(select_op==23){
    		symbol.pop_back();

    		symbol.push_back("Stmt");
    		
		}if(select_op==24){
    		symbol.pop_back();
    		symbol.push_back(";");
    		symbol.push_back("Exp");
    		symbol.push_back("=");
    		symbol.push_back("LVal");
		}if(select_op==25){
    		symbol.pop_back();
    		symbol.push_back("Block");
    		
		}if(select_op==26){
			string all_="k26";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("Stmt");
    		symbol.push_back(")");
    		symbol.push_back("Cond");
    		symbol.push_back("(");
    		symbol.push_back("if");
		}if(select_op==27){
    		symbol.pop_back();
    		symbol.push_back("Stmt");
    		symbol.push_back(")");
    		symbol.push_back("Cond");
    		symbol.push_back("(");
    		symbol.push_back("while");
    		
		}if(select_op==28){
    		symbol.pop_back();
    		symbol.push_back(";");
    		symbol.push_back("break");
    		
		}if(select_op==29){
    		symbol.pop_back();
    		symbol.push_back(";");
    		symbol.push_back("continue");
    		
		}if(select_op==30){
			string all_="k30";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(";");
    		symbol.push_back(all_);
    		symbol.push_back("return");
    		
		}if(select_op==31){
    		symbol.pop_back();
    		symbol.push_back(";");
    		
    		
		}if(select_op==32){
			string all_="k32";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(";");
    		symbol.push_back(all_);
    		
		}if(select_op==33){
    		symbol.pop_back();
    		symbol.push_back("AddExp");
    		
		}if(select_op==34){
    		symbol.pop_back();
    		symbol.push_back("LOrExp");
    	
    		
		}if(select_op==35){
			string all_="w35";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("ident");
    		
		}if(select_op==36){
    		symbol.pop_back();
    		symbol.push_back("IntConst");
    	
    		
		}if(select_op==37){
    		symbol.pop_back();
    		symbol.push_back("floatConst");

    		
		}if(select_op==38){
    		symbol.pop_back();
    		symbol.push_back(")");
    		symbol.push_back("Exp");
    		symbol.push_back("(");
		}if(select_op==39){
    		symbol.pop_back();
    		symbol.push_back("LVal");
    	
    		
		}if(select_op==40){
    		symbol.pop_back();
    		symbol.push_back("Number");
    		
		}if(select_op==41){
    		symbol.pop_back();
    		symbol.push_back("PrimaryExp");
    		
    		
		}if(select_op==42){
			string all_="w42";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(")");
    		symbol.push_back(all_);
    		symbol.push_back("(");
    		symbol.push_back("ident");
		}if(select_op==43){
    		symbol.pop_back();
    		symbol.push_back("UnaryExp");
    		symbol.push_back("UnaryOp");
    		
		}if(select_op==44){
    		symbol.pop_back();
    		symbol.push_back("+");

    		
		}if(select_op==45){
    		symbol.pop_back();
    		symbol.push_back("-");
    
    		
		}if(select_op==46){
    		symbol.pop_back();
    		symbol.push_back("!");
    
    		
		}if(select_op==47){
			string all_="w47";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("Exp");
    		
		}if(select_op==48){
			string all_="w48";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("UnaryExp");
    		
		}if(select_op==49){
			string all_="w49";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("MulExp");
    		
		}if(select_op==50){
			string all_="w50";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("AddExp");
    		
		}if(select_op==51){
			string all_="w51";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("RelExp");
    		
		}if(select_op==52){
			string all_="k52";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("EqExp");
    		
		}if(select_op==53){
			string all_="k53";
    		all_+="a";
    		all_+=std::to_string(s);
    		all_+="c0";
    		s++;
    		symbol.pop_back();
    		symbol.push_back(all_);
    		symbol.push_back("LAndExp");
    		
		}if(select_op==54){
    		symbol.pop_back();
    		symbol.push_back("AddExp");
    		
		}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
	}
	

	std::vector<AstNode*> tree;
	std::vector<Term*> term;
	tree.push_back(root);
	int t=0;
	int o=0;
	int c=0;
	Token sl;
	while(tree.size()!=0){
		
		int num=0;
		if(tree[tree.size()-1]->type==NodeType::TERMINAL){
			term[term.size()-1]->token=token_stream[t++];
			tree.pop_back();
			term.erase(term.begin()+term.size()-1);
		}
		else{
			if(ops[o]==1){
				while(counts[c]--){
					CompUnit* c1=new CompUnit(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
					
				}
				c++;
				Decl* c1=new Decl(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==2){
				while(counts[c]--){
					CompUnit* c1=new CompUnit(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
					
				}
				c++;
				FuncDef* c1=new FuncDef(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==3){
				
				ConstDecl* c1=new ConstDecl(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==4){
				
				VarDecl* c1=new VarDecl(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==5){
				
				Term* c1=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c2);
					term.push_back(c1);
					num++;
				while(counts[c]--){
					ConstDef* c1=new ConstDef(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
					
					
				}
				c++;
				ConstDef* c10=new ConstDef(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c10);
					tree.push_back(c10);
					num++;
				BType* c11=new BType(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c11);
					tree.push_back(c11);
					num++;
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==6){
				Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==7){
				Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==8){
				ConstInitVal* c10=new ConstInitVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c10);
					tree.push_back(c10);
					num++;
				Term* c1=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c2);
					term.push_back(c1);
					num++;
				while(counts[c]--){
					
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
					ConstExp* c1=new ConstExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
				}
				c++;
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==9){
				
				ConstExp* c1=new ConstExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==10){
				
				Term* c1=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c2);
					term.push_back(c1);
					num++;
					int flag=0;
					if(counts[c]==0)
					flag=1;
				while(counts[c]--){
					while(counts[c+1]--){
					
					
					ConstInitVal* c1=new ConstInitVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;}
					ConstInitVal* c11=new ConstInitVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c11);
					tree.push_back(c11);
					num++;
				}
				if(flag)
				c+=1;
				else
				c+=2;
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==11){
				
				Term* c1=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c2);
					term.push_back(c1);
					num++;
				while(counts[c]--){
					VarDef* c1=new VarDef(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
					
					
				}
				c++;
				VarDef* c10=new VarDef(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c10);
					tree.push_back(c10);
					num++;
				BType* c11=new BType(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c11);
					tree.push_back(c11);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==12){
				
				int u=c+1;
				while(counts[u]--){
					InitVal* c1=new InitVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
					
					
				}
				while(counts[c]--){
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
					ConstExp* c1=new ConstExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
					
					
				}
				c+=2;
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==13){
				
				
				Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==14){
				
				Term* c1=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c2);
					term.push_back(c1);
					num++;
					int flag=0;
					if(counts[c]==0)
					flag=1;
				while(counts[c]--){
					while(counts[c+1]--){
					
					
					InitVal* c1=new InitVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;}
					InitVal* c11=new InitVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c11);
					tree.push_back(c11);
					num++;
				}
				if(flag)
				c+=1;
				else
				c+=2;
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==15){
				Block* c11=new Block(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c11);
					tree.push_back(c11);
					num++;
					
				Term* c1=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c2);
					term.push_back(c1);
					num++;
				while(counts[c]--){
					FuncFParams* c1=new FuncFParams(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
					
					
				}
				c++;
				Term* c15=new Term(sl,tree[tree.size()-1-num]);
				Term* c25=new Term(sl,c25);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c15);
					tree.push_back(c25);
					term.push_back(c15);
					num++;
				Term* c12=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c12);
					tree.push_back(c22);
					term.push_back(c12);
					num++;
				FuncType* c17=new FuncType(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c17);
					tree.push_back(c17);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==16){
				
				
				
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==17){
				
				
				
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==18){
				
				
				
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==19){
				int flag=0;
				if(counts[c]==0)
				flag=1;
				
				while(counts[c]--){
					while(counts[c+1]--){
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
					
					Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;}
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
					Term* c331=new Term(sl,tree[tree.size()-1-num]);
				Term* c231=new Term(sl,c231);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c331);
					tree.push_back(c231);
					term.push_back(c331);
					num++;
					
				}
				if(flag)
				c+=1;
				else
				c+=2;
					Term* c32=new Term(sl,tree[tree.size()-1-num]);
				Term* c22=new Term(sl,c22);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c32);
					tree.push_back(c22);
					term.push_back(c32);
					num++;
						BType* c11=new BType(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c11);
					tree.push_back(c11);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==20){
				
				
				while(counts[c]--){
					
				
					
					FuncFParam* c1=new FuncFParam(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
				
				}
				c++;
				FuncFParam* c1=new FuncFParam(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==21){
				
				Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
				while(counts[c]--){
	
					BlockItem* c1=new BlockItem(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				
				}
				c++;
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==22){
				
				
				Decl* c1=new Decl(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==23){
				
				
				Stmt* c1=new Stmt(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==24){
				
				Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
				
	
					Exp* c14=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c14);
					tree.push_back(c14);
					num++;
					
				
				
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				LVal* c1=new LVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==25){
				
				
				Block* c1=new Block(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==26){
				while(counts[c]--){
	
					Stmt* c1=new Stmt(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
				
				}
				c++;
				Stmt* c14=new Stmt(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c14);
					tree.push_back(c14);
					num++;
				Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;
				Cond* c1=new Cond(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==27){
			
	
					Stmt* c14=new Stmt(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c14);
					tree.push_back(c14);
					num++;
					Term* c33=new Term(sl,tree[tree.size()-1-num]);
				Term* c23=new Term(sl,c23);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c33);
					tree.push_back(c23);
					term.push_back(c33);
					num++;

				Cond* c1=new Cond(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==28){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==29){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==30){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				while(counts[c]--){
	
					Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				
				}
				c++;
				Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==31){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==32){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				while(counts[c]--){
	
					Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				
				}
				c++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==33){
				
				
				AddExp* c1=new AddExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==34){
				
				
				LOrExp* c1=new LOrExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==35){
				while(counts[c]--){
					Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
	
					Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
	c++;
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
			
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==36){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==37){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==38){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==39){
	
				
				LVal* c1=new LVal(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==40){
	
			
				Number* c1=new Number(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			
			
			
			if(ops[o]==41){
				
				
				PrimaryExp* c1=new PrimaryExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==42){
				Term* c31=new Term(sl,tree[tree.size()-1-num]);
				Term* c21=new Term(sl,c21);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c31);
					tree.push_back(c21);
					term.push_back(c31);
					num++;
				while(counts[c]--){
				
	
					FuncRParams* c1=new FuncRParams(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					
				
				}
	c++;
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
			Term* c34=new Term(sl,tree[tree.size()-1-num]);
				Term* c24=new Term(sl,c24);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c34);
					tree.push_back(c24);
					term.push_back(c34);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==43){
				
	
					UnaryExp* c14=new UnaryExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c14);
					tree.push_back(c14);
					num++;
					UnaryOp* c1=new UnaryOp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
			
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==44){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==45){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==46){
	
				Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==47){
				
				while(counts[c]--){
				
	
					Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
	c++;
				Exp* c1=new Exp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==48){
				
				while(counts[c]--){
				
	
					UnaryExp* c1=new UnaryExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
	c++;
				UnaryExp* c1=new UnaryExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==49){
				
				while(counts[c]--){
				
	
					MulExp* c1=new MulExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
	c++;
				MulExp* c1=new MulExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==50){
				
				while(counts[c]--){
				
	
					AddExp* c1=new AddExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
	c++;
				AddExp* c1=new AddExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==51){
				
				while(counts[c]--){
				
	
					RelExp* c1=new RelExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
	c++;
				RelExp* c1=new RelExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==52){
				
				while(counts[c]--){
				
	
					LAndExp* c1=new LAndExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
	c++;
				EqExp* c1=new EqExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==53){
				
				while(counts[c]--){
				
	
					LOrExp* c1=new LOrExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
					Term* c3=new Term(sl,tree[tree.size()-1-num]);
				Term* c2=new Term(sl,c2);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c3);
					tree.push_back(c2);
					term.push_back(c3);
					num++;
				
				}
				c++;
				LAndExp* c1=new LAndExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			if(ops[o]==54){
			
	
				AddExp* c1=new AddExp(tree[tree.size()-1-num]);
					tree[tree.size()-1-num]->children.insert(tree[tree.size()-1-num]->children.begin(),c1);
					tree.push_back(c1);
					num++;
				
				tree.erase(tree.begin()+tree.size()-num-1);
			}
			
			o++;
		
			
		}
		
	}
    return root;
    
    
    
    
    
    
}

void Parser::log(AstNode* node){
#ifdef DEBUG_PARSER
        std::cout << "in parse" << toString(node->type) << ", cur_token_type::" << toString(token_stream[index].type) << ", token_val::" << token_stream[index].value << '\n';
#endif
}

