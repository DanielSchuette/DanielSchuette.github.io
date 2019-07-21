#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define KEYBOARD_EVENT  0
#define MOUSE_EVENT     1
#define WINDOW_EVENT    2
#define NUM_EVENTS      3
#define MAX_EVENTS      256

enum KEYS { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };

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

/* common event type */
typedef union {
    unsigned int type;
    keyboard_event key;
    mouse_event mouse;
    window_event window;
} event;

static event *queue[MAX_EVENTS];
static event **queue_ptr = queue;

void fill_queue();
bool get_event(event *);

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

    /* FIXME: free everything on the queue here! */
    return 0;
}

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
    *q_ptr = ev1;
    q_ptr++;

    ev2 = (event *)malloc(sizeof(event));
    ev2->type = MOUSE_EVENT;
    ev2->mouse.pos_x = ev2->mouse.pos_y = 42;
    *q_ptr = ev2;
    q_ptr++;

    ev3 = (event *)malloc(sizeof(event));
    ev3->type = KEYBOARD_EVENT;
    ev3->key.type = KEYBOARD_EVENT;
    ev3->key.key = KEY_UP;
    *q_ptr = ev3;
    q_ptr++;
    *q_ptr = NULL; /* this queue is NULL-terminated */
}

bool get_event(event *ev)
{
    if (*queue_ptr == NULL)
        return false;
    *ev = **queue_ptr;
    queue_ptr++;
    return true;
}
