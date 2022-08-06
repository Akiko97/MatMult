#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define THREADS_MATGEN 32

int msize = 0;
int **a = NULL, **b = NULL, **r = NULL;
int tnum = 1;

typedef struct mgt_t {
    int start;
    int rows;
} mgt_t;

void *matgen_t(void *_mgt) {
    mgt_t *mgt = _mgt;
    for (int i = mgt->start; i < mgt->start + mgt->rows; i++)
        for (int j = 0; j < msize; j++) {
            a[i][j] = (int)random() % 100;
            b[i][j] = (int)random() % 100;
            r[i][j] = 0;
        }
    pthread_exit(NULL);
}

void matgen() {
    a = malloc(sizeof(int *) * msize);
    for (int i = 0; i < msize; i++)
        a[i] = malloc(sizeof(int) * msize);
    b = malloc(sizeof(int *) * msize);
    for (int i = 0; i < msize; i++)
        b[i] = malloc(sizeof(int) * msize);
    r = malloc(sizeof(int *) * msize);
    for (int i = 0; i < msize; i++)
        r[i] = malloc(sizeof(int) * msize);
    int tnumgen = THREADS_MATGEN;
    if (msize < THREADS_MATGEN)
        tnumgen = msize;
    pthread_t threads[tnumgen];
    srandom(*((unsigned int*)&r));
    mgt_t mgt[tnumgen];
    for (int i = 0; i < tnumgen; i++) {
        mgt[i].start = (msize / tnumgen) * i;
        mgt[i].rows = (i == tnumgen - 1) ? msize - mgt[i].start : msize / tnumgen;
        pthread_create(&threads[i], NULL, matgen_t, (void *)&mgt[i]);
    }
    for (int i = 0; i < tnumgen; i++)
        pthread_join(threads[i], NULL);
}

__attribute__((unused)) void matprt() {
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++)
            printf("%d ", a[i][j]);
        putchar('\n');
    }
    putchar('\n');
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++)
            printf("%d ", b[i][j]);
        putchar('\n');
    }
    putchar('\n');
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++)
            printf("%d ", r[i][j]);
        putchar('\n');
    }
    putchar('\n');
}

typedef struct mmt_t {
    unsigned int count;
    int start_row, start_column;
} mmt_t;

void *matmul_t(void *_mmt) {
    mmt_t *mmt = _mmt;
    int k = mmt->start_column;
    for (int i = mmt->start_row; i < msize; i++) {
        for (; k < msize; k++) {
            int t = a[i][k];
            for (int j = 0; j < msize; j++)
                r[i][j] += t * b[k][j];
            if (!(--(mmt->count)))
                pthread_exit(NULL);
        }
        k = 0;
    }
    pthread_exit(NULL);
}

void matmul() {
    if (tnum == 1) {
        for (int k = 0; k < msize; k++)
            for (int i = 0; i < msize; i++) {
                int t = a[i][k];
                for (int j = 0; j < msize; j++)
                    r[i][j] += t * b[k][j];
            }
    }
    else {
        int block_size = (msize * msize) / tnum;
        pthread_t threads[tnum];
        mmt_t mmt[tnum];
        for (int i = 0; i < tnum; i++) {
            mmt[i].count = i == tnum - 1 ? (msize * msize - block_size * i) : block_size;
            mmt[i].start_row = block_size * i / msize;
            mmt[i].start_column = block_size * i % msize;
            pthread_create(&threads[i], NULL, matmul_t, (mmt + i));
        }
        for (int i = 0; i < tnum; i++)
            pthread_join(threads[i], NULL);
    }
}

int main(__attribute__((unused)) int argc, char* argv[]) {
    msize = (int)strtol(argv[1], NULL, 10);
    tnum = (int)strtol(argv[2], NULL, 10);
    matgen();
    struct timeval start, end;
    gettimeofday(&start, NULL);
    matmul();
    gettimeofday(&end, NULL);
    // matprt();
    long time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    double time_sec = ((double)time_usec / 1000000);
    printf("%f\n", time_sec);
    for (int i = 0; i < msize; i++) {
        free(a[i]);
        free(b[i]);
        free(r[i]);
    }
    free(a);
    free(b);
    free(r);
    return 0;
}
