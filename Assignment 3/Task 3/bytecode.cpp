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
            instructionName = "iand";
            break;
        case 9:
            instructionName = "ior";
            break;
        case 10:
            instructionName = "inot";
            break;
        case 11:
            instructionName = "goto";
            break;
        case 12:
            instructionName = "iffalse goto";
            break;
        case 13:
            instructionName = "invokevirtual";
            break;
        case 14:
            instructionName = "ireturn";
            break;
        case 15:
            instructionName = "print";
            break;
        case 16:
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
