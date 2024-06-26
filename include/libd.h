#ifndef LIBD_H
#define LIBD_H
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <curl/curl.h>

#define FALSE 0
#define TRUE 1

typedef unsigned int uint;

const char* err();
const char* warn();
const char* info();

void prep_env();
void init_helper(char *symbol_pair, uint interval);

#endif // !LIBD_H
