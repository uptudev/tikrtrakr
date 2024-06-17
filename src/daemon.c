#include "libd.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

/*
 *  Removes the JSON file associated with a cryptocurrency symbol pair.
 *
 *  @param argv Array of command-line arguments
 *  @return void
 *
 *  This function constructs the path to the JSON file based on the symbol pair provided in argv[2]
 *  and removes the file. It is used when the 'clean' command-line option is specified.
 */
static void clean(char **argv) {
    char path[32];
    uint interval = atoi(argv[3]);

    if (interval == 0) {
        fprintf(stderr, "%s Invalid interval; quitting.\n", err());
        return;
    }

    sprintf(path, "/tmp/tikrtrakrd.%s.json", argv[2]);
    remove(path);
}

/*
 *  Starts a daemon process to track the price of a cryptocurrency symbol pair and writes the data to a JSON file.
 *
 *  @param argc Number of command-line arguments
 *  @param argv Array of command-line arguments
 *  @return int Exit status
 *
 *  This program accepts up to three command-line arguments: the symbol pair to track, an optional tracking interval in seconds,
 *  and an optional 'clean' command to remove existing JSON files. If the 'clean' command is specified, the program will remove
 *  the JSON file associated with the symbol pair and exit. Otherwise, it starts a daemon process with the specified or default
 *  tracking interval. The daemon process is responsible for querying the price of the symbol pair and updating the JSON file.
 */
int main(int argc, char **argv) {
    uint interval;

    /* Drop perms; this program doesn't need sudo and often daemons are run as sudo */
    if (getuid() == 0) {
        fprintf(stderr, "%s Dropping privileges...\n", info());
        if (!(setgid(100) == 0 && setuid(1000) == 0)) {
            fprintf(stderr, "%s Failed to drop privileges; don't run this daemon as root\n", err());
            return 1;
        }
    }

    /* Input parsing */
    switch (argc) {
        case 2:
            interval = 5;
            break;
        case 3:
            interval = atoi(argv[2]);
            if (interval == 0) {
                fprintf(stderr, "%s Invalid interval; defaulting to 5\n", warn());
                interval = 5;
            }
            break;
        case 4:
            if (strcmp(argv[1], "clean") == 0) {
                fprintf(stderr, "%s Cleaning up...\n", info());
                clean(argv);
                return 0;
            }
        default:
            fprintf(stderr, "%s Usage: %s <symbol pair> [interval (s)]\n%s Example:\n\t%s XRPUSDT 5\n", err(), argv[0], info(), argv[0]);
            return 1;
    }

    /* Daemon process */
    prep_env();
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Can't fork daemon process\n");
        return 1;
    } else if (pid == 0) {
        // Child process (daemon)
        init_helper(argv[1], interval);
    }

	return 0;
}
