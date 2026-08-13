#include "../../include/Helper/chunk.hpp"
#include "../../include/Helper/Obj.hpp"

bool Value::isString() const
{
    return type == ValueType::VAL_OBJ && as.obj->type == ObjType::OBJ_STRING;
}
ObjString *Value::asString() const
{
    return static_cast<ObjString *>(as.obj);
}

bool Value::isInstance() const
{
    return type == ValueType::VAL_OBJ && as.obj->type == ObjType::OBJ_INSTANCE;
}
LoxInstance *Value::asInstance() const
{
    return static_cast<LoxInstance *>(as.obj);
}

bool Value::isClass() const
{
    return type == ValueType::VAL_OBJ && as.obj->type == ObjType::OBJ_CLASS;
}
LoxClass *Value::asClass() const
{
    return static_cast<LoxClass *>(as.obj);
}

bool Value::isFunction() const
{
    return type == ValueType::VAL_OBJ && as.obj->type == ObjType::OBJ_FUNCTION;
}
LoxFunction *Value::asFunction() const
{
    return static_cast<LoxFunction *>(as.obj);
}