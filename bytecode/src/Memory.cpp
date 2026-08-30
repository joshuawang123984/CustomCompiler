#include "../include/Memory.hpp"
#include "../include/Helper/types.hpp"
#include "../include/Helper/Obj.hpp"
#include <iostream>

void GarbageCollector::setMarkRootsCallback(std::function<void()> fn)
{
    markRootsFn = fn;
}
void *GarbageCollector::reallocate(void *pointer, size_t oldSize, size_t newSize)
{
    total_bytes += newSize - oldSize;

    if (newSize > oldSize)
    {
#ifdef DEBUG_STRESS_GC
        collect();
#endif
    }

    if (threshold < total_bytes)
    {
        collect();
    }

    if (newSize == 0)
    {
        free(pointer);
        return nullptr;
    }

    void *result = realloc(pointer, newSize);
    if (result == nullptr)
        exit(1);

    return result;
}
void GarbageCollector::mark(Value value)
{
    if (value.type != ValueType::VAL_OBJ)
    {
        return;
    }

    mark(value.as.obj);
}
void GarbageCollector::mark(Obj *object)
{
    if (object == nullptr)
    {
        return;
    }

    if (object->isMarked == true)
    {
        return;
    }

    object->isMarked = true;
    grayStack.push_back(object);
}
void GarbageCollector::blackenObject(Obj *object)
{
    switch (object->type)
    {
    case ObjType::OBJ_STRING:
    {
        break;
    }
    case ObjType::OBJ_FUNCTION:
    {
        LoxFunction *function = static_cast<LoxFunction *>(object);
        mark(function->name);
        for (Value &constant : function->chunk.constants)
        {
            mark(constant);
        }
        break;
    }
    case ObjType::OBJ_CLASS:
    {
        LoxClass *klass = static_cast<LoxClass *>(object);
        mark(klass->name);
        mark(klass->superclass);
        for (const auto &pair : klass->methods)
        {
            mark(pair.second);
        }

        break;
    }
    case ObjType::OBJ_INSTANCE:
    {
        LoxInstance *instance = static_cast<LoxInstance *>(object);
        mark(instance->klass);
        for (const auto &pair : instance->fields)
        {
            mark(pair.second);
        }
        break;
    }
    case ObjType::OBJ_NATIVE:
    {
        break;
    }
    case ObjType::OBJ_CLOSURE:
    {
        ObjClosure *closure = static_cast<ObjClosure *>(object);
        mark(closure->function);
        for (ObjUpvalue *upvalue : closure->upvalues)
        {
            mark(upvalue);
        }
        break;
    }
    case ObjType::OBJ_UPVALUE:
    {
        ObjUpvalue *upvalue = static_cast<ObjUpvalue *>(object);
        mark(upvalue->closed);
        break;
    }
    default:
        break;
    }
}
void GarbageCollector::traceReferences()
{
    while (!grayStack.empty())
    {
        Obj *marked = grayStack.back();
        grayStack.pop_back();
        blackenObject(marked);
    }
}
void GarbageCollector::sweep()
{
    Obj *previous = nullptr;
    Obj *current = objects;

    while (current != nullptr)
    {
        if (current->isMarked)
        {
            current->isMarked = false;
            previous = current;
            current = current->next;
        }
        else
        {
            Obj *unreached = current;
            if (previous != nullptr)
            {
                previous->next = current->next;
                current = current->next;
            }
            else
            {
                current = current->next;
                objects = current;
            }

            delete unreached;
        }
    }
}
void GarbageCollector::freeObjects()
{
    Obj *current = objects;
    while (current)
    {
        Obj *toDelete = current;
        current = current->next;
        delete toDelete;
    }

    objects = nullptr;
}
void GarbageCollector::collect()
{
    if (markRootsFn == nullptr)
    {
        std::cout << "mark roots not defined in vm." << std::endl;
        return;
    }

    std::cout << "-- starting gc --" << std::endl;

    markRootsFn();
    traceReferences();
    sweep();

    std::cout << "-- ending gc --" << std::endl;
}