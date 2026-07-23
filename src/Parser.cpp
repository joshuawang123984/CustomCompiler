#include <iostream>
#include "../include/Parser.hpp"

Parser::Parser(TokenVector &tokens) : tokenVector(TokenVector(tokens, tokens.getSource(), &current, nullptr)) {}
const TokenVector &Parser::getTokens() const { return tokenVector; }

std::vector<std::unique_ptr<Statement>> Parser::stmt_parse()
{
    std::vector<std::unique_ptr<Statement>> statements;

    while (!tokenVector.check(TokenType::EOF_TOKEN))
    {
        statements.push_back(statement());
    }

    return statements;
}
std::unique_ptr<Statement> Parser::statement()
{
    std::cout << "Parsing statement at token type: " << static_cast<int>(tokenVector.token_peek().type) << std::endl;
    if (tokenVector.token_match(TokenType::PRINT))
        return printStatement();

    if (tokenVector.token_match(TokenType::VAR))
        return varDeclaration();

    if (tokenVector.token_match(TokenType::IF))
        return ifStatement();

    if (tokenVector.token_match(TokenType::WHILE))
        return whileStatement();

    if (tokenVector.token_match(TokenType::FOR))
        return forStatement();

    if (tokenVector.token_match(TokenType::LEFT_BRACE))
        return blockStatement();

    auto expr = expression();
    tokenVector.consume(TokenType::SEMICOLON, "(statement) expect ';' after declaration");
    return std::make_unique<ExpressionStatement>(std::move(expr));
}
std::unique_ptr<Statement> Parser::printStatement()
{
    auto value = expression();
    tokenVector.consume(TokenType::SEMICOLON, "(printStatement) expect ';' after declaration");
    return std::make_unique<PrintStatement>(std::move(value));
}
std::unique_ptr<Statement> Parser::varDeclaration()
{
    Token nameToken = tokenVector.consume(TokenType::IDENTIFIER, "variable name");
    std::unique_ptr<Expr> initializer = nullptr;
    if (tokenVector.token_match(TokenType::EQUAL))
        initializer = expression();

    tokenVector.consume(TokenType::SEMICOLON, "(varDeclaration) expect ';' after declaration");
    return std::make_unique<VarStatement>(nameToken.lexeme, std::move(initializer));
}
std::unique_ptr<Statement> Parser::ifStatement()
{
    tokenVector.consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    tokenVector.consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    auto thenBranch = statement();
    std::unique_ptr<Statement> elseBranch = nullptr;

    if (tokenVector.token_match(TokenType::ELSE))
    {
        elseBranch = statement();
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}
std::unique_ptr<Statement> Parser::whileStatement()
{
    tokenVector.consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    tokenVector.consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");

    auto body = statement();

    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}
std::unique_ptr<Statement> Parser::forStatement()
{
    tokenVector.consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    std::unique_ptr<Statement> initializer = nullptr;
    if (tokenVector.token_match(TokenType::SEMICOLON))
    {
        initializer = nullptr;
    }
    else if (tokenVector.token_match(TokenType::VAR))
    {
        // Variable declaration initializer: for (var i = 0; ...)
        // varDeclaration handles semicolon token
        initializer = varDeclaration();
    }
    else
    {
        // Expression statement initializer: for (i = 0; ...)
        auto expr = expression();
        tokenVector.consume(TokenType::SEMICOLON, "Expect ';' after expression initializer.");
        initializer = std::make_unique<ExpressionStatement>(std::move(expr));
    }
    std::unique_ptr<Expr> condition = expression();

    tokenVector.consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::unique_ptr<Expr> increment = expression();

    tokenVector.consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = statement();

    return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}
std::unique_ptr<Statement> Parser::blockStatement()
{
    std::vector<std::unique_ptr<Statement>> statements;

    while (!tokenVector.check(TokenType::RIGHT_BRACE) && !tokenVector.isAtEnd() && !tokenVector.token_match(TokenType::EOF_TOKEN))
    {
        statements.push_back(statement());
    }
    tokenVector.consume(TokenType::RIGHT_BRACE, "expect '}' after block");
    return std::make_unique<BlockStatement>(std::move(statements));
}
std::unique_ptr<Expr> Parser::expr_parse()
{
    try
    {
        return expression();
    }
    catch (const std::runtime_error &error)
    {
        std::cerr << "[Parser Error]: " << error.what() << std::endl;
        return nullptr;
    }
}
std::unique_ptr<Expr> Parser::expression()
{
    return equality();
}
std::unique_ptr<Expr> Parser::equality()
{
    auto expr = comparison();

    while (tokenVector.token_match(TokenType::EQUAL_EQUAL) || tokenVector.token_match(TokenType::NOT_EQUAL))
    {
        Token oper = tokenVector.previous();
        auto right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (tokenVector.token_match(TokenType::GREATER) || tokenVector.token_match(TokenType::GREATER_EQUAL) || tokenVector.token_match(TokenType::LESS) || tokenVector.token_match(TokenType::LESS_EQUAL))
    {
        Token oper = tokenVector.previous();
        auto right = term();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::term()
{
    auto expr = factor();

    while (tokenVector.token_match(TokenType::PLUS) || tokenVector.token_match(TokenType::MINUS))
    {
        Token oper = tokenVector.previous();
        auto right = factor();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::factor()
{
    auto expr = unary();

    while (tokenVector.token_match(TokenType::STAR) || tokenVector.token_match(TokenType::SLASH))
    {
        Token oper = tokenVector.previous();
        auto right = unary();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::unary()
{

    while (tokenVector.token_match(TokenType::NOT) || tokenVector.token_match(TokenType::MINUS))
    {
        Token oper = tokenVector.previous();
        auto right = primary();
        return std::make_unique<Unary>(oper, std::move(right));
    }

    return primary();
}
std::unique_ptr<Expr> Parser::primary()
{
    if (tokenVector.token_match(TokenType::FALSE))
        return std::make_unique<Literal>(false);
    if (tokenVector.token_match(TokenType::TRUE))
        return std::make_unique<Literal>(true);
    if (tokenVector.token_match(TokenType::NIL))
        return std::make_unique<Literal>(nullptr);

    if (tokenVector.token_match(TokenType::NUMBER) || tokenVector.token_match(TokenType::STRING))
        return std::make_unique<Literal>(tokenVector.previous().literal);

    if (tokenVector.token_match(TokenType::IDENTIFIER))
    {
        return std::make_unique<Variable>(tokenVector.previous().lexeme);
    }

    if (tokenVector.token_match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();
        tokenVector.consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw std::runtime_error("Expect expression. Failed Token: " + tokenVector.token_peek().lexeme);
}