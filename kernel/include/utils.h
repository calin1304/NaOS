#ifndef UTILS_H
#define UTILS_H

int min(int, int);

#define UNUSED(x) (void)(x)

#define GET_BIT(x, n) (((x) >> (n)) & 1) // TODO: Remove. Duplicate of TEST_BIT
#define SET_BIT(x, i) ((x) | (1 << (i)))
#define UNSET_BIT(x, i) ((x) & (~(1 << (i))))
#define TEST_BIT(x, i) ((x) & (1 << (i)))

#endif