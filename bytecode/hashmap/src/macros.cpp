#include "../include/macros.hpp"

template <typename T>
void FREE_ARRAY(T *&entries, int &capacity)
{
    if (entries == nullptr)
        // maybe throw an error or smth in the future
        return;

    delete[] entries;
    entries = nullptr;
    capacity = 0;
}

template <typename T>
T *ALLOCATE(int capacity)
{
    if (capacity == 0)
        return nullptr;

    T *array = new T[capacity];

    return array;
}

int GROW_CAPACITY(int &capacity)
{
    return capacity < 8 ? 8 : capacity * 2;
}