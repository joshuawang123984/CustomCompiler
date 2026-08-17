#include "../include/Compiler.hpp"
#include "../include/Helper/Obj.hpp"

Compiler::Compiler(const std::string &source, Chunk &chunk) : source(source), tokenVector("", &dummyCurrent, &dummyStart), chunk(chunk) {}
bool Compiler::compile()
{
    Scanner scanner(source);
    tokenVector = scanner.scanTokens();

    while (!tokenVector.check(TokenType::EOF_TOKEN))
    {
        declaration();
    }

    consume(TokenType::EOF_TOKEN, "End of File.");
    emitReturn();

    return !hadError;
}

void Compiler::advance()
{
    tokenVector.token_advance();
}
void Compiler::consume(TokenType type, const std::string &message)
{
    if (tokenVector.check(type))
    {
        advance();
        return;
    }
    errorAt(tokenVector.token_peek(), message);
}

void Compiler::errorAt(const Token &token, const std::string &message)
{
    if (panicMode)
        return;
    std::cout << "[line " << token.line << "] Error (TokenType " << static_cast<int>(tokenVector.previous().type) << "): " << message << std::endl;
    hadError = true;
    panicMode = true;
}

void Compiler::expression()
{
    parsePrecedence(Precedence::ASSIGNMENT);
}
void Compiler::number()
{
    Token token = tokenVector.previous();
    double value = std::stod(token.lexeme);
    emitConstant(Value(value));
}
void Compiler::string()
{
    Token token = tokenVector.previous();
    std::string str = token.lexeme;
    str = str.substr(1, str.length() - 2);
    ObjString *obj = new ObjString(std::move(str));
    emitConstant(Value(obj));
}
void Compiler::grouping()
{
    expression();
    consume(TokenType::RIGHT_PAREN, "')' after expression");
}
void Compiler::unary()
{
    Token operatorToken = tokenVector.previous();
    parsePrecedence(Precedence::UNARY);

    switch (operatorToken.type)
    {
    case TokenType::MINUS:
        emitByte((uint8_t)OpCode::OP_NEGATE);
        break;
    case TokenType::NOT:
        emitByte((uint8_t)OpCode::OP_NOT);
        break;
    default:
        return;
    }
}
void Compiler::binary()
{
    TokenType operatorType = tokenVector.previous().type;
    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence)((int)rule->precedence + 1));

    switch (operatorType)
    {
    case TokenType::PLUS:
        emitByte((uint8_t)OpCode::OP_ADD);
        break;
    case TokenType::MINUS:
        emitByte((uint8_t)OpCode::OP_SUBTRACT);
        break;
    case TokenType::STAR:
        emitByte((uint8_t)OpCode::OP_MULTIPLY);
        break;
    case TokenType::SLASH:
        emitByte((uint8_t)OpCode::OP_DIVIDE);
        break;
    case TokenType::EQUAL_EQUAL:
        emitByte((uint8_t)OpCode::OP_EQUAL);
        break;
    case TokenType::GREATER:
        emitByte((uint8_t)OpCode::OP_GREATER);
        break;
    case TokenType::LESS:
        emitByte((uint8_t)OpCode::OP_LESS);
        break;
    case TokenType::NOT_EQUAL:
        emitBytes((uint8_t)OpCode::OP_EQUAL, (uint8_t)OpCode::OP_NOT);
        break;
    case TokenType::GREATER_EQUAL:
        emitBytes((uint8_t)OpCode::OP_LESS, (uint8_t)OpCode::OP_NOT);
        break;
    case TokenType::LESS_EQUAL:
        emitBytes((uint8_t)OpCode::OP_GREATER, (uint8_t)OpCode::OP_NOT);
        break;
    default:
        return;
    }
}
void Compiler::literal()
{
    Token token = tokenVector.previous();

    switch (token.type)
    {
    case TokenType::TRUE:
        emitByte((uint8_t)OpCode::OP_TRUE);
        break;
    case TokenType::FALSE:
        emitByte((uint8_t)OpCode::OP_FALSE);
        break;
    case TokenType::NIL:
        emitByte((uint8_t)OpCode::OP_NIL);
        break;
    default:
        return;
    }
}

