---
layout: post
title:  "AWK Language Basics"
date:   2019-08-25 20:24:00 +0200
categories: awk
---

`AWK` is an interpreted programming language used for text processing and reporting. It was originally written by Alfred Aho, Peter Weinberger, and Brian Kernighan[^1] at Bell Labs and first released in 1977. Since then, `AWK` became a hugely popular tool for text stream manipulation on UNIX systems[^2] with its own POSIX standard (POSIX 1003.1). Even today, `gawk(1)`, "[the GNU Project's implementation of the `AWK` programming language](https://www.gnu.org/software/gawk/manual/gawk.html)", is still among the core utilities of the GNU/Linux operating system. Despite its usefulness, `AWK` can seem cryptic and hardly useful at first. Thus, this post is meant to be a starting point for using `AWK` productively in your coding workflow[^3].

# Phases of an AWK Program
An `AWK` program has three phases. First, the **BEGIN** block is executed. It's usually used to set up an environment for the program to run in, e.g. initialize variables or change `AWK`s default field separator. Next, `AWK` will read the input line by line until the end and execute one or more commands on every of those lines. And lastly, the **END** block is run which can e.g. be used for reporting of results.

```awk
BEGIN { awk-commands }
/pattern/ { awk-commands }
END { awk-commands }
```

`AWK`s keywords like `BEGIN` and `END` are all uppercase and code blocks are enclosed by curly braces. The program's body can be prefixed with a `/pattern/` which filters lines based on regular expressions. That's really how difficult things get. Everything mentioned in this section will be demonstrated using examples below.

# Hello World with AWK
The following program prints the string *hello world* once and exits:

```awk
BEGIN { print("hello world") }
```

`print` is a built-in `AWK` function that takes a string and outputs it to `stdout`. As in most C-like programming languages, function arguments are enclosed in parentheses. It is valid to omit them, though (i.e. `print "hello world"` is a valid function call, too). Also, an `AWK` program is valid with any combination of `BEGIN`, `END` and body blocks, none of them is required[^4]. To execute the program above, run:

```bash
awk -f hello.awk
```

Programs can be provided to `awk(1)` directly via a command line option, too (`man awk` for different ways to invoke `AWK`[^5]):

```bash
awk 'BEGIN { print "hello world" }'
```

# Processing an Input File
Next, we'll look at line-wise processing of an input file. The first six entries of the used example file are shown below:

```bash
cat input.txt | head -n 6
# Name            Class Dorm           Room GPA
# Sally Whittaker 2018  McCarren House 312  3.75
# Belinda Jameson 2017  Cushing House  148  3.52
# Jeff Smith      2018  Prescott House 17-D 3.20
# Sandy Allen     2019  Oliver House   108  3.48
# Jim Patterson   2017  White House    97   2.99
```

The following program prints at a message at program start and exit using `printf` which is similar to the C function of the same name:

```awk
BEGIN { printf("contents of input.txt:\n") }
{
    print
}
END { printf("end of input.txt.\n") }
```

In the body block, every line read from `input.txt` is provided as an argument to `print` which results in literal printing of that line.

# Printing Fields of a Record
`AWK` splits every read line (also called record in `AWK`s jargon) into fields using a white space as the default separator. These fields are then available through variables `$1`, `$2`, `$3`, and so forth, `$0` holds the entire input line. The following examples demonstrates their usage:

```awk
BEGIN {
    # this is a comment
}

# the following block gets filtered based on a regular expression
/2018/ {
    # if e.g. `$12' doesn't exist, nothing is printed instead
    print($1 " -- " $2 " -- " $3 " -- " $12 "\n")
}

# the following block doesn't get filtered
{
    print
}

```

`/some_regex/` matches the current record against a regular expression first. It is possible to have a filtered and an unfiltered block in an `AWK` program at the same time.

# Using Variables in an AWK Program
The following `AWK` program shows how to use custom variables along with some C-like syntax for incrementing a counter and formatting strings with `printf`:

```awk
BEGIN { count=0 }

