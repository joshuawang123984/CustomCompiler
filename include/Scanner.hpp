#pragma once
#include <string>
#include <vector>
#include <map>
#include "Helper/TokenVector.hpp"

class Scanner
{
public:
    explicit Scanner(const std::string &source);
    TokenVector scanTokens();

    const TokenVector &getTokens() const;
    const std::string &getSource() const;

private:
    std::string source;
    TokenVector tokenVector;
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

    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    void number();
    void string();
    void identifier();
    void scanToken();
};