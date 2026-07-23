#include <iostream>
#include <unordered_map>
#include "Helper/Types.hpp"

class Environment
{
private:
    std::unordered_map<std::string, Value> values;

public:
    Environment *enclosing;

    Environment() : enclosing(nullptr) {}
    Environment(Environment *enclosing) : enclosing(enclosing) {}

    void define(const std::string &name, Value value)
    {
        values[name] = value;
    }

    Value get(const std::string name)
    {
        if (values.find(name) != values.end())
        {
            return values[name];
        }

        throw std::runtime_error("Undefined variable '" + name + "'.");
    }
};