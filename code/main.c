#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    pid_t  pid;

    if ((pid = fork()) < 0)
        exit(1);
    if (pid == 0) {
        for (;;)
            fprintf(stderr, "child: pid=%d\n", getpid());
    } else {
        sleep(2);
        fprintf(stderr, "parent: killing child\n");
        kill(pid, 9);
    }
}
