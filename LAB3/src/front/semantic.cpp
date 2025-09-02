#include"front/semantic.h"
#include<bits/stdc++.h>
#include<cassert>

using ir::Instruction;
using ir::Function;
using ir::Operand;
using ir::Operator;

#define TODO assert(0 && "TODO");

#define GET_CHILD_PTR(node, type, index) auto node = dynamic_cast<type*>(root->children[index]); assert(node); 
#define ANALYSIS(node, type, index) auto node = dynamic_cast<type*>(root->children[index]); assert(node); analysis##type(node);
#define COPY_EXP_NODE(from, to) to->is_computable = from->is_computable; to->v = from->v; to->t = from->t;
#define COPY_tv_NODE(from, to)  to->v = from->v; to->t = from->t;
#define COPY_tv_nt_NODE(from, to)  to->v = from.name; to->t = from.type;
#define COPY_nt_tv_NODE(from, to)  to.name = from->v; to.type = from->t;
#define ANALYSIS_HEAD() log(root);
int scnum=0;
int flag_break=0;
vector<int> break_1;
vector<int> while_1;
int while_i=0;
int flag_quan=0;
ir::Function* nowfunc;
ir::Function*  globalfunc = new ir::Function("global", ir::Type::null);


map<std::string,ir::Function*>* frontend::get_lib_funcs() {
    static map<std::string,ir::Function*> lib_funcs = {
        {"getint", new Function("getint", Type::Int)},
        {"getch", new Function("getch", Type::Int)},
        {"getfloat", new Function("getfloat", Type::Float)},
        {"getarray", new Function("getarray", {Operand("arr", Type::IntPtr)}, Type::Int)},
        {"getfarray", new Function("getfarray", {Operand("arr", Type::FloatPtr)}, Type::Int)},
        {"putint", new Function("putint", {Operand("i", Type::Int)}, Type::null)},
        {"putch", new Function("putch", {Operand("i", Type::Int)}, Type::null)},
        {"putfloat", new Function("putfloat", {Operand("f", Type::Float)}, Type::null)},
        {"putarray", new Function("putarray", {Operand("n", Type::Int), Operand("arr", Type::IntPtr)}, Type::null)},
        {"putfarray", new Function("putfarray", {Operand("n", Type::Int), Operand("arr", Type::FloatPtr)}, Type::null)},
    };
    return &lib_funcs;
}

void frontend::SymbolTable::add_scope() {
    ScopeInfo s;
    scope_stack.push_back(s);
    return;
}
void frontend::SymbolTable::exit_scope() {
    scope_stack.pop_back();
    return;
}

string frontend::SymbolTable::get_scoped_name(string id) const {
    int cnt = scope_stack.size();
    for(int i=cnt-1;i>=0;i--){
        if(scope_stack[i].table.find(id)!=scope_stack[i].table.end()){
            ScopeInfo info = scope_stack[i];
            string name = info.table[id].operand.name;
            return name;
        }
    }
    return "0";
}

Operand frontend::SymbolTable::get_operand(string id) const {
    for(int i=scope_stack.size()-1;i>=0;i--){
        if(scope_stack[i].table.find(id)!=scope_stack[i].table.end()){
        	ScopeInfo s = scope_stack[i];
			ir::Operand re=s.table[id].operand;
			return re;
        }
    }
}

frontend::STE frontend::SymbolTable::get_ste(string id) const {
   
    for(int i=scope_stack.size()-1;i>=0;i--){
        if(scope_stack[i].table.find(id)!=scope_stack[i].table.end()){
        	ScopeInfo s = scope_stack[i];
			frontend::STE re=s.table[id];
			return re;
        }
    }
 
}

frontend::Analyzer::Analyzer(): tmp_cnt(0), symbol_table() {
   
}

ir::Program frontend::Analyzer::get_ir_program(CompUnit* root) {
   
    std::cout<<"888888888888888888888888888888888888888888888888"<<std::endl;
    start1(root);
    end1();
    
    return *program;
}
void frontend::Analyzer::start1(CompUnit* root){
	 program = new ir::Program();
	symbol_table.add_scope();
    com_CompUnit(root);
    return;
}
void frontend::Analyzer::end1(){
	symbol_table.exit_scope();
	globalfunc->addInst(new ir::Instruction(ir::Operand(),ir::Operand(),ir::Operand(), ir::Operator::_return));
	program->functions.insert(program->functions.begin(), *globalfunc);
    return;
}



void frontend::Analyzer::com_CompUnit(AstNode* root){
    AstNode* now_=root->children[0];
    if(now_->type == NodeType::DECL){
        nowfunc = globalfunc;
        flag_quan=1;
        com_Decl(root->children[0]); 
    }
    else{
   
        com_FuncDef(root->children[0]);
    }
    for(int i=0;i<root->children.size()-1;i++){
        com_CompUnit(root->children[i+1]);
    }
}

void frontend::Analyzer::com_Decl(AstNode* root){ 
    if(root->children[0]->type == NodeType::CONSTDECL){
        com_ConstDecl(dynamic_cast<ConstDecl*>(root->children[0]));
    }
    else{
        com_VarDecl(dynamic_cast<VarDecl*>(root->children[0]));
    }
}
void frontend::Analyzer::com_FuncDef(AstNode* root){
    symbol_table.add_scope();
    nowfunc = new Function();
    flag_quan=0;
if(dynamic_cast<Term*>(root->children[1])->token.value=="main"){
        ir::CallInst *callGlobal = new ir::CallInst(ir::Operand("global",ir::Type::null),
                               ir::Operand("t"+std::to_string(scnum),ir::Type::null));
        scnum++;
        nowfunc->addInst(callGlobal);
    }

std::string t = dynamic_cast<Term*>(root->children[0]->children[0])->token.value;
int flag=0;
switch(t[0]){
	case 'i':{
		nowfunc->returnType = ir::Type::Int;
		break;
	}
	case 'v':{
		nowfunc->returnType = ir::Type::null;
		flag=1;

		break;
	}
	case 'f':{
		nowfunc->returnType = ir::Type::Float;
		break;
	}
}
    nowfunc->name = dynamic_cast<Term*>(root->children[1])->token.value;  
   
    if(root->children.size()==6){
        vector<ir::Operand> result = com_FuncFParams( dynamic_cast<FuncFParams*>(root->children[3]));
        for(int i=0;i<result.size();i++){

STE s={result[i],std::vector<int>({100000})};
   
    if(!flag_quan) 
	    {
		s.operand.name += std::to_string(scnum);scnum++;}
    symbol_table.scope_stack[symbol_table.scope_stack.size()-1].table[result[i].name] = s;
			
            result[i].name = symbol_table.get_scoped_name(result[i].name);
        }    
        nowfunc->ParameterList = result;
    }
    symbol_table.functions[nowfunc->name] = nowfunc;  
    for(int i=1;i<root->children[root->children.size()-1]->children.size()-1;i++){
    	com_BlockItem(dynamic_cast<BlockItem*>(root->children[root->children.size()-1]->children[i]));
    }
  if(flag)nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(), ir::Operator::_return));
    program->addFunction(*nowfunc);
    symbol_table.exit_scope();

}

void frontend::Analyzer::com_ConstDecl(ConstDecl* root){
    TokenType x=dynamic_cast<Term*>(root->children[1]->children[0])->token.type;
    ir::Type t=x==TokenType::INTTK?ir::Type::Int:ir::Type::Float;
    int num= (root->children.size()-2)/2;
    for(int i=0;i<num;i++){
            com_ConstDef(root->children[(i+1)*2], t);
    }
}
void frontend::Analyzer::com_VarDecl(VarDecl* root){
  TokenType x=dynamic_cast<Term*>(root->children[0]->children[0])->token.type;
    ir::Type t=x==TokenType::INTTK?ir::Type::Int:ir::Type::Float;
   int num= (root->children.size()-1)/2;
    for(int i=0;i<num;i++){
            com_VarDef(root->children[2*i+1], t);
    }
}

