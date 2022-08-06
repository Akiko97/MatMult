#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

struct matrix {
    int **matrix;
    int size;
};

void matinit(struct matrix *m, int size) {
    m->size = size;
    m->matrix = malloc(sizeof(int*) * size);
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        m->matrix[i] = malloc(sizeof(int) * size);
        // #pragma omp parallel for
        for (int j = 0; j < size; j++) {
            m->matrix[i][j] = 0;
        }
    }
}

void matgen(struct matrix *m) {
    int size = m->size;
    srandom(*((unsigned int*)&m));
    // omp_set_nested(1);
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        // #pragma omp parallel for
        for (int j = 0; j < size; j++) {
            m->matrix[i][j] = (int)random() % 100;
        }
    }
}

__attribute__((unused)) void matprt(struct matrix *m) {
    int size = m->size;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            printf("%d ", m->matrix[i][j]);
        putchar('\n');
    }
    putchar('\n');
}

void matmul(struct matrix *a, struct matrix *b, struct matrix *r) {
    int size = a->size;
    #pragma omp parallel for
    for (int k = 0; k < size; k++) {
        // #pragma omp parallel for
        for (int i = 0; i < size; i++) {
            int t = a->matrix[i][k];
            // #pragma omp parallel for
            for (int j = 0; j < size; j++) {
                r->matrix[i][j] += t * b->matrix[k][j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <size> <number of threads>\n", argv[0]);
        return -1;
    }
    int size = (int)strtol(argv[1], NULL, 10);
    int threads_num = (int)strtol(argv[2], NULL, 10);
    if (!threads_num || !size) {
        printf("Error input.\n");
        return -1;
    }
    omp_set_num_threads(threads_num);
    struct matrix a, b, r;
    double start, end;
    matinit(&a, size);
    matinit(&b, size);
    matinit(&r, size);
    matgen(&a);
    matgen(&b);
    start = omp_get_wtime();
    matmul(&a, &b, &r);
    end = omp_get_wtime();
    // matprt(&a);
    // matprt(&b);
    // matprt(&r);
    printf("%f\n", end - start);
    return 0;
}
