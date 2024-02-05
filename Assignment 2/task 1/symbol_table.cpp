#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "Node.h"

class Entry
{
protected:
    std::string id;
    std::string type;
    std::string symbol;

public:
    Entry(const std::string& id, const std::string& type)
    {
        this->id = id;
        this->type = type;
    }

    void SetId(const std::string& id)
    {
        this->id = id;
    }

    std::string GetId()
    {
        return this->id;
    }

    void SetType(const std::string& type)
    {
        this->type = std::move(type);
    }

    std::string GetType()
    {
        return this->type;
    }

    std::string PrintEntry()
    {
        return "name " + id + "; symbol " + symbol + "; type " + type;
    }
};

class Variable : public Entry
{
public:
    Variable(const std::string& name, const std::string& type) : Entry(name, type)
    {
        symbol = "Variable";
    }
};

class Method : public Entry
{
    std::unordered_map<std::string, Variable*> parameters;
    std::unordered_map<std::string, Variable*> variables;

public:
    Method(const std::string& name, const std::string& type) : Entry(name, type)
    {
        symbol = "Method";
    }

    void AddVariable(Variable* var)
    {
        variables[var->GetId()] = var;
    }

    void AddMethod(Variable* var)
    {
        parameters[var->GetId()] = var;
    }
};

class Class : public Entry
{
    std::unordered_map<std::string, Method*> methods;
    std::unordered_map<std::string, Variable*> variables;

public:
    Class(const std::string& name, const std::string& type) : Entry(name, type)
    {
        symbol = "Class";
    }

    void AddVariable(Variable* var)
    {
        variables[var->GetId()] = var;
    }

    void AddMethod(Method* method)
    {
        methods[method->GetId()] = method;
    }

    Variable* VarLookup(const std::string& name)
    {
        if(variables.find(name) != variables.end())
            return variables[name];

        return nullptr;
    }

    Method* MethodLookup(const std::string name)
    {
        if(methods.find(name) != methods.end())
            return methods[name];

        return nullptr;
    }
};

class Scope
{
    int next = 0;
    Scope* parentScope;
    std::vector<Scope*> childScopes;
    std::unordered_map<std::string, Entry*> entries;
    std::string scopeName;

public:
    Scope(Scope* parent, const std::string& name)
    {
        parentScope = parent;
        scopeName = name;
    }

    void Add(const std::string& key, Entry* entry)
    {
        entries[key] = entry;
    }

    Scope* Parent()
    {
        return parentScope;
    }

    Scope* NextChild(const std::string& name)
    {
        Scope* nextChild;

        if(next == childScopes.size())
        {
            nextChild = new Scope(this, name);
            childScopes.push_back(nextChild);
        }
        else
        {
            nextChild = childScopes.at(next);
        }

        next++;
        return nextChild;
    }

    Entry* Lookup(const std::string& key)
    {
        if(entries.find(key) != entries.end())
            return entries[key];

        if(parentScope == nullptr)
            return nullptr;

        return parentScope->Lookup(key);
    }

    void ResetScope()
    {
        next = 0;
        for(int i = 0; i < childScopes.size(); i++)
            childScopes.at(i)->ResetScope();
    }

    void PrintScope(int& count, ofstream* oStream) {
        int id = count;

        *oStream << "n" << id << " [label=\"Symbol table: (" << scopeName << ")\\n";

        for (auto it = entries.begin(); it != entries.end(); ++it)
            *oStream << entries[it->first]->PrintEntry() << "\\n";

        *oStream << "\"];" << std::endl;

        for (auto it = childScopes.begin(); it != childScopes.end(); ++it) {
            int n = ++count;
            (*it)->PrintScope(count, oStream);
            *oStream << "n" << id << " -> n" << n << std::endl;
        }
    }
};

class SymbolTable
{
Scope* root;
Scope* currentScope;

public:
    SymbolTable()
    {
        root = new Scope(nullptr, "Program");
        currentScope = root;
    }

    void EnterScope(const std::string& name = NULL)
    {
        currentScope = currentScope->NextChild(name);
    }

    void ExitScope()
    {
        currentScope = currentScope->Parent();
    }

    void Add(const std::string& key, Entry* entry)
    {
        currentScope->Add(key, entry);
    }

    Entry* Lookup(const std::string& key)
    {
        return currentScope->Lookup(key);
    }

    void ResetTable()
    {
        root->ResetScope();
    }

    void PrintTable()
    {
        std::ofstream oStream;
        oStream.open("symboltree.dot");

        int count = 0;
        oStream << "digraph {" << std::endl;
        root->PrintScope(count, &oStream);
        oStream << "}" << std::endl;
        oStream.close();

        std::cout << "\nBuilt new symbol tree:" << std::endl;
    }
};

void TraverseTree(Node* node, SymbolTable* ST)
{
    if(node->type == "Program")
    {
        for(auto iter = node->children.begin(); iter != node->children.end(); ++iter)
        {
            ST->Add((*iter)->value, new Class((*iter)->value, (*iter)->value));
            ST->EnterScope("Class: " + (*iter)->value);
            TraverseTree(*iter, ST);
            ST->ExitScope();
        }
    }
    else if(node->type == "Class")
    {
        ST->Add("this", new Variable("this", node->value));

        for(auto iter = node->children.begin(); iter != node->children.end(); ++iter)
        {
            TraverseTree(*iter, ST);
        }
    }
    else if(node->type == "Main Class")
    {
        ST->Add("this", new Variable("this", node->value));
        ST->Add("main", new Method("main", "void"));
        ST->EnterScope("Method: main");
        ST->Add(node->children.front()->value, new Variable(node->children.front()->value, "String[]"));
        ST->ExitScope();
    }
    else if(node->type == "Class Body")
    {
        for(auto iter = node->children.begin(); iter != node->children.end(); ++iter)
            TraverseTree(*iter, ST);
    }
    else if (node->type == "class variables")
    {
        for(auto iter = node->children.begin(); iter != node->children.end(); ++iter)
            TraverseTree(*iter, ST);
    }
    else if(node->type == "MethodDecList")
    {
        for(auto iter = node->children.begin(); iter != node->children.end(); ++iter)
        {
            Node* type = *(*iter)->children.begin();
            ST->Add((*iter)->value, new Method((*iter)->value, type->value));
            ST->EnterScope("Method: " + (*iter)->value);
            TraverseTree(*iter, ST);
            ST->ExitScope();
        }
    }
    else if (node->type == "Method") {
        for (auto iter = node->children.begin(); iter !=node->children.end(); ++iter)
        {
            TraverseTree(*iter, ST);
        }
    }
    else if (node->type == "Method parameters") {
        for (auto iter = node->children.begin(); iter != node->children.end(); ++iter) {
            TraverseTree(*iter, ST);
        }
    }
    else if (node->type == "Method body") {
        for (auto iter = node->children.begin(); iter != node->children.end(); ++iter) {
            TraverseTree(*iter, ST);
        }
    }
    else if (node->type == "Method variables") {
        for (auto iter = node->children.begin(); iter != node->children.end(); ++iter) {
            TraverseTree(*iter, ST);
        }
    }
    else if (node->type == "Variable") {
        Node* type = (*node->children.begin());
        auto endIdx = node->children.end();
        Node* name = (*(--endIdx));
        ST->Add(name->value, new Variable(name->value, type->value));
    }
}
