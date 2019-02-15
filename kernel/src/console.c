#include "console.h"

#include "io.h"
#include "clock.h"

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

static char hexTable[] = "0123456789ABCDEF";

uint8_t build_console_attribute(Color fg, Color bg)
{
    return (bg << 4) | (fg & 0x0F);
}

void console_init()
{
    console.width = DEFAULT_CONSOLE_WIDTH;
    console.height = DEFAULT_CONSOLE_HEIGHT;
    console.cursorX = 0;
    console.cursorY = 0;
    console.fg = COLOR_LIGHT_GRAY;
    console.bg = COLOR_BLACK;
    console_clear();
}

void console_update_cursor()
{
    uint16_t position = ((console.cursorY * console.width) + console.cursorX);
    outb(0x3d4, 0x0f);
    outb(0x3d5, (uint8_t)(position & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (uint8_t)((position >> 8) & 0xff));
}

void console_put_backspace()
{
    if (!console.cursorX) {
        if (console.cursorY > 0) {
            console.cursorY -= 1;
            console.cursorX = console.width-1;
            console_put_char(' ');
            console.cursorY -= 1;
            console.cursorX = console.width-1;
        }
    } else if (console.cursorX > 0) {
        console.cursorX -= 1;
        console_put_char(' ');
        console.cursorX -= 1;
    }
}

void console_put_newline()
{
    console_set_cursor_position(0, (console.cursorY+1)%(console.height));
}

void console_advance_cursor()
{
    console.cursorX += 1;
    if (console.cursorX == console.width) {
        console.cursorX = 0;
        console.cursorY += 1;
        if (console.cursorY == console.height) {
            console.cursorY = 0;
        }
    }
    console_update_cursor();
}

void console_put_char(char c)
{
    if (c == '\n') {
        console_put_newline();
        return;
    }
    if (c == '\b') {
        console_put_backspace();
        return;
    }
    uchar *vga = (uchar*)0x000b8000;
    vga[(console.cursorX + console.cursorY * console.width)*2] = c;
    vga[(console.cursorX + console.cursorY * console.width)*2 + 1] = build_console_attribute(console.fg, console.bg);
    console_advance_cursor();
}

void console_put_string(const char *s)
{
    while (*s) {
        console_put_char(*s);
        ++s;
    }
}

void console_set_cursor_position(uint x, uint y)
{
    console.cursorX = x;
    console.cursorY = y;
    console_update_cursor();
}

void console_clear()
{
    Color bg_ = console.bg;
    console.bg = COLOR_BLACK;
    for (uint i = 0; i < console.height; ++i) {
        for (uint j = 0; j < console.width; ++j) {
            console_put_char(' ');
        }
    }
    console_set_cursor_position(0, 0);
    console.bg = bg_;
}

void console_put_int_hex(uint32_t val)
{
    console_put_string("0x");
    uint i = 0;
    // while (i < 8 && (val & 0xf0000000) == 0) {
    //     val <<= 4;
    //     i++;
    // }
    // if (!val) {
    //     console_put_char('0');
    //     return;
    // }
    for (; i < 8; ++i, val <<= 4) {
        uint8_t c = hexTable[(val & 0xf0000000) >> 28];
        console_put_char(c);
    }
}

void console_put_int_dec(uint32_t val)
{
    if (val == 0) {
        console_put_char('0');
        return;
    }
    char v[10];
    int i = 0;
    while (val) {
        v[i++] = (val%10)+'0';
        val /= 10;
    }
    while (--i >= 0) {
        console_put_char(v[i]);
    }
}

void console_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    console_vprintf(fmt, args);
    va_end(args);
}

void console_vprintf(const char *fmt, va_list args)
{
    const char *p;
    for (p = fmt; *p != '\0'; ++p) {
        if (*p == '%') {
            char t = *(++p);
            if (t == 'x' || t == 'p') {
                console_put_int_hex(va_arg(args, uint32_t));
            } else if (t == 'd') {
                console_put_int_dec(va_arg(args, uint32_t));
            } else if (t == 's') {
                console_put_string(va_arg(args, const char *));
            } else if (t == 'c') {
                console_put_char(va_arg(args, int));
            } else if ('t' == '%') {
                console_put_char('%');
            }
        } else if (*p == '\\') {
            char t = *(++p);
            if (t == 'n') {
                console_put_newline();
            } else if (t == '\\') {
                console_put_char('\\');
            }
        } 
        else {
            console_put_char(*p);
        }
    }
}

void console_display_timer()
{
    uint x = console.cursorX;
    uint y = console.cursorY;
    console_set_cursor_position(DEFAULT_CONSOLE_WIDTH/2, 0);
    console_printf("Seconds: %x", clock.seconds);
    // console_put_int_hex(clock.ticks);
    console_set_cursor_position(x, y);
}