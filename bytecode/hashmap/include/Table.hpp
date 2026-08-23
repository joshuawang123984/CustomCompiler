#pragma once
#include "../../include/Helper/types.hpp"
#include "../../include/Helper/Obj.hpp"
#include <optional>

struct Entry
{
    ObjString *key = nullptr;
    std::optional<Value> value;
};

struct Table
{
    int count;
    int capacity;
    Entry *entries;
};

void adjustCapacity(Table *table, int &capacity);

void initTable(Table *table);
void freeTable(Table *table);
bool tableGet(Table *table, ObjString *key, Value *value);
bool tableSet(Table *table, ObjString *key, Value value);
bool tableDelete(Table *table, ObjString *key);
void tableAddAll(Table *from, Table *to);
