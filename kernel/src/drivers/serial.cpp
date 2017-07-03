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