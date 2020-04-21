---
layout: post
title:  "AWK Language Basics"
date:   2019-08-25 20:24:00 +0200
categories: linux
---

`AWK` is an interpreted programming language used for text processing and reporting. It was originally written by Alfred Aho, Peter Weinberger, and Brian Kernighan[^1] at Bell Labs and first released in 1977. Since then, `AWK` became a hugely popular tool for text stream manipulation on UNIX systems[^2] with its own POSIX standard (POSIX 1003.1). Even today, `gawk(1)`, "[the GNU Project's implementation of the `AWK` programming language](https://www.gnu.org/software/gawk/manual/gawk.html)", is still among the core utilities of the GNU/Linux operating system. Despite its usefulness, `AWK` can seem cryptic and hardly useful at first. Thus, this post is meant to be a starting point for using `AWK` productively in your coding workflow[^3]. It is assumed that you have at least some programming experience in a language like C or Python and some concepts will be presented implicitly through examples instead of explaining every little detail. Please ask any questions in the comments section!

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

`print` is a built-in `AWK` function that takes a string and outputs it to `stdout`, followed by a new line. As in most C-like programming languages, function arguments are enclosed in parentheses. It is valid to omit them, though (i.e. `print "hello world"` is a valid function call, too). Also, an `AWK` program is valid with any combination of `BEGIN`, `END` and body blocks, none of them is required[^4]. To execute the program above, run:

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

In the body block, every line read from `input.txt` is by default provided as an argument to `print` which results in literal printing of that line. This happens because no explicit arguments were given to `print` in this example.

# Printing Fields of a Record
`AWK` splits every read line (also called record in `AWK`s jargon) into fields using a white space as the default separator. These fields are then available through variables `$1`, `$2`, `$3`, and so forth, `$0` holds the entire input line. The following examples demonstrates their usage (refer to the comments to fully understand the example):

```awk
BEGIN {
    # this is a comment
}

# -- Start of AWK's implicit main loop -- #

# the following block gets filtered based on a regular expression,
# i.e. only gets executed if a match occurred
/2018/ {
    # if e.g. `$12' doesn't exist, nothing is printed instead
    print($1 " -- " $2 " -- " $3 " -- " $12)
}

# the following block doesn't get filtered, i.e. gets executed with
# every line of input
{
    print
}

