#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#define DEBUG_INFO(format, ...)                                     \
    do {                                                            \
        fprintf(stderr, "(%s:%d) " format "\n", __FILE__, __LINE__, \
                ##__VA_ARGS__);                                     \
    } while (0);
#else
#define DEBUG_INFO(format, ...) \
    do {                        \
    } while (0);
#endif

// TODO : define error number
#define ERROR_INFO(format, ...)                                     \
    do {                                                            \
        fprintf(stderr, "(%s:%d) " format "\n", __FILE__, __LINE__, \
                ##__VA_ARGS__);                                     \
        exit(1);                                                    \
    } while (0);
