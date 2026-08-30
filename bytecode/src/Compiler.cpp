#include "../include/Compiler.hpp"
#include "../include/Memory.hpp"
#include "../include/Helper/Obj.hpp"
#include "../include/Helper/functions.hpp"
#include "../hashmap/include/Table.hpp"

static ObjString *TOMBSTONE = reinterpret_cast<ObjString *>(0x1);

static uint32_t hashString(const std::string &str)
{
    uint32_t hash = 2166136261u;
    for (char c : str)
    {
        hash ^= (uint8_t)c;
        hash *= 16777619;
    }
    return hash;
}

static ObjString *tableFindString(Table *table, const std::string &text, uint32_t hash)
{
    if (table->capacity == 0)
        return nullptr;

    uint32_t index = hash % table->capacity;

    for (;;)
    {
        Entry *entry = &table->entries[index];

        if (entry->key == nullptr)
        {
            return nullptr;
        }
        else if (entry->key != TOMBSTONE && entry->key->hash == hash && entry->key->chars == text)
        {
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}

uint8_t Compiler::argumentList()
{
    uint8_t argCount = 0;
    if (!tokenVector.check(TokenType::RIGHT_PAREN))
    {
        do
        {
            expression();
            argCount++;
        } while (tokenVector.token_match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return argCount;
}

ObjString *Compiler::copyString(const std::string &text)
{
    uint32_t hash = hashString(text);

    ObjString *interned = tableFindString(&strings, text, hash);

    if (interned != nullptr)
    {
        return interned;
    }

    ObjString *string = gc.allocateObject<ObjString>(text);
    string->hash = hash;
    tableSet(&strings, string, Value{});

    if (interned != nullptr)
    {
        std::cout << "found existing\n";
        return interned;
    }

    return string;
}

Compiler::Compiler(TokenVector &tokenVector, Table &strings, GarbageCollector &gc)
    : tokenVector(tokenVector), strings(strings), gc(gc), functionType(FunctionType::TYPE_SCRIPT)
{
    function = gc.allocateObject<LoxFunction>();
}

Compiler::Compiler(TokenVector &tokenVector, Table &strings, GarbageCollector &gc, FunctionType type)
    : tokenVector(tokenVector), strings(strings), gc(gc), functionType(type)
{
    function = gc.allocateObject<LoxFunction>();
}

void Compiler::compileFunction(FunctionType type, ObjString *nameObj)
{
    Compiler functionCompiler(tokenVector, strings, gc, type);
    functionCompiler.enclosing = this;

    functionCompiler.function->name = nameObj;

    functionCompiler.consume(TokenType::LEFT_PAREN, "Expect '(' after func name.");
    if (!functionCompiler.tokenVector.check(TokenType::RIGHT_PAREN))
    {
        do
        {
            functionCompiler.function->arity++;
            functionCompiler.consume(TokenType::IDENTIFIER, "Expect parameter name.");
            Token paramName = functionCompiler.tokenVector.previous();
            functionCompiler.locals.push_back({paramName, functionCompiler.scopeDepth});
        } while (functionCompiler.tokenVector.check(TokenType::COMMA) && (functionCompiler.advance(), true));
    }
    functionCompiler.consume(TokenType::RIGHT_PAREN, "Expect ')' after params");
    functionCompiler.consume(TokenType::LEFT_BRACE, "Expect '{' after func sig.");

    functionCompiler.block();
    functionCompiler.emitReturn();

    uint8_t functionConstant = currentChunk()->addConstant(Value(functionCompiler.function));
    emitBytes((uint8_t)OpCode::OP_CLOSURE, functionConstant);

    for (const UpvalueInfo &upvalue : functionCompiler.upvalues)
    {
        emitByte(upvalue.isLocal ? 1 : 0);
        emitByte(upvalue.index);
    }
}

bool Compiler::compile()
{
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

int Compiler::addUpvalue(uint8_t index, bool isLocal)
{
    for (size_t i = 0; i < upvalues.size(); i++)
    {
        if (upvalues[i].index == index && upvalues[i].isLocal == isLocal)
        {
            return (int)i;
        }
    }

    upvalues.push_back({index, isLocal});
    function->upvalueCount++;
    return (int)upvalues.size() - 1;
}

int Compiler::resolveLocal(const Token &name)
{
    for (int i = locals.size() - 1; i >= 0; --i)
    {
        if (locals[i].name.lexeme == name.lexeme)
        {
            return i;
        }
    }

    return -1;
}
int Compiler::resolveUpvalue(const Token &name)
{
    std::cout << "resolveUpvalue(" << name.lexeme << "): enclosing=" << enclosing << std::endl;
    if (enclosing == nullptr)
    {
        std::cout << "  no enclosing, return -1\n";
        return -1;
    }

    int local = enclosing->resolveLocal(name);
    std::cout << "  enclosing->resolveLocal(" << name.lexeme << ") = " << local << std::endl;
    if (local != -1)
    {
        return addUpvalue((uint8_t)local, true);
    }
    return -1;
}

void Compiler::expression()
{
    parsePrecedence(Precedence::ASSIGNMENT);
}
void Compiler::_and(bool canAssign)
{
    int endJump = emitJump((uint8_t)OpCode::OP_JUMP_IF_FALSE);

    emitByte((uint8_t)OpCode::OP_POP);
    parsePrecedence(Precedence::AND);

    patchJump(endJump);
}
void Compiler::_or(bool canAssign)
{
    int elseJump = emitJump((uint8_t)OpCode::OP_JUMP_IF_FALSE);
    int endJump = emitJump((uint8_t)OpCode::OP_JUMP);

    patchJump(elseJump);
    emitByte((uint8_t)OpCode::OP_POP);

    parsePrecedence(Precedence::OR);
    patchJump(endJump);
}
void Compiler::call(bool canAssign)
{
    uint8_t argCount = argumentList();
    emitBytes((uint8_t)OpCode::OP_CALL, argCount);
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

    ObjString *obj = copyString(std::move(str));
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

    int slot = resolveLocal(identifier);

    if (canAssign && tokenVector.check(TokenType::EQUAL))
    {
        advance();
        expression();

        if (slot != -1)
            emitBytes((uint8_t)OpCode::OP_SET_LOCAL, (uint8_t)slot);
        else
        {
            int upvalueSlot = resolveUpvalue(identifier);
            if (upvalueSlot != -1)
            {
                emitBytes((uint8_t)OpCode::OP_SET_UPVALUE, (uint8_t)upvalueSlot);
            }
            else
            {
                ObjString *nameObj = copyString(identifier.lexeme);
                int nameConstant = currentChunk()->addConstant(Value(nameObj));
                emitBytes((uint8_t)OpCode::OP_SET_GLOBAL, (uint8_t)nameConstant);
            }
        }
        return;
    }

    if (slot != -1)
    {
        emitBytes((uint8_t)OpCode::OP_GET_LOCAL, (uint8_t)slot);
        return;
    }

    int upvalueSlot = resolveUpvalue(identifier);
    if (upvalueSlot != -1)
    {
        emitBytes((uint8_t)OpCode::OP_GET_UPVALUE, upvalueSlot);
        return;
    }

    ObjString *nameObj = copyString(identifier.lexeme);
    int nameConstant = currentChunk()->addConstant(Value(nameObj));
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
    else if (tokenVector.check(TokenType::FUNC))
    {
        advance();
        funcDeclaration();
    }
    else
    {
        statement();
    }
}

void Compiler::statement()
{
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
    else if (tokenVector.check(TokenType::FOR))
    {
        advance();
        forStatement();
    }
    else if (tokenVector.check(TokenType::RETURN))
    {
        advance();
        returnStatement();
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
    int loopStart = currentChunk()->code.size();
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
void Compiler::forStatement()
{
    beginScope();
    consume(TokenType::LEFT_PAREN, "Expect '(' after for");
    if (tokenVector.check(TokenType::SEMICOLON))
    {
    }
    else if (tokenVector.check(TokenType::VAR))
    {
        advance();
        varDeclaration();
    }
    else
    {
        expressionStatement();
    }

    int loopStart = currentChunk()->code.size();
    int exitJump = -1;
    if (!tokenVector.check(TokenType::SEMICOLON))
    {
        expression();
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition");

        exitJump = emitJump((uint8_t)OpCode::OP_JUMP_IF_FALSE);
        emitByte((uint8_t)OpCode::OP_POP);
    }

    if (!tokenVector.check(TokenType::RIGHT_PAREN))
    {
        int bodyJump = emitJump((uint8_t)OpCode::OP_JUMP);
        int incrementStart = currentChunk()->code.size();
        expression();
        emitByte((uint8_t)OpCode::OP_POP);
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

        emitLoop(loopStart);
        loopStart = incrementStart;
        patchJump(bodyJump);
    }

    statement();
    emitLoop(loopStart);

    if (exitJump != -1)
    {
        patchJump(exitJump);
        emitByte((uint8_t)OpCode::OP_POP);
    }
    endScope();
}
void Compiler::returnStatement()
{
    if (functionType == FunctionType::TYPE_SCRIPT)
    {
        errorAt(tokenVector.previous(), "Can't return from script");
    }

    if (tokenVector.check(TokenType::SEMICOLON))
    {
        advance();
        emitReturn();
    }
    else
    {
        expression();
        consume(TokenType::SEMICOLON, "Expect ';' after return value");
        emitByte((uint8_t)OpCode::OP_RETURN);
    }
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
    ObjString *nameObj = copyString(token.lexeme);
    int nameConstant = currentChunk()->addConstant(Value(nameObj));

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
void Compiler::funcDeclaration()
{

    consume(TokenType::IDENTIFIER, "Expect identifier after 'func'.");
    Token token = tokenVector.previous();
    ObjString *nameObj = copyString(token.lexeme);
    int nameConstant = currentChunk()->addConstant(Value(nameObj));

    compileFunction(FunctionType::TYPE_FUNCTION, nameObj);
    std::cout << "funcDeclaration: name=" << token.lexeme << " scopeDepth=" << scopeDepth << std::endl;

    if (scopeDepth == 0)
    {
        emitBytes((uint8_t)OpCode::OP_DEFINE_GLOBAL, (uint8_t)nameConstant);
    }
    else
    {
        locals.push_back({token, scopeDepth});
    }
}
void Compiler::emitByte(uint8_t byte)
{
    currentChunk()->write(byte, tokenVector.previous().line);
}
void Compiler::emitBytes(uint8_t a, uint8_t b)
{
    emitByte(a);
    emitByte(b);
}
void Compiler::emitConstant(Value value)
{
    int index = currentChunk()->addConstant(value);
    emitBytes((uint8_t)OpCode::OP_CONSTANT, (uint8_t)index);
}
void Compiler::emitReturn()
{
    emitByte((uint8_t)OpCode::OP_NIL);
    emitByte((uint8_t)OpCode::OP_RETURN);
}

uint8_t Compiler::emitJump(uint8_t opcode)
{
    emitByte((uint8_t)opcode);
    emitBytes(0xFF, 0xFF);
    return currentChunk()->code.size() - 2;
}
void Compiler::patchJump(int offset)
{
    int jump = currentChunk()->code.size() - offset - 2;
    if (jump > 0xFFFF)
    {
        errorAt(tokenVector.previous(), "Too much to jump over.");
    }

    currentChunk()->code[offset] = (jump >> 8) & 0xFF;
    currentChunk()->code[offset + 1] = jump & 0xFF;
}
void Compiler::emitLoop(int loopStart)
{
    emitByte((uint8_t)OpCode::OP_LOOP);

    int offset = currentChunk()->code.size() - loopStart + 2;

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
            {TokenType::LEFT_PAREN, {&Compiler::grouping, &Compiler::call, Precedence::CALL}},
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

            {TokenType::AND, {nullptr, &Compiler::_and, Precedence::AND}},
            {TokenType::OR, {nullptr, &Compiler::_or, Precedence::OR}},
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