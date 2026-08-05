#pragma once
#include "LoxCallable.hpp"
#include <string>
#include <unordered_map>
#include <memory>

class LoxFunction;

class LoxClass : public Callable, public std::enable_shared_from_this<LoxClass>
{
private:
    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;

public:
    std::string name;

    LoxClass(std::string name, std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods) : methods(std::move(methods)), name(std::move(name)) {}
    std::shared_ptr<LoxFunction> findMethod(const std::string &methodName) const;

    Value call(Evaluator &evaluator, std::vector<Value> &arguments) override;
    size_t arity() const override;
};