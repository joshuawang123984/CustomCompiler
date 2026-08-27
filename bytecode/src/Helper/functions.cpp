#include "../../include/Helper/chunk.hpp"
#include "../../include/Helper/functions.hpp"
#include "../../include/Helper/Obj.hpp"
#include <iomanip>
#include <fstream>

std::string runFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << path << std::endl;
        std::exit(74);
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return source;
}

void printTokens(const TokenVector &tokens)
{

    for (const auto &token : tokens.getTokens())
    {
        std::cout
            << " | Lexeme: " << token.lexeme
            << " | Line: " << token.line << std::endl;
    }
}

int simpleInstruction(const std::string &name, int offset)
{
    std::cout << name << std::endl;
    return offset + 1;
}

int constantInstruction(const std::string &name, Chunk &chunk, int offset)
{
    uint8_t constIndex = chunk.code[offset + 1];
    std::cout << name << "  " << (int)constIndex << " '";
    printValue(chunk.constants[constIndex]);
    std::cout << "'" << std::endl;
    return offset + 2;
}

int byteInstruction(const std::string &name, Chunk &chunk, int offset)
{
    uint8_t slot = chunk.code[offset + 1];
    std::cout << name << "  " << (int)slot << std::endl;
    return offset + 2;
}

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
        return simpleInstruction("OP_RETURN", offset);
    case (uint8_t)OpCode::OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);

    case (uint8_t)OpCode::OP_NIL:
        return simpleInstruction("OP_NIL", offset);
    case (uint8_t)OpCode::OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);
    case (uint8_t)OpCode::OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);

    case (uint8_t)OpCode::OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case (uint8_t)OpCode::OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case (uint8_t)OpCode::OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case (uint8_t)OpCode::OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case (uint8_t)OpCode::OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case (uint8_t)OpCode::OP_NOT:
        return simpleInstruction("OP_NOT", offset);

    case (uint8_t)OpCode::OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);
    case (uint8_t)OpCode::OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);
    case (uint8_t)OpCode::OP_LESS:
        return simpleInstruction("OP_LESS", offset);

    case (uint8_t)OpCode::OP_POP:
        return simpleInstruction("OP_POP", offset);
    case (uint8_t)OpCode::OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);

    case (uint8_t)OpCode::OP_DEFINE_GLOBAL:
        return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
    case (uint8_t)OpCode::OP_GET_GLOBAL:
        return constantInstruction("OP_GET_GLOBAL", chunk, offset);
    case (uint8_t)OpCode::OP_SET_GLOBAL:
        return constantInstruction("OP_SET_GLOBAL", chunk, offset);
    case (uint8_t)OpCode::OP_GET_LOCAL:
        return byteInstruction("OP_GET_LOCAL", chunk, offset);
    case (uint8_t)OpCode::OP_SET_LOCAL:
        return byteInstruction("OP_SET_LOCAL", chunk, offset);
    case (uint8_t)OpCode::OP_JUMP:
        return jumpInstruction("OP_JUMP", 1, chunk, offset);
    case (uint8_t)OpCode::OP_JUMP_IF_FALSE:
        return jumpInstruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
    case (uint8_t)OpCode::OP_LOOP:
        return jumpInstruction("OP_LOOP", -1, chunk, offset);
    case (uint8_t)OpCode::OP_CALL:
        return byteInstruction("OP_CALL", chunk, offset);

    default:
        std::cout << "Unknown opcode " << (int)instruction << std::endl;
        return offset + 1;
    }
}

void printValue(const Value &val)
{
    switch (val.type)
    {
    case ValueType::VAL_BOOL:
        std::cout << (val.as.boolean ? "true" : "false") << std::endl;
        break;
    case ValueType::VAL_NIL:
        std::cout << "nil" << std::endl;
        break;
    case ValueType::VAL_NUMBER:
        std::cout << val.as.number << std::endl;
        break;
    case ValueType::VAL_OBJ:
        printObject(val);
        break;
    }
}

void printObject(const Value &val)
{
    switch (val.as.obj->type)
    {
    case ObjType::OBJ_STRING:
        std::cout << val.asString()->chars << std::endl;
        break;

    case ObjType::OBJ_CLASS:
        std::cout << val.asClass()->name->chars << std::endl;
        break;

    case ObjType::OBJ_INSTANCE:
        std::cout << val.asInstance()->klass->name->chars << " instance" << std::endl;
        break;

    case ObjType::OBJ_FUNCTION:
        std::cout << "<fn " << (val.asFunction()->name ? val.asFunction()->name->chars : "anonymous") << ">" << std::endl;
        break;
    case ObjType::OBJ_NATIVE:
        std::cout << "<native fn>" << std::endl;
        break;
    }
}

bool valuesEqual(const Value &a, const Value &b)
{
    if (a.type != b.type)
        return false;
    switch (a.type)
    {
    case ValueType::VAL_NIL:
        return true;
    case ValueType::VAL_BOOL:
        return a.as.boolean == b.as.boolean;
    case ValueType::VAL_NUMBER:
        return a.as.number == b.as.number;
    case ValueType::VAL_OBJ:
        return a.as.obj == b.as.obj;
    }
    return false;
}

int jumpInstruction(const std::string &name, int sign, Chunk &chunk, int offset)
{
    uint16_t jump = (uint16_t)(chunk.code[offset + 1] << 8);
    jump |= chunk.code[offset + 2];
    std::cout << name << "  " << offset << " -> " << (offset + 3 + sign * jump) << std::endl;
    return offset + 3;
}