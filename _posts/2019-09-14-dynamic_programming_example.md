---
layout: post
title:  "How to Apply Dynamic Programming Techniques"
date:   2019-09-14 15:35:00 +0200
categories: algorithms
---

*Dynamic programming* (or *DP*) is an algorithmic method that breaks a rather complex computational problem up into smaller problems that are -- hopefully -- much easier to solve. The algorithm solving that sub-problem is then applied multiple times, either recursively or iteratively, to create an overall problem solution.

This might sound strange at first and far too abstract to be useful for solving any actual problems. Also, an extensive body of literature discussing *dynamic programming* exists and it might look like you need to know a lot of theory to use *DP* at all. To get you started with dynamic programming without diving into anything mathematics- or theory-related[^1], this post talks about just one, concrete example of *DP*.

# The Problem

Sometimes, computational problems have just too many "degrees of freedom" to be solvable via an iterative algorithm. By that I mean problems that cannot be tackled by e.g. using multiple, nested `for` loops because we would just need too many of them. Lets consider a problem similar to [this Project Euler](https://projecteuler.net/problem=81) question. Given a grid like the one below, we want to find the **minimal sum of integers** along a **path from the top left to the bottom right**. In every step, one can only go **right or down** (**not** diagonally):

![Grid Example](../../../../assets/grid_basic.png)

The overall problem solution is the sum of the integers along the green path, i.e. `4 + 2 + 6 + 3 + 1 + 8 = 24`. According to the problem definition, `24` is the correct solution only if there is **no other, valid path** that yields a smaller sum.

# The Solution
As we said in the introduction, the basis of *dynamic programming* is identifying a sub-problem that's easy to solve. One can then try to construct a solution for the original problem from there.

Now, if we look at the grid above, we first look at the simplest scenario possible and try to find a solution for it. That is, a `1x1` grid where the final result would be the integer written into that one and only tile. There is no path to find because start and finish are at the same tile. The next, more difficult grid to solve has the dimensions `2x2`. If we were able to find a pattern or commonality between the `1x1` and the `2x2` grid (and maybe one more grid of dimensions `3x3`), we could already have an algorithm at hand that we could use to solve grids with all kinds of dimensions. Even if not, we might learn something useful to derive such an algorithm. Take a look at the following grids:

![Grid Easy Example](../../../../assets/grid_easy.png)

How do we decide which path to take? With the `2x2` grid, we can just compare all possible paths by hand. `4 + 7 + 6 = 17` is larger than `4 + 2 + 6 = 12`, so the second path is the correct solution. It's already getting more difficult to find the optimal path in a `3x3` grid, though. A possible algorithm would be to always go to the tile that has a smaller integer written on it. Let's try it:

![Grid Wrong Example](../../../../assets/grid_wrong.png)

Unfortunately, there are cases where this strategy doesn't work. Why is that? Well, we have to consider **the sum** of all the numbers on the tiles we pass from start to finish. In the `2x2` grid, that is easy to achieve (ignore the `3x3` grid for the time being):

![Grid With Sums](../../../../assets/grid_with_sums.png)

...

# The Code
...

# Final Thoughts
There is an entire class of problems that is incredibly hard to solve without a dynamic programming approach. In our case for example, it just isn't feasible to iteratively try every path through the grid to find the minimal sum of integers. Whereas it's certainly possible for small grids, any such solution will not generalize at all to larger grids.

This post tried to explain some of the reasoning behind *DP* as well as how to approach a problem where you might feel like *DP* could be a viable option. Computer algorithms aren't always as easy as the example above, so some of my future posts might expand on the topic of algorithmic thinking and *dynamic programming* in particular.

As always, I hope you enjoyed reading this post. Please let me know in the comments or via [Twitter](https://twitter.com/DogtorDash) if you have any questions or suggestions for improvement!

<hr class="hr-light">

[^1]: We won't even discuss computational complexity of *DP* algorithms in this post. Maybe in the future, another post will cover more advanced topics, though.
