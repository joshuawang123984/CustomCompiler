#pragma once
#include <vector>
#include "Statement.hpp"
#include "Helper/Token.hpp"
#include "Helper/TokenVector.hpp"
#include "Expr.hpp"
#include "../include/Statements/Statements.hpp"

class Parser
{
public:
    explicit Parser(TokenVector &tokens);
    const TokenVector &getTokens() const;

    std::vector<std::unique_ptr<Statement>> stmt_parse();
    std::unique_ptr<Statement> statement();

    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<Statement> varDeclaration();
    std::unique_ptr<Expr> expr_parse();

private:
    TokenVector tokenVector;
    int current = 0;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
};