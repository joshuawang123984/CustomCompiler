#pragma once
#include <string>

enum class TokenType
{
    LEFT_PAREN,
    RIGHT_PAREN,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    IDENTIFIER,
    STRING,
    NUMBER,
    EOF_TOKEN
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line_number;

    Token(TokenType type, std::string lexeme, int line_number)
        : type(type), lexeme(lexeme), line_number(line_number) {}
};