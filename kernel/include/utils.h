#ifndef UTILS_H
#define UTILS_H

int min(int, int);

#define UNUSED(x) (void)(x)

#define GET_BIT(x, n) (((x) >> (n)) & 1) // TODO: Remove. Duplicate of TEST_BIT
#define SET_BIT(x, i) ((x) | (1 << (i)))
#define UNSET_BIT(x, i) ((x) & (~(1 << (i))))
#define TEST_BIT(x, i) ((x) & (1 << (i)))

#define LOG(format, ...) printf("[#] %s:%d (%s) : "format"\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);

unsigned int oct_to_dec(unsigned int oct);
unsigned int stoi(const char *s);

#endif