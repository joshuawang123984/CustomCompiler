#pragma once

#include "../StatementVisitor.hpp"
#include "../Statement.hpp"
#include "../../Expressions/Expr.hpp"

class VarStatement : public Statement
{
public:
    const std::string name;
    std::unique_ptr<Expr> initializer;

    VarStatement(std::string name, std::unique_ptr<Expr> initializer) : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitVarStatement(*this);
    }
};