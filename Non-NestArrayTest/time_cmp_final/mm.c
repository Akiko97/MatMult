#include "mm.h"

void mm(const int *__restrict__ a,
        const int *__restrict__ b,
        int *__restrict__ c, const int size)
{
    for (int i = 0; i < size; i++)
        for (int k = 0; k < size; k++) {
            int r = a[i * size + k];
            for (int j = 0; j < size; j++)
                c[i * size + j] += r * b[k * size + j];
        }
}