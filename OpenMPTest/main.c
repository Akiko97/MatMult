#include <stdio.h>
#include <omp.h>

int main() {
    int tn = omp_get_max_threads();
    printf("Threads: %d\n\n", tn);
    omp_set_num_threads(tn);
#pragma omp parallel default(none)
    {
        printf("%d: Hello, World!\n", omp_get_thread_num());
    }
    return 0;
}
