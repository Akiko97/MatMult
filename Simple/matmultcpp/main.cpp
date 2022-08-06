#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <sys/time.h>

typedef std::vector<std::vector<int>> matrix;

void matgen_thread(matrix &m, int start, int rows, std::default_random_engine &e, std::uniform_int_distribution<int> &uniform_dist) {
    unsigned int size = m.size();
    for (int i = start; i < start + rows; ++i)
        for (int j = 0; j < size; ++j)
            m[i][j] = uniform_dist(e);
}

void matgen(matrix &m) {
    unsigned int thread_num = std::thread::hardware_concurrency();
    unsigned int size = m.size();
    if (thread_num > size)
        thread_num = size;
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> uniform_dist(0, 100);
    std::vector<std::thread> threads(thread_num);
    for (int t = 0; t < thread_num; ++t) {
        unsigned int start = (size / thread_num) * t;
        unsigned int rows = (t == thread_num - 1) ? size - start : size / thread_num;
        threads[t] = std::thread(matgen_thread, std::ref(m), (int)start, (int)rows, std::ref(e), std::ref(uniform_dist));
    }
    for (std::thread &t : threads)
        t.join();
}

void matmult_thread(matrix &a, matrix &b, matrix &r, int count, int start_row, int start_column) {
    unsigned int size = a.size();
    int k = start_column;
    for (int i = start_row; i < size; ++i) {
        for (; k < size; ++k) {
            int t = a[i][k];
            for (int j = 0; j < size; ++j)
                r[i][j] += t * b[k][j];
            if (!(--(count)))
                return;
        }
        k = 0;
    }
}

void matmult(matrix &a, matrix &b, matrix &r, int thread_num) {
    unsigned int size = a.size();
    if (thread_num == 1) {
        for (int k = 0; k < size; ++k)
            for (int i = 0; i < size; ++i) {
                int t = a[i][k];
                for (int j = 0; j < size; ++j)
                    r[i][j] += t * b[k][j];
            }
    }
    else {
        unsigned int block_size = (size * size) / thread_num;
        std::vector<std::thread> threads(thread_num);
        for (int t = 0; t < thread_num; ++t) {
            unsigned int count = t == thread_num - 1 ? (size * size - block_size * t) : block_size;
            unsigned int start_row = block_size * t / size;
            unsigned int start_column = block_size * t % size;
            threads[t] = std::thread(
                    matmult_thread, std::ref(a), std::ref(b), std::ref(r), count, start_row, start_column);
        }
        for (std::thread &t : threads)
            t.join();
    }
}

[[maybe_unused]] void printmat(matrix &m) {
    for (int i = 0; i < m.size(); ++i) {
        for (int j = 0; j < m.size(); ++j)
            std::cout << m[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <size> <threads>." << std::endl;
        return -1;
    }
    int size = std::stoi(argv[1]);
    int thread_num = std::stoi(argv[2]);
    timeval start{}, end{};
    long time_usec;
    matrix a(size, std::vector<int>(size, 0));
    matrix b(size, std::vector<int>(size, 0));
    matrix r(size, std::vector<int>(size, 0));
    matgen(a);
    matgen(b);
    // printmat(a);
    // printmat(b);
    gettimeofday(&start, nullptr);
    matmult(a, b, r, thread_num);
    gettimeofday(&end, nullptr);
    // printmat(r);
    time_usec = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    double time_sec = ((double)time_usec / 1000000);
    std::cout << time_sec << std::endl;
    return 0;
}
