#pragma once
#include <vector>
#include "Token.hpp"

class Parser
{
public:
    explicit Parser(const std::vector<Token> &tokens);

private:
    const std::vector<Token> &tokens;
    int current = 0;

    Token previous();
    Token peek();
    bool isAtEnd();
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type);
    Token consume(TokenType type, const std::string &message);
};