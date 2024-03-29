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
    Symbol(const std::string& id, const std::string& type, const std::string& symbol);
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

public:
    Method(const std::string& name, const std::string& type);
    void AddParameter(Variable* var);
    Variable* GetParameter(const int idx);
    int GetNumberParameters();
};

class Class : public Symbol {
private:
    std::unordered_map<std::string, Method*> methods;

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
    std::unordered_map<std::string, Class*> classEntries;
    std::unordered_map<std::string, Method*> methodEntries;
    std::unordered_map<std::string, Variable*> varEntries;
    std::string scopeName;
    Symbol* scopeSymbol;

public:
    Scope(Scope* parent, const std::string& name, Symbol* symbol);
    void AddClass(const std::string& id, Class* entry);
    void AddMethod(const std::string& id, Method* entry);
    void AddVar(const std::string& id, Variable* entry);
    Symbol* GetScopeSymbol();
    std::string GetScopeName();
    Scope* Parent();
    Scope* NextChild(const std::string& name, Symbol* symbol);
    Class* ClassLookup(const std::string& key);
    Method* MethodLookup(const std::string& key);
    Variable* VarLookup(const std::string& key);
    Variable* VarLookupCurScope(const std::string& key);
    void ResetScope();
    void PrintScope(int& count, std::ofstream* oStream);
};

class SymbolTable {
private:
    Scope* root;
    Scope* currentScope;

public:
    SymbolTable();
    void EnterScope(const std::string& name, Symbol* symbol);
    void ExitScope();
    void AddClass(const std::string& id, Class* entry);
    void AddMethod(const std::string& id, Method* entry);
    void AddVar(const std::string& id, Variable* entry);
    Class* ClassLookup(const std::string& key);
    Method* MethodLookup(const std::string& key);
    Variable* VarLookup(const std::string& key);
    Variable* VarLookupCurScope(const std::string& key);
    Scope* GetCurrentScope();
    void ResetTable();
    void PrintTable();
};

void TraverseTree(Node* node, SymbolTable* ST);

#endif
