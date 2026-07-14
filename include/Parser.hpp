#pragma once
#include <vector>
#include "Token.hpp"
#include "Expr.hpp"

class Parser
{
public:
    explicit Parser(const std::vector<Token> &tokens);
    std::unique_ptr<Expr> parse();

private:
    const std::vector<Token> &tokens;
    int current = 0;

    Token peek();
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type);
    Token consume(TokenType type, const std::string &message);

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
};