#include <cstddef>

#define DEBUG_STRESS_GC

class GarbageCollector
{
public:
    void setMarkRootsCallback(std::function<void()> fn);
    void *reallocate(void *pointer, size_t oldSize, size_t newSize);
    void mark(Value value);
    void mark(Obj *object);
    void traceReferences();
    void sweep();

    void collect();

private:
    std::function<void()> markRootsFn;
    size_t threshold = 512 * 512;
    size_t total_bytes = 0;
};