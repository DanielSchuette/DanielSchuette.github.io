---
layout: post
title:  "Introduction to the X Window System API"
date:   2019-09-08 23:54:00 +0200
categories: unix
---

The *X Window System* (usually abbreviated as *X11*) implements a basic set of graphics routines for bitmap displays, which enable rendering and manipulation of windows. You can think of a windowing system as the first user space API layer that enables application programmers to draw graphics to the screen[^1]. These days, *X11* is the most widely used windowing system on *GNU/Linux*. In addition to its rendering facilities, it provides an API for working with mouse and keyboard inputs.

The implementation of more advanced and abstract graphics elements that are necessary to build GUIs -- like buttons or menus -- is left to APIs like *GTK+*. They build upon *X11* if not configured to talk to the kernel directly. Different *Linux* desktop environments like *GNOME* or *KDE* look completely different, even if they both run on an *X11* server (we'll talk about the "server" part later on). *X11* really just defines the protocols and most primitive graphical operations and does not deal with design at all.

Again, *X11* is **not** designed to be a GUI library. Thus, most programmers will never need to use it directly. But knowing some of the underlying principles can still facilitate an understanding of the *Linux* graphics stack.

# Creating an X11 Window
To work with *X11*, a number of header files can be included:

```c
/*
 * Headers are usually found in `/usr/include/X11'.
 * The information below stems from:
 * https://en.wikibooks.org/wiki/X_Window_Programming/XLib/Standard_Headers
 */
#include <X11/X10.h> 	    /* X10 compatibility functions, types and symbols. */
#include <X11/Xatom.h> 	    /* All predefined atoms, which are symbols with the prefix `XA_`.*/
#include <X11/cursorfont.h> /* Standard cursor font symbols, which have the `XC_` prefix. */
#include <X11/X.h> 	        /* Types and constants for applications. Included automatically
                             * within <X11/Xlib.h>. */
#include <X11/keysymdef.h> 	/* Standard KeySym values, which are symbols with the prefix ``XK_``.
                             * KeySyms are arranged in groups, and a preprocessor symbol controls
                             * inclusion of each group. The preprocessor symbol must be defined
                             * prior to inclusion of the file to obtain the associated values.
                             * The preprocessor symbols are XK_MISCELLANY, XK_LATIN1, XK_LATIN2,
                             * XK_LATIN3, XK_LATIN4, XK_KATAKANA, XK_ARABIC, XK_CYRILLIC, XK_GREEK,
                             * XK_TECHNICAL, XK_SPECIAL, XK_PUBLISHING, XK_APL, XK_HEBREW, XK_THAI,
                             * and XK_KOREAN.
                             */
#include <X11/keysym.h> 	/* Defines the preprocessor symbols XK_MISCELLANY, XK_LATIN1, XK_LATIN2,
                             * XK_LATIN3, XK_LATIN4, and XK_GREEK and then includes <X11/keysymdef.h>
                             */
#include <X11/Xlib.h> 	    /* Main header for Xlib which contains the majority of all symbols. Since
                             * release 5, contains the preprocessor symbol XlibSpecificationRelease.
                             */
#include <X11/Xlibint.h> 	/* Functions, types, and symbols used for extensions. Automatically includes
                             * <X11/Xlib.h>.
                             */
#include <X11/Xcms.h> 	    /* Color management functions, types and constants, which begin with the
                             * prefix ``Xcms`. Color Conversion Contexts macros are also declared in
                             * this file. Depends on <X11/Xlib.h> being included first.
                             */
#include <X11/Xutil.h> 	    /* Inter-client communication and application utility functions, types and
                             * symbols. Depends on <X11/Xlib.h> being included first.
                             */
#include <X11/Xresource.h> 	/* Resource Manager functions, types, and symbols. Depends on <X11/Xlib.h>
                             * being included first.
                             */
```

In this post, we will work with `Xlib`. The *X11* protocol follows a client -- server architecture and `Xlib` is a commonly used implementation of an `X11` **client**. There are other libraries that do the same thing, e.g. `XCB`, but we won't cover their APIs in this post.

Broadly speaking, a client can connect to an `X11` server and request the creation of windows or receive events from it. This model has the advantage of enabling remote `X11` sessions where client and server run on different systems and communicate via some kind of networking connection. This post won't talk about `X11` server implementations, though. To learn more about those, `man xinit` is the way to go.

A client-to-server connection is characterized by a few variables that are usually declared globally to be accessible by all API calls. `X11` makes a clear distinction between a **display** and its associated physical **screen**:

```c
static Display *display; /* information about device where graphical operations are done */
static int screen;       /* represents the actual, physical screen */
```

Another three variables holding window, graphics context, and event information, will be passed around function calls, too:

```c
static Window window; /* windows are controlled via this variable */
static GC context;    /* see below for an explanation */
static Event event;   /* filled with window events in the event loop */
```

One other, important property of `X11` is its statelessness. A `GC` (graphics context) saves the current drawing state between operations. Thus, we need to pass it to the server every time some state change is requested.

The five variables above are used by a client to ask the server to perform specific operations. If the client wants to change literally anything, e.g. window features, colors, fonts, and so forth, it must obtain an identifier from the server and call an appropriate function with it. The server might than fulfill the request -- or not.

For a client to create a window, a number of functions need to be called:

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

Since this post is meant to be a starting point for *X11* programming, the functions above aren't explained in much detail. Extensive documentation is available via `man some_function`, though.

After creating a window, an event queue can be polled using `XNextEvent()`:

```c
while(1) {
    /*
     * get the next event (blocks if there is none)
     * XNextEvent() calls XFlush(), so we don't need to explicitly
     * flush the request queue (requests are not send to the server
     * one-by-one but batch-wise!)
     */
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

Running `prog` opens a white window which -- as expected -- closes upon any key press. A small black cube is drawn to it, too. Also, don't forget that this code only works if there is an `X11` server running, that our client program can talk to. If your *GNU/Linux* distribution does not use `X11` as its windowing system, `XOpenDisplay()` will fail because there is no display it could obtain. That's why you should always check if this function call succeeded or not.

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

They all do what their names imply. If you have any questions, you should consult their `man` pages for more information.

# How X11 Handles Colors
When calling the functions above, you will notice that *X11* uses the current fore- and background colors of the window (i.e. the ones specified during window creation). For a nice GUI, one needs to use more colors than just black and white, tough[^2]:

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

To figure out which name a specific *RGB* color value has, read `/usr/share/X11/rgb.txt`. In addition to using explicit names, colors can also be specified using their *RGB* representation. Please read `man XColor` for a thorough introduction to the *X11* color system.

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
There is so much more to learn about *X11* before we could start writing an actual program, though. Currently, we cannot even load images or display text on the screen! The best place to look for anything *X11* related is the official [X.org webpage](https://www.x.org/wiki/Documentation/).

# Summary
As we said in the introduction, *X11* is really just the basis of libraries like *GTK+* or *Qt*. It's incredibly hard to use it for GUI creation[^3]. But if we want to understand their inner workings or write low level graphics applications that only use graphics primitives, knowing the basics of the *X Window System* protocol and how to create a window with it might come in handy! Having read this post, you should now be able to dive deeper into the `Xlib` API and learn everything you need to know to create whatever application you want.

As always, I hope you enjoyed reading this post. Please let me know in the comments or via [Twitter](https://twitter.com/DogtorDash) if you have any questions or suggestions for improvement!

<hr class="hr-light">

[^1]: On a *Unix*-like operating system and assuming you don't want to write to the [framebuffer](https://en.wikipedia.org/wiki/Linux_framebuffer) directly.
[^2]: To abstract away hardware differences, *X11* only guarantees the existence of black and white colors.Every other color must be requested from the server and might look different on different systems.
[^3]: It is probably impossible, considering the amount of work that went into graphics toolkits like *GTK+*.
