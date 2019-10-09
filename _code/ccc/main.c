#include <stdio.h>

extern long asm_add(long, long, long, long, long,
                   long, long, long, long, long);

int main(void)
{
    long a = 5, b = 2;

    printf("%lu + %lu = %lu\n", a, b,
           asm_add(a, b, a, b, a, b, a, b, a, b));

    return 0;
}
