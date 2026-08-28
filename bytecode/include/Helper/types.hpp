#pragma once
#include <string>

struct Obj;
struct ObjUpvalue;
struct ObjNative;
struct ObjString;
struct LoxFunction;
struct ObjClosure;
struct LoxClass;
struct LoxInstance;

enum class ValueType
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ,
};

struct Value
{
    ValueType type;
    union
    {
        bool boolean;
        double number;
        Obj *obj;
    } as;

    Value() : type(ValueType::VAL_NIL), as{.number = 0} {}
    Value(double d) : type(ValueType::VAL_NUMBER), as{.number = d} {}
    Value(bool b) : type(ValueType::VAL_BOOL), as{.boolean = b} {}
    Value(Obj *o) : type(ValueType::VAL_OBJ), as{.obj = o} {}

    bool isString() const;
    ObjString *asString() const;

    bool isInstance() const;
    LoxInstance *asInstance() const;

    bool isClass() const;
    LoxClass *asClass() const;

    bool isFunction() const;
    LoxFunction *asFunction() const;

    bool isNative() const;
    ObjNative *asNative() const;

    bool isClosure() const;
    ObjClosure *asClosure() const;

    bool isUpValue() const;
    ObjUpvalue *asUpValue() const;
};