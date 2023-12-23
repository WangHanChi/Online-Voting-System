#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* max text line length */
#define MAXLINE 4096
/* maximum number of client connections */
#define LISTENQ 8
/* dir permission */
#define DIR_MODE (S_IRWXU | S_IRWXG | S_IRWXO)

#define DELAY_MS(x)                        \
    do {                                   \
        clock_t start_time = clock();      \
        while (clock() < (start_time + x)) \
            ;                              \
    } while (0);

void server_select_cmd(int, int, char *);
int verify_login(int cli_sockfd, char *user);
