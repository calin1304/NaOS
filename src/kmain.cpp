#include <stdint.h>

#define DEFAULT_CONSOLE_WIDTH   80
#define DEFAULT_CONSOLE_HEIGHT  25

struct GDT {
    uint16_t    limit;
    uint16_t    base;
    uint8_t     base2;
    uint8_t     access;
    uint8_t     limit2  : 4;
    uint8_t     flags   : 4;
    uint8_t     base3;
} __attribute__((packed));

enum Color
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
};

char hexTable[] = "0123456789ABCDEF";

void itos(unsigned int x, unsigned char out[9]) 
{
    for (int i = 7; i >= 0; --i) {
        unsigned int a = x & 0xf;
        out[i] = hexTable[a];
        x >>= 4;
    }
    out[8] = '\0';
}

uint8_t getAttribute(enum Color fg, enum Color bg)
{
    return (bg << 4) | (fg & 0x0F);
}

inline unsigned char inb(unsigned int port)
{
    unsigned char ret;
    asm volatile("inb %%dx, %%al" : "=a" (ret): "d" (port));
    return ret;
}

inline void outb(unsigned int port, unsigned char value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

class Console {
public:
    Console() {
        width = DEFAULT_CONSOLE_WIDTH;
        height = DEFAULT_CONSOLE_HEIGHT;
        cursorPosX = 0;
        cursorPosY = 0;
        fg = COLOR_LIGHT_GRAY;
        bg = COLOR_BLACK;
        clear();
    }
    void putChar(char c) {
        unsigned char *vga = (unsigned char*)0x000b8000;
        vga[(cursorPosX + cursorPosY * width)*2] = c;
        vga[(cursorPosX + cursorPosY * width)*2 + 1] = getAttribute(fg, bg);
        advanceCursor();
    }

    void putString(const char *s) {
        while (*s != '\0') {
            if (*s == '\n') {
                newLine();
            } else {
                putChar(*s);
            }
            s += 1;
        }
    }

    void putHex(unsigned char value) {
        putChar(hexTable[(value & 0xf0) >> 4]);
        putChar(hexTable[value & 0x0f]);
    }

    void newLine() {
        setCursorPosition(0, (cursorPosY+1)%height);
    }

    void clear() {
        Color bg_ = bg;
        setBackgroundColor(COLOR_BLACK);
        for (unsigned char i = 0; i < height; ++i) {
            for (unsigned char j = 0; j < width; ++j) {
                putChar(0);
            }
                
        }
        setCursorPosition(0, 0);
        setBackgroundColor(bg_);
    }

    void setCursorPosition(int x, int y) {
        cursorPosX = x;
        cursorPosY = y;
        updateCursor();
    }

    void setForegroundColor(Color color) {
        fg = color;
    }

    void setBackgroundColor(Color color) {
        bg = color;
    }

    // unsigned char getChar() {
    //     unsigned char ret = inb(0x64) & 1;
    //     while (ret == 0) {
    //         ret = inb(0x64) & 1;
    //     }
    //     ret = inb(0x60);
    //     return ret;
    // }

private:
    unsigned int width;
    unsigned int height;
    unsigned int cursorPosX;
    unsigned int cursorPosY;

    Color fg;
    Color bg;

    void advanceCursor() {
        cursorPosX += 1;
        if (cursorPosX == width) {
            cursorPosX = 0;
            cursorPosY += 1;
            if (cursorPosY == height) {
                cursorPosY = 0;
            }
        }
        updateCursor();
    }
    void updateCursor() {
        // outb(0x3d4, 14);
        // outb(0x3d5, cursorPosY);
        // outb(0x3d4, 15);
        // outb(0x3d5, cursorPosX);
    }
};

// #define SERIAL_COM1_BASE                    0x3f8
// #define SERIAL_DATA_PORT(base)              (base)
// #define SERIAL_FIFO_COMMAND_PORT(base)      (base+2)
// #define SERIAL_LINE_COMMAND_PORT(base)      (base+3)
// #define SERIAL_MODEM_COMMAND_PORT(base)     (base+4)
// #define SERIAL_LINE_STATUS_PORT(base)       (base+5)

// #define SERIAL_LINE_ENABLE_DLAB 0x80

// #define SERIAL_CLOCK 115200

// class SerialPort {
// public:
//     SerialPort(unsigned int com) {
//         this->com = com;
//         setBaudRate(57600);
//         configureLine();
//     }
//     void write(unsigned char c) {
//         outb(SERIAL_DATA_PORT(com), c);
//         // if (isTransmitFIFOEmpty()) {
//         //     outb(SERIAL_DATA_PORT(com), c);
//         // }
//     }
//     void write(const unsigned char s[]) {
//         while (*s) {
//             write((unsigned char)*s);
//             s += 1;
//         }
//     }
//     void write(unsigned int x) {
//         unsigned char s[9];
//         itos(x, s);
//         write(s);
//     }
// private:
//     unsigned int com;

//     void setBaudRate(unsigned int baudRate) {
//         unsigned short divisor = SERIAL_CLOCK/baudRate;
//         outb(SERIAL_LINE_COMMAND_PORT(com),
//              SERIAL_LINE_ENABLE_DLAB);
//         outb(SERIAL_DATA_PORT(com),
//              (divisor >> 8) & 0x00FF);
//         outb(SERIAL_DATA_PORT(com),
//              divisor & 0x00FF);
//     }
//     void configureLine() {
//         outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
//         outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
//         outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
//     }
//     bool isTransmitFIFOEmpty() {
//         return inb(SERIAL_LINE_STATUS_PORT(com) & 0x20);
//     }
// };

extern "C" void main (void) 
{
    Console c;  
    c.putString("[#] Kernel running\n");
    for(;;);
}
