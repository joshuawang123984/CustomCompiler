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
    NUMBER,
    END
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
};