#pragma once
#include <string>
#include <vector>
#include "Token.hpp"

class Lexer
{
public:
    explicit Lexer(const std::string &source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd() const;
    char advance();
    void addToken(TokenType type);
    void scanToken();
};