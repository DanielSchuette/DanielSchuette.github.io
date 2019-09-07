---
layout: post
title:  "Generating Patches Using diff(1)"
date:   2019-09-05 21:58:00 +0200
categories: unix
---

On Unix systems, patches generated with `diff(1)` provide a convenient method to update source files without having to modify any code manually. Some open source software projects like *[suckless](https://suckless.org/)* actually rely on patching as a mechanism for adding features to programs. Also, patches can be used to easily share code between programmers during software development. In this short post, we will explore the basics of generating patches with `diff(1)` and applying patches with `patch(1)`.

# How to Generate a Patch
Imagine the following two *C* source files:

```c
/* main.c */
#include <stdio.h>

int main(void)
{
    printf("hello user!\n");
    return 0;
}
```

```c
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

In `main_with_feature.c`, the programmer added a highly important feature that improves the usability of his company's software by *a lot*. Thus, users that already use `main.c` require a convenient way to update their sources[^1]. The first step to achieve this is the creation of a *patch* that we can distribute to users:

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

During development, it might not always be clear whether a patch can be applied without errors. Thus, `patch(1)` has flags to create a backup as well as perform a dry run to test for any errors:

```bash
patch -b < important_feature.patch          # creates a backup `main.c.orig'
patch --dry-run < important_feature.patch   # perform a dry run
```

# Creating Patches for Entire Source Trees
A more complicated use cases are patches for multiples files that might be scattered throughout different project directories. But because `diff(1)` is great, it can compare two multi-file, multi-directory software project recursively with the following command:

```bash
diff -Naur ./source_tree-v0.0.1 ./source_tree-v0.0.2 > tree.patch
```

If users now applies `tree.patch` to `source_tree-v0.0.1`, they will have upgraded from one version to the other in no time.

# Summary
`diff(1)` and `patch(1)` are great tools to upgrade source files and share features with users and other developers. In this post, we only discussed the very basics. Obviously, every tool has its limitations though. After applying multiple patches, it gets more and more difficult for `patch(1)` to figure out how to include yet another patch file. At some point it might even be necessary to start editing code manually. Thus, it is always a good idea to think about whether a new major release is more appropriate than shipping another patch.

As always, I hope you enjoyed reading this post. Please let me know in the comments or via [Twitter](https://twitter.com/DogtorDash) if you have any questions or suggestions for improvement!

<hr class="hr-light">

[^1]: Assuming that users of our software compile their binaries from source. Patches aren't used for doing updates to binaries. That's what package managers are for.
