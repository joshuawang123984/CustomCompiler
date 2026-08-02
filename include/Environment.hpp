#pragma once
#include <iostream>
#include <unordered_map>
#include "Helper/Types.hpp"

class Environment
{
private:
    std::unordered_map<std::string, Value> values;

public:
    std::shared_ptr<Environment> enclosing;

    Environment() : enclosing(nullptr) {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    std::unordered_map<std::string, Value> getValues()
    {
        return values;
    }

    void define(const std::string &name, Value value)
    {
        values[name] = value;
    }

    void assign(const std::string &name, const Value &value)
    {
        if (values.find(name) != values.end())
        {
            values[name] = value;
            return;
        }

        else if (enclosing != nullptr)
        {
            enclosing->assign(name, value);
            return;
        }

        else
            throw std::runtime_error("variable already exists: '" + name + "'.");
    }

    Value get(const std::string name)
    {
        if (values.find(name) != values.end())
        {
            return values[name];
        }

        if (enclosing != nullptr)
        {
            return enclosing->get(name);
        }

        throw std::runtime_error("Undefined variable '" + name + "'.");
    }
};