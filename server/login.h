#pragma once

#include <stdbool.h>
#include <string.h>

#define MAX_USERS 10

// struct to store user information
typedef struct {
    char username[50];
    char password[50];
} User;

bool authenticate_user(const char *username, const char *password);
void trim_string(char *str);
