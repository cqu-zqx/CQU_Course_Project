#include"front/lexical.h"

#include<map>
#include<cassert>
#include<string>
using namespace std;
#define TODO assert(0 && "todo")

// #define DEBUG_DFA
// #define DEBUG_SCANNER
static frontend::TokenType get_type_op(std::string s){
	switch (s[0]) {
    
    case '+':return frontend::TokenType::PLUS;
    case '-':return frontend::TokenType::MINU;
    case '/':return frontend::TokenType::DIV;
    case '*':return frontend::TokenType::MULT;
    case '%':return frontend::TokenType::MOD;
    case '<':return frontend::TokenType::LSS;
    case '>':return frontend::TokenType::GTR;
    case ':':return frontend::TokenType::COLON;
    case '=':return frontend::TokenType::ASSIGN;
    case ';':return frontend::TokenType::SEMICN;
    case ',':return frontend::TokenType::COMMA;
    case '(':return frontend::TokenType::LPARENT;
    case ')':return frontend::TokenType::RPARENT;
    case '[':return frontend::TokenType::LBRACK;
    case ']':return frontend::TokenType::RBRACK;
    case '{':return frontend::TokenType::LBRACE;
    case '}':return frontend::TokenType::RBRACE;
    case '!':return frontend::TokenType::NOT;
   
   
    default:
        assert(0 && "invalid token type");
        break;
    }
    return frontend::TokenType::IDENFR;
}

frontend::TokenType get_type_op2(std::string s){
	switch (s[0]) {
    
    case '<':return frontend::TokenType::LEQ;
    case '>':return frontend::TokenType::GEQ;
    case '!':return frontend::TokenType::NEQ;
    case '&':return frontend::TokenType::AND;
    case '|':return frontend::TokenType::OR;
    case '=':return frontend::TokenType::EQL;

   
   
    default:
        assert(0 && "invalid token type");
        break;
    }
   
}
frontend::TokenType get_type_idet(std::string s){

    
    if(s== "const")return frontend::TokenType::CONSTTK;
    else if(s=="void")return frontend::TokenType::VOIDTK;
    else if(s=="int")return frontend::TokenType::INTTK;
    else if(s=="float")return frontend::TokenType::FLOATTK;
    else if(s=="if")return frontend::TokenType::IFTK;
    else if(s=="else")return frontend::TokenType::ELSETK;
    else if(s=="while")return frontend::TokenType::WHILETK;
    else if(s=="continue")return frontend::TokenType::CONTINUETK;
    else if(s=="break")return frontend::TokenType::BREAKTK;
    else if(s=="return")return frontend::TokenType::RETURNTK;
	else return frontend::TokenType::IDENFR;
   
   
    }
   

std::string frontend::toString(State s) {
    switch (s) {
    case State::Empty: return "Empty";
    case State::Ident: return "Ident";
    case State::IntLiteral: return "IntLiteral";
    case State::FloatLiteral: return "FloatLiteral";
    case State::op: return "op";
    default:
        assert(0 && "invalid State");
    }
    return "";
}

std::set<std::string> frontend::keywords= {
    "const", "int", "float", "if", "else", "while", "continue", "break", "return", "void"
};

frontend::DFA::DFA(): cur_state(frontend::State::Empty), cur_str() {}

