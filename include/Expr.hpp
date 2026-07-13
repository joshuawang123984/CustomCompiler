#pragma once
#include <memory>
#include "Token.hpp"
#include "Visitor.hpp"

struct Expr;
struct Binary;
struct Grouping;
struct Literal;
struct Unary;

struct Expr
{
    virtual ~Expr() = default;
    virtual void accept(Visitor &visitor) = 0;
};

struct Binary : public Expr
{
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    void accept(Visitor &visitor) override
    {
        visitor.visitBinary(*this);
    }
};

struct Literal : public Expr
{
    Literal(LiteralValue val) : value(val) {}
    LiteralValue value;

    void accept(Visitor &visitor) override
    {
        visitor.visitLiteral(*this);
    }
};

struct Unary : public Expr
{
    Token op;
    std::unique_ptr<Expr> right;

    Unary(Token o, std::unique_ptr<Expr> r) : op(o), right(std::move(r)) {}

    void accept(Visitor &visitor) override
    {
        visitor.visitUnary(*this);
    }
};

struct Grouping : public Expr
{
    std::unique_ptr<Expr> expression;
    Grouping(std::unique_ptr<Expr> e) : expression(std::move(e)) {}

    void accept(Visitor &visitor) override
    {
        visitor.visitGrouping(*this);
    }
};