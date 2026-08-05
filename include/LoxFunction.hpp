#pragma once
#include "Helper/Types.hpp"
#include "Helper/Exceptions.hpp"
#include <vector>
#include <memory>

class Evaluator;
class Environment;
class FuncStatement;

// LoxClass uses this classes as a base, might separate this (maybe move to Helper folder?)
class Callable
{
public:
    virtual ~Callable() = default;
    virtual Value call(Evaluator &evaluator, std::vector<Value> &arguments) = 0;
    virtual size_t arity() const = 0;
};

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