int flag=0;
int flag1=0;
bool frontend::DFA::next(char input, Token& buf) {
#ifdef DEBUG_DFA
#include<iostream>
    std::cout << "in state [" << toString(cur_state) << "], input = \'" << input << "\', str = " << cur_str << "\t";
#endif

std::cout<<"state::"<<toString(cur_state)<<std::endl;
std::cout<<"str::"<<cur_str<<std::endl;
std::cout<<"----------"<<std::endl;
std::cout<<flag<<"*****************************"<<std::endl;
switch(cur_state)
{

 case State::Empty:{
 if(flag==0&&flag1==0){
                   if(input==' '||input=='\n'||input=='\t'){
                                    
                                     return false;}
                   else if(input=='_'||input>='A'&&input<='Z'||input>='a'&&input<='z'){
                                     cur_str+=input;
                                     cur_state=State::Ident;
                                     return false;}
                     else if(input=='.'){
                                     cur_str+=input;
                                     cur_state=State::FloatLiteral;
                                     return false;}
                    else if(input>='0'&&input<='9'){
                    				 cur_str+=input;
                                     cur_state=State::IntLiteral;
                                     return false;
					}
					else{
						cur_str+=input;
                                     cur_state=State::op;
                                     return false;
					}}
else if(flag==1){
if(input=='*'){
cur_str+=input;return false;
}
else if(input=='/'){
if(cur_str!=""){
cur_str="";
flag=0;}
else{
cur_str="";
}
return false;
}
else {cur_str="";return false;}
}

else {
if(input=='\n'){
flag1=0;
}
return false;
}
}
 case State::IntLiteral:{
                   if(input==' '||input=='\n'||input=='\t'){
                                     
                                     buf.value=cur_str;
                                     buf.type=TokenType::INTLTR;
                                     cur_str="";
                                     cur_state=State::Empty;
                                    
                                     return true;}
                   else if(input>='A'&&input<='Z'||input>='a'&&input<='z'||input>='0'&&input<='9'){
                                    cur_str+=input;
                                    return false;}
					else if(input=='.'){
						cur_str+=input;
						cur_state=State::FloatLiteral;
                                    return false;
					}
					
					else if(input=='_'){
						buf.value=cur_str;
                        buf.type=TokenType::INTLTR;
                        cur_str="";
						cur_str+=input;
						cur_state=State::Ident;
						
                                    return true;
					}
					else{
						buf.value=cur_str;
                        buf.type=TokenType::INTLTR;
                        cur_str="";
						cur_str+=input;
						cur_state=State::op;
						
                                    return true;
					}
									
									
									}
									//////
case State::FloatLiteral:{
                    if(input==' '||input=='\n'||input=='\t'){
                                     
                        buf.value=cur_str;
                        buf.type=TokenType::FLOATLTR;
                        cur_str="";
                        cur_state=State::Empty;
                      
                        return true;}
                   
					
					else if(input>='0'&&input<='9'){
						
						cur_str+=input;
					
                                    return false;
					}
					else{
						buf.value=cur_str;
                        buf.type=TokenType::FLOATLTR;
                        cur_str="";
						cur_str+=input;
						cur_state=State::op;
						
                                    return true;
					}
									
}
case State::op:{
                    if(input==' '||input=='\n'||input=='\t'){
                                     
                        buf.value=cur_str;
                        buf.type=get_type_op(buf.value);
                        cur_str="";
                        cur_state=State::Empty;
                      
                        return true;}
                   
					
					else if(input=='_'||input>='A'&&input<='Z'||input>='a'&&input<='z'){
						buf.value=cur_str;
                        buf.type=get_type_op(buf.value);
                        
                        cur_str="";
						cur_str+=input;
						cur_state=State::Ident;
						
                                    return true;
					}
					else if(input>='0'&&input<='9'){
						buf.value=cur_str;
                        buf.type=get_type_op(buf.value);
                        
                        cur_str="";
						cur_str+=input;
						cur_state=State::IntLiteral;
					
                                    return true;
					}
					else if((cur_str+input)=="<="||(cur_str+input)==">="||(cur_str+input)=="=="||(cur_str+input)=="&&"||(cur_str+input)=="||"||(cur_str+input)=="!="){
						cur_str+=input;
						buf.value=cur_str;
                        buf.type=get_type_op2(cur_str);
                        cur_str="";
						
						cur_state=State::Empty;
					
                                    return true;
					}
					else if(input=='.'){
						buf.value=cur_str;
                        buf.type=get_type_op(buf.value);
                        
                        cur_str="";
						cur_str+=input;
						cur_state=State::FloatLiteral;
						
                                    return true;
					}
					else if((cur_str+input)=="/*"){
					
					cur_str="";
					cur_state=State::Empty;
					flag=1;return false;}
					else if((cur_str+input)=="//"){
					cur_str="";
					cur_state=State::Empty;
					flag1=1;return false;
					}
					else{
						
						buf.value=cur_str;
                        buf.type=get_type_op(cur_str);
                        cur_str="";
						cur_str+=input;
						cur_state=State::op;
						
                                    return true;
					}
									
}
case State::Ident:{
                    if(input==' '||input=='\n'||input=='\t'){
                             
                        buf.value=cur_str;
                        buf.type=get_type_idet(buf.value);
                        cur_str="";
                        cur_state=State::Empty;
                  
                        return true;}
                   
					
					else if(input=='_'||input>='A'&&input<='Z'||input>='a'&&input<='z'||input>='0'&&input<='9'){
					
						cur_str+=input;
					
                                    return false;
					}
					
					
					else{
						
					buf.value=cur_str;
                        buf.type=get_type_idet(buf.value);
                        cur_str="";
                        cur_str+=input;
                        cur_state=State::op;
                     
                        return true;
					}
									
}
}

                         




#ifdef DEBUG_DFA
    std::cout << "next state is [" << toString(cur_state) << "], next str = " << cur_str << "\t, ret = " << ret << std::endl;
#endif
}
 
void frontend::DFA::reset() {
    cur_state = State::Empty;
    cur_str = "";
}
frontend::DFA::~DFA()  {}
frontend::Scanner::Scanner(std::string filename): fin(filename) {
    if(!fin.is_open()) {
        assert(0 && "in Scanner constructor, input file cannot open");
    }
}

frontend::Scanner::~Scanner() {
    fin.close();
}

std::vector<frontend::Token> frontend::Scanner::run() {
	std::vector<Token> ret;
    frontend::Token tk;
    frontend::DFA dfa;
    std::string s1;
    std::string s="";
    std::string ss="";
    std::string sss="";
    int flag=0;
    	while(getline(fin,s1)){
    		int x=0;
    		
    		while(s1[x]=='\t'||s1[x]==' ')
    		x++;
    		if(s1[x]!='/'&&flag==0){
			
	
	int y=(int)s1[s1.size()-1];
	if(y==13)s+=s1.substr(0,s1.size()-1);
	else s+=s1.substr(0,s1.size());
	s+='\n';
	
	ss+=s1[s1.size()-1];
	if(s1.size()>=2)
	sss+=s1[s1.size()-2];}
	
	if(s1[x]=='/'&&s1[x+1]=='*')
	flag=1;
	if(s1[s1.size()-2]=='/'&&s1[s1.size()-3]=='*')
	flag=0;
    
	}
	int q=(int)ss[ss.size()-1];
	
	s+='\n';
	
    for(auto c: s) {
    	
        if(dfa.next(c, tk)){
        	
            ret.push_back(tk);
        }
}
	

 
    return ret;
#ifdef DEBUG_SCANNER
#include<iostream>
            std::cout << "token: " << toString(tk.type) << "\t" << tk.value << std::endl;
#endif
}

