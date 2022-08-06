// https://www.cs.cmu.edu/~213/schedule.html
// http://csapp.cs.cmu.edu/3e/code.html

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define THREADS_MAKE_MAT 4
#define FLAG_RESET 0
#define FLAG_RAND 1

struct matrix {
    int *matrix;
    int size;
};

struct mmt_t {
    struct matrix *m;
    int start;
    int rows;
    int flag;
};

void *make_matrix_thread(void *mmt_arg) {
    struct mmt_t *mmt = mmt_arg;
    for (int i = mmt->start; i < mmt->start + mmt->rows; i++)
        for (int j = 0; j < mmt->m->size; j++)
            if (mmt->flag == FLAG_RAND)
                *(mmt->m->matrix + i * mmt->m->size + j) = rand() % 100; // NOLINT(cert-msc30-c, cert-msc50-cpp)
            else if (mmt->flag == FLAG_RESET)
                *(mmt->m->matrix + i * mmt->m->size + j) = 0;
    pthread_exit(NULL);
}

void make_matrix(struct matrix *m, int flag) {
    m->matrix = malloc(sizeof(int) * m->size * m->size);
    int threads_num;
    if (m->size > THREADS_MAKE_MAT)
        threads_num = THREADS_MAKE_MAT;
    else
        threads_num = m->size;
    pthread_t threads[threads_num];
    struct mmt_t mmts[threads_num];
    srand(*((unsigned int*)&m)); // NOLINT(cert-msc32-c, cert-msc51-cpp)
    for (int i = 0; i < threads_num; i++) {
        mmts[i].m = m;
        mmts[i].start = (m->size / threads_num) * i;
        mmts[i].rows = (i == threads_num - 1) ? m->size - mmts[i].start : m->size / threads_num;
        mmts[i].flag = flag;
        pthread_create(&threads[i], NULL, make_matrix_thread, (mmts + i));
    }
    for (int i = 0; i < threads_num; i++)
        pthread_join(threads[i], NULL);
}

void destroy_matrix(struct matrix *m) {
    if (m->matrix) {
        free(m->matrix);
        m->matrix = NULL;
    }
}

__attribute__((unused)) void show_matrix(struct matrix *m) {
    for (int i = 0; i < m->size; i++) {
        for (int j = 0; j < m->size; j++)
            printf("%d ", *(m->matrix + i * m->size + j));
        putchar('\n');
    }
    putchar('\n');
}

struct mmul_t {
    struct matrix *a, *b, *r;
    unsigned int count;
    int start_row, start_column;
};

void *mat_mul_thread(void *mmul_arg) {
    struct mmul_t *mmul = mmul_arg;
    int size = mmul->a->size;
    int k = mmul->start_column;
    for (int i = mmul->start_row; i < size; i++) {
        for (; k < size; k++) {
            int r = *(mmul->a->matrix + i * mmul->a->size + k);
            for (int j = 0; j < size; j++)
                *(mmul->r->matrix + i * mmul->r->size + j) += r * *(mmul->b->matrix + k * mmul->b->size + j);
            if (!(--(mmul->count)))
                pthread_exit(NULL);
        }
        k = 0;
    }
    pthread_exit(NULL);
}

void mat_mul(struct matrix *a, struct matrix *b, struct matrix *r, int threads_num) {
    int size = a->size;
    if (threads_num == 1) {
        for (int k = 0; k < size; k++)
            for (int i = 0; i < size; i++) {
                int t = *(a->matrix + i * a->size + k);
                for (int j = 0; j < size; j++)
                    *(r->matrix + i * r->size + j) += t * *(b->matrix + k * b->size + j);
            }
    }
    else {
        int block_size = (size * size) / threads_num;
        pthread_t threads[threads_num];
        struct mmul_t mmul[threads_num];
        for (int i = 0; i < threads_num; i++) {
            mmul[i].a = a;
            mmul[i].b = b;
            mmul[i].r = r;
            mmul[i].count = i == threads_num - 1 ? (size * size - block_size * i) : block_size;
            mmul[i].start_row = block_size * i / size;
            mmul[i].start_column = block_size * i % size;
            pthread_create(&threads[i], NULL, mat_mul_thread, (mmul + i));
        }
        for (int i = 0; i < threads_num; i++)
            pthread_join(threads[i], NULL);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <size> <number of threads>\n", argv[0]);
        return -1;
    }
    struct timeval start, end;
    long time_usec;
    struct matrix a, b, r;
    int size = (int)strtol(argv[1], NULL, 10);
    int threads_num = (int)strtol(argv[2], NULL, 10);
    if (!threads_num || !size) {
        printf("Error input.\n");
        return -1;
    }
    a.size = size;
    b.size = size;
    r.size = size;
    make_matrix(&a, FLAG_RAND);
    make_matrix(&b, FLAG_RAND);
    make_matrix(&r, FLAG_RESET);
    // show_matrix(&a);
    // show_matrix(&b);
    gettimeofday(&start, NULL);
    mat_mul(&a, &b, &r, threads_num);
    gettimeofday(&end, NULL);
    // show_matrix(&r);
    time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    double time_sec = ((double)time_usec / 1000000);
    printf("%f\n", time_sec);
    destroy_matrix(&a);
    destroy_matrix(&b);
    destroy_matrix(&r);
    return 0;
}