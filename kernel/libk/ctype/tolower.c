#include "ctype.h"

int tolower(int c)
{
    char ch = (char)c;
    if (ch >= 'A' && ch <= 'Z') {
        return (int)(ch - 'A' + 'a');
    }
    return c;
}