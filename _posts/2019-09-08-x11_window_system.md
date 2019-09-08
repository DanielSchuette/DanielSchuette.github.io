---
layout: post
title:  "The X Window System Explained"
date:   2019-09-08 23:54:00 +0200
categories: unix
---

The *X Window System* (usually abbreviated as *X11*) implements a basic set of graphics routines for bitmap displays, which enable rendering and manipulation of windows. Thus, it constitutes the first API layer necessary to draw virtually anything to the screen. These days, *X11* is the most widely used windowing system on *GNU/Linux*. In addition to its rendering facilities, it provides an API for working with mouse and keyboard inputs.

More advanced graphics elements that are necessary to build GUIs -- like buttons or menus -- are left to APIs like *GTK+* that build upon *X11*. Different *Linux* desktop environments like *GNOME* or *KDE* look completely different, even if they run on an *X11* server as a windowing system backend. *X11* really just defines the protocols and primitives for the design of graphical application interfaces.

# Creating an X11 Window
The following three header files are required to work with *X11*. They must be included in the right order, otherwise strange behavior or even errors might occur:

```c
/* header are usually found in /usr/include/X11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
```



# Receiving Keyboard Input
...

# Summary
...
