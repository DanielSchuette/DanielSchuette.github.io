---
layout: post
title:  "Sharing Memory Between Processes on Linux"
date:   2019-08-19 14:59:00 +0200
categories: linux
---

On *Unix*-like operating systems like *Linux*, every process has its own address space, i.e. a range of physical addresses it can access without causing a segmentation fault. Even tough the operating system creates the illusion of a continuous range of virtual addresses that a process might use, physical memory is limited and different processes are by design restricted to using their own sets of physical addresses to not interfere with each other. The following schematic shows how hardware addresses are related to this software abstraction.

![Physical and virtual addresses](../../../../assets/addresses.png)

Now, some applications require a way for processes to communicate ([inter-process communication](https://en.wikipedia.org/wiki/Inter-process_communication) or *IPC*) and one option to exchange messages or data is via shared memory[^1]. As we saw above, processes (as opposed to threads) are not usually able to access memory pages they don't own. Conveniently enough, *POSIX* provides a standardized API to ...

<hr class="hr-light">

[^1]: Other *IPC* methods on *Linux* include sockets, signals, message queues, and pipes, among others.
