#include "../include/Table.hpp"

template <typename T>
void FREE_ARRAY(T *&entries, int &capacity)
{
    if (entries == nullptr)
        // maybe throw an error or smth in the future
        return;

    delete[] entries;
    entries = nullptr;
    capacity = 0;
}

static uint32_t hashString(const char *key, int length)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++)
    {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

void initTable(Table *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = nullptr;
}

void freeTable(Table *table)
{
    FREE_ARRAY<Entry>(table->entries, table->capacity);
    initTable(table);
}