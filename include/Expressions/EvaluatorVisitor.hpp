#pragma once
#include "../Helper/Token.hpp"

struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Call;
struct Get;
struct Set;
struct Super;

struct EvaluatorVisitor
{
    virtual ~EvaluatorVisitor() = default;
    virtual Value visitBinaryExpr(Binary &expr) = 0;
    virtual Value visitGroupingExpr(Grouping &expr) = 0;
    virtual Value visitLiteralExpr(Literal &expr) = 0;
    virtual Value visitUnaryExpr(Unary &expr) = 0;
    virtual Value visitVariableExpr(Variable &expr) = 0;
    virtual Value visitAssignExpr(Assign &expr) = 0;
    virtual Value visitCallExpr(Call &expr) = 0;
    virtual Value visitGetExpr(Get &expr) = 0;
    virtual Value visitSetExpr(Set &expr) = 0;
    virtual Value visitSuperExpr(Super &expr) = 0;
};