#include "../include/LoxStuff/LoxFunction.hpp"
#include "../include/Statements/StatementChildren/FuncStatement.hpp"
#include "../include/Environment.hpp"
#include "../include/Evaluator.hpp"

std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> instance)
{
    auto environment = std::make_shared<Environment>(closure);
    environment->define("this", instance);
    return std::make_shared<LoxFunction>(declaration, environment);
}

Value LoxFunction::call(Evaluator &evaluator, std::vector<Value> &arguments)
{
    auto callEnv = std::make_shared<Environment>(closure);

    for (size_t i = 0; i < declaration->arguments.size(); ++i)
        callEnv->define(declaration->arguments[i].lexeme, arguments[i]);

    auto previous = evaluator.getEnvironment();
    evaluator.setEnvironment(callEnv);

    Value result = nullptr;

    try
    {
        declaration->body->accept(evaluator);
    }
    catch (ReturnException &ret)
    {
        result = ret.value;
    }

    evaluator.setEnvironment(previous);
    return result;
}

size_t LoxFunction::arity() const
{
    return declaration->arguments.size();
}