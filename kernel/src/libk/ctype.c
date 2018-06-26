#include <libk/ctype.h>

int isalpha(int c)
{
    char ch = (char)c;
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

int tolower(int c)
{
    char ch = (char)c;
    if (ch >= 'A' && ch <= 'Z') {
        return (int)(ch - 'A' + 'a');
    }
    return c;
}

int toupper(int c)
{
    char ch = (char)c;
    if (ch >= 'a' && ch <= 'z') { 
        return (int)(ch - 'a' + 'A');
    }
    return c;
}