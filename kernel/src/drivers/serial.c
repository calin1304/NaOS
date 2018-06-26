#include "drivers/serial.h"

#include <stdint.h>
#include <stdarg.h>

#include "io.h"

void serialPortInit(SerialPort *r, int com, int baudRate)
{
    r->com = com;
    serialPortSetBaudRate(r, baudRate);
    serialPortConfigureLine(r);
}

void serialPortSetBaudRate(SerialPort *r, int rate)
{
    short divisor = SERIAL_CLOCK/rate;
    outb(SERIAL_LINE_COMMAND_PORT(r->com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(r->com), (divisor >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(r->com), divisor & 0x00FF);
}

void serialPortConfigureLine(SerialPort *r)
{
    outb(SERIAL_LINE_COMMAND_PORT(r->com), 0x03);
    outb(SERIAL_FIFO_COMMAND_PORT(r->com), 0xC7);
    outb(SERIAL_MODEM_COMMAND_PORT(r->com), 0x03);
}

void serialPortWriteByte(SerialPort *r, uint8_t c)
{
    outb(SERIAL_DATA_PORT(r->com), c);
}

void serialPortWriteBytes(SerialPort *r, uint8_t *s, int count)
{
    while (count) {
        serialPortWriteByte(r, *s);
        count -= 1;
        s += 1;
    }
}

void serialPortWriteString(SerialPort *r, const char *s)
{
    while (*s) {
        serialPortWriteByte(r, *s);
        s += 1;
    }
}

void serialPortWriteStringNL(SerialPort *r, const char *s)
{
    serialPortWriteString(r, s);
    serialPortWriteByte(r, '\n');
}

void serialPortPrintf(SerialPort *r, const char *format, ...)
{
    char out[100];
    va_list args;
    va_start(args, format);
    vsprintf(out, format, args); //FIXME: Use vsnprintf
    serialPortWriteString(r, out);
    va_end(args);
}
