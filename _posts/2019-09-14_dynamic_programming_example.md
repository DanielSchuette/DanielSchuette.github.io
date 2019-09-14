---
layout: post
title:  "How to Apply Dynamic Programming Techniques"
date:   2019-09-14 15:35:00 +0200
categories: algorithms
---

*Dynamic programming* (or *DP*) is an algorithmic method that breaks a rather complex computational problem up into smaller problems that are -- hopefully -- much easier to solve. The algorithm solving that sub-problem is then applied multiple times, either recursively or iteratively, to create an overall problem solution.

This might sound strange at first and far too abstract to be useful for solving any actual problems. Also, an extensive body of literature discussing *dynamic programming* exists and it might look like you need to know a lot of theory to use *DP*. To get you started with dynamic programming without diving into anything mathematics related[^1], this post talk about just one, concrete example of *DP*.

# The Problem

Sometimes, computational problems have just too "degrees of freedom" to be solvable by iterative algorithms. By that I mean problems that cannot tackled by using multiple, nested `for` loops because we would just need too many of them. Lets consider [this Project Euler](https://projecteuler.net/problem=81) problem. Given the grid below, we want to find the **minimal path sum** from the top left to the bottom right. In every step, one can only go **right or left**:

<hr class="hr-light">

[^1]: We won't even discuss computational complexity of *DP* algorithms in this post. Maybe in the future, another post will cover more advanced *DP*, though.
