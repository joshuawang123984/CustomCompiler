#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../include/Scanner.hpp"
#include "../include/Parser.hpp"
#include "../include/Expressions/Printing/AstPrinter.hpp"
#include "../include/Evaluator.hpp"

void testExpr();
void testStatement();
void testControlFlow();
void printTokens(const TokenVector &tokens);
void printValue(Value val);
void runFile(const std::string &path);
void runPrompt();
void run(const std::string &source);
TokenVector testScanner(const std::string &sourceCode);
std::unique_ptr<Expr> testParse(TokenVector tokens);
Value testEvaluator(std::unique_ptr<Expr> ast);

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
        // testExpr();
        // testStatement();
        testControlFlow();
    }

    return 0;
}

void testExpr()
{
    // scanner can do expressions like var x = 10 but parser cant yet
    std::string sourceCode = "30 + 10 * 5 * 9 + 301 - 31983 / 3 * (-1 + 4 / 2)";
    TokenVector tokens = testScanner(sourceCode);
    std::unique_ptr<Expr> ast = testParse(tokens);
    Value result = testEvaluator(std::move(ast));
}
void testStatement()
{
    std::string source =
        "var x = 10 + 5;\n"
        "var y = x * 2;\n"
        "print y;\n"
        "x + 5;\n";

    TokenVector tokens = testScanner(source);

    Parser parser(tokens);
    std::vector<std::unique_ptr<Statement>> statements = parser.stmt_parse();

    Evaluator evaluator;
    for (const auto &stmt : statements)
    {
        stmt->accept(evaluator);
    }
}

void testControlFlow()
{
    std::string source =
        "var x = 10;\n"
        "var y = 0;\n"
        "\n"
        "// Test if/else\n"
        "if (x > 5) {\n"
        "    y = 100;\n"
        "} else {\n"
        "    y = 42;\n"
        "}\n"
        "\n"
        "// Test while loop\n"
        "var count = 0;\n"
        "while (count < 3) {\n"
        "    count = count + 1;\n"
        "}\n"
        "\n"
        "// Test for loop\n"
        "for (var i = 0; i < 3; i = i + 1) {\n"
        "    y = y + i;\n"
        "}\n"
        "\n"
        "print x;\n"
        "print y;\n"
        "print count;\n";

    TokenVector tokens = testScanner(source);

    Parser parser(tokens);
    std::vector<std::unique_ptr<Statement>> statements = parser.stmt_parse();

    Evaluator evaluator;
    for (const auto &stmt : statements)
    {
        stmt->accept(evaluator);
    }
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
    std::unique_ptr<Expr> ast = parser.expr_parse();

    AstPrinter printer;
    std::string ast_result = printer.print(*ast);
    std::cout << "| AstTree Result: " << ast_result << "|" << std::endl;

    return ast;
}

Value testEvaluator(std::unique_ptr<Expr> ast)
{
    Evaluator evaluator;
    Value result = evaluator.evaluate(*ast);

    printValue(result);
    return result;
}
