#include "../../include/Helper/VM.hpp"
#include "../../include/Helper/chunk.hpp"
#include "../../include/Helper/functions.hpp"

InterpretResult VM::run()
{
    while (true)
    {
        uint8_t instruction = *ip;
        ip++;

        switch (instruction)
        {
        case (uint8_t)OpCode::OP_RETURN:
            return InterpretResult::INTERPRET_OK;

        case (uint8_t)OpCode::OP_CONSTANT:
        {
            uint8_t constIndex = *ip;
            ip++;
            Value constant = chunk->constants[constIndex];
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
        case (uint8_t)OpCode::OP_NIL:
            stack.push_back(Value());
            break;
        }
    }
}

InterpretResult VM::interpret(Chunk &chunkArg)
{
    chunk = &chunkArg;
    ip = chunk->code.data();
    return run();
}

void VM::runtimeError(const std::string &message)
{
    size_t instructionIndex = ip - chunk->code.data() - 1;
    int line = chunk->lines[instructionIndex];
    std::cerr << "[line " << line << "] Runtime Error: " << message << std::endl;
    stack.clear();
}