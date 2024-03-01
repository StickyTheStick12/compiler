#ifndef CFG_H
#define CFG_H
#include "tac.h"
#include "symbol_table.h"
#include "Node.h"
#include <algorithm>

class BBlock {
public:
    int blockCounter = 0;
    static inline int tempCounter = 0;
    std::string name;
    std::vector<Tac*> tacInstructions;
    Tac* condition;
    BBlock* trueExit;
    BBlock* falseExit;

    BBlock() : trueExit(nullptr), falseExit(nullptr), condition(nullptr), name(GenBlockName()) {}

    static std::string GenTempName();

    std::string GenBlockName();
};

extern BBlock* currentBlock;
extern std::vector<BBlock*> methods;
extern std::vector<std::string> renderedBlocks;

std::string TraverseTreeTac(SymbolTable* ST, Node* node);
void CreateBlockCfg(BBlock* block, std::ofstream *oStream);
void CreateCfg(BBlock* block);

#endif
