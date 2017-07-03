#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stdarg.h>

#include "defines.h"
#include "string.h"

#define DEFAULT_CONSOLE_WIDTH   80
#define DEFAULT_CONSOLE_HEIGHT  25

typedef enum Color_
{
    COLOR_BLACK = 0,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_LIGHT_GRAY,
    COLOR_DARK_GRAY,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_CYAN,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE
} Color;

typedef struct Console_ {
    uint width;
    uint height;
    uint cursorX;
    uint cursorY;
    Color fg;
    Color bg;
} Console;

Console console;

void console_init(Console *this);

void console_advance_cursor(Console *this);
void console_set_cursor_position(Console *this, uint x, uint y);

void console_put_char(Console *this, char c);
void console_put_backspace(Console *this);
void console_put_newline(Console *this);
void console_put_string(Console *this, const char *s);
void console_put_int_hex(Console *this, uint32_t val);
void console_printf(Console *this, const char *fmt, ...);
void console_vprintf(Console *this, const char *fmt, va_list args);
void console_display_timer(Console *this);

void console_clear(Console *this);

#endif