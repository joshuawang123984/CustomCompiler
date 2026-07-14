#pragma once
#include <iostream>
#include <string>
#include "Visitor.hpp"
#include "Expr.hpp"

struct ASTPrinter : public Visitor
{
    void visitBinary(Binary &expr) override
    {
        std::cout << "(" << expr.op.lexeme << " ";
        expr.left->accept(*this);
        std::cout << " ";
        expr.right->accept(*this);
        std::cout << ")";
    }

    void visitGrouping(Grouping &expr) override
    {
        std::cout << "(group ";
        expr.expression->accept(*this);
        std::cout << ")";
    }

    void visitLiteral(Literal &expr) override
    {
        std::visit([](auto &&arg)
                   {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "nil";
        } else {
            std::cout << arg;
        } }, expr.value);
    }

    void visitUnary(Unary &expr) override
    {
        std::cout << "(" << expr.op.lexeme << " ";
        expr.right->accept(*this);
        std::cout << ")";
    }
};