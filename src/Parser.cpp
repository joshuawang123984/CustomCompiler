#include <iostream>
#include "../include/Parser.hpp"
#include "../include/Token.hpp"

Parser::Parser(TokenVector &tokens) : tokenVector(TokenVector(tokens, tokens.getSource(), &current, nullptr)) {}
const TokenVector &Parser::getTokens() const { return tokenVector; }

std::unique_ptr<Expr> Parser::parse()
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

    if (tokenVector.token_match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();
        tokenVector.consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw std::runtime_error("Expect expression.");
}