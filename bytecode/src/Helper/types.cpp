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

bool Value::isNative() const
{
    return type == ValueType::VAL_OBJ && as.obj->type == ObjType::OBJ_NATIVE;
}
ObjNative *Value::asNative() const
{
    return static_cast<ObjNative *>(as.obj);
}

bool Value::isClosure() const
{
    return type == ValueType::VAL_OBJ && as.obj->type == ObjType::OBJ_CLOSURE;
}
ObjClosure *Value::asClosure() const
{
    return static_cast<ObjClosure *>(as.obj);
}

bool Value::isUpValue() const
{
    return type == ValueType::VAL_OBJ && as.obj->type == ObjType::OBJ_UPVALUE;
}
ObjUpvalue *Value::asUpValue() const
{
    return static_cast<ObjUpvalue *>(as.obj);
}