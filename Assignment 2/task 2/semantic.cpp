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
    else if(node->type == "Class" || node->type == "Class body" || node->type == "Method statement" || node->type == "Statements" || node->type == "Main class statement" || node->type == "MethodList" || node->type == "Class variables")
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

        auto returnType = TraverseTreeSemantic(ST, node->children.back());

        if(node->children.front()->value != returnType)
        {
            std::cout << "TypeError: return value differs from expected type. Cannot convert" << returnType << " to " <<
                      node->children.front()->value << std::endl;
            exit(-1);
        }

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
        for(auto & iter : node->children)
            TraverseTreeSemantic(ST, iter);

        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "bool")
        {
            std::cout << "Cannot convert " << lhs << " to a bool" << std::endl;
            exit(-1);
        }

        return "void";
    }
    else if(node->type == "Assignment")
    {
        //todo: check if this is a error
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != rhs)
        {
            std::cout << "TypeError: " << lhs << " type does not match with " << rhs << " type" << std::endl;
            std::cout << node->children.front()->type << std::endl;
            std::cout << node->children.back()->type << std::endl;
            exit(-1);
        }

        return "void";
    }
    else if(node->type == "Array assignment")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto idx = TraverseTreeSemantic(ST, *(std::next(node->children.begin())));
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int[]")
        {
            std::cout << "TypeError: the type of the expression must be an array but is a " << lhs << std::endl;
            exit(-1);
        }
        else if(idx != "int")
        {
            std::cout << "TypeError: Cannot convert " << idx << " to a integer" <<  std::endl;
            exit(-1);
        }
        else if(rhs != "int")
        {
            std::cout << "TypeError: Cannot convert " << rhs << " to a integer" <<  std::endl;
            exit(-1);
        }

        return "void";
    }
    else if(node->type == "or expression" || node->type == "and expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "bool" || rhs != "bool")
        {
            std::cout << "TypeError: unsupported operands: " << lhs << " and " << rhs << std::endl;
            exit(-1);
        }

        return "bool";
    }
    else if(node->type == "EQ expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != rhs)
        {
            std::cout << "TypeError: Unsupported operand types for ==: " << lhs << " and " << rhs << std::endl;
            exit(-1);
        }

        return "bool";
    }
    else if(node->type == "LT expression" || node->type == "GT expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int" || rhs != "int")
        {
            std::cout << "TypeError: Unsupported operand types for ==: " << lhs << " and " << rhs << std::endl;
            exit(-1);
        }

        return "bool";
    }
    else if(node->type == "Add expression" || node->type == "Sub expression" || node->type == "Mul expression" ||
            node->type == "Div expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int" || rhs != "int")
        {
            std::cout << "TypeError: Unsupported operand types for +, -, * or /: " << lhs << " and " << rhs << std::endl;
            exit(-1);
        }

        return "int";
    }
    else if(node->type == "Not expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "bool")
        {
            std::cout << "TypeError: Unsupported operand type for !: " << lhs << std::endl;
            exit(-1);
        }

        return "bool";
    }
    else if(node->type == "(Expression)" || node->type == "Object instantiation")
        return TraverseTreeSemantic(ST, node->children.front());
    else if(node->type == "Array access")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto idx = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int[]")
        {
            std::cout << "Type must be an array but is a " << lhs << std::endl;
            exit(-1);
        }
        else if(idx != "int")
        {
            std::cout << "Type must be an int but is a " << idx << std::endl;
            exit(-1);
        }

        return "int";
    }
    else if(node->type == "Method call") {
        auto caller = TraverseTreeSemantic(ST, node->children.front());
        auto callerRecord = ST->Lookup(caller);
        auto callerClass = (Class*)ST->Lookup(callerRecord->GetType());
        if (callerClass == nullptr) {
            cout << "Error: Method " << (*(++node->children.begin()))->value << " is not defined on instance of type " << callerRecord->GetType() << endl;
            exit(-1);
        }
        auto childrenFront = node->children.begin();
        advance(childrenFront, 1);
        auto method = callerClass->MethodLookup((*childrenFront)->value);
        if (method == nullptr) {
            cout << "Error: Method " << (*childrenFront)->value << " is not defined on instance of type " << callerRecord->GetType() << endl;
            exit(-1);
        }
        advance(childrenFront, 1);
        if (childrenFront == node->children.end()) {
            if (method->GetNumberParameters() != 0) {
                cout << "Not the correct number of parameters for method " << method->GetId() << endl;
                exit(-1);
            }
        } else if (method->GetNumberParameters() != (*childrenFront)->children.size()) {
            //std::cout << "Not the correct number of parameters for method " << method->GetId() << std::endl;
            //std::cout << (*childrenFront)->children.size() << std::endl;
            //std::cout << method->GetNumberParameters() << std::endl;
            //exit(-1);
        } else {
            int idx = 0;
            for (auto i=(*childrenFront)->children.begin(); i!=(*childrenFront)->children.end(); i++) {
                auto type = TraverseTreeSemantic(ST, *i);
                if (type != method->GetParameter(idx)->GetType()) {
                    cout << "TypeError: type missmatch, '" << type << "' does not match with '" << method->GetParameter(idx)->GetType() << "' in method call " << method->GetId() << endl;
                    exit(-1);
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
        {
            std::cout << "This type does not have a field length" << std::endl;
            exit(-1);
        }

        return "int";
    }
    else if(node->type == "New int[] expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "int")
        {
            std::cout << "Cannot convert " << lhs << " to int" << std::endl;
            exit(-1);
        }

        return "int[]";
    }
    else if(node->type == "Variable")
        return (*node->children.begin())->value;
    else if(node->type == "ID" || node->type == "THIS")
    {
        auto id = ST->Lookup(node->value);

        if(id == nullptr)
        {
            std::cout << "Error: identifier " << node->value << " is not defined" << std::endl;
            exit(-1);
        }

        return id->GetType();
    }
    else if(node->type == "Int")
        return "int";
    else if(node->type == "TRUE" || node->type == "FALSE")
        return "bool";

    return node->value;
}

int Semantic_analysis(SymbolTable* symbolTable, Node* root)
{
    symbolTable->ResetTable();
    return !TraverseTreeSemantic(symbolTable, root).empty();
}