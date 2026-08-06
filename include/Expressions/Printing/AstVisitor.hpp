#pragma once
#include <iostream>

struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Call;
struct Get;
struct Set;

struct AstVisitor
{
    virtual ~AstVisitor() = default;
    virtual std::string visitBinary(Binary &expr) = 0;
    virtual std::string visitGrouping(Grouping &expr) = 0;
    virtual std::string visitLiteral(Literal &expr) = 0;
    virtual std::string visitUnary(Unary &expr) = 0;
    virtual std::string visitVariable(Variable &expr) = 0;
    virtual std::string visitAssign(Assign &expr) = 0;
    virtual std::string visitCall(Call &expr) = 0;
    virtual std::string visitGet(Get &expr) = 0;
    virtual std::string visitSet(Set &expr) = 0;
};