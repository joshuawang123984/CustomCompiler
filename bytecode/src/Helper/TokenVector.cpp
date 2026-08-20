#include <stdexcept>
#include "../../include/Helper/TokenVector.hpp"

TokenVector::TokenVector(const std::string &source, int *current, int *start) : source(source), current(current), start(start) {}
const std::vector<Token> &TokenVector::getTokens() const { return tokens; }
const std::string &TokenVector::getSource() const { return source; }

bool TokenVector::isAtEnd() const
{
    return *current >= static_cast<int>(source.length());
}
bool TokenVector::isAtTokenEnd() const
{
    return tokenIndex >= tokens.size();
}
bool TokenVector::check(TokenType type)
{
    if (isAtTokenEnd())
        return false;

    return token_peek().type == type;
}
bool TokenVector::char_match(char expected)
{
    if (isAtEnd())
        return false;
    if (source[*current] != expected)
        return false;

    (*current)++;
    return true;
}
bool TokenVector::token_match(TokenType type)
{
    if (check(type))
    {
        token_advance();
        return true;
    }

    return false;
}
char TokenVector::char_peek()
{
    if (isAtEnd())
        return '\0';

    return source[*current];
}
Token TokenVector::token_peek()
{
    if (isAtTokenEnd())
        return Token(TokenType::NIL, "null", -1);

    return tokens.at(tokenIndex);
}
char TokenVector::peekNext()
{
    if ((*current) + 1 >= source.length())
        return '\0';
    return source[(*current) + 1];
}
char TokenVector::char_advance()
{
    if (isAtEnd())
        return '\0';
    return source[(*current)++];
}
Token TokenVector::token_advance()
{
    if (isAtTokenEnd())
        return Token(TokenType::NIL, "null", -1);
    return tokens.at(tokenIndex++);
}
Token TokenVector::previous()
{
    if (isAtTokenEnd() || tokenIndex <= 0)
        return Token(TokenType::NIL, "null", -1);
    return tokens.at(tokenIndex - 1);
}
Token TokenVector::consume(TokenType type, const std::string &message)
{
    if (check(type))
        return token_advance();

    throw std::runtime_error(message);
}
void TokenVector::addToken(TokenType type, int line)
{
    std::string text = source.substr(*start, *current - *start);
    tokens.emplace_back(type, text, line);
}

void TokenVector::addEOF(int line)
{
    tokens.emplace_back(TokenType::EOF_TOKEN, "<EOF>", line);
}
