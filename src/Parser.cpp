#include "../include/Parser.hpp"

Parser::Parser(TokenVector &tokens) : tokenVector(tokens) {}
const TokenVector &Parser::getTokens() const { return tokenVector; }

std::unique_ptr<Expr> Parser::parse()
{
}