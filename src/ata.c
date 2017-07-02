#include "ata.h"

#include "io.h"
#include "console.h"

const uint16_t ata_drive_data_port          = 0x1f0;
const uint16_t ata_drive_err_port           = 0x1f1;
const uint16_t ata_sector_count_port        = 0x1f2;
const uint16_t ata_low_lba_port             = 0x1f3;
const uint16_t ata_mid_lba_port             = 0x1f4;
const uint16_t ata_high_lba_port            = 0x1f5;
const uint16_t ata_drive_head_port          = 0x1f6;
const uint16_t ata_cmd_port                 = 0x1f7;
const uint16_t ata_primary_control_register = 0x3F6;


void ata_reset()
{
    outb(ata_primary_control_register, 4);
    outb(ata_primary_control_register, 0);
}

void ata_wait_until_data_available()
{
    while ((inb(ata_cmd_port) & 8) == 0);
}

/*
    Source: http://wiki.osdev.org/ATA_read/write_sectors
*/
void ata_read_lba(uint32_t lba, uint8_t sectors, uint16_t *dst)
{
    if ((inb(ata_cmd_port) & 0x88) == 0) {
        ata_reset();
    }

    outb(ata_drive_head_port, 0xe0 | (((lba >> 24)) & 0x0f)); // set bit 6 for LBA mode
    outb(ata_drive_err_port, 0);
    outb(ata_sector_count_port, sectors); // sector count
    outb(ata_low_lba_port, lba & 0xff);
    outb(ata_mid_lba_port, (lba >> 8) & 0xff);
    outb(ata_high_lba_port, (lba >> 16) & 0xff);
    outb(ata_cmd_port, 0x20);

    ata_wait_until_data_available(); 
    for (int i = 0; i < sectors; ++i) {
        for (int j = 0; j < 256; ++j) {
            ata_wait_until_data_available(); 
            dst[i * 256 + j] = inw(0x1f0);
        }
    }
}