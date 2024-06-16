#ifndef DAEMON_H
#define DAEMON_H
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#define FALSE 0;
#define TRUE 1;

typedef unsigned int uint;

void init_daemon();
void main_loop();

#endif // !DAEMON_H
