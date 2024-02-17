#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symbol_table.h"

std::string TraverseTreeSemantic(SymbolTable* ST, Node* node);

int Semantic_analysis(SymbolTable* ST, Node* root);

#endif
