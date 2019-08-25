---
layout: post
title:  "AWK Language Basics"
date:   2019-08-25 20:24:00 +0200
categories: awk
---

`AWK` is an interpreted programming language used for text processing and reporting. It was originally written by Alfred Aho, Peter Weinberger, and Brian Kernighan[^1] at Bell Labs and first released in 1977. Since then, `AWK` became a hugely popular tool for text stream manipulation on UNIX systems[^2] with its own POSIX standard (POSIX 1003.1). Even today, `gawk(1)`, "[the GNU Project's implementation of the `AWK` programming language](https://www.gnu.org/software/gawk/manual/gawk.html)", is still among the core utilities of the GNU/Linux operating system. Despite its usefulness, `AWK` can seem cryptic and hardly useful at first. Thus, this post is meant to be a starting point for using `AWK` productively in your coding workflow.

# AWK Language Paradigms
`AWK` reads an input stream line by line ...

[^1]: The word `AWK` is an acronym formed from the first letters of its authors' last names.
[^2]: All programmers deal with text on a daily basis and `AWK` is one of the best tools to achieve even complex transformations relatively easily. For example, instead of writing a new `Python` program for analyzing or reformatting your source code, consider using `AWK` next time!
