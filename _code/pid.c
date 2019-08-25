#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    /* Change the file mode mask */
    //umask(0);

    /* Create a new SID for the child process */
    //sid = setsid();
    //if (sid < 0) exit(EXIT_FAILURE);

    /* Change the current working directory */
    if ((chdir("/")) < 0) exit(EXIT_FAILURE);

    while (1) {
        fprintf(stderr, "log: doing stuff\n");
        sleep(2);
    }

    return 0;
}
