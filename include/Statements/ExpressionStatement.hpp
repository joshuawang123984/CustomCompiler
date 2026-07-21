#pragma once

#include "../Visitors/StatementVisitor.hpp"
#include "../Statement.hpp"
#include "../Expr.hpp"

class ExpressionStatement : public Statement
{
public:
    std::unique_ptr<Expr> expression;

    ExpressionStatement(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitExpressionStatement(*this);
    }
};