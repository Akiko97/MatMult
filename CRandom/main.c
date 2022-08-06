#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

int random_dev(int s, int e) {
    int n;
    int fd= open("/dev/random", O_RDONLY);
    read(fd, &n, sizeof(int));
    if (n < 0)
        n = -n;
    n = n % e;
    if (n < s)
        n = n + s;
    return n;
}

int main() {
    int n[100];
    for (int i = 0; i < 100; i++)
        n[i] = random_dev(50, 100);
    for (int i = 0; i < 100; i++)
        printf("%d ", n[i]);
    putchar('\n');
    srandom(time(NULL));
    long x = random() % 100;
    printf("%ld\n", x);
    return 0;
}
