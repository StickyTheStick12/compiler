#include "semantic.h"

std::string TraverseTreeSemantic(SymbolTable* ST, Node* node)
{
    if(node->type == "Program")
    {
        for(auto & iter : node->children)
        {
            ST->EnterScope("", nullptr);
            TraverseTreeSemantic(ST, iter);
            ST->ExitScope();
        }
        return "void";
    }
    else if(node->type == "Class" || node->type == "Class body" || node->type == "Method statement" || node->type == "Statements" || node->type == "Main class statement" || node->type == "MethodList" || node->type == "Class variables" )
    {
        for(auto & iter : node->children)
            TraverseTreeSemantic(ST, iter);

        return "void";
    }
    else if(node->type == "Main Class")
    {
        ST->EnterScope("", nullptr);
        for(auto iter = std::next(node->children.begin()); iter != node->children.end(); ++iter)
            TraverseTreeSemantic(ST, *iter);

        ST->ExitScope();
        return "void";
    }
    else if(node->type == "MethodDecList")
    {
        for(auto & iter : node->children) {
            ST->EnterScope("", nullptr);
            TraverseTreeSemantic(ST, iter);
            ST->ExitScope();
        }

        return "void";
    }
    else if(node->type == "Method")
    {
        for(auto iter = node->children.begin(); iter != std::prev(node->children.end()); ++iter)
            TraverseTreeSemantic(ST, *iter);

        std::string returnType = TraverseTreeSemantic(ST, node->children.back());

        if(node->children.front()->value != returnType)
            std::cerr << "@error at line " << node->children.back()->lineno <<
            ". Return type of method doesnt match type of returned value. Cannot convert "<< returnType <<
            " to " << node->children.front()->value << "  " << std::endl;

        return node->children.front()->value;
    }
    else if(node->type == "Method body")
    {
        for(auto iter = node->children.begin(); iter != std::prev(node->children.end()); ++iter)
            TraverseTreeSemantic(ST, *iter);

        return TraverseTreeSemantic(ST, node->children.back());
    }
    else if(node->type == "If statement" || node->type == "If else" || node->type == "While loop")
    {
        for(auto iter = std::next(node->children.begin()); iter != node->children.end(); ++iter)
            TraverseTreeSemantic(ST, *iter);

        std::string lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "bool")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". If and while require a bool. Cannot convert " << lhs << " to a bool" << std::endl;

        return "void";
    }
    else if(node->type == "Assignment")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs == "-")
            return "void";

        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(rhs == "-")
            return "void";

        if(lhs != rhs)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". " << lhs << " type does not match with " << rhs << " type" << std::endl;

        return "void";
    }
    else if(node->type == "Array assignment")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::string idx = TraverseTreeSemantic(ST, *(std::next(node->children.begin())));
        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int[]")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". the type of the expression must be an int array but is a " << lhs << std::endl;
        else if(idx != "int")
            std::cerr << "@error at line " << (*(std::next(node->children.begin())))->lineno <<
                      ". the type of the expression must be an int but is a " << idx << std::endl;
        else if(rhs != "int")
            std::cerr << "@error at line " << node->children.back()->lineno <<
                      ". Cannot convert " << rhs << " to a integer" <<  std::endl;

        return "void";
    }
    else if(node->type == "Print")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());

        if (lhs != "int" && lhs != "bool")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Cannot convert " << lhs << " to an int" << endl;

        return "void";
    }
    else if(node->type == "or expression" || node->type == "and expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "bool" || rhs != "bool")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". unsupported operands for 'or' or 'and' expression. Can't convert " << lhs << " or "
                                                                             << rhs << " to a bool" << std::endl;
        return "bool";
    }
    else if(node->type == "EQ expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != rhs)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". == requires that both types are the same." << lhs << " and " << rhs << " is not" << std::endl;

        return "bool";
    }
    else if(node->type == "LT expression" || node->type == "GT expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int" || rhs != "int")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Cannot convert " << lhs << " or " << rhs << " to an int for LT or GT" << std::endl;

        return "bool";
    }
    else if(node->type == "Add expression" || node->type == "Sub expression" || node->type == "Mul expression" ||
            node->type == "Div expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int" || rhs != "int")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Unsupported operand type for +, -, * or /. Cannot convert to an int: " << lhs <<
                                                                             " or " << rhs << std::endl;
        return "int";
    }
    else if(node->type == "Not expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "bool")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Unsupported operand type for !: " << lhs << std::endl;

        return "bool";
    }
    else if(node->type == "(Expression)" || node->type == "Object instantiation")
        return TraverseTreeSemantic(ST, node->children.front());
    else if(node->type == "Array access")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::string idx = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int[]")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Type must be an array but is a " << lhs << std::endl;
        else if(idx != "int")
            std::cerr << "@error at line " << node->children.back()->lineno <<
                      ". Type must be an int but is a " << idx << std::endl;

        return "int";
    }
    else if(node->type == "Method call") {
        std::string callerType = TraverseTreeSemantic(ST, node->children.front());
        Symbol* callerSymbol = ST->Lookup(callerType);

        if (callerSymbol == nullptr) {
            std::cerr << "@error at line " << node->lineno <<
                      ". Method " << (*(++node->children.begin()))->value << " is not defined on instance of type " << callerType << endl;
            return "-";
        }

        Class* callerClass = (Class*)ST->Lookup(callerSymbol->GetType());


        if (callerClass == nullptr) {
            std::cerr << "@error at line " << node->lineno <<
                      ". Method " << (*(++node->children.begin()))->value << " is not defined on instance of type " << callerSymbol->GetType() << endl;
            return "-";
        }

        auto childIter = std::next(node->children.begin());
        Method* method = callerClass->MethodLookup((*childIter)->value);

        if(method == nullptr)
        {
            std::cerr << "@error at line " << node->lineno <<
                      ". Method " << (*childIter)->value << " is not defined instance " << node->children.front()->value << std::endl;
            return "-";
        }

        childIter = std::next(childIter);

        if (childIter == node->children.end())
        {
            if (method->GetNumberParameters() != 0)
            {
                std::cerr << "@error at line " << node->lineno <<
                          ". Method expects 1 or more arguments. 0 were provided for " << method->GetId() << std::endl;
            }
        }
        else if (method->GetNumberParameters() != (*childIter)->children.size()) {
            std::cerr << "@error at line " << node->lineno <<
                      ". Incorrect number of arguments for method " << method->GetId() << std::endl;
        } else {
            int idx = 0;
            for(Node*& child : (*childIter)->children)
            {
                std::string type = TraverseTreeSemantic(ST, child);
                if (type != method->GetParameter(idx)->GetType()) {
                    std::cerr << "@error at line " << node->lineno <<
                              ". Type of argument" << type <<" for method call " << method->GetId() << "differs from requested type, " << method->GetParameter(idx)->GetType() << std::endl;
                }
                idx++;
            }
        }

        return method->GetType();
    }
    else if(node->type == "Expression.Length")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "int[]")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". " << lhs << " does not have a field length." << std::endl;

        return "int";
    }
    else if(node->type == "New int[] expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "int")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Cannot convert " << lhs << " to an int" << std::endl;

        return "int[]";
    }
    else if(node->type == "Variable")
    {
        Node* type = *node->children.begin();
        Node* name = *std::prev(node->children.end());

        if(type->value != "int" && type->value != "bool" && type->value != "int[]" && ST->Lookup(type->value) == nullptr)
            std::cerr << "@error at line " << node->lineno <<
                      ". Type of variable " << name->value << " doesnt exist" << std::endl;

        return (*node->children.begin())->value;
    }
    else if(node->type == "ID" || node->type == "THIS")
    {
        Variable* varPtr = ST->VarLookup(node->value);
        Symbol* idPtr = ST->Lookup(node->value);

        if(varPtr == nullptr && idPtr == nullptr)
        {
            std::cerr << "@error at line " << node->lineno <<
                      ". Identifier " << node->value << " is not defined" << std::endl;
            return "-";
        }

        return (varPtr == nullptr) ? idPtr->GetType() : varPtr->GetType();
    }
    else if(node->type == "Int")
        return "int";
    else if(node->type == "TRUE" || node->type == "FALSE")
        return "bool";

    return node->value;
}
