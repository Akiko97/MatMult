#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void matmul(const int *__restrict__ a,
            const int *__restrict__ b,
            int *__restrict__ c, int msize)
{
  for (int k = 0; k < msize; k++)
    for (int i = 0; i < msize; i++) {
      int t = a[i * msize + k];
      for (int j = 0; j < msize; j++)
        c[i * msize + j] += t * b[k * msize + j];
    }
}

int main(int argc, char *argv[])
{
    int msize = argc >= 2 ? atoi(argv[1]) : 128;
    int *a = malloc(sizeof(int) * msize * msize);
    int *b = malloc(sizeof(int) * msize * msize);
    int *r = malloc(sizeof(int) * msize * msize);
    for (int i = 0; i < msize * msize; i++) {
        a[i] = 1;
        b[i] = 1;
        r[i] = 0;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < 10; i++)
        matmul(a, b, r, msize);
    gettimeofday(&end, NULL);
    long time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    double time_sec = ((double)time_usec / 1000000);
    printf("%.3f\n", time_sec);
    free(a);
    free(b);
    free(r);
    return 0;
}