{ count++ }

END { printf("number of lines in file: %d\n", count) }
```

In addition, variables can be used with arithmetic (`+`, `-`, `*`, `/`, `%`), logical (`&&`, `||`, `!`) and relational (`==`, `<=`, `>=`, `>`, `<`) operators. Strings can be concatenated like this: `new_str = str1 str2`.

`AWK` also supports associative arrays which are similar to e.g. Python's `dict` type. `my_array["a_value"] = 42` assigns a number to an entry in the array, `printf("%d\n", my_array["a_value"])` retrieves it[^6] and `delete my_array["a_value"]` deletes it.

There are a number of built-in variables that determine a program's behavior, the field separator is one of them. Their defaults can be changed for the whole program in the `BEGIN` block:

| Built-in | Meaning                                                                 |
|----------|-------------------------------------------------------------------------|
| FS       | the field separator (default is `" "`)                                  |
| OFS      | the output field separator                                              |
| RS       | the record separator (separator between lines, `"\n"` by default)       |
| ORS      | the output record separator                                             |
| NR       | the number of records in a file                                         |
| NF       | the number of fields in a line                                          |
| FILENAME | the name of the current input file                                      |
| ARGC     | the number of arguments (equivalent to C's `argc`)                      |
| ARGV     | the argument array (`0` through `ARGC-1`)                               |
| ENVIRON  | an associative array of `env` variables (`ENVIRON["VAR"]`)              |

# Control Flow and Looping
`AWK` control flow statements and looping constructs are similar to C. The following example demonstrates their basic usage:

```awk
{
    for (i = 0; i < 10; i++) {
        if (i < 5) {
            printf("index: %d\n")
        }
    }
}
```

# Regular Expression Usage
`AWK` has strong support for regular expressions. Since this is not a regex tutorial, the general concept of regular expression-based pattern matching [is not explained](https://en.wikipedia.org/wiki/Regular_expression#Basic_concepts). The following table summarizes the most important operations to construct regex in `AWK`:

| Operation   | Matches                                                 |
|-------------|---------------------------------------------------------|
| `.`         | any single character except `"\n"`                      |
| `^`         | the start of a line                                     |
| `$`         | the end of a line                                       |
| `[abc]`     | one of several characters (a character set)             |
| `[^abc]`    | none of several characters (an exclusive character set) |
| `(aaa|bbb)` | one of two string (`()` are required for grouping)      |
| `?`         | zero or one occurance of the preceding character        |
| `*`         | zero or more occurances of the preceding character      |
| `+`         | one or more occurances of the preceding character       |

# Built-in Functions
We already mentioned two functions for printing text to the screen (`print` and `printf`). In `AWK`, a number of other useful functions exist:

1. `length(str)` returns the length of a string in characters
1. `match(str, substr)` returns the index of `substr` in `str` in `RSTART` and a boolean indicating a match
1. `...`

# Custom Functions in AWK
...

# Conclusion
...

<hr class="hr-light">

[^1]: The word `AWK` is an acronym formed from the first letters of its authors' last names.
[^2]: All programmers deal with text on a daily basis and `AWK` is one of the best tools to achieve even complex transformations relatively easily. For example, instead of writing a new `Python` program for analyzing or reformatting your source code, consider using `AWK` next time!
[^3]: That's why this post deals with `AWK` only and *doesn't* explain concepts like loops or how to run programs on a UNIX system.
[^4]: Without any of the three, the resulting program obviously does not do anything at all.
[^5]: Other useful flags are `-v name=value` which assigns a value to a variable before script execution, `--dump-variables` and `--lint`.
[^6]: Array indices like `arr[1] = 42` can be used, but they don't need to be a continuous set of numbers. String and number indices can be mixed, because everything is an associative array in `AWK`.
