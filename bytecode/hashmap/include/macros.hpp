#pragma once
#include "../../include/Helper/types.hpp"

#define TABLE_MAX_LOAD 0.75

template <typename T>
void FREE_ARRAY(T *&entries, int &capacity);

template <typename T>
T *ALLOCATE(int capacity);

int GROW_CAPACITY(int &capacity);