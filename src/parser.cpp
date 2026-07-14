#include <iostream>
#include "../include/Parser.hpp"
#include "../include/Token.hpp"

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

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

Token Parser::previous()
{
    return tokens.at(current - 1);
}

Token Parser::peek()
{
    return tokens.at(current);
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::EOF_TOKEN;
}
Token Parser::advance()
{
    if (!isAtEnd())
        current++;
    return tokens.at(current - 1);
}
bool Parser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}
bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;
    return peek().type == type;
}
Token Parser::consume(TokenType type, const std::string &message)
{
    if (check(type))
        return advance();

    throw std::runtime_error(message);
}

std::unique_ptr<Expr> Parser::primary()
{
    if (match(TokenType::FALSE))
        return std::make_unique<Literal>(false);
    if (match(TokenType::TRUE))
        return std::make_unique<Literal>(true);
    if (match(TokenType::NIL))
        return std::make_unique<Literal>(nullptr);

    if (match(TokenType::NUMBER) || match(TokenType::STRING))
    {
        return std::make_unique<Literal>(previous().literal);
    }

    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }
    throw std::runtime_error("Expect expression.");
}
std::unique_ptr<Expr> Parser::unary()
{
    if (match(TokenType::NOT) || match(TokenType::MINUS))
    {
        Token oper = previous();
        auto right = unary();
        return std::make_unique<Unary>(oper, std::move(right));
    }

    return primary();
}
std::unique_ptr<Expr> Parser::factor()
{
    auto expr = unary();

    while (match(TokenType::SLASH) || match(TokenType::STAR))
    {
        Token oper = previous();
        auto right = unary();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::term()
{
    auto expr = factor();

    while (match(TokenType::MINUS) || match(TokenType::PLUS))
    {
        Token oper = previous();
        auto right = factor();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL))
    {
        Token oper = previous();
        auto right = term();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::equality()
{
    auto expr = comparison();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL))
    {
        Token oper = previous();
        auto right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), oper, std::move(right));
    }

    return expr;
}
std::unique_ptr<Expr> Parser::expression()
{
    return equality();
}
