#pragma once
#include "Helper/TokenVector.hpp"
#include "Helper/chunk.hpp"
#include "Helper/types.hpp"
#include "Helper/Obj.hpp"
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

enum class FunctionType
{
    TYPE_FUNCTION,
    TYPE_SCRIPT,
};

class Compiler;
struct Table;

typedef void (Compiler::*ParseFn)(bool canAssign);

struct ParseRule
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

struct Local
{
    Token name;
    int depth;
};

struct UpvalueInfo
{
    uint8_t index;
    bool isLocal;
};

class Compiler
{
private:
    std::string source;
    TokenVector &tokenVector;

    bool hadError = false;
    bool panicMode = false;

    int scopeDepth = 0;
    std::vector<Local> locals;
    Table &strings;

    FunctionType functionType;
    LoxFunction *function;

    Compiler *enclosing = nullptr;
    std::vector<UpvalueInfo> upvalues;

    void compileFunction(FunctionType type, ObjString *nameObj);
    uint8_t argumentList();

    void advance();
    void consume(TokenType type, const std::string &message);
    void errorAt(const Token &token, const std::string &message);

    int addUpvalue(uint8_t index, bool isLocal);
    int resolveLocal(const Token &name);
    int resolveUpvalue(const Token &name);

    void expression();
    void _and(bool canAssign);
    void _or(bool canAssign);
    void call(bool canAssign);
    void number(bool canAssign);
    void string(bool canAssign);
    void grouping(bool canAssign);
    void unary(bool canAssign);
    void binary(bool canAssign);
    void literal(bool canAssign);
    void variable(bool canAssign);

    void declaration();
    void statement();
    void expressionStatement();
    void printStatement();
    void ifStatement();
    void whileStatement();
    void forStatement();
    void returnStatement();
    void block();
    void varDeclaration();
    void funcDeclaration();

    void emitByte(uint8_t byte);
    void emitBytes(uint8_t a, uint8_t b);
    void emitConstant(Value value);
    void emitReturn();
    uint8_t emitJump(uint8_t opcode);
    void patchJump(int offset);
    void emitLoop(int loopStart);

    void parsePrecedence(Precedence precedence);
    ParseRule *getRule(TokenType type);

    void beginScope();
    void endScope();

    Chunk *currentChunk() { return &function->chunk; }

public:
    Compiler(TokenVector &tokenVector, Table &strings);
    Compiler(TokenVector &tokenVector, Table &strings, FunctionType type);

    ObjString *copyString(const std::string &text);
    bool compile();

    LoxFunction *getFunction() { return function; }
};