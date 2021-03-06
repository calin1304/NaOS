#include "console.h"

#include "io.h"
#include "clock.h"

static char hexTable[] = "0123456789ABCDEF";

uint8_t build_console_attribute(Color fg, Color bg)
{
    return (bg << 4) | (fg & 0x0F);
}

void console_init(Console *this)
{
    this->width = DEFAULT_CONSOLE_WIDTH;
    this->height = DEFAULT_CONSOLE_HEIGHT;
    this->cursorX = 0;
    this->cursorY = 0;
    this->fg = COLOR_LIGHT_GRAY;
    this->bg = COLOR_BLACK;
    console_clear(this);
}

void console_update_cursor(Console *this)
{
    uint16_t position = ((this->cursorY * this->width) + this->cursorX);
    outb(0x3d4, 0x0f);
    outb(0x3d5, (uint8_t)(position & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (uint8_t)((position >> 8) & 0xff));
}

void console_put_backspace(Console *this)
{
    if (!this->cursorX) {
        if (this->cursorY > 0) {
            this->cursorY -= 1;
            this->cursorX = this->width-1;
            console_put_char(this, ' ');
            this->cursorY -= 1;
            this->cursorX = this->width-1;
        }
    } else if (this->cursorX > 0) {
        this->cursorX -= 1;
        console_put_char(this, ' ');
        this->cursorX -= 1;
    }
}

void console_put_newline(Console *this)
{
    console_set_cursor_position(this, 0, (this->cursorY+1)%(this->height));
}

void console_advance_cursor(Console *this)
{
    this->cursorX += 1;
    if (this->cursorX == this->width) {
        this->cursorX = 0;
        this->cursorY += 1;
        if (this->cursorY == this->height) {
            this->cursorY = 0;
        }
    }
    console_update_cursor(this);
}

void console_put_char(Console *this, char c)
{
    if (c == '\n') {
        console_put_newline(this);
        return;
    }
    if (c == '\b') {
        console_put_backspace(this);
        return;
    }
    uchar *vga = (uchar*)0x000b8000;
    vga[(this->cursorX + this->cursorY * this->width)*2] = c;
    vga[(this->cursorX + this->cursorY * this->width)*2 + 1] = build_console_attribute(this->fg, this->bg);
    console_advance_cursor(this);
}

void console_put_string(Console *this, const char *s)
{
    while (*s) {
        console_put_char(this, *s);
        ++s;
    }
}

void console_set_cursor_position(Console *this, uint x, uint y)
{
    this->cursorX = x;
    this->cursorY = y;
    console_update_cursor(this);
}

void console_clear(Console *this)
{
    Color bg_ = this->bg;
    this->bg = COLOR_BLACK;
    for (uint i = 0; i < this->height; ++i) {
        for (uint j = 0; j < this->width; ++j) {
            console_put_char(this, ' ');
        }
    }
    console_set_cursor_position(this, 0, 0);
    this->bg = bg_;
}

void console_put_int_hex(Console *this, uint32_t val)
{
    console_put_string(this, "0x");
    uint i = 0;
    // while (i < 8 && (val & 0xf0000000) == 0) {
    //     val <<= 4;
    //     i++;
    // }
    // if (!val) {
    //     console_put_char(this, '0');
    //     return;
    // }
    for (; i < 8; ++i, val <<= 4) {
        uint8_t c = hexTable[(val & 0xf0000000) >> 28];
        console_put_char(this, c);
    }
}

void console_put_int_dec(Console *this, uint32_t val)
{
    if (val == 0) {
        console_put_char(this, '0');
        return;
    }
    char v[10];
    int i = 0;
    while (val) {
        v[i++] = (val%10)+'0';
        val /= 10;
    }
    while (--i >= 0) {
        console_put_char(this, v[i]);
    }
}

void console_printf(Console *this, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    console_vprintf(this, fmt, args);
    va_end(args);
}

void console_vprintf(Console *this, const char *fmt, va_list args)
{
    const char *p;
    for (p = fmt; *p != '\0'; ++p) {
        if (*p == '%') {
            char t = *(++p);
            if (t == 'x' || t == 'p') {
                console_put_int_hex(this, va_arg(args, uint32_t));
            } else if (t == 'd') {
                console_put_int_dec(this, va_arg(args, uint32_t));
            } else if (t == 's') {
                console_put_string(this, va_arg(args, const char *));
            } else if (t == 'c') {
                console_put_char(this, va_arg(args, int));
            } else if ('t' == '%') {
                console_put_char(this ,'%');
            }
        } else if (*p == '\\') {
            char t = *(++p);
            if (t == 'n') {
                console_put_newline(this);
            } else if (t == '\\') {
                console_put_char(this, '\\');
            }
        } 
        else {
            console_put_char(this, *p);
        }
    }
}

void console_display_timer(Console *this)
{
    uint x = this->cursorX;
    uint y = this->cursorY;
    console_set_cursor_position(this, DEFAULT_CONSOLE_WIDTH/2, 0);
    console_printf(this, "Seconds: %x", clock.seconds);
    // console_put_int_hex(this, clock.ticks);
    console_set_cursor_position(this, x, y);
}