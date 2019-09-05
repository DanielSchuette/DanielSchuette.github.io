---
layout: post
title:  "Generating Patches Using `diff(1)`"
date:   2019-09-05 21:58:00 +0200
categories: unix
---

On Unix systems, patches generated with `diff(1)` provide a convenient method to update source files without having to modify any code manually. Some open source software projects like *[suckless](https://suckless.org/)* actually rely on patching as a mechanism for adding features to programs. Also, patches can be used to easily share code between programmers during software development. In this short post, we will explore the basics of generating patches with `diff(1)` and applicating patches with `patch(1)`.

# How to Generate a Patch
Imaging the following two *C* source files:

```c
/* main.c */
#include <stdio.h>

int main(void)
{
    printf("hello user!\n");
    return 0;
}

/* main_with_feature.c */
#include <stdio.h>

int main(int argc, char **argv)
{
    int i;

    for (i = 0; i < argc; i++)
        printf("%s\n", *(argv++));
    return 0;
}
```

In `main_with_feature.c`, the programmer added a highly important feature that improves the usability of his company's software by *a lot*. Thus, users that already use `main.c` require a convenient way to update their sources[^1]. The first step to achieve this is creation of a *patch* that we can distribute to users:

```bash
diff -u main.c main_with_feature.c > important_feature.patch
```

`diff(1)` with the `-u` flag prints the differences between the files provided as arguments in a format that can be used with `patch(1)`. By default, the output is printed to `stdout`, but we redirect it to `important_feature.patch` and send it to all our users.

# Applying a Patch to a File
Now, users can apply the patch with a single command:

```bash
patch < important_feature.patch
```

Since `important_feature.patch` contains not only the differences between the files but also their names, the patch command is able to find `main.c` and apply all the necessary changes to it. Running the following command will no longer report any differences between the files:

```bash
diff main.c main_with_feature.c
```

If a user does not want to use the new feature, it is just as easy to revert `main.c` back to the pre-patched version:

```bash
patch -R < important_feature.patch
```

# Summary
...

<hr class="hr-light">

[^1]: Assuming that users of our software compile their binaries from source. Patches aren't used for doing updates to binaries. That's what package managers are for.
