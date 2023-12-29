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

#include "proto.h"

#define DELAY_MS(x)                        \
    do {                                   \
        clock_t start_time = clock();      \
        while (clock() < (start_time + x)) \
            ;                              \
    } while (0);


void client_select_cmd(UserInfo_t *);