#include "../include/Lexer.hpp"
#include "../include/Token.hpp"
#include "../include/ErrorReporter.hpp"

Lexer::Lexer(const std::string &source) : source(source), start(0), current(0), line(1) {}

bool Lexer::isAtEnd() const
{
    return current >= source.length();
}

bool Lexer::match(char expected)
{
    if (isAtEnd())
        return false;
    if (source[current] != expected)
        return false;

    current++;
    return true;
}

char Lexer::peek()
{
    if (isAtEnd())
        return '\0';
    return source[current];
}

char Lexer::peekNext()
{
    if (current + 1 >= source.length())
        return '\0';
    return source[current + 1];
}

void Lexer::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
            line++;
        advance();
    }

    if (isAtEnd())
    {
        ErrorReporter::error(line, "Unterminated string.");
        return;
    }

    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

bool Lexer::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

void Lexer::number()
{
    while (isDigit(peek()))
        advance();

    if (peek() == '.' && isDigit(peekNext()))
    {
        advance();
        while (isDigit(peek()))
            advance();
    }

    std::string text = source.substr(start, current - start);
    addToken(TokenType::NUMBER, std::stod(text));
}

char Lexer::advance()
{
    return source[current++];
}
bool Lexer::isAlpha(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}
bool Lexer::isAlphaNumeric(char c)
{
    return isAlpha(c) || isDigit(c);
}
void Lexer::identifier()
{
    while (isAlphaNumeric(peek()))
        advance();

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    TokenType type = (it == keywords.end()) ? TokenType::IDENTIFIER : it->second;

    addToken(type);
}

void Lexer::addToken(TokenType type)
{
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, line);
}
void Lexer::addToken(TokenType type, double literal)
{
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}
void Lexer::addToken(TokenType type, std::string literal)
{
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}
void Lexer::scanToken()
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
            ErrorReporter::error(line, "Unexpected character.");
        }
        break;
    }
}

std::vector<Token> Lexer::scanTokens()
{
    while (!isAtEnd())
    {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::EOF_TOKEN, "", line);
    return tokens;
}