void frontend::Analyzer::com_ConstDef(AstNode* root, ir::Type type){

    int num = 1;
    //先得到数组或者整数或者浮点数的值
    std::vector<ir::Operand> nums = com_ConstInitVal(dynamic_cast<ConstInitVal*> (root->children[root->children.size()-1])); 
   
    //数组
    if( (dynamic_cast<Term*>(root->children[1]))->token.value=="["){
        vector<int> nums1;
        for(int i = 0; i<(root->children.size()-2)/3; i++){
            ConstExp* result=dynamic_cast<ConstExp*> (root->children[2+3*i]);
            com_ConstExp(result);
            if(result->v[0]>'9'||result->v[0]<'0')
            nums1.push_back(100);
            else  nums1.push_back(stoi(result->v));
        }
        for(int i=0;i<nums1.size();i++)
        		num=num*nums1[i];//得到大小
        		
        //全局变量
       if(flag_quan){
       if(type==ir::Type::Int)
            program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::IntPtr), num);
          else program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::FloatPtr), num);
            
        }
        //插入符号表
        STE s={Operand((dynamic_cast<Term*> (root->children[0]))->token.value, type==ir::Type::Int?ir::Type::IntPtr:ir::Type::FloatPtr), nums1};
   
    if(!flag_quan) 
	    {
		s.operand.name += std::to_string(scnum);scnum++;}
    symbol_table.scope_stack[symbol_table.scope_stack.size()-1].table[dynamic_cast<Term*> (root->children[0])->token.value] = s;
         //得到重命名
		ir::Operand coperand=symbol_table.get_operand((dynamic_cast<Term*> (root->children[0]))->token.value);
        
		ir::Operand num1=ir::Operand(std::to_string(num), ir::Type::IntLiteral);//大小变成操作数
        nowfunc->addInst(new ir::Instruction(num1, ir::Operand(),coperand, ir::Operator::alloc));//开辟空间
        
        for(int i=0;i<nums.size();i++){
            if(type == ir::Type::Int){ 
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
            	ir::Operand value1=nums[i];
				nowfunc->addInst(new Instruction(coperand,xu,value1,ir::Operator::store));                   
            }
            else{
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::FloatLiteral);
            	ir::Operand value1=nums[i];
                nowfunc->addInst(new Instruction(coperand,xu,value1,ir::Operator::store));
            }
        }
        
    }
    //整数或者浮点数
    else {
    	//全局变量
    	if(flag_quan){
    		if(type==Type::Int)
            program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::Int));
        
        else  program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::Float));}
    	//插入符号表
        STE s={Operand((dynamic_cast<Term*> (root->children[0]))->token.value, type), vector<int>()};
   
    if(!flag_quan) 
	    {
		s.operand.name += std::to_string(scnum);scnum++;}
    symbol_table.scope_stack[symbol_table.scope_stack.size()-1].table[dynamic_cast<Term*> (root->children[0])->token.value] = s;
      
	
        //得到重命名
        ir::Operand coperand=symbol_table.get_operand((dynamic_cast<Term*> (root->children[0]))->token.value);
      
        if(type==Type::Int){   
		 if(nums[0].type==ir::Type::FloatLiteral)
        nums[0].type=ir::Type::IntLiteral;
        if(nums[0].type==ir::Type::Float)
        nums[0].type=ir::Type::Int;  
        nowfunc->addInst(new ir::Instruction(nums[0],ir::Operand(),coperand, ir::Operator::def));}
        else { 
        if(nums[0].type==ir::Type::IntLiteral)
        nums[0].type=ir::Type::FloatLiteral;
        if(nums[0].type==ir::Type::Int)
        nums[0].type=ir::Type::Float;
        nowfunc->addInst(new ir::Instruction(nums[0],ir::Operand(),coperand, ir::Operator::fdef));}

    }

}


void frontend::Analyzer::com_ConstExp(ConstExp* root){
    AddExp* re=dynamic_cast<AddExp*>(root->children[0]);
    com_AddExp(re);
    COPY_EXP_NODE(re, root);
    
}
std::vector<ir::Operand> frontend::Analyzer::com_ConstInitVal(ConstInitVal* root){
    std::vector<ir::Operand> result;
    if(root->children.size()>1){	
    for(int i = 0;i<(root->children.size()-1)/2; i++){
            std::vector<ir::Operand> s = com_ConstInitVal(dynamic_cast<ConstInitVal*> (root->children[1+2*i]));
            for(int j=0;j<s.size();j++){
            result.push_back(s[j]);}
      }
      return result;
	  }
    ConstExp* result1=dynamic_cast<ConstExp*> (root->children[0]);
    com_ConstExp(result1);
    result.push_back(Operand(result1->v,result1->t));   
    return result;
}



void frontend::Analyzer::com_VarDef(AstNode* root, ir::Type type){
    int num=1;
    if(root->children.size()==1){
	
           	//全局变量
    	if(flag_quan){
    		if(type==Type::Int)
            program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::Int));
        
        else  program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::Float));}
    	//插入符号表
    	 STE s={Operand((dynamic_cast<Term*> (root->children[0]))->token.value, type), vector<int>()};
   
    if(!flag_quan) 
	    {
		s.operand.name += std::to_string(scnum);scnum++;}
    symbol_table.scope_stack[symbol_table.scope_stack.size()-1].table[dynamic_cast<Term*> (root->children[0])->token.value] = s;
      
       
        //得到重命名
        ir::Operand coperand=symbol_table.get_operand((dynamic_cast<Term*> (root->children[0]))->token.value);
        if(type==Type::Int){     
        nowfunc->addInst(new ir::Instruction(ir::Operand("0", ir::Type::IntLiteral),ir::Operand(),coperand, ir::Operator::def));}
        else { 
        nowfunc->addInst(new ir::Instruction(ir::Operand("0.0", ir::Type::FloatLiteral),ir::Operand(),coperand, ir::Operator::fdef));}
        
        
     }
     else if((dynamic_cast<Term*>(root->children[1]))->token.value=="["){
        // 插入符号表
        int flag2=0;
        vector<int> nums1;
        for(int i = 2; i<root->children.size() && root->children[i]->type==NodeType::CONSTEXP; i+=3){
        	std::cout<<"start"<<std::endl;
            ConstExp* result=dynamic_cast<ConstExp*> (root->children[i]);
            com_ConstExp(result);
            std::cout<<"end  "<<result->v<<std::endl;
              if(result->v[0]>'9'||result->v[0]<'0'){
			  flag2=1;
            nums1.push_back(30);}
            else  nums1.push_back(stoi(result->v));
        }
        for(int i=0;i<nums1.size();i++)
        		num=num*nums1[i];//得到大小
        std::cout<<"*-*-*-*-*-"<<num<<std::endl;
        std::cout<<(dynamic_cast<Term*> (root->children[0]))->token.value<<std::endl;
         //全局变量
       if(flag_quan){
       if(type==ir::Type::Int)
            program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::IntPtr), num);
          else program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::FloatPtr), num);
            
        }
        STE s={Operand((dynamic_cast<Term*> (root->children[0]))->token.value, type==ir::Type::Int?ir::Type::IntPtr:ir::Type::FloatPtr), nums1};
   
    if(!flag_quan) 
	    {
		s.operand.name += std::to_string(scnum);scnum++;}
    symbol_table.scope_stack[symbol_table.scope_stack.size()-1].table[dynamic_cast<Term*> (root->children[0])->token.value] = s;
      
           ir::Operand coperand=symbol_table.get_operand((dynamic_cast<Term*> (root->children[0]))->token.value);
		ir::Operand num1=ir::Operand(std::to_string(num), ir::Type::IntLiteral);//大小变成操作数
        nowfunc->addInst(new ir::Instruction(num1, ir::Operand(),coperand, ir::Operator::alloc));//开辟空间

        if(root->children[root->children.size()-1]->type!=NodeType::INITVAL){
            if(type==ir::Type::Int)
            for(int i=0;i<num;i++){
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
                nowfunc->addInst(new ir::Instruction(coperand, xu, ir::Operand("0", ir::Type::IntLiteral), ir::Operator::store));
            }
            else for(int i=0;i<num;i++){
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
                nowfunc->addInst(new ir::Instruction(coperand, xu, ir::Operand("0.0", ir::Type::FloatLiteral), ir::Operator::store));
            }
        }

        if(root->children[root->children.size()-1]->type==NodeType::INITVAL){    
            std::vector<ir::Operand> nums = com_InitVal(dynamic_cast<InitVal*> (root->children[root->children.size()-1])); 
            if(nums.size()==0||nums.size()==1&&nums[0].name=="0"){
            if(type==ir::Type::Int)
            for(int i=0;i<num;i++){
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
                nowfunc->addInst(new ir::Instruction(coperand, xu, ir::Operand("0", ir::Type::IntLiteral), ir::Operator::store));
            }
            else for(int i=0;i<num;i++){
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
                nowfunc->addInst(new ir::Instruction(coperand, xu, ir::Operand("0.0", ir::Type::FloatLiteral), ir::Operator::store));
            }
        }
        else{
		
			for(int i=0;i<nums.size();i++){
            
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
            	ir::Operand value1=nums[i];
            	if(type==ir::Type::Int){
            		if(value1.type==ir::Type::Float){
            			ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            			scnum++;
            			nowfunc->addInst(new Instruction(value1,ir::Operand(),re,ir::Operator::cvt_f2i));                   
						nowfunc->addInst(new Instruction(coperand,xu,re,ir::Operator::store)); 
					}
					else if(value1.type==ir::Type::FloatLiteral){
            			ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            			scnum++;
            			ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            			scnum++;
            			nowfunc->addInst(new Instruction(value1,ir::Operand(),re1,ir::Operator::fmov)); 
						nowfunc->addInst(new Instruction(re1,ir::Operand(),re,ir::Operator::cvt_f2i));                        
						nowfunc->addInst(new Instruction(coperand,xu,re,ir::Operator::store)); 
					}
					else 	nowfunc->addInst(new Instruction(coperand,xu,value1,ir::Operator::store));                   

				}
				else if(type==ir::Type::Float){
					if(value1.type==ir::Type::Int){
            			ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            			scnum++;
            			nowfunc->addInst(new Instruction(value1,ir::Operand(),re,ir::Operator::cvt_i2f));                   
						nowfunc->addInst(new Instruction(coperand,xu,re,ir::Operator::store)); 
					}
					else if(value1.type==ir::Type::IntLiteral){
            			ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            			scnum++;
            			ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            			scnum++;
            			nowfunc->addInst(new Instruction(value1,ir::Operand(),re,ir::Operator::mov)); 
						nowfunc->addInst(new Instruction(re,ir::Operand(),re1,ir::Operator::cvt_i2f));                        
						nowfunc->addInst(new Instruction(coperand,xu,re1,ir::Operator::store)); 
					}
					else 	nowfunc->addInst(new Instruction(coperand,xu,value1,ir::Operator::store));                   

				}
				else nowfunc->addInst(new Instruction(coperand,xu,value1,ir::Operator::store));                   

        }
        std::cout<<"-----------48489    "<<nums.size()<<"    "<<num<<std::endl;
        if(flag2==0||flag2==1){
		
        if(type==ir::Type::Int)
            for(int i=nums.size();i<num;i++){
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
                nowfunc->addInst(new ir::Instruction(coperand, xu, ir::Operand("0", ir::Type::IntLiteral), ir::Operator::store));
            }
            else for(int i=nums.size();i<num;i++){
            	ir::Operand xu=ir::Operand(std::to_string(i),ir::Type::IntLiteral);
                nowfunc->addInst(new ir::Instruction(coperand, xu, ir::Operand("0.0", ir::Type::FloatLiteral), ir::Operator::store));
            }}
		}
        }
    }
	else {

           	//全局变量
    	if(flag_quan){
    		if(type==Type::Int)
            program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::Int));
        
        else  program->globalVal.emplace_back(ir::Operand((dynamic_cast<Term*> (root->children[0]))->token.value, ir::Type::Float));}
    	//插入符号表
    	STE s={Operand((dynamic_cast<Term*> (root->children[0]))->token.value, type), vector<int>()};
   
    if(!flag_quan) 
	    {
		s.operand.name += std::to_string(scnum);scnum++;}
    symbol_table.scope_stack[symbol_table.scope_stack.size()-1].table[dynamic_cast<Term*> (root->children[0])->token.value] = s;
      
   
        //得到重命名
        ir::Operand coperand=symbol_table.get_operand((dynamic_cast<Term*> (root->children[0]))->token.value);
      
	    std::vector<ir::Operand> nums = com_InitVal(dynamic_cast<InitVal*> (root->children[root->children.size()-1])); 
	    
          if(type==Type::Int){     
             nowfunc->addInst(new ir::Instruction(nums[0],ir::Operand(),coperand, ir::Operator::def));}
           else { 
             nowfunc->addInst(new ir::Instruction(nums[0],ir::Operand(),coperand, ir::Operator::fdef));}
  }}


