#pragma once
#include <iostream>

class Chunk;
struct Value;

void disassembleChunk(Chunk &chunk, const std::string &name);
int disassembleInstruction(Chunk &chunk, int offset);
void printValue(const Value &val);
void printObject(const Value &val);
bool valuesEqual(const Value &a, const Value &b);
