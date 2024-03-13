#include "bytecode.h"

void Instruction::Print(std::ofstream* outStream) const {
    std::string instructionName;
    switch(id) {
        case 0:
            instructionName = "iload";
            break;
        case 1:
            instructionName = "iconst";
            break;
        case 2:
            instructionName = "istore";
            break;
        case 3:
            instructionName = "iadd";
            break;
        case 4:
            instructionName = "isub";
            break;
        case 5:
            instructionName = "imul";
            break;
        case 6:
            instructionName = "idiv";
            break;
        case 7:
            instructionName = "ilt";
            break;
        case 8:
            instructionName = "ieq";
            break;
        case 9:
            instructionName = "iand";
            break;
        case 10:
            instructionName = "ior";
            break;
        case 11:
            instructionName = "inot";
            break;
        case 12:
            instructionName = "goto";
            break;
        case 13:
            instructionName = "iffalse";
            break;
        case 14:
            instructionName = "invokevirtual";
            break;
        case 15:
            instructionName = "ireturn";
            break;
        case 16:
            instructionName = "print";
            break;
        case 17:
            instructionName = "stop";
            break;
    }
    *outStream << "    " << instructionName << " " << argument << std::endl;
}

void MethodBlock::Print(std::ofstream* outStream) const {
    *outStream << name << ":" << std::endl;
    for (Instruction* i : instructions) {
        i->Print(outStream);
    }
}

void BCMethod::Print(std::ofstream* outStream) const {
    for (MethodBlock* i : methodBlocks) {
        i->Print(outStream);
    }
}

Instruction* BCMethod::GetInstruction(int pc) {
    int i = 0;
    MethodBlock* currentBlock;
    while (pc >= 0) {
        currentBlock = methodBlocks[i++];
        pc -= currentBlock->instructions.size();
    }
    return currentBlock->instructions[currentBlock->instructions.size() + pc];
}

int BCMethod::GetPcValue(const std::string& block) {
    int i = 0;
    int pc = 0;
    MethodBlock* currentBlock = methodBlocks[i++];
    while (currentBlock->name != block){
        pc += currentBlock->instructions.size();
        currentBlock = methodBlocks[i++];

    }
    return pc;
}

void Program::Print() const
{
    std::ofstream outStream;
    outStream.open("bytecode.bc");

    for(std::pair<std::string, BCMethod*> pair : methods) {
        pair.second->Print(&outStream);
    }
    std::cout << "Wrote bytecode to bytecode.bc" << std::endl;
    outStream.close();
}