std::vector<ir::Operand> frontend::Analyzer::com_InitVal(InitVal* root){
std::vector<ir::Operand> result;
    if(root->children.size()>1){
	if(root->children.size()==2)
	return result;	
    for(int i = 0;i<(root->children.size()-1)/2; i++){
            std::vector<ir::Operand> s = com_InitVal(dynamic_cast<InitVal*> (root->children[1+2*i]));
            for(int j=0;j<s.size();j++){
            result.push_back(s[j]);}
      }
      return result;
	  }
    Exp* result1=dynamic_cast<Exp*> (root->children[0]);
    com_Exp(result1);
    result.push_back(Operand(result1->v,result1->t));   
    return result;
}

void frontend::Analyzer::com_Exp(Exp* root){
        
    AddExp* a=dynamic_cast<AddExp*>(root->children[0]);
    com_AddExp(a);
    COPY_EXP_NODE(a, root);
}

std::vector<ir::Operand> frontend::Analyzer::com_FuncFParams(FuncFParams * root){
    std::vector<ir::Operand> result;
    for(int i=0;i<(root->children.size()+1)/2; i++){
        result.push_back(com_FuncFParam(dynamic_cast<FuncFParam*>(root->children[2*i])));
    } 
    return result;
}

ir::Operand frontend::Analyzer::com_FuncFParam(FuncFParam* root){  
	ir::Type t;
	if(dynamic_cast<Term*>(root->children[0]->children[0])->token.value=="int")
	   t=ir::Type::Int;
	else t=ir::Type::Float;
     ir::Operand x1=ir::Operand(dynamic_cast<Term*>(root->children[1])->token.value, t);
     ir::Operand x2;
     if(t==ir::Type::Int)
	     x2=ir::Operand(dynamic_cast<Term*>(root->children[1])->token.value, ir::Type::IntPtr );
	 else 
	     x2=ir::Operand(dynamic_cast<Term*>(root->children[1])->token.value, ir::Type::FloatPtr );
    if(dynamic_cast<Term*>(root->children[root->children.size()-1])->token.value=="]"){

        return x2;
    }
	else{
        return x1;
    }

  
}




void frontend::Analyzer::com_BlockItem(BlockItem* root){
    if(root->children[0]->type == NodeType::DECL){
        com_Decl(dynamic_cast<Decl*>(root->children[0]));
    }else{
        com_Stmt(dynamic_cast<Stmt*>(root->children[0]));
    }
    return;
}


