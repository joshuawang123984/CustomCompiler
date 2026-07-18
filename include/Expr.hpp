#pragma once
#include <memory>
#include "Token.hpp"
#include "AstVisitor.hpp"
#include "EvaluatorVisitor.hpp"

struct Expr
{
    virtual ~Expr() = default;
    virtual void accept(AstVisitor &visitor) = 0;
    virtual Value evaluate(EvaluatorVisitor &visitor) = 0;
};

struct Binary : public Expr
{
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r) : left(std::move(l)), op(o), right(std::move(r)) {}

    void accept(AstVisitor &visitor) override
    {
        return visitor.visitBinary(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitBinaryExpr(*this);
    }
};

struct Grouping : public Expr
{
    std::unique_ptr<Expr> expression;
    Grouping(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}

    void accept(AstVisitor &visitor) override
    {
        return visitor.visitGrouping(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitGroupingExpr(*this);
    }
};

using LiteralValue = std::variant<std::monostate, double, std::string, bool>;

struct Literal : public Expr
{
    LiteralValue value;
    Literal(LiteralValue val) : value(val) {}

    void accept(AstVisitor &visitor) override
    {
        return visitor.visitLiteral(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitLiteralExpr(*this);
    }
};

struct Unary : public Expr
{
    Token op;
    std::unique_ptr<Expr> right;

    Unary(Token o, std::unique_ptr<Expr> r) : op(o), right(std::move(r)) {}

    void accept(AstVisitor &visitor) override
    {
        return visitor.visitUnary(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitUnaryExpr(*this);
    }
};
