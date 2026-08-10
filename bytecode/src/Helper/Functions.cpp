#include "../../include/Helper/Chunk.hpp"
#include "../../include/Helper/Functions.hpp"
#include <iomanip>

void disassembleChunk(Chunk &chunk, const std::string &name)
{
    std::cout << "== " << name << " ==" << std::endl;

    int offset = 0;
    while (offset < chunk.code.size())
    {
        offset = disassembleInstruction(chunk, offset);
    }
}

int disassembleInstruction(Chunk &chunk, int offset)
{
    std::cout << std::setfill('0') << std::setw(4) << offset << " ";

    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1])
        std::cout << "   | ";
    else
        std::cout << std::setw(4) << chunk.lines[offset] << " ";

    uint8_t instruction = chunk.code[offset];

    switch (instruction)
    {
    case (uint8_t)OpCode::OP_RETURN:
        std::cout << "OP_RETURN" << std::endl;
        return offset + 1;

    case (uint8_t)OpCode::OP_CONSTANT:
    {
        uint8_t constIndex = chunk.code[offset + 1];
        std::cout << "OP_CONSTANT  " << (int)constIndex << " '";
        printValue(chunk.constants[constIndex]);
        std::cout << "'" << std::endl;
        return offset + 2;
    }

    default:
        std::cout << "Unknown opcode " << (int)instruction << std::endl;
        return offset + 1;
    }
}

void printValue(Value val)
{
    switch (val.type)
    {
    case ValueType::VAL_BOOL:
        std::cout << (val.as.boolean ? "true" : "false");
        break;
    case ValueType::VAL_NIL:
        std::cout << "nil";
        break;
    case ValueType::VAL_NUMBER:
        std::cout << val.as.number;
        break;
    case ValueType::VAL_OBJ:
        printObject(val);
        break;
    }
}

void printObject(Value val)
{
    // switch ()
    // {
    // }
}