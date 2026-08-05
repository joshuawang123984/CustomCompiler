#pragma once
#include "../Helper/Types.hpp"
#include "../Helper/Exceptions.hpp"
#include <vector>
#include <memory>

class Evaluator;

class Callable
{
public:
    virtual ~Callable() = default;
    virtual Value call(Evaluator &evaluator, std::vector<Value> &arguments) = 0;
    virtual size_t arity() const = 0;
};