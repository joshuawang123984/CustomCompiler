#pragma once

#include "../StatementVisitor.hpp"
#include "../Statement.hpp"

class ReturnStatement : public Statement
{
public:
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStatement(Token keyword, std::unique_ptr<Expr> value) : keyword(std::move(keyword)), value(std::move(value)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitReturnStatement(*this);
    }
};