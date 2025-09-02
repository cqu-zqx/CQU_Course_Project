/**
 * @file semantic.h
 * @author Yuntao Dai (d1581209858@live.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-06
 * 
 * a Analyzer should 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include"ir/ir.h"
#include"front/abstract_syntax_tree.h"

#include<map>
#include<string>
#include<vector>
using std::map;
using std::string;
using std::vector;

namespace frontend
{

// definition of symbol table entry
struct STE {
    ir::Operand operand;
    vector<int> dimension;
};

using map_str_ste = map<string, STE>;
// definition of scope infomation
struct ScopeInfo {
    int cnt;
    string name;
    map_str_ste table;
};

// surpport lib functions
map<std::string,ir::Function*>* get_lib_funcs();

// definition of symbol table
struct SymbolTable{
    vector<ScopeInfo> scope_stack;
    map<std::string,ir::Function*> functions;

    /**
     * @brief enter a new scope, record the infomation in scope stacks
     * @param node: a Block node, entering a new Block means a new name scope
     */
    void add_scope();

    /**
     * @brief exit a scope, pop out infomations
     */
    void exit_scope();

    /**
     * @brief Get the scoped name, to deal the same name in different scopes, we change origin id to a new one with scope infomation,
     * for example, we have these code:
     * "     
     * int a;
     * {
     *      int a; ....
     * }
     * "
     * in this case, we have two variable both name 'a', after change they will be 'a' and 'a_block'
     * @param id: origin id 
     * @return string: new name with scope infomations
     */
    string get_scoped_name(string id) const;
    
   
    /**
     * @brief get the right operand with the input name
     * @param id identifier name
     * @return Operand 
     */
    ir::Operand get_operand(string id) const;

    /**
     * @brief get the right ste with the input name
     * @param id identifier name
     * @return STE 
     */
    STE get_ste(string id) const;
};


// singleton class
struct Analyzer {
    int tmp_cnt;
    vector<ir::Instruction*> g_init_inst;
    SymbolTable symbol_table;
    ir::Program *program;

    /**
     * @brief constructor
     */
    Analyzer();



    // analysis functions
    ir::Program get_ir_program(CompUnit*);

    // reject copy & assignment
    Analyzer(const Analyzer&) = delete;
    Analyzer& operator=(const Analyzer&) = delete;

    void start1(CompUnit*);
    void end1();
   
    // define the analyze function
   
    void com_CompUnit(AstNode* root);
    void com_Decl(AstNode* root);
    void com_LAndExp(LAndExp* root);
    void com_LOrExp(LOrExp *root);
     std::vector<ir::Operand> com_ConstInitVal(ConstInitVal* root);
    void com_VarDecl(VarDecl* root);
    void com_VarDef(AstNode* root, ir::Type);
    std::vector<ir::Operand> com_InitVal(InitVal* root);
    void com_FuncDef(AstNode* root);
    ir::Operand com_FuncFParam(FuncFParam* root);
    void com_ConstExp(ConstExp* root);
    void com_MulExp(MulExp* root);
    std::vector<ir::Operand> com_FuncFParams(FuncFParams * root);
    void com_BlockItem(BlockItem* root);
    int com_Stmt(Stmt* root);
    void com_PrimaryExp(PrimaryExp *root);
    void com_UnaryExp(UnaryExp* root);
      void com_Exp(Exp* root);
    void com_Cond(Cond* root);
    std::vector<ir::Operand> com_LVal(LVal* root);
    void com_Number(Number* root);
     void com_AddExp(AddExp* root);
    void com_ConstDecl(ConstDecl* root);
    void com_ConstDef(AstNode* root, ir::Type);
    void com_RelExp(RelExp* root);
    void com_EqExp(EqExp* root);
    std::vector<ir::Operand> com_FuncRParams(FuncRParams* root);
    
  
  
};

} // namespace frontend

#endif