int frontend::Analyzer::com_Stmt(Stmt* root){
    NodeType firstNodeType = root->children[0]->type;
    if(root->children[0]->type == NodeType::LVAL){
    	vector<ir::Operand>re;
        re = com_LVal(dynamic_cast<LVal*>(root->children[0]));
		Exp* result=dynamic_cast<Exp*>(root->children[2]);
		com_Exp(result);
        ir::Operand des=ir::Operand(result->v, result->t);
        if(re.size()==1){
            nowfunc->addInst(new ir::Instruction(des, ir::Operand(),re[0], ir::Operator::mov));
         return -1;
        }
        nowfunc->addInst(new ir::Instruction(re[0], re[1], des, ir::Operator::store));
		return -1;
    }
	if(root->children[0]->type == NodeType::BLOCK){

symbol_table.add_scope();
   
    
for(int i=1;i<root->children[root->children.size()-1]->children.size()-1;i++){
    
    	com_BlockItem(dynamic_cast<BlockItem*>(root->children[root->children.size()-1]->children[i]));
      
    }symbol_table.exit_scope();
        return -1;
    }
    if(root->children[0]->type == NodeType::EXP){
        Exp* b=dynamic_cast<Exp*>(root->children[0]);
        com_Exp(b);
		return -1;
    }

    if(dynamic_cast<Term*>(root->children[0])->token.value== "if"){
            
            Cond* b=dynamic_cast<Cond*>(root->children[2]);
            com_Cond(b);
            std::cout<<";';';';''"<<"    "<<b->v<<std::endl;
            ir::Operand go_=ir::Operand(b->v,b->t);
            ir::Operand go_num=ir::Operand("2", ir::Type::IntLiteral);//满足if pc+2
            nowfunc->addInst(new ir::Instruction(go_, ir::Operand(), go_num, ir::Operator::_goto));
            int num = nowfunc->InstVec.size();
            nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand("0", ir::Type::IntLiteral), ir::Operator::_goto));
            Stmt* b2=dynamic_cast<Stmt*>(root->children[4]);
            com_Stmt(b2);
            int num2 = nowfunc->InstVec.size()-num;
            if(root->children.size()==7){
				int num1 = nowfunc->InstVec.size();
                nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand("0", ir::Type::IntLiteral), ir::Operator::_goto));
                Stmt* b1=dynamic_cast<Stmt*>(root->children[6]);
            com_Stmt(b1);
                nowfunc->InstVec.erase(nowfunc->InstVec.begin()+num1);
                nowfunc->InstVec.insert(nowfunc->InstVec.begin()+num1,new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(std::to_string(nowfunc->InstVec.size()-num1+1), ir::Type::IntLiteral), ir::Operator::_goto));
             nowfunc->InstVec.erase(nowfunc->InstVec.begin()+num);
			nowfunc->InstVec.insert(nowfunc->InstVec.begin()+num,new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(std::to_string(num2+1), ir::Type::IntLiteral), ir::Operator::_goto));
}
			   else{nowfunc->InstVec.erase(nowfunc->InstVec.begin()+num);
			nowfunc->InstVec.insert(nowfunc->InstVec.begin()+num,new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(std::to_string(nowfunc->InstVec.size()-num+1), ir::Type::IntLiteral), ir::Operator::_goto));
            
			}
			
            nowfunc->addInst(new ir::Instruction(ir::Operand(),ir::Operand(),ir::Operand(),ir::Operator::__unuse__));
			return -1;
        }
		if(dynamic_cast<Term*>(root->children[0])->token.value== "while"){
            
			while_i++;
            int num1 = nowfunc->InstVec.size();
            while_1 .push_back(nowfunc->InstVec.size());
            Cond* b=dynamic_cast<Cond*>(root->children[2]);
            com_Cond(b);
            ir::Operand go_=ir::Operand(b->v,b->t);
            ir::Operand go_num=ir::Operand("2", ir::Type::IntLiteral);//满足if pc+2
            nowfunc->addInst(new ir::Instruction(go_, ir::Operand(), go_num, ir::Operator::_goto));
            
            int num = nowfunc->InstVec.size();
            nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand("0", ir::Type::IntLiteral), ir::Operator::_goto));
            break_1.clear();
			Stmt* b2=dynamic_cast<Stmt*>(root->children[4]);
            com_Stmt(b2);
            nowfunc->InstVec.erase(nowfunc->InstVec.begin()+num);
			nowfunc->InstVec.insert(nowfunc->InstVec.begin()+num,new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(std::to_string(nowfunc->InstVec.size()-num+2), ir::Type::IntLiteral), ir::Operator::_goto));
            int num2 = nowfunc->InstVec.size()-num1;
            nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(std::to_string(-num2), ir::Type::IntLiteral), ir::Operator::_goto));
			for(int k=0;k<break_1.size();k++){
				 nowfunc->InstVec.erase(nowfunc->InstVec.begin()+break_1[k]);
				nowfunc->InstVec.insert(nowfunc->InstVec.begin()+break_1[k],new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(std::to_string(nowfunc->InstVec.size()-break_1[k]+1), ir::Type::IntLiteral), ir::Operator::_goto));
			}
            nowfunc->addInst(new ir::Instruction(ir::Operand(),ir::Operand(),ir::Operand(),ir::Operator::__unuse__));
            while_i--;
			return -1;
        }
	if(dynamic_cast<Term*>(root->children[0])->token.value== "break"){
            break_1.push_back(nowfunc->InstVec.size());
            nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand("0", ir::Type::IntLiteral), ir::Operator::_goto));
			return nowfunc->InstVec.size()-1;
        }
		if(dynamic_cast<Term*>(root->children[0])->token.value== "continue"){
          
            int num = nowfunc->InstVec.size()-while_1[while_i-1];
            nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(std::to_string(-num), ir::Type::IntLiteral), ir::Operator::_goto));
			return -1;
        }
		if(dynamic_cast<Term*>(root->children[0])->token.value== "return"){
        
            if(root->children.size() >2){
            	Exp* e=dynamic_cast<Exp*>(root->children[1]);
                com_Exp(e);
                ir::Operand re;
                COPY_nt_tv_NODE(e, re);
                ir::Operand s=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);scnum++;
                ir::Operand s1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);scnum++;
                ir::Operand s2=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);scnum++;
                ir::Operand s3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);scnum++;
                if(nowfunc->returnType==ir::Type::Int){
                	if(re.type==ir::Type::Float){
                		nowfunc->addInst(new ir::Instruction(re, ir::Operand(), s, ir::Operator::cvt_f2i));
                		nowfunc->addInst(new ir::Instruction(s, ir::Operand(), ir::Operand(), ir::Operator::_return));
					}
					else if(re.type==ir::Type::FloatLiteral){
						nowfunc->addInst(new ir::Instruction(re, ir::Operand(), s2, ir::Operator::fdef));
						nowfunc->addInst(new ir::Instruction(s2, ir::Operand(), s1, ir::Operator::fmov));
						nowfunc->addInst(new ir::Instruction(s1, ir::Operand(), s, ir::Operator::cvt_f2i));
                		nowfunc->addInst(new ir::Instruction(s, ir::Operand(), ir::Operand(), ir::Operator::_return));
					}
					else{
						nowfunc->addInst(new ir::Instruction(re, ir::Operand(), ir::Operand(), ir::Operator::_return));
					}
				}
				if(nowfunc->returnType==ir::Type::Float){
					if(re.type==ir::Type::Int){
                		nowfunc->addInst(new ir::Instruction(re, ir::Operand(), s1, ir::Operator::cvt_i2f));
                		nowfunc->addInst(new ir::Instruction(s1, ir::Operand(), ir::Operand(), ir::Operator::_return));
					}
					else if(re.type==ir::Type::IntLiteral){
						nowfunc->addInst(new ir::Instruction(re, ir::Operand(), s, ir::Operator::def));
						nowfunc->addInst(new ir::Instruction(s, ir::Operand(), s3, ir::Operator::mov));
						nowfunc->addInst(new ir::Instruction(s3, ir::Operand(), s1, ir::Operator::cvt_i2f));
                		nowfunc->addInst(new ir::Instruction(s1, ir::Operand(), ir::Operand(), ir::Operator::_return));
					}
					else{
						nowfunc->addInst(new ir::Instruction(re, ir::Operand(), ir::Operand(), ir::Operator::_return));
					}
				}
              
            }else{
                nowfunc->addInst(new ir::Instruction(ir::Operand(), ir::Operand(), ir::Operand(), ir::Operator::_return));
            }
            return -1;
        }
    return -1;
}
std::vector<ir::Operand> frontend::Analyzer::com_LVal(LVal* root){
    vector<ir::Operand> r;
    ir::Operand operand = symbol_table.get_operand(dynamic_cast<Term*>(root->children[0])->token.value);
    vector<ir::Operand> nums;  
    if(root->children.size()==1){

        COPY_tv_nt_NODE(operand, root);root->is_computable=false;
      	r.push_back(operand);
    
    return r;
	}
    if(root->children.size()>1){
        
        for(int i = 0; i<(root->children.size()-1)/3; i++){
        	Exp* result=dynamic_cast<Exp*>(root->children[3*i+2]);
            com_Exp(result);
            ir::Operand x=ir::Operand(result->v, result->t);
            nums.push_back(x);
           
            
        }
		
            vector<int> muls;
            STE ste = symbol_table.get_ste(dynamic_cast<Term*>(root->children[0])->token.value);
            
            for(int k=1;k<nums.size()-1;k++){
            	int w=1;
            	for(int q=k+1;q<nums.size();q++)
            	     w*=ste.dimension[q];
            	muls.push_back(w);
			}
			muls.push_back(ste.dimension[ste.dimension.size()-1]);
		
			ir::Operand re("t"+std::to_string(scnum), ir::Type::Int);
			scnum++;
            nowfunc->addInst(new ir::Instruction(nums[nums.size()-1], ir::Operand(), re, ir::Operator::mov));
            for(int i=0,j=0;i<muls.size()-1,j<nums.size()-1;i++,j++){
            	if(nums[j].type==ir::Type::Int||nums[j].type==ir::Type::Float){
            		ir::Operand re1("t"+std::to_string(scnum), ir::Type::Int);scnum++;
            		ir::Operand op2=Operand(std::to_string(muls[i]),ir::Type::IntLiteral);
                        nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), re1, ir::Operator::def));
                        nowfunc->addInst(new ir::Instruction(re1, nums[j], re1, ir::Operator::mul));
                        nowfunc->addInst(new ir::Instruction(re, re1, re, ir::Operator::add));
				}
				else{
					ir::Operand re1("t"+std::to_string(scnum), ir::Type::Int);scnum++;
					ir::Operand re2("t"+std::to_string(scnum), ir::Type::Int);scnum++;
            		ir::Operand op2=Operand(std::to_string(muls[i]),ir::Type::IntLiteral);
                        nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), re1, ir::Operator::def));
                        nowfunc->addInst(new ir::Instruction(nums[j], ir::Operand(), re2, ir::Operator::def));
                        nowfunc->addInst(new ir::Instruction(re2, re1, re1, ir::Operator::mul));
                        nowfunc->addInst(new ir::Instruction(re, re1, re, ir::Operator::add));
				}
               
            }
            
            
            ir::Operand des1;
            if(operand.type==ir::Type::IntPtr){
            	des1=Operand("t"+std::to_string(scnum),ir::Type::Int);
            	root->t=ir::Type::Int;
            	scnum++;
			}
			else{
				des1=Operand("t"+std::to_string(scnum),ir::Type::Float);
				root->t=ir::Type::Float;
            	scnum++;
			}

            nowfunc->addInst(new ir::Instruction(operand, re,des1, ir::Operator::load));
            root->v = des1.name; root->is_computable=false;
			r.push_back(operand);
			r.push_back(re);       
            return r;
    }

}



