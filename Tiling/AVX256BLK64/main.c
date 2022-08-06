#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void tiled_gemm(float *, size_t,
                float *, size_t,
                float *, size_t,
                int, int, int);

int main(int argc, char const *argv[])
{
    int size = argc >= 2 ? atoi(argv[1]) : 2048;
    int ld = size + 32;
    float *a = malloc(sizeof(float) * size * ld);
    float *b = malloc(sizeof(float) * size * ld);
    float *c = malloc(sizeof(float) * size * ld);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    tiled_gemm(a, ld, b, ld, c, ld, size, size, size);
    gettimeofday(&end, NULL);
    double time = (((double)end.tv_sec + end.tv_usec * 1e-6) - ((double)start.tv_sec + start.tv_usec * 1e-6)) * 1e3;
    printf("%.3f ms\n", time);
    return 0;
}
