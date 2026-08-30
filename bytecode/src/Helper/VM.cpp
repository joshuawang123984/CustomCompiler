#include "../../include/Helper/VM.hpp"
#include "../../include/Helper/chunk.hpp"
#include "../../include/Helper/functions.hpp"
#include "../../include/Helper/Obj.hpp"

#include <ctime>

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

ObjString *VM::copyString(const std::string &text)
{
    uint32_t hash = hashString(text);

    ObjString *interned = tableFindString(&strings, text, hash);
    if (interned != nullptr)
    {
        return interned;
    }

    ObjString *string = new ObjString(text);
    string->hash = hash;
    tableSet(&strings, string, Value{});

    return string;
}

Table &VM::getStrings()
{
    return strings;
}

InterpretResult VM::run()
{
    CallFrame *frame = &frames.back();

    while (true)
    {
        uint8_t instruction = *frame->ip;
        frame->ip++;

        switch (instruction)
        {
        case (uint8_t)OpCode::OP_RETURN:
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
            break;
        }
        case (uint8_t)OpCode::OP_CONSTANT:
        {
            uint8_t constIndex = *frame->ip;
            frame->ip++;
            Value constant = frame->closure->function->chunk.constants[constIndex];
            stack.push_back(constant);
            break;
        }
        case (uint8_t)OpCode::OP_TRUE:
            stack.push_back(Value(true));
            break;
        case (uint8_t)OpCode::OP_FALSE:
            stack.push_back(Value(false));
            break;
        case (uint8_t)OpCode::OP_ADD:
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
            stack.push_back(Value(a.as.number + b.as.number));
            break;
        }
        case (uint8_t)OpCode::OP_SUBTRACT:
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
            break;
        }
        case (uint8_t)OpCode::OP_MULTIPLY:
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
            break;
        }
        case (uint8_t)OpCode::OP_DIVIDE:
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
            break;
        }
        case (uint8_t)OpCode::OP_NEGATE:
        {
            Value a = stack.back();
            stack.pop_back();

            if (a.type != ValueType::VAL_NUMBER)
            {
                runtimeError("Operands must be numbers.");
                return InterpretResult::INTERPRET_RUNTIME_ERROR;
            }
            stack.push_back(Value(a.as.number * -1));
            break;
        }
        case (uint8_t)OpCode::OP_NOT:
        {
            Value a = stack.back();
            stack.pop_back();
            bool falsy = (a.type == ValueType::VAL_NIL) || (a.type == ValueType::VAL_BOOL && !a.as.boolean);
            stack.push_back(Value(falsy));
            break;
        }
        case (uint8_t)OpCode::OP_EQUAL:
        {
            Value b = stack.back();
            stack.pop_back();
            Value a = stack.back();
            stack.pop_back();

            stack.push_back(Value(valuesEqual(a, b)));
            break;
        }
        case (uint8_t)OpCode::OP_GREATER:
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
            break;
        }
        case (uint8_t)OpCode::OP_LESS:
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
            break;
        }
        case (uint8_t)OpCode::OP_POP:
            stack.pop_back();
            break;
        case (uint8_t)OpCode::OP_PRINT:
        {
            Value val = stack.back();
            stack.pop_back();
            printValue(val);
            break;
        }
        case (uint8_t)OpCode::OP_DEFINE_GLOBAL:
        {
            uint8_t nameIndex = *frame->ip;
            frame->ip++;
            ObjString *name = frame->closure->function->chunk.constants[nameIndex].asString();
            tableSet(&globals, name, stack.back());
            stack.pop_back();
            break;
        }
        case (uint8_t)OpCode::OP_GET_GLOBAL:
        {
            uint8_t nameIndex = *frame->ip;
            frame->ip++;
            ObjString *name = frame->closure->function->chunk.constants[nameIndex].asString();
            Value value;
            if (!tableGet(&globals, name, &value))
            {
                runtimeError("Undefined variable '" + name->chars + "'.");
                return InterpretResult::INTERPRET_RUNTIME_ERROR;
            }
            stack.push_back(value);
            break;
        }
        case (uint8_t)OpCode::OP_SET_GLOBAL:
        {
            uint8_t nameIndex = *frame->ip;
            frame->ip++;
            ObjString *name = frame->closure->function->chunk.constants[nameIndex].asString();

            if (tableSet(&globals, name, stack.back()))
            {
                tableDelete(&globals, name);
                runtimeError("Undefined variable '" + name->chars + "'.");
                return InterpretResult::INTERPRET_RUNTIME_ERROR;
            }

            break;
        }
        case (uint8_t)OpCode::OP_SET_LOCAL:
        {
            uint8_t slot = *frame->ip;
            frame->ip++;
            stack[frame->slotStart + slot] = stack.back();
            break;
        }
        case (uint8_t)OpCode::OP_GET_LOCAL:
        {
            uint8_t slot = *frame->ip;
            frame->ip++;
            stack.push_back(stack[frame->slotStart + slot]);
            break;
        }
        case (uint8_t)OpCode::OP_JUMP:
        {
            uint16_t offset = (frame->ip[0] << 8) | frame->ip[1];
            frame->ip += 2;
            frame->ip += offset;
            break;
        }
        case (uint8_t)OpCode::OP_JUMP_IF_FALSE:
        {
            uint16_t offset = (frame->ip[0] << 8) | frame->ip[1];
            frame->ip += 2;
            Value condition = stack.back();
            bool falsy = (condition.type == ValueType::VAL_NIL) || (condition.type == ValueType::VAL_BOOL && !condition.as.boolean);
            if (falsy)
            {
                frame->ip += offset;
            }
            break;
        }
        case (uint8_t)OpCode::OP_LOOP:
        {
            uint16_t offset = (frame->ip[0] << 8) | frame->ip[1];
            frame->ip += 2;
            frame->ip -= offset;
            break;
        }
        case (uint8_t)OpCode::OP_CALL:
        {
            uint8_t argCount = *frame->ip;
            frame->ip++;

            Value callee = stack[stack.size() - 1 - argCount];

            if (callee.isNative())
            {
                NativeFn native = callee.asNative()->function;
                // make sure this is right
                Value result = native(argCount, &stack[stack.size() - argCount]);
                stack.resize(stack.size() - argCount - 1);
                stack.push_back(result);
                break;
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
            break;
        }
        case (uint8_t)OpCode::OP_CLOSURE:
        {

            uint8_t constIndex = *frame->ip;
            frame->ip++;
            LoxFunction *fn = frame->closure->function->chunk.constants[constIndex].asFunction();

            ObjClosure *closure = new ObjClosure(fn);

            for (int i = 0; i < fn->upvalueCount; i++)
            {
                uint8_t isLocal = *frame->ip;
                frame->ip++;
                uint8_t index = *frame->ip;
                frame->ip++;

                if (isLocal)
                {
                    closure->upvalues.push_back(captureUpvalue(&stack[frame->slotStart + index]));
                }
                else
                {
                    closure->upvalues.push_back(frame->closure->upvalues[index]);
                }
            }

            stack.push_back(Value(closure));
            break;
        }
        case (uint8_t)OpCode::OP_GET_UPVALUE:
        {
            uint8_t slot = *frame->ip;
            frame->ip++;
            stack.push_back(*frame->closure->upvalues[slot]->location);
            break;
        }
        case (uint8_t)OpCode::OP_SET_UPVALUE:
        {
            uint8_t slot = *frame->ip;
            frame->ip++;
            *frame->closure->upvalues[slot]->location = stack.back();
            break;
        }
        case (uint8_t)OpCode::OP_NIL:
            stack.push_back(Value());
            break;
        }
    }
}

InterpretResult VM::interpret(LoxFunction *script)
{
    ObjClosure *closure = new ObjClosure(script);

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
    stack.push_back(Value(new ObjNative(function)));
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
        current = current->next;
    }

    if (current != nullptr && current->location > localSlot)
    {
        return current;
    }

    ObjUpvalue *created = new ObjUpvalue(localSlot);
    created->next = current;

    if (prev == nullptr)
    {
        openUpvalues = created;
    }
    else
    {
        prev->next = created;
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
        openUpvalues = upvalue->next;
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
        gc.mark(entry.key);
        gc.mark(entry.value.value());
    }

    for (const CallFrame &frame : frames)
    {
        gc.mark(frame.closure);
    }

    ObjUpvalue *current = openUpvalues;
    while (current)
    {
        gc.mark(current);
        current = current->next;
    }
}
