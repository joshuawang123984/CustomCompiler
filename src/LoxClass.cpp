#include "../include/LoxStuff/LoxImports.hpp"

std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string &methodName) const
{
    if (methods.find(methodName) != methods.end())
    {
        return methods.at(methodName);
    }
    return nullptr;
}

Value LoxClass::call(Evaluator &evaluator, std::vector<Value> &arguments)
{
    auto instance = std::make_shared<LoxInstance>(shared_from_this());

    auto initializer = findMethod("init");
    if (initializer != nullptr)
    {
        initializer->bind(instance)->call(evaluator, arguments);
    }

    return instance;
}
size_t LoxClass::arity() const
{
    auto initializer = findMethod("init");
    if (initializer == nullptr)
        return 0;
    return initializer->arity();
}