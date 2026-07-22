#include "../include/Statements/Statements.hpp"
#include "Helper/Functions.hpp"
#include "../include/Evaluator.hpp"

void Evaluator::checkNumberOperands(const Value &left, const Value &right)
{
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
        return;
    throw std::runtime_error("Operands must be numbers.");
}

Value Evaluator::evaluate(Expr &expr)
{
    return expr.evaluate(*this);
}

Value Evaluator::visitBinaryExpr(Binary &expr)
{
    Value left = evaluate(*expr.left);
    Value right = evaluate(*expr.right);

    switch (expr.op.type)
    {
    case TokenType::PLUS:
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            return std::get<double>(left) + std::get<double>(right);

        if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
            return std::get<std::string>(left) + std::get<std::string>(right);

        throw std::runtime_error("Operands must be two numbers or two strings.");
    case TokenType::MINUS:
        checkNumberOperands(left, right);
        return std::get<double>(left) - std::get<double>(right);
    case TokenType::STAR:
        checkNumberOperands(left, right);
        return std::get<double>(left) * std::get<double>(right);
    case TokenType::SLASH:
        checkNumberOperands(left, right);
        return std::get<double>(left) / std::get<double>(right);

    case TokenType::EQUAL_EQUAL:
        return left == right;
    case TokenType::NOT_EQUAL:
        return left != right;
    case TokenType::GREATER:
        checkNumberOperands(left, right);
        return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(left, right);
        return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESS:
        checkNumberOperands(left, right);
        return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESS_EQUAL:
        checkNumberOperands(left, right);
        return std::get<double>(left) <= std::get<double>(right);
    default:
        throw std::runtime_error("Unknown binary operator.");
    }
    return nullptr;
}
Value Evaluator::visitGroupingExpr(Grouping &expr)
{
    return evaluate(*expr.expression);
}
Value Evaluator::visitLiteralExpr(Literal &expr)
{
    return expr.value;
}
Value Evaluator::visitUnaryExpr(Unary &expr)
{
    Value right = evaluate(*expr.right);

    switch (expr.op.type)
    {
    case TokenType::NOT:
        if (std::holds_alternative<bool>(right))
            return !std::get<bool>(right);

        throw std::runtime_error("Operand must be bool");
    case TokenType::MINUS:
        if (std::holds_alternative<double>(right))
            return std::get<double>(right) * -1;

        throw std::runtime_error("Operand must be number");
    default:
        throw std::runtime_error("Unknown unary operator.");
    }

    return nullptr;
}

Value Evaluator::visitVariableExpr(Variable &expr)
{
    return environment->get(expr.name);
}

void Evaluator::visitVarStatement(VarStatement &stmt)
{
    Value val = nullptr;
    if (stmt.initializer)
    {
        val = evaluate(*stmt.initializer);
    }

    environment->define(stmt.name, val);
}
void Evaluator::visitPrintStatement(PrintStatement &stmt)
{
    Value val = evaluate(*stmt.expression);
    std::cout << stringify(val) << std::endl;
}

void Evaluator::visitExpressionStatement(ExpressionStatement &stmt)
{
    evaluate(*stmt.expression);
}