#include "symbol_table.h"

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

Variable::Variable(const std::string& name, const std::string& type) : Symbol(name, type, "Variable") {}

bool Variable::IsDefined() {
    return isDefined;
}

Method::Method(const std::string& name, const std::string& type) : Symbol(name, type, "Method") {}

void Method::AddParameter(Variable* var) {
    parameters.push_back(var);
}

Variable* Method::GetParameter(const int idx) {
    return parameters[idx];
}

int Method::GetNumberParameters() {
    return parameters.size();
}

Class::Class(const std::string& name, const std::string& type) : Symbol(name, type, "Class") {}

void Class::AddMethod(Method* method) {
    methods[method->GetId()] = method;
}

void Class::AddVariable(Variable* variable)
{
    variables[variable->GetId()] = variable;
}

Method* Class::MethodLookup(const std::string& name)
{
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
    entries[id] = entry;
}

void Scope::AddVar(const std::string &id, Variable* entry) {
    varEntries[id] = entry;
}

Symbol* Scope::GetScopeSymbol() {
    return scopeSymbol;
}

Scope* Scope::Parent() {
    return parentScope;
}

Scope* Scope::NextChild(const std::string& name, Symbol* symbol)
{
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

Symbol* Scope::Lookup(const std::string& key)
{
    auto iter = entries.find(key);

    if(iter != entries.end())
        return iter->second;

    if(parentScope == nullptr)
        return nullptr;

    return parentScope->Lookup(key);
}

Variable* Scope::VarLookup(const std::string& key)
{
    auto iter = varEntries.find(key);

    if(iter != varEntries.end())
        return iter->second;

    if(parentScope == nullptr)
        return nullptr;

    return parentScope->VarLookup(key);
}

Variable* Scope::VarLookupCurScope(const std::string& key)
{
    auto iter = varEntries.find(key);

    if(iter != varEntries.end())
        return iter->second;

    return nullptr;
}

void Scope::ResetScope() {
    next = 0;

    for(auto & childScope : childScopes)
        childScope->ResetScope();
}

void Scope::PrintScope(int& num, ofstream* oStream) {

    *oStream << "n" << num << " [label=\"Symbol table: ("<< scopeName << ")\\n";

    for(auto & entry : entries)
        *oStream << entries[entry.first]->PrintEntry() << "\\n";

    *oStream << "\"];" << endl;

    int id = num;

    for(auto & childScope : childScopes)
    {
        int n = ++num;
        childScope->PrintScope(num, oStream);
        *oStream << "n" << id << " -> n" << n << endl;
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

void SymbolTable::AddVar(const std::string& id, Variable* entry) {
    currentScope->AddVar(id, entry);
}

Symbol* SymbolTable::Lookup(const std::string& key) {
    return currentScope->Lookup(key);
}

Variable* SymbolTable::VarLookup(const std::string& key) {
    return currentScope->VarLookup(key);
}

Variable* SymbolTable::VarLookupCurScope(const std::string& key) {
    return currentScope->VarLookupCurScope(key);
}

Scope* SymbolTable::GetCurrentScope() {
    return currentScope;
}

void SymbolTable::PrintTable()
{
    std::ofstream oStream;
    oStream.open("ST.dot");

    int num = 0;
    oStream << "digraph {" << std::endl;
    root->PrintScope(num, &oStream);
    oStream << "}" << std::endl;
    oStream.close();

    std::cout << "Built a symbol-tree at ST.dot. Use 'make st' to generate the pdf version." << std::endl;
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
                std::cerr << "@error at line " << iter->lineno <<
                          ". A class with name '" << iter->value << "' already exists" << std::endl;
                //std::cerr << "@error at line " << (iter->children.front()->children.front()->lineno - 1) <<
                          //". A class with name '" << iter->value << "' already exists" << std::endl;

            auto nClass = new Class(iter->value, iter->value);

            ST->Add(iter->value, nClass);
            ST->EnterScope("Class: " + iter->value, nClass);
            TraverseTree(iter, ST);
            ST->ExitScope();
        }
    }
    else if(node->type == "Class")
    {
        //the type will be the class name
        ST->AddVar("this", new Variable("this", node->value));

        for(auto & iter : node->children)
            TraverseTree(iter, ST);
    }
    else if(node->type == "Main Class")
    {
        //the type will be the class name
        ST->AddVar("this", new Variable("this", node->value));
        ST->Add("main", new Method("main", "void"));
        ST->EnterScope("Method: main", nullptr);
        ST->AddVar(node->children.front()->value, new Variable(node->children.front()->value, "String[]"));
        ST->ExitScope();
    }
    else if(node->type == "Class body" || node->type == "Class variables" || node->type == "Method" ||
            node->type == "Method body" || node->type == "Method variables" || node->type == "Main class statement" ||
            node->type == "MethodList" || node->type == "Method statement")
    {
        for(auto & iter : node->children)
            TraverseTree(iter, ST);
    }
    else if(node->type == "MethodDecList")
    {
        for(auto & iter : node->children)
        {
            if(ST->Lookup(iter->value) != nullptr)
                std::cerr << "@error at line " << iter->lineno <<
                          ". A method with name '" << iter->value << "' already exists" << std::endl;
            auto nMethod = new Method(iter->value, (*iter->children.front()).value);

            ((Class*)ST->GetCurrentScope()->GetScopeSymbol())->AddMethod(nMethod);

            ST->Add(iter->value, nMethod);

            ST->EnterScope("Method: " + iter->value, nMethod);
            TraverseTree(iter, ST);
            ST->ExitScope();
        }
    }
    else if (node->type == "Variable") {
        Node* type = *node->children.begin();
        Node* name = *std::prev(node->children.end());

        if (ST->VarLookupCurScope(name->value) != nullptr)
            std::cerr << "@error at line " << node->lineno <<
                  ". A variable with name '" << name->value << "' already exists" << std::endl;

        ST->AddVar(name->value, new Variable(name->value, type->value));
    }
    else if(node->type == "Method parameters")
    {
        for(auto & child : node->children)
        {
            TraverseTree(child, ST);
            Node* name = *(std::prev(child->children.end()));

            Variable* new_param = ST->VarLookup(name->value);
            ((Method*)ST->GetCurrentScope()->GetScopeSymbol())->AddParameter(new_param);
        }
    }
    else if(node->type == "Assignment")
    {
        if (ST->VarLookup(node->children.front()->value) == nullptr)
            std::cerr << "@error at line " << node->lineno <<
                      ". Variable with name '" << node->children.front()->value << "' doesnt exists before assignment" << std::endl;
    }
}
