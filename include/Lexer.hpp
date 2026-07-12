#pragma once
#include <string>
#include <vector>
#include <map>
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
    std::map<std::string, TokenType> keywords = {{"and", TokenType::AND},
                                                 {"class", TokenType::CLASS},
                                                 {"else", TokenType::ELSE},
                                                 {"false", TokenType::FALSE},
                                                 {"for", TokenType::FOR},
                                                 {"fun", TokenType::FUN},
                                                 {"if", TokenType::IF},
                                                 {"nil", TokenType::NIL},
                                                 {"or", TokenType::OR},
                                                 {"print", TokenType::PRINT},
                                                 {"return", TokenType::RETURN},
                                                 {"super", TokenType::SUPER},
                                                 {"this", TokenType::THIS},
                                                 {"true", TokenType::TRUE},
                                                 {"var", TokenType::VAR},
                                                 {"while", TokenType::WHILE}};

    bool isAtEnd() const;
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    bool isDigit(char c);
    void number();
    char advance();
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    void identifier();
    void addToken(TokenType type);
    void addToken(TokenType type, double literal);
    void addToken(TokenType type, std::string literal);
    void scanToken();
};