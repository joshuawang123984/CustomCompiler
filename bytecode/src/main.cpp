#include "../include/Helper/functions.hpp"
#include "../include/Helper/chunk.hpp"
#include "../include/Scanner.hpp"
#include "../include/Compiler.hpp"

void test(const std::string &source);

int main(int argc, char *argv[])
{

    if (argc > 2)
    {
        std::cout << "Usage: clox [script]" << std::endl;
        std::exit(64);
    }

    else if (argc == 2)
    {
        std::string source = runFile(argv[1]);
        test(source);
    }

    else
    {
        std::cout << "Usage: clox [script]" << std::endl;
    }

    return 0;
}

void test(const std::string &source)
{
    Scanner scanner(source);
    TokenVector tokens = scanner.scanTokens();

    for (const Token &token : tokens.getTokens())
    {
        std::cout << "[line " << token.line << "] "
                  << static_cast<int>(token.type)
                  << " '" << token.lexeme << "'"
                  << std::endl;
    }

    Chunk chunk;
    Compiler compiler(source, chunk);
    bool ok = compiler.compile();
    if (ok)
    {
        disassembleChunk(chunk, "test");
    }
}