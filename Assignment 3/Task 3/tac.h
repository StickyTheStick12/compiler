#ifndef TAC_H
#define TAC_H

#include "Node.h"
#include "bytecode.h"

class Tac {
protected:
    std::string op;
    std::string lhs;
    std::string rhs;
    std::string result;
public:
    Tac(std::string op, std::string lhs, std::string rhs, std::string result);

    virtual void GenerateCode(MethodBlock* method);
    virtual std::string GetStr();
};

class Expression : public Tac {
public:
    Expression(const std::string& op, const std::string& lhs, const std::string& rhs, const std::string& result);

    void GenerateCode(MethodBlock* method) override;
    std::string GetStr() override;
};

class UnaryExpression : public Tac {
public:
    UnaryExpression(const std::string& op, const std::string& rhs, const std::string& result);

    void GenerateCode(MethodBlock* method) override;
    std::string GetStr() override;
};

class Copy : public Tac {
public:
    Copy(const std::string& lhs, const std::string& result);
    void GenerateCode(MethodBlock* method) override;
    std::string GetStr() override;
};

class CopyArray : public Tac {
public:
    CopyArray(const std::string& lhs, const std::string& rhs, const std::string& result);
    void GenerateCode(MethodBlock* method) override;
    std::string GetStr() override;
};

class ArrayAccess : public Tac {
public:
    ArrayAccess(const std::string& lhs, const std::string& idx, const std::string& result);
    void GenerateCode(MethodBlock* method) override;
    std::string GetStr() override;
};

class Jump : public Tac {
public:
    Jump(const std::string& label);
    void GenerateCode(MethodBlock* method) override;
    std::string GetStr() override;
};

class MethodCall : public Tac {
public:
    MethodCall(const std::string& function, const std::string& name, const std::string& result);
    void GenerateCode(MethodBlock* method) override;
    std::string GetStr() override;
};

class CondJump : public Tac {
public:
    CondJump(const std::string& cond, const std::string& label);
    void GenerateCode(MethodBlock* method) override;

    std::string GetStr() override;
};

class New : public Tac {
public:
    New(const std::string& lhs, const std::string& result);
    void GenerateCode(MethodBlock* method) override;

    std::string GetStr() override;
};

class NewArray : public Tac {
public:
    NewArray(const std::string& type, const std::string& number, const std::string& result);
    std::string GetStr() override;
    void GenerateCode(MethodBlock* method) override;
};

class Length : public Tac {
public:
    Length(const std::string& lhs, const std::string& result);
    std::string GetStr() override;
    void GenerateCode(MethodBlock* method) override;
};

class Parameter : public Tac {
public:
    Parameter(const std::string& result);
    std::string GetStr() override;
    void GenerateCode(MethodBlock* method) override;
};

class Return : public Tac {
public:
    Return(const std::string& result);
    std::string GetStr() override;
    void GenerateCode(MethodBlock* method) override;
};

class Print : public Tac {
public:
    Print(const std::string& result);
    std::string GetStr() override;
    void GenerateCode(MethodBlock* method) override;
};

#endif
