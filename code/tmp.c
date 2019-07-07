#include <stdio.h>
#include <sys/types.h>

struct unioned {
    char *name;
    union {
        char u1;
        size_t u2;
    };
};

int main(void)
{
    struct unioned ued;
    ued.name = "0x01";
    ued.u1 = 0;         /* will be 2 */
    ued.u2 = 258;

    fprintf(stderr, "%s\n", ued.name);
    fprintf(stderr, "%d\n", ued.u1);
    fprintf(stderr, "%lu\n", ued.u2);

    return 0;
}
