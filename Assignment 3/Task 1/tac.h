#ifndef TAC_H
#define TAC_H

#include "node.h"
#include <string>

class Tac {
protected:
    std::string op;
    std::string lhs;
    std::string rhs;
    std::string result;
public:
    Tac(const std::string& op, const std::string& lhs, const std::string& rhs, const std::string& result);

    std::string GetOp();
    std::string GetLhs();
    std::string GetRhs();
    std::string GetResult();

    void SetOp(const std::string& op);
    void SetLhs(const std::string& lhs);
    void SetRhs(const std::string& rhs);
    void SetResult(const std::string& result);

    virtual std::string GetStr();

    void Dump();
};

class Expression : public Tac {
public:
    Expression(const std::string& op, const std::string& lhs, const std::string& rhs, const std::string& result);

    std::string GetStr();
};

class UnaryExpression : public Tac {
public:
    UnaryExpression(const std::string& op, const std::string& lhs, const std::string& rhs, const std::string& result);

    std::string GetStr();
};

class Copy : public Tac {
public:
    Copy(const std::string lhs, const std::string result);
    std::string GetStr();
};

class CopyArray : public Tac {
public:
    CopyArray(const std::string& lhs, const std::string& result);

    std::string GetStr();
};

class ArrayAccess : public Tac {
public:
    ArrayAccess(const std::string& lhs, const std::string& idx, const std::string& result);

    std::string GetStr();
};

class Jump : public Tac {
public:
    Jump(const std::string& label);

    std::string GetStr();
};

class MethodCall : public Tac {
public:
    MethodCall(const std::string& function, const std::string& name, const std::string& result);

    std::string GetStr();
};

class CondJump : public Tac {
public:
    CondJump(const std::string& cond, const std::string& label);

    std::string GetStr();
};

class New : public Tac {
public:
    New(const std::string& lhs, const std::string& result);

    std::string GetStr();
};

class NewArray : public Tac {
public:
    NewArray(const std::string& type, const std::string& number, const std::string& result);

    std::string GetStr();
};

class Length : public Tac {
public:
    Length(const std::string& lhs, const std::string& result);
    string GetStr();
};

class Param : public Tac {
public:
    Param(const std::string& result);
    string GetStr();
};

class Return : public Tac {
public:
    Return(const std::string& result)
    string GetStr();
};

class Print : public Tac {
public:
    Print(const std::string& result)
    string GetStr();
};


#endif
