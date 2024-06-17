#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "libd.h"

/*
 *  Trims leading and trailing zeroes from a string.
 *
 *  @param str Pointer to the string to trim
 *  @return void
 *
 *  This function modifies the input string in place.
 */
void trim_zeroes(char *str) {
    int n = strspn(str, "0");
    if (n > 0) {
        str += n;
    }

    char *end = strchr(str, '\0');
    while ((end -= 1) >= str && *end == '0');
    *(end + 1) = '\0';
}

/*
 *  Reads and displays the price of a cryptocurrency symbol pair from a JSON file.
 *
 *  @param argc Number of command-line arguments
 *  @param argv Array of command-line arguments
 *  @return int Exit status
 *
 *  This program expects one command-line argument: the symbol pair to query.
 *  It reads the corresponding JSON file and extracts the price, trimming any leading or trailing zeroes.
 *  If the file does not exist or contains invalid JSON, an error message is displayed.
 */
int main(int argc, char **argv) {
    // Input parsing
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

    // Parse JSON in file
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
    trim_zeroes(token);
    printf("%s\n", token);
    return 0;
}
