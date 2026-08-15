#pragma once
#include "Helper/TokenVector.hpp"
#include "Helper/chunk.hpp"
#include "Helper/types.hpp"
#include "Scanner.hpp"

enum class Precedence
{
    NONE,
    ASSIGNMENT, // =
    OR,         // or
    AND,        // and
    EQUALITY,   // == !=
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * /
    UNARY,      // ! -
    CALL,       // . ()
    PRIMARY,
};

class Compiler;

typedef void (Compiler::*ParseFn)();

struct ParseRule
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};
class Compiler
{
private:
    TokenVector tokenVector;
    Chunk &chunk;
    bool hadError = false;
    bool panicMode = false;

    int dummyStart = 0;
    int dummyCurrent = 0;

    std::string source;

    void advance();
    void consume(TokenType type, const std::string &message);
    void errorAt(const Token &token, const std::string &message);

    void expression();
    void number();
    void string();
    void grouping();
    void unary();
    void binary();
    void literal();

    void emitByte(uint8_t byte);
    void emitBytes(uint8_t a, uint8_t b);
    void emitConstant(Value value);
    void emitReturn();

    void parsePrecedence(Precedence precedence);
    ParseRule *getRule(TokenType type);

public:
    Compiler(const std::string &source, Chunk &chunk);
    bool compile();
};