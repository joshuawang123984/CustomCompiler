#include "StatementVisitor.hpp"
#include "Types.hpp"

class Statement
{
public:
    virtual ~Statement() = default;

    virtual Value accept(StatementVisitor &visitor) = 0;
};