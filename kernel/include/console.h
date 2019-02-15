#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stdarg.h>

#include "defines.h"
#include <string.h>

void console_init();

void console_advance_cursor();
void console_set_cursor_position(uint x, uint y);

void console_put_char(char c);
void console_put_backspace();
void console_put_newline();
void console_put_string(const char *s);
void console_put_int_hex(uint32_t val);
void console_printf(const char *fmt, ...);
void console_vprintf(const char *fmt, va_list args);
void console_display_timer();

void console_clear();

#endif