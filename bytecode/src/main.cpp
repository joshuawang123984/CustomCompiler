#include "../include/Helper/functions.hpp"
#include "../include/Helper/chunk.hpp"

int main()
{
    Chunk chunk;

    int constIndex = chunk.addConstant(1.2);
    chunk.write((uint8_t)OpCode::OP_CONSTANT, 123);
    chunk.write((uint8_t)constIndex, 123);

    chunk.write((uint8_t)OpCode::OP_RETURN, 123);

    disassembleChunk(chunk, "test chunk");

    return 0;
}