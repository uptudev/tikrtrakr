#include <unistd.h>
#include <stdio.h>
#include "daemon.h"

int main(int argc, char **argv) {

    /* Parse input */
    if (argc != 2) {
        fprintf(stderr, "%s Usage: %s <symbol pair>\n%s Example:\n\t%s XRPUSDT\n", err(), argv[0], info(), argv[0]);
        return 1;
    }
    char filename[32];
    sprintf(filename, "/tmp/tictrackd.%s.json", argv[1]);

    FILE *fd = fopen(filename, "r");
    if (fd == NULL) {
        fprintf(stderr, "%s File /tmp/tictrackd.%s doesn't exist; please start the daemon with `tictrack %s`\n", err(), argv[1], argv[1]);
        return 1;
    }
    fprintf(stderr, "%s File /tmp/tictrackd.%s exists\n", info(), argv[1]); 
    return 0;
}
