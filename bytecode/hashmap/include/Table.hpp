#pragma once
#include "../../include/Helper/types.hpp"

struct Entry
{
    ObjString *key;
    std::optional<Value> value;
};

struct Table
{
    int count;
    int capacity;
    Entry *entries;
};

struct Obj
{
    virtual ~Obj() = default;
    Obj *next = nullptr;
};

struct ObjString : public Obj
{
    int length;
    uint32_t hash;
    char *chars;

    ObjString(char *chars, int length);
};

void adjustCapacity(Table *table, int &capacity);

void initTable(Table *table);
void freeTable(Table *table);
bool tableGet(Table *table, ObjString *key, Value *value);
bool tableSet(Table *table, ObjString *key, Value value);
bool tableDelete(Table *table, ObjString *key);
void tableAddAll(Table *from, Table *to);
