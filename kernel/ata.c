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


void ata_reset(ATADrive *drive)
{
    outb(drive->ports.primaryControlRegister, 4);
    outb(drive->ports.primaryControlRegister, 0);
}

void ata_wait_until_data_available(ATADrive *drive)
{
    while ((inb(drive->ports.commandPort) & 8) == 0);
}

/*
    Source: http://wiki.osdev.org/ATA_read/write_sectors
*/
void ata_readLBA(ATADrive *drive, uint32_t lba, uint8_t sectors, uint16_t *dst)
{
    if ((inb(drive->ports.commandPort) & 0x88) == 0) {
        ata_reset(drive);
    }

    outb(drive->ports.headPort, 0xe0 | (((lba >> 24)) & 0x0f)); // set bit 6 for LBA mode
    outb(drive->ports.errorPort, 0);
    outb(drive->ports.sectorCountPort, sectors); // sector count
    outb(drive->ports.lowLBAPort, lba & 0xff);
    outb(drive->ports.midLBAPort, (lba >> 8) & 0xff);
    outb(drive->ports.highLBAPort, (lba >> 16) & 0xff);
    outb(drive->ports.commandPort, 0x20);

    ata_wait_until_data_available(drive);
    for (int i = 0; i < sectors; ++i) {
        for (int j = 0; j < 256; ++j) {
            ata_wait_until_data_available(drive); 
            dst[i * 256 + j] = inw(0x1f0);
        }
    }
}