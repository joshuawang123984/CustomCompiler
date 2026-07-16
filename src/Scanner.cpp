#include <iostream>
#include "../include/Scanner.hpp"

Scanner::Scanner(const std::string &source) : source(source), tokenVector(source, &current, &start) {}
const TokenVector &Scanner::getTokens() const { return tokenVector; }
const std::string &Scanner::getSource() const { return source; }

bool Scanner::isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

bool Scanner::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}
bool Scanner::isAlphaNumeric(char c)
{
    return isAlpha(c) || isDigit(c);
}

void Scanner::number()
{
    while (isDigit(tokenVector.char_peek()))
        tokenVector.char_advance();

    if (tokenVector.char_peek() == '.' && isDigit(tokenVector.peekNext()))
    {
        tokenVector.char_advance();
        while (isDigit(tokenVector.char_peek()))
            tokenVector.char_advance();
    }

    std::string num = source.substr(start, current - start);
    tokenVector.addToken(TokenType::NUMBER, std::stod(num), line);
}

void Scanner::string()
{
    while (tokenVector.char_peek() != '"' && !tokenVector.isAtEnd())
    {
        if (tokenVector.char_peek() != '\n')
            line++;
    }

    if (tokenVector.isAtEnd())
    {
        // implement an error class later rathern than print
        std::cerr << "Unterminated String." << std::endl;
        return;
    }

    tokenVector.char_advance();

    std::string text = source.substr(start + 1, current - start - 2);
    tokenVector.addToken(TokenType::STRING, text, line);
}
void Scanner::identifier()
{
    while (isAlphaNumeric(tokenVector.char_peek()))
        tokenVector.char_advance();

    std::string identifier = source.substr(start, current - start);
    auto it = keywords.find(identifier);

    // checks if token was found. if not, its an identifier. if is, extract value of key from keywords map
    TokenType type = (it == keywords.end()) ? TokenType::IDENTIFIER : it->second;
    tokenVector.addToken(type, line);
}
void Scanner::scanToken()
{
    char c = tokenVector.char_advance();

    switch (c)
    {
    case '(':
        tokenVector.addToken(TokenType::LEFT_PAREN, line);
        break;
    case ')':
        tokenVector.addToken(TokenType::RIGHT_PAREN, line);
        break;
    case '{':
        tokenVector.addToken(TokenType::LEFT_BRACE, line);
        break;
    case '}':
        tokenVector.addToken(TokenType::RIGHT_BRACE, line);
        break;
    case ',':
        tokenVector.addToken(TokenType::COMMA, line);
        break;
    case '.':
        tokenVector.addToken(TokenType::DOT, line);
        break;
    case '-':
        tokenVector.addToken(TokenType::MINUS, line);
        break;
    case '+':
        tokenVector.addToken(TokenType::PLUS, line);
        break;
    case ';':
        tokenVector.addToken(TokenType::SEMICOLON, line);
        break;
    case '*':
        tokenVector.addToken(TokenType::STAR, line);
        break;

    case '!':
        tokenVector.addToken(tokenVector.char_match('=') ? TokenType::NOT_EQUAL : TokenType::NOT, line);
        break;
    case '=':
        tokenVector.addToken(tokenVector.char_match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL, line);
        break;
    case '<':
        tokenVector.addToken(tokenVector.char_match('=') ? TokenType::LESS_EQUAL : TokenType::LESS, line);
        break;
    case '>':
        tokenVector.addToken(tokenVector.char_match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER, line);
        break;

    case '/':
        // if comment, skip
        if (tokenVector.char_match('/'))
        {
            while (tokenVector.char_peek() != '\n' && !tokenVector.isAtEnd())
                tokenVector.char_advance();
        }
        else
        {
            tokenVector.addToken(TokenType::SLASH, line);
        }
        break;

    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        line++;
        break;

    case '"':
        string();
        break;

    default:
        if (isDigit(c))
        {
            number();
        }
        else if (isAlpha(c))
        {
            identifier();
        }

        else
        {
            std::cerr << "Unexpected character." << std::endl;
        }
        break;
    }
}

TokenVector Scanner::scanTokens()
{
    while (!tokenVector.isAtEnd())
    {
        start = current;
        scanToken();
    }
    tokenVector.addEOF(line);
    return tokenVector;
}