#pragma once

#include "../StatementVisitor.hpp"
#include "../Statement.hpp"

class ReturnStatement : public Statement
{
public:
    ReturnStatement() {}

    void accept(StatementVisitor &visitor) override
    {
        visitor.visitReturnStatement(*this);
    }
};