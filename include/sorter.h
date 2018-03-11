#include <algorithm>

#if !defined(SORTER_H_)
#define SORTER_H_

// simple sorter for arrays
template <typename T>
void array_sort(T * arr, size_t len)
{
    std::sort(arr, arr+len);
}

#endif
