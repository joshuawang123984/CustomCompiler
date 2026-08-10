#pragma once

class Obj;

enum class ValueType
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ, // strings, functions, classes, instances, etc
};

struct Value
{
    ValueType type;
    union
    {
        bool boolean;
        double number;
        // implement this later
        Obj *obj; // raw pointer to a heap-allocated object
    } as;

    Value() : type(ValueType::VAL_NIL), as{.number = 0} {}
    Value(double d) : type(ValueType::VAL_NUMBER), as{.number = d} {}
    Value(bool b) : type(ValueType::VAL_BOOL), as{.boolean = b} {}
};