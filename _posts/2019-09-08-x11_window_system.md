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
        /* get the next event (blocks if there is none) */
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

# Drawing to a Window
`Xlib` provides a number of functions for drawing graphical primitives to a window. They usually take a `Display`, a `Window`, a `GC`, and a number of variables that determine the starting position, width, height, etc. of the shape that's to be drawn:

```c
XDrawLine(display, window, context, x1, y1, x2, y2);
XDrawArc(display, window, context, x, y, width, height, arc_start, arc_stop);
XDrawRectangle(display, window, context, x, y, width, height)
XFillArc(display, window, context, x, y, width, height, arc_start, arc_stop);
XFillRectangle(display, window, context, x, y, width, height);
XDrawString(display, window, context, x, y, string, strlen(string));
```

See its `man` page for more information about a specific function.

# How X11 Handles Colors
When using the functions above, you will notice that *X11* uses the current fore- and background colors of the window (i.e. the ones specified during window creation). One can create and use more colors than just black and white, tough:

```c
/* get_color: return a pixel value corresponding to `col_name'. */
unsigned long get_color(const char *col_name)
{
    XColor color;

    XParseColor(display, DefaultColormap(display, screen), col_name, &color);
    XAllocColor(display, DefaultColormap(display, screen), &color);
    return color.pixel;
}
```

To figure out which name a specific *RGB* color value has, read `/usr/share/X11/rgb.txt`. In addition to using explicit names, colors can also be specified using their *RGB* representation. Consult `man XColor` for more information.

Whereas the code above creates a shared color map, private color maps exist, too. Creating them is a little more difficult and works as follows:

```c
void create_private_colormap() {
	int i;
	Colormap map;
	XColor color[255];

	for(i = 0; i < 255; i++) {
		color[i].pixel = i;
		color[i].flags = DoRed | DoGreen | DoBlue;
		color[i].red = i*256;
		color[i].blue = i*256;
		color[i].green = i*256;
	}

	map = XCreateColormap(display, RootWindow(display, screen),
		                  DefaultVisual(display, screen), AllocAll);
	XStoreColors(display, map, color, 255);
	XSetWindowColormap(display, window, map);
}

/* later, this resource should be freed */
XFreeColormap(display, map);
```

Either way, to apply that newly generated color information to a window, one more function needs to be called:

```c
XSetForeground(display, context, some_pixel_value);
```

In this call, `some_pixel_value` is the `XColor.pixel` member of an individual color of either a shared or private color map.

# More Advanced Topics
XImage, .xbm files, cursors, etc.

# Summary
...
