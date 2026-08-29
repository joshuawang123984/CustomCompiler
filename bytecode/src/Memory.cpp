#include "../include/Memory.hpp"
#include "../include/Helper/types.hpp"
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
}
void GarbageCollector::mark(Obj *object)
{
}
void GarbageCollector::traceReferences()
{
}
void GarbageCollector::sweep()
{
}
void GarbageCollector::collect()
{
    std::cout << "-- starting gc --" << std::endl;
    std::cout << "-- ending gc --" << std::endl;
}