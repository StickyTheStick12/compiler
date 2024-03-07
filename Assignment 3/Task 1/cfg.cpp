#include "cfg.h"

//TODO. fix block for if statement, (Only if, so not if else)

BBlock* currentBlock;
std::vector<BBlock*> methods;
std::vector<std::string> renderedBlocks;

std::string BBlock::GenTempName() {
    return "_temp_" + std::to_string(tempCounter++);
}

std::string BBlock::GenBlockName() {
    std::cout << blockCounter << std::endl;
    return "block" + std::to_string(blockCounter++);
}

std::string TraverseTreeTac(SymbolTable* ST, Node* node) {
    if (node->type == "Method") {
        currentBlock = new BBlock();
        methods.push_back(currentBlock);
    }
    if (node->type == "Method body") {
        std::string name;
        for(auto & child : node->children)
            name = TraverseTreeTac(ST, child);

        Tac* ret = new Return(name);
        currentBlock->tacInstructions.push_back(ret);
        return "";
    }
    else if (node->type == "Empty statement")
        return "";
    else if (node->type == "If else") {
        BBlock* trueBlock = new BBlock();
        BBlock* falseBlock = new BBlock();
        BBlock* joinBlock = new BBlock();

        currentBlock->trueExit = trueBlock;
        currentBlock->falseExit = falseBlock;

        auto child = node->children.begin();
        std::string condName = TraverseTreeTac(ST, *child);
        Tac* condition = new CondJump(condName, falseBlock->name);
        currentBlock->tacInstructions.push_back(condition);

        child = std::next(child);
        currentBlock = trueBlock;
        TraverseTreeTac(ST, *child);
        currentBlock->trueExit = joinBlock;

        child = std::next(child);
        currentBlock = falseBlock;
        TraverseTreeTac(ST, *child);
        currentBlock->trueExit = joinBlock;

        currentBlock = joinBlock;

        return "";
    }
    else if (node->type == "While loop") {
        BBlock* headerBlock = new BBlock();
        BBlock* trueBlock = new BBlock();
        BBlock* joinBlock = new BBlock();

        currentBlock->trueExit = headerBlock;

        auto child = node->children.begin();
        currentBlock = headerBlock;
        std::string condName = TraverseTreeTac(ST, *child);
        Tac* cond = new CondJump(condName, joinBlock->name);
        currentBlock->tacInstructions.push_back(cond);

        child = std::next(child);
        currentBlock = trueBlock;
        TraverseTreeTac(ST, *child);
        Tac* jmp = new Jump(headerBlock->name);
        currentBlock->tacInstructions.push_back(jmp);
        currentBlock->trueExit = headerBlock;

        headerBlock->trueExit = trueBlock;
        headerBlock->falseExit = joinBlock;
        currentBlock = joinBlock;
        return "";
    }
    else if (node->type == "Print") {
        Node* child = node->children.front();
        std::string name = TraverseTreeTac(ST, child);
        Tac* instruction = new Print(name);
        currentBlock->tacInstructions.push_back(instruction);
        return "";
    }
    else if (node->type == "Assignment") {
        auto child = node->children.begin();
        std::string name = (*child)->value;
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Copy(rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Array Assignment") {
        auto child = node->children.begin();
        std::string name = (*child)->value;
        child = std::next(child);
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new CopyArray(lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "or expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "boolean"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("or", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "and expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "boolean"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("and", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "EQ expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "boolean"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("EQ", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "LT expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "boolean"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("LT", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "GT expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "boolean"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("GT", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Sub expression") {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "int"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("SUB", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Add expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "int"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("ADD", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Mul expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "int"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("MUL", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Div expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "int"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string rhsName = TraverseTreeTac(ST, *child);
        Tac* instruction = new Expression("DIV", lhsName, rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Not expression")
    {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "boolean"));
        auto child = node->children.front();
        std::string rhsName = TraverseTreeTac(ST, child);
        Tac* instruction = new UnaryExpression("NOT", rhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "TRUE") {
        return "TRUE";
    }
    else if (node->type == "FALSE") {
        return "FALSE";
    }
    else if (node->type == "THIS") {
        return "THIS";
    }
    else if (node->type == "(Expression)") {
        return TraverseTreeTac(ST, node->children.front());
    }
    else if (node->type == "Array access") {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "int"));
        auto child = node->children.begin();
        std::string lhsName = TraverseTreeTac(ST, *child);
        child = std::next(child);
        std::string idx = TraverseTreeTac(ST, *child);
        Tac* instruction = new ArrayAccess(lhsName, idx, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Method call") {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, ""));
        auto child = node->children.begin();
        std::string caller = TraverseTreeTac(ST, *child);
        Tac* callerTac = new Parameter(caller);
        currentBlock->tacInstructions.push_back(callerTac);
        child = std::next(child);
        std::string methodName = (*child)->value;
        int length = 1;
        child = std::next(child);
        if (child != node->children.end()) {
            TraverseTreeTac(ST, *child);
            length = (*child)->children.size() + 1;
        }
        Tac* instruction = new MethodCall(methodName,std::to_string(length),name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Expression.Length") {
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "int"));
        auto child = node->children.front();
        std::string lhsName = TraverseTreeTac(ST, child);
        Tac* instruction = new Length(lhsName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "New int[] expression") {
        auto child = node->children.front();
        std::string lenName = TraverseTreeTac(ST, child);
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, "int[]"));
        Tac* instruction = new NewArray("int[]", lenName, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Object instantiation") {
        auto child = node->children.front();
        std::string className = TraverseTreeTac(ST, child);
        std::string name = BBlock::GenTempName();
        ST->AddVar(name, new Variable(name, className));
        Tac* instruction = new New(className, name);
        currentBlock->tacInstructions.push_back(instruction);
        return name;
    }
    else if (node->type == "Parameter") {
        for(auto& child : node->children) {
            std::string name = TraverseTreeTac(ST, child);
            Tac* instruction = new Parameter(name);
            currentBlock->tacInstructions.push_back(instruction);
        }
        return "";
    }
    else if (node->type == "ID" || node->type == "Int")
    {
        return node->value;
    }
    else {
        for (auto & child : node->children) {
            TraverseTreeTac(ST, child);
        }
        return node->value;
    }
}

void CreateBlockCfg(BBlock* block, std::ofstream* outStream) {
    *outStream << block->name << " [label=\"" << block->name << "\\n";
    renderedBlocks.push_back(block->name);

    for (auto & tacInstruction : block->tacInstructions) {
        *outStream << tacInstruction->GetStr() << "\\n";
    }
    *outStream << "\"];" << std::endl;
    if (block->trueExit != nullptr) {
        *outStream << block->name << " -> " << block->trueExit->name << "[xlabel=\"true\"];" << std::endl;
        if (find(renderedBlocks.begin(), renderedBlocks.end(), block->trueExit->name) == renderedBlocks.end()) {
            CreateBlockCfg(block->trueExit, outStream);
        }
    }
    if (block->falseExit != nullptr) {
        *outStream << block->name << " -> " << block->falseExit->name << "[xlabel=\"false\"];" << std::endl;
        CreateBlockCfg(block->falseExit, outStream);
    }
}

void CreateCfg(BBlock* block) {
    std::ofstream outStream;
    outStream.open("cfg.dot");

    outStream << "digraph {\ngraph [splines=ortho]\nnode [shape=box]\n";
    for(auto i = methods.begin(); i != methods.end(); ++i) {
        CreateBlockCfg(*i, &outStream);
    }
    outStream << "\n}";
    outStream.close();
    std::cout << "Built a cfg-tree" << std::endl;
}
