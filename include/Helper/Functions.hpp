#include <iostream>
#include <fstream>
#include "Types.hpp"
#include "TokenVector.hpp"

std::string runFile(const std::string &path);
std::string stringify(Value value);

bool isTruthy(Value value);

void printTokens(const TokenVector &tokens);
void printValue(Value val);
