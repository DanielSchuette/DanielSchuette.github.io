#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRLEN 256

typedef struct foo {
    int i;
    char *s;
} foo;

void copy(foo *f1, foo *f2)
{
    *f2 = *f1;
}

int main(void)
{
    /*
     * If foo.s is not malloced but a const char *,
     * the copied value in f2 does not change after
     * the strncpy call because the *ptr* in f1 is
     * altered. After mallocing a pointer, whatever
     * mem is *pointed to* is changed, so f1 *and* f2
     * both change.
     */
    foo f1, f2;

    f1.i = 3;
    f1.s = (char *)malloc(sizeof(char)*STRLEN);
    copy(&f1, &f2);

    printf("i: %d, s: %s\n", f2.i, f2.s);
    strncpy(f1.s, "another string pointer", STRLEN-1);
    printf("i: %d, s: %s\n", f1.i, f1.s);
    printf("i: %d, s: %s\n", f2.i, f2.s);

    free(f1.s);
    return 0;
}
