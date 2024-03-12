#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "bytecode.h"
#include <unordered_map>
#include <stack>

#define ILOAD 0
#define ICONST 1
#define ISTORE 2
#define IADD 3
#define ISUB 4
#define IMUL 5
#define IDIV 6
#define ILT 7
#define IEQ 8
#define IAND 9
#define IOR 10
#define INOT 11
#define GOTO 12
#define IFFALSE 13
#define INVOKEVIRTUAL 14
#define IRETURN 15
#define PRINT 16
#define STOP 17

std::vector<std::string> SplitString(std::string input, std::string delimiter);

Program* ReadFile(const std::string& filename);

void Interpret(Program* program);

class Activation {
public:
    BCMethod* method;
    std::unordered_map<std::string, int> variables;
    int pc = 0;
    Activation(BCMethod* method);
    Instruction* GetNextInstruction();
    void Jump(const std::string& block);
};

const std::unordered_map<std::string, int> instructions = {
        {"iload", 0},
        {"iconst", 1},
        {"istore", 2},
        {"iadd", 3},
        {"isub", 4},
        {"imul", 5},
        {"idiv", 6},
        {"ilt", 7},
        {"ieq", 8},
        {"iand", 9},
        {"ior", 10},
        {"inot", 11},
        {"goto", 12},
        {"iffalse", 13},
        {"invokevirtual", 14},
        {"ireturn", 15},
        {"print", 16},
        {"stop", 17}
};

#endif
