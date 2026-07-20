#pragma once

class StatementVisitor;

class Statement
{
public:
    virtual ~Statement() = default;

    virtual void accept(StatementVisitor &visitor) = 0;
};