void frontend::Analyzer::com_Cond(Cond* root){
    LOrExp* l=dynamic_cast<LOrExp*>(root->children[0]);
    com_LOrExp(l);
    COPY_EXP_NODE(l, root);
    return;
}

void frontend::Analyzer::com_MulExp(MulExp* root){
	std::cout<<"mulexp"<<std::endl;
	int flag=0;
		
    UnaryExp* fir=dynamic_cast<UnaryExp*>(root->children[0]);
    com_UnaryExp(fir);
    if(fir->t==ir::Type::Float||fir->t==ir::Type::FloatLiteral)
          flag=1;
    if(root->children.size()==1){
    	 COPY_EXP_NODE(fir, root);
    return;
	}
	for(int i=0;i<(root->children.size()-1)/2;i++){
		UnaryExp* sec=dynamic_cast<UnaryExp*>(root->children[2*i+2]);
    com_UnaryExp(sec);
    		if(sec->t==ir::Type::Float||sec->t==ir::Type::FloatLiteral)
    		flag=1;
	}
    std::cout<<root->children.size()<<std::endl;
    std::cout<<"4564684848    "<<fir->v<<"   "<<toString(fir->t)<<std::endl;
    ir::Operand des1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    scnum++;
    ir::Operand des2=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    scnum++;
  
    if(flag==0){
			if(fir->t==ir::Type::Int)
          nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::mov));
          else{
          	nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::def));
		  }
		  }
    else{
    	if(fir->t==ir::Type::Int)
          nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::mov));
          else if(fir->t==ir::Type::IntLiteral){
          	nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::def));
		  }
    	ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(fir->t==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), des2, ir::Operator::fmov));
			}
    else if(fir->t==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), des2, ir::Operator::fmov));
		  }
    	
	else	if(fir->t==ir::Type::Float)
		{
		
          nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des2, ir::Operator::fmov));}
	else  {
		  	nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), w, ir::Operator::fdef));
		  	nowfunc->addInst(new Instruction(w, ir::Operand(), des2, ir::Operator::fmov));
		  
		  }}
    if(root->children.size()>1){
	
	for(int i=0;i<(root->children.size()-1)/2;i++){
		UnaryExp* sec=dynamic_cast<UnaryExp*>(root->children[2*i+2]);
   
    if(flag){
    				
    	
    	ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(sec->t==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			}
    else if(sec->t==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
		  }
    	
	else	if(sec->t==ir::Type::Float)
		{
		
          nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), w1, ir::Operator::fmov));}
	else  {
		  	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), w, ir::Operator::fdef));
		  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
		  
		  }
    	
    	if(dynamic_cast<Term*>(root->children[2*i+1])->token.value=="%"){
    		if(sec->t==ir::Type::IntLiteral){
				ir::Operand w9=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t),ir::Operand(),  w9, ir::Operator::def));
    		nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::mod));
		}
		else{
				nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::mod));
		}
		nowfunc->addInst(new Instruction(des1, ir::Operand(), des2, ir::Operator::cvt_i2f));
		}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	if(dynamic_cast<Term*>(root->children[2*i+1])->token.value=="*"){
		
    
				nowfunc->addInst(new Instruction(des2,w1,  des2, ir::Operator::fmul));
		
		}
		else nowfunc->addInst(new Instruction(des2,w1,  des2, ir::Operator::fdiv));
	}
	else{
			if(dynamic_cast<Term*>(root->children[2*i+1])->token.value=="*"){
		
    	if(sec->t==ir::Type::IntLiteral){
    			ir::Operand w9=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t),ir::Operand(),  w9, ir::Operator::def));
    		nowfunc->addInst(new Instruction(des1,w9,  des1, ir::Operator::mul));
		}
		else{
				nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::mul));
		}}
		else if(dynamic_cast<Term*>(root->children[2*i+1])->token.value=="/"){
			if(sec->t==ir::Type::IntLiteral){
				ir::Operand w9=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t),ir::Operand(),  w9, ir::Operator::def));
    		nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::div));
		}
		else{
				nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::div));
		}
		}
		else{
			if(sec->t==ir::Type::IntLiteral){
				ir::Operand w9=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t),ir::Operand(),  w9, ir::Operator::def));
    		nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::mod));
		}
		else{
				nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::mod));
		}
		}
	}
	}
	
	
	
	
	}
	if(flag){
		COPY_tv_nt_NODE(des2, root);root->is_computable = false;
	}
	else{
		COPY_tv_nt_NODE(des1, root);root->is_computable = false;
	}
	return;


}

void frontend::Analyzer::com_AddExp(AddExp* root){
	std::cout<<"addexp"<<std::endl;
		int flag=0;
		
    MulExp* fir=dynamic_cast<MulExp*>(root->children[0]);
    com_MulExp(fir);
    if(fir->t==ir::Type::Float||fir->t==ir::Type::FloatLiteral)
          flag=1;
    if(root->children.size()==1){
    	 COPY_EXP_NODE(fir, root);
    return;
	}
	for(int i=0;i<(root->children.size()-1)/2;i++){
		MulExp* sec=dynamic_cast<MulExp*>(root->children[2*i+2]);
    com_MulExp(sec);
    		if(sec->t==ir::Type::Float||sec->t==ir::Type::FloatLiteral)
    		flag=1;
	}
    std::cout<<root->children.size()<<std::endl;
    std::cout<<"4564684848    "<<fir->v<<"   "<<toString(fir->t)<<std::endl;
    ir::Operand des1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    scnum++;
    ir::Operand des2=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    scnum++;
  
    if(flag==0){
			if(fir->t==ir::Type::Int)
          nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::mov));
          else{
          	nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::def));
		  }
		  }
    else{
    	ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(fir->t==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), des2, ir::Operator::fmov));
			}
    else if(fir->t==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), des2, ir::Operator::fmov));
		  }
    	
	else	if(fir->t==ir::Type::Float)
		{
		
          nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des2, ir::Operator::fmov));}
	else  {
		  	nowfunc->addInst(new Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), w, ir::Operator::fdef));
		  	nowfunc->addInst(new Instruction(w, ir::Operand(), des2, ir::Operator::fmov));
		  
		  }}
    if(root->children.size()>1){
	
	for(int i=0;i<(root->children.size()-1)/2;i++){
		MulExp* sec=dynamic_cast<MulExp*>(root->children[2*i+2]);
   
    if(flag){
    				
    	
    	ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(sec->t==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			}
    else if(sec->t==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
		  }
    	
	else	if(sec->t==ir::Type::Float)
		{
		
          nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), w1, ir::Operator::fmov));}
	else  {
		  	nowfunc->addInst(new Instruction(ir::Operand(sec->v,sec->t), ir::Operand(), w, ir::Operator::fdef));
		  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
		  
		  }
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	if(dynamic_cast<Term*>(root->children[2*i+1])->token.value=="+"){
		
    
				nowfunc->addInst(new Instruction(des2,w1,  des2, ir::Operator::fadd));
		
		}
		else nowfunc->addInst(new Instruction(des2,w1,  des2, ir::Operator::fsub));
	}
	else{
			if(dynamic_cast<Term*>(root->children[2*i+1])->token.value=="+"){
		
    	if(sec->t==ir::Type::IntLiteral){
    		nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::addi));
		}
		else{
				nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::add));
		}}
		else{
			if(sec->t==ir::Type::IntLiteral){
    		nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::subi));
		}
		else{
				nowfunc->addInst(new Instruction(des1,ir::Operand(sec->v,sec->t),  des1, ir::Operator::sub));
		}
		}
	}
	}
	
	
	
	
	}
	if(flag){
		COPY_tv_nt_NODE(des2, root);root->is_computable = false;
	}
	else{
		COPY_tv_nt_NODE(des1, root);root->is_computable = false;
	}
	return;

}




void frontend::Analyzer::com_Number(Number* root){
	 std::cout<<"number"<<std::endl;
	if(dynamic_cast<Term*>(root->children[0])->token.type==TokenType::INTLTR){
		root->is_computable = true;
		root->v = dynamic_cast<Term*>(root->children[0])->token.value;
		std::cout<<"?????"<<root->v<<std::endl;
		root->t = ir::Type::IntLiteral;
		return;
	}
	else{
		root->is_computable = true;
		root->v = dynamic_cast<Term*>(root->children[0])->token.value;
		root->t = ir::Type::FloatLiteral;
		return;
	}
}



