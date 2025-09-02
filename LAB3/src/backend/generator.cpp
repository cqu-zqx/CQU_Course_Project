#include "backend/generator.h"
#include <cstring>
#include <assert.h>
#include <vector>

#define TODO assert(0 && "todo")
using namespace std;
int w, w1;
int P1 = 10, P2 = 10;
int t = 0;
int wi = 0;
std::set<std::string> globalvs;
backend::stackVarMap stack_;
backend::stackVarMap stack_1;
std::vector<rv::rv_inst> insts;
std::vector<string> floats;
std::map<int, std::string> labels;
std::map<int, std::vector<int>> j_;
std::vector<int> instrs;
backend::Generator::Generator(ir::Program &p, std::ofstream &f) : program(p), fout(f) {}

void backend::Generator::gen_instr(const ir::Instruction &instr)
{

    switch (instr.op)
    {
    case ir::Operator::_return:
    {
        if (instr.op1.type == ir::Type::Int)
        {
            int flag = 0;
            for (auto j : globalvs)
            {
                if (j == instr.op1.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (instr.op1.name)));

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X10, 0));
            }
            else
            {
                int arr = stack_.find_operand(instr.op1);
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
            }
        }
        else if (instr.op1.type == ir::Type::Float)
        {
            int flag = 0;
            for (auto j : globalvs)
            {
                if (j == instr.op1.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (instr.op1.name)));
                insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F10, rv::rvREG::X10, 0));
            }
            else
            {
                int arr = stack_.find_operand(instr.op1);
                insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F10, rv::rvREG::X2, arr));
            }
        }
        else if (instr.op1.type == ir::Type::IntLiteral)
        {
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X10, std::stoi(instr.op1.name)));
        }
        else if (instr.op1.type == ir::Type::FloatLiteral)
        {

            ir::Operand op1 = instr.op1;
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name.find('.') != string::npos)
            {
                string s = op1.name;
                for (int p = 0; p < op1.name.size() - 1; p++)
                {
                    if (op1.name[p] == '.')
                        break;
                    else
                    {
                        if (op1.name[p + 1] != '.' && op1.name[p] == '0')
                            s.erase(p, 1);
                    }
                }
                op1.name = s;
            }
            else
            {
            }
            floats.push_back(op1.name);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, "VALUES" + std::to_string(floats.size() - 1)));

            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F10, rv::rvREG::X10, 0));
        }
        else
        {
        }

        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X1, rv::rvREG::X2,
                                    w1));

        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X2, rv::rvREG::X2, 0));

        insts.push_back(rv::rv_inst(rv::rvOPCODE::JR, rv::rvREG::X1));

        break;
    }

    case ir::Operator::call:
    {

        for (auto param : ((ir::CallInst &)instr).argumentList)
        {
            if (param.type == ir::Type::Int)
            {
                int flag = 0;
                for (auto j : globalvs)
                {
                    if (j == param.name)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                {
                    if (P1 < 31)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG(P1), (param.name)));

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG(P1), rv::rvREG(P1), 0));
                        P1++;
                    }
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG(P1), (param.name)));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG(P1), rv::rvREG(P1), 0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG(P2), rv::rvREG(P1), 1));
                        P2++;
                    }
                }
                else
                {
                    int arr = stack_.find_operand(param);
                    if (P1 < 31)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG(P1), rv::rvREG::X2, arr));
                        P1++;
                    }
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG(P1), rv::rvREG::X2, arr));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG(P2), rv::rvREG(P1), 1));
                        P2++;
                    }
                }
            }
            else if (param.type == ir::Type::IntPtr || param.type == ir::Type::FloatPtr)
            {
                int flag = 0;
                for (auto j : globalvs)
                {
                    if (j == param.name)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                {
                    if (P1 < 31)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG(P1++), (param.name)));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG(P1), (param.name)));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG(P2++), rv::rvREG(P1), 1));
                    }
                }
                else
                {
                    int arr = stack_.find_operand(param);
                    if (P1 < 31)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG(P1++), rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG(P1), rv::rvREG::X2, arr));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG(P2++), rv::rvREG(P1), 1));
                    }
                }
            }
            else if (param.type == ir::Type::Float)
            {
                int flag = 0;
                for (auto j : globalvs)
                {
                    if (j == param.name)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X31, (param.name)));

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG(P2), rv::rvREG::X31, 0));
                }
                else
                {
                    int arr = stack_.find_operand(param);
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG(P2), rv::rvREG::X2, arr));
                }

                P2++;
            }
            else if (param.type == ir::Type::IntLiteral)
            {
                if (P1 < 31)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG(P1++), UINT32(param.name)));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG(P1), UINT32(param.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG(P2++), rv::rvREG(P1), 1));
                }
            }
            else
            {

                floats.push_back(instr.op1.name);
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X4, "VALUES" + std::to_string(floats.size() - 1)));

                insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG(P2++), rv::rvREG::X4, 0));
            }
        }

        insts.push_back(rv::rv_inst(rv::rvOPCODE::CALL, instr.op1.name));

        if (instr.des.type == ir::Type::Int)
        {

            int flag = 0;
            for (auto j : globalvs)
            {
                if (j == instr.des.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (instr.des.name)));

                insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X11, 0, rv::rvREG::X10));
            }
            else
            {
                if (stack_._table.find(instr.des) != stack_._table.end())
                {
                    int arr = stack_.find_operand(instr.des);

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr, rv::rvREG::X10));
                }
                else
                {

                    stack_.add_operand(instr.des);
                    int arr = stack_.find_operand(instr.des);
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr, rv::rvREG::X10));
                }
            }
        }
        else if (instr.des.type == ir::Type::Float)
        {
            int flag = 0;
            for (auto j : globalvs)
            {
                if (j == instr.des.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (instr.des.name)));

                insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X11, 0, rv::rvFREG::F10));
            }
            else
            {
                if (stack_._table.find(instr.des) != stack_._table.end())
                {
                    int arr = stack_.find_operand(instr.des);
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr, rv::rvFREG::F10));
                }
                else
                {
                    stack_.add_operand(instr.des);
                    int arr = stack_.find_operand(instr.des);
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr, rv::rvFREG::F10));
                }
            }
        }

        P1 = 10;
        P2 = 10;
        break;
    }

    case ir::Operator::fdef:
    {

        int flag = 0;

        int flag1 = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        std::string op_name = des.name;
        if (op1.type == ir::Type::FloatLiteral)
        {
            ir::Operand op1 = instr.op1;
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name.find('.') != string::npos)
            {
                string s = op1.name;
                for (int p = 0; p < op1.name.size() - 1; p++)
                {
                    if (op1.name[p] == '.')
                        break;
                    else
                    {
                        if (op1.name[p + 1] != '.' && op1.name[p] == '0')
                            s.erase(p, 1);
                    }
                }
                op1.name = s;
            }
            else
            {
            }
            floats.push_back(op1.name);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, "VALUES" + std::to_string(floats.size() - 1)));
            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F14, rv::rvREG::X10, 0));
            flag1 = 1;
        }
        int ci = 0;
        for (auto operand : {op1, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }

            if (flag == 1)
            {
                if (ci == 1)
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, operand.name));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F14, rv::rvREG::X10, 0));
                }
                else
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X11, 0, rv::rvFREG::F14));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F14, rv::rvREG::X2, arr));
                    }
                    else
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr, rv::rvFREG::F14));
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                    {
                    }
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr, rv::rvFREG::F14));
                    }
                }
            }
        }
        break;
    }

    case ir::Operator::add:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (stack_._table.find(operand) != stack_._table.end())
            {
                int arr = stack_.find_operand(operand);
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    int arr1 = stack_.find_operand(operand);
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                }
            }

            else
            {
                if (flag == 1)
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));

                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));

                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::fadd:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FADD, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FADD, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FADD, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::sub:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::fsub:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        int flag1 = 0;
        int flag2 = 0;
        if (op1.type == ir::Type::FloatLiteral)
        {
            ir::Operand op1 = instr.op1;
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name.find('.') != string::npos)
            {
                string s = op1.name;
                for (int p = 0; p < op1.name.size() - 1; p++)
                {
                    if (op1.name[p] == '.')
                        break;
                    else
                    {
                        if (op1.name[p + 1] != '.' && op1.name[p] == '0')
                            s.erase(p, 1);
                    }
                }
                op1.name = s;
            }
            else
            {
            }
            floats.push_back(op1.name);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, "VALUES" + std::to_string(floats.size() - 1)));
            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
            flag1 = 1;
        }
        if (op2.type == ir::Type::FloatLiteral)
        {
            ir::Operand op2 = instr.op2;
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name.find('.') != string::npos)
            {
                string s = op2.name;
                for (int p = 0; p < op2.name.size() - 1; p++)
                {
                    if (op2.name[p] == '.')
                        break;
                    else
                    {
                        if (op2.name[p + 1] != '.' && op2.name[p] == '0')
                            s.erase(p, 1);
                    }
                }
                op2.name = s;
            }
            else
            {
            }
            floats.push_back(op2.name);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, "VALUES" + std::to_string(floats.size() - 1)));
            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::mul:
    {

        int flag = 0, flag1 = 0, flag2 = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;

        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::fmul:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FMUL, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FMUL, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FMUL, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::div:
    {

        int flag = 0;
        int ci = 0;
        int flag1 = 0;
        int flag2 = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::DIV, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::DIV, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::DIV, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::fdiv:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FDIV, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FDIV, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FDIV, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::mod:
    {

        int flag = 0;
        int ci = 0;
        int flag1 = 0;
        int flag2 = 0;

        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::REM, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::REM, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::REM, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::eq:
    {

        int flag = 0, flag1 = 0, flag2 = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }
    case ir::Operator::feq:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::neq:
    {

        int flag = 0, flag1 = 0, flag2 = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        if (op2.name == "0.0")
        {
            floats.push_back(op2.name);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, "VALUES" + std::to_string(floats.size() - 1)));
            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F10, rv::rvREG::X10, 0));
            insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X12, rv::rvFREG::F10, 1));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SUB, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::fneq:
    {

        int flag = 0;
        int ci = 0;
        int flag1 = 0;
        int flag2 = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op2.type == ir::Type::FloatLiteral)
        {
            floats.push_back(op2.name);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, "VALUES" + std::to_string(floats.size() - 1)));
            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));

            flag2 = 1;
        }
        if (op1.type == ir::Type::FloatLiteral)
        {
            floats.push_back(op1.name);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, "VALUES" + std::to_string(floats.size() - 1)));
            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));

            flag1 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X31, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F11, rv::rvREG::X31, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FADD, rv::rvFREG::F10, rv::rvFREG::F10, rv::rvFREG::F11));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X31, 1));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F11, rv::rvREG::X31, 1));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FADD, rv::rvFREG::F10, rv::rvFREG::F10, rv::rvFREG::F11));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X31, 1));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F11, rv::rvREG::X31, 1));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FADD, rv::rvFREG::F10, rv::rvFREG::F10, rv::rvFREG::F11));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::addi:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;

        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
        }
        for (ir::Operand operand : {op1, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));

                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X10, rv::rvREG::X11, UINT32(op2.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X10, rv::rvREG::X11, UINT32(op2.name)));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X10, rv::rvREG::X11, UINT32(op2.name)));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::subi:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
        }
        for (ir::Operand operand : {op1, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));

                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X10, rv::rvREG::X11, 0 - UINT32(op2.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X10, rv::rvREG::X11, 0 - UINT32(op2.name)));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X10, rv::rvREG::X11, 0 - UINT32(op2.name)));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::mov:
    case ir::Operator::def:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        int flag1 = 0;
        int flag2 = 0;

        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));

                else
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X11));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X11));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X11));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::fmov:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F14, rv::rvREG::X10, 0));

                else
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F14));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F14, rv::rvREG::X2, arr));

                    else
                    {

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F14));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F14, rv::rvREG::X2, arr));

                    else
                    {

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F14));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::lss:
    {

        int flag = 0;
        int ci = 0;
        int flag1 = 0;
        int flag2 = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::flss:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLT, rv::rvREG::X10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLT, rv::rvREG::X10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLT, rv::rvREG::X10, rv::rvFREG::F11, rv::rvFREG::F12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::geq:
    {

        int flag = 0, flag1 = 0, flag2 = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }
    case ir::Operator::fgeq:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F11, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }
    case ir::Operator::leq:
    {

        int flag = 0, flag1 = 0, flag2 = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X12, rv::rvREG::X11));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X12, rv::rvREG::X11));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X12, rv::rvREG::X11));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }
    case ir::Operator::fleq:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F12, rv::rvFREG::F11));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F12, rv::rvFREG::F11));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F12, rv::rvFREG::F11));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F10, rv::rvFREG::F12));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::XORI, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::gtr:
    {

        int flag = 0, flag1 = 0, flag2 = 0;
        int ci = 0;

        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        if (op1.type == ir::Type::IntLiteral)
        {
            if (op1.name[0] == '0' && op1.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0' && op1.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op1.name[i] >= 'A' && op1.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'A' + 10));
                    }
                    else if (op1.name[i] >= 'a' && op1.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op1.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else if (op1.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op1.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op1.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op1.name[i] - '0'));
                }
                op1.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X11, UINT32(op1.name)));
            flag1 = 1;
        }
        if (op2.type == ir::Type::IntLiteral)
        {
            if (op2.name[0] == '0' && op2.name[1] == 'b')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 2 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0' && op2.name[1] == 'x')
            {
                int p = 0;
                for (int i = 2; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 16 * q;
                    }
                    if (op2.name[i] >= 'A' && op2.name[i] <= 'F')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'A' + 10));
                    }
                    else if (op2.name[i] >= 'a' && op2.name[i] <= 'f')
                    {
                        p = p + q * stoi(to_string(op2.name[i] - 'a' + 10));
                    }
                    else
                        p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else if (op2.name[0] == '0')
            {
                int p = 0;
                for (int i = 1; i <= op2.name.size() - 1; i++)
                {
                    int q = 1;
                    for (int j = 0; j < op2.name.size() - 1 - i; j++)
                    {
                        q = 8 * q;
                    }
                    p = p + q * stoi(to_string(op2.name[i] - '0'));
                }
                op2.name = to_string(p);
            }
            else
            {
            }
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(op2.name)));
            flag2 = 1;
        }
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            if (ci == 1 && flag1 == 1 || ci == 2 && flag2 == 1)
                continue;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X12, rv::rvREG::X11));

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X12, rv::rvREG::X11));

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvREG::X10, rv::rvREG::X12, rv::rvREG::X11));

                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }
    case ir::Operator::fgtr:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X10, 0));
                else
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F12, rv::rvFREG::F11));

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F12, rv::rvFREG::F11));

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X2, arr));
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SLT, rv::rvFREG::F10, rv::rvFREG::F12, rv::rvFREG::F11));

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X10, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }
    case ir::Operator::cvt_i2f:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));

                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X11, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X14, 0, rv::rvFREG::F10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X11, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X11, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr1, rv::rvFREG::F10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::cvt_f2i:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X10, 0));
                    floats.push_back("0.01");
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X31, "VALUES" + std::to_string(floats.size() - 1)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F13, rv::rvREG::X31, 0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::FSUB, rv::rvFREG::F11, rv::rvFREG::F11, rv::rvFREG::F13));
                }

                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F11, 1));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    }
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F11, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F11, rv::rvREG::X2, arr));
                    }
                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG::X10, rv::rvFREG::F11, 1));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::_not:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));

                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X0));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));

                    else
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SEQZ, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X0));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::_or:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X12, rv::rvREG::X12, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::OR, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X12, rv::rvREG::X12, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::OR, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X12, rv::rvREG::X12, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::OR, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::_and:
    {

        int flag = 0;
        int ci = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        for (ir::Operand operand : {op1, op2, des})
        {
            ci++;
            for (auto j : globalvs)
            {
                if (j == operand.name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {

                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X10, (operand.name)));
                if (ci == 1)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X10, 0));
                else if (ci == 2)
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X10, 0));
                else
                {
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X12, rv::rvREG::X12, rv::rvREG::X0));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::AND, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X14, (operand.name)));
                    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X14, 0, rv::rvREG::X10));
                }
            }
            else
            {
                if (stack_._table.find(operand) != stack_._table.end())
                {
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X12, rv::rvREG::X12, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::AND, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
                else
                {
                    stack_.add_operand(operand);
                    int arr = stack_.find_operand(operand);
                    if (ci == 1)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SNEZ, rv::rvREG::X12, rv::rvREG::X12, rv::rvREG::X0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::AND, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                        int arr1 = stack_.find_operand(operand);
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                    }
                }
            }
            flag = 0;
        }
        break;
    }

    case ir::Operator::alloc:
    {

        string a;
        if (stoi(instr.op1.name) > 50)
            a = "50";
        else
            a = instr.op1.name;
        stack_.add_operand(instr.des, UINT32(a) * 4 + 4);
        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X10, rv::rvREG::X2, stack_.find_operand(instr.des) + 4));
        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, stack_.find_operand(instr.des), rv::rvREG::X10));
        break;
    }

    case ir::Operator::_goto:
    {

        int flag = 0;
        ir::Operand op1 = instr.op1;
        ir::Operand op2 = instr.op2;
        ir::Operand des = instr.des;
        std::string op_name = op1.name;
        if (op_name == "null")
        {
            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X10, 1));
        }
        else
        {

            for (auto j : globalvs)
            {
                if (j == op_name)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (op_name)));
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X11, 0));
            }
            else
            {
                int arr = stack_.find_operand(op1);
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
            }
        }

        insts.push_back(rv::rv_inst(rv::rvOPCODE::BNE, rv::rvREG::X10, rv::rvREG::X0, ""));

        j_[wi + std::stoi(instr.des.name)].push_back(insts.size() - 1);

        break;
    }

    case ir::Operator::store:
    {

        if (instr.op1.type == ir::Type::IntPtr)
        {
            int flag = 0;
            int ci = 0;
            ir::Operand op1 = instr.op1;
            ir::Operand op2 = instr.op2;
            ir::Operand des = instr.des;
            int flag1 = 0;
            int flag2 = 0;
            if (op2.type == ir::Type::IntLiteral)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X10, UINT32(op2.name) * 4));
                flag1 = 1;
            }
            if (des.type == ir::Type::IntLiteral)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, UINT32(des.name)));
                flag2 = 1;
            }

            for (ir::Operand operand : {op2, des, op1})
            {
                ci++;
                if (ci == 1 && flag1 == 1)
                    continue;
                if (ci == 2 && flag2 == 1)
                    continue;
                for (auto j : globalvs)
                {
                    if (j == operand.name)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (operand.name)));
                    if (ci == 1)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X11, 0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X13, 4));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X13));
                    }
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X11, 0));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X11, 0, rv::rvREG::X12));
                    }
                }
                else
                {
                    if (stack_._table.find(operand) != stack_._table.end())
                    {
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X13, 4));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X13));
                        }
                        else if (ci == 2)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X11, 0, rv::rvREG::X12));
                        }
                    }
                    else
                    {
                        stack_.add_operand(operand);
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                        else if (ci == 2)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                            int arr1 = stack_.find_operand(operand);
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                        }
                    }
                }
                flag = 0;
            }
        }
        else
        {
            int flag = 0;
            int ci = 0;
            ir::Operand op1 = instr.op1;
            ir::Operand op2 = instr.op2;
            ir::Operand des = instr.des;
            int flag1 = 0;
            int flag2 = 0;
            if (op2.type == ir::Type::IntLiteral)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X10, UINT32(op2.name) * 4));
                flag1 = 1;
            }
            if (des.type == ir::Type::FloatLiteral)
            {
                floats.push_back(des.name);
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X31, "VALUES" + std::to_string(floats.size() - 1)));
                insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F10, rv::rvREG::X31, 0));

                flag2 = 1;
            }
            if (des.type == ir::Type::IntLiteral)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X31, UINT32(des.name)));
                insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_I2F, rv::rvFREG::F10, rv::rvREG::X31, 1));

                flag2 = 1;
            }
            for (ir::Operand operand : {op2, des, op1})
            {
                ci++;
                if (ci == 1 && flag1 == 1)
                    continue;
                if (ci == 2 && flag2 == 1)
                    continue;
                for (auto j : globalvs)
                {
                    if (j == operand.name)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (operand.name)));
                    if (ci == 1)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X11, 0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                    }
                    else if (ci == 2)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F10, rv::rvREG::X11, 0));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X11, 0, rv::rvFREG::F10));
                    }
                }
                else
                {
                    if (stack_._table.find(operand) != stack_._table.end())
                    {
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                        }
                        else if (ci == 2)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F10, rv::rvREG::X2, arr));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X11, 0, rv::rvFREG::F10));
                        }
                    }
                    else
                    {
                        stack_.add_operand(operand);
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                        else if (ci == 2)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X2, arr));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X10, rv::rvREG::X11, rv::rvREG::X12));
                            int arr1 = stack_.find_operand(operand);
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr1, rv::rvREG::X10));
                        }
                    }
                }
                flag = 0;
            }
        }

        break;
    }

    case ir::Operator::load:
    {

        if (instr.des.type == ir::Type::Int)
        {
            int flag = 0;
            int ci = 0;
            ir::Operand op1 = instr.op1;
            ir::Operand op2 = instr.op2;
            ir::Operand des = instr.des;
            int flag1 = 0;
            int flag2 = 0;
            if (op2.type == ir::Type::IntLiteral)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X10, UINT32(op2.name) * 4));
                flag1 = 1;
            }
            for (ir::Operand operand : {op2, op1, des})
            {
                ci++;
                if (ci == 1 && flag1 == 1)
                    continue;
                for (auto j : globalvs)
                {
                    if (j == operand.name)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (operand.name)));
                    if (ci == 1)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X11, 0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                    }
                    else if (ci == 3)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X11, 0, rv::rvREG::X12));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X11, 0));
                    }
                }
                else
                {
                    if (stack_._table.find(operand) != stack_._table.end())
                    {
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                        }
                        else if (ci == 3)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr, rv::rvREG::X12));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X11, 0));
                        }
                    }
                    else
                    {
                        stack_.add_operand(operand);
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                        }
                        else if (ci == 3)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr, rv::rvREG::X12));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X12, rv::rvREG::X11, 0));
                        }
                    }
                }
                flag = 0;
            }
        }
        else
        {
            int flag = 0;
            int ci = 0;
            ir::Operand op1 = instr.op1;
            ir::Operand op2 = instr.op2;
            ir::Operand des = instr.des;
            int flag1 = 0;
            int flag2 = 0;
            if (op2.type == ir::Type::IntLiteral)
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X10, UINT32(op2.name) * 4));
                flag1 = 1;
            }
            for (ir::Operand operand : {op2, op1, des})
            {
                ci++;
                if (ci == 1 && flag1 == 1)
                    continue;
                for (auto j : globalvs)
                {
                    if (j == operand.name)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1)
                {

                    insts.push_back(rv::rv_inst(rv::rvOPCODE::LA, rv::rvREG::X11, (operand.name)));
                    if (ci == 1)
                    {

                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X11, 0));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                    }
                    else if (ci == 3)
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X11, 0, rv::rvFREG::F12));
                    else
                    {
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                        insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X11, 0));
                    }
                }
                else
                {
                    if (stack_._table.find(operand) != stack_._table.end())
                    {
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                        }
                        else if (ci == 3)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr, rv::rvFREG::F12));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X11, 0));
                        }
                    }
                    else
                    {
                        stack_.add_operand(operand);
                        int arr = stack_.find_operand(operand);
                        if (ci == 1)
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X10, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LI, rv::rvREG::X12, 4));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::MUL, rv::rvREG::X10, rv::rvREG::X10, rv::rvREG::X12));
                        }
                        else if (ci == 3)
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr, rv::rvFREG::F12));
                        else
                        {

                            insts.push_back(rv::rv_inst(rv::rvOPCODE::LW, rv::rvREG::X11, rv::rvREG::X2, arr));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::ADD, rv::rvREG::X11, rv::rvREG::X11, rv::rvREG::X10));
                            insts.push_back(rv::rv_inst(rv::rvOPCODE::FLW, rv::rvFREG::F12, rv::rvREG::X11, 0));
                        }
                    }
                }
                flag = 0;
            }
        }
        break;
    }

    case ir::Operator::__unuse__:
    {

        insts.push_back(rv::rv_inst(rv::rvOPCODE::NOP, rv::rvREG::X2, ""));
        break;
    }

    default:
    {

        assert(0);
    }
    }
}

