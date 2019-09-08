---
layout: post
title:  "Sharing Memory Between Processes on Linux"
date:   2019-08-19 14:59:00 +0200
categories: linux
---

In *Unix*-derived operating systems like *Linux*, processes each have their own address space, i.e. a range of physical addresses they can access without causing a segmentation fault. Even tough the operating system creates the illusion of a continuous range of virtual addresses that a process might use, physical memory is limited and different processes are by design limited to using their own sets of physical addresses to not interfere with each other.

![Physical and virtual addresses](../../../../assets/addresses.png)
