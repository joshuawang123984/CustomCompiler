#include "Types.hpp"
#include <iostream>

class Chunk;

enum class InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

class VM
{
private:
    Chunk *chunk;
    uint8_t *ip;
    std::vector<Value> stack;
    InterpretResult run();

public:
    InterpretResult interpret(Chunk &chunkArg);
};