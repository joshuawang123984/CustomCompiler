#pragma once
#include <vector>
#include "TypesAndStructures/Token.hpp"
#include "TypesAndStructures/TokenVector.hpp"
#include "Expr.hpp"

class Parser
{
public:
    explicit Parser(TokenVector &tokens);
    const TokenVector &getTokens() const;

    std::unique_ptr<Expr> parse();

private:
    TokenVector tokenVector;
    int current = 0;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
};