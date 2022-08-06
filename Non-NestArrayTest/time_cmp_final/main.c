#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "gmm.h"
#include "mm.h"

int g_size;
int *g_a, *g_b, *g_c;

int main(int argc, char *argv[])
{
    int size = argc >= 2 ? atoi(argv[1]) : 1000;
    g_size = size;
    int *a, *b, *c;
    struct timeval start, end;
    long time_usec;
    double time_sec;
    g_a = malloc(sizeof(int) * size * size);
    g_b = malloc(sizeof(int) * size * size);
    g_c = malloc(sizeof(int) * size * size);
    a = malloc(sizeof(int) * size * size);
    b = malloc(sizeof(int) * size * size);
    c = malloc(sizeof(int) * size * size);
    for (int i = 0; i < size * size; i++) {
        g_a[i] = 1;
        g_b[i] = 1;
        g_c[i] = 0;
        g_a[i] = 1;
        g_b[i] = 1;
        g_c[i] = 0;
    }
    gettimeofday(&start, NULL);
    for (int i = 0; i < 10; i++)
        g_mm();
    gettimeofday(&end, NULL);
    time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    time_sec = ((double)time_usec / 1000000);
    printf("global var: %.5f\n", time_sec);
    fflush(stdout);
    gettimeofday(&start, NULL);
    for (int i = 0; i < 10; i++)
        mm(a, b, c, size);
    gettimeofday(&end, NULL);
    time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    time_sec = ((double)time_usec / 1000000);
    printf("local var: %.5f\n", time_sec);
    fflush(stdout);
    free(g_a);
    free(g_b);
    free(g_c);
    free(a);
    free(b);
    free(c);
    return 0;
}
