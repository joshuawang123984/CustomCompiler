#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../include/Token.hpp"
#include "../include/Scanner.hpp"

void printTokens(const std::vector<Token> &tokens);
void runFile(const std::string &path);
void runPrompt();
void run(const std::string &source);
void testScanner();

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
        testScanner();
    }

    return 0;
}

void printTokens(const std::vector<Token> &tokens)
{
    for (const auto &token : tokens)
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

void testScanner()
{
    std::string sourceCode = "var x = 10;";
    Scanner scanner(sourceCode);

    scanner.scanTokens();

    const auto &tokens = scanner.getTokens();
    printTokens(tokens);
}
