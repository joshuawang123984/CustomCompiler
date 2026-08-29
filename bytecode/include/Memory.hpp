#include <cstddef>

class GarbageCollector
{
public:
    void *reallocate(void *pointer, size_t oldSize, size_t newSize);
    void collect();

private:
};