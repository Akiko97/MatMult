// compile option "-mavx -mavx2 -mfma"
#pragma GCC target ("avx,avx2,fma")
// compile option "-funroll-loops"
#pragma GCC optimize ("unroll-loops")

// https://stackoverflow.com/questions/63217621/pragma-optimize-vs-pragma-target-what-is-difference

#include <x86intrin.h>
#include <immintrin.h>
#include <string.h>
#include <omp.h>

#define NDEBUG
#include <assert.h>

#define BLK_M 64 // i
#define BLK_N 64 // j
#define BLK_K 64 // k

#define THREADS 16

#define USE_AVX

static __attribute__((always_inline)) inline
void blk_gemm(const float *__restrict__ a, size_t lda,
              const float *__restrict__ b, size_t ldb,
              float       *__restrict__ c, size_t ldc)
{
    for (int m = 0; m < BLK_M; m++, c += ldc) {
        for (int n = 0; n < BLK_N; n++) {
            const float *am = a + m * lda;
            const float *bn = b + n * ldb;
#pragma GCC ivdep
            for (int k = 0; k < BLK_K; k++) {
                c[k] += am[n] * b[k];
            }
        }
    }
}

static __attribute__((always_inline)) inline
void blk_gemm_avx(const float *__restrict__ a, size_t lda,
                  const float *__restrict__ b, size_t ldb,
                  float       *__restrict__ c, size_t ldc)
{
    for (int m = 0; m < BLK_M; m++, a += lda, c += ldc) {
        for (int n = 0; n < BLK_N; n++) {
            const float *bn = b + n * ldb;
            __m256 amn = _mm256_loadu_ps(a + n);
            for (int k = 0; k < BLK_K; k += 8) {
                __m256 bnk = _mm256_loadu_ps(bn + k);
                __m256 cmk = _mm256_loadu_ps(c + k);
                cmk = _mm256_fmadd_ps(amn, bnk, cmk);
                _mm256_storeu_ps(c + k, cmk);
            }
        }
    }
}

void tiled_gemm(const float *__restrict__ a, size_t lda,
                const float *__restrict__ b, size_t ldb,
                float       *__restrict__ c, size_t ldc,
                int M, int N, int K)
{
    assert(N % BLK_N == 0 && lda >= K);
    assert(M % BLK_M == 0 && ldb >= N);
    assert(K % BLK_K == 0 && ldc >= N);
    size_t fillsize = sizeof(float) * BLK_N;
    omp_set_num_threads(THREADS);
#pragma omp parallel for
    for (int mb = 0; mb < M; mb += BLK_M)
        for (int nb = 0; nb < N; nb += BLK_N) {
            // clear matrix c
            for (int m = 0; m < BLK_M; m++)
                memset(c + mb * ldc + nb, 0, fillsize);
            // tiled_gemm
            for (int kb = 0; kb < K; kb += BLK_K) {
#ifdef USE_AVX
                blk_gemm_avx(a + mb * lda +kb, lda,
                             b + kb * ldb + nb, ldb,
                             c + mb * ldc + nb, ldc);
#else
                blk_gemm(a + mb * lda +kb, lda,
                         b + kb * ldb + nb, ldb,
                         c + mb * ldc + nb, ldc);
#endif
            }
        }
}
