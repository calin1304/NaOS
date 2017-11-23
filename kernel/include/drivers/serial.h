#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SERIAL_COM1_BASE                    0x3f8
#define SERIAL_DATA_PORT(base)              (base)
#define SERIAL_FIFO_COMMAND_PORT(base)      (base+2)
#define SERIAL_LINE_COMMAND_PORT(base)      (base+3)
#define SERIAL_MODEM_COMMAND_PORT(base)     (base+4)
#define SERIAL_LINE_STATUS_PORT(base)       (base+5)

#define SERIAL_LINE_ENABLE_DLAB 0x80

#define SERIAL_CLOCK 115200

typedef struct SerialPort_ {
    int com;
} SerialPort;

void serialPortInit(SerialPort *r, int com, int baudRate);
void serialPortSetBaudRate(SerialPort *r, int rate);
void serialPortConfigureLine(SerialPort *r);
void serialPortWriteByte(SerialPort *r, uint8_t c);
void serialPortWriteBytes(SerialPort *r, uint8_t *s, int count);
void serialPortWriteString(SerialPort *r, const char *s);
void serialPortWriteStringNL(SerialPort *r, const char *s);
void serialPortPrintf(SerialPort *r, const char *format, ...);

#endif