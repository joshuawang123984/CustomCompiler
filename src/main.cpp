#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../include/Token.hpp"
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/ErrorReporter.hpp"

void runFile(const std::string &path);
void runPrompt();
void run(const std::string &source);

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        std::cout << "Usage: lox [script]" << std::endl;
        std::exit(64);
    }

    else if (argc == 2)
    {
        runFile(argv[1]);
    }

    else
    {
        runPrompt();
    }

    return 0;
}

void runFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << path << std::endl;
        std::exit(74);
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    run(source);
}

void runPrompt()
{
    std::string line;
    for (;;)
    {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            break;
        run(line);
    }
}

void run(const std::string &source)
{
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    for (const auto &token : tokens)
    {
        std::cout << "[" << static_cast<int>(token.type) << "] '"
                  << token.lexeme << "'";

        std::visit([](auto &&val)
                   {
        using T = std::decay_t<decltype(val)>;
        if constexpr (!std::is_same_v<T, std::monostate>)
            std::cout << " (" << val << ")"; }, token.literal);

        std::cout << " line " << token.line_number << "\n";
    }

    // Parser parser(tokens);
    // Expr *expression = parser.parse();

    if (ErrorReporter::hadError)
        return;
}