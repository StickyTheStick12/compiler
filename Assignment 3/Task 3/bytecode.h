#ifndef BYTECODE_H
#define BYTECODE_H

#include "symbol_table.h"
#include <map>

class Instruction {
public:
    int id;
    std::string argument;
    void Print(std::ofstream* outStream) const;
};

class MethodBlock {
public:
    std::string name;
    std::vector<Instruction*> instructions;
    void Print(std::ofstream* outSteam) const;
};

class BCMethod {
public:
    std::vector<Variable> variables;
    std::vector<MethodBlock*> methodBlocks;
    void Print(std::ofstream* outStream) const;
};


class Program {
public:
    std::map<std::string, BCMethod*> methods;
    void Print() const;
};

#endif
