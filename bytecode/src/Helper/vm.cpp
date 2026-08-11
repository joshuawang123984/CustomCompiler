#include "../../include/Helper/vm.hpp"
#include "../../include/Helper/chunk.hpp"

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
            break;
        case (uint8_t)OpCode::OP_FALSE:
            break;
        case (uint8_t)OpCode::OP_ADD:
            break;
        case (uint8_t)OpCode::OP_SUBRACT:
            break;
        case (uint8_t)OpCode::OP_MULTIPLY:
            break;
        case (uint8_t)OpCode::OP_DIVIDE:
            break;
        case (uint8_t)OpCode::OP_NEGATE:
            break;
        case (uint8_t)OpCode::OP_NOT:
            break;
        case (uint8_t)OpCode::OP_EQUAL:
            break;
        case (uint8_t)OpCode::OP_GREATER:
            break;
        case (uint8_t)OpCode::OP_LESS:
            break;
        case (uint8_t)OpCode::OP_NIL:
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