#include "gmm.h"

void g_mm()
{
    for (int i = 0; i < g_size; i++)
        for (int k = 0; k < g_size; k++) {
            int r = g_a[i * g_size + k];
            for (int j = 0; j < g_size; j++)
                g_c[i * g_size + j] += r * g_b[k * g_size + j];
        }
}