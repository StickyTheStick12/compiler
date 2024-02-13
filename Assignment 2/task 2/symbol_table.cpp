#include "symbol_table.h"
#include "semantic.h"

Symbol::Symbol(const std::string& id, const std::string& type, const std::string& symbol)
{
    this->id = id;
    this->type = type;
    this->symbol = symbol;
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

Variable::Variable(const std::string& name, const std::string& type) : Symbol(name, type, "Variable") {
}

Method::Method(const std::string& name, const std::string& type) : Symbol(name, type, "Method") {
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

Class::Class(const std::string& name, const std::string& type) : Symbol(name, type, "Class") {
}

void Class::AddMethod(Method* method) {
    methods[method->GetId()] = method;
}

Method* Class::MethodLookup(const std::string& name) {
    auto iter = methods.find(name);

    if(iter != methods.end())
        return iter->second;

    return nullptr;
}

Scope::Scope(Scope* parent, const std::string& name, Symbol* symbol) {
    parentScope = parent;
    scopeName = name;
    scopeSymbol = symbol;
}

void Scope::Add(const std::string& id, Symbol* entry) {
    // Function to add a symbol to the scope
    entries[id] = entry;
}

Symbol* Scope::GetScopeSymbol() {
    // Function to get the symbol associated with the scope, Class or method
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
    auto iter = entries.find(key);

    if(iter != entries.end())
        return iter->second;

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

void SymbolTable::Add(const std::string& id, Symbol* entry) {
    currentScope->Add(id, entry);
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
            TraverseTree(iter, ST);
    }
    else if(node->type == "Main Class")
    {
        ST->Add("this", new Variable("this", node->value));
        ST->Add("main", new Method("main", "void"));
        ST->EnterScope("Method: main", nullptr);
        ST->Add(node->children.front()->value, new Variable(node->children.front()->value, "String[]"));
        ST->ExitScope();
    }
    else if(node->type == "Class body" || node->type == "Class variables" || node->type == "Method" ||
    node->type == "Method body" || node->type == "Method variables")
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

            auto nMethod = new Method(iter->value, (*iter->children.begin())->value);
            ST->Add(iter->value, nMethod);

            ((Class*)ST->GetCurrentScope()->GetScopeSymbol())->AddMethod(nMethod);

            ST->EnterScope("Method: " + iter->value, nMethod);
            TraverseTree(iter, ST);
            ST->ExitScope();
        }
    }
    else if (node->type == "Variable") {
        Node* type = (*node->children.begin());
        Node* name = std::next(type);

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
            Node* name = std::prev(*iter->children.end());
            auto nParameter = (Variable*)ST->Lookup(name->value);
            ((Method*)ST->GetCurrentScope()->GetScopeSymbol())->AddParameter(nParameter);
        }
    }
}