void backend::Generator::gen_func(const ir::Function &function)
{

    auto Q = ir::Operand("123", ir::Type::Int);
    stack_.add_operand(Q);
    w1 = stack_.delt - 4;
    insts.push_back(rv::rv_inst(rv::rvOPCODE::ADDI, rv::rvREG::X2, rv::rvREG::X2, 0));

    insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, w1, rv::rvREG::X1));

    for (int i = 0; i < function.ParameterList.size(); i++)
    {
        if (function.ParameterList[i].type == ir::Type::Int || function.ParameterList[i].type == ir::Type::IntPtr || function.ParameterList[i].type == ir::Type::FloatPtr)
        {
            stack_.add_operand(function.ParameterList[i]);
            int arr = stack_.find_operand(function.ParameterList[i]);
            if (P1 < 31)
                insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr, rv::rvREG(P1++)));
            else
            {
                insts.push_back(rv::rv_inst(rv::rvOPCODE::CVT_F2I, rv::rvREG(P1), rv::rvFREG(P2++), 1));
                insts.push_back(rv::rv_inst(rv::rvOPCODE::SW, rv::rvREG::X2, arr, rv::rvREG(P1)));
            }
        }
        else
        {

            stack_.add_operand(function.ParameterList[i]);
            int arr = stack_.find_operand(function.ParameterList[i]);
            insts.push_back(rv::rv_inst(rv::rvOPCODE::FSW, rv::rvREG::X2, arr, rv::rvFREG(P2++)));
        }
    }
    P1 = 10;
    P2 = 10;
    int all_ = insts.size();

    for (int i = 0; i < function.InstVec.size(); i++)
    {
        instrs.push_back(insts.size());

        gen_instr(*function.InstVec[i]);
        wi++;
    }

    insts[0].imm = (-stack_.delt);

    for (int i = 0; i < function.InstVec.size(); i++)
    {
        if (j_.find(i) != j_.end())
        {

            labels[i] = "label" + std::to_string(t);
            for (auto idx : j_[i])
            {
                insts[idx].label = "label" + std::to_string(t);
            }
            t++;
        }
    }
    for (int i = 0; i < function.InstVec.size() - 1; i++)
    {
        if (function.InstVec[i]->op == ir::Operator::_return)
        {
            insts[instrs[i + 1] - 2].imm = stack_.delt;
        }
    }
    if (function.InstVec[function.InstVec.size() - 1]->op == ir::Operator::_return)
        insts[insts.size() - 2].imm = stack_.delt;
    std::vector<rv::rv_inst> al;

    for (int i = 0; i < all_; i++)
        al.push_back(insts[i]);

    for (int i = 0; i < function.InstVec.size() - 1; i++)
    {

        if (labels.find(i) != labels.end())
        {

            al.push_back(rv::rv_inst(rv::rvOPCODE::LABEL, labels[i]));
        }
        for (int s = instrs[i]; s < instrs[i + 1]; s++)
            al.push_back(insts[s]);
    }
    if (labels.find(function.InstVec.size() - 1) != labels.end())
    {

        al.push_back(rv::rv_inst(rv::rvOPCODE::LABEL, labels[function.InstVec.size() - 1]));
    }
    for (int s = instrs[function.InstVec.size() - 1]; s < insts.size(); s++)
        al.push_back(insts[s]);
    for (auto inst : al)
    {
        fout << inst.draw();
    }

    labels.clear();
    instrs.clear();
    j_.clear();
    insts.clear();
    stack_ = stack_1;
}
void backend::Generator::glo()
{

    fout << "   .bss\n";
    fout << "   .align 4\n";
    for (auto x : program.globalVal)
    {
        if (x.maxlen == 0)
            x.maxlen = 1;
        fout << (x.val.name) + ":   .space " + std::to_string(x.maxlen * 4) + "\n";

        globalvs.insert(x.val.name);
    }
    fout << "   .text\n";
}
void backend::Generator::gen()
{

    glo();
    fun();
    FLOAT1();
}
void backend::Generator::fun()
{

    for (auto f : program.functions)
    {
        fout << "   .global   " + f.name + "\n";
        fout << f.name << ":\n";
        gen_func(f);
        wi = 0;
    }
}
void backend::Generator::FLOAT1()
{

    for (int i = 0; i < floats.size(); i++)
    {
        fout << "VALUES" + std::to_string(i) + ":\n   .float  " + floats[i] + "\n";
    }
}
std::string rv::toString1(rv::rvOPCODE r)
{
    switch (r)
    {
    case rv::rvOPCODE::ADD:
    {
        return "add";
        break;
    }
    case rv::rvOPCODE::SUB:
    {
        return "sub";
        break;
    }
    case rv::rvOPCODE::XOR:
    {
        return "xor";
        break;
    }
    case rv::rvOPCODE::OR:
    {
        return "or";
        break;
    }
    case rv::rvOPCODE::AND:
    {
        return "and";
        break;
    }
    case rv::rvOPCODE::SLL:
    {
        return "sll";
        break;
    }

    case rv::rvOPCODE::SLT:
    {
        return "slt";
        break;
    }

    case rv::rvOPCODE::ADDI:
    {
        return "addi";
        break;
    }
    case rv::rvOPCODE::XORI:
    {
        return "xori";
        break;
    }

    case rv::rvOPCODE::LW:
    {
        return "lw";
        break;
    }
    case rv::rvOPCODE::SW:
    {
        return "sw";
        break;
    }

    case rv::rvOPCODE::BNE:
    {
        return "bne";
        break;
    }

    case rv::rvOPCODE::MUL:
    {
        return "mul";
        break;
    }
    case rv::rvOPCODE::DIV:
    {
        return "div";
        break;
    }
    case rv::rvOPCODE::REM:
    {
        return "rem";
        break;
    }
    case rv::rvOPCODE::FLW:
    {
        return "flw";
        break;
    }
    case rv::rvOPCODE::FSW:
    {
        return "fsw";
        break;
    }
    case rv::rvOPCODE::FADD:
    {
        return "fadd.s";
        break;
    }
    case rv::rvOPCODE::FSUB:
    {
        return "fsub.s";
        break;
    }
    case rvOPCODE::FMUL:
    {
        return "fmul.s";
        break;
    }
    case rvOPCODE::FDIV:
    {
        return "fdiv.s";
        break;
    }
    case rvOPCODE::CVT_I2F:
    {
        return "fcvt.s.w";
        break;
    }
    case rvOPCODE::CVT_F2I:
    {
        return "fcvt.w.s";
        break;
    }

    case rvOPCODE::FLT:
    {
        return "flt.s";
        break;
    }

    case rvOPCODE::LA:
    {
        return "la";
        break;
    }
    case rvOPCODE::LI:
    {
        return "li";
        break;
    }
    case rvOPCODE::MOV:
    {
        return "mv";
        break;
    }

    case rvOPCODE::JR:
    {
        return "jr";
        break;
    }
    case rvOPCODE::SEQZ:
    {
        return "seqz";
        break;
    }
    case rvOPCODE::SNEZ:
    {
        return "snez";
        break;
    }
    case rvOPCODE::CALL:
    {
        return "call";
        break;
    }
    case rvOPCODE::NOP:
    {
        return "nop";
        break;
    }
    }
}

