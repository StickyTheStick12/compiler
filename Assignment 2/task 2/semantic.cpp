#include "semantic.h"

std::string TraverseTreeSemantic(SymbolTable* ST, Node* node)
{
    if(node->type == "Program")
    {
        for(auto & iter : node->children)
        {
            ST->enterScope("", nullptr);
            TraverseTreeSemantic(ST, iter);
            ST->exitScope();
        }
        return "void";
    }
    else if(node->type == "Class" || node->type == "Class body" || node->type == "Method statement" || node->type == "Statements" || node->type == "Main class statement")
    {
        for(auto & iter : node->children)
            TraverseTreeSemantic(ST, iter);

        return "void";
    }
    else if(node->type == "Main Class")
    {
        ST->enterScope("", nullptr);

        for(auto & iter : node->children)
            TraverseTreeSemantic(ST, iter);

        ST->exitScope();
        return "void";
    }
    else if(node->type == "MethodDecList")
    {
        for(auto & iter : node->children) {
            ST->enterScope("", nullptr);
            TraverseTreeSemantic(ST, iter);
            ST->exitScope();
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

        if(lhs != "boolean")
        {
            std::cout << "Cannot convert " << lhs << " to a boolean" << std::endl;
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
            std::cout << "TypeError: " << lhs << " type does not match with " << rhs << " type" << std::endl;

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

        if(lhs != "boolean" || rhs != "boolean")
        {
            std::cout << "TypeError: unsupported operands: " << lhs << " and " << rhs << std::endl;
            exit(-1);
        }

        return "boolean";
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

        return "boolean";
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

        return "boolean";
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

        if(lhs != "boolean")
        {
            std::cout << "TypeError: Unsupported operand type for !: " << lhs << std::endl;
            exit(-1);
        }

        return "boolean";
    }
    else if(node->type == "(Expression)" || node->type == "object instantiation")
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
        auto callerSymbol = ST->lookup(caller);
        auto callerClass = (Class*)ST->lookup(callerSymbol->getType());

        if (callerClass == nullptr) {
            std::cout << "Error: method is not defined on type" << std::endl;
            exit(-1);
        }

        auto childrenFront = node->children.begin();
        auto method = callerClass->lookupMethod((*std::next(childrenFront))->value);

        if (method == nullptr)
        {
            std::cout << "Error: method is not defined on type" << std::endl;
            exit(-1);
        }

        childrenFront = std::next(childrenFront);

        if (childrenFront == node->children.end()) {
            if (method->getNumParams() != 0 ||
                method->getNumParams() != (*childrenFront)->children.size())
            {
                std::cout << "incorrect number of parameters for this method" << std::endl;
                exit(-1);
            }
            else {
                int idx = 0;
                for (auto & iter: (*childrenFront)->children) {
                    auto type = TraverseTreeSemantic(ST, iter);

                    if (type != method->getParam(idx)->getType())
                    {
                        std::cout << "TypeError: type missmatch" << std::endl;
                        exit(-1);
                    }

                    idx++;
                }
            }

            return method->getType();
        }
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
        auto id = ST->lookup(node->value);

        if(id == nullptr)
        {
            std::cout << "Error: identifier " << node->value << " is not defined" << std::endl;
            exit(-1);
        }

        return id->getType();
    }
    else if(node->type == "Int")
        return "int";
    else if(node->type == "TRUE" || node->type == "FALSE")
        return "boolean";

    return node->value;
}
