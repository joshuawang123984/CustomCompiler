#pragma once
#include <iostream>

class Chunk;
struct Value;

void disassembleChunk(Chunk &chunk, const std::string &name);
int disassembleInstruction(Chunk &chunk, int offset);
void printValue(Value val);
void printObject(Value val);
