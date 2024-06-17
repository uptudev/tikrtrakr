#include <unistd.h>
#include <stdio.h>
#include "libd.h"

int main(int argc, char **argv) {
    /* Parse input */
    if (argc != 2) {
        fprintf(stderr, "%s Usage: %s <symbol argv[1]>\n%s Example:\n\t%s XRPUSDT\n", err(), argv[0], info(), argv[0]);
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
    fclose(fd);
    char *token = strtok(buffer, "\"");
    if (token == NULL) {
        fprintf(stderr, "%s Invalid JSON in file /tmp/tikrtrakrd.%s.json\n", err(), argv[1]);
        return 1;
    }
    while (strcmp(token, "price") != 0) {
        token = strtok(NULL, "\"");
        if (token == NULL) {
            fprintf(stderr, "%s Invalid JSON in file /tmp/tikrtrakrd.%s.json\n", err(), argv[1]);
            exit(1);
        }
    }
    token = strtok(NULL, "\"");
    if (token == NULL) {
        fprintf(stderr, "%s Invalid JSON in file /tmp/tikrtrakrd.%s.json\n", err(), argv[1]);
        exit(1);
    }
    token = strtok(NULL, "\"");
    if (token == NULL) {
        fprintf(stderr, "%s Invalid JSON in file /tmp/tikrtrakrd.%s.json\n", err(), argv[1]);
        exit(1);
    }
    printf("%s\n", token);
    return 0;
}
