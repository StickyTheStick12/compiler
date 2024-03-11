#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symbol_table.h"
#include <utility>

std::string TraverseTreeSemantic(SymbolTable* ST, Node* node);

#endif
