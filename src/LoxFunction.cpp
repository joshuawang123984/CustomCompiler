#include "../include/LoxFunction.hpp"
#include "../include/Statements/StatementChildren/FuncStatement.hpp"
#include "../include/Environment.hpp"

Value LoxFunction::call(Evaluator &evaluator, std::vector<Value> &arguments)
{
}

size_t LoxFunction::arity() const
{
    return declaration->arguments.size();
}