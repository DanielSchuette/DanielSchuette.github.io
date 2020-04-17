#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TNODE {
    struct TNODE *left;
    struct TNODE *right;
    int           data;
} node_t;

node_t *tree_add(node_t *root, int val)
{
    node_t *new, *tptr;

    new = (node_t *)malloc(sizeof(node_t));
    new->left = new->right = NULL;
    new->data = val;

    if (!root)
        root = new;
    else {
        tptr = root;
        while (true) {
            if (tptr->data <= val) {
                if (tptr->right) tptr = tptr->right;
                else {
                    tptr->right = new;
                    break;
                }
            } else {
                if (tptr->left) tptr = tptr->left;
                else {
                    tptr->left = new;
                    break;
                }
            }
        }
    }

    return root;
}

void subtree_print(node_t **queue, int len)
{
    int i, new_len;
    node_t **new_queue;

    new_len = 0;
    new_queue = (node_t **)malloc(sizeof(node_t *)*2*len);
    for (i = 0; i < len; i++) {
        fprintf(stderr, "%d ", queue[i]->data);
        if (queue[i]->left)
            new_queue[new_len++] = queue[i]->left;
        if (queue[i]->right)
            new_queue[new_len++] = queue[i]->right;
    }
    fprintf(stderr, "\n");

    if (new_len) subtree_print(new_queue, new_len);
    free(new_queue);
}

void tree_print(node_t *root)
{
    int len;
    node_t **queue;


    len = 0;
    queue = (node_t **)malloc(sizeof(node_t *));
    queue[len++] = root;

    fprintf(stderr, "tree:\n");
    subtree_print(queue, len);
    free(queue);
}

void tree_free(node_t *subtree)
{
    node_t *tptr = subtree;

    if (!tptr) return;
    if (tptr->left) tree_free(tptr->left);
    if (tptr->right) tree_free(tptr->right);
    free(tptr);
}

int main(void)
{
    int c, val;
    char buf[BUFSIZ], *bufptr;
    node_t *root;

    root = NULL;
    memset((void *)buf, '\0', BUFSIZ);
    bufptr = buf;
    while ((c = fgetc(stdin)) != EOF) {
        if (bufptr - buf >= BUFSIZ) {
            fprintf(stderr, "error: input buffer overflow\n");
            exit(1);
        }

        *bufptr++ = c;
        if (c == '\n' || c == '\r') {
            *--bufptr = '\0';
            val = atoi(buf);
            root = tree_add(root, val);

            memset((void *)buf, '\0', BUFSIZ); /* to be save */
            bufptr = buf;
        }
    }
    tree_print(root);
    tree_free(root);

    return 0;
}
