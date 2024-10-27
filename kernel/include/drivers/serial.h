#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SERIAL_COM1 0x3f8

void serial_init(uint16_t port, uint16_t baudRate);
void serial_write_byte(uint16_t port, uint8_t c);
void serial_write_bytes(uint16_t port, uint8_t *s, int count);
void serial_write_string(uint16_t port, const char *s);
void serial_write_string_nl(uint16_t port, const char *s);
void serial_printf(uint16_t port, const char *format, ...);

#endif
