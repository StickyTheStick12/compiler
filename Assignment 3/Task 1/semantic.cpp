#include "semantic.h"

std::pair<std::string, std::string> t;

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

        std::string expRet = node->children.front()->value;

        if(returnType != expRet && ((returnType == "Q" && t.first != expRet && t.second != expRet) || returnType != "Q"))
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

        if(lhs != "bool" && ((lhs == "Q" && t.first != "bool" && t.second != "bool") || lhs != "Q"))
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". If and while require a bool. Cannot convert " << lhs << " to a bool" << std::endl;

        return "void";
    }
    else if(node->type == "Assignment")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::pair<std::string, std::string> temp = t;

        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != rhs && lhs != "Q" && rhs != "Q")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". " << lhs << " type does not match with " << rhs << " type" << std::endl;
        else if(lhs == "Q" && rhs != "Q" && temp.first != rhs && temp.second != rhs)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". " << lhs << " type does not match with " << rhs << " type" << std::endl;
        else if(rhs == "Q" && lhs != "Q" && t.first != lhs && t.second != lhs)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". " << lhs << " type does not match with " << rhs << " type" << std::endl;
        else if(rhs == "Q" && lhs == "Q" && t.first != temp.first && t.first != temp.second && t.second != temp.first && t.second != temp.second)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". " << lhs << " type does not match with " << rhs << " type" << std::endl;

        return "void";
    }
    else if(node->type == "Array assignment")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        if(lhs != "int[]" && ((lhs == "Q" && t.first != "int[]" && t.second != "int[]") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". the type of the expression must be an int array but is a " << lhs << std::endl;

            return "void";
        }

        std::string idx = TraverseTreeSemantic(ST, *(std::next(node->children.begin())));
        if(idx != "int" && ((lhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q"))
        {
            std::cerr << "@error at line " << (*(std::next(node->children.begin())))->lineno <<
                      ". the type of the expression must be an int but is a " << idx << std::endl;

            return "void";
        }

        std::string rhs = TraverseTreeSemantic(ST, node->children.back());
        if(idx != "int" && ((lhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q")){
            std::cerr << "@error at line " << node->children.back()->lineno <<
                      ". Cannot convert " << rhs << " to a integer" << std::endl;
            return "void";
        }

        return "void";
    }
    else if(node->type == "Print")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        if(lhs != "int" && lhs != "bool" && ((lhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q"))
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Cannot convert " << lhs << " to an int" << std::endl;

        return "void";
    }
    else if(node->type == "or expression" || node->type == "and expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        if(lhs != "bool" && ((lhs == "Q" && t.first != "bool" && t.second != "bool") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". unsupported operands for 'or' or 'and' expression. Can't convert " << lhs << " to a bool" << std::endl;
            return "bool";
        }

        std::string rhs = TraverseTreeSemantic(ST, node->children.back());
        if(rhs != "bool" && ((rhs == "Q" && t.first != "bool" && t.second != "bool") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". unsupported operands for 'or' or 'and' expression. Can't convert " << rhs << " to a bool" << std::endl;
            return "bool";
        }

        return "bool";
    }
    else if(node->type == "EQ expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        std::pair<std::string, std::string> temp = t;
        std::string rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != rhs && lhs != "Q" && rhs != "Q")
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". == requires that both types are the same." << lhs << " and " << rhs << " is not" << std::endl;
        else if(lhs == "Q" && rhs != "Q" && temp.first != rhs && temp.second != rhs)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". == requires that both types are the same." << lhs << " and " << rhs << " is not" << std::endl;
        else if(rhs == "Q" && lhs != "Q" && t.first != lhs && t.second != lhs)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". == requires that both types are the same." << lhs << " and " << rhs << " is not" << std::endl;
        else if(rhs == "Q" && lhs == "Q" && t.first != temp.first && t.first != temp.second && t.second != temp.first && t.second != temp.second)
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". == requires that both types are the same." << lhs << " and " << rhs << " is not" << std::endl;

        return "bool";
    }
    else if(node->type == "LT expression" || node->type == "GT expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        if(lhs != "int" && ((lhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Cannot convert " << lhs << " to an int for LT or GT" << std::endl;
            return "bool";
        }

        std::string rhs = TraverseTreeSemantic(ST, node->children.back());
        if(rhs != "int" && ((rhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Cannot convert " << rhs << " to an int for LT or GT" << std::endl;
            return "bool";
        }

        return "bool";
    }
    else if(node->type == "Add expression" || node->type == "Sub expression" || node->type == "Mul expression" ||
            node->type == "Div expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());
        if(lhs != "int" && ((lhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Unsupported operand type for +, -, * or /. Cannot convert to an int: " << lhs << std::endl;
            return "int";
        }

        std::string rhs = TraverseTreeSemantic(ST, node->children.back());
        if(rhs != "int" && ((rhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Unsupported operand type for +, -, * or /. Cannot convert to an int: " << rhs << std::endl;
            return "int";
        }

        return "int";
    }
    else if(node->type == "Not expression")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "bool" && ((lhs == "Q" && t.first != "bool" && t.second != "bool") || lhs != "Q"))
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Unsupported operand type for !: " << lhs << std::endl;

        return "bool";
    }
    else if(node->type == "(Expression)" || node->type == "Object instantiation")
        return TraverseTreeSemantic(ST, node->children.front());
    else if(node->type == "Array access")
    {
        std::string lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "int[]" && ((lhs == "Q" && t.first != "int[]" && t.second != "int[]") || lhs != "Q"))
        {
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Type must be an array but is a " << lhs << std::endl;
            return "int";
        }

        std::string idx = TraverseTreeSemantic(ST, node->children.back());
        if(idx != "int" && ((idx == "Q" && t.first != "int" && t.second != "int") || idx != "Q"))
        {
            std::cerr << "@error at line " << node->children.back()->lineno <<
                      ". Type must be an int but is a " << idx << std::endl;
            return "int";
        }

        return "int";
    }
    else if(node->type == "Method call") {
        std::string callerType = TraverseTreeSemantic(ST, node->children.front());
        Class* callerClass;

        if(callerType == "Q")
        {
            callerClass = ST->ClassLookup(t.first);

            if(callerClass == nullptr)
                callerClass = ST->ClassLookup(t.second);
        }
        else
            callerClass = ST->ClassLookup(callerType);

        if(callerClass == nullptr)
        {
            std::cerr << "@error at line " << node->lineno <<
                      ". Method " << (*(++node->children.begin()))->value << " is not defined on instance of type " << node->children.front()->value << std::endl;
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

        if(lhs != "int[]" && ((lhs == "Q" && t.first != "int[]" && t.second != "int[]") || lhs != "Q"))
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". " << lhs << " does not have a field length." << std::endl;

        return "int";
    }
    else if(node->type == "New int[] expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "int" && ((lhs == "Q" && t.first != "int" && t.second != "int") || lhs != "Q"))
            std::cerr << "@error at line " << node->children.front()->lineno <<
                      ". Cannot convert " << lhs << " to an int" << std::endl;

        return "int[]";
    }
    else if(node->type == "Variable")
    {
        Node* type = *node->children.begin();
        Node* name = *std::prev(node->children.end());

        if(type->value != "int" && type->value != "bool" && type->value != "int[]" && ST->ClassLookup(type->value) == nullptr)
            std::cerr << "@error at line " << node->lineno <<
                      ". Type of variable " << name->value << " doesnt exist" << std::endl;

        return (*node->children.begin())->value;
    }
    else if(node->type == "ID" || node->type == "THIS")
    {
        Variable* varPtr = ST->VarLookup(node->value);
        Symbol* classPtr = ST->ClassLookup(node->value);

        if(varPtr == nullptr && classPtr == nullptr)
        {
            std::cerr << "@error at line " << node->lineno <<
                      ". Identifier " << node->value << " is not defined" << std::endl;
            return "-";
        }
        else if(varPtr != nullptr && classPtr != nullptr)
        {
            t = std::make_pair(varPtr->GetType(), classPtr->GetType());

            return "Q";
        }

        return (varPtr == nullptr) ? classPtr->GetType() : varPtr->GetType();
    }
    else if(node->type == "Int")
        return "int";
    else if(node->type == "TRUE" || node->type == "FALSE")
        return "bool";

    return node->value;
}
