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
};

struct Obj
{
    ObjType type;
    virtual ~Obj() = default;

protected:
    Obj(ObjType type) : type(type) {}
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
    ObjString *name = nullptr;

    LoxFunction() : Obj(ObjType::OBJ_FUNCTION) {}
    LoxFunction(int arity, Chunk chunk, ObjString *name) : Obj(ObjType::OBJ_FUNCTION), arity(arity), chunk(std::move(chunk)), name(name) {}
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