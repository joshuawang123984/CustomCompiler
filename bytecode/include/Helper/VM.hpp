#include "types.hpp"
#include "../../hashmap/include/Table.hpp"
#include "../Memory.hpp"
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>

#define STACK_MAX 1024

class Chunk;

enum class InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

struct CallFrame
{
    ObjClosure *closure;
    uint8_t *ip;
    size_t slotStart;
};

Value clockNative(int argCount, Value *args);

class VM
{
private:
    std::vector<Value> stack;

    Table globals;
    Table strings;

    std::vector<CallFrame> frames;

    ObjUpvalue *openUpvalues = nullptr;

    GarbageCollector gc;

    ObjString *copyString(const std::string &text);
    InterpretResult run();
    void runtimeError(const std::string &message);
    void defineNative(const std::string &name, NativeFn function);
    ObjUpvalue *captureUpvalue(Value *localSlot);
    void closeUpvalues(Value *last);

    void markRoots();
    void tableRemoveWhite(Table *strings);

public:
    InterpretResult interpret(LoxFunction *script);
    Table &getStrings();
    GarbageCollector &getGC();

    VM()
    {
        initTable(&globals);
        initTable(&strings);
        stack.reserve(STACK_MAX);
        gc.setMarkRootsCallback([this]
                                { markRoots(); });
        gc.setTableRemoveWhite([this]
                               { tableRemoveWhite(&strings); });

        defineNative("clock", clockNative);
    }

    ~VM()
    {
        freeTable(&globals);
        freeTable(&strings);
        gc.freeObjects();
    }
};