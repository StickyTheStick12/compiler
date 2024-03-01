#include "tac.h"
#include <utility>

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

void Tac::Dump()
{
    std::cout << result << " := " << lhs << " " << op << " " << rhs << std::endl;
}

Expression::Expression(const std::string& op, const std::string& lhs, const std::string& rhs, const std::string& result) : Tac(op, lhs, rhs, result) {}

std::string Expression::GetStr() {
    return result + " := " + lhs + " " + op + " " + rhs ;
}

UnaryExpression::UnaryExpression(const std::string& op, const std::string& rhs, const std::string& result) : Tac(op, "", rhs, result) {}

std::string UnaryExpression::GetStr() {
    return result + " := " + op + " " + rhs ;
}

Copy::Copy(const std::string& lhs, const std::string& result) : Tac("Copy", lhs, "", result) {}

std::string Copy::GetStr() {
    return result + " := " + lhs ;
}

CopyArray::CopyArray(const std::string& lhs, const std::string& rhs, const std::string& result) : Tac("CopyArray", lhs, rhs, result) {}

std::string CopyArray::GetStr() {
    return result + "[" + lhs + "] := " + rhs ;
}

ArrayAccess::ArrayAccess(const std::string& index, const std::string& lhs, const std::string& result) : Tac("ArrayAccess", lhs, index, result) {}

std::string ArrayAccess::GetStr() {
    return result + " := " + lhs + "[" + rhs + "]" ;
}

MethodCall::MethodCall(const std::string& function, const std::string& name, const std::string& result) : Tac("call", function, name, result) {}

std::string MethodCall::GetStr() {
    return result + " := " + op + " " + lhs + ", " + rhs ;
}

Jump::Jump(const std::string &label) : Tac("goto", "", "", label) {}

std::string Jump::GetStr() {
    return op + " " + result ;
}

CondJump::CondJump(const std::string& cond, const std::string& label) : Tac("iffalse", cond, "goto", label) {}

std::string CondJump::GetStr() {
    return op + " " + lhs + " " + rhs + " " + result ;
}

New::New(const std::string& rhs, const std::string& result) : Tac("new", "", rhs, result) {}

std::string New::GetStr() {
    return result + " := " + op + " " + rhs ;
}

NewArray::NewArray(const std::string& type, const std::string& number, const std::string& result) : Tac("new", type, number, result) {}

std::string NewArray::GetStr() {
    return result + " " + lhs + " " + rhs ;
}

Length::Length(const std::string& lhs, const std::string& result) : Tac("length", lhs, "", result) {}

std::string Length::GetStr() {
    return result + " := " + op + " " + lhs ;
}

Parameter::Parameter(const std::string& result) : Tac("param", "", "", result) {}

std::string Parameter::GetStr() {
    return op + " " + result;
}

Return::Return(const std::string& result) : Tac("return", "", "", result) {}

std::string Return::GetStr() {
    return op + " " + result ;
}

Print::Print(const std::string& result) : Tac("print", "", "", result) {}

std::string Print::GetStr() {
    return op + " " + result ;
}
