---
layout: post
title:  "The X Window System Explained"
date:   2019-09-08 23:54:00 +0200
categories: unix
---

The *X Window System* (usually abbreviated as *X11*) implements a basic set of graphics routines for bitmap displays, which enable rendering and manipulation of windows. Thus, it constitutes the first API layer necessary to draw virtually anything to the screen. These days, *X11* is the most widely used windowing system on *GNU/Linux*. In addition to its rendering facilities, it provides an API for working with mouse and keyboard inputs.

More advanced graphics elements that are necessary to build GUIs -- like buttons or menus -- are left to APIs like *GTK+* that build upon *X11*. Different *Linux* desktop environments like *GNOME* or *KDE* look completely different, even if they run on an *X11* server as a windowing system backend. *X11* really just defines the protocols and primitives for the design of graphical application interfaces.

# Creating an X11 Window
To work with *X11*, a number of header files must be included (in this post, only `<X11/Xlib.h>` is actually required):

```c
/* header are usually found in /usr/include/X11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
```

*X11* follows a client -- server model. A connection is characterized by two variables:

```c
Display *display; /* can be thought of as the backend (server) */
int screen;       /* represents the actual screen (client) */
```

Another three variables holding window, context, and event information, will be passed around function calls, too:

```c
Window window; /* windows are controlled via this variable */
GC context;    /* the graphics context determines rendering behavior */
Event event;   /* filled with window events in the event loop */
```

To create a window, a number of API functions need to be called:

```c
void create_window(void) {
    unsigned long black, white;
    Atom del_window;

    /* get a display and a screen */
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "failed to open an X11 display\n");
        exit(1);
    }
    screen = DefaultScreen(display);

    /* get black and white colors */
    black = BlackPixel(display, screen);
    white = WhitePixel(display, screen);

    /* now, create a window at position (10, 10) of size (200, 300) */
    window = XCreateSimpleWindow(display, RootWindow(display),
                                 10, 10, 200, 300, 1, black, white);

    /* create a graphics context */
    context = XCreateGC(display, window, 0, 0);

    /* set window properties */
    XSetStandardProperties(display, window, "My X11 Window", "Minimized",
                           None, NULL, 0, NULL);

    /* set up window delete events */
    del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &del_window, 1);

    /* define which events are accepted by the window */
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    /* move window to the top */
    XMapWindow(display, window);
}
```

Since this post is meant to be a starting point for *X11* programming, the functions above aren't explained in much detail. Extensive documentation is available online, though.

After creating a window, an event queue is polled using `XNextEvent()`.

```c
    while(1) {
        XNextEvent(display, &event);
        /* draw a small black rectangle */
        if(event.type == Expose)
            XFillRectangle(display, window, context, 20, 20, 10, 10);

        /* end the loop upon any key press or client message (e.g. delete event) */
        if(event.type == KeyPress)
            break;
        if(event.type == ClientMessage)
           break;
    }
```

Lastly, system resources are freed by destroying the window and closing the display:

```c
    XDestroyWindow(display, window);
    XCloseDisplay(display);
```

When compiling the code above, the linker needs to be instructed to link against the *X11* library as follows:

```bash
gcc -o prog main.c -lX11
```

Running `prog` opens a white window which -- as expected -- closes upon any key press. A small black cube is drawn to it, too.

# Summary
...
