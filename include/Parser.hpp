#pragma once
#include <vector>
#include "Token.hpp"
#include "TokenVector.hpp"
#include "Expr.hpp"

class Parser
{
public:
    explicit Parser(TokenVector &tokens);
    const TokenVector &getTokens() const;

    std::unique_ptr<Expr> parse();

private:
    const TokenVector &tokenVector;
    int current = 0;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
};