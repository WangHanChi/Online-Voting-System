#include "syslog.h"

#define SYSLOG_FILENAME "syslog.txt"

struct timeval tv;
time_t t;
struct tm *info;

void syslog_init()
{
    FILE *fd;
    fd = fopen(SYSLOG_FILENAME, OVERWRITE);
    fprintf(fd, "FTP start recording !\n\n");
    fclose(fd);
}

void syslog_record(char *str, char *user)
{
    FILE *fd;
    fd = fopen(SYSLOG_FILENAME, APPEND);
    gettimeofday(&tv, NULL);
    t = tv.tv_sec;
    info = localtime(&t);
    fprintf(fd, "%suser : %s \t\t|request op %s", asctime(info), user, str);
    fprintf(fd, "----------------------------------------------------------\n");
    fclose(fd);
}