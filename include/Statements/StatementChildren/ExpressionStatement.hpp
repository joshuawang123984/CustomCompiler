#pragma once

#include "../StatementVisitor.hpp"
#include "../Statement.hpp"
#include "../../Expressions/Expr.hpp"

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