#pragma once
#include "Printing/AstVisitor.hpp"
#include "EvaluatorVisitor.hpp"
#include <vector>

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
    const Token name;

    Variable(Token name) : name(std::move(name)) {}

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitVariable(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitVariableExpr(*this);
    }
};

struct Assign : public Expr
{
public:
    Token name;
    std::unique_ptr<Expr> value;

    Assign(Token name, std::unique_ptr<Expr> value) : name(std::move(name)), value(std::move(value)) {}

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitAssign(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitAssignExpr(*this);
    }
};

struct Call : public Expr
{
public:
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;

    Call(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments) : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitCall(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitCallExpr(*this);
    }
};

struct Get : public Expr
{
private:
public:
    std::unique_ptr<Expr> object;
    Token name;

    Get(std::unique_ptr<Expr> object, Token name) : object(std::move(object)), name(std::move(name)) {}

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitGet(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitGetExpr(*this);
    }
};

struct Set : public Expr
{
private:
public:
    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;

    Set(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value) : object(std::move(object)), name(std::move(name)), value(std::move(value)) {}

    std::string accept(AstVisitor &visitor) override
    {
        return visitor.visitSet(*this);
    }

    Value evaluate(EvaluatorVisitor &visitor) override
    {
        return visitor.visitSetExpr(*this);
    }
};