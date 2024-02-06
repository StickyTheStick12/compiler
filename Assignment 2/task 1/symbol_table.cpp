#include "symbol_table.h"

Symbol::Symbol(const std::string& id, const std::string& type)
{
    this->id = id;
    this->type = type;
}

std::string Symbol::GetId() {
    return id;
}

std::string Symbol::GetType() {
    return type;
}

std::string Symbol::PrintEntry() {
    return "name " + id + "; symbol " + symbol + "; type " + type;
}

Variable::Variable(const std::string& name, const std::string& type) : Symbol(name, type) {
    symbol = "Variable";
}

Method::Method(const std::string& name, const std::string& type) : Symbol(name, type) {
    symbol = "Method";
}

void Method::AddParameter(Variable* var) {
    parameters.push_back(var);
}

Variable* Method::GetParameter(const int idx) {
    return parameters[idx];
}

int Method::GetNumberParameters() {
    return parameters.size();
}

Class::Class(const std::string& name, const std::string& type) : Symbol(name, type) {
    symbol = "Class";
}

void Class::AddMethod(Method* method) {
    methods[method->GetId()] = method;
}

Method* Class::MethodLookup(const std::string& name) {
    if(methods.find(name) != methods.end())
        return methods[name];

    return nullptr;
}

Scope::Scope(Scope* parent, const std::string& name, Symbol* symbol) {
    parentScope = parent;
    scopeName = name;
    scopeSymbol = symbol;
}

void Scope::Add(const std::string& key, Symbol* entry) {
    entries[key] = entry;
}

Symbol* Scope::GetScopeSymbol() {
    return scopeSymbol;
}

Scope* Scope::Parent() {
    return parentScope;
}

Scope* Scope::NextChild(const std::string& name, Symbol* symbol) {
    Scope* nextChild;

    if(next == childScopes.size())
    {
        nextChild = new Scope(this, name, symbol);
        childScopes.push_back(nextChild);
    }
    else
        nextChild = childScopes[next];

    next++;
    return nextChild;
}

Symbol* Scope::Lookup(const std::string& key) {
    if(entries.find(key) != entries.end())
        return entries[key];

    //if we cant find it, check parent
    if(parentScope == nullptr)
        return nullptr;

    return parentScope->Lookup(key);
}

void Scope::ResetScope() {
    next = 0;

    for(auto & childScope : childScopes)
        childScope->ResetScope();
}

void Scope::PrintScope(int& count, ofstream* oStream) {
    int id = count;

    *oStream << "n" << id << " [label=\"Symbol table: (" << scopeName << ")\\n";

    for (auto it = entries.begin(); it != entries.end(); ++it)
        *oStream << entries[it->first]->PrintEntry() << "\\n";

    *oStream << "\"];" << std::endl;

    for (auto & childScope : childScopes) {
        int n = ++count;
        childScope->PrintScope(count, oStream);
        *oStream << "n" << id << " -> n" << n << std::endl;
    }
}

SymbolTable::SymbolTable() {
    root = new Scope(nullptr, "Program", nullptr);
    currentScope = root;
}

void SymbolTable::EnterScope(const std::string& name, Symbol* symbol)
{
    currentScope = currentScope->NextChild(name, symbol);
}

void SymbolTable::ExitScope() {
    currentScope = currentScope->Parent();
}

void SymbolTable::Add(const std::string& key, Symbol* entry) {
    currentScope->Add(key, entry);
}

Symbol* SymbolTable::Lookup(const std::string& key) {
    return currentScope->Lookup(key);
}

Scope* SymbolTable::GetCurrentScope() {
    return currentScope;
}

void SymbolTable::PrintTable()
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

void SymbolTable::ResetTable() {
    root->ResetScope();
}

void TraverseTree(Node* node, SymbolTable* ST)
{
    if(node->type == "Program")
    {
        for(auto iter : node->children)
        {
            if(ST->Lookup(iter->value) != nullptr)
                std::cout << "An error occurred when creating class " << iter->value << ". A class with this name "
                                                                                        "already exists" << std::endl;

            auto nClass = new Class(iter->value, iter->value);
            ST->Add(iter->value, nClass);
            ST->EnterScope("Class: " + iter->value, nClass);
            TraverseTree(iter, ST);
            ST->ExitScope();
        }
    }
    else if(node->type == "Class")
    {
        ST->Add("this", new Variable("this", node->value));

        for(auto & iter : node->children)
        {
            TraverseTree(iter, ST);
        }
    }
    else if(node->type == "Main Class")
    {
        ST->Add("this", new Variable("this", node->value));
        ST->Add("main", new Method("main", "void"));
        ST->EnterScope("Method: main", nullptr);
        ST->Add(node->children.front()->value, new Variable(node->children.front()->value, "String[]"));
        ST->ExitScope();
    }
    else if(node->type == "Class Body" || node->type == "class variables" || node->type == "Method" || node->type == "Method body" || node->type == "Method variables")
    {
        for(auto & iter : node->children)
            TraverseTree(iter, ST);
    }
    else if(node->type == "MethodDecList")
    {
        for(auto & iter : node->children)
        {
            if(ST->Lookup(iter->value) != nullptr)
                std::cout << "An error occurred when creating method " << iter->value << ". A method with this name "
                                                                                        "already exists" << std::endl;

            Node* methodType = *iter->children.begin();
            auto nMethod = new Method(iter->value, methodType->value);
            ST->Add(iter->value, nMethod);

            ((Class*)ST->GetCurrentScope()->GetScopeSymbol())->AddMethod(nMethod);

            ST->EnterScope("Method: " + iter->value, nMethod);
            TraverseTree(iter, ST);
            ST->ExitScope();
        }
    }
    else if (node->type == "Variable") {
        Node* type = (*node->children.begin());
        auto endIdx = node->children.end();
        Node* name = (*(--endIdx));

        if(ST->Lookup(name->value) != nullptr)
        {
            std::cout << "An error occurred when creating variable " << name->value << ". A variable with this name "
                                                                                     "already exists" << std::endl;
        }

        ST->Add(name->value, new Variable(name->value, type->value));
    }
    else if(node->type == "Method parameters")
    {
        for(auto & iter : node->children)
        {
            TraverseTree(iter, ST);
            auto endIdx = iter->children.end();
            Node* name = *(--endIdx);
            auto nParameter = (Variable*)ST->Lookup(name->value);
            ((Method*)ST->GetCurrentScope()->GetScopeSymbol())->AddParameter(nParameter);
        }
    }
}
