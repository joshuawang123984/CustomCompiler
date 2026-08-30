#include <cstddef>
#include <functional>
#include <vector>

struct Obj;
struct Value;
struct Table;

// #define DEBUG_STRESS_GC

class GarbageCollector
{
public:
    template <typename T, typename... Args>
    T *allocateObject(Args &&...args)
    {
        T *object = new T(std::forward<Args>(args)...);
        object->next = objects;
        objects = object;
        return object;
    }

    void setMarkRootsCallback(std::function<void()> fn);
    void setTableRemoveWhite(std::function<void()> fn);

    void *reallocate(void *pointer, size_t oldSize, size_t newSize);
    void mark(Value value);
    void mark(Obj *object);

    void freeObjects();

    void collect();

private:
    std::function<void()> markRootsFn = nullptr;
    std::function<void()> tableRemoveWhiteFn = nullptr;

    size_t threshold = 512 * 512;
    size_t total_bytes = 0;

    std::vector<Obj *> grayStack;
    Obj *objects = nullptr;

    void blackenObject(Obj *object);
    void traceReferences();
    void sweep();
};