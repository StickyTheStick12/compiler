#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "Node.h"

class Symbol {
        protected:
        std::string id;
        std::string type;
        std::string symbol;

        public:
        Symbol(const std::string& id, const std::string& type);
        std::string GetId();
        std::string GetType();
        std::string PrintEntry();
};

class Variable : public Symbol {
public:
    Variable(const std::string& name, const std::string& type);
};

class Method : public Symbol {
private:
    std::vector<Variable*> parameters;
    std::unordered_map<std::string, Variable*> variables;

public:
    Method(const std::string& name, const std::string& type);
    void AddParameter(Variable* var);
    Variable* GetParameter(const int idx);
    int GetNumberParameters();
};

class Class : public Symbol {
private:
    std::unordered_map<std::string, Method*> methods;
    std::unordered_map<std::string, Variable*> variables;

public:
    Class(const std::string& name, const std::string& type);
    void AddMethod(Method* method);
    Method* MethodLookup(const std::string& name);
};

class Scope {
private:
    int next = 0;
    Scope* parentScope;
    std::vector<Scope*> childScopes;
    std::unordered_map<std::string, Symbol*> entries;
    std::string scopeName;
    Symbol* scopeSymbol;

public:
    Scope(Scope* parent, const std::string& name, Symbol* symbol);
    void Add(const std::string& key, Symbol* entry);
    Symbol* GetScopeSymbol();
    Scope* Parent();
    Scope* NextChild(const std::string& name, Symbol* symbol);
    Symbol* Lookup(const std::string& key);
    void ResetScope();
    void PrintScope(int& count, ofstream* oStream);
};

class SymbolTable {
private:
    Scope* root;
    Scope* currentScope;

public:
    SymbolTable();
    void EnterScope(const std::string& name, Symbol* symbol);
    void ExitScope();
    void Add(const std::string& key, Symbol* entry);
    Symbol* Lookup(const std::string& key);
    Scope* GetCurrentScope();
    void ResetTable();
    void PrintTable();
};

void TraverseTree(Node* node, SymbolTable* ST);

#endif
