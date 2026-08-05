#pragma once
#include "LoxFunction.hpp"
#include <string>
#include <unordered_map>
#include <memory>

class LoxFunction;

class LoxClass : public Callable
{
private:
    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;

public:
    std::string name;

    LoxClass(std::string name, std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods) : name(std::move(name)), methods(std::move(methods)) {}
    std::shared_ptr<LoxFunction> findMethod(const std::string &methodName);

    Value call(Evaluator &evaluator, std::vector<Value> &arguments) override;
    size_t arity() const override;
};