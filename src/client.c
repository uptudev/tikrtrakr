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
    sprintf(filename, "/tmp/tikrtrakrd.%s.json", argv[1]);

    FILE *fd = fopen(filename, "r");
    if (fd == NULL) {
        fprintf(stderr, "%s File /tmp/tikrtrakrd.%s.json doesn't exist; please start the daemon with `tikrtrakr %s`\n", err(), argv[1], argv[1]);
        return 1;
    }

    /* Parse JSON in file */
    char buffer[1024];
    fread(buffer, 1, 1024, fd);
    return 0;
}
