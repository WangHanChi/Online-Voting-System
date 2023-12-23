#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define OVERWRITE "w"
#define APPEND "a"

void syslog_init();
void syslog_record(char *str, char *user);