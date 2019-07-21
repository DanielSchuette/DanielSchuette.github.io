---
layout: post
title:  "The Union Type in C"
date:   2019-07-21 18:05:00 +0200
categories: c
---

# The Union Type in C
The **union** data type is probably the least often used type in the C language overall -- as far as I can tell from source code I read and code I wrote myself[^1]. One reason for this might be a lack of use cases and that's why I'm writing this post. While doing some [SDL2](https://wiki.libsdl.org/SDL_Event?highlight=%28%5CbCategoryStruct%5Cb%29%7C%28CategoryEvents%29) development, I realized that they implement event handling really elegantly using **union**s. After reading some key parts of the event handler sources ([SDL_events.h](https://github.com/DanielSchuette/SDL-mirror/blob/master/include/SDL_events.h)), I came up with a simplified event queue example that uses **unions** in a similar way. As always, refer to [this gist](https://gist.github.com/DanielSchuette/eda98376a3b6f750d649d0eb23b16602) or the end of the post if you want to see a full source code listing.

## Setting Up
```c
/* unions.c: demonstrates the use of unions. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define KEYBOARD_EVENT  0
#define MOUSE_EVENT     1
#define WINDOW_EVENT    2
#define NUM_EVENTS      3   /* number of events in this example */
#define MAX_EVENTS      256 /* max length of the event queue */

/* keys are identified via the `KEYS' enum */
enum KEYS { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
```

In this example, we'll handle 3 different event types (i.e. keyboard and mouse input as well as window events). The event queue is going to have an arbitrary length of 256 items max, but in this example there will be only 3 events on the queue to keep things simple. Usually, putting events on the queue would be a dynamic process, but our solution does things statically as you will see later[^2]. Now, every event will be a **struct** with additional event-related data:

```c
/* keyboard event */
typedef struct {
    unsigned int type;
    enum KEYS key;
} keyboard_event;

/* mouse click event */
typedef struct {
    unsigned int type;
    int pos_x;
    int pos_y;
} mouse_event;

/* window event */
typedef struct {
    unsigned int type;
    bool resize;
    bool quit;
} window_event;
```

Notice the first member of every **struct**. Because a **union** can only hold one of (potentially multiple) different data types -- but always with the same memory layout -- we will be able to reuse the **unsigned int type** if we arrange the members in the **union event** correctly. If we didn't do that, writing to e.g. a **struct mouse_event** in the **union** would lead to unpredictable values for every other **union** member (if that's not clear, quickly go back to your K&R and read the section about **union**s again!).


## The Union
Then, how does this **union** look like?

```c
/* common event type */
typedef union {
    unsigned int type;
    keyboard_event key;
    mouse_event mouse;
    window_event window;
} event;
```

Hopefully, the previous explanation actually makes sense now. In the **union event**, the first member is the **unsigned int type** that was also defined in every event **struct**. Thus, if e.g. a **union event** member **mouse_event** is created, its memory layout will be such that accessing the **union**s **unsigned int type** will actually access the memory location that **mouse_event.type** is in, too! This behavior of **union**s lets us not only use memory very effectively; in this example we can also detect the type of event we got from the queue via one member and the actual event-related data from the corresponding other member of **union event**. How neat is that!

## Putting Data on the Queue
The following section should be pretty self-explanatory. A global **static event *queue[MAX_EVENTS]** is accessed and modified via a global **static event **queue_ptr**. **void fill_queue(void)** and **bool get_event(event \*)** are used for queue manipulation.

```c
static event *queue[MAX_EVENTS];
static event **queue_ptr = queue;

/* fill_queue: a mock function that fills the event queue. */
void fill_queue()
{
    event **q_ptr;
    event *ev1, *ev2, *ev3;

    q_ptr = queue_ptr; /* temporary pointer to global queue */

    /* create events and enqueue them */
    ev1 = (event *)malloc(sizeof(event));
    ev1->type = WINDOW_EVENT;
    ev1->window.type = WINDOW_EVENT;
    ev1->window.quit = false;
    ev1->window.resize = true;
    *q_ptr = ev1; /* enqueue event 1 */
    q_ptr++;      /* advance the temporary queue ptr */

    ev2 = (event *)malloc(sizeof(event));
    ev2->type = MOUSE_EVENT;
    ev2->mouse.pos_x = ev2->mouse.pos_y = 42;
    *q_ptr = ev2; /* enqueue event 2 */
    q_ptr++;      /* advance the temporary queue ptr */

    ev3 = (event *)malloc(sizeof(event));
    ev3->type = KEYBOARD_EVENT;
    ev3->key.type = KEYBOARD_EVENT;
    ev3->key.key = KEY_UP;
    *q_ptr = ev3;   /* enqueue event 3 */
    q_ptr++;        /* advance the temporary queue ptr */
    *q_ptr = NULL;  /* this queue is NULL-terminated */
}

bool get_event(event *ev)
{
    if (*queue_ptr == NULL)
        return false;   /* the event queue is empty, return 0 */
    *ev = **queue_ptr;  /* point caller to current event */
    queue_ptr++;        /* advance global queue ptr */
    return true;
}
```

Obviously, this queue implementation is not very usable in the real world[^3]. I might demonstrate the use of a ring buffer in a future post, though.

## Using the Queue
The queue is continuously polled in a loop. For every event, a **switch** statement identifies its type and the appropriate actions can be taken. In this case we just print the event-related data to show that everything works as expected.

```c
int main(void)
{
    event ev;

    fill_queue();
    while (get_event(&ev)) {
        switch (ev.type) {
        case KEYBOARD_EVENT:
            switch (ev.key.key) {
            case KEY_UP:
                fprintf(stderr, "got a keyboard event (up)\n");
                break;
            case KEY_DOWN:
                fprintf(stderr, "got a keyboard event (down)\n");
                break;
            case KEY_LEFT:
                fprintf(stderr, "got a keyboard event (left)\n");
                break;
            case KEY_RIGHT:
                fprintf(stderr, "got a keyboard event (right)\n");
                break;
            }
            break;
        case MOUSE_EVENT:
            fprintf(stderr, "got a mouse event (x=%d, y=%d)\n",
                    ev.mouse.pos_x, ev.mouse.pos_y);
            break;
        case WINDOW_EVENT:
            fprintf(stderr, "got a window event (quit=%d, resize=%d)\n",
                    ev.window.quit, ev.window.resize);
            break;
        default:
            fprintf(stderr, "error: unknown event type %d\n", ev.type);
            break;
        }
    }

    /* FIXME: perform `malloc()' error checking! */
    /* FIXME: free everything on the queue here! */
    return 0;
}
```

## Final Thoughts
I haven't really used **union**s in my own code so far, so it was really helpful to see this use case and the elegant code it produces. If you like this example of C **union**s and/or have questions/concerns related to the code or any explanations, [please send me your feedback via twitter](https://twitter.com/DogtorDash)!

## The Full Example:
<style>
/* make gist look nicer, this really only works when put in manually,
 * probably because of remote theme...
 */
/* body, line numbers, Code, Comments */
.gist-data tbody {
    background-color: Black;
}
.gist-data tbody td:nth-of-type(1) {
    color: #2B91AF !important;
}
.gist-data tbody td:nth-of-type(2){
    color: #FFFFFF !important;
}
.pl-c {
    color: #57A64A !important;
}

/* Function, Function Name, Function Method */
.pl-k, tbody tr:first-child .blob-code, tbody tr:last-child .blob-code {
    color: #569CD6 !important;
}
.pl-en {
    color: #FFFFFF !important;
}
.pl-c1 {
    color: #FFFFFF !important;
}
/* strings, quotes around strings */
.pl-s {
    color: #D69D85 !important;
}
.pl-pds {
    color: #D69D85 !important;
}

/* gist meta*/
.gist-meta {
    display: none !important;
}
</style>
<script src="https://gist.github.com/DanielSchuette/eda98376a3b6f750d649d0eb23b16602.js"></script>

[^1]: This might be different in embedded and systems programming where memory layout and a small footprint are extremely important. There are many cases in which **union**s can be used to e.g. access bytes of a multi-byte integer type:
```c
union access {
    unsigned char bytes[8]; /* can be used to access individual bytes of `number' */
    unsigned long number;
}
```
[^2]: It should be easy to expand the code to enqueue events dynamically, though. Initially, I thought of **getchar()**ing 'events' from the user, but that would have added unnecessary complexity to the example.
[^3]: Not to speak of error checking, which isn't done at all.
