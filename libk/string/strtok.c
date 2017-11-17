#include "libk/include/string.h"

char* strtok(char *s, const char *delims)
{
    static char* currPos;
    static char* stringEnd;
    if (s != NULL) {
        currPos = s;
        stringEnd = s;
        while (*stringEnd != '\0') {
            if (strchr(delims, *stringEnd)) {
                *stringEnd = '\0';
            }
            stringEnd += 1;
        }
    }
    while (*currPos != '\0') {
        currPos += 1;
    }
    while (*currPos == '\0' && currPos < stringEnd) {
        currPos += 1;
    }
    if (currPos == stringEnd){
        return NULL;
    }
    return currPos;
}