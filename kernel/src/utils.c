#include "utils.h"

int min(int a, int b)
{
    return a < b ? a : b;
}

unsigned int oct_to_dec(unsigned int oct)
{
    unsigned int ret = 0;
    for (int i = 0; oct; ++i, oct /= 10) {
        ret+= ((oct%10) << (3*i));
    }
    return ret;
}

unsigned int stoin(const char *s, int n)
{
    unsigned int ret = 0;
    for (int i = 0; i < n; ++i) {
        ret = ret * 10 + (s[i] - '0');
    }
    return ret;
}