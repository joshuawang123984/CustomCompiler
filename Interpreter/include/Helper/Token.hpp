#pragma once
#include <string>
#include "Types.hpp"

enum class TokenType
{
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    NOT,
    NOT_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    IDENTIFIER,
    STRING,
    NUMBER,
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUNC,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,
    EOF_TOKEN
};

struct Token
{
public:
    TokenType type;
    std::string lexeme;
    const Value literal;
    int line;

    Token(TokenType type, std::string lexeme, int line_number)
        : type(type), lexeme(lexeme), line(line_number) {}

    Token(TokenType type, std::string lexeme, Value literal, int line_number)
        : type(type), lexeme(lexeme), literal(std::move(literal)), line(line_number) {}

    std::string toString() const
    {
        return std::to_string(static_cast<int>(type)) + " " + lexeme;
    }
};