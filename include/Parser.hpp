#pragma once
#include <vector>
#include "Token.hpp"
#include "Expr.hpp"

class Parser
{
public:
    explicit Parser(const std::vector<Token> &tokens);
    Expr *parse();

private:
    const std::vector<Token> &tokens;
    int current = 0;

    Token peek();
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type);
    Token consume(TokenType type, const std::string &message);
};