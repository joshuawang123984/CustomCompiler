#pragma once
#include <iostream>

struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;

struct AstVisitor
{
    virtual ~AstVisitor() = default;
    virtual std::string visitBinary(Binary &expr) = 0;
    virtual std::string visitGrouping(Grouping &expr) = 0;
    virtual std::string visitLiteral(Literal &expr) = 0;
    virtual std::string visitUnary(Unary &expr) = 0;
    virtual std::string visitVariable(Variable &expr) = 0;
};