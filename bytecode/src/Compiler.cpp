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
void Compiler::number(bool canAssign)
{
    Token token = tokenVector.previous();
    double value = std::stod(token.lexeme);
    emitConstant(Value(value));
}
void Compiler::string(bool canAssign)
{
    Token token = tokenVector.previous();
    std::string str = token.lexeme;
    str = str.substr(1, str.length() - 2);
    ObjString *obj = new ObjString(std::move(str));
    emitConstant(Value(obj));
}
void Compiler::grouping(bool canAssign)
{
    expression();
    consume(TokenType::RIGHT_PAREN, "')' after expression");
}
void Compiler::unary(bool canAssign)
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
void Compiler::binary(bool canAssign)
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
void Compiler::literal(bool canAssign)
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

void Compiler::variable(bool canAssign)
{
    Token identifier = tokenVector.previous();

    int slot = -1;
    for (int i = locals.size() - 1; i >= 0; --i)
    {
        if (locals[i].name.lexeme == identifier.lexeme)
        {
            slot = i;
            break;
        }
    }

    if (canAssign && tokenVector.check(TokenType::EQUAL))
    {
        advance();
        expression();

        if (slot != -1)
            emitBytes((uint8_t)OpCode::OP_SET_LOCAL, (uint8_t)slot);
        else
        {
            ObjString *nameObj = new ObjString(identifier.lexeme);
            int nameConstant = chunk.addConstant(Value(nameObj));
            emitBytes((uint8_t)OpCode::OP_SET_GLOBAL, (uint8_t)nameConstant);
        }
        return;
    }

    if (slot != -1)
    {
        emitBytes((uint8_t)OpCode::OP_GET_LOCAL, (uint8_t)slot);
        return;
    }

    ObjString *nameObj = new ObjString(identifier.lexeme);
    int nameConstant = chunk.addConstant(Value(nameObj));
    emitBytes((uint8_t)OpCode::OP_GET_GLOBAL, (uint8_t)nameConstant);
}

void Compiler::declaration()
{
    Token token = tokenVector.previous();
    if (tokenVector.check(TokenType::VAR))
    {
        advance();
        varDeclaration();
    }
    else
    {
        statement();
    }
}

void Compiler::statement()
{
    Token token = tokenVector.previous();
    if (tokenVector.check(TokenType::PRINT))
    {
        advance();
        printStatement();
    }

    else if (tokenVector.check(TokenType::LEFT_BRACE))
    {
        advance();
        block();
    }

    else if (tokenVector.check(TokenType::IF))
    {
        advance();
        ifStatement();
    }

    else if (tokenVector.check(TokenType::WHILE))
    {
        advance();
        whileStatement();
    }
    else
    {
        expressionStatement();
    }
}

void Compiler::expressionStatement()
{
    expression();
    consume(TokenType::SEMICOLON, "End of Line (expression statement).");
    emitByte((uint8_t)OpCode::OP_POP);
}
void Compiler::printStatement()
{
    expression();
    consume(TokenType::SEMICOLON, "End of Line (print statement.");
    emitByte((uint8_t)OpCode::OP_PRINT);
}
void Compiler::ifStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after if");
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if");

    int thenJump = emitJump((uint8_t)OpCode::OP_JUMP_IF_FALSE);
    emitByte((uint8_t)OpCode::OP_POP);
    statement();

    int elseJump = emitJump((uint8_t)OpCode::OP_JUMP);
    patchJump(thenJump);

    emitByte((uint8_t)OpCode::OP_POP);

    if (tokenVector.check(TokenType::ELSE))
    {
        advance();
        statement();
    }

    patchJump(elseJump);
}
void Compiler::whileStatement()
{
    int loopStart = chunk.code.size();
    consume(TokenType::LEFT_PAREN, "Expect '(' after while");
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition");

    int exitJump = emitJump((uint8_t)OpCode::OP_JUMP_IF_FALSE);
    emitByte((uint8_t)OpCode::OP_POP);
    statement();
    emitLoop(loopStart);

    patchJump(exitJump);
    emitByte((uint8_t)OpCode::OP_POP);
}
void Compiler::block()
{
    beginScope();
    while (!tokenVector.check(TokenType::RIGHT_BRACE) && !tokenVector.check(TokenType::EOF_TOKEN))
    {
        declaration();
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    endScope();
}

void Compiler::varDeclaration()
{
    consume(TokenType::IDENTIFIER, "Expect identifier after 'var'.");
    Token token = tokenVector.previous();
    ObjString *nameObj = new ObjString(token.lexeme);
    int nameConstant = chunk.addConstant(Value(nameObj));

    if (tokenVector.check(TokenType::EQUAL))
    {
        advance();
        expression();
    }
    else
    {
        emitByte((uint8_t)OpCode::OP_NIL);
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

    if (scopeDepth == 0)
    {
        emitBytes((uint8_t)OpCode::OP_DEFINE_GLOBAL, (uint8_t)nameConstant);
        return;
    }

    locals.push_back({token, scopeDepth});
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

uint8_t Compiler::emitJump(uint8_t opcode)
{
    emitByte((uint8_t)opcode);
    emitBytes(0xFF, 0xFF);
    return chunk.code.size() - 2;
}
void Compiler::patchJump(int offset)
{
    int jump = chunk.code.size() - offset - 2;
    if (jump > 0xFFFF)
    {
        errorAt(tokenVector.previous(), "Too much to jump over.");
    }

    chunk.code[offset] = (jump >> 8) & 0xFF;
    chunk.code[offset + 1] = jump & 0xFF;
}
void Compiler::emitLoop(int loopStart)
{
    emitByte((uint8_t)OpCode::OP_LOOP);

    int offset = chunk.code.size() - loopStart + 2;

    if (offset > 0xFFFF)
        errorAt(tokenVector.previous(), "Loop body too large.");

    emitByte((offset >> 8) & 0xFF);
    emitByte(offset & 0xFF);
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
    bool canAssign = precedence <= Precedence::ASSIGNMENT;
    (this->*prefixRule)(canAssign);

    while (precedence <= getRule(tokenVector.token_peek().type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(tokenVector.previous().type)->infix;
        (this->*infixRule)(canAssign);
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

            {TokenType::IDENTIFIER, {&Compiler::variable, nullptr, Precedence::NONE}},
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

void Compiler::beginScope()
{
    scopeDepth++;
}
void Compiler::endScope()
{
    scopeDepth--;

    while (!locals.empty() && locals.back().depth > scopeDepth)
    {
        emitByte((uint8_t)OpCode::OP_POP);
        locals.pop_back();
    }
}