#include <string.h>

char* strcpy(char* destination, const char* src)
{
    const size_t len = strlen(src);
    memcpy(destination, src, len+1);
    return destination;
}

char* strncpy(size_t maxlen, char* destination, const char* src)
{
    const size_t len = strlen(src);
    maxlen -= 1; // null termination
    const size_t effective_len = (len > maxlen) ? maxlen : len;
    memcpy(destination, src, effective_len+1);
    destination[effective_len] = 0;
    return destination;
}
