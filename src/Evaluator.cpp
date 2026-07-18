#include "../include/Evaluator.hpp"

Value Evaluator::evaluate(Expr &expr)
{
    return expr.evaluate(*this);
}

Value Evaluator::visitBinaryExpr(Binary &expr) {}
Value Evaluator::visitGroupingExpr(Grouping &expr) {}
Value Evaluator::visitLiteralExpr(Literal &expr) {}
Value Evaluator::visitUnaryExpr(Unary &expr) {}