#include "../include/Helper/Functions.hpp"

std::string runFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << path << std::endl;
        std::exit(74);
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return source;
}

std::string stringify(Value value)
{
    if (std::holds_alternative<double>(value))
        return std::to_string(std::get<double>(value));

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value) ? "true" : "false";

    if (std::holds_alternative<std::string>(value))
        return std::get<std::string>(value);

    if (std::holds_alternative<std::nullptr_t>(value))
        return "nil";

    return "unknown";
}

bool isTruthy(Value value)
{
    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value);

    if (std::holds_alternative<std::nullptr_t>(value))
        return false;

    return true;
}

void printTokens(const TokenVector &tokens)
{

    for (const auto &token : tokens.getTokens())
    {
        std::cout
            << " | Lexeme: " << token.lexeme
            << " | Line: " << token.line << std::endl;
    }
}

void printValue(Value val)
{
    std::visit([](auto &&arg)
               { std::cout << arg << std::endl; }, val);
}