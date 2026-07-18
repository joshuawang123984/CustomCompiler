#include "../include/Evaluator.hpp"

Value Evaluator::evaluate(Expr &expr)
{
    return expr.evaluate(*this);
}

Value Evaluator::visitBinaryExpr(Binary &expr) { return nullptr; }
Value Evaluator::visitGroupingExpr(Grouping &expr) { return nullptr; }
Value Evaluator::visitLiteralExpr(Literal &expr) { return expr.value; }
Value Evaluator::visitUnaryExpr(Unary &expr) { return nullptr; }