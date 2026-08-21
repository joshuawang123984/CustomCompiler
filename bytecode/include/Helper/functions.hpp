#pragma once
#include <iostream>
#include "types.hpp"
#include "TokenVector.hpp"

std::string runFile(const std::string &path);
void printTokens(const TokenVector &tokens);

class Chunk;
struct Value;

int simpleInstruction(const std::string &name, int offset);
int constantInstruction(const std::string &name, Chunk &chunk, int offset);

void disassembleChunk(Chunk &chunk, const std::string &name);
int disassembleInstruction(Chunk &chunk, int offset);
int byteInstruction(const std::string &name, Chunk &chunk, int offset);
void printValue(const Value &val);
void printObject(const Value &val);
bool valuesEqual(const Value &a, const Value &b);

int jumpInstruction(const std::string &name, int sign, Chunk &chunk, int offset);