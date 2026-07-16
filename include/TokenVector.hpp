#pragma once
#include <string>
#include <vector>
#include <map>
#include "Token.hpp"

class TokenVector
{
public:
    explicit TokenVector(std::string &source);

    const std::vector<Token> &getTokens() const;
    const std::string &getSource() const;

private:
    std::vector<Token> tokens;
    std::string source;
    int current = 0;

    bool isAtEnd() const;
    bool check(TokenType type);
    bool char_match(char expected);
    bool token_match(TokenType type);
    char char_peek();
    Token token_peek();
    char peekNext();
    char char_advance();
    Token token_advance();
    Token previous();
    // maybe add consume as well

    void addToken(TokenType type, int start, int line);
    void addToken(TokenType type, double literal, int start, int line);
    void addToken(TokenType type, std::string literal, int start, int line);
    void scanToken();
};