#include "../../include/Helper/types.hpp"

struct Entry
{
    ObjString *key;
    Value value;
};

struct Table
{
    int count;
    int capacity;
    Entry *entries;
};

struct Obj
{
    ObjType type;
    Obj *next;
};

struct ObjString : public Obj
{
    int length;
    uint32_t hash;
    char *chars;

    ObjString(char *chars, int length, uint32_t hash)
    {
        this->type = OBJ_STRING;
        this->length = length;
        this->hash = hash;
        this->chars = chars;
    }
};

template <typename T>
void FREE_ARRAY(T *&entries, int &capacity);

void initTable(Table *table);
void freeTable(Table *table);