void frontend::Analyzer::com_PrimaryExp(PrimaryExp *root){
  
    if(root->children.size()==3){
    	Exp* r=dynamic_cast<Exp*>(root->children[1]);
        com_Exp(r);
        COPY_EXP_NODE(r, root);
		return;
    }
	if(root->children[0]->type == NodeType::LVAL){
        LVal* r=dynamic_cast<LVal*>(root->children[0]);
        com_LVal(r);
        COPY_EXP_NODE(r, root);
        return;
    }
    std::cout<<"primaryExp"<<std::endl;
	Number* r=dynamic_cast<Number*>(root->children[0]);
    com_Number(r); 
    COPY_EXP_NODE(r, root);
    return;
    

    
}


void frontend::Analyzer::com_UnaryExp(UnaryExp* root){
  
    if(root->children.size()==1){
    	std::cout<<"unaryexp"<<std::endl;
        PrimaryExp* r=dynamic_cast<PrimaryExp*>(root->children[0]);
        com_PrimaryExp(r);
        COPY_EXP_NODE(r, root);
		return;
    }
	 if(root->children.size()==2){
        
        string fu = dynamic_cast<Term*>(root->children[0]->children[0])->token.value;
        UnaryExp* r1=dynamic_cast<UnaryExp*>(root->children[1]);
        com_UnaryExp(r1);
      
        if(fu=="-"){
            if(r1->t==ir::Type::Int){
            	ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(ir::Operand("0", ir::Type::IntLiteral), ir::Operand(), re1, ir::Operator::def));
            	nowfunc->addInst(new ir::Instruction(re1, op2, re, ir::Operator::sub));
                COPY_tv_nt_NODE(re, root);
                root->is_computable=false;
                return;
            }
			if(r1->t==ir::Type::IntLiteral){
                ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(ir::Operand("0", ir::Type::IntLiteral), ir::Operand(), re1, ir::Operator::def));
            
            	nowfunc->addInst(new ir::Instruction(re1, op2, re, ir::Operator::subi));
                COPY_tv_nt_NODE(re, root);
                root->is_computable=false;
                return;
            }
            if(r1->t==ir::Type::Float){
            	ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(ir::Operand("0.0", ir::Type::FloatLiteral), ir::Operand(), re1, ir::Operator::fdef));
            
            	nowfunc->addInst(new ir::Instruction(re1, op2, re, ir::Operator::fsub));
                COPY_tv_nt_NODE(re, root);
                root->is_computable=false;
                return;
            }
			if(r1->t==ir::Type::FloatLiteral){
                ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(ir::Operand("0.0", ir::Type::FloatLiteral), ir::Operand(), re1, ir::Operator::fdef));
            
            	nowfunc->addInst(new ir::Instruction(re1, op2, re, ir::Operator::fsub));
                COPY_tv_nt_NODE(re, root);
                root->is_computable=false;
                return;
            }
        }
		if(fu=="!"){
            if(r1->t==ir::Type::Int){
            	ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), re1, ir::Operator::mov));
            	nowfunc->addInst(new ir::Instruction(re1, ir::Operand(), re, ir::Operator::_not));
                COPY_tv_nt_NODE(re, root);
                root->is_computable=false;
                return;
            }
			if(r1->t==ir::Type::IntLiteral){
                ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), re1, ir::Operator::def));
            
            	nowfunc->addInst(new ir::Instruction(re1, ir::Operand(), re, ir::Operator::_not));
                COPY_tv_nt_NODE(re, root);
                root->is_computable=false;
                return;
            }
            if(r1->t==ir::Type::Float){
            	ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            	ir::Operand re3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand re4=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), re1, ir::Operator::fmov));
            	nowfunc->addInst(new ir::Instruction(re1, ir::Operand(), re3, ir::Operator::cvt_f2i));
            	nowfunc->addInst(new ir::Instruction(re3, ir::Operand(), re4, ir::Operator::_not));
                COPY_tv_nt_NODE(re4, root);
                root->is_computable=false;
                return;
            }
			if(r1->t==ir::Type::FloatLiteral){
                ir::Operand re=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            	ir::Operand re1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
            	scnum++;
            		ir::Operand re3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand re4=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
            	scnum++;
            	ir::Operand op2;
            	COPY_nt_tv_NODE(r1, op2);
            	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), re1, ir::Operator::fdef));
            	nowfunc->addInst(new ir::Instruction(re1, ir::Operand(), re3, ir::Operator::cvt_f2i));
            	nowfunc->addInst(new ir::Instruction(re3, ir::Operand(), re4, ir::Operator::_not));
            
                COPY_tv_nt_NODE(re4, root);
                root->is_computable=false;
                return;
            }

        }
  
        COPY_EXP_NODE(r1, root);
        return;

        
    }
	if(root->children.size()>2){
        ir::Function* f;
        if(symbol_table.functions.find(dynamic_cast<Term*>(root->children[0])->token.value)!=symbol_table.functions.end()){
            f = symbol_table.functions[dynamic_cast<Term*>(root->children[0])->token.value];
        }
		else{
            f = get_lib_funcs()->find(dynamic_cast<Term*>(root->children[0])->token.value)->second;
        }
        string re5 = "t"+std::to_string(scnum);
        scnum++;
        ir::Operand rere=ir::Operand(re5, f->returnType);
        if(root->children.size()==4){
			vector<ir::Operand> re=com_FuncRParams(dynamic_cast<FuncRParams*>(root->children[2]));
			
			for(int i=0;i<re.size();i++){
				
				ir::Operand s=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);scnum++;
                ir::Operand s1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);scnum++;
                ir::Operand s2=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);scnum++;
                ir::Operand s3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);scnum++;
                if(f->ParameterList[i].type==ir::Type::Int){
                	if(re[i].type==ir::Type::Float){
                		nowfunc->addInst(new ir::Instruction(re[i], ir::Operand(), s, ir::Operator::cvt_f2i));
                		re[i]=s;
					}
					else if(re[i].type==ir::Type::FloatLiteral){
						nowfunc->addInst(new ir::Instruction(re[i], ir::Operand(), s2, ir::Operator::fdef));
						nowfunc->addInst(new ir::Instruction(s2, ir::Operand(), s1, ir::Operator::fmov));
						nowfunc->addInst(new ir::Instruction(s1, ir::Operand(), s, ir::Operator::cvt_f2i));
                		re[i]=s;
					}
					
				}
				if(f->ParameterList[i].type==ir::Type::Float){
					if(re[i].type==ir::Type::Int){
                		nowfunc->addInst(new ir::Instruction(re[i], ir::Operand(), s1, ir::Operator::cvt_i2f));
                		re[i]=s1;
					}
					else if(re[i].type==ir::Type::IntLiteral){
						nowfunc->addInst(new ir::Instruction(re[i], ir::Operand(), s, ir::Operator::def));
						nowfunc->addInst(new ir::Instruction(s, ir::Operand(), s3, ir::Operator::mov));
						nowfunc->addInst(new ir::Instruction(s3, ir::Operand(), s1, ir::Operator::cvt_i2f));
                	re[i]=s1;
					}
				
				}
				
				
				
			}
			
			
			
			
			
			
			
			
			
			
            nowfunc->addInst(new ir::CallInst(ir::Operand(dynamic_cast<Term*>(root->children[0])->token.value, f->returnType),re,         
      rere));    
        }else{
           
          
            nowfunc->addInst(new ir::CallInst(ir::Operand(dynamic_cast<Term*>(root->children[0])->token.value, f->returnType),rere));
        }  
        
COPY_tv_nt_NODE(rere, root);
return;

    }
    return;
    
}






std::vector<ir::Operand> frontend::Analyzer::com_FuncRParams(FuncRParams* root){
	vector<ir::Operand> re;
	for(int i=0;i<(root->children.size()+1)/2;i++){
	
	Exp* e=dynamic_cast<Exp*>(root->children[2*i]);
	com_Exp(e);
	ir::Operand op;
	COPY_nt_tv_NODE(e, op);
	re.push_back(op);
	
	}
	return re;

}




