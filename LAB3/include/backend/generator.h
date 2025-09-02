#ifndef GENERARATOR_H
#define GENERARATOR_H

#include "ir/ir.h"
#include "backend/rv_def.h"
#include "backend/rv_inst_impl.h"

#include<map>
#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<set>


#define UINT32(content) uint32_t(std::stoi(content))

namespace backend {


struct cmd
{
    bool operator()(ir::Operand op1, ir::Operand op2){
        return op1.name < op2.name;
    }
};

// it is a map bewteen variable and its mem addr, the mem addr of a local variable can be identified by ($sp + off)
struct stackVarMap {
    std::map<ir::Operand, int, cmd> _table;
    uint32_t delt = 0;
	
    /**
     * @brief find the addr of a ir::Operand
     * @return the offset
    */

int find_operand(ir::Operand operand){
    
    return _table[operand];
}

int add_operand(ir::Operand operand, uint32_t size=4){
  
    _table[operand] = delt;
    delt += size;
    
    return _table[operand];
}
    /**
     * @brief add a ir::Operand into current map, alloc space for this variable in memory 
     * @param[in] size: the space needed(in byte)
     * @return the offset
    */
   
};


struct Generator {
    const ir::Program& program;         // the program to gen
    std::ofstream& fout;                 // output file
    
    Generator(ir::Program&, std::ofstream&);

    // reg allocate api
    rv::rvREG getRd(ir::Operand);
    rv::rvFREG fgetRd(ir::Operand);
    rv::rvREG getRs1(ir::Operand);
    rv::rvREG getRs2(ir::Operand);
    rv::rvFREG fgetRs1(ir::Operand);
    rv::rvFREG fgetRs2(ir::Operand);

    // generate wrapper function
    void gen();
    void gen_func(const ir::Function&);
    void gen_instr(const ir::Instruction&);
    void glo();
    void fun();
    void FLOAT1();
};



} // namespace backend


#endif