std::string rv::toString1(rv::rvREG r)
{
    switch (r)
    {
    case rvREG::X0:
    {
        return "zero";
        break;
    }
    case rvREG::X1:
    {
        return "ra";
        break;
    }
    case rvREG::X2:
    {
        return "sp";
        break;
    }
    case rvREG::X3:
    {
        return "gp";
        break;
    }
    case rvREG::X4:
    {
        return "tp";
        break;
    }
    case rvREG::X5:
    {
        return "t0";
        break;
    }
    case rvREG::X6:
    {
        return "t1";
        break;
    }
    case rvREG::X7:
    {
        return "t2";
        break;
    }
    case rvREG::X8:
    {
        return "s0";
        break;
    }
    case rvREG::X9:
    {
        return "s1";
        break;
    }
    case rvREG::X10:
    {
        return "a0";
        break;
    }
    case rvREG::X11:
    {
        return "a1";
        break;
    }
    case rvREG::X12:
    {
        return "a2";
        break;
    }
    case rvREG::X13:
    {
        return "a3";
        break;
    }
    case rvREG::X14:
    {
        return "a4";
        break;
    }
    case rvREG::X15:
    {
        return "a5";
        break;
    }
    case rvREG::X16:
    {
        return "a6";
        break;
    }
    case rvREG::X17:
    {
        return "a7";
        break;
    }
    case rvREG::X18:
    {
        return "s2";
        break;
    }
    case rvREG::X19:
    {
        return "s3";
        break;
    }
    case rvREG::X20:
    {
        return "s4";
        break;
    }
    case rvREG::X21:
    {
        return "s5";
        break;
    }
    case rvREG::X22:
    {
        return "s6";
        break;
    }
    case rvREG::X23:
    {
        return "s7";
        break;
    }
    case rvREG::X24:
    {
        return "s8";
        break;
    }
    case rvREG::X25:
    {
        return "s9";
        break;
    }
    case rvREG::X26:
    {
        return "s10";
        break;
    }
    case rvREG::X27:
    {
        return "s11";
        break;
    }
    case rvREG::X28:
    {
        return "t3";
        break;
    }
    case rvREG::X29:
    {
        return "t4";
        break;
    }
    case rvREG::X30:
    {
        return "t5";
        break;
    }
    case rvREG::X31:
    {
        return "t6";
        break;
    }
    }
}

