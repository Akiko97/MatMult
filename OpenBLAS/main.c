#include <stdio.h>
#include <stdlib.h>
#include "cblas.h"
#include <sys/time.h>

#define THREADS_NUM 1

int main(int argc, char const *argv[])
{
    int size = argc >= 2 ? atoi(argv[1]) : 2048;
    int ld = size + 32;
    float *a = malloc(sizeof(float) * size * ld);
    float *b = malloc(sizeof(float) * size * ld);
    float *c = malloc(sizeof(float) * size * ld);
    openblas_set_num_threads(THREADS_NUM);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0, a, ld, b, ld, 1.0, c, ld);
    gettimeofday(&end, NULL);
    double time = (((double)end.tv_sec + end.tv_usec * 1e-6) - ((double)start.tv_sec + start.tv_usec * 1e-6)) * 1e3;
    printf("%.3f ms\n", time);
    return 0;
}
