#pragma once
#include <string>
#include <unordered_map>
#include "types.hpp"
#include "chunk.hpp"

enum class ObjType
{
    OBJ_STRING,
    OBJ_FUNCTION,
    OBJ_CLASS,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_CLOSURE,
    OBJ_UPVALUE,
};

struct Obj
{
    ObjType type;
    Obj *next = nullptr;
    bool isMarked = false;
    virtual ~Obj() = default;

protected:
    Obj(ObjType type) : type(type) {}
};

struct ObjUpvalue : Obj
{
    Value *location;
    Value closed;
    ObjUpvalue *nextOpen = nullptr;

    ObjUpvalue(Value *slot) : Obj(ObjType::OBJ_UPVALUE), location(slot) {}
};

typedef Value (*NativeFn)(int argCount, Value *args);

struct ObjNative : Obj
{
    NativeFn function;
    ObjNative(NativeFn fn) : Obj(ObjType::OBJ_NATIVE), function(fn) {}
};

struct ObjString : Obj
{
    std::string chars;
    uint32_t hash;
    ObjString(std::string s) : Obj(ObjType::OBJ_STRING), chars(std::move(s)) {}
};

struct LoxFunction : Obj
{
    int arity = 0;
    Chunk chunk;
    int upvalueCount = 0;
    ObjString *name = nullptr;

    LoxFunction() : Obj(ObjType::OBJ_FUNCTION) {}
    LoxFunction(int arity, Chunk chunk, ObjString *name) : Obj(ObjType::OBJ_FUNCTION), arity(arity), chunk(std::move(chunk)), name(name) {}
};

struct ObjClosure : Obj
{
    LoxFunction *function;
    std::vector<ObjUpvalue *> upvalues;
    ObjClosure(LoxFunction *function) : Obj(ObjType::OBJ_CLOSURE), function(function) {}
};

struct LoxClass : Obj
{
    ObjString *name;
    std::unordered_map<std::string, Value> methods;
    LoxClass *superclass = nullptr;
    LoxClass(ObjString *name, LoxClass *superclass) : Obj(ObjType::OBJ_CLASS), name(name), superclass(superclass) {}
};

struct LoxInstance : Obj
{
    LoxClass *klass;
    std::unordered_map<std::string, Value> fields;
    LoxInstance(LoxClass *klass) : Obj(ObjType::OBJ_INSTANCE), klass(klass) {}
};