std::string rv::toString1(rv::rvFREG r)
{
    switch (r)
    {
    case rvFREG::F0:
    {
        return "ft0";
        break;
    }
    case rvFREG::F1:
    {
        return "ft1";
        break;
    }
    case rvFREG::F2:
    {
        return "ft2";
        break;
    }
    case rvFREG::F3:
    {
        return "ft3";
        break;
    }
    case rvFREG::F4:
    {
        return "ft4";
        break;
    }
    case rvFREG::F5:
    {
        return "ft5";
        break;
    }
    case rvFREG::F6:
    {
        return "ft6";
        break;
    }
    case rvFREG::F7:
    {
        return "ft7";
        break;
    }
    case rvFREG::F8:
    {
        return "fs0";
        break;
    }
    case rvFREG::F9:
    {
        return "fs1";
        break;
    }
    case rvFREG::F10:
    {
        return "fa0";
        break;
    }
    case rvFREG::F11:
    {
        return "fa1";
        break;
    }
    case rvFREG::F12:
    {
        return "fa2";
        break;
    }
    case rvFREG::F13:
    {
        return "fa3";
        break;
    }
    case rvFREG::F14:
    {
        return "fa4";
        break;
    }
    case rvFREG::F15:
    {
        return "fa5";
        break;
    }
    case rvFREG::F16:
    {
        return "fa6";
        break;
    }
    case rvFREG::F17:
    {
        return "fa7";
        break;
    }
    case rvFREG::F18:
    {
        return "fs2";
        break;
    }
    case rvFREG::F19:
    {
        return "fs3";
        break;
    }
    case rvFREG::F20:
    {
        return "fs4";
        break;
    }
    case rvFREG::F21:
    {
        return "fs5";
        break;
    }
    case rvFREG::F22:
    {
        return "fs6";
        break;
    }
    case rvFREG::F23:
    {
        return "fs7";
        break;
    }
    case rvFREG::F24:
    {
        return "fs8";
        break;
    }
    case rvFREG::F25:
    {
        return "fs9";
        break;
    }
    case rvFREG::F26:
    {
        return "fs10";
        break;
    }
    case rvFREG::F27:
    {
        return "fs11";
        break;
    }
    case rvFREG::F28:
    {
        return "ft3";
        break;
    }
    case rvFREG::F29:
    {
        return "ft4";
        break;
    }
    case rvFREG::F30:
    {
        return "ft5";
        break;
    }
    case rvFREG::F31:
    {
        return "ft6";
        break;
    }
    }
}
std::string rv::rv_inst::draw() const
{
    switch (op)
    {
    case rvOPCODE::ADD:
    case rvOPCODE::SUB:
    case rvOPCODE::XOR:
    case rvOPCODE::OR:
    case rvOPCODE::AND:
    case rvOPCODE::SLT:
    case rvOPCODE::MUL:
    case rvOPCODE::DIV:
    case rvOPCODE::REM:
    {
        return toString1(op) + " " + toString1(rd) + "," + toString1(rs1) + "," + toString1(rs2) + "\n";
    }

    case rv::rvOPCODE::FADD:
    case rv::rvOPCODE::FSUB:
    case rv::rvOPCODE::FMUL:
    case rv::rvOPCODE::FDIV:
    {
        return toString1(op) + " " + toString1(frd) + "," + toString1(frs1) + "," + toString1(frs2) + "\n";
    }

    case rv::rvOPCODE::FLT:
    {
        return toString1(op) + " " + toString1(rd) + "," + toString1(frs1) + "," + toString1(frs2) + "\n";
    }

    case rvOPCODE::ADDI:
    case rvOPCODE::XORI:

    {
        return toString1(op) + " " + toString1(rd) + "," + toString1(rs1) + "," + std::to_string(int(imm)) + "\n";
    }

    case rvOPCODE::LW:
    {
        return toString1(op) + " " + toString1(rd) + "," + std::to_string(int(imm)) + "(" + toString1(rs1) + ")" + "\n";
    }

    case rvOPCODE::SW:
    {
        return toString1(op) + " " + toString1(rs2) + "," + std::to_string(int(imm)) + "(" + toString1(rs1) + ")" + "\n";
    }

    case rv::rvOPCODE::FLW:
    {
        return toString1(op) + " " + toString1(frd) + "," + std::to_string(int(imm)) + "(" + toString1(rs1) + ")" + "\n";
    }

    case rv::rvOPCODE::FSW:
    {
        return toString1(op) + " " + toString1(frs2) + "," + std::to_string(int(imm)) + "(" + toString1(rs1) + ")" + "\n";
    }

    case rvOPCODE::BNE:
    {
        return toString1(op) + " " + toString1(rs1) + "," + toString1(rs2) + "," + label + "\n";
    }

    case rvOPCODE::LA:
    {
        return toString1(op) + " " + toString1(rd) + "," + label + "\n";
    }

    case rvOPCODE::LI:
    {
        return toString1(op) + " " + toString1(rd) + "," + std::to_string(int(imm)) + "\n";
    }

    case rvOPCODE::MOV:
    {
        return toString1(op) + " " + toString1(rd) + "," + toString1(rs1) + "\n";
    }

    case rvOPCODE::JR:
    {
        return toString1(op) + " " + toString1(rs1) + "\n";
    }

    case rv::rvOPCODE::CVT_F2I:
    {
        return toString1(op) + " " + toString1(rd) + "," + toString1(frs1) + "\n";
    }

    case rv::rvOPCODE::CVT_I2F:
    {
        return toString1(op) + " " + toString1(frd) + "," + toString1(rs1) + "\n";
    }

    case rv::rvOPCODE::SEQZ:
    case rv::rvOPCODE::SNEZ:
    {
        return toString1(op) + " " + toString1(rd) + "," + toString1(rs1) + "\n";
    }

    case rv::rvOPCODE::CALL:
    {
        return toString1(op) + " " + label + "\n";
    }

    case rv::rvOPCODE::NOP:
    {
        return " nop\n";
    }

    case rv::rvOPCODE::LABEL:
    {
        return label + ":\n";
    }
    }
}
