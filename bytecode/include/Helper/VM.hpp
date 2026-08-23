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

    Table globals;
    Table strings;

    ObjString *copyString(const std::string &text);
    InterpretResult run();
    void runtimeError(const std::string &message);

public:
    InterpretResult interpret(Chunk &chunkArg);
    Table &getStrings();

    VM()
    {
        initTable(&globals);
        initTable(&strings);
    }

    ~VM()
    {
        freeTable(&globals);
        freeTable(&strings);
    }
};