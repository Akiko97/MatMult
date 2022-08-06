#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define LOOP_COUNT 1000

void add(float*, float*, float*, int);
void vadd(float*, float*, float*, int);

int main(int argc, char const *argv[])
{
    size_t size = argc == 2 ? atoi(argv[1]) : 2048;
    if (size % 8)
        return -1;
    __attribute__((aligned((256/8)))) float a[size], b[size], c[size];
    for (size_t i = 0; i < size; i++) {
        a[i] = (float)i;
        b[i] = (float)i;
    }
    // free <=> posix_memalign instead malloc
    // munlock <=> mlock( A,  4096 ); fix to physical page <= virtual page

    struct timeval start, end;
    double time;
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < LOOP_COUNT; i++)
        add(a, b, c, size);
    gettimeofday(&end, NULL);
    time = (((double)end.tv_sec + end.tv_usec * 1e-6) - ((double)start.tv_sec + start.tv_usec * 1e-6)) * 1e3;
    printf("%.6f ms\n", time);
    memset(c, 0, sizeof(*c * size));
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < LOOP_COUNT; i++)
        vadd(a, b, c, size);
    gettimeofday(&end, NULL);
    time = (((double)end.tv_sec + end.tv_usec * 1e-6) - ((double)start.tv_sec + start.tv_usec * 1e-6)) * 1e3;
    printf("%.6f ms\n", time);
    return 0;
}
