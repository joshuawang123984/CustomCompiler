#include "../include/Compiler.hpp"

Compiler::Compiler(const std::string &source, Chunk &chunk) : scanner(source), tokenVector(source, &start, &current), chunk(chunk) {}
bool Compiler::compile()
{
    //     tokenVector = scanner.scanTokens();

    advance();
    expression();

    consume(TokenType::EOF_TOKEN, "End of File.");
    emitReturn();

    return !hadError;
}

void Compiler::advance() {}
void Compiler::consume(TokenType type, const std::string &message) {}
void Compiler::errorAt(const Token &token, const std::string &message) {}

void Compiler::expression() {}
void Compiler::number() {}
void Compiler::string() {}
void Compiler::grouping() {}
void Compiler::unary() {}
void Compiler::binary() {}
void Compiler::literal() {}

void Compiler::emitByte(uint8_t byte) {}
void Compiler::emitBytes(uint8_t a, uint8_t b) {}
void Compiler::emitConstant(Value value) {}
void Compiler::emitReturn() {}

void Compiler::parsePrecedence(Precedence precedence) {}
ParseRule *Compiler::getRule(TokenType type) {}