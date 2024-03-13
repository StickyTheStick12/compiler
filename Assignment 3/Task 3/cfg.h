#ifndef CFG_H
#define CFG_H
#include "tac.h"
#include "Node.h"
#include <algorithm>
#include <utility>
#include "bytecode.h"

class BBlock {
public:
    static inline int blockCounter = 0;
    static inline int tempCounter = 0;
    std::string name;
    std::vector<Tac*> tacInstructions;
    Tac* condition;
    BBlock* trueExit;
    BBlock* falseExit;

    BBlock() : trueExit(nullptr), falseExit(nullptr), condition(nullptr), name(GenBlockName()) {}
    BBlock(std::string name) : trueExit(nullptr), falseExit(nullptr), condition(nullptr), name(std::move(name)) {}
    void GenerateCode(BCMethod* method);

    static std::string GenTempName();

    static std::string GenBlockName();
    static void GenerateByteCode(Program* program);
};

extern BBlock* currentBlock;
extern std::vector<BBlock*> methods;
extern std::vector<std::string> renderedBlocks;

std::string TraverseTreeTac(SymbolTable* ST, Node* node);
void CreateBlockCfg(BBlock* block, std::ofstream *oStream);
void CreateCfg();

#endif
