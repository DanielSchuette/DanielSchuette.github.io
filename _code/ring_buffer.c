#include <stdio.h>
#include <stdlib.h>

#define RING_BUF_CAPACITY 4

typedef struct {
    char *head;             /* head ptr */
    unsigned int cap;       /* buffer capacity */
    char *read;             /* start of valid data */
    char *write;            /* end of valid data, if read==write buf is empty */
} ring_buffer_t;

void fail(const char *msg, int code)
{
    perror(msg);
    exit(code);
}

ring_buffer_t *make_ring_buffer(unsigned int capacity)
{
    ring_buffer_t *buf;

    buf = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));
    if (!buf) return NULL;

    buf->cap = capacity;
    buf->head = (char *)calloc(buf->cap, sizeof(char));
    buf->read = buf->write = buf->head;

    return buf;
}

void write_ring_buffer(ring_buffer_t *buf, char val)
{
    /* TODO: implement */
}

int read_ring_buffer(ring_buffer_t *buf, char *ret)
{
    /* TODO: implement */
    return 0;
}

int main(void)
{
    char c;
    int i;
    ring_buffer_t *buf;

    buf = make_ring_buffer(RING_BUF_CAPACITY);
    if (!buf) fail("too little memory", 1);

    for (i = 0; i < buf->cap*2; i++)
        write_ring_buffer(buf, (char)i+64);

    for (i = 0; i < buf->cap*3; i++)
        if (!read_ring_buffer(buf, &c))
            fprintf(stderr, "read %c\n", c);

    return 0;
}
