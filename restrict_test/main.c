#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/* add/add_c:
 * | | | | | | | a
 *  + + + + + +
 * | | | | | | | b
 *  ↓ ↓ ↓ ↓ ↓ ↓
 * | | | | | | | c
 */

void add_c(const int *__restrict__ a, const int *__restrict__ b, int *__restrict__ c, unsigned int size) {
    for (int i = 0; i < size; ++i)
        c[i] = a[i] + b[i];
}

void add(const int *a, const int *b, int *c, unsigned int size) {
    for (int i = 0; i < size; ++i)
        c[i] = a[i] + b[i];
}

int main(__attribute__((unused)) int argc, char *argv[]) {
    struct timeval start, end;
    long time_usec;
    double time_sec;
    unsigned int size = (int)strtol(argv[1], NULL, 10);
    int *a = malloc(sizeof(int) * size);
    int *b = malloc(sizeof(int) * size);
    int *c = malloc(sizeof(int) * size);
    for (int i = 0; i < size; ++i) {
        a[i] = 1;
        b[i] = 1;
    }
    memset(c, 0, sizeof(int) * size);
    gettimeofday(&start, NULL);
    for (int i = 0; i < 1000000; ++i)
        add(a, b, c, size);
    gettimeofday(&end, NULL);
    time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    time_sec = ((double)time_usec / 1000000);
    printf("without restrict: %f\n", time_sec);
    for (int i = 0; i < size; ++i) {
        a[i] = 1;
        b[i] = 1;
    }
    memset(c, 0, sizeof(int) * size);
    gettimeofday(&start, NULL);
    for (int i = 0; i < 1000000; ++i)
        add_c(a, b, c, size);
    gettimeofday(&end, NULL);
    time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    time_sec = ((double)time_usec / 1000000);
    printf("with restrict: %f\n", time_sec);
    free(a);
    free(b);
    free(c);
    return 0;
}

/* memo:
 * restrict:
 * Ref to: Mac Memo
 */