void frontend::Analyzer::com_RelExp(RelExp* root){
AddExp* fir=dynamic_cast<AddExp*>(root->children[0]);
    com_AddExp(fir);
    int flag9=0;
    ir::Operand des1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	ir::Operand des2=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	if(fir->t==ir::Type::Int)
    nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::mov));
    else if(fir->t==ir::Type::IntLiteral) nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::def));
    else if(fir->t==ir::Type::Float){
    	flag9=1;
	nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des2, ir::Operator::fmov));}
	else{
		flag9=1;
	nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des2, ir::Operator::fdef));
	}
    if(root->children.size()>1){
    	ir::Operand op1=ir::Operand(fir->v,fir->t);
    	
    	ir::Operand des3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	AddExp* sec=dynamic_cast<AddExp*>(root->children[2]);
        com_AddExp(sec);
        ir::Operand op2=ir::Operand(sec->v,sec->t);
        int fp=0;
        if(dynamic_cast<Term*>(root->children[1])->token.value==">"){
      	if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::gtr));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgtr));
			}
		
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgtr));
		  }
    	
//	else	if(op1.type==ir::Type::Float)
//		{
//		
//          nowfunc->addInst(new Instruction(op1, ir::Operand(), w1, ir::Operator::fmov));
//          nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgtr));
//		  }
//	else if(op1.type==ir::Type::FloatLiteral) {
//		  	nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::fdef));
//		  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
//		  nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgtr));
//		  }
    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgtr));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgtr));
		  }
    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fgtr));
		}
		
       		
       		
       		
    
       		
       		
       		
		   }
       	
		}
		 
    else if(dynamic_cast<Term*>(root->children[1])->token.value==">="){
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::geq));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgeq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgeq));
		  }
    	

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgeq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgeq));
		  }
    	

    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fgeq));
		}
    	
		   }
       	
		   }
	else if(dynamic_cast<Term*>(root->children[1])->token.value=="<"){
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::lss));
       	else
       	{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::flss));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::flss));
		  }

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::flss));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::flss));
		  }
    	

    	
    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::flss));
		}
		   }
		   }
	else {
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::leq));
       	else
       	{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fleq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fleq));
		  }
    	

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fleq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fleq));
		  }
    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fleq));
		}

		   }
		   }
		   if(fp==0){
		    COPY_tv_nt_NODE(des1,root);root->is_computable=false;
			nowfunc->addInst(new Instruction(des1, ir::Operand(), des3, ir::Operator::_not));}
			else {ir::Operand p=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
		scnum++;
		nowfunc->addInst(new Instruction(des2, ir::Operand(), p, ir::Operator::cvt_f2i));
		nowfunc->addInst(new Instruction(p, ir::Operand(), des3, ir::Operator::_not)); 
			ir::Operand w3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
	nowfunc->addInst(new Instruction(des2, ir::Operand(), w3, ir::Operator::cvt_f2i));
		COPY_tv_nt_NODE(w3,root);root->is_computable=false;}
		vector<int> nums;
		nums.push_back(nowfunc->InstVec.size());
		nowfunc->addInst(new Instruction(des3, ir::Operand(), ir::Operand("0",ir::Type::IntLiteral), ir::Operator::_goto));
fp=0;
    for(int i=1; i<(root->children.size()-1)/2; i++){
    	ir::Operand re1("t"+std::to_string(scnum), ir::Type::Int);
        scnum++;
        AddExp* sec1=dynamic_cast<AddExp*>(root->children[2*i+2]);
        com_AddExp(sec1);
        op1=op2;
         COPY_nt_tv_NODE(sec1, op2);
        
       if(dynamic_cast<Term*>(root->children[1])->token.value==">"){
      	if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::gtr));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgtr));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgtr));
		  }
 
    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1 ,ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgtr));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgtr));
		  }
    	

    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fgtr));
		}
       		
       		
       		
       		
       		
       		
    
       		
       		
		   }
       	
		}
		 
    else if(dynamic_cast<Term*>(root->children[1])->token.value==">="){
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::geq));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgeq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fgeq));
		  }
    	

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgeq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fgeq));
		  }
 
    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fgeq));
		}
    	
		   }
       	
		   }
	else if(dynamic_cast<Term*>(root->children[1])->token.value=="<"){
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::lss));
       	else
       	{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::flss));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::flss));
		  }

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::flss));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::flss));
		  }

    	
    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::flss));
		}
		   }
		   }
	else {
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::leq));
       	else
       	{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fleq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fleq));
		  }

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fleq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fleq));
		  }
    	

   	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fleq));
		}
    	
		   }
		   }
        
        if(fp==0)
			{
			nowfunc->addInst(new Instruction(des1, ir::Operand(), des3, ir::Operator::_not));COPY_tv_nt_NODE(des1,root);root->is_computable=false;}
	else {	ir::Operand w3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
	nowfunc->addInst(new Instruction(des2, ir::Operand(), w3, ir::Operator::cvt_f2i));
	COPY_tv_nt_NODE(w3,root);root->is_computable=false;
		ir::Operand p=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
		scnum++;
		nowfunc->addInst(new Instruction(des2, ir::Operand(), p, ir::Operator::cvt_f2i));
		nowfunc->addInst(new Instruction(p, ir::Operand(), des3, ir::Operator::_not));
	}
		nums.push_back(nowfunc->InstVec.size());
		nowfunc->addInst(new Instruction(des3, ir::Operand(), ir::Operand("0",ir::Type::IntLiteral), ir::Operator::_goto));
fp=0;
		}
		for(int j=0;j<nums.size();j++){
		nowfunc->InstVec[nums[j]]->des.name=std::to_string(nowfunc->InstVec.size()-nums[j]);
		}

   
   return;}
   std::cout<<"75362555555555555555555555555555555555555  "<<toString(fir->t)<<"     "<<fir->v<<std::endl;
    if(fir->t==ir::Type::Int||fir->t==ir::Type::IntLiteral){
	
    COPY_tv_nt_NODE(des1,root);root->is_computable=false;}
    else {
    
    COPY_tv_nt_NODE(des2,root);root->is_computable=false;}
    
    return;

    
}





void frontend::Analyzer::com_EqExp(EqExp* root){
	RelExp* fir=dynamic_cast<RelExp*>(root->children[0]);
    com_RelExp(fir);
    int flag9=0;
    ir::Operand des1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	ir::Operand des2=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	if(fir->t==ir::Type::Int)
    nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::mov));
    else if(fir->t==ir::Type::IntLiteral) nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des1, ir::Operator::def));
    else if(fir->t==ir::Type::Float){
    	flag9=1;
	nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des2, ir::Operator::fmov));}
	else{
		flag9=1;
	nowfunc->addInst(new ir::Instruction(ir::Operand(fir->v,fir->t), ir::Operand(), des2, ir::Operator::fdef));
	}
    if(root->children.size()>1){
    	ir::Operand op1=ir::Operand(fir->v,fir->t);
    	
    	ir::Operand des3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	RelExp* sec=dynamic_cast<RelExp*>(root->children[2]);
        com_RelExp(sec);
        ir::Operand op2=ir::Operand(sec->v,sec->t);
        int fp=0;
        if(dynamic_cast<Term*>(root->children[1])->token.value=="=="){
      	if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::eq));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::feq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::feq));
		  }
    	

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::feq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::feq));
		  }
    	
	
       		
       		
       		else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::feq));
		}
       		
       		
       		
       		
       		
      
		   }
       	
		}
		 
    else {
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::neq));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fneq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fneq));
		  }

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fneq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fneq));
		  }
    	
 	
    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fneq));
		}
		   }
       	
		   }
	
    
		   if(fp==0){
		    COPY_tv_nt_NODE(des1,root);root->is_computable=false;
			nowfunc->addInst(new Instruction(des1, ir::Operand(), des3, ir::Operator::_not));}
			else {ir::Operand p=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
		scnum++;
		nowfunc->addInst(new Instruction(des2, ir::Operand(), p, ir::Operator::cvt_f2i));
		nowfunc->addInst(new Instruction(p, ir::Operand(), des3, ir::Operator::_not)); 
			ir::Operand w3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
	nowfunc->addInst(new Instruction(des2, ir::Operand(), w3, ir::Operator::cvt_f2i));
		COPY_tv_nt_NODE(w3,root);root->is_computable=false;}
		vector<int> nums;
		nums.push_back(nowfunc->InstVec.size());
		nowfunc->addInst(new Instruction(des3, ir::Operand(), ir::Operand("0",ir::Type::IntLiteral), ir::Operator::_goto));
