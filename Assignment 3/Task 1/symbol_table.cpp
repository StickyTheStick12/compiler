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

void Scope::AddClass(const std::string& id, Class* entry) {
    classEntries[id] = entry;
}

void Scope::AddMethod(const std::string& id, Method* entry) {
    methodEntries[id] = entry;
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

Class* Scope::ClassLookup(const std::string& key)
{
    auto iter = classEntries.find(key);

    if(iter != classEntries.end())
        return iter->second;

    if(parentScope == nullptr)
        return nullptr;

    return parentScope->ClassLookup(key);
}

Method* Scope::MethodLookup(const std::string& key)
{
    auto iter = methodEntries.find(key);

    if(iter != methodEntries.end())
        return iter->second;

    if(parentScope == nullptr)
        return nullptr;

    return parentScope->MethodLookup(key);
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

void Scope::PrintScope(int& num, std::ofstream* oStream) {

    *oStream << "n" << num << " [label=\"Symbol table: ("<< scopeName << ")\\n";

    for(auto & entry : classEntries)
        *oStream << classEntries[entry.first]->PrintEntry() << "\\n";

    for(auto & entry : methodEntries)
        *oStream << methodEntries[entry.first]->PrintEntry() << "\\n";

    for(auto & entry : varEntries)
        *oStream << varEntries[entry.first]->PrintEntry() << "\\n";

    *oStream << "\"];" << std::endl;

    int id = num;

    for(auto & childScope : childScopes)
    {
        int n = ++num;
        childScope->PrintScope(num, oStream);
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

void SymbolTable::AddClass(const std::string& id, Class* entry) {
    currentScope->AddClass(id, entry);
}

void SymbolTable::AddMethod(const std::string& id, Method* entry) {
    currentScope->AddMethod(id, entry);
}

void SymbolTable::AddVar(const std::string& id, Variable* entry) {
    currentScope->AddVar(id, entry);
}

Method* SymbolTable::MethodLookup(const std::string& key) {
    return currentScope->MethodLookup(key);
}

Class* SymbolTable::ClassLookup(const std::string& key) {
    return currentScope->ClassLookup(key);
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
            if(ST->ClassLookup(iter->value) != nullptr)
                std::cerr << "@error at line " << iter->lineno <<
                          ". A class with name '" << iter->value << "' already exists" << std::endl;
            //std::cerr << "@error at line " << (iter->children.front()->children.front()->lineno - 1) <<
            //". A class with name '" << iter->value << "' already exists" << std::endl;

            auto nClass = new Class(iter->value, iter->value);

            ST->AddClass(iter->value, nClass);
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
        ST->AddMethod("main", new Method("main", "void"));
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
            if(ST->MethodLookup(iter->value) != nullptr)
                std::cerr << "@error at line " << iter->lineno <<
                          ". A method with name '" << iter->value << "' already exists" << std::endl;
            auto nMethod = new Method(iter->value, (*iter->children.front()).value);

            ((Class*)ST->GetCurrentScope()->GetScopeSymbol())->AddMethod(nMethod);

            ST->AddMethod(iter->value, nMethod);

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
