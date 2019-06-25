---
layout: post
title:  "Hello World!"
date:   2019-06-25 06:55:15 +0200
categories: jekyll update
---
It's easy to print the string *hello world* onto the screen. All you need is a **C** compiler (which hopefully comes with your system, otherwise something's fundamentally wrong). Save the following to a file named `hello.c`:

```c
#include <stdio.h>

int main(void)
{
    printf("hello world\n");
    return 0;
}
```

You can use any text editor you like, although *vim* or some other terminal-based editor is highly recommended for productivity reasons - and because it looks much cooler (check out [my vim configs](https://github.com/DanielSchuette/dotfiles)).

Then, open a terminal (you should already have one open, really), compile your code and run it (yep, that's how easy it is):

```bash
gcc -o prog hello.c # the compiler might be clang or
                    # whatever C compiler Windo** comes with
./prog              # this runs the freshly compiled program
```

A neat, first little post for this website. The upcoming posts will hopefully be more interesting, though. Stay tuned!
