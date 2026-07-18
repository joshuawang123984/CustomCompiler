#pragma once
#include <memory>

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

using Value = std::variant<double, bool, std::string, std::nullptr_t>;

struct EvaluatorVisitor
{
    virtual ~EvaluatorVisitor() = default;
    virtual Value visitBinaryExpr(Binary &expr) = 0;
    virtual Value visitGroupingExpr(Grouping &expr) = 0;
    virtual Value visitLiteralExpr(Literal &expr) = 0;
    virtual Value visitUnaryExpr(Unary &expr) = 0;
};