#pragma once

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

struct AstVisitor
{
    virtual ~AstVisitor() = default;
    virtual void visitBinary(Binary &expr) = 0;
    virtual void visitGrouping(Grouping &expr) = 0;
    virtual void visitLiteral(Literal &expr) = 0;
    virtual void visitUnary(Unary &expr) = 0;
};