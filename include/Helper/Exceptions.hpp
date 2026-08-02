
#include "Types.hpp"

class ReturnException
{
public:
    Value value;

    ReturnException(Value value) : value(std::move(value)) {}
};