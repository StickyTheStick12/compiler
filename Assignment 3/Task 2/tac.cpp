#include "tac.h"

Tac::Tac(std::string  op, std::string  lhs, std::string  rhs, std::string  result) : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)), result(std::move(result)) {}

std::string Tac::GetOp() {
    return op;
}

std::string Tac::GetLhs() {
    return lhs;
}

std::string Tac::GetRhs() {
    return rhs;
}

std::string Tac::GetResult() {
    return result;
}

void Tac::SetOp(const std::string& op) {
    this->op = op;
}

void Tac::SetLhs(const std::string& lhs) {
    this->lhs = lhs;
}

void Tac::SetRhs(const std::string& rhs) {
    this->rhs = rhs;
}

void Tac::SetResult(const std::string& result) {
    this->result = result;
}

std::string Tac::GetStr() {
    return "";
}

void Tac::GenerateCode(MethodBlock *method) {}

void Tac::Dump()
{
    std::cout << result << " := " << lhs << " " << op << " " << rhs << std::endl;
}

Expression::Expression(const std::string& op, const std::string& lhs, const std::string& rhs, const std::string& result) : Tac(op, lhs, rhs, result) {}

std::string Expression::GetStr() {
    return result + " := " + lhs + " " + op + " " + rhs ;
}

void Expression::GenerateCode(MethodBlock *method) {
    Instruction* loadLhsInstruction = new Instruction();
    Instruction* loadRhsInstruction = new Instruction();
    Instruction* opInstruction = new Instruction();
    Instruction* storeInstruction = new Instruction();

    if(op == ">")
        std::swap(lhs, rhs);

    if(lhs == "true")
        lhs = "1";
    else if(lhs == "false")
        lhs = "0";

    try {
        std::stoi(lhs);
        loadLhsInstruction->id = 1;
    } catch (std::invalid_argument& ex) {
        loadLhsInstruction->id = 0;
    }

    loadLhsInstruction->argument = lhs;

    if(rhs == "true")
        rhs = "1";
    else if(rhs == "false")
        rhs = "0";

    try {
        std::stoi(rhs);
        loadRhsInstruction->id = 1;
    } catch (std::invalid_argument& ex) {
        loadRhsInstruction->id = 0;
    }
    loadRhsInstruction->argument = rhs;

    if (op == "ADD")
        opInstruction->id = 3;
    else if (op == "SUB")
         opInstruction->id = 4;
    else if (op == "MUL")
        opInstruction->id = 5;
    else if (op == "DIV")
        opInstruction->id = 6;
    else if (op == "LT" || op == "GT")
        opInstruction->id = 7;
    else if (op == "AND")
        opInstruction->id = 8;
    else if (op == "OR")
        opInstruction->id = 9;
    storeInstruction->id = 2;
    storeInstruction->argument = result;

    method->instructions.push_back(loadLhsInstruction);
    method->instructions.push_back(loadRhsInstruction);
    method->instructions.push_back(opInstruction);
    method->instructions.push_back(storeInstruction);
}

UnaryExpression::UnaryExpression(const std::string& op, const std::string& rhs, const std::string& result) : Tac(op, "", rhs, result) {}

std::string UnaryExpression::GetStr() {
    return result + " := " + op + " " + rhs ;
}

void UnaryExpression::GenerateCode(MethodBlock *method)
{
    Instruction* loadRhsInstruction = new Instruction();
    Instruction* opInstruction = new Instruction();
    Instruction* storeInstruction = new Instruction();

    if(rhs == "true")
        rhs = "1";
    else if(rhs == "false")
        rhs = "0";

    try {
        stoi(rhs);
        loadRhsInstruction->id = 1;
    } catch (std::invalid_argument& ex)
    {
        loadRhsInstruction->id = 0;
    }

    loadRhsInstruction->argument = rhs;

    if(op == "!")
        opInstruction->id = 10;

    storeInstruction->id = 2;
    storeInstruction->argument = result;
    method->instructions.push_back(loadRhsInstruction);
    method->instructions.push_back(opInstruction);
    method->instructions.push_back(storeInstruction);
}

Copy::Copy(const std::string& lhs, const std::string& result) : Tac("Copy", lhs, "", result) {}

std::string Copy::GetStr() {
    return result + " := " + lhs ;
}

void Copy::GenerateCode(MethodBlock *method) {
    Instruction* loadInstruction = new Instruction();
    Instruction* storeInstruction = new Instruction();

    if(lhs == "true")
        lhs = "1";
    else if(lhs == "false")
        lhs = "0";

    try {
        std::stoi(lhs);
        loadInstruction->id = 1;
    } catch (std::invalid_argument ex)
    {
        loadInstruction->id = 0;
    }
    loadInstruction->argument = lhs;

    storeInstruction->id = 2;
    storeInstruction->argument = result;
    method->instructions.push_back(loadInstruction);
    method->instructions.push_back(storeInstruction);
}

CopyArray::CopyArray(const std::string& lhs, const std::string& rhs, const std::string& result) : Tac("CopyArray", lhs, rhs, result) {}

