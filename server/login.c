#include "login.h"
#include "error.h"
#include "string_utils.h"

User user_database[MAX_USERS] = {
    {"root", "root"},
    {"computer", "network"},
    // ... add it to 10 :)
};

// Authenticate a user by comparing the provided username and password
bool authenticate_user(const char *username, const char *password)
{
    char cleaned_username[50];
    char cleaned_password[50];

    // Remove leading and trailing spaces and newline characters from username
    // and password
    strcpy(cleaned_username, username);
    trim_string(cleaned_username);

    strcpy(cleaned_password, password);
    trim_string(cleaned_password);

    // Perform authentication
    for (int i = 0; i < MAX_USERS; ++i) {
        if (!strcmp(cleaned_username, user_database[i].username) &&
            !strcmp(cleaned_password, user_database[i].password)) {
            return true;  // Authentication successful
        }
    }
    return false;  // Authentication failed
}
