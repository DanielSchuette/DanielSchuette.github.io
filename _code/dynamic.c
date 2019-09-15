#include <stdio.h>

#define ROWS 3
#define COLS 4

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int grid[ROWS][COLS] = {
    { 3, 2, 1, 3 },
    { 1, 9, 2, 3 },
    { 9, 1, 5, 4 }
};

void dump_arr(const int arr[ROWS][COLS])
{
    int i, j;

    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            printf("%d ", arr[i][j]);
        }
        putc('\n', stdout);
    }
}

int dynamic(int grid[ROWS][COLS])
{
    int i, j;

    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            if (i == 0 && j == 0) /* first tile */
                continue;
            else if (i == 0) /* only look left */
                grid[i][j] += grid[i][j-1];
            else if (j == 0) /* only look up */
                grid[i][j] += grid[i-1][j];
            else
                grid[i][j] += MIN(grid[i-1][j], grid[i][j-1]);
        }
    }

    dump_arr(grid);
    return grid[ROWS-1][COLS-1];
}

int main(void)
{
    printf("solution: %d\n", dynamic(grid));

    return 0;
}
