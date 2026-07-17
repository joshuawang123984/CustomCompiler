#pragma once
#include <memory>
#include "Token.hpp"
#include "Visitor.hpp"

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

    Binary(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r) : left(std::move(l)), op(o), right(std::move(r)) {}

    void accept(Visitor &visitor) override
    {
        return visitor.visitBinary(*this);
    }
};

struct Grouping : public Expr
{
    std::unique_ptr<Expr> expression;
    Grouping(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}

    void accept(Visitor &visitor) override
    {
        return visitor.visitGrouping(*this);
    }
};

using LiteralValue = std::variant<std::monostate, double, std::string, bool>;

struct Literal : public Expr
{
    LiteralValue value;
    Literal(LiteralValue val) : value(val) {}

    void accept(Visitor &visitor) override
    {
        return visitor.visitLiteral(*this);
    }
};

struct Unary : public Expr
{
    Token op;
    std::unique_ptr<Expr> right;

    Unary(Token o, std::unique_ptr<Expr> r) : op(o), right(std::move(r)) {}

    void accept(Visitor &visitor) override
    {
        return visitor.visitUnary(*this);
    }
};
