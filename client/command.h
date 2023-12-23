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
/* max vote event number */
#define MAX_VOTE_EVENTS 10
#define MAX_CANDIDATES 10
#define MAX_NAME_LENGTH 50

#define DELAY_MS(x)                        \
    do {                                   \
        clock_t start_time = clock();      \
        while (clock() < (start_time + x)) \
            ;                              \
    } while (0);

/* userinfo struct */
typedef struct {
    char username[50];
    uint8_t menu_stage;
    char sendline[MAX_NAME_LENGTH];
    int sockfd;
    char server_addr[16];
    int port;
} UserInfo_t;

void client_select_cmd(UserInfo_t *);