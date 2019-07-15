#include <stdio.h>

#define _cleanup_notify_ __attribute__((cleanup(notify)))
static inline void notify(int *d)
{
    fprintf(stderr, "%d went out of scope.\n", *d);
}

int main(void)
{
    _cleanup_notify_ int a = 5;
    _cleanup_notify_ int b = 8;

    fprintf(stderr, "using a (%d) and b (%d) here.\n", a, b);

    return 0;
}
