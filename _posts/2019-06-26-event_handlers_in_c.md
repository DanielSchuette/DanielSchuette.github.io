---
layout: post
title:  "Event Handlers in C"
date:   2019-06-26 22:18:00 +0200
categories: c
---

# Event Handlers in C
While reading some source code, I came across a neat way to do event handling in a fairly generic way in **C**. I will explain the idea along with a minimal, working example in this post. You can refer to [this gist](https://gist.github.com/DanielSchuette/fc0fe0214c55668743b71a5317b1e7f2) or go to the end of this page for a full source code listing.

## The Event Handler Table
Imaging you are hacking on some library that provides graphics utilities to GUI programmers (the code I read was some X11-based window manipulation program, so this is the first thing I could think of). Obviously, you will need to update a window that you draw on the screen based on a user's actions like resizing, button presses, and so on. To make this as simple and extensible as possible, a table of function pointers is set up. Functions take a generic `void *` argument and a number of preprocessor constants can be used to index into the table (`enum`s are great for this, too). At this point, it's demonstrated that not every event that the application backend recognizes, needs to have a callback handler. In this example application, only window resizing, button presses, and killing a window is handled. The backend actually sends two more events, though (pausing the application and spawning a new window). Obviously, all this is just toy code, but in my opinion, this is a nice code pattern for sending messages between library code and the code of a library's user. Here's the code described above:

```c
#include <stdio.h> /* for debugging output */

#define EV_SPAWN        0   /* a new window is spawned */
#define EV_RESIZ        1   /* an existing window is resized */
#define EV_BTNPRESS     2   /* a button is pressed */
#define EV_PAUSE        3   /* some stream is paused */
#define EV_KILL         4   /* a window is killed */
#define NUM_HANDLERS    5   /* the total number of events */

/* event handlers */
static int resize(void *);
static int button_click(void *);
static int kill(void *);

static int handle_event(unsigned int, void *);

/* event handler table, not every event is handled */
static int (*handlers[NUM_HANDLERS])(void *) = {
    [EV_RESIZ]    = resize,
    [EV_BTNPRESS] = button_click,
    [EV_KILL]     = kill,
};
```
Many additional abstractions come to mind, e.g. calling a library function to initialize the `NULL` pointers in the table with default actions. But we'll keep this example as minimal as possible.

## The Event Queue
`redraw(event_queue *)` will be used to demonstrate a possible main loop. A queue is filled with events by the application so that the main loop can poll it inbetween redraws (or whatever your application does):

```c
typedef struct {
    int len;
    int evs[3];
} event_queue;

static void redraw(event_queue *);
```

With this code, handling an event is now just selecting an event handler from the table (if it exists) and calling it with the arguments that the user provided. Obviously, a convention is needed that specifies which actual type this argument has behind the `void *`. If you don't like the way it's done in this example, you can just go another route you like better. Anyways, the callback function is responsible for returning an appropriate error code. If a nonexistent handler is requested by the caller of `handle_event()`, a generic `-1` is returned:

```c
static int handle_event(unsigned int id, void *args)
{
    if (id < NUM_HANDLERS && handlers[id])
        return handlers[id](args);
    return -1;
}
```

For the event handlers themselves, take a look at the full code listing. They are self-explanatory and don't do anything reasonable. Your application would, of course. They are still just examples.

## The Main Loop
Now to the main loop. An example `event_queue` is created which would usually be heap allocated and passed around via a pointer so that library functions can enqueue events (that's what `redraw(event_queue *)` is meant to demonstrate). Then, an infinite loop checks for any events on the queue and calls the appropriate handler from the table, based on the event number. Again, argument passing could be done differently. Lastly, the imaginary windows of this application would be redrawn and more events would be enqueued. The example just breaks so you don't have to *ctrl+c* when trying it out:

```c
int main(void)
{
    int i, ev, len, dims[] = { 300, 400 };
    char key = 'c';
    event_queue ev_queue = { .len = 3, .evs = { 1, 2, 4 } };

    for (;;) {
        /* poll the event queue */
        for (i = 0, len = ev_queue.len; i < len; i++) {
            ev = ev_queue.evs[i];
            /* call event handlers (their return status is currently ignored) */
            if (ev == EV_RESIZ) handle_event(ev, (void *)dims);
            if (ev == EV_KILL) handle_event(ev, "some kind of application");
            if (ev == EV_BTNPRESS) handle_event(ev, (void *)(&key));
        }
        redraw(&ev_queue);      /* redraw enqueues new events */
        break;                  /* any actual application would keep running */
    }

    return 0;
}
```
This is not the most difficult code pattern in the world and everything is done in plain, old **C** without fancy algorithms or data structures. I still hope you learned something and maybe you will even be able to use a similar strategy in one of your own projects. As usual, feedback and bug reports are highly welcome!

## The Full Example:
<script src="https://gist.github.com/DanielSchuette/fc0fe0214c55668743b71a5317b1e7f2.js"></script>
