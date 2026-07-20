#include "Visitors/StatementVisitor.hpp"
#include "TypesAndStructures/Types.hpp"

class Statement
{
public:
    virtual ~Statement() = default;

    virtual Value accept(StatementVisitor &visitor) = 0;
};