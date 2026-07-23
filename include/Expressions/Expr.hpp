#pragma once
#include <memory>
#include "Helper/Token.hpp"
#include "Printing/AstVisitor.hpp"
#include "EvaluatorVisitor.hpp"

struct Expr
{
    virtual ~Expr() = default;
    virtual std::string accept(AstVisitor &visitor) = 0;
    virtual Value evaluate(EvaluatorVisitor &visitor) = 0;
};

struct Binary : public Expr
{
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r) : left(std::move(l)), op(o), right(std::move(r)) {}

    std::string accept(AstVisitor &visitor) override
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

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitGrouping(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitGroupingExpr(*this);
    }
};

struct Literal : public Expr
{
    Value value;
    Literal(Value val) : value(val) {}

    std::string accept(AstVisitor &visitor) override
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

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitUnary(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitUnaryExpr(*this);
    }
};

struct Variable : public Expr
{
public:
    const std::string name;

    Variable(std::string name) : name(std::move(name)) {}

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitVariable(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitVariableExpr(*this);
    }
};