#include "string.h"

#include "libc/include/string.h"

// String string_new()
// {
//     String ret;
//     ret.data[0] = '\0';
//     ret.size = 0;
//     return ret;
// }

// void string_append_char(String *this, char c)
// {
//     this->data[this->size++] = c;
//     this->data[this->size] = '\0';
// }

// void string_append_asciiz(String *this, const char *s)
// {
//     strcpy(this->data, s);
//     this->size += strlen(s);
// }

// void string_append_string(String *this, const String *other)
// {
//     string_append_asciiz(this, other->data);
// }

// void string_clear(String *this)
// {
//     this->data[0] = '\0';
//     this->size = 0;
// }