void Compiler::declaration()
{
    Token token = tokenVector.previous();
    if (tokenVector.check(TokenType::PRINT))
    {
        emitByte((uint8_t)OpCode::OP_PRINT);
    }

    else
    {
        expressionStatement();
    }
}

void Compiler::expressionStatement()
{
    expression();
    consume(TokenType::SEMICOLON, "End of Line.");
    emitByte((uint8_t)OpCode::OP_POP);
}
void Compiler::printStatement()
{
    expression();
    consume(TokenType::SEMICOLON, "End of Line.");
    emitByte((uint8_t)OpCode::OP_PRINT);
}

void Compiler::emitByte(uint8_t byte)
{
    chunk.write(byte, tokenVector.previous().line);
}
void Compiler::emitBytes(uint8_t a, uint8_t b)
{
    emitByte(a);
    emitByte(b);
}
void Compiler::emitConstant(Value value)
{
    int index = chunk.addConstant(value);
    emitBytes((uint8_t)OpCode::OP_CONSTANT, (uint8_t)index);
}
void Compiler::emitReturn()
{
    emitByte((uint8_t)OpCode::OP_RETURN);
}

void Compiler::parsePrecedence(Precedence precedence)
{
    advance();

    ParseFn prefixRule = getRule(tokenVector.previous().type)->prefix;
    if (prefixRule == nullptr)
    {
        errorAt(tokenVector.previous(), "Expect expression.");
        return;
    }
    (this->*prefixRule)();

    while (precedence <= getRule(tokenVector.token_peek().type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(tokenVector.previous().type)->infix;
        (this->*infixRule)();
    }
}

ParseRule *Compiler::getRule(TokenType type)
{
    static std::unordered_map<TokenType, ParseRule> rules =
        {
            {TokenType::LEFT_PAREN, {&Compiler::grouping, nullptr, Precedence::NONE}},
            {TokenType::RIGHT_PAREN, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::LEFT_BRACE, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::RIGHT_BRACE, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::COMMA, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::DOT, {nullptr, nullptr, Precedence::NONE}},

            {TokenType::MINUS, {&Compiler::unary, &Compiler::binary, Precedence::TERM}},
            {TokenType::PLUS, {nullptr, &Compiler::binary, Precedence::TERM}},
            {TokenType::SLASH, {nullptr, &Compiler::binary, Precedence::FACTOR}},
            {TokenType::STAR, {nullptr, &Compiler::binary, Precedence::FACTOR}},

            {TokenType::NOT, {&Compiler::unary, nullptr, Precedence::NONE}},
            {TokenType::NOT_EQUAL, {nullptr, &Compiler::binary, Precedence::EQUALITY}},
            {TokenType::EQUAL, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::EQUAL_EQUAL, {nullptr, &Compiler::binary, Precedence::EQUALITY}},
            {TokenType::GREATER, {nullptr, &Compiler::binary, Precedence::COMPARISON}},
            {TokenType::GREATER_EQUAL, {nullptr, &Compiler::binary, Precedence::COMPARISON}},
            {TokenType::LESS, {nullptr, &Compiler::binary, Precedence::COMPARISON}},
            {TokenType::LESS_EQUAL, {nullptr, &Compiler::binary, Precedence::COMPARISON}},

            {TokenType::IDENTIFIER, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::STRING, {&Compiler::string, nullptr, Precedence::NONE}},
            {TokenType::NUMBER, {&Compiler::number, nullptr, Precedence::NONE}},

            {TokenType::AND, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::OR, {nullptr, nullptr, Precedence::NONE}},
            {TokenType::TRUE, {&Compiler::literal, nullptr, Precedence::NONE}},
            {TokenType::FALSE, {&Compiler::literal, nullptr, Precedence::NONE}},
            {TokenType::NIL, {&Compiler::literal, nullptr, Precedence::NONE}},

            {TokenType::EOF_TOKEN, {nullptr, nullptr, Precedence::NONE}},
        };

    static ParseRule emptyRule = {nullptr, nullptr, Precedence::NONE};

    auto it = rules.find(type);
    if (it == rules.end())
        return &emptyRule;
    return &it->second;
}