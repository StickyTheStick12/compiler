#include "tac.h"

Tac::Tac(std::string op, std::string lhs, std::string rhs, std::string result) : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)), result(std::move(result)) {}

std::string Tac::GetStr() {
    return "";
}

void Tac::GenerateCode(MethodBlock *method) {}

Expression::Expression(const std::string& op, const std::string& lhs, const std::string& rhs, const std::string& result) : Tac(op, lhs, rhs, result) {}

std::string Expression::GetStr() {
    return result + " := " + lhs + " " + op + " " + rhs ;
}

void Expression::GenerateCode(MethodBlock *method) {
    Instruction* lhsInstruction = new Instruction();
    Instruction* rhsInstruction = new Instruction();
    Instruction* opInstruction = new Instruction();
    Instruction* storeInstruction = new Instruction();

    if(op == ">")
        std::swap(lhs, rhs);

    lhs = (lhs == "true") ? "1" : (lhs == "false") ? "0" : lhs;

    try {
        std::stoi(lhs);
        lhsInstruction->id = 1; //iconst
    } catch (std::invalid_argument& ex) {
        lhsInstruction->id = 0; //iload
    }

    lhsInstruction->argument = lhs;

    rhs = (rhs == "true") ? "1" : (rhs == "false") ? "0" : rhs;

    try {
        std::stoi(rhs);
        rhsInstruction->id = 1; //iconst
    } catch (std::invalid_argument& ex) {
        rhsInstruction->id = 0; //iload
    }
    rhsInstruction->argument = rhs;

    if (op == "ADD")
        opInstruction->id = 3; //iadd
    else if (op == "SUB")
         opInstruction->id = 4; //isub
    else if (op == "MUL")
        opInstruction->id = 5; //imul
    else if (op == "DIV")
        opInstruction->id = 6;  //ilt
    else if (op == "LT" || op == "GT")
        opInstruction->id = 7; //ilt
    else if (op == "AND")
        opInstruction->id = 8; //iand
    else if (op == "OR")
        opInstruction->id = 9; //ior

    storeInstruction->id = 2; //istore
    storeInstruction->argument = result;

    method->instructions.push_back(lhsInstruction);
    method->instructions.push_back(rhsInstruction);
    method->instructions.push_back(opInstruction);
    method->instructions.push_back(storeInstruction);
}

UnaryExpression::UnaryExpression(const std::string& op, const std::string& rhs, const std::string& result) : Tac(op, "", rhs, result) {}

std::string UnaryExpression::GetStr() {
    return result + " := " + op + " " + rhs ;
}

void UnaryExpression::GenerateCode(MethodBlock *method)
{
    Instruction* rhsInstruction = new Instruction();
    Instruction* opInstruction = new Instruction();
    Instruction* storeInstruction = new Instruction();

    rhs = (rhs == "true") ? "1" : (rhs == "false") ? "0" : rhs;

    try {
        stoi(rhs);
        rhsInstruction->id = 1;
    } catch (std::invalid_argument& ex)
    {
        rhsInstruction->id = 0;
    }

    rhsInstruction->argument = rhs;

    opInstruction->id = 10;
    storeInstruction->id = 2;
    storeInstruction->argument = result;
    method->instructions.push_back(rhsInstruction);
    method->instructions.push_back(opInstruction);
    method->instructions.push_back(storeInstruction);
}

Copy::Copy(const std::string& lhs, const std::string& result) : Tac("Copy", lhs, "", result) {}

std::string Copy::GetStr() {
    return result + " := " + lhs ;
}

void Copy::GenerateCode(MethodBlock *method) {
    Instruction* lhsInstruction = new Instruction();
    Instruction* storeInstruction = new Instruction();

    lhs = (lhs == "true") ? "1" : (lhs == "false") ? "0" : lhs;

    try {
        std::stoi(lhs);
        lhsInstruction->id = 1;
    } catch (std::invalid_argument& ex)
    {
        lhsInstruction->id = 0;
    }
    lhsInstruction->argument = lhs;

    storeInstruction->id = 2;
    storeInstruction->argument = result;
    method->instructions.push_back(lhsInstruction);
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
    Instruction* lhsInstruction = new Instruction();
    Instruction* jumpInstruction = new Instruction();

    lhs = (lhs == "true") ? "1" : (lhs == "false") ? "0" : lhs;

    try {
        std::stoi(lhs);
        lhsInstruction->id = 1;
    } catch (std::invalid_argument& ex)
    {
        lhsInstruction->id = 0;
    }

    lhsInstruction->argument = lhs;
    jumpInstruction->id = 12;
    jumpInstruction->argument = result;
    method->instructions.push_back(lhsInstruction);
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

    result = (result == "true") ? "1" : (result == "false") ? "0" : result;

    try {
        std::stoi(result);
        parameterInstruction->id = 1;
    } catch (std::invalid_argument& ex)
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
    Instruction* retValueInstruction = new Instruction();
    Instruction* retInstruction = new Instruction();

    result = (result == "true") ? "1" : (result == "false") ? "0" : result;

    try {
        std::stoi(result);
        retValueInstruction->id = 1;
    } catch (std::invalid_argument& ex)
    {
        retValueInstruction->id = 0;
    }

    retValueInstruction->argument = result;
    retInstruction->id = 14;
    method->instructions.push_back(retValueInstruction);
    method->instructions.push_back(retInstruction);
}

Print::Print(const std::string& result) : Tac("print", "", "", result) {}

std::string Print::GetStr() {
    return op + " " + result ;
}

void Print::GenerateCode(MethodBlock *method) {
    Instruction* resultInstruction = new Instruction();
    Instruction* opInstruction = new Instruction();

    result = (result == "true") ? "1" : (result == "false") ? "0" : result;

    try {
        std::stoi(result);
        resultInstruction->id = 1;
    } catch (std::invalid_argument& ex)
    {
        resultInstruction->id = 0;
    }

    resultInstruction->argument = result;

    opInstruction->id = 15;

    method->instructions.push_back(resultInstruction);
    method->instructions.push_back(opInstruction);
}
