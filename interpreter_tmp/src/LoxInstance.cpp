#include "../include/LoxStuff/LoxImports.hpp"

Value LoxInstance::get(const std::string &name)
{
    if (fields.find(name) != fields.end())
    {
        return fields.at(name);
    }

    auto method = klass->findMethod(name);
    if (method != nullptr)
    {
        return method->bind(shared_from_this());
    }

    throw std::runtime_error("Undefined property '" + name + "'.");
}
void LoxInstance::set(const std::string &name, const Value &value)
{
    fields[name] = value;
}