fp=0;
    for(int i=1; i<(root->children.size()-1)/2; i++){
    	ir::Operand re1("t"+std::to_string(scnum), ir::Type::Int);
        scnum++;
        RelExp* sec1=dynamic_cast<RelExp*>(root->children[2*i+2]);
        com_RelExp(sec1);
        op1=op2;
         COPY_nt_tv_NODE(sec1, op2);
        
       if(dynamic_cast<Term*>(root->children[1])->token.value=="=="){
      	if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::eq));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::feq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::feq));
		  }
    	

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::feq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::feq));
		  }
    	
  	
       		
       		
       		else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::feq));
		}
       		
  
       		
       		
		   }
       	
		}
		 
    else {
		if((op1.type==ir::Type::Int||op1.type==ir::Type::IntLiteral)&&(op2.type==ir::Type::Int||op2.type==ir::Type::IntLiteral))
       	nowfunc->addInst(new Instruction(op1, op2, des1, ir::Operator::neq));
       	else{fp=1;
       			ir::Operand w=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand w1=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	ir::Operand e=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	if(op1.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op1, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fneq));
			}
    else if(op1.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op1, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(w1, op2, des2, ir::Operator::fneq));
		  }
    	

    else	if(op2.type==ir::Type::Int){
		
    	    nowfunc->addInst(new Instruction(op2, ir::Operand(), w, ir::Operator::cvt_i2f));
			nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
			nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fneq));
			}
    else if(op2.type==ir::Type::IntLiteral)
    	  {
    	  	nowfunc->addInst(new Instruction(op2, ir::Operand(), e, ir::Operator::def));
    	  	nowfunc->addInst(new Instruction(e, ir::Operand(), w, ir::Operator::cvt_i2f));
    	  	nowfunc->addInst(new Instruction(w, ir::Operand(), w1, ir::Operator::fmov));
    	  	nowfunc->addInst(new Instruction(op1, w1, des2, ir::Operator::fneq));
		  }
    	
    	
    	else{
    		nowfunc->addInst(new Instruction(op1, op2, des2, ir::Operator::fneq));
		}
		   }
       	
		   }
        
        
        if(fp==0)
			{
			nowfunc->addInst(new Instruction(des1, ir::Operand(), des3, ir::Operator::_not));COPY_tv_nt_NODE(des1,root);root->is_computable=false;}
	else {	ir::Operand w3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
	nowfunc->addInst(new Instruction(des2, ir::Operand(), w3, ir::Operator::cvt_f2i));
	COPY_tv_nt_NODE(w3,root);root->is_computable=false;
		ir::Operand p=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
		scnum++;
		nowfunc->addInst(new Instruction(des2, ir::Operand(), p, ir::Operator::cvt_f2i));
		nowfunc->addInst(new Instruction(p, ir::Operand(), des3, ir::Operator::_not));
	}
		nums.push_back(nowfunc->InstVec.size());
		nowfunc->addInst(new Instruction(des3, ir::Operand(), ir::Operand("0",ir::Type::IntLiteral), ir::Operator::_goto));
fp=0;
		}
		for(int j=0;j<nums.size();j++){
		nowfunc->InstVec[nums[j]]->des.name=std::to_string(nowfunc->InstVec.size()-nums[j]);
		}

   
   return;}
   std::cout<<"75362555555555555555555555555555555555555  "<<toString(fir->t)<<"     "<<fir->v<<std::endl;
    if(fir->t==ir::Type::Int||fir->t==ir::Type::IntLiteral){
	
    COPY_tv_nt_NODE(des1,root);root->is_computable=false;}
    else {

    COPY_tv_nt_NODE(des2,root);root->is_computable=false;}
    
    return;

 
}


void frontend::Analyzer::com_LAndExp(LAndExp* root){
    EqExp* re=dynamic_cast<EqExp*>(root->children[0]);
    com_EqExp(re);

    if(root->children.size() == 1){
        COPY_EXP_NODE(re, root);
        return;
    }else{
    	ir::Operand op1=ir::Operand(re->v,re->t);
    	ir::Operand des1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    		ir::Operand des10=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	if(re->t==ir::Type::Int)
    	nowfunc->addInst(new ir::Instruction(op1, ir::Operand(), des1, ir::Operator::mov));
    	else if(re->t==ir::Type::IntLiteral)
    	   {
    	   	nowfunc->addInst(new ir::Instruction(op1, ir::Operand(), des1, ir::Operator::def));
		   }
		else {
		nowfunc->addInst(new ir::Instruction(op1, ir::Operand("0.0",ir::Type::FloatLiteral), des10, ir::Operator::fneq));
		nowfunc->addInst(new ir::Instruction(des10, ir::Operand(), des1, ir::Operator::cvt_f2i));
		}
    	ir::Operand des2=ir::Operand("1",ir::Type::IntLiteral);
    	ir::Operand t1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    	ir::Operand is_=ir::Operand(re->v, re->t);
    	nowfunc->addInst(new ir::Instruction(des1, ir::Operand(), t1, ir::Operator::_not));
    	int num=nowfunc->InstVec.size();
    	 LAndExp* re1=dynamic_cast<LAndExp*>(root->children[2]);
    com_LAndExp(re1);

        ir::Operand op2=ir::Operand(re1->v,re1->t);
        	ir::Operand des3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    		ir::Operand des11=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	if(re1->t==ir::Type::Int)
    	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), des3, ir::Operator::mov));
    	else if(re1->t==ir::Type::IntLiteral)
    	   {
    	   	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), des3, ir::Operator::def));
		   }
		else {
		nowfunc->addInst(new ir::Instruction(op2, ir::Operand("0.0",ir::Type::FloatLiteral), des11, ir::Operator::fneq));
		nowfunc->addInst(new ir::Instruction(des11, ir::Operand(), des3, ir::Operator::cvt_f2i));
		}
        nowfunc->addInst(new ir::Instruction(des1, des3,des1, ir::Operator::_and));
		std::cout<<"*-***********************----------------------------*************************---------"<<std::to_string(nowfunc->InstVec.size()-num)<<std::endl;
		nowfunc->InstVec.insert(nowfunc->InstVec.begin()+num,new ir::Instruction(t1, ir::Operand(), ir::Operand(std::to_string(nowfunc->InstVec.size()-num+1),ir::Type::IntLiteral), ir::Operator::_goto));

        COPY_tv_nt_NODE(des1, root);
root->is_computable=false;   
        return;
    }
}


void frontend::Analyzer::com_LOrExp(LOrExp *root){
  LAndExp* re=dynamic_cast<LAndExp*>(root->children[0]);
    com_LAndExp(re);

    
    if(re->is_computable==true)
         root->is_computable=true;
    else root->is_computable=false;
    if(root->children.size()==1){
        COPY_EXP_NODE(re, root);
        return;
    }else{
    	 ir::Operand op1=ir::Operand(re->v,re->t);
    	ir::Operand des1=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
        scnum++;
        	ir::Operand des10=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
        if(re->t==ir::Type::Int)
    	nowfunc->addInst(new ir::Instruction(op1, ir::Operand(), des1, ir::Operator::mov));
    	else if(re->t==ir::Type::IntLiteral)
    	   {
    	   	nowfunc->addInst(new ir::Instruction(op1, ir::Operand(), des1, ir::Operator::def));
		   }
		else {
		nowfunc->addInst(new ir::Instruction(op1, ir::Operand("0.0",ir::Type::FloatLiteral), des10, ir::Operator::fneq));
				nowfunc->addInst(new ir::Instruction(des10, ir::Operand(), des1, ir::Operator::cvt_f2i));
		}
        ir::Operand des2=ir::Operand("1",ir::Type::IntLiteral);
    	ir::Operand is_=des1;
    	int num=nowfunc->InstVec.size();
        LOrExp* re1=dynamic_cast<LOrExp*>(root->children[2]);
    com_LOrExp(re1);ir::Operand op2=ir::Operand(re1->v,re1->t);
ir::Operand des3=ir::Operand("t"+std::to_string(scnum),ir::Type::Int);
    	scnum++;
    		ir::Operand des11=ir::Operand("t"+std::to_string(scnum),ir::Type::Float);
    	scnum++;
    	if(re1->t==ir::Type::Int)
    	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), des3, ir::Operator::mov));
    	else if(re1->t==ir::Type::IntLiteral)
    	   {
    	   	nowfunc->addInst(new ir::Instruction(op2, ir::Operand(), des3, ir::Operator::def));
		   }
		else {
		nowfunc->addInst(new ir::Instruction(op2, ir::Operand("0.0",ir::Type::FloatLiteral), des11, ir::Operator::fneq));
		nowfunc->addInst(new ir::Instruction(des11, ir::Operand(), des3, ir::Operator::cvt_f2i));
    }
        nowfunc->addInst(new ir::Instruction(des1, des3,des1, ir::Operator::_or));
        COPY_tv_nt_NODE(des1, root);
		root->is_computable=false;
nowfunc->InstVec.insert(nowfunc->InstVec.begin()+num,new ir::Instruction(is_, ir::Operand(), ir::Operand(std::to_string(nowfunc->InstVec.size()-num+1),ir::Type::IntLiteral), ir::Operator::_goto));
       
        return;
    }
    
}


