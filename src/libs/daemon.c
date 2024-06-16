#include "daemon.h"

volatile int TERM = FALSE;

/* Daemon initialization */

/*
 *  Kills the daemon process when a termination signal is received.
 *
 *  @param signum Signal number
 *  @return void
 *
 *  Should be used via signal(2) to handle SIGINT and SIGTERM.
 */
static void handle_termination(int signum) {
    TERM = TRUE;
}

/*
 *  Closes all file descriptors except for stdin, stdout, and stderr.
 *
 *  @return TRUE if successful, FALSE if getrlimit(2) fails
 *
 *  This function is used to prevent the daemon from holding open inherited file descriptors.
 */
static uint close_fds() {
    uint i;
    struct rlimit rlim;
    int fds = getrlimit(RLIMIT_NOFILE, &rlim);

    if (fds == -1) {
        return FALSE;
    }

    for (i = 3; i < fds; i++) {
        close(i);
    }

    return TRUE;
}

/*
 *  Initializes the daemon process.
 *
 *  @return void
 *
 *  This function should be called before the main loop of the daemon process.
 *  It does all the necessary setup to ensure the daemon is properly isolated from the parent process.
 */
void init_daemon() {
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);
    switch (close_fds()) {
        case TRUE:
            break;
        case FALSE:
            fprintf(stderr, "Can't close file descriptors; daemon creation unsafe.\n");
            exit(1);
    }
}

/* Main loop */

/*
 *  Main loop of the daemon process.
 *
 *  @return void
 *
 *  This function should be called after init_daemon() to start the main loop of the daemon process.
 */
void main_loop() {
    while (!TERM) {
        puts("Hello, World!");
        sleep(5);
    }
}
