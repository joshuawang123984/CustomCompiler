#include <iostream>
#include "../include/Scanner.hpp"

bool Scanner::isAtEnd() const
{
    return current >= source.length();
}
bool Scanner::match(char expected)
{
    if (isAtEnd())
        return false;

    return source[current++] == expected;
}
char Scanner::peek()
{
    if (isAtEnd())
        return '\0';

    return source[current];
}
char Scanner::peekNext()
{
    if (current + 1 > source.length())
        return '\0';
    return source[current + 1];
}
char Scanner::advance()
{
    return source[current++];
}
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
void Scanner::addToken(TokenType type)
{
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, line);
}
void Scanner::addToken(TokenType type, double literal)
{
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}
void Scanner::addToken(TokenType type, std::string literal)
{
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}
void Scanner::number()
{
    while (isDigit(peek()))
        advance();

    if (peek() == '.' && isDigit(peekNext()))
    {
        advance();
        while (isDigit(peek()))
            advance();
    }

    std::string num = source.substr(start, current - start);
    addToken(TokenType::NUMBER, std::stod(num));
}

void Scanner::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() != '\n')
            line++;
    }

    if (isAtEnd())
    {
        // implement an error class later rathern than print
        std::cerr << "Unterminated String." << std::endl;
        return;
    }

    advance();

    std::string text = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, text);
}
void Scanner::identifier()
{
    while (isAlphaNumeric(peek()))
        advance();

    std::string identifier = source.substr(start, current - start);
    auto it = keywords.find(identifier);

    // checks if token was found. if not, its an identifier. if is, extract value of key from keywords map
    TokenType type = (it == keywords.end()) ? TokenType::IDENTIFIER : it->second;
    addToken(type);
}
void Scanner::scanToken()
{
    char c = advance();

    switch (c)
    {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case '.':
        addToken(TokenType::DOT);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;

    case '!':
        addToken(match('=') ? TokenType::NOT_EQUAL : TokenType::NOT);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;

    case '/':
        // if comment, skip
        if (match('/'))
        {
            while (peek() != '\n' && !isAtEnd())
                advance();
        }
        else
        {
            addToken(TokenType::SLASH);
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

Scanner::Scanner(const std::string &source) : source(source) {}
std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd())
    {
        start = current;
        scanToken();
    }
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line);
    return tokens;
}

const std::vector<Token> &Scanner::getTokens() const { return tokens; }
const std::string &Scanner::getSource() const { return source; }