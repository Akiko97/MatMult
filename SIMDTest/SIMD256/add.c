#include <stddef.h>

void add(float const *__restrict__ a,
         float const *__restrict__ b,
         float       *__restrict__ c, int size)
{
    for (size_t i = 0; i < size; i++)
        c[i] = a[i] + b[i];
}
