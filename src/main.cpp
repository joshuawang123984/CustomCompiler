#include <iostream>
#include <string>
#include <vector>
#include "../include/Scanner.hpp"
#include "../include/Parser.hpp"
#include "../include/Expressions/Printing/AstPrinter.hpp"
#include "../include/Evaluator.hpp"
#include "../include/Resolver.hpp"

void testExpr();
void testStatement();
void testComplete(std::string source);
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
        std::string source = runFile(argv[1]);
        testComplete(source);
    }

    else
    {
        std::cout << "Usage: lox [script]" << std::endl;
    }

    return 0;
}

void testExpr()
{
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

void testComplete(std::string source)
{
    try
    {
        Scanner scanner(source);
        TokenVector tokens = scanner.scanTokens();
        printTokens(tokens);

        Parser parser(tokens);
        std::vector<std::unique_ptr<Statement>> statements = parser.stmt_parse();

        Evaluator evaluator;
        Resolver resolver(evaluator);
        resolver.resolve(statements);

        for (const auto &stmt : statements)
        {
            stmt->accept(evaluator);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "RUNTIME/PARSER ERROR: " << e.what() << std::endl;
    }
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
