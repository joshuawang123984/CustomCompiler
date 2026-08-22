#include "../include/Table.hpp"
#include "../include/macros.hpp"

static ObjString *TOMBSTONE = reinterpret_cast<ObjString *>(0x1);

// linear strat
static Entry *findEntry(Entry *entries, int capacity, ObjString *key)
{
    if (capacity == 0)
        return nullptr;

    uint32_t index = key->hash % capacity;
    Entry *tombstone_slot = nullptr;

    for (;;)
    {
        Entry *entry = &entries[index];

        if (entry->key == key)
        {
            return entry;
        }
        else if (entry->key == nullptr)
        {
            return tombstone_slot == nullptr ? entry : tombstone_slot;
        }
        else if (entry->key == TOMBSTONE)
        {
            if (tombstone_slot == nullptr)
                // saves the first tombstone so val can be put
                tombstone_slot = entry;
        }

        index = (index + 1) % capacity;
    }
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

int GROW_CAPACITY(int &capacity)
{
}

void adjustCapacity(Table *table, int &capacity)
{
    Entry *entries = ALLOCATE<Entry>(capacity);
    for (int i = 0; i < capacity; i++)
    {
        entries[i].key = nullptr;
        entries[i].value = std::nullopt;
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i++)
    {
        Entry *entry = &table->entries[i];
        if (entry->key == nullptr || entry->key == TOMBSTONE)
            continue;

        Entry *dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY<Entry>(table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

ObjString::ObjString(char *chars, int length)
{
    this->length = length;
    this->chars = chars;
    this->hash = hashString(chars, length);
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

bool tableGet(Table *table, ObjString *key, Value *value)
{
    if (table->count == 0)
        return false;

    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == nullptr)
        return false;

    *value = *entry->value;
    return true;
}

bool tableSet(Table *table, ObjString *key, Value value)
{
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD)
    {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    Entry *entry = findEntry(table->entries, table->capacity, key);

    bool isNewKey = entry->key != key;
    if (entry->key == nullptr)
        table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool tableDelete(Table *table, ObjString *key)
{
    if (table->count == 0)
        return false;

    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == nullptr)
        return false;

    entry->key = TOMBSTONE;
    entry->value = std::nullopt;

    return true;
}

void tableAddAll(Table *from, Table *to)
{
    for (int i = 0; i < from->capacity; i++)
    {
        Entry *entry = &from->entries[i];
        if (entry->key != nullptr)
        {
            tableSet(to, entry->key, *entry->value);
        }
    }
}