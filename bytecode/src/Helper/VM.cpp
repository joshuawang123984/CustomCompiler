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
            uint8_t constIndex = *ip;
            ip++;
            Value constant = chunk->constants[constIndex];
            stack.push_back(constant);
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