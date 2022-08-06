#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define BLOCK_SIZE 64
#define THREADS 1

void tiling_gemm(const float *__restrict__ a,
                 const float *__restrict__ b,
                 float *__restrict__ c, int size)
{
    // clean c
    for (int i = 0; i < size; i++)
	    for (int j = 0; j < size; j++)
	        c[i * size + j] = 0.0;
    // gemm
    // split into submatrix
#pragma omp parallel for
    for (int k_out = 0; k_out < size; k_out += BLOCK_SIZE)
	    for (int j_out = 0; j_out < size; j_out += BLOCK_SIZE)
            // for submatrix do ikj
	        for (int i = 0; i < size; i++)
		        for (int k = k_out; k < k_out + BLOCK_SIZE; k++) {
		            float r = a[i * size + k];
		            for (int j = j_out; j < j_out + BLOCK_SIZE; j++)
			            c[i * size +j] += r * b[k * size +j];
		        }
}

int main(int argc, char const *argv[])
{
    int size = argc >= 2 ? atoi(argv[1]) : 2048;
    float *a = malloc(sizeof(float) * size * size);
    float *b = malloc(sizeof(float) * size * size);
    float *c = malloc(sizeof(float) * size * size);
    struct timeval start, end;
    omp_set_num_threads(THREADS);
    gettimeofday(&start, NULL);
    tiling_gemm(a, b, c, size);
    gettimeofday(&end, NULL);
    double time = (((double)end.tv_sec + end.tv_usec * 1e-6) - ((double)start.tv_sec + start.tv_usec * 1e-6)) * 1e3;
    printf("%.3f ms\n", time);
    return 0;
}
