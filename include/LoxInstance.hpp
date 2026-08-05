#pragma once
#include "Helper/Types.hpp"
#include <string>
#include <unordered_map>
#include <memory>

class LoxClass;

class LoxInstance : public std::enable_shared_from_this<LoxInstance>
{
private:
    std::shared_ptr<LoxClass> klass;
    std::unordered_map<std::string, Value> fields;

public:
    LoxInstance(std::shared_ptr<LoxClass> klass) : klass(std::move(klass)) {}

    Value get(const std::string &name);
    void set(const std::string &name, const Value &value);
};