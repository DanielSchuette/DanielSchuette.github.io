#include <assert.h>
#include <stdio.h>
#include <stddef.h>

#define typeof __typeof__
#define my_offsetof(type, member) ({        \
        (size_t)&((type *)0)->member;       \
        })

/* need char * cast for _ptr, otherwise we're 24 bytes off */
#define container_of(ptr, type, member) ({          \
        typeof(((type *)0)->member) *_ptr = (ptr);  \
        ((type *)(_ptr - offsetof(type, member)));  \
        })

struct foo {
    char c;
    int i;
    float f;
};

int main(void)
{
    struct foo instance = { .c = 'a', .i = 42, .f = .5f };
    struct foo *ptr = &instance;
    struct foo *my_container;
    my_container = container_of(&(ptr->f), struct foo, f);
    fprintf(stderr, "container_of: %p, original: %p, size: %lu, offset: %lu, f-ptr: %p\n",
            my_container, ptr, sizeof(struct foo),
            offsetof(struct foo, f), &(ptr->f));
    fprintf(stderr, "char *:%lu, struct *:%lu\n",
            sizeof(char *), sizeof(struct foo *));

    fprintf(stderr, "stddef: %lu\n", offsetof(struct foo, f));
    fprintf(stderr, "my: %lu\n", my_offsetof(struct foo, f));
}
