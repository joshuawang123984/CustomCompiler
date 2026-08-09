#include <iostream>
#include <vector>

#include "Types.hpp"

enum class OpCode : uint8_t
{
    OP_CONSTANT,
    OP_RETURN,
};

class Chunk
{
private:
public:
    std::vector<uint8_t> code;
    std::vector<int> lines;
    std::vector<Value> constants;

    void write(uint8_t byte, int line);
    int addConstant(Value value);
};