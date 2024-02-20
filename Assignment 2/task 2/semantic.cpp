#include "semantic.h"

//kolla assignment så rhs inte är void heller

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
    else if(node->type == "Class" || node->type == "Class body" || node->type == "Method statement" || node->type == "Statements" || node->type == "Main class statement" || node->type == "MethodList" || node->type == "Class variables" || node->type == "Print")
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
            std::cout << "Return type of method doesnt match type of returned value. Cannot convert " << returnType <<
            " to " << node->children.front()->value << std::endl;

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

        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "bool")
            std::cout << "Error: if and while require a bool. Cannot convert " << lhs << " to a bool" << std::endl;

        return "void";
    }
    else if(node->type == "Assignment")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs == "void")
            return "void";

        auto rhs = TraverseTreeSemantic(ST, node->children.back());
        if(lhs != rhs)
            std::cout << "Error: " << lhs << " type does not match with " << rhs << " type" << std::endl;

        return "void";
    }
    else if(node->type == "Array assignment")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto idx = TraverseTreeSemantic(ST, *(std::next(node->children.begin())));
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int[]")
            std::cout << "Error: the type of the expression must be an int array but is a " << lhs << std::endl;
        else if(idx != "int")
            std::cout << "Error: Cannot convert " << idx << " to a integer. Idx for the int array must be an int" <<  std::endl;
        else if(rhs != "int")
            std::cout << "Error: Cannot convert " << rhs << " to a integer" <<  std::endl;

        return "void";
    }
    else if(node->type == "or expression" || node->type == "and expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "bool" || rhs != "bool")
            std::cout << "Error: unsupported operands for 'or' or 'and' expression. Can't convert " << lhs << " or "
            << rhs << " to a bool" << std::endl;

        return "bool";
    }
    else if(node->type == "EQ expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != rhs)
            std::cout << "Error: == requires that both types are the same." << lhs << " and " << rhs << " is not" << std::endl;

        return "bool";
    }
    else if(node->type == "LT expression" || node->type == "GT expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int" || rhs != "int")
            std::cout << "Error: Cannot convert " << lhs << " or " << rhs << " to an int for LT or GT" << std::endl;

        return "bool";
    }
    else if(node->type == "Add expression" || node->type == "Sub expression" || node->type == "Mul expression" ||
            node->type == "Div expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto rhs = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int" || rhs != "int")
            std::cout << "Error: Unsupported operand type for +, -, * or /. Cannot convert to an int: " << lhs <<
            " or " << rhs << std::endl;

        return "int";
    }
    else if(node->type == "Not expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "bool")
            std::cout << "Error: Unsupported operand type for !: " << lhs << std::endl;

        return "bool";
    }
    else if(node->type == "(Expression)" || node->type == "Object instantiation")
        return TraverseTreeSemantic(ST, node->children.front());
    else if(node->type == "Array access")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());
        auto idx = TraverseTreeSemantic(ST, node->children.back());

        if(lhs != "int[]")
            std::cout << "Error: Type must be an array but is a " << lhs << std::endl;
        else if(idx != "int")
            std::cout << "Error: Type must be an int but is a " << idx << std::endl;

        return "int";
    }
    else if(node->type == "Method call") {
        auto callerSymbol = TraverseTreeSemantic(ST, node->children.front());
        auto callerClass = (Class*)ST->Lookup((ST->Lookup(callerSymbol))->GetType());


        auto caller = TraverseTreeSemantic(ST, node->children.front());
        auto callerRecord = ST->Lookup(caller);

        if(callerClass != (Class*)ST->Lookup(callerRecord->GetType()))
            std::cout << "WRONG" << std::endl;

        //auto callerClass = (Class*)ST->Lookup(callerRecord->GetType());
        if (callerClass == nullptr) {
            cout << "Error: Method " << (*(++node->children.begin()))->value << " is not defined on instance of type " << callerRecord->GetType() << endl;
            return "void";
        }

        auto childIter = std::next(node->children.begin());
        auto method = callerClass->MethodLookup((*childIter)->value);

        if(method == nullptr)
        {
            std::cout << "Error: method " << (*childIter)->value << " is not defined instance " << node->children.front()->value << std::endl;
            return "void";
        }

        childIter = std::next(childIter);

        if(childIter == node->children.end())
        {
            if(method->GetNumberParameters() != 0)
            {
                std::cout << "Error: method expects 0 arguments, 1 or more were provided for method " << method->GetId()
                << std::endl;
                exit(-1);
                return "void";
            }
        }
        else if (method->GetNumberParameters() != (*childIter)->children.size()) {
            std::cout << method->GetNumberParameters() << std::endl;
            //std::cout << "Not the correct number of parameters for method " << method->GetId() << std::endl;
            //exit(-1);
        } else {
            int idx = 0;
            for (auto i=(*childIter)->children.begin(); i!=(*childIter)->children.end(); i++) {
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
            std::cout << "Error: " << lhs << " does not have a field length." << std::endl;

        return "int";
    }
    else if(node->type == "New int[] expression")
    {
        auto lhs = TraverseTreeSemantic(ST, node->children.front());

        if(lhs != "int")
            std::cout << "Error: Cannot convert " << lhs << " to an int" << std::endl;

        return "int[]";
    }
    else if(node->type == "Variable")
        return (*node->children.begin())->value;
    else if(node->type == "ID" || node->type == "THIS")
    {
        auto idPtr = ST->Lookup(node->value);

        if(idPtr == nullptr)
        {
            std::cout << "Error: identifier " << node->value << " is not defined" << std::endl;
            return "void";
        }

        return idPtr->GetType();
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
