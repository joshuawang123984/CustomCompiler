#include "types.hpp"
#include "../../hashmap/include/Table.hpp"
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>

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
    std::unordered_map<std::string, Value> globals;

    // Table globals;

    InterpretResult run();
    void runtimeError(const std::string &message);

public:
    InterpretResult interpret(Chunk &chunkArg);
};