std::string CopyArray::GetStr() {
    return result + "[" + lhs + "] := " + rhs ;
}

void CopyArray::GenerateCode(MethodBlock *method) {}

ArrayAccess::ArrayAccess(const std::string& index, const std::string& lhs, const std::string& result) : Tac("ArrayAccess", lhs, index, result) {}

std::string ArrayAccess::GetStr() {
    return result + " := " + lhs + "[" + rhs + "]" ;
}

void ArrayAccess::GenerateCode(MethodBlock *method) {}

MethodCall::MethodCall(const std::string& function, const std::string& name, const std::string& result) : Tac("call", function, name, result) {}

std::string MethodCall::GetStr() {
    return result + " := " + op + " " + lhs + ", " + rhs ;
}

void MethodCall::GenerateCode(MethodBlock *method) {
    Instruction* callInstruction = new Instruction();
    Instruction* retInstruction = new Instruction();
    callInstruction->id = 13;
    callInstruction->argument = lhs;
    retInstruction->id = 2;
    retInstruction->argument = result;
    method->instructions.push_back(callInstruction);
    method->instructions.push_back(retInstruction);
}

Jump::Jump(const std::string &label) : Tac("goto", "", "", label) {}

std::string Jump::GetStr() {
    return op + " " + result ;
}

void Jump::GenerateCode(MethodBlock *method) {
    Instruction* gotoInstruction = new Instruction();
    gotoInstruction->id = 11;
    gotoInstruction->argument = result;
    method->instructions.push_back(gotoInstruction);
}

CondJump::CondJump(const std::string& cond, const std::string& label) : Tac("iffalse", cond, "goto", label) {}

std::string CondJump::GetStr() {
    return op + " " + lhs + " " + rhs + " " + result ;
}

void CondJump::GenerateCode(MethodBlock *method) {
    Instruction* loadInstruction = new Instruction();
    Instruction* jumpInstruction = new Instruction();

    if(lhs == "true")
        lhs = "1";
    else if(lhs == "false")
        lhs = "0";

    try {
        std::stoi(lhs);
        loadInstruction->id = 1;
    } catch (std::invalid_argument ex)
    {
        loadInstruction->id = 0;
    }

    loadInstruction->argument = lhs;
    jumpInstruction->id = 12;
    jumpInstruction->argument = result;
    method->instructions.push_back(loadInstruction);
    method->instructions.push_back(jumpInstruction);
}

New::New(const std::string& rhs, const std::string& result) : Tac("new", "", rhs, result) {}

std::string New::GetStr() {
    return result + " := " + op + " " + rhs ;
}

void New::GenerateCode(MethodBlock *method) {}

NewArray::NewArray(const std::string& type, const std::string& number, const std::string& result) : Tac("new", type, number, result) {}

std::string NewArray::GetStr() {
    return result + " " + lhs + " " + rhs ;
}

void NewArray::GenerateCode(MethodBlock *method) {}

Length::Length(const std::string& lhs, const std::string& result) : Tac("length", lhs, "", result) {}

std::string Length::GetStr() {
    return result + " := " + op + " " + lhs ;
}

void Length::GenerateCode(MethodBlock *method) {}

Parameter::Parameter(const std::string& result) : Tac("param", "", "", result) {}

std::string Parameter::GetStr() {
    return op + " " + result;
}

void Parameter::GenerateCode(MethodBlock *method) {
    Instruction* parameterInstruction = new Instruction();

    if(result == "true")
        result = "1";
    else if(result == "false")
        result = "0";

    try {
        std::stoi(result);
        parameterInstruction->id = 1;
    } catch (std::invalid_argument ex)
    {
        parameterInstruction->id = 0;
    }

    parameterInstruction->argument = result;
    method->instructions.push_back(parameterInstruction);
}

Return::Return(const std::string& result) : Tac("return", "", "", result) {}

std::string Return::GetStr() {
    return op + " " + result ;
}

void Return::GenerateCode(MethodBlock *method) {
    Instruction* valInstruction = new Instruction();
    Instruction* retInstruction = new Instruction();

    if(result == "true")
        result = "1";
    else if(result == "false")
        result = "0";

    try {
        std::stoi(result);
        valInstruction->id = 1;
    } catch (std::invalid_argument ex)
    {
        valInstruction->id = 0;
    }

    valInstruction->argument = result;
    retInstruction->id = 14;
    method->instructions.push_back(valInstruction);
    method->instructions.push_back(retInstruction);
}

Print::Print(const std::string& result) : Tac("print", "", "", result) {}

std::string Print::GetStr() {
    return op + " " + result ;
}

void Print::GenerateCode(MethodBlock *method) {
    Instruction* loadResInstruction = new Instruction();
    Instruction* opInstruction = new Instruction();

    if(result == "true")
        result = "1";
    else if(result == "false")
        result = "0";

    try {
        std::stoi(result);
        loadResInstruction->id = 1;
    } catch (std::invalid_argument ex)
    {
        loadResInstruction->id = 0;
    }

    loadResInstruction->argument = result;

    opInstruction->id = 15;

    method->instructions.push_back(loadResInstruction);
    method->instructions.push_back(opInstruction);
}
