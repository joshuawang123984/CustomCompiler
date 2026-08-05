#pragma once
#include "LoxCallable.hpp"

class Evaluator;
class Environment;
class FuncStatement;

class LoxFunction : public Callable
{
public:
    LoxFunction(FuncStatement *declaration, std::shared_ptr<Environment> closure) : declaration(declaration), closure(std::move(closure)) {}

    Value call(Evaluator &evaluator, std::vector<Value> &arguments) override;
    size_t arity() const override;

private:
    FuncStatement *declaration;
    std::shared_ptr<Environment> closure;
};