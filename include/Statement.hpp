#pragma once
#include "Visitors/StatementVisitor.hpp"
#include "Helper/Types.hpp"

class Statement
{
public:
    virtual ~Statement() = default;

    virtual Value accept(StatementVisitor &visitor) = 0;
};