#pragma once
#include "../Helper/Token.hpp"

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

struct EvaluatorVisitor
{
    virtual ~EvaluatorVisitor() = default;
    virtual Value visitBinaryExpr(Binary &expr) = 0;
    virtual Value visitGroupingExpr(Grouping &expr) = 0;
    virtual Value visitLiteralExpr(Literal &expr) = 0;
    virtual Value visitUnaryExpr(Unary &expr) = 0;
};