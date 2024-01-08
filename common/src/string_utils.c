#include <string.h>

#include "string_utils.h"

// Remove leading and trailing spaces and newline characters from a string
void trim_string(char *str)
{
    size_t len = strlen(str);

    // Remove leading spaces and newline characters
    size_t start = 0;
    while (start < len && (str[start] == ' ' || str[start] == '\n')) {
        ++start;
    }

    // Remove trailing spaces and newline characters
    size_t end = len - 1;
    while (end > start && (str[end] == ' ' || str[end] == '\n')) {
        --end;
    }

    // Move non-empty characters to the beginning of the string
    size_t i;
    for (i = start; i <= end; ++i) {
        str[i - start] = str[i];
    }

    // Add the string terminator
    str[i - start] = '\0';
}
