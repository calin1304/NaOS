#include "drivers/serial.h"

#include <stdint.h>
#include <stdarg.h>
#include <utils.h>

#include "io.h"

#define SERIAL_DATA_PORT(base)              (base)
#define SERIAL_FIFO_COMMAND_PORT(base)      (base+2)
#define SERIAL_LINE_COMMAND_PORT(base)      (base+3)
#define SERIAL_MODEM_COMMAND_PORT(base)     (base+4)
#define SERIAL_LINE_STATUS_PORT(base)       (base+5)

#define SERIAL_LINE_DLAB 0x80

#define SERIAL_CLOCK 115200

void serial_init(uint16_t port, uint16_t baudRate)
{
    uint16_t divisor = SERIAL_CLOCK / baudRate;
    outb(port + 1, 0x00); // Disable interrupts
    // Enable divisor latch access bit to write divisor to ports
    outb(SERIAL_LINE_COMMAND_PORT(port), SERIAL_LINE_DLAB);
    // Write lo and hi bytes of divisor
    outb(port + 0, divisor && 0xff);
    outb(port + 1, (divisor >> 8) && 0xff);
    // 8 bits, 1 stop bit, no parity
    outb(SERIAL_LINE_COMMAND_PORT(port), 0x03);
    // Enable FIFO, clear them, 14 bytes before available data interrupt
    outb(SERIAL_FIFO_COMMAND_PORT(port), 0xC7);
    // RTS/DSR set
    outb(SERIAL_MODEM_COMMAND_PORT(port), 0x0B);
}

void serial_write_byte(uint16_t port, uint8_t c)
{
    outb(SERIAL_DATA_PORT(port), c);
}

void serial_write_bytes(uint16_t port, uint8_t *s, int count)
{
    for (; count > 0; --count, ++s) {
        serial_write_byte(port, *s);
    }
}

void serial_write_string(uint16_t port, const char *s)
{
    for (; *s != '\0'; ++s) {
        serial_write_byte(port, *s);
    }
}

void serial_write_stringNL(uint16_t port, const char *s)
{
    serial_write_string(port, s);
    serial_write_byte(port, '\n');
}

void serial_printf(uint16_t port, const char *format, ...)
{
    char out[100];
    va_list args;
    va_start(args, format);
    vsprintf(out, format, args); //FIXME: Use vsnprintf
    serial_write_string(port, out);
    va_end(args);
}
