---
layout: post
title:  "Explaining container_of and offsetof"
date:   2019-07-28 14:37:00 +0200
categories: c
---
In this post, I'll discuss two closely related C macros: `offsetof` and `container_of`. Whereas the first is part of the ANSI C header `stddef.h`, a definition of the second one can be found in many, many C codebases (for example the Linux kernel!). Understanding their implementation requires some knowledge about pointer arithmetic, but I'll try to explain everything in the most simple way possible[^1].

# offsetof
`offsetof` is a [function-like macro](https://gcc.gnu.org/onlinedocs/cpp/Function-like-Macros.html#Function-like-Macros) which is used as follows:

```c
#include <stddef.h>

struct foo {
    int i;
    char c;
    float f;
};

int main(void)
{
    size_t offset;

    offset = offsetof(foo, f);
    printf("offset of f into foo: %lu\n", offset);

    return 0;
}
```

Now, `size_t offset` holds the byte offset of member `float f` within `struct foo`. By default[^2], compilers align structure members to *natural* boundaries, depending on the platform you're working on. This behavior can be changed using `__attribute__((packed))` and we could try do manually calculate the offset of a certain member of `foo`. But because data types can have different sizes, too, this gets difficult and error-prone really quickly if portability is a concern. Thus, `offsetof` should be used. But how does it work?

```c
#define my_offsetof(type, memb) ({                        \
        (size_t)((char *)&((type *)0)->memb - (char *)0;  \
        })
```
First, let me say that if you don't use `gcc`, your compiler might not support [statement expressions](https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html). It should be an easy task to rewrite things in a way that conforms with your compiler, though.

The following operations are done by `offsetof` to get to the result:

- it creates a `type *` `NULL` pointer (i.e. a `NULL` pointer to the `struct` in question: `((type *)0)`)

- it accesses its member (`((type *)0)->memb`)

- it takes the address of that member (`&((type *)0)->memb`), which is now `NULL`-pointer-address + `offset`

- it subtracts the memory address of the `NULL` pointer[^3] (`(char *)&((type *)0)->memb - (char *)0`), yielding just `offset`

- finally, the result is cast into a `size_t`, which should be used when representing memory-related quantities (since it is guaranteed to be able to hold the size of any object and `offset` cannot be larger then the size of `struct foo` for obvious reasons)

All these steps involve constant expression, so the compiler will calculate `offset` at compile time and not at runtime. This is actually a good thing, because otherwise [undefined behavior might occur](https://stackoverflow.com/questions/26906621/does-struct-name-null-b-cause-undefined-behaviour-in-c11).

# container_of
The second macro to discuss, `container_of`, is defined this way:

```c
#define container_of(ptr, type, member) ({          \
        typeof(((type *)0)->member) *_ptr = (ptr);  \
        ((type *)(_ptr - offsetof(type, member)));  \
        })
```

It can be used with the address of a `struct` member `ptr` to find the `struct` that contains it. It can be used as follows:

```c
/* #define and #include's omitted for simplicity */
struct foo {
    int i;
    char c;
    float f;
};

int main()
{
    float *f_ptr;
    struct foo parent, *_ptr, *ptr;

    parent = { .c = 'a', .i = 42, .f = .5f };
    _ptr = &instance;                              /* assume this address is unknown */
    f_ptr = &(_ptr->f);                            /* and assume only this ptr is known */

    ptr = container_of(f_ptr, struct foo, f);
    printf("%p and %p are the same\n", _ptr, ptr);

    return 0;
}
```

Especially in program that contain many nested `struct`s, it is often useful to find the address of `struct foo` based on the address of one of its members. At the first glance, `container_of` looks quite cryptic. But with some explanations, it is straight forward because we already have `offsetof` at our disposal.

The arguments to `container_of` are a pointer to the member for which we want to find the containing `struct`, the type of the containing `struct` and the name of the member in that `struct` (as shown in the example). Again, things will be broken down into steps. The macro:

- one the second line, creates a temporary pointer `_ptr` to the same address as the argument pointer `ptr` because we need to change the address it points to and don't want to screw with the user's `ptr` to do so[^4]

- one the third line, casts `_ptr` to `char *` to force the compiler to do byte-wise arithmetic (remember that `offsetof` returned a byte offset)

- then, subtracts the offset of `member` within `type` from the address stored in `_ptr` to arrive at the base address of the `struct` containing `ptr`

- last, the result another cast is done to return the correct type (that is, a pointer to the containing `struct` or `type *`)

# Conclusion
Pointers are a notoriously difficult topic to understand, for beginners as well as advanced users of the C language. Thus, reproducing and understanding the implementation of macros like `container_of` and `offsetof` is a great way to consolidate knowledge while practicing abstract thinking (what points where again?). Hopefully, the explanations in this post were easy enough to follow. If you find any mistakes or have suggestions for improvements, comment below or on twitter!

---

[^1]: I found these macros pretty difficult to fully understand at first. I had to test different versions and leave out certain pieces to approach complete understanding. I highly encourage you to do the same if I don't explain things well enough in this post!
[^2]: Are there compilers that don't do this?
[^3]: Some implementations of `offsetof` rely on the `NULL` pointer being at memory address 0. Since that is not part of the C specification, it shouldn't be done. The `char *` casts are required because pointer arithmetic must be done on pointers of the same type and we want to calculate `offset` in bytes!
[^4]: The construct `typeof(((type *)0)->member)` creates a `NULL` pointer again. This is done so that we can access use `typeof` to find the type of `member` which is needed to create a correct temporary pointer `_ptr`.
