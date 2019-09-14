---
layout: post
title:  "How to Apply Dynamic Programming Techniques"
date:   2019-09-14 15:35:00 +0200
categories: algorithms
---

*Dynamic programming* (or *DP*) is an algorithmic method that breaks a rather complex computational problem up into smaller problems that are -- hopefully -- much easier to solve. The algorithm solving that sub-problem is then applied multiple times, either recursively or iteratively, to create an overall problem solution.

This might sound strange at first and far too abstract to be useful for solving any actual problems. Also, an extensive body of literature discussing *dynamic programming* exists and it might look like you need to know a lot of theory to use *DP* at all. To get you started with dynamic programming without diving into anything mathematics- or theory-related[^1], this post talk about just one, concrete example of *DP*.

# The Problem

Sometimes, computational problems have just too many "degrees of freedom" to be solvable via an iterative algorithm. By that I mean problems that cannot be tackled by e.g. using multiple, nested `for` loops because we would just need too many of them. Lets consider [this Project Euler](https://projecteuler.net/problem=81) problem. Given the grid below, we want to find the **minimal sum of integers** along a **path from the top left to the bottom right**. In every step, one can only go **right or down** (**not** diagonally):

![Grid Example](../../../../assets/grid_basic.png)

The overall problem solution is the sum of the integers along the green path, i.e. `4 + 2 + 6 + 3 + 1 + 8 = 24`. According to the problem definition, `24` is the correct solution only if there is **no other, valid path** that yields a smaller sum.

# The Solution
...

# Final Thoughts
There is an entire class of problems that is incredibly hard to solve without a dynamic programming approach. In our case for example, it just isn't feasible to iteratively try every path through the grid to find the minimal sum of integers. Whereas it's certainly possible for small grids, any such solution will not generalize to larger grids at all.

Thus, realizing a key feature of the problem -- in this case ...

<hr class="hr-light">

[^1]: We won't even discuss computational complexity of *DP* algorithms in this post. Maybe in the future, another post will cover more advanced topics, though.
