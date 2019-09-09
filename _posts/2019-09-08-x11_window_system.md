---
layout: post
title:  "Introduction to the X Window System API"
date:   2019-09-08 23:54:00 +0200
categories: unix
---

The *X Window System* (usually abbreviated as *X11*) implements a basic set of graphics routines for bitmap displays, which enable rendering and manipulation of windows. Thus, it constitutes the first API layer necessary to draw virtually anything to the screen. These days, *X11* is the most widely used windowing system on *GNU/Linux*. In addition to its rendering facilities, it provides an API for working with mouse and keyboard inputs.

More advanced graphics elements that are necessary to build GUIs -- like buttons or menus -- are left to APIs like *GTK+* that build upon *X11*. Different *Linux* desktop environments like *GNOME* or *KDE* look completely different, even if they run on an *X11* server as a windowing system backend. *X11* really just defines the protocols and primitives for the design of graphical application interfaces.

# Creating an X11 Window
To work with *X11*, a number of header files can be included:

```c
/* headers are usually found in /usr/include/X11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xft/Xft.h>
#include <X11/XKBlib.h>
```

In this post, we will mainly work with `Xlib`. The *X11* protocol follows a client -- server architecture and `Xlib` is a commonly used implementation of an `X11` **client**. There are other libraries that do the same thing, e.g. `XCB`, but we won't cover their APIs in this post.

Broadly speaking, a client can connect to an `X11` server and request the creation of windows or receive events from it. This model has the advantage of enabling remote `X11` sessions where client and server run on different systems and communicate via some kind of network connection. This post won't talk about `X11` server implementations at all. To learn more about those, `man xinit` is a good starting point.

A client -- server connection is characterized a few variables that are usually declared globally to be accessible by all API calls. `X11` makes a clear distinction between a **display** and its associated physical **screen**:

```c
static Display *display; /* information about device where graphical operations are done */
static int screen;       /* represents the actual, physical screen */
```

Another three variables holding window, context, and event information, will be passed around function calls, too:

```c
static Window window; /* windows are controlled via this variable */
static GC context;    /* the graphics context determines rendering behavior */
static Event event;   /* filled with window events in the event loop */
```

The five variables above are used by a client to request specific operations from the server. If the client wants to change literally anything, e.g. windows features, colors, fonts, and so forth, it must obtain an identifier from the server and call an appropriate function with it. The server might than fulfill the request -- or not.

For a client to create a window, a number of API functions need to be called:

```c
void create_window(void) {
    unsigned long black, white;
    Atom del_window;

    /* create a connection to the server */
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "failed to open an X11 display\n");
        exit(1);
    }

    /* get the default screen associated with the display */
    screen = DefaultScreen(display);

    /* define black and white colors */
    black = BlackPixel(display, screen);
    white = WhitePixel(display, screen);

    /* request the creation of a window at position (10, 10) of size (200, 300) */
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

    /* select which events are listened to by the window */
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    /* move window to the top */
    XMapWindow(display, window);
}
```

Since this post is meant to be a starting point for *X11* programming, the functions above aren't explained in much detail. Extensive documentation is available online, though.

After creating a window, an event queue can be polled using `XNextEvent()`:

```c
    while(1) {
        XNextEvent(display, &event);

        /* draw a small black rectangle whenever the window is not covered (i.e. exposed) */
        if(event.type == Expose)
            XFillRectangle(display, window, context, 20, 20, 10, 10);

        /* end the loop upon any key press or client message (e.g. delete event) */
        if(event.type == KeyPress)
            break;
        if(event.type == ClientMessage)
           break;
    }
```

Lastly, system resources should be freed by destroying the window and closing the display:

```c
    XDestroyWindow(display, window);
    XCloseDisplay(display);
```

When compiling the code above, the linker needs to be instructed to link against the *X11* library as follows:

```bash
gcc -o prog main.c -lX11
```

Running `prog` opens a white window which -- as expected -- closes upon any key press. A small black cube is drawn to it, too. We should not forget that this code only works if there is an `X11` server running that our client code can talk to. If your *GNU/Linux* distribution does not use `X11` as its windowing system, `XOpenDisplay()` will fail because there is no display it could obtain. That's why you should always check if this function call succeeded or not.

# The XEvent API
...

# Summary
...
