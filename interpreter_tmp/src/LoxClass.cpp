#include "../include/LoxStuff/LoxImports.hpp"

std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string &methodName) const
{
    auto it = methods.find(methodName);
    if (it != methods.end())
    {
        return it->second;
    }

    if (superclass != nullptr)
    {
        return superclass->findMethod(methodName);
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