#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

void matmul1(float const *__restrict__ a,
             float const *__restrict__ b,
             float       *__restrict__ c, int size)
{
    for (int i = 0; i < size; ++i)
        for (int k = 0; k < size; ++k) {
            float t = a[i * size + k];
            for (int j = 0; j < size; ++j)
                c[i * size + j] += t * b[k * size + j];
        }
}

struct mmarg {
    float *__restrict__ a, *__restrict__ b, *__restrict__ c;
    int size;
    int start;
    int end;
};

void *matmuln(void *__restrict__ arg)
{
    struct mmarg *mmarg = arg;
    float *__restrict__ a = mmarg->a;
    float *__restrict__ b = mmarg->b;
    float *__restrict__ c = mmarg->c;
    int size = mmarg->size;
    int start = mmarg->start;
    int end = mmarg->end;
    for (int i = start; i < end; ++i)
        for (int k = 0; k < size; ++k) {
            float t = a[i * size + k];
            for (int j = 0; j < size; ++j)
                c[i * size + j] += t * b[k * size + j];
        }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    float *a, *b, *c;
    int size = argc == 3 ? atoi(argv[1]) : 128;
    int tn = argc == 3 ? atoi(argv[2]) : 1;
    int sizeb = size * size;
    struct timeval start, end;
    a = malloc(sizeof(float) * sizeb);
    b = malloc(sizeof(float) * sizeb);
    c = malloc(sizeof(float) * sizeb);
    for (int i = 0; i < sizeb; ++i) {
        a[i] = 1.0F;
        b[i] = 1.0F;
    }
    memset(c, 0, sizeof(float) * sizeb);
    if (tn == 1) {
        gettimeofday(&start, NULL);
        matmul1(a, b, c, size);
        gettimeofday(&end, NULL);
    }
    else {
        pthread_t threads[tn];
        struct mmarg args[tn];
        int block_size = size / tn;
        for (int i = 0; i < tn; ++i) {
            args[i].a = a;
            args[i].b = b;
            args[i].c = c;
            args[i].size = size;
            args[i].start = i * block_size;
            args[i].end = (i + 1) * block_size > size ? size : (i + 1) * block_size;
        }
        gettimeofday(&start, NULL);
        for (int i = 0; i < tn; ++i)
            pthread_create(&threads[i], NULL, matmuln, (void*)&args[i]);
        for (int i = 0; i < tn; i++)
            pthread_join(threads[i], NULL);
        gettimeofday(&end, NULL);
    }
    double time = (((double)end.tv_sec + end.tv_usec * 1e-6) - ((double)start.tv_sec + start.tv_usec * 1e-6)) * 1e3;
    printf("%.3f ms\n", time);
    return 0;
}
