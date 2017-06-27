#include "ata.h"

#include "io.h"

const uint16_t ata_drive_lba_port = 0x01f6;
const uint16_t ata_sector_count_port = 0x01f2;
const uint16_t ata_low_lba_port = 0x01f3;
const uint16_t ata_mid_lba_port = 0x01f4;
const uint16_t ata_high_lba_port = 0x01f5;
const uint16_t ata_cmd_port = 0x01f7;

/*
    Source: http://wiki.osdev.org/ATA_read/write_sectors
*/
void ata_read_lba(uint32_t lba, uint8_t sectors, uint16_t *dst)
{
    outb(ata_drive_lba_port, 0xe0); // set bit 6 for LBA mode
    outb(ata_sector_count_port, sectors); // sector count
    outb(ata_low_lba_port, lba & 0xff);
    outb(ata_mid_lba_port, (lba >> 8) & 0xff);
    outb(ata_high_lba_port, (lba >> 16) & 0xff);
    outb(ata_cmd_port, 0x20);

    while (inb(ata_cmd_port) == 8);
    for (int i = 0; i < sectors; ++i) {
        for (int j = 0; j < 256; ++j) {
            dst[i * 256 + j] = inw(0x1f0);
        }
    }
}