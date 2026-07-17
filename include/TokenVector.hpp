#pragma once
#include <string>
#include <vector>
#include <map>
#include "Token.hpp"

class TokenVector
{
public:
    explicit TokenVector(const std::string &source, int *current, int *start);

    const std::vector<Token> &getTokens() const;
    const std::string &getSource() const;

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
    Token consume(TokenType type, const std::string &message);

    void addToken(TokenType type, int line);
    void addToken(TokenType type, double literal, int line);
    void addToken(TokenType type, std::string literal, int line);
    void addEOF(int line);

private:
    std::vector<Token> tokens;
    const std::string &source;
    int *current;
    int *start;
};