#include "../../include/Helper/VM.hpp"
#include "../../include/Helper/chunk.hpp"
#include "../../include/Helper/functions.hpp"
#include "../../include/Helper/Obj.hpp"

#include <ctime>

#ifndef USE_COMPUTED_GOTO
#if defined(__GNUC__) || defined(__clang__)
#define USE_COMPUTED_GOTO 1
#else
#define USE_COMPUTED_GOTO 0
#endif
#endif
#define READ_BYTE() (*frame->ip++)

static ObjString *TOMBSTONE = reinterpret_cast<ObjString *>(0x1);

static uint32_t hashString(const std::string &str)
{
    uint32_t hash = 2166136261u;
    for (char c : str)
    {
        hash ^= (uint8_t)c;
        hash *= 16777619;
    }
    return hash;
}

static ObjString *tableFindString(Table *table, const std::string &text, uint32_t hash)
{
    if (table->capacity == 0)
        return nullptr;

    uint32_t index = hash % table->capacity;

    for (;;)
    {
        Entry *entry = &table->entries[index];

        if (entry->key == nullptr)
        {
            return nullptr;
        }
        else if (entry->key != TOMBSTONE && entry->key->hash == hash && entry->key->chars == text)
        {
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}

Value clockNative(int argCount, Value *args)
{
    return Value((double)clock() / CLOCKS_PER_SEC);
}

void VM::concatenate(ObjString *a, ObjString *b)
{

    std::string result = a->chars + b->chars;
    ObjString *concatenated = copyString(result);

    stack.push_back(Value(concatenated));
}

ObjString *VM::copyString(const std::string &text)
{
    uint32_t hash = hashString(text);

    ObjString *interned = tableFindString(&strings, text, hash);
    if (interned != nullptr)
    {
        return interned;
    }

    ObjString *string = gc.allocateObject<ObjString>(text);
    ;
    string->hash = hash;
    tableSet(&strings, string, Value{});

    return string;
}

Table &VM::getStrings()
{
    return strings;
}
GarbageCollector &VM::getGC()
{
    return gc;
}

InterpretResult VM::run()
{
    CallFrame *frame = &frames.back();

#if USE_COMPUTED_GOTO
    static void *dispatchTable[256];
    static bool tableReady = false;
    if (!tableReady)
    {
        for (auto &entry : dispatchTable)
            entry = &&L_BAD_OPCODE;

        dispatchTable[(uint8_t)OpCode::OP_RETURN] = &&L_OP_RETURN;
        dispatchTable[(uint8_t)OpCode::OP_CONSTANT] = &&L_OP_CONSTANT;
        dispatchTable[(uint8_t)OpCode::OP_TRUE] = &&L_OP_TRUE;
        dispatchTable[(uint8_t)OpCode::OP_FALSE] = &&L_OP_FALSE;
        dispatchTable[(uint8_t)OpCode::OP_NIL] = &&L_OP_NIL;
        dispatchTable[(uint8_t)OpCode::OP_ADD] = &&L_OP_ADD;
        dispatchTable[(uint8_t)OpCode::OP_SUBTRACT] = &&L_OP_SUBTRACT;
        dispatchTable[(uint8_t)OpCode::OP_MULTIPLY] = &&L_OP_MULTIPLY;
        dispatchTable[(uint8_t)OpCode::OP_DIVIDE] = &&L_OP_DIVIDE;
        dispatchTable[(uint8_t)OpCode::OP_NEGATE] = &&L_OP_NEGATE;
        dispatchTable[(uint8_t)OpCode::OP_NOT] = &&L_OP_NOT;
        dispatchTable[(uint8_t)OpCode::OP_EQUAL] = &&L_OP_EQUAL;
        dispatchTable[(uint8_t)OpCode::OP_GREATER] = &&L_OP_GREATER;
        dispatchTable[(uint8_t)OpCode::OP_LESS] = &&L_OP_LESS;
        dispatchTable[(uint8_t)OpCode::OP_POP] = &&L_OP_POP;
        dispatchTable[(uint8_t)OpCode::OP_PRINT] = &&L_OP_PRINT;
        dispatchTable[(uint8_t)OpCode::OP_DEFINE_GLOBAL] = &&L_OP_DEFINE_GLOBAL;
        dispatchTable[(uint8_t)OpCode::OP_GET_GLOBAL] = &&L_OP_GET_GLOBAL;
        dispatchTable[(uint8_t)OpCode::OP_SET_GLOBAL] = &&L_OP_SET_GLOBAL;
        dispatchTable[(uint8_t)OpCode::OP_GET_LOCAL] = &&L_OP_GET_LOCAL;
        dispatchTable[(uint8_t)OpCode::OP_SET_LOCAL] = &&L_OP_SET_LOCAL;
        dispatchTable[(uint8_t)OpCode::OP_JUMP] = &&L_OP_JUMP;
        dispatchTable[(uint8_t)OpCode::OP_JUMP_IF_FALSE] = &&L_OP_JUMP_IF_FALSE;
        dispatchTable[(uint8_t)OpCode::OP_LOOP] = &&L_OP_LOOP;
        dispatchTable[(uint8_t)OpCode::OP_CALL] = &&L_OP_CALL;
        dispatchTable[(uint8_t)OpCode::OP_CLOSURE] = &&L_OP_CLOSURE;
        dispatchTable[(uint8_t)OpCode::OP_GET_UPVALUE] = &&L_OP_GET_UPVALUE;
        dispatchTable[(uint8_t)OpCode::OP_SET_UPVALUE] = &&L_OP_SET_UPVALUE;
        tableReady = true;
    }

#define CASE(op) L_##op:
#define NEXT() goto *dispatchTable[READ_BYTE()]

    NEXT();
#else
#define CASE(op) case (uint8_t)OpCode::op:
#define NEXT() break

    while (true)
    {
        switch (READ_BYTE())
        {
#endif

    CASE(OP_RETURN)
    {
        Value result = stack.back();
        stack.pop_back();

        closeUpvalues(&stack[frame->slotStart]);

        size_t frameBase = frame->slotStart - 1;
        frames.pop_back();

        if (frames.empty())
        {
            return InterpretResult::INTERPRET_OK; // global script is done
        }

        stack.resize(frameBase);
        stack.push_back(result);
        frame = &frames.back();
        NEXT();
    }
    CASE(OP_CONSTANT)
    {
        uint8_t constIndex = READ_BYTE();
        Value constant = frame->closure->function->chunk.constants[constIndex];
        stack.push_back(constant);
        NEXT();
    }
    CASE(OP_TRUE)
    {
        stack.push_back(Value(true));
        NEXT();
    }
    CASE(OP_FALSE)
    {
        stack.push_back(Value(false));
        NEXT();
    }
    CASE(OP_NIL)
    {
        stack.push_back(Value());
        NEXT();
    }
    CASE(OP_ADD)
    {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        if (!(a.type == ValueType::VAL_NUMBER && b.type == ValueType::VAL_NUMBER))
        {
            if (a.isString() && b.isString())
            {
                concatenate(a.asString(), b.asString());
                NEXT();
            }
            runtimeError("Operands must be numbers.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(Value(a.as.number + b.as.number));
        NEXT();
    }
    CASE(OP_SUBTRACT)
    {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        if (a.type != ValueType::VAL_NUMBER || b.type != ValueType::VAL_NUMBER)
        {
            runtimeError("Operands must be numbers.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(Value(a.as.number - b.as.number));
        NEXT();
    }
    CASE(OP_MULTIPLY)
    {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        if (a.type != ValueType::VAL_NUMBER || b.type != ValueType::VAL_NUMBER)
        {
            runtimeError("Operands must be numbers.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(Value(a.as.number * b.as.number));
        NEXT();
    }
    CASE(OP_DIVIDE)
    {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        if (a.type != ValueType::VAL_NUMBER || b.type != ValueType::VAL_NUMBER)
        {
            runtimeError("Operands must be numbers.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(Value(a.as.number / b.as.number));
        NEXT();
    }
    CASE(OP_NEGATE)
    {
        Value a = stack.back();
        stack.pop_back();

        if (a.type != ValueType::VAL_NUMBER)
        {
            runtimeError("Operands must be numbers.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(Value(a.as.number * -1));
        NEXT();
    }
    CASE(OP_NOT)
    {
        Value a = stack.back();
        stack.pop_back();
        bool falsy = (a.type == ValueType::VAL_NIL) || (a.type == ValueType::VAL_BOOL && !a.as.boolean);
        stack.push_back(Value(falsy));
        NEXT();
    }
    CASE(OP_EQUAL)
    {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        stack.push_back(Value(valuesEqual(a, b)));
        NEXT();
    }
    CASE(OP_GREATER)
    {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        if (a.type != ValueType::VAL_NUMBER || b.type != ValueType::VAL_NUMBER)
        {
            runtimeError("Operands must be numbers.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(Value(a.as.number > b.as.number));
        NEXT();
    }
    CASE(OP_LESS)
    {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();

        if (a.type != ValueType::VAL_NUMBER || b.type != ValueType::VAL_NUMBER)
        {
            runtimeError("Operands must be numbers.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(Value(a.as.number < b.as.number));
        NEXT();
    }
    CASE(OP_POP)
    {
        stack.pop_back();
        NEXT();
    }
    CASE(OP_PRINT)
    {
        Value val = stack.back();
        stack.pop_back();
        printValue(val);
        NEXT();
    }
    CASE(OP_DEFINE_GLOBAL)
    {
        uint8_t nameIndex = READ_BYTE();
        ObjString *name = frame->closure->function->chunk.constants[nameIndex].asString();
        tableSet(&globals, name, stack.back());
        stack.pop_back();
        NEXT();
    }
    CASE(OP_GET_GLOBAL)
    {
        uint8_t nameIndex = READ_BYTE();
        ObjString *name = frame->closure->function->chunk.constants[nameIndex].asString();
        Value value;
        if (!tableGet(&globals, name, &value))
        {
            runtimeError("Undefined variable '" + name->chars + "'.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        stack.push_back(value);
        NEXT();
    }
    CASE(OP_SET_GLOBAL)
    {
        uint8_t nameIndex = READ_BYTE();
        ObjString *name = frame->closure->function->chunk.constants[nameIndex].asString();

        if (tableSet(&globals, name, stack.back()))
        {
            tableDelete(&globals, name);
            runtimeError("Undefined variable '" + name->chars + "'.");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
        NEXT();
    }
    CASE(OP_SET_LOCAL)
    {
        uint8_t slot = READ_BYTE();
        stack[frame->slotStart + slot] = stack.back();
        NEXT();
    }
    CASE(OP_GET_LOCAL)
    {
        uint8_t slot = READ_BYTE();
        stack.push_back(stack[frame->slotStart + slot]);
        NEXT();
    }
    CASE(OP_JUMP)
    {
        uint16_t offset = (frame->ip[0] << 8) | frame->ip[1];
        frame->ip += 2;
        frame->ip += offset;
        NEXT();
    }
    CASE(OP_JUMP_IF_FALSE)
    {
        uint16_t offset = (frame->ip[0] << 8) | frame->ip[1];
        frame->ip += 2;
        Value condition = stack.back();
        bool falsy = (condition.type == ValueType::VAL_NIL) || (condition.type == ValueType::VAL_BOOL && !condition.as.boolean);
        if (falsy)
        {
            frame->ip += offset;
        }
        NEXT();
    }
    CASE(OP_LOOP)
    {
        uint16_t offset = (frame->ip[0] << 8) | frame->ip[1];
        frame->ip += 2;
        frame->ip -= offset;
        NEXT();
    }
    CASE(OP_CALL)
    {
        uint8_t argCount = READ_BYTE();

        Value callee = stack[stack.size() - 1 - argCount];

        if (callee.isNative())
        {
            NativeFn native = callee.asNative()->function;
            // make sure this is right
            Value result = native(argCount, &stack[stack.size() - argCount]);
            stack.resize(stack.size() - argCount - 1);
            stack.push_back(result);
            NEXT();
        }
        else if (!callee.isClosure())
        {
            runtimeError("not a function");
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }

        ObjClosure *closure = callee.asClosure();
        if (argCount != closure->function->arity)
        {
            runtimeError("Expected " + std::to_string(closure->function->arity) + " arguments but got " + std::to_string(argCount));
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }

        CallFrame newFrame;
        newFrame.closure = closure;
        newFrame.ip = closure->function->chunk.code.data();
        newFrame.slotStart = stack.size() - argCount;

        frames.push_back(newFrame);
        frame = &frames.back();
        NEXT();
    }
    CASE(OP_CLOSURE)
    {
        uint8_t constIndex = READ_BYTE();
        LoxFunction *fn = frame->closure->function->chunk.constants[constIndex].asFunction();

        ObjClosure *closure = gc.allocateObject<ObjClosure>(fn);
        gc.pinRoot(closure);

        for (int i = 0; i < fn->upvalueCount; i++)
        {
            uint8_t isLocal = READ_BYTE();
            uint8_t index = READ_BYTE();

            if (isLocal)
            {
                closure->upvalues.push_back(captureUpvalue(&stack[frame->slotStart + index]));
            }
            else
            {
                closure->upvalues.push_back(frame->closure->upvalues[index]);
            }
        }

        gc.unpinRoot(closure);
        stack.push_back(Value(closure));
        NEXT();
    }
    CASE(OP_GET_UPVALUE)
    {
        uint8_t slot = READ_BYTE();
        stack.push_back(*frame->closure->upvalues[slot]->location);
        NEXT();
    }
    CASE(OP_SET_UPVALUE)
    {
        uint8_t slot = READ_BYTE();
        *frame->closure->upvalues[slot]->location = stack.back();
        NEXT();
    }

#if USE_COMPUTED_GOTO
L_BAD_OPCODE:
#else
        default:
#endif
    {
        runtimeError("Unknown opcode.");
        return InterpretResult::INTERPRET_RUNTIME_ERROR;
    }

#if !USE_COMPUTED_GOTO
}
}
#endif

#undef CASE
#undef NEXT
#undef READ_BYTE
}

InterpretResult VM::interpret(LoxFunction *script)
{
    ObjClosure *closure = gc.allocateObject<ObjClosure>(script);

    CallFrame frame;
    frame.closure = closure;
    frame.ip = closure->function->chunk.code.data();
    frame.slotStart = 0;

    frames.push_back(frame);

    return run();
}

void VM::runtimeError(const std::string &message)
{
    CallFrame &frame = frames.back();
    size_t instructionIndex = frame.ip - frame.closure->function->chunk.code.data() - 1;
    int line = frame.closure->function->chunk.lines[instructionIndex];
    std::cerr << "[line " << line << "] Runtime Error: " << message << std::endl;
    stack.clear();
}

void VM::defineNative(const std::string &name, NativeFn function)
{
    ObjString *nameObj = copyString(name);
    stack.push_back(Value(nameObj));
    stack.push_back(Value(gc.allocateObject<ObjNative>(function)));
    tableSet(&globals, nameObj, stack[stack.size() - 1]);
    stack.pop_back();
    stack.pop_back();
}

ObjUpvalue *VM::captureUpvalue(Value *localSlot)
{
    ObjUpvalue *prev = nullptr;
    ObjUpvalue *current = openUpvalues;

    while (current != nullptr && current->location > localSlot)
    {
        prev = current;
        current = current->nextOpen;
    }

    if (current != nullptr && current->location > localSlot)
    {
        return current;
    }

    ObjUpvalue *created = gc.allocateObject<ObjUpvalue>(localSlot);
    created->nextOpen = current;

    if (prev == nullptr)
    {
        openUpvalues = created;
    }
    else
    {
        prev->nextOpen = created;
    }

    return created;
}

void VM::closeUpvalues(Value *last)
{
    while (openUpvalues != nullptr && openUpvalues->location >= last)
    {
        ObjUpvalue *upvalue = openUpvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        openUpvalues = upvalue->nextOpen;
    }
}

void VM::markRoots()
{
    for (const Value &v : stack)
    {
        gc.mark(v);
    }

    for (int i = 0; i < globals.capacity; i++)
    {
        Entry &entry = globals.entries[i];
        if (entry.key == nullptr || entry.key == TOMBSTONE)
        {
            continue;
        }
        gc.mark(entry.key);
        if (entry.value.has_value())
        {
            gc.mark(entry.value.value());
        }
    }

    for (const CallFrame &frame : frames)
    {
        gc.mark(frame.closure);
    }

    ObjUpvalue *current = openUpvalues;
    while (current)
    {
        gc.mark(current);
        current = current->nextOpen;
    }
}

void VM::tableRemoveWhite(Table *strings)
{
    for (int i = 0; i < strings->capacity; ++i)
    {
        Entry &entry = strings->entries[i];
        if (entry.key != NULL && !entry.key->isMarked)
        {
            tableDelete(strings, entry.key);
        }
    }
}