# -- End of AWK's implicit main loop -- #
```

`/some_regex/` matches the current record against a regular expression first. It is possible to have a filtered and an unfiltered main block in an `AWK` program at the same time. Every of those blocks gets executed with every newly read record until `AWK` finds the end of the input file. Thus, the program above produces the following output:

```bash
awk -f main.awk input.txt
# Name            Class Dorm           Room GPA
# Sally -- Whittaker -- 2018
# Sally Whittaker 2018  McCarren House 312  3.75
# Belinda Jameson 2017  Cushing House  148  3.52
# Jeff -- Smith -- 2018
# Jeff Smith      2018  Prescott House 17-D 3.20
# Sandy Allen     2019  Oliver House   108  3.48
# Jim Patterson   2017  White House    97   2.99
```

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
`AWK` control flow statements and looping constructs are (again) similar to the syntax of the C programming language. The following example demonstrates their basic usage:

```awk
{
    for (i = 0; i < 10; i++) {
        if (i < 5) {
            printf("smaller than 5\n")
        } else {
            printf("equal to or larger than 5\n")
        }
    }
}
```

Furthermore, a `while` (and `do-while`) loop exists along with `continue` and `break` statements:

```awk
{
    i = 0
    while (1) {
        if (i > 5) {
            print("breaking out of loop")
            break
        } else {
            print("still looping")
            i++
            continue
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
1. `exit(num)` exits the program with `num` as the return status
1. `sin(num)`, `cos(num)`, `log(num)` return the sine, cosine and natural logarithm of `num`
1. `rand()` returns a random number between `0` and `1` (the seed can be set via `srand(num)`)
1. `asort(arr)` sorts the array `arr`
1. `gsub(regex, sub, str)` globally replaces occurrences of `sub` in `str` with `regex` (if `str` is omitted, `$0` is used; `sub(regex, sub, str)` exists, too)
1. `index(str, sub)` returns the index of `sub` in `str`, starting at `1` (`0` if strings don't match)
1. `split(str, arr, regex)` splits `str` into fields based on `regex` (fields are now accessible through `arr`)
1. `int(num)`, `strtonum(str)` truncates `num` to an integer and returns the numerical value of `str`, respectively
1. `tolower(str)`, `toupper(str)` return a copy of `str` in lower- or uppercase
1. `systime()` returns the seconds since the epoch
1. `strftime(fmt, systime())` formats the output of `systime()` according to `fmt`
1. `and(num1, num2)`, `or(num1, num2)`, `xor(num1, num2)`, `rshift(num1, num2)`, `lshift(num1, num2)`, `compl(num)` do the respective bitwise calculations on their arguments

There are many more functions one can us, some of them even allow for execution of shell programs from within `AWK` (e.g. with the `system(cmd)` function). Please refer to the documentation of your `AWK` implementation to learn about more advanced topics like that.

# Custom Functions in AWK
In addition to the built-ins above, users can define their own, custom functions to achieve code re-usability. They are defined using the `function` keyword and return values using the `return` keyword. Calling a custom function works the same way as calling a built-in:

```awk
function add(a, b) {
    res = a + b
    return res
}

{
    c = add(1, 2)
    print c
}
```

# Output Redirection
The UNIX shell's redirection operators `>` and `>>` have a special meaning in `AWK` programs, too. Consider the following example:

```bash
awk 'BEGIN { print "some log message" > "/tmp/awk_log"}'
```

The program above writes a string to a log file. `>>` would have appended the string to the file if it already existed. Another shell concept are pipes and (surprise, surprise) those do exist in `AWK`, too:

```bash
awk 'BEGIN { print "important information\nnot interesting at all" | "grep important" }'
```

For executing shell commands that are more complex than e.g. the `grep` command above, one would probably pipe `AWK`s output over to a shell script, though. Lastly, a two-way communication channel can be established with `|&` if a user wants to use the output of a shell command in the remainder of their `AWK` program:

```awk
BEGIN {
    # define the shell command
    cmd = "grep important"

    # execute the command
    print "important information\nnot interesting at all" |& cmd

    # close the sending end of the channel
    close(cmd, "to")

    # use `getline' to store the output of `cmd' in `out'
    # `out' can now be used like a regular variable
    cmd |& getline out

    # finally, close the receiving end of the channel
    close(cmd)
}
```

# Conclusion
`AWK` is a Turing complete, interpreted programming language for text manipulation and analysis with a C-like syntax, a vast amount of built-in functions and variables, an associative array data type, and shell communication features. Even though this post only touches the surface of `AWK`s capabilities and didn't provide more than toy examples of how to use the language, now you hopefully got all the basics needed to use `AWK` productively for day-to-day tasks like scanning log files, extracting knowledge from source code, re-structuring CSV files and much more. Please let me know in the comments or via [Twitter](https://twitter.com/DogtorDash) if you have any questions or suggestions for improvement!

<hr class="hr-light">

[^1]: The word `AWK` is an acronym formed from the first letters of its authors' last names.
[^2]: All programmers deal with text on a daily basis and `AWK` is one of the best tools to achieve even complex transformations relatively easily. For example, instead of writing a new `Python` program for analyzing or reformatting your source code, consider using `AWK` next time!
[^3]: That's why this post deals with `AWK` only and *doesn't* explain concepts like loops or how to run programs on a UNIX system.
[^4]: Without any of the three, the resulting program obviously does not do anything at all.
[^5]: Other useful flags are `-v name=value` which assigns a value to a variable before script execution, `--dump-variables` and `--lint`.
[^6]: Array indices like `arr[1] = 42` can be used, but they don't need to be a continuous set of numbers. String and number indices can be mixed, because everything is an associative array in `AWK`.
