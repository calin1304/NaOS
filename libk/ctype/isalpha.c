#include "libk/include/ctype.h"

int isalpha(int c)
{
    char ch = (char)c;
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}