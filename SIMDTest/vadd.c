#include <stddef.h>
#include <immintrin.h>

#define FLOAT_PER_256BIT (256 / 32)

void vadd(float const *__restrict__ a,
          float const *__restrict__ b,
          float       *__restrict__ c, int size)
{
    // down clock when avx instruction issued.
    // example
    //  base 3.0GHz (non avx instruction)
    //   avx 2.8GHz?
    // avx instrunction accererates computation. 
    // if computation is bottlneck, application will be faster.
    // メモリアクセス時間がボトルネックの時は、演算を速くしても、処理時間は短くならない。avxはdownclockする代わりに演算を速くするもの
    // つまり、演算がボトルネックじゃない時に、avxを使うと、downclockした分だけ、遅くなってしまう・・・
    size_t block = size / FLOAT_PER_256BIT;
    __m256 va, vb, vc;
    for (size_t i = 0; i < block; i++) {
        va = _mm256_load_ps(&a[i * FLOAT_PER_256BIT]);
        vb = _mm256_load_ps(&b[i * FLOAT_PER_256BIT]);
        vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i * FLOAT_PER_256BIT], vc);
    }
}
