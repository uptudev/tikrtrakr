#include "daemon.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Can't fork daemon process\n");
        return 1;
    } else if (pid == 0) {
        // Child process (daemon)
        init_daemon();
        main_loop();
    }

	return 0;
}
