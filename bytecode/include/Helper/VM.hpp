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

struct CallFrame
{
    LoxFunction *function;
    uint8_t *ip;
    size_t slotStart;
};

class VM
{
private:
    std::vector<Value> stack;

    Table globals;
    Table strings;

    std::vector<CallFrame> frames;

    ObjString *copyString(const std::string &text);
    InterpretResult run();
    void runtimeError(const std::string &message);

public:
    InterpretResult interpret(LoxFunction *script);
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