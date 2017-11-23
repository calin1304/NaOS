#include "ctype.h"

int toupper(int c)
{
    char ch = (char)c;
    if (ch >= 'a' && ch <= 'z') { 
        return (int)(ch - 'a' + 'A');
    }
    return c;
}