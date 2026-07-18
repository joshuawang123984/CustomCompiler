#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../include/Token.hpp"
#include "../include/Scanner.hpp"
#include "../include/Parser.hpp"

void printTokens(const TokenVector &tokens);
void runFile(const std::string &path);
void runPrompt();
void run(const std::string &source);
TokenVector testScanner(const std::string &sourceCode);
std::unique_ptr<Expr> testParse(TokenVector tokens);

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
        // scanner can do expressions like var x = 10 but parser cant yet
        std::string sourceCode = "30 + 10 * 5 * 9 + 301 - 31983 / 3 * (-1 + 4 / 2)";
        TokenVector tokens = testScanner(sourceCode);
        std::unique_ptr<Expr> ast = testParse(tokens);
    }

    return 0;
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
    return;
}

TokenVector testScanner(const std::string &sourceCode)
{
    Scanner scanner(sourceCode);

    scanner.scanTokens();

    const auto &tokens = scanner.getTokens();
    printTokens(tokens);

    return tokens;
}

std::unique_ptr<Expr> testParse(TokenVector tokens)
{
    Parser parser(tokens);
    std::unique_ptr<Expr> ast = parser.parse();

    return ast;
}
