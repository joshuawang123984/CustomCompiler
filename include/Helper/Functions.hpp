#include <iostream>
#include "Types.hpp"

std::string stringify(Value value)
{
    if (std::holds_alternative<double>(value))
        return std::to_string(std::get<double>(value));

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value) ? "true" : "false";

    if (std::holds_alternative<std::string>(value))
        return std::get<std::string>(value);

    if (std::holds_alternative<std::nullptr_t>(value))
        return "nil";

    return "unknown";
}