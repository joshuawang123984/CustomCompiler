#include "../include/TokenVector.hpp"

TokenVector::TokenVector(const std::string &source, int *current, int *start) : source(source), current(current), start(start) {}
const std::vector<Token> &TokenVector::getTokens() const { return tokens; }
const std::string &TokenVector::getSource() const { return source; }

bool TokenVector::isAtEnd() const
{
    return *current >= source.length();
}
bool TokenVector::check(TokenType type)
{
    if (isAtEnd())
        return false;

    return token_peek().type == type;
}
bool TokenVector::char_match(char expected)
{
    if (isAtEnd())
        return false;

    return source[*current] == expected;
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
    if (isAtEnd())
        return Token(TokenType::NIL, "null", -1);

    return tokens.at(*current);
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
    if (isAtEnd())
        return Token(TokenType::NIL, "null", -1);
    return tokens.at((*current)++);
}
Token TokenVector::previous()
{
    if (isAtEnd() || *current <= 0)
        return Token(TokenType::NIL, "null", -1);
    return tokens.at((*current) - 1);
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
void TokenVector::addToken(TokenType type, double literal, int line)
{
    std::string text = source.substr(*start, *current - *start);
    tokens.emplace_back(type, text, literal, line);
}
void TokenVector::addToken(TokenType type, std::string literal, int line)
{
    std::string text = source.substr(*start, *current - *start);
    tokens.emplace_back(type, text, literal, line);
}
void TokenVector::addEOF(int line)
{
    tokens.emplace_back(TokenType::EOF_TOKEN, "-_-", line);
}
