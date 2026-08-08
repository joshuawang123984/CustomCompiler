#include "Types.hpp"
#include <stdexcept>

class ReturnException : public std::runtime_error
{
public:
    Value value;

    ReturnException(Value value, const std::string &msg) : std::runtime_error(msg), value(std::move(value)) {}
};